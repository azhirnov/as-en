// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
	CoopMatrixConfig ctor
=================================================
*/
	CoopMatrixConfig::CoopMatrixConfig (ECoopMatrixCfg cfg)	__NE___
	{
		switch_enum( cfg )
		{
			case ECoopMatrixCfg::Afp16_Bfp16_Cfp16_Rfp16_M16_N16_K16 :
			{
				a	= ECoopMatrixComponentType::Float16;
				b	= ECoopMatrixComponentType::Float16;
				c	= ECoopMatrixComponentType::Float16;
				res	= ECoopMatrixComponentType::Float16;
				m	= 16;
				n	= 16;
				k	= 16;
				break;
			}
			case ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M16_N16_K16 :
			case ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M8_N8_K16 :
			{
				a	= ECoopMatrixComponentType::Float16;
				b	= ECoopMatrixComponentType::Float16;
				c	= ECoopMatrixComponentType::Float32;
				res	= ECoopMatrixComponentType::Float32;

				if ( cfg == ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M16_N16_K16 ){
					m = 16;		n = 16;		k = 16;
				}
				if ( cfg == ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M8_N8_K16 ){
					m = 8;		n = 8;		k = 16;
				}
				break;
			}
			case ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M16_N16_K32 :
			case ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M16_N16_K16 :
			case ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M8_N8_K32 :
			{
				a	= ECoopMatrixComponentType::UInt8;
				b	= ECoopMatrixComponentType::UInt8;
				c	= ECoopMatrixComponentType::UInt32;
				res	= ECoopMatrixComponentType::UInt32;

				if ( cfg == ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M16_N16_K32 ){
					m = 16;		n = 16;		k = 32;
				}
				if ( cfg == ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M16_N16_K16 ){
					m = 16;		n = 16;		k = 16;
				}
				if ( cfg == ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M8_N8_K32 ){
					m = 8;		n = 8;		k = 32;
				}
				break;
			}
			case ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M16_N16_K32 :
			case ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M16_N16_K16 :
			case ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M8_N8_K32 :
			{
				a	= ECoopMatrixComponentType::SInt8;
				b	= ECoopMatrixComponentType::SInt8;
				c	= ECoopMatrixComponentType::SInt32;
				res	= ECoopMatrixComponentType::SInt32;

				if ( cfg == ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M16_N16_K32 ){
					m = 16;		n = 16;		k = 32;
				}
				if ( cfg == ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M16_N16_K16 ){
					m = 16;		n = 16;		k = 16;
				}
				if ( cfg == ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M8_N8_K32 ){
					m = 8;		n = 8;		k = 32;
				}
				break;
			}
			case ECoopMatrixCfg::_Count :
			default :
				DBG_WARNING( "unknown cooperative matrix config" );
		}
		switch_end
	}

/*
=================================================
	ToECoopMatrixCfg
=================================================
*/
	ECoopMatrixCfg  CoopMatrixConfig::ToECoopMatrixCfg () C_NE___
	{
		if ( a		== ECoopMatrixComponentType::Float16	and
			 b		== ECoopMatrixComponentType::Float16	and
			 c		== ECoopMatrixComponentType::Float16	and
			 res	== ECoopMatrixComponentType::Float16	and
			 m		== 16									and
			 n		== 16									and
			 k		== 16 )
			return ECoopMatrixCfg::Afp16_Bfp16_Cfp16_Rfp16_M16_N16_K16;

		if ( a		== ECoopMatrixComponentType::Float16	and
			 b		== ECoopMatrixComponentType::Float16	and
			 c		== ECoopMatrixComponentType::Float32	and
			 res	== ECoopMatrixComponentType::Float32 )
		{
			if ( m == 16 and n == 16 and k == 16 )
				return ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M16_N16_K16;

			if ( m ==  8 and n ==  8 and k == 16 )
				return ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M8_N8_K16;
		}

		if ( a		== ECoopMatrixComponentType::UInt8		and
			 b		== ECoopMatrixComponentType::UInt8		and
			 c		== ECoopMatrixComponentType::UInt32		and
			 res	== ECoopMatrixComponentType::UInt32 )
		{
			if ( m == 16 and n == 16 and k == 32 )
				return ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M16_N16_K32;

			if ( m == 16 and n == 16 and k == 16 )
				return ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M16_N16_K16;

			if ( m ==  8 and n ==  8 and k == 32 )
				return ECoopMatrixCfg::Au8_Bu8_Cu32_Ru32_M8_N8_K32;
		}

		if ( a		== ECoopMatrixComponentType::SInt8		and
			 b		== ECoopMatrixComponentType::SInt8		and
			 c		== ECoopMatrixComponentType::SInt32		and
			 res	== ECoopMatrixComponentType::SInt32	)
		{
			if ( m == 16 and n == 16 and k == 32 )
				return ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M16_N16_K32;

			if ( m == 16 and n == 16 and k == 16 )
				return ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M16_N16_K16;

			if ( m ==  8 and n ==  8 and k == 32 )
				return ECoopMatrixCfg::As8_Bs8_Cs32_Rs32_M8_N8_K32;
		}

		StaticAssert( uint(ECoopMatrixCfg::_Count) == 9 );

		return Default;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CoopVectorConfig ctor
=================================================
*/
	CoopVectorConfig::CoopVectorConfig (ECoopVecCfg cfg) __NE___
	{
		switch_enum( cfg )
		{
			case ECoopVecCfg::Tfp16_Ifp16_Mfp16_Bfp16_Rfp16_Tp :
				inputType				= ECoopMatrixComponentType::Float16;
				inputInterpretation		= ECoopMatrixComponentType::Float16;
				matrixInterpretation	= ECoopMatrixComponentType::Float16;
				biasInterpretation		= ECoopMatrixComponentType::Float16;
				resultType				= ECoopMatrixComponentType::Float16;
				transpose				= true;
				break;

			case ECoopVecCfg::Tfp16_Ifp8e4m3_Mfp8e4m3_Bfp16_Rfp16 :
				inputType				= ECoopMatrixComponentType::Float16;
				inputInterpretation		= ECoopMatrixComponentType::Float8_E4M3;
				matrixInterpretation	= ECoopMatrixComponentType::Float8_E4M3;
				biasInterpretation		= ECoopMatrixComponentType::Float16;
				resultType				= ECoopMatrixComponentType::Float16;
				transpose				= false;
				break;

			case ECoopVecCfg::Tfp16_Ifp8e5m2_Mfp8e5m2_Bfp16_Rfp16 :
				inputType				= ECoopMatrixComponentType::Float16;
				inputInterpretation		= ECoopMatrixComponentType::Float8_E5M2;
				matrixInterpretation	= ECoopMatrixComponentType::Float8_E5M2;
				biasInterpretation		= ECoopMatrixComponentType::Float16;
				resultType				= ECoopMatrixComponentType::Float16;
				transpose				= false;
				break;

			case ECoopVecCfg::Ts8_Is8_Ms8_Bs32_Rs32 :
				inputType				= ECoopMatrixComponentType::SInt8;
				inputInterpretation		= ECoopMatrixComponentType::SInt8;
				matrixInterpretation	= ECoopMatrixComponentType::SInt8;
				biasInterpretation		= ECoopMatrixComponentType::SInt32;
				resultType				= ECoopMatrixComponentType::SInt32;
				transpose				= false;
				break;

			case ECoopVecCfg::_Count :
			default :
				DBG_WARNING( "unknown cooperative vector config" );
		}
		switch_end
	}

/*
=================================================
	ToECoopVecCfg
=================================================
*/
	ECoopVecCfg  CoopVectorConfig::ToECoopVecCfg () C_NE___
	{
		if ( inputType				== ECoopMatrixComponentType::Float16	and
			 inputInterpretation	== ECoopMatrixComponentType::Float16	and
			 matrixInterpretation	== ECoopMatrixComponentType::Float16	and
			 biasInterpretation		== ECoopMatrixComponentType::Float16	and
			 resultType				== ECoopMatrixComponentType::Float16	and
			 transpose				== true )
			return ECoopVecCfg::Tfp16_Ifp16_Mfp16_Bfp16_Rfp16_Tp;


		if ( inputType				== ECoopMatrixComponentType::Float16		and
			 inputInterpretation	== ECoopMatrixComponentType::Float8_E4M3	and
			 matrixInterpretation	== ECoopMatrixComponentType::Float8_E4M3	and
			 biasInterpretation		== ECoopMatrixComponentType::Float16		and
			 resultType				== ECoopMatrixComponentType::Float16		and
			 transpose				== false )
			return ECoopVecCfg::Tfp16_Ifp8e4m3_Mfp8e4m3_Bfp16_Rfp16;

		if ( inputType				== ECoopMatrixComponentType::Float16		and
			 inputInterpretation	== ECoopMatrixComponentType::Float8_E5M2	and
			 matrixInterpretation	== ECoopMatrixComponentType::Float8_E5M2	and
			 biasInterpretation		== ECoopMatrixComponentType::Float16		and
			 resultType				== ECoopMatrixComponentType::Float16		and
			 transpose				== false )
			return ECoopVecCfg::Tfp16_Ifp8e5m2_Mfp8e5m2_Bfp16_Rfp16;

		if ( inputType				== ECoopMatrixComponentType::SInt8		and
			 inputInterpretation	== ECoopMatrixComponentType::SInt8		and
			 matrixInterpretation	== ECoopMatrixComponentType::SInt8		and
			 biasInterpretation		== ECoopMatrixComponentType::SInt32		and
			 resultType				== ECoopMatrixComponentType::SInt32		and
			 transpose				== false )
			return ECoopVecCfg::Ts8_Is8_Ms8_Bs32_Rs32;

		return Default;
	}
//-----------------------------------------------------------------------------

} // AE::Graphics
