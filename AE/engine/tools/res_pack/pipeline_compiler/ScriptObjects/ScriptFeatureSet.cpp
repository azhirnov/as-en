// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_pack/pipeline_compiler/ScriptObjects/ScriptFeatureSet.h"
#include "res_pack/pipeline_compiler/ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
	enum class EFormatFeature : uint
	{
		Unknown,
		StorageImageAtomic,
		StorageImage,
		AttachmentBlend,
		Attachment,					// color or depth
		LinearSampled,
		UniformTexelBuffer,
		StorageTexelBuffer,
		StorageTexelBufferAtomic,
		HWCompressedAttachment,		// lossless
		LossyCompressedAttachment,
		_Count
	};
}

AE_DECL_SCRIPT_TYPE( AE::PipelineCompiler::EFormatFeature, "EFormatFeature" );

namespace AE::PipelineCompiler
{
namespace
{
	using namespace AE::Scripting;

	static ScriptFeatureSet*  ScriptFeatureSet_Ctor (const String &name) {
		return ScriptFeatureSet::Create( name ).Detach();
	}
//-----------------------------------------------------------------------------


	using PerDescriptorSet		= Graphics::FeatureSet::PerDescriptorSet;
	using PerShaderStage		= Graphics::FeatureSet::PerShaderStage;
	using SubgroupOperationBits	= Graphics::FeatureSet::SubgroupOperationBits;
	using VendorIDs_t			= Graphics::FeatureSet::VendorIDs_t;
	using GraphicsDevices_t		= Graphics::FeatureSet::GraphicsDevices_t;
	using PixelFormatSet_t		= Graphics::FeatureSet::PixelFormatSet_t;
	using VertexFormatSet_t		= Graphics::FeatureSet::VertexFormatSet_t;
	using SampleCountBits		= Graphics::FeatureSet::SampleCountBits;
	using ShaderVersion			= Graphics::FeatureSet::ShaderVersion;
	using Queues				= Graphics::FeatureSet::Queues;
	using SurfaceFormatSet_t	= Graphics::FeatureSet::SurfaceFormatSet_t;
	using ShadingRateSet_t		= Graphics::FeatureSet::ShadingRateSet_t;
	using CoopMatrixSet_t		= Graphics::FeatureSet::CoopMatrixSet_t;
	using CoopVecSet_t			= Graphics::FeatureSet::CoopVecSet_t;
	using VRSTexelSize			= Graphics::FeatureSet::VRSTexelSize;
	using EFeature				= Graphics::FeatureSet::EFeature;
	using KiBytes				= Graphics::FeatureSet::KiBytes;

	template <typename T>	struct FS_ReplaceInType						{ using dst = T;	using src = T;				};
	template <>				struct FS_ReplaceInType< ubyte >			{ using dst = uint;	using src = ubyte;			};
	template <>				struct FS_ReplaceInType< ushort >			{ using dst = uint;	using src = ushort;			};
	template <>				struct FS_ReplaceInType< EShaderStages >	{ using dst = uint;	using src = EShaderStages;	};
	template <>				struct FS_ReplaceInType< ESubgroupTypes >	{ using dst = uint;	using src = ESubgroupTypes;	};
	template <>				struct FS_ReplaceInType< POTValue >			{ using dst = uint;	using src = POTValue;		};
	template <>				struct FS_ReplaceInType< POTBytes >			{ using dst = uint;	using src = POTBytes;		};
	template <>				struct FS_ReplaceInType< KiBytes >			{ using dst = uint;	using src = KiBytes;		};
	template <>				struct FS_ReplaceInType< Bytes32u >			{ using dst = uint;	using src = Bytes32u;		};

	template <typename T>	struct FS_ReplaceOutType					{ using dst = T;	static T	Cast (T src)				{ return src; }};
	template <>				struct FS_ReplaceOutType< EFeature >		{ using dst = bool;	static dst	Cast (EFeature src)			{ return src == EFeature::RequireTrue; }};
	template <>				struct FS_ReplaceOutType< POTValue >		{ using dst = uint;	static dst	Cast (POTValue src)			{ return uint{src}; }};
	template <>				struct FS_ReplaceOutType< POTBytes >		{ using dst = uint;	static dst	Cast (POTBytes src)			{ return uint{src}; }};
	template <>				struct FS_ReplaceOutType< SampleCountBits >	{ using dst = uint;	static dst	Cast (SampleCountBits src)	{ return uint(src); }};


	#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )															\
		static void  Set_FS_ ## _name_ (ScriptFeatureSet* ptr, const typename FS_ReplaceInType<_type_>::dst val) {	\
			ptr->fs._name_ = typename FS_ReplaceInType<_type_>::src( val );											\
		}																											\
		static typename FS_ReplaceOutType<_type_>::dst  Get_FS_ ## _name_ (ScriptFeatureSet* ptr) {					\
			 return FS_ReplaceOutType<_type_>::Cast( ptr->fs._name_ );												\
		}
	AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
	#undef AE_FEATURE_SET_VISIT

	#define AE_FEATURE_SET_PER_DS( _visitor_ ) \
		_visitor_( uint,	maxInputAttachments	);\
		_visitor_( uint,	maxSampledImages	);\
		_visitor_( uint,	maxSamplers			);\
		_visitor_( uint,	maxStorageBuffers	);\
		_visitor_( uint,	maxStorageImages	);\
		_visitor_( uint,	maxUniformBuffers	);\
		_visitor_( uint,	maxAccelStructures	);\
		_visitor_( uint,	maxTotalResources	);\

	#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ )\
		static void		Set_FS_perPipeline_ ## _name_ (ScriptFeatureSet* ptr, const _type_ val)	{ ptr->fs.perPipeline._name_ = val; }\
		static _type_	Set_FS_getPerPipeline_ ## _name_ (ScriptFeatureSet* ptr)				{ return ptr->fs.perPipeline._name_; }
	AE_FEATURE_SET_PER_DS( AE_FEATURE_SET_PER_DS_VISIT )
	#undef AE_FEATURE_SET_PER_DS_VISIT

	#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ )\
		static void		Set_FS_perStage_ ## _name_ (ScriptFeatureSet* ptr, const _type_ val)	{ ptr->fs.perStage._name_ = val; }\
		static _type_	Set_FS_getPerStage_ ## _name_ (ScriptFeatureSet* ptr)					{ return ptr->fs.perStage._name_; }
	AE_FEATURE_SET_PER_DS( AE_FEATURE_SET_PER_DS_VISIT )
	#undef AE_FEATURE_SET_PER_DS_VISIT

	static void  FS_AddSubgroupOperation (ScriptFeatureSet* ptr, ESubgroupOperation val) {
		ptr->fs.subgroupOperations.insert( val );
	}

	static void  FS_AddSubgroupOperations (ScriptFeatureSet* ptr, const ScriptArray<ESubgroupOperation> &arr)
	{
		for (auto val : arr) {
			ptr->fs.subgroupOperations.insert( val );
		}
	}

	static void  FS_AddSubgroupOperationRange (ScriptFeatureSet* ptr, ESubgroupOperation first, ESubgroupOperation last) {
		CHECK_THROW_MSG( first <= last );
		ptr->fs.subgroupOperations.InsertRange( first, last );
	}

	static void  FS_AddTexelFormats (ScriptFeatureSet* ptr, EFormatFeature features, const ScriptArray<EPixelFormat> &arr) __Th___
	{
		PixelFormatSet_t	set;
		for (auto fmt : arr) {
			set.insert( fmt );
		}

		switch_enum( features )
		{
			case EFormatFeature::StorageImageAtomic :		ptr->fs.storageImageAtomicFormats		|= set;	break;
			case EFormatFeature::StorageImage :				ptr->fs.storageImageFormats				|= set;	break;
			case EFormatFeature::AttachmentBlend :			ptr->fs.attachmentBlendFormats			|= set;	break;
			case EFormatFeature::Attachment :				ptr->fs.attachmentFormats				|= set;	break;
			case EFormatFeature::LinearSampled :			ptr->fs.linearSampledFormats			|= set;	break;
			case EFormatFeature::UniformTexelBuffer :		ptr->fs.uniformTexBufferFormats			|= set;	break;
			case EFormatFeature::StorageTexelBuffer :		ptr->fs.storageTexBufferFormats			|= set;	break;
			case EFormatFeature::StorageTexelBufferAtomic:	ptr->fs.storageTexBufferAtomicFormats	|= set;	break;
			case EFormatFeature::HWCompressedAttachment :	ptr->fs.hwCompressedAttachmentFormats	|= set;	break;
			case EFormatFeature::LossyCompressedAttachment:	ptr->fs.lossyCompressedAttachmentFormats|= set;	break;
			case EFormatFeature::Unknown :
			case EFormatFeature::_Count :
			default :										CHECK_THROW_MSG( false, "unknown format feature type" );
		}
		switch_end

		ptr->fs.storageImageFormats		|= ptr->fs.storageImageAtomicFormats;
		ptr->fs.storageTexBufferFormats	|= ptr->fs.storageTexBufferAtomicFormats;
		ptr->fs.attachmentFormats		|= ptr->fs.attachmentBlendFormats;
		ptr->fs.attachmentFormats		|= ptr->fs.hwCompressedAttachmentFormats;
		ptr->fs.attachmentFormats		|= ptr->fs.lossyCompressedAttachmentFormats;
	}

	static void  FS_AddSurfaceFormats (ScriptFeatureSet* ptr, const ScriptArray<ESurfaceFormat> &arr) __Th___
	{
		for (auto sf : arr)
		{
			ptr->fs.surfaceFormats.insert( sf );
			ptr->fs.attachmentFormats.insert( ESurfaceFormat_Cast( sf ).first );
		}
	}

	static void  FS_AddVertexFormats (ScriptFeatureSet* ptr, const ScriptArray<EVertexType> &arr) {
		for (auto vt : arr) {
			ptr->fs.vertexFormats.insert( vt );
		}
	}

	static void  FS_AddAccelStructVertexFormats (ScriptFeatureSet* ptr, const ScriptArray<EVertexType> &arr) {
		for (auto vt : arr) {
			ptr->fs.accelStructVertexFormats.insert( vt );
		}
	}

	static void  FS_framebufferColorSampleCounts (ScriptFeatureSet* ptr, const ScriptArray<uint> &arr) __Th___
	{
		for (uint val : arr)
		{
			CHECK_THROW_MSG( val > 0 and IsPowerOfTwo( val ));
			ptr->fs.framebufferColorSampleCounts = SampleCountBits( uint(ptr->fs.framebufferColorSampleCounts) | val );
		}
	}

	static void  FS_framebufferDepthSampleCounts (ScriptFeatureSet* ptr, const ScriptArray<uint> &arr) __Th___
	{
		for (uint val : arr)
		{
			CHECK_THROW_MSG( val > 0 and IsPowerOfTwo( val ));
			ptr->fs.framebufferDepthSampleCounts = SampleCountBits( uint(ptr->fs.framebufferDepthSampleCounts) | val );
		}
	}

	static void  FS_IncludeVendorId (ScriptFeatureSet* ptr, EGPUVendor val) {
		ptr->fs.vendorIds.include.insert( val );
	}

	static void  FS_ExcludeVendorId (ScriptFeatureSet* ptr, EGPUVendor val) {
		ptr->fs.vendorIds.exclude.insert( val );
	}

	static void  FS_IncludeVendorIds (ScriptFeatureSet* ptr, const ScriptArray<EGPUVendor> &arr) {
		for (auto val : arr) {
			ptr->fs.vendorIds.include.insert( val );
		}
	}

	static void  FS_ExcludeVendorIds (ScriptFeatureSet* ptr, const ScriptArray<EGPUVendor> &arr) {
		for (auto val : arr) {
			ptr->fs.vendorIds.exclude.insert( val );
		}
	}

	static void  FS_IncludeGraphicsDevice (ScriptFeatureSet* ptr, EGraphicsDeviceID val) {
		ptr->fs.devicesIds.include.insert( val );
	}

	static void  FS_ExcludeGraphicsDevice (ScriptFeatureSet* ptr, EGraphicsDeviceID val) {
		ptr->fs.devicesIds.exclude.insert( val );
	}

	static void  FS_maxSpirvVersion (ScriptFeatureSet* ptr, uint val) {
		ptr->fs.maxShaderVersion.spirv = CheckCast{val};
	}

	static void  FS_maxMetalVersion (ScriptFeatureSet* ptr, uint val) {
		ptr->fs.maxShaderVersion.metal = CheckCast{val};
	}

	static void  FS_supportedQueues (ScriptFeatureSet* ptr, EQueueMask val) {
		ptr->fs.queues.supported = val;
	}

	static void  FS_requiredQueues (ScriptFeatureSet* ptr, EQueueMask val) {
		ptr->fs.queues.required = val;
	}

	static void  FS_AddShadingRate (ScriptFeatureSet* ptr, const ScriptArray<uint> &fragSize, const ScriptArray<uint> &samples) __Th___
	{
		CHECK_THROW_MSG( fragSize.size() == 2 );
		CHECK_THROW_MSG( fragSize[0] > 0 and fragSize[1] > 0 );
		CHECK_THROW_MSG( IsPowerOfTwo( fragSize[0] ) and IsPowerOfTwo( fragSize[1] ));
		CHECK_THROW_MSG( not samples.empty() );

		uint	samp_bits = 0;
		for (uint s : samples)
		{
			CHECK_THROW_MSG( s > 0 and IsPowerOfTwo( s ));
			samp_bits |= s;
		}
		CHECK_THROW_MSG( samp_bits != 0 );

		ptr->fs.fragmentShadingRates.push_back( EShadingRate_FromSize({ fragSize[0], fragSize[1] }) | EShadingRate_FromSampleBits( samp_bits ));
		std::sort( ptr->fs.fragmentShadingRates.begin(), ptr->fs.fragmentShadingRates.end() );
	}


	static void  FS_fragmentShadingRateTexelSize (ScriptFeatureSet* ptr, const ScriptArray<uint> &minTexelSize, const ScriptArray<uint> &maxTexelSize, uint aspect)
	{
		CHECK_THROW_MSG( minTexelSize.size() == 2 );
		CHECK_THROW_MSG( maxTexelSize.size() == 2 );
		CHECK_THROW_MSG( minTexelSize[0] > 0 and minTexelSize[1] > 0 );
		CHECK_THROW_MSG( minTexelSize[0] <= 256 and minTexelSize[1] <= 256 );
		CHECK_THROW_MSG( maxTexelSize[0] > 0 and maxTexelSize[1] > 0 );
		CHECK_THROW_MSG( maxTexelSize[0] <= 256 and maxTexelSize[1] <= 256 );
		CHECK_THROW_MSG( IsPowerOfTwo( minTexelSize[0] ) and IsPowerOfTwo( minTexelSize[1] ));
		CHECK_THROW_MSG( IsPowerOfTwo( maxTexelSize[0] ) and IsPowerOfTwo( maxTexelSize[1] ));
		CHECK_THROW_MSG( minTexelSize[0] <= maxTexelSize[0] and minTexelSize[1] <= maxTexelSize[1] );
		CHECK_THROW_MSG( aspect > 0 and aspect <= 256 and IsPowerOfTwo( aspect ));

		ASSERT( ptr->fs.attachmentFragmentShadingRate == EFeature::RequireTrue );

		ptr->fs.fragmentShadingRateTexelSize.minX		= POTValue{ minTexelSize[0] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.minY		= POTValue{ minTexelSize[1] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.maxX		= POTValue{ maxTexelSize[0] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.maxY		= POTValue{ maxTexelSize[1] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.aspectRatio= POTValue{ aspect }.GetPOT();

		CHECK_THROW( ptr->fs.fragmentShadingRateTexelSize.minX <= ptr->fs.fragmentShadingRateTexelSize.maxX );
		CHECK_THROW( ptr->fs.fragmentShadingRateTexelSize.minY <= ptr->fs.fragmentShadingRateTexelSize.maxY );
	}

	static void  FS_AddIntegerDotProduct (ScriptFeatureSet* ptr,  const ScriptArray<EIntegerDotProductFeat> &idotFeats) __Th___
	{
		for (auto value : idotFeats) {
			ptr->fs.integerDotProductFeatures.insert( value );
		}
	}

	static void  FS_MergeMin (ScriptFeatureSet* ptr, ScriptFeatureSet* from) __Th___
	{
		CHECK_THROW_MSG( from != null );
		ptr->fs.MergeMin( from->fs );
	}

	static void  FS_MergeMax (ScriptFeatureSet* ptr, ScriptFeatureSet* from) __Th___
	{
		CHECK_THROW_MSG( from != null );
		ptr->fs.MergeMax( from->fs );
	}

	static void  FS_Copy (ScriptFeatureSet* ptr, ScriptFeatureSet* from) __Th___
	{
		CHECK_THROW_MSG( from != null );
		ptr->fs = from->fs;
	}

	static bool  Has_FS_subgroupOperation (ScriptFeatureSet* ptr, ESubgroupOperation op) __Th___ {
		return ptr->fs.subgroupOperations.contains( op );
	}

	static bool  Has_FS_uniformTexBufferFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.uniformTexBufferFormats.contains( fmt );
	}

	static bool  Has_FS_storageTexBufferFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.storageTexBufferFormats.contains( fmt );
	}

	static bool  Has_FS_storageTexBufferAtomicFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.storageTexBufferAtomicFormats.contains( fmt );
	}

	static bool  Has_FS_storageImageAtomicFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.storageImageAtomicFormats.contains( fmt );
	}

	static bool  Has_FS_storageImageFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.storageImageFormats.contains( fmt );
	}

	static bool  Has_FS_attachmentBlendFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.attachmentBlendFormats.contains( fmt );
	}

	static bool  Has_FS_attachmentFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.attachmentFormats.contains( fmt );
	}

	static bool  Has_FS_linearSampledFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.linearSampledFormats.contains( fmt );
	}

	static bool  Has_FS_minmaxFilterFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.minmaxFilterFormats.contains( fmt );
	}

	static bool  Has_FS_hwCompressedAttachmentFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.hwCompressedAttachmentFormats.contains( fmt );
	}

	static bool  Has_FS_lossyCompressedAttachmentFormat (ScriptFeatureSet* ptr, EPixelFormat fmt) __Th___ {
		return ptr->fs.lossyCompressedAttachmentFormats.contains( fmt );
	}

	static bool  Has_FS_vertexFormat (ScriptFeatureSet* ptr, EVertexType fmt) __Th___ {
		return ptr->fs.vertexFormats.contains( fmt );
	}

	static bool  Has_FS_accelStructVertexFormat (ScriptFeatureSet* ptr, EVertexType fmt) __Th___ {
		return ptr->fs.accelStructVertexFormats.contains( fmt );
	}

	static bool  Has_FS_surfaceFormat (ScriptFeatureSet* ptr, ESurfaceFormat fmt) __Th___ {
		return ptr->fs.surfaceFormats.contains( fmt );
	}

	static bool  Has_FS_cooperativeMatrixConfig (ScriptFeatureSet* ptr, ECoopMatrixCfg cfg) __Th___ {
		return ptr->fs.cooperativeMatrixConfig.contains( cfg );
	}

	static bool  Has_FS_cooperativeVectorConfig (ScriptFeatureSet* ptr, ECoopVecCfg cfg) __Th___ {
		return ptr->fs.cooperativeVectorConfig.contains( cfg );
	}

	static bool  Has_FS_integerDotProductFeature (ScriptFeatureSet* ptr, EIntegerDotProductFeat feat) __Th___ {
		return ptr->fs.integerDotProductFeatures.contains( feat );
	}
}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptFeatureSet::ScriptFeatureSet (const String &name) __NE___ :
		_name{name}, _hash{name}
	{}

	ScriptFeatureSetPtr  ScriptFeatureSet::Create (const String &name) __Th___
	{
		ScriptFeatureSetPtr	result	{new ScriptFeatureSet{ name }};
		auto&				storage	= *ObjectStorage::Instance();

		result->fs.Init( EFeature::Ignore );

		storage.AddName<FeatureSetName>( name );
		CHECK_THROW_MSG( storage.featureSets.emplace( name, result ).second,
			"FeatureSet with name '"s << name << "' is already defined" );

		return result;
	}

/*
=================================================
	Build
=================================================
*/
	void  ScriptFeatureSet::Build () __NE___
	{
		// TODO
		CHECK( fs.IsValid() );
	}

/*
=================================================
	Find
=================================================
*/
	ScriptFeatureSet*  ScriptFeatureSet::Find (const String &fsName) __Th___
	{
		const auto&	feature_set	= ObjectStorage::Instance()->featureSets;
		auto		iter		= feature_set.find( FeatureSetName{fsName} );
		CHECK_THROW_MSG( iter != feature_set.end(),
			"FeatureSet '"s << fsName << "' is not exists" );
		return iter->second.Get();
	}

/*
=================================================
	Minimize
=================================================
*/
	void  ScriptFeatureSet::Minimize (INOUT Array<ScriptFeatureSetPtr> &feats)
	{
		RemoveDuplicates( INOUT feats );

		// required for tests
		std::sort( feats.begin(), feats.end(), [](auto& lhs, auto &rhs) { return lhs->Hash() < rhs->Hash(); });
	}

/*
=================================================
	TestFeature_VertexType
=================================================
*/
	String  ScriptFeatureSet::GetNames (ArrayView<ScriptFeatureSetPtr> features)
	{
		String	str = " (feature sets: ";
		for (auto& feat : features)
		{
			if ( &feat != features.data() )
				str << ", ";

			str << feat->Name();
		}
		str << ")";
		return str;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptFeatureSet::Bind (const ScriptEnginePtr &se) __Th___
	{
		{
			EnumBinder<EFormatFeature>	binder{ se };
			binder.Create();
			switch_enum( EFormatFeature::Unknown )
			{
				case EFormatFeature::Unknown :
				case EFormatFeature::_Count :
				#define BIND( _name_ )		case EFormatFeature::_name_ : binder.AddValue( #_name_, EFormatFeature::_name_ );
				BIND( StorageImageAtomic )
				BIND( StorageImage )
				BIND( AttachmentBlend )
				BIND( Attachment )
				BIND( LinearSampled )
				BIND( UniformTexelBuffer )
				BIND( StorageTexelBuffer )
				BIND( StorageTexelBufferAtomic )
				BIND( HWCompressedAttachment )
				BIND( LossyCompressedAttachment )
				#undef BIND
				default : break;
			}
			switch_end
		}
		{
			ClassBinder<ScriptFeatureSet>	binder{ se };
			binder.CreateRef( 0, False{} );

			const auto	ToMethodName = [] (StringView prefix, String s)
			{{
				return String{prefix} + s;
			}};
			const auto	ToMethodName2 = [] (StringView prefix, String s)
			{{
				s[0] = ToUpperCase( s[0] );
				return String{prefix} + s;
			}};

			binder.AddFactoryCtor( &ScriptFeatureSet_Ctor, {"name"} );
			AS_METHOD( binder, FS_AddTexelFormats,				"AddTexelFormats",				{} );
			AS_METHOD( binder, FS_AddSurfaceFormats,			"AddSurfaceFormats",			{} );
			AS_METHOD( binder, FS_AddSubgroupOperation,			"AddSubgroupOperation",			{} );
			AS_METHOD( binder, FS_AddSubgroupOperations,		"AddSubgroupOperations",		{} );
			AS_METHOD( binder, FS_AddSubgroupOperationRange,	"AddSubgroupOperationRange",	{} );
			AS_METHOD( binder, FS_AddVertexFormats,				"AddVertexFormats",				{} );
			AS_METHOD( binder, FS_AddAccelStructVertexFormats,	"AddAccelStructVertexFormats",	{} );
			AS_METHOD( binder, FS_IncludeVendorId,				"IncludeVendor",				{} );
			AS_METHOD( binder, FS_ExcludeVendorId,				"ExcludeVendor",				{} );
			AS_METHOD( binder, FS_IncludeVendorIds,				"IncludeVendors",				{} );
			AS_METHOD( binder, FS_ExcludeVendorIds,				"ExcludeVendors",				{} );
			AS_METHOD( binder, FS_IncludeGraphicsDevice,		"IncludeDevice",				{} );
			AS_METHOD( binder, FS_ExcludeGraphicsDevice,		"ExcludeDevice",				{} );
			AS_METHOD( binder, FS_MergeMin,						"MergeMin",						{} );
			AS_METHOD( binder, FS_MergeMax,						"MergeMax",						{} );
			AS_METHOD( binder, FS_Copy,							"Copy",							{} );
			AS_METHOD( binder, FS_framebufferColorSampleCounts,	"framebufferColorSampleCounts",	{} );
			AS_METHOD( binder, FS_framebufferDepthSampleCounts,	"framebufferDepthSampleCounts",	{} );
			AS_METHOD( binder, FS_maxSpirvVersion,				"maxSpirvVersion",				{} );
			AS_METHOD( binder, FS_maxMetalVersion,				"maxMetalVersion",				{} );
			AS_METHOD( binder, FS_supportedQueues,				"supportedQueues",				{} );
			AS_METHOD( binder, FS_requiredQueues,				"requiredQueues",				{} );
			AS_METHOD( binder, FS_AddShadingRate,				"AddShadingRate",				{} );
			AS_METHOD( binder, FS_fragmentShadingRateTexelSize,	"fragmentShadingRateTexelSize",	{} );
			AS_METHOD( binder, FS_AddIntegerDotProduct,			"AddIntegerDotProduct",			{} );

			AS_METHOD( binder, Has_FS_subgroupOperation,			"hasSubgroupOperation",				{} );
			AS_METHOD( binder, Has_FS_uniformTexBufferFormat,		"hasUniformTexBufferFormat",		{} );
			AS_METHOD( binder, Has_FS_storageTexBufferFormat,		"hasStorageTexBufferFormat",		{} );
			AS_METHOD( binder, Has_FS_storageTexBufferAtomicFormat,	"hasStorageTexBufferAtomicFormat",	{} );
			AS_METHOD( binder, Has_FS_storageImageAtomicFormat,		"hasStorageImageAtomicFormat",		{} );
			AS_METHOD( binder, Has_FS_storageImageFormat,			"hasStorageImageFormat",			{} );
			AS_METHOD( binder, Has_FS_attachmentBlendFormat,		"hasAttachmentBlendFormat",			{} );
			AS_METHOD( binder, Has_FS_attachmentFormat,				"hasAttachmentFormat",				{} );
			AS_METHOD( binder, Has_FS_linearSampledFormat,			"hasLinearSampledFormat",			{} );
			AS_METHOD( binder, Has_FS_minmaxFilterFormat,			"hasMinmaxFilterFormat",			{} );
			AS_METHOD( binder, Has_FS_hwCompressedAttachmentFormat,	"hasHwCompressedAttachmentFormat",	{} );
			AS_METHOD( binder, Has_FS_lossyCompressedAttachmentFormat,"hasLossyCompressedAttachmentFormat",	{} );
			AS_METHOD( binder, Has_FS_vertexFormat,					"hasVertexFormat",					{} );
			AS_METHOD( binder, Has_FS_accelStructVertexFormat,		"hasAccelStructVertexFormat",		{} );
			AS_METHOD( binder, Has_FS_surfaceFormat,				"hasSurfaceFormat",					{} );
			AS_METHOD( binder, Has_FS_cooperativeMatrixConfig,		"hasCooperativeMatrixConfig",		{} );
			AS_METHOD( binder, Has_FS_cooperativeVectorConfig,		"hasCooperativeVectorConfig",		{} );
			AS_METHOD( binder, Has_FS_integerDotProductFeature,		"hasIntegerDotProductFeature",		{} );

			#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )					\
				if constexpr( (not IsSame< _type_, PerDescriptorSet			>)	and \
							  (not IsSame< _type_, ShaderVersion			>)	and \
							  (not IsSame< _type_, SubgroupOperationBits	>)	and \
							  (not IsSame< _type_, SampleCountBits			>)	and \
							  (not IsSame< _type_, VendorIDs_t				>)	and \
							  (not IsSame< _type_, GraphicsDevices_t		>)	and \
							  (not IsSame< _type_, PixelFormatSet_t			>)	and	\
							  (not IsSame< _type_, VertexFormatSet_t		>)	and	\
							  (not IsSame< _type_, SurfaceFormatSet_t		>)	and	\
							  (not IsSame< _type_, Queues					>)	and	\
							  (not IsSame< _type_, ShadingRateSet_t			>)	and	\
							  (not IsSame< _type_, CoopMatrixSet_t			>)	and	\
							  (not IsSame< _type_, CoopVecSet_t				>)	and	\
							  (not IsSame< _type_, EIntegerDotProductFeats	>)	and	\
							  (not IsSame< _type_, VRSTexelSize				>))		\
					AS_METHOD( binder, Set_FS_ ## _name_, ToMethodName( "", AE_TOSTRING( _name_ )), {} );		\
																												\
				if constexpr( IsSame< _type_, EFeature >)														\
					AS_METHOD( binder, Get_FS_ ## _name_, ToMethodName2( "has", AE_TOSTRING( _name_ )), {} );	\
				else																							\
				if constexpr( IsEnum< _type_ >)																	\
					AS_METHOD( binder, Get_FS_ ## _name_, ToMethodName2( "get", AE_TOSTRING( _name_ )), {} );	\
																												\
				if constexpr( IsInteger< _type_ > or IsPowerOf2Value< _type_ >)									\
					AS_METHOD( binder, Get_FS_ ## _name_, ToMethodName2( "get", AE_TOSTRING( _name_ )), {} );	\

			AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
			#undef AE_FEATURE_SET_VISIT

			#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ ) \
				AS_METHOD( binder, Set_FS_perPipeline_ ## _name_,    ToMethodName( "perPipeline_",    AE_TOSTRING( _name_ )), {} );	\
				AS_METHOD( binder, Set_FS_getPerPipeline_ ## _name_, ToMethodName( "getPerPipeline_", AE_TOSTRING( _name_ )), {} );
			AE_FEATURE_SET_PER_DS( AE_FEATURE_SET_PER_DS_VISIT )
			#undef AE_FEATURE_SET_PER_DS_VISIT

			#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ ) \
				AS_METHOD( binder, Set_FS_perStage_ ## _name_,    ToMethodName( "perStage_",    AE_TOSTRING( _name_ )), {} );	\
				AS_METHOD( binder, Set_FS_getPerStage_ ## _name_, ToMethodName( "getPerStage_", AE_TOSTRING( _name_ )), {} );
			AE_FEATURE_SET_PER_DS( AE_FEATURE_SET_PER_DS_VISIT )
			#undef AE_FEATURE_SET_PER_DS_VISIT
		}
		AS_GLOBAL_FN( se, ScriptFeatureSet::Find, "FindFeatureSet", {"name"} );

		Unused( &Set_FS_subgroupOperations, &Set_FS_perPipeline, &Set_FS_perStage,
				&Set_FS_storageImageFormats, &Set_FS_storageImageAtomicFormats,
				&Set_FS_attachmentBlendFormats, &Set_FS_attachmentFormats,
				&Set_FS_vertexFormats, &Set_FS_uniformTexBufferFormats, &Set_FS_storageTexBufferFormats,
				&Set_FS_storageTexBufferAtomicFormats, &Set_FS_linearSampledFormats,
				&Set_FS_framebufferColorSampleCounts, &Set_FS_framebufferDepthSampleCounts,
				&Set_FS_surfaceFormats, &Set_FS_vendorIds, &Set_FS_devicesIds, &Set_FS_accelStructVertexFormats,
				&Set_FS_maxShaderVersion, &Set_FS_queues, &Set_FS_hwCompressedAttachmentFormats,
				&Set_FS_lossyCompressedAttachmentFormats, &Set_FS_fragmentShadingRates, &Set_FS_fragmentShadingRateTexelSize );
	}

/*
=================================================
	TestFeature_PixelFormat
=================================================
*/
	void  TestFeature_PixelFormat (ArrayView<ScriptFeatureSetPtr> features, EnumSet<EPixelFormat> FeatureSet::*member,
								   EPixelFormat fmt, StringView memberName, StringView message) __Th___
	{
		CHECK_THROW_MSG( not features.empty(), "empty FeatureSet array" );

		if ( fmt == EPixelFormat::SwapchainColor )
			return;  // always supported

		CHECK_THROW( fmt < EPixelFormat::_Count );

		bool	supported = false;

		for (auto& feat : features)
		{
			EnumSet<EPixelFormat> const&	set = feat->fs.*member;
			if ( set.contains( fmt ))
				supported = true;
		}

		CHECK_THROW_MSG( supported,
			"PixelFormat "s << ToString( fmt ) << " is not supported in '" << memberName << "' " << message <<
			ScriptFeatureSet::GetNames( features ));
	}

/*
=================================================
	TestFeature_VertexType
=================================================
*/
	void  TestFeature_VertexType (ArrayView<ScriptFeatureSetPtr> features, EnumSet<EVertexType> FeatureSet::*member,
								  EVertexType fmt, StringView memberName, StringView message) __Th___
	{
		CHECK_THROW_MSG( not features.empty(), "empty FeatureSet array" );
		CHECK_THROW( fmt < EVertexType::_Count );

		bool	supported = false;

		for (auto& feat : features)
		{
			EnumSet<EVertexType> const&	set = feat->fs.*member;
			if ( set.contains( fmt ))
				supported = true;
		}

		CHECK_THROW_MSG( supported,
			"VertexType "s << ToString( fmt ) << " is not supported in '" << memberName << "'" << message <<
			ScriptFeatureSet::GetNames( features ));
	}


} // AE::PipelineCompiler
