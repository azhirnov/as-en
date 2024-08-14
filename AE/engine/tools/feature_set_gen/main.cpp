// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	popular desktop GPUs:
	https://store.steampowered.com/hwsurvey/videocard/

	popular mobile GPUs:
	TODO

	feature & properties json in:
	https://vulkan.gpuinfo.org/
*/

#include "FeatureSetUtils.h"
#include "base/Algorithms/StringUtils.h"

#include "graphics/Private/ImageDesc.cpp"
#include "graphics/Private/FeatureSet.cpp"
#include "graphics/Private/RenderState.cpp"
#include "graphics/Private/EnumUtils.cpp"
#include "graphics/Metal/MFeatureSet.cpp"

using namespace AE;
using namespace AE::Base;
using namespace AE::Graphics;


#define FS_INIT( _name_ )		min_fs._name_ = fs._name_;
#define FS_MERGE( _name_ )		min_fs._name_ = FS_MergeMin( min_fs._name_, fs._name_, AE_TOSTRING( _name_ ));
#define FS_ALL_TRUE( _name_ )	(fs._name_ == EFeature::RequireTrue) and
#define FS_ANY_TRUE( _name_ )	(fs._name_ == EFeature::RequireTrue) or
#define FS_ANY_TRUE2( _name_ )	(min_fs._name_ == EFeature::RequireTrue) or


enum class EType : uint
{
	Unknown		= 0,
	Desktop,
	Mobile,
};

struct FeatureSetInfo
{
	Path			path;
	FeatureSetExt	fs;
	String			name;
	EType			type	= Default;
};

/*
=================================================
	SaveToFile
=================================================
*
static bool  SaveToFile (ArrayView<FeatureSetInfo> fsInfo)
{
	FileSystem::CreateDirectories( OUTPUT_FOLDER );

	for (auto& info : fsInfo)
	{
		const auto&		src_path	= info.path;
		const auto&		fs			= info.fs;

		Path	dst_path = OUTPUT_FOLDER;
		dst_path /= src_path.filename();
		dst_path.set_extension( ".as" );

		CHECK_ERR( FeatureSetToScript( dst_path, "", fs ));
	}
	return true;
}

/*
=================================================
	ValidateFS
=================================================
*/
template <typename T>
ND_ static T  FS_Validate (T feat)
{
	if constexpr( IsSameTypes< T, EFeature >)
		return feat == EFeature::RequireFalse ? EFeature::Ignore : feat;
	else
		return {};
}

static void  ValidateFS (INOUT FeatureSet &fs)
{
	using PerDescriptorSet		= Graphics::FeatureSet::PerDescriptorSet;
	using PerShaderStage		= Graphics::FeatureSet::PerShaderStage;
	using SubgroupOperationBits	= Graphics::FeatureSet::SubgroupOperationBits;
	using VendorIDs_t			= Graphics::FeatureSet::VendorIDs_t;
	using GraphicsDevices_t		= Graphics::FeatureSet::GraphicsDevices_t;
	using PixelFormatSet_t		= Graphics::FeatureSet::PixelFormatSet_t;
	using SurfaceFormatSet_t	= Graphics::FeatureSet::SurfaceFormatSet_t;
	using VertexFormatSet_t		= Graphics::FeatureSet::VertexFormatSet_t;
	using SampleCountBits		= Graphics::FeatureSet::SampleCountBits;
	using ShaderVersion			= Graphics::FeatureSet::ShaderVersion;
	using Queues				= Graphics::FeatureSet::Queues;
	using ShadingRateSet_t		= Graphics::FeatureSet::ShadingRateSet_t;
	using VRSTexelSize			= Graphics::FeatureSet::VRSTexelSize;

	#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )		if constexpr( IsSameTypes< _type_, EFeature >)  fs._name_ = FS_Validate<_type_>( fs._name_ );
	AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
	#undef AE_FEATURE_SET_VISIT
}

static void  ValidateAppleFS (INOUT FeatureSet &fs)
{
	if ( fs.maxShaderVersion.metal >= 220 and fs.subgroup == EFeature::RequireTrue )
	{
		fs.subgroupOperations = FeatureSet::SubgroupOperationBits{
			// Basic
			ESubgroupOperation::IndexAndSize, ESubgroupOperation::Elect, ESubgroupOperation::Barrier,
			// Vote
			ESubgroupOperation::Any, ESubgroupOperation::All,
			// Arithmetic
			ESubgroupOperation::Add, ESubgroupOperation::Mul, ESubgroupOperation::Min, ESubgroupOperation::Max,
			ESubgroupOperation::Or, ESubgroupOperation::Xor, ESubgroupOperation::InclusiveMul, ESubgroupOperation::InclusiveAdd,
			ESubgroupOperation::ExclusiveAdd, ESubgroupOperation::ExclusiveMul,
			//ESubgroupOperation::And	- macOS only
			// Ballot
			ESubgroupOperation::Ballot, ESubgroupOperation::BroadcastFirst, ESubgroupOperation::Broadcast,
			// Shuffle
			ESubgroupOperation::Shuffle, ESubgroupOperation::ShuffleXor,
			// Shuffle Relative
			ESubgroupOperation::ShuffleUp, ESubgroupOperation::ShuffleDown,
			// Quad
			ESubgroupOperation::QuadBroadcast
		};

		if ( fs.shaderInt64 != EFeature::RequireTrue )
			fs.subgroupTypes &= ~ESubgroupTypes::Int64;

		fs.geometryShader		= EFeature::Ignore;
		fs.tessellationShader	= EFeature::Ignore;
	}
}

/*
=================================================
	GenMinimalFS
=================================================
*/
static bool  GenMinimalFS (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs = fsInfo.front().fs;

	// Mali T8xx supports 128 with half register count and 64 with full size registers
	AssignMin( min_fs.maxComputeWorkGroupInvocations,	64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeX,			64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeY,			64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeZ,			64u );

	for (auto& info : fsInfo)
	{
		min_fs.MergeMin( info.fs );
	}

	CHECK( min_fs.maxShaderVersion.metal >= 200 );
	ValidateFS( INOUT min_fs );
	ValidateAppleFS( INOUT min_fs );

	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	for (auto& info : fsInfo)
	{
		CHECK( info.fs >= min_fs );
	}

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "minimal.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinimalFS", min_fs ));
	return true;
}

/*
=================================================
	GenMinDescriptorIndexing
=================================================
*/
static bool  GenMinDescriptorIndexing (ArrayView<FeatureSetInfo> fsInfo)
{
	#define FS_LIST( _visitor_ ) \
		_visitor_( shaderSampledImageArrayDynamicIndexing		)\
		_visitor_( shaderStorageBufferArrayDynamicIndexing		)\
		_visitor_( shaderStorageImageArrayDynamicIndexing		)\
		_visitor_( shaderUniformBufferArrayDynamicIndexing		)\
		_visitor_( shaderInputAttachmentArrayDynamicIndexing	)\
		_visitor_( shaderUniformTexelBufferArrayDynamicIndexing	)\
		_visitor_( shaderStorageTexelBufferArrayDynamicIndexing	)

	#define FS_LIST2( _visitor_ ) \
		_visitor_( runtimeDescriptorArray				)\
		_visitor_( perDescrSet							)\
		_visitor_( perStage								)\
		_visitor_( maxUniformBufferSize					)\
		_visitor_( maxStorageBufferSize					)\
		_visitor_( maxDescriptorSets					)\
		_visitor_( maxPushConstantsSize					)\
		_visitor_( maxFragmentOutputAttachments			)\
		_visitor_( maxFragmentCombinedOutputResources	)

	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( not (FS_LIST( FS_ANY_TRUE ) false) )
			continue;

		if ( init )
		{
			FS_LIST( FS_MERGE );
			FS_LIST2( FS_MERGE );
			CHECK( FS_LIST( FS_ANY_TRUE2 ) false );
		}
		else
		{
			FS_LIST( FS_INIT );
			FS_LIST2( FS_INIT );
			init = true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	comment << "\n";

	ValidateFS( INOUT min_fs );
	//min_fs.Validate();
	//CHECK( min_fs.IsValid() );

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "parts/min_desc_indexing.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "part.MinDescriptorIndexing", min_fs, comment ));
	return true;

#undef FS_LIST
#undef FS_LIST2
}

/*
=================================================
	GenMinNonUniformDescIndexing
=================================================
*/
static bool  GenMinNonUniformDescIndexing (ArrayView<FeatureSetInfo> fsInfo)
{
	#define FS_LIST( _visitor_ ) \
		_visitor_( shaderUniformBufferArrayNonUniformIndexing	)\
		_visitor_( shaderSampledImageArrayNonUniformIndexing	)\
		_visitor_( shaderStorageBufferArrayNonUniformIndexing	)\
		_visitor_( shaderStorageImageArrayNonUniformIndexing	)\
		_visitor_( shaderInputAttachmentArrayNonUniformIndexing	)\

	#define FS_LIST2( _visitor_ ) \
		_visitor_( runtimeDescriptorArray								)\
		_visitor_( shaderUniformTexelBufferArrayNonUniformIndexing		)\
		_visitor_( shaderStorageTexelBufferArrayNonUniformIndexing		)\
		_visitor_( shaderUniformBufferArrayNonUniformIndexingNative		)\
		_visitor_( shaderSampledImageArrayNonUniformIndexingNative		)\
		_visitor_( shaderStorageBufferArrayNonUniformIndexingNative		)\
		_visitor_( shaderStorageImageArrayNonUniformIndexingNative		)\
		_visitor_( shaderInputAttachmentArrayNonUniformIndexingNative	)\
		_visitor_( shaderSampledImageArrayDynamicIndexing				)\
		_visitor_( shaderStorageBufferArrayDynamicIndexing				)\
		_visitor_( shaderStorageImageArrayDynamicIndexing				)\
		_visitor_( shaderUniformBufferArrayDynamicIndexing				)\
		_visitor_( shaderInputAttachmentArrayDynamicIndexing			)\
		_visitor_( shaderUniformTexelBufferArrayDynamicIndexing			)\
		_visitor_( shaderStorageTexelBufferArrayDynamicIndexing			)\
		_visitor_( perDescrSet											)\
		_visitor_( perStage												)\
		_visitor_( maxUniformBufferSize									)\
		_visitor_( maxStorageBufferSize									)\
		_visitor_( maxDescriptorSets									)\
		_visitor_( maxPushConstantsSize									)\
		_visitor_( maxFragmentOutputAttachments							)\
		_visitor_( maxFragmentCombinedOutputResources					)

	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( not (FS_LIST( FS_ANY_TRUE ) false) )
			continue;

		if ( fs.shaderSampledImageArrayNonUniformIndexing != EFeature::RequireTrue )
			continue;

		if ( init )
		{
			FS_LIST( FS_MERGE );
			FS_LIST2( FS_MERGE );
			CHECK( FS_LIST( FS_ANY_TRUE2 ) false );
		}
		else
		{
			FS_LIST( FS_INIT );
			FS_LIST2( FS_INIT );
			init = true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	comment << "\n";

	ValidateFS( INOUT min_fs );
	//min_fs.Validate();
	//CHECK( min_fs.IsValid() );

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "parts/min_nonuniform_desc_idx.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "part.MinNonUniformDescriptorIndexing", min_fs, comment ));
	return true;

#undef FS_LIST
#undef FS_LIST2
}

/*
=================================================
	GenMinNativeNonUniformDescIndexing
=================================================
*/
static bool  GenMinNativeNonUniformDescIndexing (ArrayView<FeatureSetInfo> fsInfo)
{
	#define FS_LIST( _visitor_ ) \
		_visitor_( shaderUniformBufferArrayNonUniformIndexingNative		)\
		_visitor_( shaderSampledImageArrayNonUniformIndexingNative		)\
		_visitor_( shaderStorageBufferArrayNonUniformIndexingNative		)\
		_visitor_( shaderStorageImageArrayNonUniformIndexingNative		)\
		_visitor_( shaderInputAttachmentArrayNonUniformIndexingNative	)\

	#define FS_LIST2( _visitor_ ) \
		_visitor_( runtimeDescriptorArray								)\
		_visitor_( shaderUniformTexelBufferArrayNonUniformIndexing		)\
		_visitor_( shaderStorageTexelBufferArrayNonUniformIndexing		)\
		_visitor_( shaderUniformBufferArrayNonUniformIndexing			)\
		_visitor_( shaderSampledImageArrayNonUniformIndexing			)\
		_visitor_( shaderStorageBufferArrayNonUniformIndexing			)\
		_visitor_( shaderStorageImageArrayNonUniformIndexing			)\
		_visitor_( shaderInputAttachmentArrayNonUniformIndexing			)\
		_visitor_( shaderSampledImageArrayDynamicIndexing				)\
		_visitor_( shaderStorageBufferArrayDynamicIndexing				)\
		_visitor_( shaderStorageImageArrayDynamicIndexing				)\
		_visitor_( shaderUniformBufferArrayDynamicIndexing				)\
		_visitor_( shaderInputAttachmentArrayDynamicIndexing			)\
		_visitor_( shaderUniformTexelBufferArrayDynamicIndexing			)\
		_visitor_( shaderStorageTexelBufferArrayDynamicIndexing			)\
		_visitor_( perDescrSet											)\
		_visitor_( perStage												)\
		_visitor_( maxUniformBufferSize									)\
		_visitor_( maxStorageBufferSize									)\
		_visitor_( maxDescriptorSets									)\
		_visitor_( maxPushConstantsSize									)\
		_visitor_( maxFragmentOutputAttachments							)\
		_visitor_( maxFragmentCombinedOutputResources					)

	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( not (FS_LIST( FS_ANY_TRUE ) false) )
			continue;

		if ( fs.shaderSampledImageArrayNonUniformIndexingNative != EFeature::RequireTrue )
			continue;

		if ( init )
		{
			FS_LIST( FS_MERGE );
			FS_LIST2( FS_MERGE );
			CHECK( FS_LIST( FS_ANY_TRUE2 ) false );
		}
		else
		{
			FS_LIST( FS_INIT );
			FS_LIST2( FS_INIT );
			init = true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	comment << "\n";

	ValidateFS( INOUT min_fs );
	//min_fs.Validate();
	//CHECK( min_fs.IsValid() );

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "parts/min_native_nonuniform_desc_idx.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "part.MinNativeNonUniformDescriptorIndexing", min_fs, comment ));
	return true;

#undef FS_LIST
#undef FS_LIST2
}

/*
=================================================
	GenMinRecursiveRayTracing
=================================================
*/
static bool  GenMinRecursiveRayTracing (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( fs.maxRayRecursionDepth <= 1 )
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = ushort(Max( min_fs.maxShaderVersion.metal, 230u ));

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_recursive_rt.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinRecursiveRayTracing", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinInlineRayTracing
=================================================
*/
static bool  GenMinInlineRayTracing (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( fs.rayQuery != EFeature::RequireTrue )
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = ushort(Max( min_fs.maxShaderVersion.metal, 230u ));

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_inline_rt.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinInlineRayTracing", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinMeshShader
=================================================
*/
static bool  GenMinMeshShader (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( fs.taskShader != EFeature::RequireTrue or
			 fs.meshShader != EFeature::RequireTrue )
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.rayQuery					= EFeature::RequireFalse;
	min_fs.rayTracingPipeline		= EFeature::RequireFalse;
	min_fs.maxShaderVersion.metal	= ushort(Max( min_fs.maxShaderVersion.metal, 300u ));

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_mesh_shader.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinMeshShader", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinMobile
=================================================
*/
static bool  GenMinMobile (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( info.type != EType::Mobile )
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	// Mali T8xx supports 128 with half register count and 64 with full size registers
	AssignMin( min_fs.maxComputeWorkGroupInvocations,	64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeX,			64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeY,			64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeZ,			64u );

	min_fs.maxShaderVersion.metal = ushort(Max( min_fs.maxShaderVersion.metal, 220u ));	// iOS 13

	ValidateFS( INOUT min_fs );
	ValidateAppleFS( INOUT min_fs );

	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_mobile.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinMobile", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinMobileMali
=================================================
*/
static bool  GenMinMobileMali (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( info.type != EType::Mobile or not fs.vendorIds.include.contains( EGPUVendor::ARM ))
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	// Mali T8xx supports 128 with half register count and 64 with full size registers
	AssignMin( min_fs.maxComputeWorkGroupInvocations,	64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeX,			64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeY,			64u );
	AssignMin( min_fs.maxComputeWorkGroupSizeZ,			64u );

	min_fs.maxShaderVersion.metal = 0;
	//min_fs.hwCompressedAttachmentFormats.insert( EPixelFormat::RGBA8_UNorm );

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_mobile_mali.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinMobileMali", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinMobileAdreno
=================================================
*/
static bool  GenMinMobileAdreno (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( info.type != EType::Mobile or not fs.vendorIds.include.contains( EGPUVendor::Qualcomm ))
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = 0;

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_mobile_adreno.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinMobileAdreno", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinMobilePowerVR
=================================================
*/
static bool  GenMinMobilePowerVR (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( info.type != EType::Mobile or not fs.vendorIds.include.contains( EGPUVendor::ImgTech ))
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = 0;

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_mobile_pvr.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinMobilePowerVR", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinDesktop
=================================================
*/
static bool  GenMinDesktop (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( info.type != EType::Desktop )
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = ushort(Max( min_fs.maxShaderVersion.metal, 220u ));	// MacOS 10.15

	ValidateFS( INOUT min_fs );
	ValidateAppleFS( INOUT min_fs );

	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_desktop.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinDesktop", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinDesktopAMD
=================================================
*/
static bool  GenMinDesktopAMD (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( info.type != EType::Desktop or not fs.vendorIds.include.contains( EGPUVendor::AMD ))
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = 0;

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_desktop_amd.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinDesktopAMD", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinDesktopNV
=================================================
*/
static bool  GenMinDesktopNV (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( info.type != EType::Desktop or not fs.vendorIds.include.contains( EGPUVendor::NVidia ))
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = 0;

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_desktop_nv.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinDesktopNV", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinDesktopIntel
=================================================
*/
static bool  GenMinDesktopIntel (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( not fs.vendorIds.include.contains( EGPUVendor::Intel ))
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal = 0;

	ValidateFS( INOUT min_fs );
	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_desktop_intel.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinDesktopIntel", min_fs, comment ));
	return true;
}

/*
=================================================
	GenMinApple
----
	vulkan features only, additionally use 'apple_metal' features
=================================================
*/
static bool  GenMinApple (ArrayView<FeatureSetInfo> fsInfo)
{
	FeatureSet	min_fs;
	String		comment;
	bool		init	= false;

	comment << "\t// include:\n";

	for (auto& info : fsInfo)
	{
		const auto&	fs = info.fs;

		if ( not fs.vendorIds.include.contains( EGPUVendor::Apple ))
			continue;

		if ( init )
		{
			min_fs.MergeMin( fs );
		}
		else
		{
			min_fs	= fs;
			init	= true;
		}

		comment << "\t//\t" << info.name << "\n";
	}

	CHECK_ERR( init );

	min_fs.maxShaderVersion.metal	= ushort(Max( min_fs.maxShaderVersion.metal, 220u ));	// macOS 10.15, iOS 13
	min_fs.tessellationShader		= EFeature::Ignore;

	ValidateFS( INOUT min_fs );
	ValidateAppleFS( INOUT min_fs );

	min_fs.Validate();
	CHECK( min_fs.IsValid() );

	comment << "\n";

	Path	dst_path = FEATURE_SET_FOLDER;
	dst_path.append( "min_apple.as" );

	CHECK_ERR( FeatureSetToScript( dst_path, "MinApple", min_fs, comment ));
	return true;
}

/*
=================================================
	GenAppleFamily
=================================================
*/
static bool  GenAppleFamily (INOUT Array<FeatureSetInfo> &fsInfo)
{
	const auto	FixAndValidate = [] (INOUT FeatureSet &fs)
	{{
		const auto	True	= FeatureSet::EFeature::RequireTrue;

		fs.shaderSampleRateInterpolationFunctions	= True;
		fs.vertexDivisor							= True;
		fs.maxVertexAttribDivisor					= 1u << 30;
		fs.textureCompressionASTC_LDR				= True;
		fs.textureCompressionETC2					= True;
		fs.framebufferColorSampleCounts				= FeatureSet::SampleCountBits( 1 | 2 | 4 );
		fs.framebufferDepthSampleCounts				= FeatureSet::SampleCountBits( 1 | 2 | 4 );

		fs.storageTexBufferAtomicFormats.insert( EPixelFormat::R32I );
		fs.storageTexBufferAtomicFormats.insert( EPixelFormat::R32U );

		fs.vendorIds.include.insert( EGPUVendor::Apple );

		ValidateFS( INOUT fs );

		CHECK( fs.IsValid() );
		fs.Validate();
	}};

	const auto	InitFeatureSet = [&FixAndValidate] (const MFeatureSet &mfs, const MGPUFamilies &f, OUT FeatureSet &fs)
	{{
		fs.vendorIds.include.insert( EGPUVendor::Apple );
		mfs.InitFeatureSet( f, OUT fs );
		FixAndValidate( INOUT fs );
	}};


	// Apple9
	{
		MGPUFamilies	f;
		f.apple	= 9;
		f.metal	= 3;

		MFeatureSet		mfs;
		MFeatureSet::InitFromFSTable( f, Version2{3,0}, OUT mfs.features, OUT mfs.properties );

		auto&	fs = fsInfo.emplace_back();
		fs.name = "Apple9";
		fs.type	= EType::Unknown;	// both
		fs.fs.maxShaderVersion.metal = 300;
		fs.fs.maxShaderVersion.spirv = 140;

		InitFeatureSet( mfs, f, OUT fs.fs );

		Path	dst_path = FEATURE_SET_FOLDER;
		dst_path.append( "apple_metal/apple9.as" );
		CHECK_ERR( FeatureSetToScript( dst_path, "Apple9", fs.fs, "" ));
	}

	// Apple8
	{
		// Apple8 (A15, A16, M2)
		{
			MGPUFamilies	f;
			f.apple	= 8;
			f.metal	= 3;

			MFeatureSet		mfs;
			MFeatureSet::InitFromFSTable( f, Version2{3,0}, OUT mfs.features, OUT mfs.properties );

			auto&	fs = fsInfo.emplace_back();
			fs.name = "Apple8";
			fs.type	= EType::Mobile;
			fs.fs.maxShaderVersion.metal = 300;
			fs.fs.maxShaderVersion.spirv = 140;

			InitFeatureSet( mfs, f, OUT fs.fs );

			Path	dst_path = FEATURE_SET_FOLDER;
			dst_path.append( "apple_metal/apple8.as" );
			CHECK_ERR( FeatureSetToScript( dst_path, "Apple8", fs.fs, "" ));
		}

		// Apple8 Mac (M2)
		{
			MGPUFamilies	f;
			f.apple	= 8;
			f.mac	= 2;
			f.metal	= 3;

			MFeatureSet		mfs;
			MFeatureSet::InitFromFSTable( f, Version2{3,0}, OUT mfs.features, OUT mfs.properties );

			auto&	fs = fsInfo.emplace_back();
			fs.name = "Apple8_Mac";
			fs.type	= EType::Desktop;
			fs.fs.maxShaderVersion.metal = 300;
			fs.fs.maxShaderVersion.spirv = 140;

			InitFeatureSet( mfs, f, OUT fs.fs );

			Path	dst_path = FEATURE_SET_FOLDER;
			dst_path.append( "apple_metal/apple8_mac.as" );
			CHECK_ERR( FeatureSetToScript( dst_path, "Apple8_Mac", fs.fs, "" ));
		}
	}

	// Apple7
	{
		// Apple7 (A14, M1)
		{
			MGPUFamilies	f;
			f.apple	= 7;
			f.metal	= 3;

			MFeatureSet		mfs;
			MFeatureSet::InitFromFSTable( f, Version2{3,0}, OUT mfs.features, OUT mfs.properties );

			auto&	fs = fsInfo.emplace_back();
			fs.name = "Apple7_Metal3";
			fs.type	= EType::Mobile;
			fs.fs.maxShaderVersion.metal = 300;
			fs.fs.maxShaderVersion.spirv = 140;

			InitFeatureSet( mfs, f, OUT fs.fs );

			Path	dst_path = FEATURE_SET_FOLDER;
			dst_path.append( "apple_metal/apple7_metal3.as" );
			CHECK_ERR( FeatureSetToScript( dst_path, "Apple7_Metal3", fs.fs, "" ));
		}

		// Apple7 Mac (M1)
		{
			MGPUFamilies	f;
			f.apple	= 7;
			f.mac	= 2;
			f.metal	= 3;

			MFeatureSet		mfs;
			MFeatureSet::InitFromFSTable( f, Version2{3,0}, OUT mfs.features, OUT mfs.properties );

			auto&	fs = fsInfo.emplace_back();
			fs.name = "Apple7_Mac_Metal3";
			fs.type	= EType::Desktop;
			fs.fs.maxShaderVersion.metal = 300;
			fs.fs.maxShaderVersion.spirv = 140;

			InitFeatureSet( mfs, f, OUT fs.fs );

			Path	dst_path = FEATURE_SET_FOLDER;
			dst_path.append( "apple_metal/apple7_mac_metal3.as" );
			CHECK_ERR( FeatureSetToScript( dst_path, "Apple7_Mac_Metal3", fs.fs, "" ));
		}

		// Apple7 (A14, M1), metal2.4
		{
			MGPUFamilies	f;
			f.apple	= 7;
			f.metal	= 2;

			MFeatureSet		mfs;
			MFeatureSet::InitFromFSTable( f, Version2{2,4}, OUT mfs.features, OUT mfs.properties );

			auto&	fs = fsInfo.emplace_back();
			fs.name = "Apple7";
			fs.type	= EType::Mobile;
			fs.fs.maxShaderVersion.metal = 240;
			fs.fs.maxShaderVersion.spirv = 140;

			InitFeatureSet( mfs, f, OUT fs.fs );

			Path	dst_path = FEATURE_SET_FOLDER;
			dst_path.append( "apple_metal/apple7.as" );
			CHECK_ERR( FeatureSetToScript( dst_path, "Apple7", fs.fs, "" ));
		}
	}

	// Apple6 (A13)
	{
		// Apple6 (A13)  metal3
		{
			MGPUFamilies	f;
			f.apple	= 6;
			f.metal	= 3;

			MFeatureSet		mfs;
			MFeatureSet::InitFromFSTable( f, Version2{3,0}, OUT mfs.features, OUT mfs.properties );

			auto&	fs = fsInfo.emplace_back();
			fs.name = "Apple6_Metal3";
			fs.type	= EType::Mobile;
			fs.fs.maxShaderVersion.metal = 300;
			fs.fs.maxShaderVersion.spirv = 140;

			InitFeatureSet( mfs, f, OUT fs.fs );

			Path	dst_path = FEATURE_SET_FOLDER;
			dst_path.append( "apple_metal/apple6_metal3.as" );
			CHECK_ERR( FeatureSetToScript( dst_path, "Apple6_Metal3", fs.fs, "" ));
		}

		// Apple6 (A13) metal2
		{
			MGPUFamilies	f;
			f.apple	= 6;
			f.metal	= 2;

			MFeatureSet		mfs;
			MFeatureSet::InitFromFSTable( f, Version2{2,4}, OUT mfs.features, OUT mfs.properties );

			auto&	fs = fsInfo.emplace_back();
			fs.name = "Apple6";
			fs.type	= EType::Mobile;
			fs.fs.maxShaderVersion.metal = 240;
			fs.fs.maxShaderVersion.spirv = 140;

			InitFeatureSet( mfs, f, OUT fs.fs );

			Path	dst_path = FEATURE_SET_FOLDER;
			dst_path.append( "apple_metal/apple6.as" );
			CHECK_ERR( FeatureSetToScript( dst_path, "Apple6", fs.fs, "" ));
		}
	}

	// Apple5 (A12)
	/*{
		MGPUFamilies	f;
		f.apple	= 5;
		f.metal	= 2;

		MFeatureSet		mfs;
		MFeatureSet::InitFromFSTable( f, Version2{2,4}, OUT mfs.features, OUT mfs.properties );

		auto&	fs = fsInfo.emplace_back();
		fs.name = "Apple5";
		fs.type	= EType::Mobile;
		fs.fs.maxShaderVersion.metal = 240;
		fs.fs.maxShaderVersion.spirv = 100;

		InitFeatureSet( mfs, f, OUT fs.fs );

		Path	dst_path = FEATURE_SET_FOLDER;
		dst_path.append( "apple_metal/apple5.as" );
		CHECK_ERR( FeatureSetToScript( dst_path, "Apple5", fs.fs, "" ));
	}

	// Apple4 (A11)
	{
		MGPUFamilies	f;
		f.apple	= 5;
		f.metal	= 2;

		MFeatureSet		mfs;
		MFeatureSet::InitFromFSTable( f, Version2{2,4}, OUT mfs.features, OUT mfs.properties );

		auto&	fs = fsInfo.emplace_back();
		fs.name = "Apple4";
		fs.type	= EType::Mobile;
		fs.fs.maxShaderVersion.metal = 240;
		fs.fs.maxShaderVersion.spirv = 100;

		InitFeatureSet( mfs, f, OUT fs.fs );

		Path	dst_path = FEATURE_SET_FOLDER;
		dst_path.append( "apple_metal/apple4.as" );
		CHECK_ERR( FeatureSetToScript( dst_path, "Apple4", fs.fs, "" ));
	}*/

	// Mac2
	{
		MGPUFamilies	f;
		f.mac	= 2;
		f.metal	= 2;

		MFeatureSet		mfs;
		MFeatureSet::InitFromFSTable( f, Version2{2,4}, OUT mfs.features, OUT mfs.properties );

		auto&	fs = fsInfo.emplace_back();
		fs.name = "Apple_Mac2";
		fs.type	= EType::Desktop;

		{
			FeatureSet	min_fs;
			min_fs.maxShaderVersion.metal = 240;
			min_fs.maxShaderVersion.spirv = 140;
			min_fs.vendorIds.include.insert( EGPUVendor::AMD );
			min_fs.devicesIds.include.insert( EGraphicsDeviceID::AMD_RDNA2 );

			mfs.InitFeatureSet( f, OUT min_fs );
			fs.fs = min_fs;
		}{
			FeatureSet	min_fs;
			min_fs.maxShaderVersion.metal = 240;
			min_fs.maxShaderVersion.spirv = 140;
			min_fs.vendorIds.include.insert( EGPUVendor::Intel );

			mfs.InitFeatureSet( f, OUT min_fs );
			fs.fs.MergeMin( min_fs );
		}

		FixAndValidate( INOUT fs.fs );

		Path	dst_path = FEATURE_SET_FOLDER;
		dst_path.append( "apple_metal/mac2.as" );
		CHECK_ERR( FeatureSetToScript( dst_path, "Apple_Mac2", fs.fs, "" ));
	}

	// Mac Metal 3
	{
		MGPUFamilies	f;
		f.mac	= 2;
		f.metal	= 3;

		MFeatureSet		mfs;
		MFeatureSet::InitFromFSTable( f, Version2{3,0}, OUT mfs.features, OUT mfs.properties );

		auto&	fs = fsInfo.emplace_back();
		fs.name = "Apple_Mac_Metal3";
		fs.type	= EType::Desktop;

		{
			FeatureSet	min_fs;
			min_fs.maxShaderVersion.metal = 300;
			min_fs.maxShaderVersion.spirv = 140;
			min_fs.vendorIds.include.insert( EGPUVendor::AMD );
			min_fs.devicesIds.include.insert( EGraphicsDeviceID::AMD_RDNA2 );

			mfs.InitFeatureSet( f, OUT min_fs );
			fs.fs = min_fs;
		}{
			FeatureSet	min_fs;
			min_fs.maxShaderVersion.metal = 300;
			min_fs.maxShaderVersion.spirv = 140;
			min_fs.vendorIds.include.insert( EGPUVendor::Intel );

			mfs.InitFeatureSet( f, OUT min_fs );
			fs.fs.MergeMin( min_fs );
		}{
			FeatureSet	min_fs;
			min_fs.maxShaderVersion.metal = 300;
			min_fs.maxShaderVersion.spirv = 140;
			min_fs.vendorIds.include.insert( EGPUVendor::Apple );

			mfs.InitFeatureSet( f, OUT min_fs );
			fs.fs.MergeMin( min_fs );
		}

		FixAndValidate( INOUT fs.fs );

		Path	dst_path = FEATURE_SET_FOLDER;
		dst_path.append( "apple_metal/mac_metal3.as" );
		CHECK_ERR( FeatureSetToScript( dst_path, "Apple_Mac_Metal3", fs.fs, "" ));
	}

	const auto	GreaterThan = [&fsInfo] (StringView lhs, StringView rhs) -> bool
	{{
		const FeatureSet*	lhs_p = null;

		for (auto& item : Reverse(fsInfo))
		{
			if ( item.name == lhs ) {
				lhs_p = &item.fs;
				break;
			}
		}
		CHECK_ERR( lhs_p != null );

		for (auto& item : Reverse(fsInfo))
		{
			if ( item.name == rhs )
				return *lhs_p >= item.fs;
		}
		return false;
	}};

	CHECK_ERR( GreaterThan( "Apple_Mac_Metal3",		"Apple_Mac2"	));
	CHECK_ERR( GreaterThan( "Apple8_Mac",			"Apple8"		));
	CHECK_ERR( GreaterThan( "Apple8",				"Apple7"		));
	CHECK_ERR( GreaterThan( "Apple7",				"Apple6"		));
	CHECK_ERR( GreaterThan( "Apple7_Metal3",		"Apple7"		));
	CHECK_ERR( GreaterThan( "Apple7_Mac_Metal3",	"Apple7"		));
	CHECK_ERR( GreaterThan( "Apple7_Mac_Metal3",	"Apple7_Metal3"	));
	CHECK_ERR( GreaterThan( "Apple6_Metal3",		"Apple6"		));

	return true;
}

/*
=================================================
	main
=================================================
*/
int main ()
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	Array<FeatureSetInfo>	fs_infos;
	{
		CHECK_ERR( FileSystem::IsDirectory( DEVICE_INFO_FOLDER ));

		RingBuffer<Path>	stack;
		stack.push_back( DEVICE_INFO_FOLDER );

		while ( not stack.empty() )
		{
			Path	path = stack.front();
			stack.pop_front();

			for (auto fname : FileSystem::Enum( path ))
			{
				if ( fname.IsDirectory() )
				{
					if ( fname.Get().filename() != "except" )
						stack.push_back( fname );
				}
				else
				{
					auto&	info = fs_infos.emplace_back();
					info.path = FileSystem::ToAbsolute( fname );

					Path	p = fname.Get();
					p = p.parent_path();
					p = p.stem();

					if ( p == "mobile" )	info.type = EType::Mobile;	else
					if ( p == "desktop" )	info.type = EType::Desktop;
				}
			}
		}
	}
	CHECK( not fs_infos.empty() );

	for (auto& info : fs_infos)
	{
		AE_LOGI( "Process: "s << ToString( info.path ));
		CHECK_ERR( FeatureSetFromJSON( info.path, OUT info.fs, OUT info.name ), -1 );

		info.fs.Validate();

		// make compatible
		{
			uint	msl_ver = 200;
			if ( info.fs.shaderInt64 == EFeature::RequireTrue )				msl_ver = 230;
			if ( info.fs.accelerationStructure() == EFeature::RequireTrue )	msl_ver = 240;
			if ( info.fs.meshShader == EFeature::RequireTrue )				msl_ver = 300;

			info.fs.maxShaderVersion.metal	= ushort(Max( info.fs.maxShaderVersion.metal, msl_ver ));
		}

		// validate device limits
		{
			const auto	limit = DeviceLimits;

			CHECK_GE( Bytes{limit.res.minUniformBufferOffsetAlign},			info.fs.ext.minUniformBufferOffsetAlignment );
			CHECK_GE( Bytes{limit.res.minStorageBufferOffsetAlign},			info.fs.ext.minStorageBufferOffsetAlignment );
		//	CHECK_GE( Bytes{limit.res.minThreadgroupMemoryLengthAlign},		info.fs.ext. );
		//	CHECK_GE( Bytes{limit.res.minVertexBufferOffsetAlign},			info.fs.ext. );
			CHECK_GE( Bytes{limit.res.minUniformTexelBufferOffsetAlign},	info.fs.ext.minTexelBufferOffsetAlignment );
			CHECK_GE( Bytes{limit.res.minStorageTexelBufferOffsetAlign},	info.fs.ext.minTexelBufferOffsetAlignment );
			CHECK_LE( limit.res.maxUniformBufferRange,						info.fs.ext.maxUniformBufferRange );
			CHECK_LE( limit.res.maxBoundDescriptorSets,						info.fs.ext.maxBoundDescriptorSets );
			CHECK_GE( Bytes{limit.res.minMemoryMapAlign},					info.fs.ext.minMemoryMapAlignment );
			CHECK_GE( Bytes{limit.res.minNonCoherentAtomSize},				info.fs.ext.nonCoherentAtomSize );
			CHECK_GE( Bytes{limit.res.minBufferCopyOffsetAlign},			info.fs.ext.optimalBufferCopyOffsetAlignment );
			CHECK_GE( Bytes{limit.res.minBufferCopyRowPitchAlign},			info.fs.ext.optimalBufferCopyRowPitchAlignment );

			if ( info.fs.accelerationStructure() == EFeature::RequireTrue )
			{
				CHECK_GE( Bytes{limit.rayTracing.scratchBufferAlign},	info.fs.ext.minAccelerationStructureScratchOffsetAlignment );
				CHECK_LE( limit.rayTracing.maxGeometries,				info.fs.ext.maxGeometryCount );
				CHECK_LE( limit.rayTracing.maxInstances,				info.fs.ext.maxInstanceCount );
				CHECK_LE( limit.rayTracing.maxPrimitives,				info.fs.ext.maxPrimitiveCount );
				CHECK_LE( limit.rayTracing.maxRecursion,				info.fs.ext.maxRayRecursionDepth );
			}
		}

		// not supported
		info.fs.shaderSubgroupUniformControlFlow = EFeature::Ignore;

		CHECK( info.fs.IsValid() );
	}

	CHECK_ERR( GenAppleFamily( INOUT fs_infos ), -9 );

	CHECK_ERR( GenMinimalFS					( fs_infos ),	-10 );
	CHECK_ERR( GenMinDescriptorIndexing		( fs_infos ),	-10 );
	CHECK_ERR( GenMinNonUniformDescIndexing	( fs_infos ),	-10 );
	CHECK_ERR( GenMinNativeNonUniformDescIndexing( fs_infos ),	-10 );
	CHECK_ERR( GenMinRecursiveRayTracing	( fs_infos ),	-10 );
	CHECK_ERR( GenMinInlineRayTracing		( fs_infos ),	-10 );
	CHECK_ERR( GenMinMeshShader				( fs_infos ),	-10 );
	CHECK_ERR( GenMinDesktop				( fs_infos ),	-10 );
	CHECK_ERR( GenMinDesktopAMD				( fs_infos ),	-10 );
	CHECK_ERR( GenMinDesktopNV				( fs_infos ),	-10 );
	CHECK_ERR( GenMinDesktopIntel			( fs_infos ),	-10 );
	CHECK_ERR( GenMinMobile					( fs_infos ),	-10 );
	CHECK_ERR( GenMinMobileMali				( fs_infos ),	-10 );
	CHECK_ERR( GenMinMobileAdreno			( fs_infos ),	-10 );
	CHECK_ERR( GenMinMobilePowerVR			( fs_infos ),	-10 );
	CHECK_ERR( GenMinApple					( fs_infos ),	-10 );

	// TODO:
	//	HightPerfDesktop
	//	HightPerfMobile

	//CHECK_ERR( SaveToFile( fs_infos ), -11 );

	return 0;
}
