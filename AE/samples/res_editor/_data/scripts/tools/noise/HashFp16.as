// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		const array<float>	params = {
			3,				// iHash
			2,				// iInSize
			1,				// iOutSize
			-1,				// iComp
			1.f, 0.f,		// iVScaleBias
			20.f,			// iPScale
			0,				// iPScalePow10
			0.f, 0.f, 0.5f,	// iPBias
		};

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );

			pass.Slider( "iHash",			0,						8,						int(params[0]) );
			pass.Slider( "iInSize",			1,						4,						int(params[1]) );
			pass.Slider( "iOutSize",		1,						4,						int(params[2]) );
			pass.Slider( "iComp",			-1,						3,						int(params[3]) );
			pass.Slider( "iVScaleBias",		float2(0.01f, -1.f),	float2(2.f, 2.f),		float2( params[4], params[5] ));
			pass.Slider( "iPScale",			0.1f,					100.f,					params[6] );
			pass.Slider( "iPScalePow10",	0,						38,						int(params[7]) );
			pass.Slider( "iPBias",			float3(-10.f),			float3(10.f),			float3( params[8], params[9], params[10] ));
		}
		Present( rt );
		DbgView( rt, DbgViewFlags::Histogram );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "InvocationID.glsl"


	#if !AE_ENABLE_HALF_TYPE
	# error require float16 support
	#endif

	#define float	half
	#define float2	half2
	#define float3	half3
	#define float4	half4
	#include "Hash.glsl"


	ND_ float4  DHash (const float4 inValue)
	{
		switch ( iInSize )
		{
			case 1 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(DHash11( inValue.x ));
					case 2 :	return DHash21( inValue.x ).rgrg;
					case 3 :	return float4(DHash31( inValue.x ), 0.0);
					case 4 :	return DHash41( inValue.x );
				}
				break;
			}
			case 2 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(DHash12( inValue.xy ));
					case 2 :	return DHash22( inValue.xy ).rgrg;
					case 3 :	return float4(DHash32( inValue.xy ), 0.0);
					case 4 :	return DHash42( inValue.xy );
				}
				break;
			}
			case 3 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(DHash13( inValue.xyz ));
					case 2 :	return DHash23( inValue.xyz ).rgrg;
					case 3 :	return float4(DHash33( inValue.xyz ), 0.0);
					case 4 :	return DHash43( inValue.xyz );
				}
				break;
			}
			case 4 :
			{
				switch ( iOutSize )
				{
					case 4 :	return float4(DHash44( inValue ));
				}
				break;
			}
		}
		return float4(0.0);
	}


	ND_ float4  HEHash (const uint4 inUint)
	{
		switch ( iInSize )
		{
			case 1 :	return float4(HEHash11hf( inUint.x ));
			case 2 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(HEHash12hf( inUint.xy ));
					case 2 :	return HEHash22hf( inUint.xy ).rgrg;
				}
				break;
			}
			case 3 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(HEHash13hf( inUint.xyz ));
					case 3 :	return float4(HEHash33hf( inUint.xyz ), 0.0);
				}
				break;
			}
			case 4 :
			{
				switch ( iOutSize )
				{
					case 2 :	return HEHash24hf( inUint ).rgrg;
					case 4 :	return HEHash44hf( inUint );
				}
				break;
			}
		}
		return float4(0.0);
	}


	ND_ float4  HashV3 (const float4 inFloat)
	{
		switch ( iInSize )
		{
			case 1 :	return float4(Hash_Uniform( inFloat.xy, inFloat.z ));
			case 2 :	return float4(Hash_Triangular( inFloat.xy, inFloat.z ));
			case 3 :	return float4(Hash_Gaussianish( inFloat.xy, inFloat.z ));
			case 4 :	return float4(Hash_MoarGaussianish( inFloat.xy, inFloat.z ));
		}
		return float4(0.0);
	}


	ND_ float4  MHash (const float4 inValue)
	{
		switch ( iInSize )
		{
			case 1 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(MHash11( inValue.x ));
					case 2 :	return MHash21( inValue.x ).rgrg;
					case 3 :	return float4(MHash31( inValue.x ), 0.0);
					case 4 :	return MHash41( inValue.x );
				}
				break;
			}
			case 2 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(MHash12( inValue.xy ));
					case 2 :	return MHash22( inValue.xy ).rgrg;
					case 3 :	return float4(MHash32( inValue.xy ), 0.0);
					case 4 :	return MHash42( inValue.xy );
				}
				break;
			}
			case 3 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(MHash13( inValue.xyz ));
					case 2 :	return MHash23( inValue.xyz ).rgrg;
					case 3 :	return float4(MHash33( inValue.xyz ), 0.0);
					case 4 :	return MHash43( inValue.xyz );
				}
				break;
			}
			case 4 :
			{
				switch ( iOutSize )
				{
					case 1 :	return float4(MHash14( inValue ));
					case 2 :	return MHash24( inValue ).rgrg;
					case 3 :	return float4(MHash34( inValue ), 0.0);
					case 4 :	return MHash44( inValue );
				}
				break;
			}
		}
		return float4(0.0);
	}


	ND_ float4  Hash (const float4 inFloat, const uint4 inUint)
	{
		#if iHash_max != 8
		#	error Hash type count must be 8
		#endif
		switch ( iHash )															//     License     |  scale  |   valid range   |  errors
		{																			//-----------------|---------|-----------------|-----------------
			case 0 :	return DHash( inFloat );									//       MIT       |	-	-	failed	-	-	-
			case 1 :	return float4(WeylHash12( inFloat.xy ));					//    unlicense    |	-	-	failed	-	-	-
			case 2 :	return float4(ModHash12( inFloat.xy ));						// CC BY-NC-SA 3.0 |	-	-	failed	-	-	-
			case 3 :	return HEHash( inUint );									//       MIT       |   any   |   0  .. 3*10^38 | -
			case 4 :	return HashV3( inFloat );									// CC BY-NC-SA 3.0 |	-	-	failed	-	-	-
			case 5 :	return MHash( inFloat );									// CC BY-NC-SA 3.0 |   any   |   0  .. 3*10^38 |
			case 6 :	return float4(UEFastHash12( inFloat.xy ));					//       ???       |	-	-	failed	-	-	-
			case 7 :	return float4(InterleavedGradientNoise12( inFloat.xy ));	//       ???       |  >100   |   100           | visible pattern, bad quality for >10^3, invalid for >10^5
			case 8 :	return float4(PseudoHash12( inFloat.xy ));					//       ???       |	-	-	failed	-	-	-
		}
	}

	#undef float
	#undef float2
	#undef float3
	#undef float4

	void  Main ()
	{
		const float		scale	= iPScale.x * Pow( 10.0, float(iPScalePow10) );
		const float3	pos		= float3( GetGlobalCoordSNormCorrected(), 0.f ) * scale + iPBias * float2(scale * 0.1, 1.0).xxy;
		const float4	fval	= float4( pos, (pos.x + pos.y + pos.z) * 0.33 );
		const uint4		uval	= floatBitsToUint( fval );
	//	const uint4		uval	= uint4(float16BitsToUint16( half4(fval) ));
		float4			hash	= Hash( half4(fval), uval );

		if ( iComp >= 0 and iComp < 4 )
			hash = float4(hash[iComp]);

		out_Color = hash * iVScaleBias.x + iVScaleBias.y;
	}

#endif
//-----------------------------------------------------------------------------
