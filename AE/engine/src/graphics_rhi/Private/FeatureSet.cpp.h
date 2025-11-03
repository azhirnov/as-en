// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_rhi/Private/EnumUtils.h"
#include "graphics_rhi/Private/EnumToString.h"
#include "graphics_rhi/Public/FeatureSet.h"


template <>
struct std::hash< AE::Graphics::FeatureSet::PerDescriptorSet > :
	AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::PerDescriptorSet >
{};

template <typename E>
struct std::hash< AE::Graphics::FeatureSet::IncludeExcludeBits<E> > :
	AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::IncludeExcludeBits<E> >
{};

template <>
struct std::hash< AE::Graphics::FeatureSet::ShaderVersion > :
	AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::ShaderVersion >
{};

template <>
struct std::hash< AE::Graphics::FeatureSet::Queues > :
	AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::Queues >
{};

template <>
struct std::hash< AE::Graphics::FeatureSet::VRSTexelSize > :
	AE::Graphics::DefaultHasher_CalcHash< AE::Graphics::FeatureSet::VRSTexelSize >
{};

template <>
struct std::hash< AE::Graphics::FeatureSet::KiBytes >
{
	ND_ size_t  operator () (const AE::Graphics::FeatureSet::KiBytes &x) C_NE___ {
		return std::hash<AE::ushort>{}( x._value );
	}
};


#ifdef AE_ENABLE_LOGS
namespace AE::Base
{
	using namespace AE::Graphics;

	using EFeature	= FeatureSet::EFeature;


	ND_ StringView  ToString (EFeature f)
	{
		switch ( f ) {
			case EFeature::Ignore :			return "Ignore";
			case EFeature::RequireFalse :	return "RequireFalse";
			case EFeature::RequireTrue :	return "RequireTrue";
		}
		return "";
	}

	ND_ String  ToString (ESubgroupTypes types)
	{
		String	str;
		for (auto t : BitfieldIterate( types ))
		{
			if ( not str.empty() )
				str << " | ";

			switch_enum( t )
			{
				case ESubgroupTypes::Float32 :	str << "Float32";	break;
				case ESubgroupTypes::Int32 :	str << "Int32";		break;

				case ESubgroupTypes::Int8 :		str << "Int8";		break;
				case ESubgroupTypes::Int16 :	str << "Int16";		break;
				case ESubgroupTypes::Int64 :	str << "Int64";		break;
				case ESubgroupTypes::Float16 :	str << "Float16";	break;

				case ESubgroupTypes::Unknown :
				case ESubgroupTypes::_Last :
				case ESubgroupTypes::All :		break;
			}
			switch_end
		}
		return str;
	}

	ND_ String  ToString (FeatureSet::SampleCountBits bits)
	{
		if (uint(bits) == 0)
			return "";

		String	str;
		for (uint bit : Base::BitfieldIterate(uint(bits)))
		{
			str << ToString( bit ) << ", ";
		}
		str.pop_back();
		str.pop_back();
		return str;
	}

	ND_ String  ToString (const FeatureSet::VRSTexelSize &size)
	{
		return "min: "s << Base::ToString( size.Min() ) << " max: " << Base::ToString( size.Max() );
	}

	ND_ String  ToString (FeatureSet::KiBytes value)
	{
		return ToString( Bytes{value} );
	}

	ND_ String  ToString (EShadingRate type)
	{
		return "size: "s << Base::ToString( EShadingRate_Size( type )) << " samples: " << Base::ToString<16>( EShadingRate_SampleBits( type ));
	}

	ND_ String  ToString (const FeatureSet::ShadingRateSet_t &set)
	{
		String	str;
		for (auto& sr : set)	str << Base::ToString( sr );
		return str;
	}

	ND_ StringView  ToString (FeatureSet::ShaderVersion)
	{
		return "";
	}

	ND_ StringView  ToString (const FeatureSet::PerDescriptorSet &ds)
	{
		return	"\n    maxInputAttachments: "s << Base::ToString( ds.maxInputAttachments ) <<
				"\n    maxSampledImages:    " << Base::ToString( ds.maxSampledImages ) <<
				"\n    maxSamplers:         " << Base::ToString( ds.maxSamplers ) <<
				"\n    maxStorageBuffers:   " << Base::ToString( ds.maxStorageBuffers ) <<
				"\n    maxStorageImages:    " << Base::ToString( ds.maxStorageImages ) <<
				"\n    maxUniformBuffers:   " << Base::ToString( ds.maxUniformBuffers ) <<
				"\n    maxAccelStructures:  " << Base::ToString( ds.maxAccelStructures ) <<
				"\n    maxTotalResources:   " << Base::ToString( ds.maxTotalResources );
	}

	ND_ StringView  ToString (FeatureSet::Queues)
	{
		return "";
	}

	ND_ StringView  ToString (FeatureSet::VendorIDs_t)
	{
		return "";
	}

	ND_ StringView  ToString (FeatureSet::GraphicsDevices_t)
	{
		return "";
	}
	
	ND_ StringView  ToString (ECoopMatrixCfg type)
	{
		switch_enum( type )
		{
			case ECoopMatrixCfg::Afp16_Bfp16_Cfp16_Rfp16_M16_N16_K16 :		return "A: fp16, B: fp16, C: fp16, Res: fp16, MxNxK: 16x16x16";
			case ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M16_N16_K16 :		return "A: fp16, B: fp16, C: fp32, Res: fp32, MxNxK: 16x16x16";
			case ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M8_N8_K16 :		return "A: fp16, B: fp16, C: fp32, Res: fp32, MxNxK: 8x8x32";
			case ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M16_N16_K32 :			return "A: u8, B: u8, C: u32, Res: u32, MxNxK: 16x16x32";
			case ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M16_N16_K32 :			return "A: s8, B: s8, C: s32, Res: s32, MxNxK: 16x16x32";
			case ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M8_N8_K32 :				return "A: u8, B: u8, C: u32, Res: u32, MxNxK: 8x8x32";
			case ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M8_N8_K32 :				return "A: s8, B: s8, C: s32, Res: s32, MxNxK: 8x8x32";
			case ECoopMatrixCfg::_Count :									break;
		}
		switch_end
		return "";
	}

	ND_ StringView  ToString (ECoopVecCfg type)
	{
		switch_enum( type )
		{
			case ECoopVecCfg::Tfp16_Ifp16_Mfp16_Bfp16_Rfp16_Tp :		return "Vec: fp16, In: fp16, Mat: fp16, Bias: fp16, Res: fp16, transpose";
			case ECoopVecCfg::Ts8_Is8_Ms8_Bs32_Rs32 :					return "Vec: s8, In: s8, Mat: s8, Bias: s32, Res: s32";
			case ECoopVecCfg::Tfp16_Ifp8e4m3_Mfp8e4m3_Bfp16_Rfp16 :		return "Vec: fp16, In: fp8_e7m3, Mat: fp8_e7m3, Bias: fp16, Res: fp16";
			case ECoopVecCfg::Tfp16_Ifp8e5m2_Mfp8e5m2_Bfp16_Rfp16 :		return "Vec: fp16, In: fp8_e5m2, Mat: fp8_e5m2, Bias: fp16, res: fp16";
			case ECoopVecCfg::_Count :									break;
		}
		switch_end
		return "";
	}

	ND_ StringView  EIntegerDotProductFeat_ToString (EIntegerDotProductFeat type)
	{
		switch_enum( type )
		{
			#define CASE( _name_ )	case EIntegerDotProductFeat::_name_: return AE_TOSTRING( _name_ );
			CASE( Unsigned8bit )
			CASE( Signed8bit )
			CASE( MixedSignedness8bit )
			CASE( Unsigned4x8bit )
			CASE( Signed4x8bit )
			CASE( MixedSignedness4x8bit )
			CASE( Unsigned16bit )
			CASE( Signed16bit )
			CASE( MixedSignedness16bit )
			CASE( Unsigned32bit )
			CASE( Signed32bit )
			CASE( MixedSignedness32bit )
			CASE( Unsigned64bit )
			CASE( Signed64bit )
			CASE( MixedSignedness64bit )
			CASE( AccSat_Unsigned8bit )
			CASE( AccSat_Signed8bit )
			CASE( AccSat_MixedSignedness8bit )
			CASE( AccSat_Unsigned4x8bit )
			CASE( AccSat_Signed4x8bit )
			CASE( AccSat_MixedSignedness4x8bit )
			CASE( AccSat_Unsigned16bit )
			CASE( AccSat_Signed16bit )
			CASE( AccSat_MixedSignedness16bit )
			CASE( AccSat_Unsigned32bit )
			CASE( AccSat_Signed32bit )
			CASE( AccSat_MixedSignedness32bit )
			CASE( AccSat_Unsigned64bit )
			CASE( AccSat_Signed64bit )
			CASE( AccSat_MixedSignedness64bit )
			case EIntegerDotProductFeat::_Count : break;
			#undef CASE
		}
		switch_end
		return "";
	}
	
	ND_ String  ToString (EIntegerDotProductFeats bits)
	{
		return Base::ToString( bits, &EIntegerDotProductFeat_ToString );
	}

	template <typename E>
	ND_ String  ToString (const EnumSet<E> &bits)
	{
		return ToString( bits, &ToString );
	}

} // AE::Base
#endif // AE_ENABLE_LOGS


namespace AE::Graphics
{
namespace
{
	template <typename T1, typename T2>
	using MajType2 = Conditional< (sizeof(T1) >= sizeof(T2)), T1, T2 >;

	template <typename T1, typename T2>
	using MajType = MajType2< RemoveCVRef<T1>, RemoveCVRef<T2> >;

/*
=================================================
	GetVertexTypeBits
=================================================
*/
	NdCx__ static FeatureSet::VertexFormatSet_t  GetVertexTypeBits () __NE___
	{
		FeatureSet::VertexFormatSet_t	bits;
		#define AE_VERTEXTYPE_VISIT( _name_, _value_ )	bits.insert( EVertexType::_name_ );
		AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT );
		#undef AE_VERTEXTYPE_VISIT
		return bits;
	}

	static constexpr FeatureSet::VertexFormatSet_t	VertexTypeBits = GetVertexTypeBits();

/*
=================================================
	GetVertexTypeBits
=================================================
*/
#ifdef AE_ENABLE_LOGS

	template <typename T>
	ND_ static String  FS_DiffToString (const T &, const T &)	{ return "-"; }

	ND_ static String  FS_DiffToString (const FeatureSet::PixelFormatSet_t &lhs, const FeatureSet::PixelFormatSet_t &rhs)
	{
		return ToString( lhs ^ rhs );
	}

#endif // AE_ENABLE_LOGS

/*
=================================================
	FS_Equal
=================================================
*/
	template <typename T>
	ND_ static bool  FS_Equal (const T &lhs, const T &rhs, const char*) __NE___ {
		return lhs == rhs;
	}

	ND_ static bool  FS_Equal (EFeature lhs, EFeature rhs, const char*) __NE___ {
		return	(lhs == rhs) or (lhs == EFeature::Ignore) or (rhs == EFeature::Ignore);
	}

	template <typename E>
	ND_ static bool  FS_Equal (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___ {
		return	lhs.include == rhs.include	and
				lhs.exclude == rhs.exclude;
	}

	ND_ static bool  FS_Equal (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___ {
		return	lhs.spirv == rhs.spirv		and
				lhs.metal == rhs.metal;
	}

	ND_ static bool  FS_Equal (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___ {
		return	lhs.minX		== rhs.minX		and
				lhs.minY		== rhs.minY		and
				lhs.maxX		== rhs.maxX		and
				lhs.maxY		== rhs.maxY		and
				lhs.aspectRatio	== rhs.aspectRatio;
	}

/*
=================================================
	FS_GreaterEqual
=================================================
*/
	ND_ static bool  FS_GreaterEqual (EFeature lhs, EFeature rhs, const char*) __NE___ {
		return	(lhs >= rhs) or (lhs == EFeature::Ignore) or (rhs == EFeature::Ignore);
	}

	ND_ static bool  FS_GreaterEqual (ESubgroupTypes lhs, ESubgroupTypes rhs, const char*) __NE___ {
		return rhs == Default or AllBits( lhs, rhs );
	}

	ND_ static bool  FS_GreaterEqual (EShaderStages lhs, EShaderStages rhs, const char*) __NE___ {
		return rhs == Default or AllBits( lhs, rhs );
	}

	ND_ static bool  FS_GreaterEqual (const FeatureSet::Queues &lhs, const FeatureSet::Queues &rhs, const char*) __NE___ {
		return	rhs.required == Default or
				(AllBits( lhs.supported | rhs.supported, lhs.required | rhs.required ) and
				 AllBits( lhs.required, rhs.required ));
	}

	ND_ static bool  FS_GreaterEqual (ubyte lhs, ubyte rhs, const char*) __NE___ {
		return lhs >= rhs;
	}

	ND_ static bool  FS_GreaterEqual (ushort lhs, ushort rhs, const char*) __NE___ {
		return lhs >= rhs;
	}

	ND_ static bool  FS_GreaterEqual (uint lhs, uint rhs, const char*) __NE___ {
		return lhs >= rhs;
	}
	
	ND_ static bool  FS_GreaterEqual (Bytes32u lhs, Bytes32u rhs, const char*) __NE___ {
		return lhs >= rhs;
	}

	ND_ static bool  FS_GreaterEqual (POTValue lhs, POTValue rhs, const char*) __NE___ {
		return lhs >= rhs;
	}

	ND_ static bool  FS_GreaterEqual (POTBytes lhs, POTBytes rhs, const char*) __NE___ {
		return lhs >= rhs;
	}

	ND_ static bool  FS_GreaterEqual (FeatureSet::KiBytes lhs, FeatureSet::KiBytes rhs, const char*) __NE___ {
		return lhs >= rhs;
	}

	ND_ static bool  FS_GreaterEqual (float lhs, float rhs, const char*) __NE___
	{
		return lhs > rhs or BitEqual( lhs, rhs, EnabledBitCount(18) );
	}

	ND_ static bool  FS_GreaterEqual (const FeatureSet::SampleCountBits &lhs, const FeatureSet::SampleCountBits &rhs, const char*) __NE___ {
		return (uint(lhs) & uint(rhs)) == uint(rhs);
	}

	template <typename E>
	ND_ static bool  FS_GreaterEqual (const EnumSet<E> &lhs, const EnumSet<E> &rhs, const char*) __NE___ {
		return (lhs & rhs) == rhs;
	}

	ND_ static bool  FS_GreaterEqual (const FeatureSet::PerDescriptorSet &lhs, const FeatureSet::PerDescriptorSet &rhs, const char*) __NE___ {
		return lhs >= rhs;
	}

	template <typename E>
	ND_ static bool  FS_GreaterEqual (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___ {
		return	(lhs.include & rhs.include) == rhs.include	and
				(lhs.exclude & rhs.exclude) == rhs.exclude;
	}

	ND_ static bool  FS_GreaterEqual (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___ {
		return	lhs.spirv >= rhs.spirv	and
				lhs.metal >= rhs.metal;
	}

	ND_ static bool  FS_GreaterEqual (const FeatureSet::ShadingRateSet_t &lhs, const FeatureSet::ShadingRateSet_t &rhs, const char*) __NE___
	{
		if ( lhs.empty() and rhs.empty() )
			return true;

		const uint	size_mask	= uint(EShadingRate::_SizeMask);
		const uint	samp_mask	= uint(EShadingRate::_SamplesMask);
		auto		it1			= lhs.begin(), it2 = rhs.begin();

		for (;;)
		{
			if ( it2 == rhs.end() )	break;
			if ( it1 == lhs.end() )	break;

			if ( (uint(*it1) & size_mask) == (uint(*it2) & size_mask) )
			{
				if ( (uint(*it1) & samp_mask) < (uint(*it2) & samp_mask) )
					return false;

				++it1; ++it2;
			}
			else
			if ( (uint(*it1) & size_mask) < (uint(*it2) & size_mask) )
				++it1;
			else
			if ( (uint(*it1) & size_mask) > (uint(*it2) & size_mask) )
				++it2;
		}
		return true;
	}

	ND_ static bool  FS_GreaterEqual (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___
	{
		return	lhs.minX		>= rhs.minX		and
				lhs.minY		>= rhs.minY		and
				lhs.maxX		>= rhs.maxX		and
				lhs.maxY		>= rhs.maxY		and
				lhs.aspectRatio	== rhs.aspectRatio;
	}

/*
=================================================
	FS_IsCompatible
=================================================
*/
	ND_ static bool  FS_IsCompatible (EFeature lhs, EFeature rhs, const char*) __NE___ {
		return	(lhs == rhs) or (lhs == EFeature::Ignore) or (rhs == EFeature::Ignore);	// 'True' is not compatible with 'False'
	}

	template <typename T>
	ND_ static bool  FS_IsCompatible (const T& lhs, const T& rhs, const char* name) __NE___ {
		return FS_GreaterEqual( lhs, rhs, name );
	}

	ND_ static bool  FS_IsCompatible (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___
	{
		bool	result = true;
		if ( lhs.spirv != 0 and rhs.spirv != 0 )	result &= (lhs.spirv >= rhs.spirv);
		if ( lhs.metal != 0 and rhs.metal != 0 )	result &= (lhs.metal >= rhs.metal);
		return result;
	}

	template <typename E>
	ND_ static bool  FS_IsCompatible (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___
	{
		if ( lhs.None() or rhs.None() )
			return true;

		return	(lhs.include & rhs.include).Any()	and
				(lhs.include & rhs.exclude).None()	and
				(rhs.include & lhs.exclude).None();
	}

/*
=================================================
	FS_Set
=================================================
*/
	template <typename T>
	ND_ static T  FS_Set (EFeature feat) __NE___
	{
		if constexpr( IsSame< T, EFeature >)
			return feat;
		else
			return {};
	}

	template <typename T>
	ND_ static T  FS_Set (POTValue value) __NE___
	{
		if constexpr( IsSame< T, POTValue >)
			return value;
		else
			return {};
	}

	template <typename T>
	ND_ static T  FS_Set (POTBytes value) __NE___
	{
		if constexpr( IsSame< T, POTBytes >)
			return value;
		else
			return {};
	}

/*
=================================================
	FS_MergeMin
=================================================
*/
	ND_ static EFeature  FS_MergeMin (EFeature lhs, EFeature rhs, const char*) __NE___
	{
		if ( lhs == rhs )
			return lhs;

		if ( lhs == EFeature::RequireFalse or rhs == EFeature::RequireFalse )
			return EFeature::RequireFalse;

		return EFeature::Ignore;
	}

	template <typename E>
	ND_ static EnumSet<E>  FS_MergeMin (const EnumSet<E> &lhs, const EnumSet<E> &rhs, const char*) __NE___ {
		return lhs & rhs;
	}

	ND_ static ESubgroupTypes  FS_MergeMin (ESubgroupTypes lhs, ESubgroupTypes rhs, const char*) __NE___ {
		return lhs & rhs;
	}

	ND_ static EShaderStages  FS_MergeMin (EShaderStages lhs, EShaderStages rhs, const char*) __NE___ {
		return lhs & rhs;
	}

	ND_ static FeatureSet::Queues  FS_MergeMin (const FeatureSet::Queues &lhs, const FeatureSet::Queues &rhs, const char*) __NE___
	{
		FeatureSet::Queues	res;
		res.supported	= lhs.supported & rhs.supported;
		res.required	= (lhs.required & rhs.required) & res.supported;
		return res;
	}

	ND_ static ubyte  FS_MergeMin (ubyte lhs, ubyte rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}

	ND_ static ushort  FS_MergeMin (ushort lhs, ushort rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}

	ND_ static uint  FS_MergeMin (uint lhs, uint rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}
	
	ND_ static Bytes32u  FS_MergeMin (Bytes32u lhs, Bytes32u rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}

	ND_ static POTValue  FS_MergeMin (POTValue lhs, POTValue rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}

	ND_ static POTBytes  FS_MergeMin (POTBytes lhs, POTBytes rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}

	ND_ static FeatureSet::KiBytes  FS_MergeMin (FeatureSet::KiBytes lhs, FeatureSet::KiBytes rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}

	ND_ static float  FS_MergeMin (float lhs, float rhs, const char*) __NE___ {
		return Min( lhs, rhs );
	}

	ND_ static FeatureSet::ShadingRateSet_t  FS_MergeMin (const FeatureSet::ShadingRateSet_t &lhs, const FeatureSet::ShadingRateSet_t &rhs, const char*) __NE___
	{
		if ( lhs.empty() and rhs.empty() )
			return {};

		FeatureSet::ShadingRateSet_t	result;
		const uint						size_mask	= uint(EShadingRate::_SizeMask);
		const uint						samp_mask	= uint(EShadingRate::_SamplesMask);
		auto							it1			= lhs.begin(), it2 = rhs.begin();

		for (;;)
		{
			if ( it1 == lhs.end() )	break;
			if ( it2 == rhs.end() )	break;

			if ( (uint(*it1) & size_mask) == (uint(*it2) & size_mask) )
			{
				result.push_back(EShadingRate( (uint(*it1) & size_mask) | ((uint(*it1) & samp_mask) & (uint(*it2) & samp_mask)) ));
				++it1; ++it2;
				continue;
			}

			if ( (uint(*it1) & size_mask) < (uint(*it2) & size_mask) )
				++it1;
			else
			if ( (uint(*it1) & size_mask) > (uint(*it2) & size_mask) )
				++it2;
		}
		ASSERT( IsSorted( result.begin(), result.end() ));
		return result;
	}

	ND_ static FeatureSet::ShaderVersion  FS_MergeMin (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___
	{
		FeatureSet::ShaderVersion	res;
		if ( lhs.spirv != 0 and rhs.spirv != 0 )	res.spirv = Min( lhs.spirv, rhs.spirv );
		if ( lhs.metal != 0 and rhs.metal != 0 )	res.metal = Min( lhs.metal, rhs.metal );
		return res;
	}

	ND_ static FeatureSet::VRSTexelSize  FS_MergeMin (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___
	{
		if ( not IsIntersects( lhs.minX, lhs.maxX, rhs.minX, rhs.maxX ) or
			 not IsIntersects( lhs.minY, lhs.maxY, rhs.minY, rhs.maxY ))
			return {};

		FeatureSet::VRSTexelSize	res;
		res.minX		= Max( lhs.minX,		rhs.minX );
		res.minY		= Max( lhs.minY,		rhs.minY );
		res.maxX		= Min( lhs.maxX,		rhs.maxX );
		res.maxY		= Min( lhs.maxY,		rhs.maxY );
		res.aspectRatio	= Min( lhs.aspectRatio,	rhs.aspectRatio );
		return res;
	}

	ND_ static FeatureSet::SampleCountBits  FS_MergeMin (const FeatureSet::SampleCountBits &lhs, const FeatureSet::SampleCountBits &rhs, const char*) __NE___ {
		return FeatureSet::SampleCountBits{ uint(lhs) & uint(rhs) };
	}

	ND_ static FeatureSet::PerDescriptorSet  FS_MergeMin (const FeatureSet::PerDescriptorSet &lhs, const FeatureSet::PerDescriptorSet &rhs, const char*) __NE___
	{
		FeatureSet::PerDescriptorSet	res;
		res.maxInputAttachments	= FS_MergeMin( lhs.maxInputAttachments,	rhs.maxInputAttachments,"maxInputAttachments"	);
		res.maxSampledImages	= FS_MergeMin( lhs.maxSampledImages,	rhs.maxSampledImages,	"maxSampledImages"		);
		res.maxSamplers			= FS_MergeMin( lhs.maxSamplers,			rhs.maxSamplers,		"maxSamplers"			);
		res.maxStorageBuffers	= FS_MergeMin( lhs.maxStorageBuffers,	rhs.maxStorageBuffers,	"maxStorageBuffers"		);
		res.maxStorageImages	= FS_MergeMin( lhs.maxStorageImages,	rhs.maxStorageImages,	"maxStorageImages"		);
		res.maxUniformBuffers	= FS_MergeMin( lhs.maxUniformBuffers,	rhs.maxUniformBuffers,	"maxUniformBuffers"		);
		res.maxAccelStructures	= FS_MergeMin( lhs.maxAccelStructures,	rhs.maxAccelStructures,	"maxAccelStructures"	);
		res.maxTotalResources	= FS_MergeMin( lhs.maxTotalResources,	rhs.maxTotalResources,	"maxTotalResources"		);
		StaticAssert( sizeof(FeatureSet::PerDescriptorSet) == sizeof(uint)*8 );
		return res;
	}

	template <typename E>
	ND_ static FeatureSet::IncludeExcludeBits<E>  FS_MergeMin (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___
	{
		FeatureSet::IncludeExcludeBits<E>	res;
		res.include  = lhs.include & rhs.include;
		res.include &= ~(lhs.exclude | rhs.exclude);
		res.exclude  = lhs.exclude & rhs.exclude;
		res.exclude &= ~res.include;
		return res;
	}

/*
=================================================
	FS_MergeMax
=================================================
*/
	ND_ static EFeature  FS_MergeMax (EFeature lhs, EFeature rhs, const char*) __NE___
	{
		if ( lhs == EFeature::RequireTrue or rhs == EFeature::RequireTrue )
			return EFeature::RequireTrue;

		if ( lhs == EFeature::RequireFalse or rhs == EFeature::RequireFalse )
			return EFeature::RequireFalse;

		return EFeature::Ignore;
	}

	template <typename E>
	ND_ static EnumSet<E>  FS_MergeMax (const EnumSet<E> &lhs, const EnumSet<E> &rhs, const char*) __NE___ {
		return lhs | rhs;
	}

	ND_ static ESubgroupTypes  FS_MergeMax (ESubgroupTypes lhs, ESubgroupTypes rhs, const char*) __NE___ {
		return lhs | rhs;
	}

	ND_ static EShaderStages  FS_MergeMax (EShaderStages lhs, EShaderStages rhs, const char*) __NE___ {
		return lhs | rhs;
	}

	ND_ static FeatureSet::Queues  FS_MergeMax (const FeatureSet::Queues &lhs, const FeatureSet::Queues &rhs, const char*) __NE___
	{
		FeatureSet::Queues	res;
		res.supported	= lhs.supported | rhs.supported;
		res.required	= (lhs.required | rhs.required) & res.supported;
		return res;
	}

	ND_ static ubyte  FS_MergeMax (ubyte lhs, ubyte rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}

	ND_ static ushort  FS_MergeMax (ushort lhs, ushort rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}

	ND_ static uint  FS_MergeMax (uint lhs, uint rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}
	
	ND_ static Bytes32u  FS_MergeMax (Bytes32u lhs, Bytes32u rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}

	ND_ static POTValue  FS_MergeMax (POTValue lhs, POTValue rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}

	ND_ static POTBytes  FS_MergeMax (POTBytes lhs, POTBytes rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}

	ND_ static FeatureSet::KiBytes  FS_MergeMax (FeatureSet::KiBytes lhs, FeatureSet::KiBytes rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}

	ND_ static float  FS_MergeMax (float lhs, float rhs, const char*) __NE___ {
		return Max( lhs, rhs );
	}

	ND_ static FeatureSet::ShaderVersion  FS_MergeMax (const FeatureSet::ShaderVersion &lhs, const FeatureSet::ShaderVersion &rhs, const char*) __NE___
	{
		FeatureSet::ShaderVersion	res;
		res.spirv = Max( lhs.spirv, rhs.spirv );
		res.metal = Max( lhs.metal, rhs.metal );
		return res;
	}

	ND_ static FeatureSet::VRSTexelSize  FS_MergeMax (const FeatureSet::VRSTexelSize &lhs, const FeatureSet::VRSTexelSize &rhs, const char*) __NE___
	{
		FeatureSet::VRSTexelSize	res;
		res.minX		= Min( lhs.minX,		rhs.minX );
		res.minY		= Min( lhs.minY,		rhs.minY );
		res.maxX		= Max( lhs.maxX,		rhs.maxX );
		res.maxY		= Max( lhs.maxY,		rhs.maxY );
		res.aspectRatio	= Max( lhs.aspectRatio,	rhs.aspectRatio );
		return res;
	}

	ND_ static FeatureSet::ShadingRateSet_t  FS_MergeMax (const FeatureSet::ShadingRateSet_t &lhs, const FeatureSet::ShadingRateSet_t &rhs, const char*) __NE___
	{
		if ( lhs.empty() and rhs.empty() )
			return {};

		FeatureSet::ShadingRateSet_t	result;
		const uint						size_mask	= uint(EShadingRate::_SizeMask);
		const uint						samp_mask	= uint(EShadingRate::_SamplesMask);
		auto							it1			= lhs.begin(), it2 = rhs.begin();

		for (;;)
		{
			if ( it1 == lhs.end() )	break;
			if ( it2 == rhs.end() )	break;

			if ( (uint(*it1) & size_mask) == (uint(*it2) & size_mask) )
			{
				result.push_back(EShadingRate( uint(*it1) | (uint(*it2) & samp_mask) ));
				++it1; ++it2;
				continue;
			}

			if ( (uint(*it1) & size_mask) < (uint(*it2) & size_mask) )
				++it1;
			else
			if ( (uint(*it1) & size_mask) > (uint(*it2) & size_mask) )
				++it2;
		}
		ASSERT( IsSorted( result.begin(), result.end() ));
		return result;
	}

	ND_ static FeatureSet::SampleCountBits  FS_MergeMax (const FeatureSet::SampleCountBits &lhs, const FeatureSet::SampleCountBits &rhs, const char*) __NE___ {
		return FeatureSet::SampleCountBits{ uint(lhs) | uint(rhs) };
	}

	ND_ static FeatureSet::PerDescriptorSet  FS_MergeMax (const FeatureSet::PerDescriptorSet &lhs, const FeatureSet::PerDescriptorSet &rhs, const char*) __NE___
	{
		FeatureSet::PerDescriptorSet	res;
		res.maxInputAttachments	= FS_MergeMax( lhs.maxInputAttachments,	rhs.maxInputAttachments,	"maxInputAttachments"	);
		res.maxSampledImages	= FS_MergeMax( lhs.maxSampledImages,	rhs.maxSampledImages,		"maxSampledImages"		);
		res.maxSamplers			= FS_MergeMax( lhs.maxSamplers,			rhs.maxSamplers,			"maxSamplers"			);
		res.maxStorageBuffers	= FS_MergeMax( lhs.maxStorageBuffers,	rhs.maxStorageBuffers,		"maxStorageBuffers"		);
		res.maxStorageImages	= FS_MergeMax( lhs.maxStorageImages,	rhs.maxStorageImages,		"maxStorageImages"		);
		res.maxUniformBuffers	= FS_MergeMax( lhs.maxUniformBuffers,	rhs.maxUniformBuffers,		"maxUniformBuffers"		);
		res.maxAccelStructures	= FS_MergeMax( lhs.maxAccelStructures,	rhs.maxAccelStructures,		"maxAccelStructures"	);
		res.maxTotalResources	= FS_MergeMax( lhs.maxTotalResources,	rhs.maxTotalResources,		"maxTotalResources"		);
		return res;
	}

	template <typename E>
	ND_ static FeatureSet::IncludeExcludeBits<E>  FS_MergeMax (const FeatureSet::IncludeExcludeBits<E> &lhs, const FeatureSet::IncludeExcludeBits<E> &rhs, const char*) __NE___
	{
		FeatureSet::IncludeExcludeBits<E>	res;
		res.include	= lhs.include | rhs.include;
		res.exclude = lhs.exclude & rhs.exclude;
		res.exclude &= ~res.include;
		return res;
	}

/*
=================================================
	FS_Validate***
=================================================
*/
	template <typename T1, typename T2>
	ND_ static T1  FS_ValidateEq (const T1, const T2 rhs) __NE___
	{
		return rhs;
	}

	template <bool Mutable, typename T1, typename T2, typename T3>
	static void  FS_ValidateNotEq (T1 &lhs, const T2 &rhs, const T3 &) __NE___
	{
		if constexpr( Mutable )
		{
			lhs = rhs;
		}
		else
		{
			CHECK_ERRV( lhs != rhs );
		}
	}

} // namespace

/*
=================================================
	operator ==
=================================================
*/
	bool  FeatureSet::PerDescriptorSet::operator == (const PerDescriptorSet &rhs) C_NE___
	{
		return	maxInputAttachments	== rhs.maxInputAttachments	and
				maxSampledImages	== rhs.maxSampledImages		and
				maxSamplers			== rhs.maxSamplers			and
				maxStorageBuffers	== rhs.maxStorageBuffers	and
				maxStorageImages	== rhs.maxStorageImages		and
				maxUniformBuffers	== rhs.maxUniformBuffers	and
				maxAccelStructures	== rhs.maxAccelStructures	and
				maxTotalResources	== rhs.maxTotalResources;
	}

/*
=================================================
	operator >=
=================================================
*/
	bool  FeatureSet::PerDescriptorSet::operator >= (const PerDescriptorSet &rhs) C_NE___
	{
		return	maxInputAttachments	>= rhs.maxInputAttachments	and
				maxSampledImages	>= rhs.maxSampledImages		and
				maxSamplers			>= rhs.maxSamplers			and
				maxStorageBuffers	>= rhs.maxStorageBuffers	and
				maxStorageImages	>= rhs.maxStorageImages		and
				maxUniformBuffers	>= rhs.maxUniformBuffers	and
				maxAccelStructures	>= rhs.maxAccelStructures	and
				maxTotalResources	>= rhs.maxTotalResources;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  FeatureSet::PerDescriptorSet::CalcHash () C_NE___
	{
		return	HashOf( maxInputAttachments ) + HashOf( maxSampledImages ) + HashOf( maxSamplers ) +
				HashOf( maxStorageBuffers ) + HashOf( maxStorageImages ) + HashOf( maxUniformBuffers ) +
				HashOf( maxTotalResources );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Validate
=================================================
*/
	template <bool Mutable>
	bool  FeatureSet::_Validate () __NE___
	{
		#define chEqual2( _lhs_, _ref_, _op_ )						\
			{using A = MajType< decltype(_lhs_), decltype(_ref_)>;	\
			 if constexpr( Mutable ){								\
				if ( not (A(_lhs_) == A(_ref_)) )					\
					(_op_);											\
			}else{													\
				CHECK_ERR( A(_lhs_) == A(_ref_) );					\
			}}

		#define chNotEqual2( _lhs_, _ref_, _valid_ )				\
			{using A = MajType< decltype(_lhs_), decltype(_ref_)>;	\
			 if constexpr( Mutable ){								\
				if ( not (A(_lhs_) != A(_ref_)) )					\
					(_lhs_) = (_valid_);							\
			}else{													\
				CHECK_ERR( A(_lhs_) != A(_ref_) );					\
			}}

		#define chLessEq2( _lhs_, _ref_, _valid_ )					\
			{using A = MajType< decltype(_lhs_), decltype(_ref_)>;	\
			 if constexpr( Mutable ){								\
				if ( not (A(_lhs_) <= A(_ref_)) )					\
					(_lhs_) = (_valid_);							\
			}else{													\
				CHECK_ERR( A(_lhs_) <= A(_ref_) );					\
			}}

		#define chGreaterEq2( _lhs_, _ref_, _valid_ )				\
			{using A = MajType< decltype(_lhs_), decltype(_ref_)>;	\
			 if constexpr( Mutable ){								\
				if ( not (A(_lhs_) >= A(_ref_)) )					\
					(_lhs_) = (_valid_);							\
			}else{													\
				CHECK_ERR( A(_lhs_) >= A(_ref_) );					\
			}}

		#define chEqual(     _lhs_, _ref_ )		chEqual2(     (_lhs_), (_ref_), (_lhs_) = (_ref_) )		// if lhs != ref then lhs = ref
		#define chNotEqual(  _lhs_, _ref_ )		chNotEqual2(  (_lhs_), (_ref_), (_ref_) )				// TODO: check
		#define chLessEq(    _lhs_, _ref_ )		chLessEq2(    (_lhs_), (_ref_), (_ref_) )				// if lhs > ref then lhs = ref
		#define chGreaterEq( _lhs_, _ref_ )		chGreaterEq2( (_lhs_), (_ref_), (_ref_) )				// if lhs < ref then lhs = ref

		#define fEqual( _lhs_, _rhs_ )			(EFeature(_lhs_) == (_rhs_))
		#define fNotEq( _lhs_, _rhs_ )			(EFeature(_lhs_) != (_rhs_))

		const auto	True			= EFeature::RequireTrue;
		const auto	neg_feat		= EFeature::Ignore;
		const auto	invalid_pot		= POTValue::Invalid();
		const auto	invalid_potb	= POTBytes::Invalid();

		EShaderStages	all_stages = EShaderStages::All;
		if ( fNotEq( computeShader,		 True ))	all_stages &= ~EShaderStages::Compute;
		if ( fNotEq( geometryShader,	 True ))	all_stages &= ~EShaderStages::Geometry;
		if ( fNotEq( tessellationShader, True ))	all_stages &= ~(EShaderStages::TessControl | EShaderStages::TessEvaluation);
		if ( fNotEq( tileShader,		 True ))	all_stages &= ~EShaderStages::Tile;
		if ( fNotEq( taskShader,		 True ))	all_stages &= ~EShaderStages::MeshTask;
		if ( fNotEq( meshShader,		 True ))	all_stages &= ~EShaderStages::Mesh;
		if ( fNotEq( rayTracingPipeline, True ))	all_stages &= ~EShaderStages::AllRayTracing;

		if ( fEqual( subgroup, True ))
		{
			chNotEqual2( subgroupOperations, SubgroupOperationBits{},	SubgroupOperationBits{}.InsertRange( ESubgroupOperation::_Basic_Begin, ESubgroupOperation::_Basic_End ));
			chNotEqual2( subgroupTypes,		 ESubgroupTypes::Unknown,	ESubgroupTypes::Float32 );
			chNotEqual2( subgroupStages,	 EShaderStages::Unknown,	EShaderStages::Fragment | EShaderStages::Compute);

			chEqual2(	 AllBits( subgroupTypes,  ESubgroupTypes::Float32 ),	true,	subgroupTypes  |= ESubgroupTypes::Float32 );
			chEqual2(	 AllBits( subgroupStages, EShaderStages::Compute ),		true,	subgroupStages |= EShaderStages::Compute  );
			chEqual2(	 AnyBits( subgroupStages, ~all_stages ),				false,	subgroupStages &= all_stages  );

			if ( fNotEq( shaderInt8,	True ))	chEqual2(	 AllBits( subgroupTypes, ESubgroupTypes::Int8 ),	false,	subgroupTypes &= ~ESubgroupTypes::Int8 );
			if ( fNotEq( shaderInt16,	True ))	chEqual2(	 AllBits( subgroupTypes, ESubgroupTypes::Int16 ),	false,	subgroupTypes &= ~ESubgroupTypes::Int16 );
			if ( fNotEq( shaderInt64,	True ))	chEqual2(	 AllBits( subgroupTypes, ESubgroupTypes::Int64 ),	false,	subgroupTypes &= ~ESubgroupTypes::Int64 );
			if ( fNotEq( shaderFloat16,	True ))	chEqual2(	 AllBits( subgroupTypes, ESubgroupTypes::Float16 ),	false,	subgroupTypes &= ~ESubgroupTypes::Float16 );

			chGreaterEq( minSubgroupSize,	POTValue_From<1> );
			chGreaterEq( maxSubgroupSize,	minSubgroupSize );

			if ( subgroupOperations.AnyInRange( ESubgroupOperation::_Quad_Begin, ESubgroupOperation::_Quad_End ))
			{
				chNotEqual2( subgroupQuadStages,							EShaderStages::Unknown,	EShaderStages::Fragment | EShaderStages::Compute );
				chEqual2(	 AnyBits( subgroupQuadStages, ~all_stages ),	false,					subgroupQuadStages &= all_stages				 );
			}
		}
		else
		{
			chEqual( subgroupOperations,	SubgroupOperationBits{} );
			chEqual( subgroupTypes,			ESubgroupTypes::Unknown );
			chEqual( subgroupStages,		EShaderStages::Unknown );
			chEqual( subgroupQuadStages,	EShaderStages::Unknown );
			chEqual( minSubgroupSize,		invalid_pot );
			chEqual( maxSubgroupSize,		invalid_pot );
		}

		if ( fEqual( subgroupSizeControl, True ))
		{
			chEqual( subgroup, True );
			// requiredSubgroupSizeStages can be 0
			chEqual2( AnyBits( requiredSubgroupSizeStages, ~all_stages ), false, requiredSubgroupSizeStages &= all_stages );
		}
		else
		{
			chEqual( maxSubgroupSize,				minSubgroupSize );
			chEqual( requiredSubgroupSizeStages,	EShaderStages::Unknown );
		}

		if ( fEqual( attachmentFragmentShadingRate, True )) {
			if constexpr( Mutable ) {
				if (not fragmentShadingRateTexelSize)
					attachmentFragmentShadingRate = EFeature::Ignore;
			}else
				CHECK_ERR( fragmentShadingRateTexelSize );
		}else{
			if constexpr( Mutable )
				fragmentShadingRateTexelSize = VRSTexelSize{};
			else
				CHECK_ERR( not fragmentShadingRateTexelSize );
		}

		if ( fEqual( pipelineFragmentShadingRate,   True ) or
			 fEqual( primitiveFragmentShadingRate,  True ) or
			 fEqual( attachmentFragmentShadingRate, True ))
		{
			if constexpr( Mutable ){
				if ( fragmentShadingRates.empty() )
					fragmentShadingRates.push_back( EShadingRate::Size1x1 | EShadingRate::Samples1 );
			}else{
				CHECK_ERR( not fragmentShadingRates.empty() );
			}
		}
		else
		{
			chNotEqual2( primitiveFragmentShadingRateWithMultipleViewports,	True, neg_feat );
			chNotEqual2( layeredShadingRateAttachments,						True, neg_feat );
			chNotEqual2( fragmentShadingRateWithShaderDepthStencilWrites,	True, neg_feat );
			chNotEqual2( fragmentShadingRateWithSampleMask,					True, neg_feat );
			chNotEqual2( fragmentShadingRateWithShaderSampleMask,			True, neg_feat );
			chNotEqual2( fragmentShadingRateWithFragmentShaderInterlock,	True, neg_feat );
			chNotEqual2( fragmentShadingRateWithCustomSampleLocations,		True, neg_feat );
		}
		
		if ( fEqual( fragmentDensityMap, EFeature::RequireTrue ))
		{
			chGreaterEq( maxSubsampledArrayLayers,				POTValue_From<1> );
			chGreaterEq( perPipeline_maxSubsampledSamplers,		1 );
		}
		else
		{
			chNotEqual2( fragmentDensityMapDynamic,				EFeature::RequireTrue, neg_feat );
			chNotEqual2( fragmentDensityMapNonSubsampledImages,	EFeature::RequireTrue, neg_feat );
			chNotEqual2( fragmentDensityInvocations,			EFeature::RequireTrue, neg_feat );
			chNotEqual2( subsampledLoads,						EFeature::RequireTrue, neg_feat );
			chEqual(	 maxSubsampledArrayLayers,				invalid_pot );
			chEqual(	 perPipeline_maxSubsampledSamplers,		0 );
		}

		if ( fNotEq( bufferDeviceAddress, True ))
		{
			if constexpr( Mutable ){
				rayTracingPipeline	= neg_feat;
				rayQuery			= neg_feat;
			}
		}

		if ( fEqual( rayTracingPipeline, True ))
		{
			chGreaterEq( maxShaderVersion.spirv, 140 );
			if constexpr( not Mutable ) { CHECK( fEqual( bufferDeviceAddress, True )); }
		}
		else
		{
			chNotEqual2( rayTraversalPrimitiveCulling,	True, neg_feat );
			chEqual(	 maxRayRecursionDepth,			0 );
		}

		if ( fEqual( rayQuery, True ))
		{
			chNotEqual2( rayQueryStages,							EShaderStages::Unknown,	EShaderStages::Fragment | EShaderStages::Compute );
			chEqual2(	 AnyBits( rayQueryStages, ~all_stages ),	false,					rayQueryStages &= all_stages					 );
			if ( maxShaderVersion.spirv != 0 ) { chGreaterEq( maxShaderVersion.spirv, 140 ); }
			if ( maxShaderVersion.metal != 0 ) { chGreaterEq( maxShaderVersion.metal, 230 ); }
			if constexpr( not Mutable ) { CHECK( fNotEq( bufferDeviceAddress, EFeature::RequireFalse )); }
		}
		else
		{
			chEqual( rayQueryStages, EShaderStages::Unknown );
		}

		if ( fEqual( rayTracingPipeline, True ) or
			 fEqual( rayQuery, True ))
		{
			chNotEqual2( accelStructVertexFormats,	VertexFormatSet_t{},	VertexFormatSet_t{}.insert( EVertexType::Float3 ));
		}
		else
		{
			chEqual( accelStructVertexFormats,	VertexFormatSet_t{} );
		}

		if ( maxShaderVersion.spirv != 0 )
		{
			chGreaterEq( maxShaderVersion.spirv, MinSpirvVersion );
			chLessEq(	 maxShaderVersion.spirv, MaxSpirvVersion );
		}
		if ( maxShaderVersion.metal != 0 )
		{
			chGreaterEq( maxShaderVersion.metal, MinMetalVersion );
			chLessEq(	 maxShaderVersion.metal, MaxMetalVersion );
		}

		if ( fEqual( multiview, True ))
		{
			chGreaterEq( maxMultiviewViewCount, 1 );
		}
		else
		{
			chNotEqual2( multiviewGeometryShader,		True, neg_feat );
			chNotEqual2( multiviewTessellationShader,	True, neg_feat );
			chLessEq(	 maxMultiviewViewCount,			1 );
		}

		if ( fNotEq( multiViewport, True ))
		{
			chEqual( maxViewports, 1 );
		}
		chGreaterEq( maxViewports, 1 );

		chGreaterEq( maxUniformBufferSize,	16_KiB );
		chGreaterEq( maxStorageBufferSize,	0_b );

		chGreaterEq( perPipeline.maxUniformBuffers, 1 );
		chGreaterEq( perPipeline.maxSampledImages,  1 );
		chGreaterEq( perPipeline.maxTotalResources, 1 );
		chGreaterEq( perStage.maxUniformBuffers, 1 );
		chGreaterEq( perStage.maxSampledImages,  1 );
		chGreaterEq( perStage.maxTotalResources, 1 );

		chGreaterEq( maxDescriptorSets, 1 );
		// maxTexelOffset, maxTexelGatherOffset can be 0
		chGreaterEq( maxFragmentOutputAttachments,		 1 );
		chGreaterEq( maxFragmentCombinedOutputResources, 1 );
		// maxFragmentDualSrcAttachments can be 0
		chGreaterEq( maxPushConstantsSize, POTBytes_From<16> );

		if ( fEqual( computeShader, True ))
		{
			chGreaterEq( maxComputeSharedMemorySize,	 KiBytes{1<<10} );
			chGreaterEq( maxComputeWorkGroupInvocations, POTValue_From<1>);
			chGreaterEq( maxComputeWorkGroupSizeX,		 POTValue_From<1> );
			chLessEq(	 maxComputeWorkGroupSizeX,		 maxComputeWorkGroupInvocations );
			chGreaterEq( maxComputeWorkGroupSizeY,		 POTValue_From<1> );
			chLessEq(	 maxComputeWorkGroupSizeY,		 maxComputeWorkGroupInvocations );
			chGreaterEq( maxComputeWorkGroupSizeZ,		 POTValue_From<1> );
			chLessEq(	 maxComputeWorkGroupSizeZ,		 maxComputeWorkGroupInvocations );

			chNotEqual2( storageImageFormats, PixelFormatSet_t{},  PixelFormatSet_t{}.insert( EPixelFormat::R32I ));
			//CHECK_ERR( storageImageAtomicFormats.Any() );	// can be empty
		}
		else
		{
			chEqual( maxComputeSharedMemorySize,	 KiBytes{} );
			chEqual( maxComputeWorkGroupInvocations, invalid_pot );
			chEqual( maxComputeWorkGroupSizeX,		 invalid_pot );
			chEqual( maxComputeWorkGroupSizeY,		 invalid_pot );
			chEqual( maxComputeWorkGroupSizeZ,		 invalid_pot );
		}

		if ( fEqual( taskShader, True ))
		{
			chGreaterEq( maxTaskWorkGroupSize,					POTValue_From<32>	);
			chGreaterEq( maxTaskPayloadSize,					KiBytes{ 16<<10 }	);
			chGreaterEq( maxTaskSharedMemorySize,				KiBytes{ 16<<10 }	);
			chGreaterEq( maxTaskPayloadAndSharedMemorySize,		KiBytes{ 16<<10 }	);
			chGreaterEq( maxPreferredTaskWorkGroupInvocations,	POTValue_From<16>	);
			chEqual(	 meshShader,							True				);
			if ( maxShaderVersion.spirv != 0 ) { chGreaterEq( maxShaderVersion.spirv, 140 ); }
			if ( maxShaderVersion.metal != 0 ) { chGreaterEq( maxShaderVersion.metal, 300 ); }
		}
		else
		{
			chEqual( maxTaskWorkGroupSize,					invalid_pot );
			chEqual( maxTaskPayloadSize,					KiBytes{} );
			chEqual( maxTaskSharedMemorySize,				KiBytes{} );
			chEqual( maxTaskPayloadAndSharedMemorySize,		KiBytes{} );
			chEqual( maxPreferredTaskWorkGroupInvocations,	invalid_pot );
		}

		if ( fEqual( meshShader, True ))
		{
			chGreaterEq( maxMeshWorkGroupSize,					POTValue_From<32>	);
			chGreaterEq( maxMeshOutputVertices,					POTValue_From<128>	);
			chGreaterEq( maxMeshOutputPrimitives,				POTValue_From<128>	);
			chGreaterEq( maxMeshOutputPerVertexGranularity,		POTValue_From<1>	);
			chGreaterEq( maxMeshOutputPerPrimitiveGranularity,	POTValue_From<1>	);
			chGreaterEq( maxMeshSharedMemorySize,				KiBytes{ 16<<10 }	);
			chGreaterEq( maxMeshPayloadAndSharedMemorySize,		KiBytes{ 16<<10 }	);
			chGreaterEq( maxMeshOutputMemorySize,				KiBytes{ 16<<10 }	);
			chGreaterEq( maxMeshPayloadAndOutputMemorySize,		KiBytes{ 16<<10 }	);
			chGreaterEq( maxPreferredMeshWorkGroupInvocations,	POTValue_From<16>	);
			// maxMeshMultiviewViewCount can be 0
			if ( maxShaderVersion.spirv != 0 ) { chGreaterEq( maxShaderVersion.spirv, 140 ); }
			if ( maxShaderVersion.metal != 0 ) { chGreaterEq( maxShaderVersion.metal, 300 ); }
		}
		else
		{
			chEqual( maxMeshWorkGroupSize,					invalid_pot );
			chEqual( maxMeshOutputVertices,					invalid_pot );
			chEqual( maxMeshOutputPrimitives,				invalid_pot );
			chEqual( maxMeshOutputPerVertexGranularity,		invalid_pot );
			chEqual( maxMeshOutputPerPrimitiveGranularity,	invalid_pot );
			chEqual( maxMeshSharedMemorySize,				KiBytes{} );
			chEqual( maxMeshPayloadAndSharedMemorySize,		KiBytes{} );
			chEqual( maxMeshOutputMemorySize,				KiBytes{} );
			chEqual( maxMeshPayloadAndOutputMemorySize,		KiBytes{} );
			chEqual( maxPreferredMeshWorkGroupInvocations,	invalid_pot );
			chEqual( maxMeshMultiviewViewCount,				0 );
		}

		chGreaterEq( maxImageDimension1D,	POTValue_From<1> );
		chGreaterEq( maxImageDimension2D,	POTValue_From<1> );
		chGreaterEq( maxImageDimension3D,	POTValue_From<1> );
		chGreaterEq( maxImageDimensionCube,	POTValue_From<1> );
		chGreaterEq( maxImageArrayLayers,	POTValue_From<1> );
		chGreaterEq( maxFramebufferLayers,	POTValue_From<1> );

		// must be at least one depth format
		/*if ( attachmentFormats.Any() )
		{
			CHECK_ERR(	attachmentFormats.contains( EPixelFormat::Depth16 )				or
						attachmentFormats.contains( EPixelFormat::Depth16_Stencil8 )	or
						attachmentFormats.contains( EPixelFormat::Depth24 )				or
						attachmentFormats.contains( EPixelFormat::Depth24_Stencil8 )	or
						attachmentFormats.contains( EPixelFormat::Depth32F )			or
						attachmentFormats.contains( EPixelFormat::Depth32F_Stencil8 ));
		}*/

		if ( fEqual( dualSrcBlend, True ) or fEqual( independentBlend, True ) or fEqual( constantAlphaColorBlendFactors, True ))
			CHECK_ERR( attachmentBlendFormats.Any() );

		if ( attachmentBlendFormats.Any() )
		{
			CHECK_ERR( attachmentFormats.Any() );
			CHECK_ERR( (attachmentFormats & attachmentBlendFormats) == attachmentBlendFormats );
		}

		CHECK_ERR( (VertexTypeBits & vertexFormats) == vertexFormats );

		if ( fNotEq( vulkanMemoryModel, True ))
		{
			chNotEqual2( vulkanMemoryModelDeviceScope,					True, neg_feat );
			chNotEqual2( vulkanMemoryModelAvailabilityVisibilityChains,	True, neg_feat );
		}

		if ( fEqual( variableSampleLocations, True )) {
			 chEqual( sampleLocations, True );
		}
		if ( fEqual( shaderSampleRateInterpolationFunctions, True )) {
			 chEqual( sampleRateShading, True );
		}
		if ( maxShaderVersion.metal != 0 and maxShaderVersion.metal < 230 ) {
			chNotEqual( shaderInt64, True );
		}
		// not supported in Metal
		if ( maxShaderVersion.spirv == 0 and maxShaderVersion.metal != 0 )
		{
			chNotEqual( shaderFloat64,			True );
			chNotEqual( shaderSubgroupClock,	True );
			chNotEqual( shaderDeviceClock,		True );
			chNotEqual( rayTracingPipeline,		True );
			// TODO
		}

		if ( fEqual( cooperativeMatrix, True )) {
			chNotEqual2( cooperativeMatrixStages, EShaderStages::Unknown, EShaderStages::Compute );
		}else{
			chEqual( cooperativeMatrixStages, EShaderStages::Unknown );
		}
		
		if ( fEqual( cooperativeVectorTraining, True )) {
			chEqual( cooperativeVector, True );
		}

		if ( fEqual( externalFormatAndroid, True )) {
			 chEqual( samplerYcbcrConversion, True );
		}

		chNotEqual2( queues.supported, EQueueMask::Unknown, EQueueMask::Graphics );
		chEqual2(	 AnyBits( queues.required, ~queues.supported ),  false,  queues.required &= queues.supported );

		if ( fEqual( shaderUniformBufferArrayNonUniformIndexingNative, True ))
			chEqual( shaderUniformBufferArrayNonUniformIndexing, True );

		if ( fEqual( shaderSampledImageArrayNonUniformIndexingNative, True ))
			chEqual( shaderSampledImageArrayNonUniformIndexing, True );

		if ( fEqual( shaderStorageBufferArrayNonUniformIndexingNative, True ))
			chEqual( shaderStorageBufferArrayNonUniformIndexing, True );

		if ( fEqual( shaderStorageImageArrayNonUniformIndexingNative, True ))
			chEqual( shaderStorageImageArrayNonUniformIndexing, True );

		if ( fEqual( shaderInputAttachmentArrayNonUniformIndexingNative, True ))
			chEqual( shaderInputAttachmentArrayNonUniformIndexing, True );

		chGreaterEq( maxDrawIndirectCount, 1 );

		return true;

	#undef chEqual
	#undef chNotEqual
	#undef chLessEq
	#undef chGreaterEq
	}

	bool  FeatureSet::IsValid ()	C_NE___	{ return ConstCast<FeatureSet>(this)->_Validate<false>(); }
	void  FeatureSet::Validate ()	__NE___	{ _Validate<true>(); }

/*
=================================================
	IsSupported (RenderState)
=================================================
*/
	bool  FeatureSet::IsSupported (const RenderState &rs) C_NE___
	{
		const auto	True = EFeature::RequireTrue;

		if ( rs.multisample.alphaToOne )
			CHECK_ERR( alphaToOne == True );

		if ( rs.multisample.sampleShading )
			CHECK_ERR( sampleRateShading == True );

		if ( rs.rasterization.depthBiasClamp != 0.f )
			CHECK_ERR( depthBiasClamp == True );

		if ( rs.rasterization.depthClamp )
			CHECK_ERR( depthClamp == True );

		if ( rs.rasterization.polygonMode != EPolygonMode::Fill )
			CHECK_ERR( fillModeNonSolid == True );

		if ( rs.rasterization.polygonMode == EPolygonMode::Point )
			CHECK_ERR( pointPolygons == True );

		if ( rs.depth.bounds )
			CHECK_ERR( depthBounds == True );
		
		if ( (rs.rasterization.polygonMode == EPolygonMode::Line or
			  (rs.inputAssembly.topology >= EPrimitive::LineList and rs.inputAssembly.topology >= EPrimitive::LineStripAdjacency)) and
			 rs.rasterization.lineWidth > 1 )
			CHECK_ERR( wideLines == True );

		const auto	CheckBlend = [dual_src		= dualSrcBlend == True,
								  const_alpha	= constantAlphaColorBlendFactors == True] (EBlendFactor factor) -> bool
		{{
			switch_enum( factor )
			{
				case EBlendFactor::Zero :
				case EBlendFactor::One :
				case EBlendFactor::SrcColor :
				case EBlendFactor::OneMinusSrcColor :
				case EBlendFactor::DstColor :
				case EBlendFactor::OneMinusDstColor :
				case EBlendFactor::SrcAlpha :
				case EBlendFactor::OneMinusSrcAlpha :
				case EBlendFactor::DstAlpha :
				case EBlendFactor::OneMinusDstAlpha :
				case EBlendFactor::ConstColor :
				case EBlendFactor::OneMinusConstColor :
				case EBlendFactor::SrcAlphaSaturate :
					return true;

				case EBlendFactor::ConstAlpha :
				case EBlendFactor::OneMinusConstAlpha :
					return const_alpha;

				case EBlendFactor::Src1Color :
				case EBlendFactor::OneMinusSrc1Color :
				case EBlendFactor::Src1Alpha :
				case EBlendFactor::OneMinusSrc1Alpha :
					return dual_src;

				case EBlendFactor::Unknown :
				default_unlikely :
					return false;
			}
			switch_end
		}};

		for (auto& cb : rs.color.buffers)
		{
			if ( cb.blend )
			{
				CHECK_ERR( CheckBlend( cb.srcBlendFactor.color ));
				CHECK_ERR( CheckBlend( cb.srcBlendFactor.alpha ));
				CHECK_ERR( CheckBlend( cb.dstBlendFactor.color ));
				CHECK_ERR( CheckBlend( cb.dstBlendFactor.alpha ));

			//	if ( cb.srcBlendFactor.color != cb.srcBlendFactor.alpha )	// TODO: 'independentBlend' is not a separate blend
			//		CHECK_ERR( independentBlend == True );

			//	if ( cb.dstBlendFactor.color != cb.dstBlendFactor.alpha )
			//		CHECK_ERR( independentBlend == True );
			}
		}

		if ( independentBlend != True and rs.color.buffers.size() > 0 )
		{
			const auto&	ref = rs.color.buffers[0];
			for (usize i = 1; i < rs.color.buffers.size(); ++i)
			{
				auto&	cb = rs.color.buffers[i];
				CHECK_ERR( ref == cb );
			}
		}

		if ( rs.color.logicOp != ELogicOp::None )
			CHECK_ERR( logicOp == True );

		if ( rs.inputAssembly.topology == EPrimitive::TriangleFan )
			CHECK_ERR( triangleFans == True );
		
		if ( rs.rasterOrderAccess.color )
			CHECK_ERR( rasterizationOrderColorAttachmentAccess == True );
			
		if ( rs.rasterOrderAccess.depth )
			CHECK_ERR( rasterizationOrderDepthAttachmentAccess == True );
			
		if ( rs.rasterOrderAccess.stencil )
			CHECK_ERR( rasterizationOrderStencilAttachmentAccess == True );

		return true;
	}

/*
=================================================
	IsSupported (BufferDesc)
=================================================
*/
	bool  FeatureSet::IsSupported (const BufferDesc &desc) C_NE___
	{
		StaticAssert( uint(EBufferOpt::All) == 0x1F );

		bool	result = true;

		result &= (desc.queues == Default) or AllBits( queues.supported, desc.queues );

		for (auto opt : BitfieldIterate( desc.options ))
		{
			switch_enum( opt )
			{
				case EBufferOpt::SparseResidency :			break;
				case EBufferOpt::SparseAliased :			break;
				case EBufferOpt::FragmentPplnStore :		result &= (fragmentStoresAndAtomics			== EFeature::RequireTrue);	break;
				case EBufferOpt::VertexPplnStore :			result &= (vertexPipelineStoresAndAtomics	== EFeature::RequireTrue);	break;
				case EBufferOpt::StorageTexelAtomic :		break;

				case EBufferOpt::_Last :
				case EBufferOpt::SparseResidencyAliased :
				case EBufferOpt::All :
				case EBufferOpt::Unknown :
				default_unlikely :							DBG_WARNING( "unknown buffer option" ); break;
			}
			switch_end
		}

		return result;
	}

/*
=================================================
	IsSupported (BufferDesc, BufferViewDesc)
=================================================
*/
	bool  FeatureSet::IsSupported (const BufferDesc &desc, const BufferViewDesc &view) C_NE___
	{
		StaticAssert( uint(EBufferUsage::All) == 0x3FFF );
		StaticAssert( uint(EBufferOpt::All) == 0x1F );

		bool	result = true;

		if ( AllBits( desc.usage, EBufferUsage::UniformTexel ))
			result &= uniformTexBufferFormats.contains( view.format );

		if ( AllBits( desc.usage, EBufferUsage::StorageTexel ))
			result &= storageTexBufferFormats.contains( view.format );

		if ( AllBits( desc.options, EBufferOpt::StorageTexelAtomic ))
			result &= storageTexBufferAtomicFormats.contains( view.format );

		return result;
	}

/*
=================================================
	IsSupported (ImageDesc)
=================================================
*/
	bool  FeatureSet::IsSupported (const ImageDesc &desc) C_NE___
	{
		bool	result = true;

		if ( (desc.format >= EPixelFormat::ASTC_RGBA8_4x4 and desc.format <= EPixelFormat::ASTC_RGBA8_12x12) or
			 (desc.format >= EPixelFormat::ASTC_sRGB8_A8_4x4 and desc.format <= EPixelFormat::ASTC_sRGB8_A8_12x12) )
			result &= (textureCompressionASTC_LDR == EFeature::RequireTrue);

		if ( desc.format >= EPixelFormat::ASTC_RGBA16F_4x4 and desc.format <= EPixelFormat::ASTC_RGBA16F_12x12 )
			result &= (textureCompressionASTC_HDR == EFeature::RequireTrue);

		if ( desc.format >= EPixelFormat::BC1_RGB8_UNorm and desc.format <= EPixelFormat::BC7_sRGB8_A8 )
			result &= (textureCompressionBC == EFeature::RequireTrue);

		if ( (desc.format >= EPixelFormat::ETC2_RGB8_UNorm and desc.format <= EPixelFormat::ETC2_sRGB8_A8) or
			 (desc.format >= EPixelFormat::EAC_R11_SNorm and desc.format <= EPixelFormat::EAC_RG11_UNorm) )
			result &= (textureCompressionETC2 == EFeature::RequireTrue);

		switch ( desc.imageDim )
		{
			case EImageDim_1D :		result &= (desc.dimension.x <= maxImageDimension1D);				break;
			case EImageDim_2D :		result &= All( desc.Dimension2() <= uint2{maxImageDimension2D} );	break;
			case EImageDim_3D :		result &= All( desc.Dimension()  <= uint3{maxImageDimension3D} );	break;
		}
		result &= (desc.arrayLayers.Get() <= maxImageArrayLayers);

		if ( AllBits( desc.options, EImageOpt::CubeCompatible ))
			result &= All( desc.Dimension2() <= uint2{maxImageDimensionCube} );

		if ( desc.arrayLayers.Get() > 1 and desc.samples.Get() > 1 )
			result &= (multisampleArrayImage == EFeature::RequireTrue);

		//if ( desc.arrayLayers > 6 and AllBits( options, EImageOpt::CubeCompatible ))
		//	result &= (imageCubeArray == EFeature::RequireTrue);

		result &= (desc.queues == Default) or AllBits( queues.supported, desc.queues );

		const auto	CheckFormatUsage = [&desc] (const PixelFormatSet_t &fmtSet)
		{{
			bool	compat = fmtSet.contains( desc.format );
			if ( not compat and AllBits( desc.options, EImageOpt::ExtendedUsage ))
			{
				uint	count = 0;
				for (auto fmt : desc.viewFormats)
				{
					if ( fmt != Default )
					{
						compat |= fmtSet.contains( fmt );
						++count;
					}
				}
				if ( count == 0 )
					compat = true;
			}
			return compat;
		}};

		for (auto usage : BitfieldIterate( desc.usage ))
		{
			switch_enum( usage )
			{
				case EImageUsage::Storage :					result &= CheckFormatUsage( storageImageFormats );	break;
				case EImageUsage::ColorAttachment :
				case EImageUsage::DepthStencilAttachment :
				case EImageUsage::InputAttachment :			result &= CheckFormatUsage( attachmentFormats );	break;
				case EImageUsage::ShadingRate :				result &= (attachmentFragmentShadingRate == EFeature::RequireTrue);	break;
				case EImageUsage::FragmentDensityMap :		result &= (fragmentDensityMap			 == EFeature::RequireTrue);	break;
				case EImageUsage::TransferSrc :				break;
				case EImageUsage::TransferDst :				break;
				case EImageUsage::Sampled :					break;

				case EImageUsage::_Last :
				case EImageUsage::All :
				case EImageUsage::Transfer :
				case EImageUsage::RWAttachment :
				case EImageUsage::Unknown :
				default_unlikely :							DBG_WARNING( "unknown image usage" ); break;
			}
			switch_end
		}

		for (auto opt : BitfieldIterate( desc.options ))
		{
			switch_enum( opt )
			{
				case EImageOpt::CubeCompatible :			break;
				case EImageOpt::MutableFormat :				break;	// TODO
				case EImageOpt::Array2DCompatible :			break;	// TODO
				case EImageOpt::BlockTexelViewCompatible :	break;	// TODO
				case EImageOpt::SparseResidency :			break;
				case EImageOpt::SparseAliased :				break;
				case EImageOpt::Alias :						break;
				case EImageOpt::SampleLocationsCompatible :	break;	// TODO
				case EImageOpt::StorageAtomic :				result &= CheckFormatUsage( storageImageAtomicFormats );	break;
				case EImageOpt::ColorAttachmentBlend :		result &= CheckFormatUsage( attachmentBlendFormats );		break;
				case EImageOpt::SampledLinear :				result &= CheckFormatUsage( linearSampledFormats );			break;
				case EImageOpt::SampledMinMax :				break;	// TODO
				case EImageOpt::VertexPplnStore :			result &= (fragmentStoresAndAtomics			== EFeature::RequireTrue);	break;
				case EImageOpt::FragmentPplnStore :			result &= (vertexPipelineStoresAndAtomics	== EFeature::RequireTrue);	break;
				case EImageOpt::Subsampled :				result &= (fragmentDensityMap				== EFeature::RequireTrue);	break;
				case EImageOpt::LossyRTCompression :		break;	// TODO
				case EImageOpt::BlitSrc :					break;
				case EImageOpt::BlitDst :					break;
				case EImageOpt::ExtendedUsage :				break;	// TODO

				case EImageOpt::_Last :
				case EImageOpt::SparseResidencyAliased :
				case EImageOpt::Blit :
				case EImageOpt::All :
				case EImageOpt::Unknown :
				default_unlikely :							DBG_WARNING( "unknown image option" ); break;
			}
			switch_end
		}

		if ( desc.HasViewFormatList() )
			result &= (imageViewFormatList == EFeature::RequireTrue);

		return result;
	}

/*
=================================================
	IsSupported (ImageDesc, ImageViewDesc)
=================================================
*/
	bool  FeatureSet::IsSupported (const ImageDesc &desc, const ImageViewDesc &view) C_NE___
	{
		bool	result = true;

		if ( view.viewType == EImage_CubeArray )
			result &= (imageCubeArray == EFeature::RequireTrue);

		if ( desc.format != view.format and view.extUsage != Default )
		{
			result &= (imageViewExtendedUsage == EFeature::RequireTrue);

			for (auto usage : BitfieldIterate( view.extUsage ))
			{
				switch_enum( usage )
				{
					case EImageUsage::TransferSrc :				break;
					case EImageUsage::TransferDst :				break;
					case EImageUsage::Sampled :					break;
					case EImageUsage::Storage :					result &= storageImageFormats.contains( view.format );				break;
					case EImageUsage::ColorAttachment :
					case EImageUsage::DepthStencilAttachment :
					case EImageUsage::InputAttachment :			result &= attachmentFormats.contains( view.format );				break;
					case EImageUsage::ShadingRate :				result &= (attachmentFragmentShadingRate == EFeature::RequireTrue);	break;
					case EImageUsage::FragmentDensityMap :		result &= (fragmentDensityMap			 == EFeature::RequireTrue);	break;

					case EImageUsage::_Last :
					case EImageUsage::All :
					case EImageUsage::Transfer :
					case EImageUsage::RWAttachment :
					case EImageUsage::Unknown :
					default_unlikely :							DBG_WARNING( "unknown image usage" ); break;
				}
				switch_end
			}
		}

		const EImageUsage	all_usage = desc.usage | view.extUsage;		// may contain incompatible flags
		
		if ( AllBits( view.options, EImageViewOpt::FragmentDensityMap_Dynamic ))
			result &= (fragmentDensityMapDynamic == EFeature::RequireTrue);

		if ( AllBits( desc.options, EImageOpt::Subsampled ) and AllBits( all_usage, EImageUsage::Sampled ))
			result &= (view.layerCount <= maxSubsampledArrayLayers);

		return result;
	}

/*
=================================================
	IsCompatible
=================================================
*/
	bool  FeatureSet::IsCompatible (const FeatureSet &rhs) C_NE___
	{
	#ifdef AE_CI_BUILD_TEST
		return DbgIsCompatible( rhs );
	#else
		bool	res = true;
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )	res &= FS_IsCompatible( this->_name_, rhs._name_, AE_TOSTRING(_name_) );
		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT
		return res;
	#endif
	}

	bool  FeatureSet::DbgIsCompatible (const FeatureSet &rhs) C_NE___
	{
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )																			\
			if ( not FS_IsCompatible( this->_name_, rhs._name_, AE_TOSTRING(_name_) )) {												\
				AE_LOGW( String{AE_TOSTRING(_name_)} << "\n  lhs: " << ToString(this->_name_) << "\n  rhs: " << ToString(rhs._name_) <<	\
						 "\n  diff: " << FS_DiffToString( this->_name_, rhs._name_ ));													\
				return false;																											\
			}

		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT
		return true;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  FeatureSet::operator == (const FeatureSet &rhs) C_NE___
	{
		bool	res = true;
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )	res &= FS_Equal( this->_name_, rhs._name_, AE_TOSTRING(_name_) );
		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT
		return res;
	}

/*
=================================================
	operator >=
=================================================
*/
	bool  FeatureSet::operator >= (const FeatureSet &rhs) C_NE___
	{
		bool	res = true;
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )		res &= FS_GreaterEqual( this->_name_, rhs._name_, AE_TOSTRING(_name_) );
		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT
		return res;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  FeatureSet::CalcHash () C_NE___
	{
		HashVal	res;
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )		res << HashOf( _name_ );
		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT
		return res;
	}

/*
=================================================
	Init
=================================================
*/
	void  FeatureSet::Init (EFeature value) __NE___
	{
		UnsafeZeroMem( *this );

		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )\
			if constexpr( IsSame< _type_, EFeature >)	_name_ = FS_Set<_type_>( value );				\
			if constexpr( IsSame< _type_, POTValue >)	_name_ = FS_Set<_type_>( POTValue::Invalid() );	\
			if constexpr( IsSame< _type_, POTBytes >)	_name_ = FS_Set<_type_>( POTBytes::Invalid() );

		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT

		#define FS_CHECK_ALIGN( _name_, _align_ )	StaticAssert( alignof(_name_) == _align_, "" #_name_ );
		#define FS_CHECK_ALIGN_1( _name_, ... )		FS_CHECK_ALIGN( _name_, 1 )
		#define FS_CHECK_ALIGN_2( _name_, ... )		FS_CHECK_ALIGN( _name_, 2 )
		#define FS_CHECK_ALIGN_4( _name_, ... )		FS_CHECK_ALIGN( _name_, 4 )
		#define FS_CHECK_ALIGN_8( _name_, ... )		FS_CHECK_ALIGN( _name_, 8 )

		AE_FEATURE_SET_FIELDS( FS_CHECK_ALIGN_1, FS_CHECK_ALIGN_1, FS_CHECK_ALIGN_2, FS_CHECK_ALIGN_4, FS_CHECK_ALIGN_8 )

		#undef FS_CHECK_ALIGN
		#undef FS_CHECK_ALIGN_1
		#undef FS_CHECK_ALIGN_2
		#undef FS_CHECK_ALIGN_4
		#undef FS_CHECK_ALIGN_8
	}

/*
=================================================
	SetDefault
=================================================
*/
	void  FeatureSet::SetDefault () __NE___
	{
		maxShaderVersion.spirv	= MinSpirvVersion;
		maxShaderVersion.metal	= MinMetalVersion;

		maxViewports	= 1;

		maxUniformBufferSize				= 16_KiB;
		maxDescriptorSets					= 1;
		maxFragmentOutputAttachments		= 1;
		maxFragmentCombinedOutputResources	= 1;

		maxImageArrayLayers	= POTValue_From<1>;

		framebufferColorSampleCounts = SampleCountBits( 1 );
		framebufferDepthSampleCounts = SampleCountBits( 1 );
		maxFramebufferLayers		 = POTValue_From<1>;
	}

/*
=================================================
	MergeMin
=================================================
*/
	void  FeatureSet::MergeMin (const FeatureSet &rhs) __NE___
	{
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )	_name_ = FS_MergeMin( _name_, rhs. _name_, AE_TOSTRING(_name_) );
		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT
	}

/*
=================================================
	MergeMax
=================================================
*/
	void  FeatureSet::MergeMax (const FeatureSet &rhs) __NE___
	{
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )	_name_ = FS_MergeMax( _name_, rhs. _name_, AE_TOSTRING(_name_) );
		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT
	}

/*
=================================================
	AddDevice
=================================================
*/
	void  FeatureSet::AddDevice (uint vendorId, uint deviceId, StringView name) __NE___
	{
		EGPUVendor			vendor	= GetVendorTypeByID( vendorId );
		EGraphicsDeviceID	device	= GetEGraphicsDeviceByID( deviceId );

		if ( device == Default )
			device = GetEGraphicsDeviceByName( name );

		if ( vendor == Default )
			vendor = GetVendorTypeByDevice( device );

		if ( vendor != Default )
			vendorIds.include.insert( vendor );

		if ( device != Default )
			devicesIds.include.insert( device );
	}

/*
=================================================
	GetHashOfFieldNames
=================================================
*/
namespace {
	ND_ static HashVal64  HashOfStr (const char* str, uint shift) { return HashVal64{ ulong(uint(CT_Hash( str, UMax, 0x7453 ))) << (shift & 32) }; }
}
	HashVal64  FeatureSet::GetHashOfFieldNames () __NE___
	{
		HashVal64	result;
		uint		counter	= 0;

		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )														\
			result += (HashOfStr( AE_TOSTRING( _type_ ), counter ) + HashOfStr( AE_TOSTRING( _name_ ), counter ));	\
			++counter;																								\

		AE_FEATURE_SET_FIELDS_ALL( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT

		return result;
	}

/*
=================================================
	GetHashOfDependencies
=================================================
*/
	HashVal64  FeatureSet::GetHashOfDependencies () __NE___
	{
		HashVal64	result;
		uint		counter	= 0;

		#define AE_GRAPHICS_DEVICE_VISIT( _name_ )		result += HashOfStr( AE_TOSTRING( _name_ ), ++counter );
		AE_GRAPHICS_DEVICE_LIST( AE_GRAPHICS_DEVICE_VISIT )
		#undef AE_GRAPHICS_DEVICE_VISIT

		#define AE_PIXELFORMAT_VISIT( _name_ )			result += HashOfStr( AE_TOSTRING( _name_ ), ++counter );
		AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT )
		#undef AE_PIXELFORMAT_VISIT

		#define AE_VERTEXTYPE_VISIT( _name_, _value_ )	result += HashOfStr( AE_TOSTRING( _name_ ), ++counter );
		AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT )
		#undef AE_VERTEXTYPE_VISIT

		result += HashVal64{ sizeof(PerDescriptorSet) };
		result += HashVal64{ uint(ESubgroupOperation::_Count) };
		result += HashVal64{ uint(ESurfaceFormat::_Count) };
		result += HashVal64{ uint(EGPUVendor::_Count) };
		result += HashVal64{ uint(ESubgroupTypes::All) };
		result += HashVal64{ uint(EShaderStages::All) };

		result += HashVal64{ MaxSpirvVersion };
		result += HashVal64{ MaxMetalVersion };

		return result;
	}

/*
=================================================
	accelerationStructure
=================================================
*/
	EFeature  FeatureSet::accelerationStructure () C_NE___
	{
		if_likely( rayTracingPipeline == EFeature::RequireTrue )
			return EFeature::RequireTrue;

		if_likely( rayQuery == EFeature::RequireTrue )
			return EFeature::RequireTrue;

		if ( rayTracingPipeline == EFeature::Ignore or rayQuery == EFeature::Ignore )
			return EFeature::Ignore;

		return EFeature::RequireFalse;
	}

/*
=================================================
	SupportedShaderStages
=================================================
*/
	EShaderStages  FeatureSet::SupportedShaderStages () C_NE___
	{
		const EFeature	True		= EFeature::RequireTrue;
		EShaderStages	all_stages	= EShaderStages::Vertex | EShaderStages::Fragment;

		if ( computeShader == True )		all_stages |= EShaderStages::Compute;
		if ( geometryShader == True )		all_stages |= EShaderStages::Geometry;
		if ( tessellationShader == True )	all_stages |= EShaderStages::TessControl | EShaderStages::TessEvaluation;
		if ( tileShader == True )			all_stages |= EShaderStages::Tile;
		if ( meshShader == True )			all_stages |= EShaderStages::Mesh;
		if ( taskShader == True )			all_stages |= EShaderStages::MeshTask;
		if ( rayTracingPipeline == True )	all_stages |= EShaderStages::AllRayTracing;

		return all_stages;
	}

/*
=================================================
	GetHashOfFS_Precalculated
=================================================
*/
	HashVal64  FeatureSet::GetHashOfFS_Precalculated () __NE___
	{
		return HashVal64{0x88d7baf8f3f47b8cull};
	}


} // AE::Graphics
