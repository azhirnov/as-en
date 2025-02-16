// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Compare TBN matrix calculation.
*/
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color", rt );
			pass.Slider( "iCmp",	0,		3,		0 );
			pass.Slider( "iDiff",	1.f,	100.f,	10.f );
			pass.Slider( "iTBN",	0,		2,		0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"

	float3  Sphere (float2 uv, float2 duv)
	{
		uv = ToSNorm( uv ) * (duv.yx / duv.x);
		return UVtoSphereNormal( uv ).xyz;
	}

	#define GetPosition( _pos_, _offset_ )		Sphere( ((_pos_) + (_offset_) * duv), duv )
	#define GetUV( _uv_, _offset_ )				((_uv_) + (_offset_) * duv)

	float3  CalcTBN (uint idx, float2 uv, float2 duv)
	{
		switch ( idx )
		{
			case 0 : {
				float3		pos	= Sphere( uv, duv );
				float3x3	tbn	= ComputeTBNinWS_dxdy( uv, pos );
				return tbn[iTBN] * (iTBN == 2 ? -1.f : 1.f);
			}

			case 1 : {
				float3x3	tbn;
				SmoothTBN2x2f( tbn, GetPosition, GetUV, uv, 1.0 );
				return tbn[iTBN];
			}

			case 2 : {
				float3x3	tbn;
				SmoothTBN3x3f( tbn, GetPosition, GetUV, uv, 1.0 );
				return tbn[iTBN];
			}
		}
		return float3(0.0);
	}


	void  Main ()
	{
		const float2	scale	= float2(2.0,2.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const uint		idx		= uint(uv.x * scale.x) + uint(uv.y * scale.y) * uint(scale.x) - 1;
		const float2	uv2		= Fract( uv * scale );
		const float2	duv		= 1.0 / float2(GetGlobalSize().xy);
		const float2	duv2	= scale / float2(GetGlobalSize().xy * scale);

		out_Color = float4(1.0);

		switch ( iCmp )
		{
			case 0 :	out_Color.rgb = CalcTBN( idx, uv2, duv2 );		break;
			case 1 :	out_Color.rgb = -CalcTBN( idx, uv2, duv2 );		break;
			case 2 :	out_Color.rgb = Abs( CalcTBN( 0, uv, duv ) - CalcTBN( 1, uv, duv )) * iDiff;	break;
			case 3 :	out_Color.rgb = Abs( CalcTBN( 0, uv, duv ) - CalcTBN( 2, uv, duv )) * iDiff;	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
