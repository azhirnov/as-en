// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptFeatureSet.h"
#include "ScriptObjects/Common.inl.h"

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
		return ScriptFeatureSetPtr{ new ScriptFeatureSet{ name }}.Detach();
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
	using VRSTexelSize			= Graphics::FeatureSet::VRSTexelSize;
	using EFeature				= Graphics::FeatureSet::EFeature;

	template <typename T>	struct FS_ReplaceInType						{ using dst = T;	using src = T;				};
	template <>				struct FS_ReplaceInType< ubyte >			{ using dst = uint;	using src = ubyte;			};
	template <>				struct FS_ReplaceInType< ushort >			{ using dst = uint;	using src = ushort;			};
	template <>				struct FS_ReplaceInType< EShaderStages >	{ using dst = uint;	using src = EShaderStages;	};
	template <>				struct FS_ReplaceInType< ESubgroupTypes >	{ using dst = uint;	using src = ESubgroupTypes;	};

	template <typename T>	struct FS_ReplaceOutType					{ using dst = T;	static T	Cast (T src)		{ return src; }};
	template <>				struct FS_ReplaceOutType< EFeature >		{ using dst = bool;	static dst	Cast (EFeature src)	{ return src == EFeature::RequireTrue; }};


	#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )															\
		static void  Set_FS_ ## _name_ (ScriptFeatureSet* ptr, const typename FS_ReplaceInType<_type_>::dst val) {	\
			ptr->fs._name_ = typename FS_ReplaceInType<_type_>::src( val );											\
		}																											\
		static typename FS_ReplaceOutType<_type_>::dst  Get_FS_ ## _name_ (ScriptFeatureSet* ptr) {					\
			 return FS_ReplaceOutType<_type_>::Cast( ptr->fs._name_ );											\
		}
	AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
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

	#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ )	static void  Set_FS_perDescrSet_ ## _name_ (ScriptFeatureSet* ptr, const _type_ val) { ptr->fs.perDescrSet._name_ = val; }
	AE_FEATURE_SET_PER_DS( AE_FEATURE_SET_PER_DS_VISIT )
	#undef AE_FEATURE_SET_PER_DS_VISIT

	#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ )	static void  Set_FS_perStage_ ## _name_ (ScriptFeatureSet* ptr, const _type_ val) { ptr->fs.perStage._name_ = val; }
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
		ptr->fs.maxShaderVersion.spirv = CheckCast<ushort>(val);
	}

	static void  FS_maxMetalVersion (ScriptFeatureSet* ptr, uint val) {
		ptr->fs.maxShaderVersion.metal = CheckCast<ushort>(val);
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

		ptr->fs.fragmentShadingRateTexelSize.minX	= POTValue{ minTexelSize[0] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.minY	= POTValue{ minTexelSize[1] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.maxX	= POTValue{ maxTexelSize[0] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.maxY	= POTValue{ maxTexelSize[1] }.GetPOT();
		ptr->fs.fragmentShadingRateTexelSize.aspect	= POTValue{ aspect }.GetPOT();
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
}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptFeatureSet::ScriptFeatureSet (const String &name) __Th___ :
		_name{name}, _hash{name}
	{
		ObjectStorage::Instance()->AddName<FeatureSetName>( name );
		CHECK_THROW_MSG( ObjectStorage::Instance()->featureSets.emplace( _name, ScriptFeatureSetPtr{this} ).second,
			"FeatureSet with name '"s << name << "' is already defined" );
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
			binder.CreateRef();

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
			binder.AddMethodFromGlobalObjFirst( &FS_AddTexelFormats,				"AddTexelFormats",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_AddSurfaceFormats,				"AddSurfaceFormats",			{} );
			binder.AddMethodFromGlobalObjFirst( &FS_AddSubgroupOperation,			"AddSubgroupOperation",			{} );
			binder.AddMethodFromGlobalObjFirst( &FS_AddSubgroupOperations,			"AddSubgroupOperations",		{} );
			binder.AddMethodFromGlobalObjFirst( &FS_AddSubgroupOperationRange,		"AddSubgroupOperationRange",	{} );
			binder.AddMethodFromGlobalObjFirst( &FS_AddVertexFormats,				"AddVertexFormats",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_AddAccelStructVertexFormats,	"AddAccelStructVertexFormats",	{} );
			binder.AddMethodFromGlobalObjFirst( &FS_IncludeVendorId,				"IncludeVendor",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_ExcludeVendorId,				"ExcludeVendor",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_IncludeVendorIds,				"IncludeVendors",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_ExcludeVendorIds,				"ExcludeVendors",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_IncludeGraphicsDevice,			"IncludeDevice",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_ExcludeGraphicsDevice,			"ExcludeDevice",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_MergeMin,						"MergeMin",						{} );
			binder.AddMethodFromGlobalObjFirst( &FS_MergeMax,						"MergeMax",						{} );
			binder.AddMethodFromGlobalObjFirst( &FS_Copy,							"Copy",							{} );
			binder.AddMethodFromGlobalObjFirst( &FS_framebufferColorSampleCounts,	"framebufferColorSampleCounts",	{} );
			binder.AddMethodFromGlobalObjFirst( &FS_framebufferDepthSampleCounts,	"framebufferDepthSampleCounts",	{} );
			binder.AddMethodFromGlobalObjFirst( &FS_maxSpirvVersion,				"maxSpirvVersion",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_maxMetalVersion,				"maxMetalVersion",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_supportedQueues,				"supportedQueues",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_requiredQueues,					"requiredQueues",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_AddShadingRate,					"AddShadingRate",				{} );
			binder.AddMethodFromGlobalObjFirst( &FS_fragmentShadingRateTexelSize,	"fragmentShadingRateTexelSize",	{} );

			#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )						\
				if constexpr( (not IsSameTypes< _type_, PerDescriptorSet		>)	and \
							  (not IsSameTypes< _type_, ShaderVersion			>)	and \
							  (not IsSameTypes< _type_, SubgroupOperationBits	>)	and \
							  (not IsSameTypes< _type_, SampleCountBits			>)	and \
							  (not IsSameTypes< _type_, VendorIDs_t				>)	and \
							  (not IsSameTypes< _type_, GraphicsDevices_t		>)	and \
							  (not IsSameTypes< _type_, PixelFormatSet_t		>)	and	\
							  (not IsSameTypes< _type_, VertexFormatSet_t		>)	and	\
							  (not IsSameTypes< _type_, SurfaceFormatSet_t		>)	and	\
							  (not IsSameTypes< _type_, Queues					>)	and	\
							  (not IsSameTypes< _type_, ShadingRateSet_t		>)	and	\
							  (not IsSameTypes< _type_, VRSTexelSize			>))		\
					binder.AddMethodFromGlobalObjFirst( &Set_FS_ ## _name_, ToMethodName( "", AE_TOSTRING( _name_ )), {} );\
				\
				if constexpr( IsSameTypes< _type_, EFeature >)\
					binder.AddMethodFromGlobalObjFirst( &Get_FS_ ## _name_, ToMethodName2( "has", AE_TOSTRING( _name_ )), {} );

			AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
			#undef AE_FEATURE_SET_VISIT

			#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ ) \
				binder.AddMethodFromGlobalObjFirst( &Set_FS_perDescrSet_ ## _name_, ToMethodName( "perDescrSet_", AE_TOSTRING( _name_ )), {} );
			AE_FEATURE_SET_PER_DS( AE_FEATURE_SET_PER_DS_VISIT )
			#undef AE_FEATURE_SET_PER_DS_VISIT

			#define AE_FEATURE_SET_PER_DS_VISIT( _type_, _name_ ) \
				binder.AddMethodFromGlobalObjFirst( &Set_FS_perStage_ ## _name_, ToMethodName( "perStage_", AE_TOSTRING( _name_ )), {} );
			AE_FEATURE_SET_PER_DS( AE_FEATURE_SET_PER_DS_VISIT )
			#undef AE_FEATURE_SET_PER_DS_VISIT
		}
		se->AddFunction( &ScriptFeatureSet::Find, "FindFeatureSet", {"name"} );

		Unused( &Set_FS_subgroupOperations, &Set_FS_perDescrSet, &Set_FS_perStage,
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
