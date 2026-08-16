// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "graphics_rhi/Public/CoopVector.h"

namespace AE::Graphics
{

/*
=================================================
	ConvertCoopMatrixCmd ctor
=================================================
*/
	ConvertCoopMatrixCmd::ConvertCoopMatrixCmd (const ConvertCoopMatrixCmd2 &other) __NE___ :
		srcSize{other.srcSize}, dstSize{other.dstSize},
		numRows{other.numRows}, numColumns{other.numColumns},
		srcStride{other.srcStride}, dstStride{other.dstStride},
		srcType{other.srcType}, dstType{other.dstType},
		srcLayout{other.srcLayout}, dstLayout{other.dstLayout}
	{
		// ignored: srcOffset, srcBuffer, dstOffset, dstBuffer
		// not initialized: srcAddress, dstAddress
	}

	ConvertCoopMatrixCmd::ConvertCoopMatrixCmd (const ConvertCoopMatrixOnHost &other) __NE___ :
		srcSize{other.srcSize}, dstSize{other.dstSize},
		numRows{other.numRows}, numColumns{other.numColumns},
		srcStride{other.srcStride}, dstStride{other.dstStride},
		srcType{other.srcType}, dstType{other.dstType},
		srcLayout{other.srcLayout}, dstLayout{other.dstLayout}
	{
		// ignored: srcData, dstData
		// not initialized: srcAddress, dstAddress
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ConvertCoopMatrixCmd2 ctor
=================================================
*/
	ConvertCoopMatrixCmd2::ConvertCoopMatrixCmd2 (const ConvertCoopMatrixCmd &other) __NE___ :
		srcSize{other.srcSize}, dstSize{other.dstSize},
		numRows{other.numRows}, numColumns{other.numColumns},
		srcStride{other.srcStride}, dstStride{other.dstStride},
		srcType{other.srcType}, dstType{other.dstType},
		srcLayout{other.srcLayout}, dstLayout{other.dstLayout}
	{
		// ignored: srcAddress, dstAddress
		// not initialized: srcOffset, srcBuffer, dstOffset, dstBuffer
	}

	ConvertCoopMatrixCmd2::ConvertCoopMatrixCmd2 (const ConvertCoopMatrixOnHost &other) __NE___ :
		srcSize{other.srcSize}, dstSize{other.dstSize},
		numRows{other.numRows}, numColumns{other.numColumns},
		srcStride{other.srcStride}, dstStride{other.dstStride},
		srcType{other.srcType}, dstType{other.dstType},
		srcLayout{other.srcLayout}, dstLayout{other.dstLayout}
	{
		// ignored: srcData, dstData
		// not initialized: srcOffset, srcBuffer, dstOffset, dstBuffer
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ConvertCoopMatrixOnHost ctor
=================================================
*/
	ConvertCoopMatrixOnHost::ConvertCoopMatrixOnHost (const ConvertCoopMatrixCmd &other) __NE___ :
		srcSize{other.srcSize}, dstSize{other.dstSize},
		numRows{other.numRows}, numColumns{other.numColumns},
		srcStride{other.srcStride}, dstStride{other.dstStride},
		srcType{other.srcType}, dstType{other.dstType},
		srcLayout{other.srcLayout}, dstLayout{other.dstLayout}
	{
		// ignored: srcAddress, dstAddress
		// not initialized: srcData, dstData
	}

	ConvertCoopMatrixOnHost::ConvertCoopMatrixOnHost (const ConvertCoopMatrixCmd2 &other) __NE___ :
		srcSize{other.srcSize}, dstSize{other.dstSize},
		numRows{other.numRows}, numColumns{other.numColumns},
		srcStride{other.srcStride}, dstStride{other.dstStride},
		srcType{other.srcType}, dstType{other.dstType},
		srcLayout{other.srcLayout}, dstLayout{other.dstLayout}
	{
		// ignored: srcOffset, srcBuffer, dstOffset, dstBuffer
		// not initialized: srcData, dstData
	}
//-----------------------------------------------------------------------------



/*
=================================================
	c_CoopMatrixConfigs
=================================================
*/
namespace {
	static constexpr CoopMatrixConfig  c_CoopMatrixConfigs [] = {
		//|				AB					|					CR				  | M | N | K | SatAccum | ECoopMatrixCfg |
		// fp16 -> fp16
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float16,	16, 16, 16 },		// ABfp16_CRfp16_M16_N16_K16
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float16,	64, 64, 16 },		// ABfp16_CRfp16_M64_N64_K16
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float16,	64, 32, 16 },		// ABfp16_CRfp16_M64_N32_K16
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float16,	64, 16, 16 },		// ABfp16_CRfp16_M64_N16_K16
		// fp16 -> fp32
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float32,	16, 16, 16 },		// ABfp16_CRfp32_M16_N16_K16
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float32,	 8,  8, 16 },		// ABfp16_CRfp32_M8_N8_K16
		// fp32 -> fp32
		{ ECoopMatrixComponentType::Float32,	ECoopMatrixComponentType::Float32,	64, 64,  8 },		// ABfp32_CRfp32_M64_N64_K8
		{ ECoopMatrixComponentType::Float32,	ECoopMatrixComponentType::Float32,	64, 32,  8 },		// ABfp32_CRfp32_M64_N32_K8
		{ ECoopMatrixComponentType::Float32,	ECoopMatrixComponentType::Float32,	64, 16,  8 },		// ABfp32_CRfp32_M64_N16_K8
		// bf16 -> bf16
		{ ECoopMatrixComponentType::BFloat16,	ECoopMatrixComponentType::BFloat16,	16, 16, 16 },		// ABbf16_CRbf16_M16_N16_K16
		// fp8e5m2 -> fp16/fp32
		{ ECoopMatrixComponentType::Float8_E5M2, ECoopMatrixComponentType::Float16, 16, 16, 32 },		// ABe5m2_CRfp16_M16_N16_K32
		{ ECoopMatrixComponentType::Float8_E5M2, ECoopMatrixComponentType::Float32, 16, 16, 32 },		// ABe5m2_CRfp32_M16_N16_K32
		// fp8e4m3 -> fp16/fp32
		{ ECoopMatrixComponentType::Float8_E4M3, ECoopMatrixComponentType::Float16, 16, 16, 32 },		// ABe4m3_CRfp16_M16_N16_K32
		{ ECoopMatrixComponentType::Float8_E4M3, ECoopMatrixComponentType::Float32, 16, 16, 32 },		// ABe4m3_CRfp32_M16_N16_K32
		// u8 -> u32
	/*	{ ECoopMatrixComponentType::UInt8,		ECoopMatrixComponentType::UInt32,	16, 16, 32 },		// ABu8_CRu32_M16_N16_K32
		{ ECoopMatrixComponentType::UInt8,		ECoopMatrixComponentType::UInt32,	16, 16, 16 },		// ABu8_CRu32_M16_N16_K16
		{ ECoopMatrixComponentType::UInt8,		ECoopMatrixComponentType::UInt32,	 8,  8, 32 },		// ABu8_CRu32_M8_N8_K32
		{ ECoopMatrixComponentType::UInt8,		ECoopMatrixComponentType::UInt32,	64, 64, 32 },		// ABu8_CRu32_M64_N64_K32
		{ ECoopMatrixComponentType::UInt8,		ECoopMatrixComponentType::UInt32,	64, 64, 32, true },	// ABu8_CRu32_M64_N64_K32_SA
		// s8 -> s32
		{ ECoopMatrixComponentType::SInt8,		ECoopMatrixComponentType::SInt32,	16, 16, 32 },		// ABs8_CRs32_M16_N16_K32
		{ ECoopMatrixComponentType::SInt8,		ECoopMatrixComponentType::SInt32,	16, 16, 32 },		// ABs8_CRs32_M16_N16_K16
		{ ECoopMatrixComponentType::SInt8,		ECoopMatrixComponentType::SInt32,	16, 16, 32 },		// ABs8_CRs32_M8_N8_K32
		{ ECoopMatrixComponentType::SInt8,		ECoopMatrixComponentType::SInt32,	16, 16, 32 },		// ABs8_CRs32_M64_N64_K32
		{ ECoopMatrixComponentType::SInt8,		ECoopMatrixComponentType::SInt32,	16, 16, 32 },		// ABs8_CRs32_M64_N64_K32_SA */
	};
	StaticAssert( CountOf(c_CoopMatrixConfigs) == uint(ECoopMatrixCfg::_Count) );
}
/*
=================================================
	CoopMatrixConfig ctor
=================================================
*/
	CoopMatrixConfig::CoopMatrixConfig (const ECoopMatrixCfg cfg) __NE___
	{
		if ( cfg < ECoopMatrixCfg::_Count )
			*this = c_CoopMatrixConfigs[ uint(cfg) ];
		else
			DBG_WARNING( "unknown ECoopMatrixCfg" );
	}

/*
=================================================
	ToECoopMatrixCfg
=================================================
*/
	ECoopMatrixCfg  CoopMatrixConfig::ToECoopMatrixCfg () C_NE___
	{
		for (usize i = 0; i < CountOf(c_CoopMatrixConfigs); ++i)
		{
			if ( c_CoopMatrixConfigs[i] == *this )
				return ECoopMatrixCfg(i);
		}
		return Default;
	}

/*
=================================================
	ToString
=================================================
*/
#ifdef AE_ENABLE_LOGS
namespace
{
	static StringView  ToStr (ECoopMatrixComponentType type) __NE___
	{
		switch_enum( type )
		{
			using enum ECoopMatrixComponentType;
			case Float16 :		return "fp16";
			case Float32 :		return "fp32";
			case Float64 :		return "fp64";
			case BFloat16 :		return "bf16";
			case Float8_E4M3 :	return "e4m3";
			case Float8_E5M2 :	return "e5m2";
			case SInt8 :		return "s8";
			case SInt16 :		return "s16";
			case SInt32 :		return "s32";
			case SInt64 :		return "s64";
			case SInt8x4 :		return "s8x4";
			case UInt8 :		return "u8";
			case UInt16 :		return "u16";
			case UInt32 :		return "u32";
			case UInt64 :		return "u64";
			case UInt8x4 :		return "u8x4";
			case _Count :		break;
		}
		switch_end
		return "unk";
	}
}

	String  CoopMatrixConfig::ToString () C_Th___
	{
		String	str;
		if ( a == b and c == res )
			str << "AB: " << ToStr( a ) << ", CR: " << ToStr( c );
		else
			str << "A: " << ToStr( a ) << ", B: " << ToStr( b ) << ", C: " << ToStr( c ) << ", R: " << ToStr( res );

		str << ", MxNxK: " << Base::ToString( m ) << 'x' << Base::ToString( n ) << 'x' << Base::ToString( k );
		return	str;
	}
#endif
//-----------------------------------------------------------------------------



/*
=================================================
	c_CoopVectorConfig
=================================================
*/
namespace {
	static constexpr CoopVectorConfig  c_CoopVectorConfig [] = {
		//|					T				|				I & M				|					B & Res				|	transpose	|		ECoopVecCfg		|
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float16,		ECoopMatrixComponentType::Float16,		true	},	// Tfp16_IMfp16_BRfp16_Tp
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float8_E4M3,	ECoopMatrixComponentType::Float16,		false	},	// Tfp16_IMe4m3_BRfp16
		{ ECoopMatrixComponentType::Float16,	ECoopMatrixComponentType::Float8_E5M2,	ECoopMatrixComponentType::Float16,		false	},	// Tfp16_IMe5m2_BRfp16
		{ ECoopMatrixComponentType::SInt8,		ECoopMatrixComponentType::SInt8,		ECoopMatrixComponentType::SInt32,		false	},	// Ts8_IMs8_BRs32
	};
	StaticAssert( CountOf(c_CoopVectorConfig) == uint(ECoopVecCfg::_Count) );
}
/*
=================================================
	CoopVectorConfig ctor
=================================================
*/
	CoopVectorConfig::CoopVectorConfig (ECoopVecCfg cfg) __NE___
	{
		if ( cfg < ECoopVecCfg::_Count )
			*this = c_CoopVectorConfig[ uint(cfg) ];
		else
			DBG_WARNING( "unknown ECoopVecCfg" );
	}

/*
=================================================
	ToECoopVecCfg
=================================================
*/
	ECoopVecCfg  CoopVectorConfig::ToECoopVecCfg () C_NE___
	{
		for (usize i = 0; i < CountOf(c_CoopVectorConfig); ++i)
		{
			if ( *this == c_CoopVectorConfig[i] )
				return ECoopVecCfg(i);
		}
		return Default;
	}

/*
=================================================
	ToString
=================================================
*/
#ifdef AE_ENABLE_LOGS
	String  CoopVectorConfig::ToString () C_Th___
	{
		String	str;
		str << "Vec: " << ToStr( inputType )
			<< ", In: " << ToStr( inputInterpretation )
			<< ", Mat: " << ToStr( matrixInterpretation )
			<< ", Bias: " << ToStr( biasInterpretation )
			<< ", Res: " << ToStr( resultType );

		if ( transpose )
			str << ", transpose";

		return	str;
	}
#endif
//-----------------------------------------------------------------------------

} // AE::Graphics
