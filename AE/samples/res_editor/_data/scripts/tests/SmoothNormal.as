// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Compare normal vector calculation.
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
			pass.Slider( "iCmp",	0,		6,		0 );
			pass.Slider( "iDiff",	1.f,	100.f,	10.f );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"

	float3  SpherePos (float2 uv, float2 duv)
	{
		uv = ToSNorm( uv ) * (duv.yx / duv.x);
		return UVtoSphereNormal( uv ).xyz;
	}

	#define GetPosition( _pos_, _offset_ )\
		SpherePos( (_pos_ + (_offset_) * duv), duv )

	float3  ComputeNormalInWS2_dxdy (const float3 worldPos)
	{
		float3	dx   = gl.dFdxFine( worldPos );
		float3	dy   = gl.dFdyFine( worldPos );
		float3	norm = Cross( dx, dy );
		return Normalize( norm );
	}

	float3  CalcNormal (uint idx, float2 uv, float2 duv)
	{
		switch ( idx )
		{
			case 0 : {
				uv = ToSNorm( uv ) * (duv.yx / duv.x);
				return float3(uv, 1.0);	// reference
			}

			case 1 : {
				float3	pos	= SpherePos( uv, duv );
				float3	n	= -ComputeNormalInWS_dxdy( pos );
				return n;
			}

			case 2 : {
				float3	pos	= SpherePos( uv, duv );
				float3	n	= ComputeNormalInWS2_dxdy( pos );
				return n;
			}

			case 3 : {
				float3	n;
				SmoothNormal2x1f( n, GetPosition, uv, 1.0 );
				return n;
			}

			case 4 : {
				float3	n;
				SmoothNormal2x2f( n, GetPosition, uv, 1.0 );
				return n;
			}

			case 5 : {
				float3	n;
				SmoothNormal3x3f( n, GetPosition, uv, 1.0 );
				return n;
			}
		}
		return float3(0.0);
	}


	void  Main ()
	{
		const float2	scale	= float2(3.0,2.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const uint		idx		= uint(uv.x * scale.x) + uint(uv.y * scale.y) * uint(scale.x);
		const float2	uv2		= Fract( uv * scale );
		const float2	duv		= 1.0 / float2(GetGlobalSize().xy);
		const float2	duv2	= scale / float2(GetGlobalSize().xy);

		out_Color = float4(1.0);

		switch ( iCmp )
		{
			case 0 :	out_Color.rgb = CalcNormal( idx, uv2, duv2 );	break;
			case 1 :	out_Color.rgb = -CalcNormal( idx, uv2, duv2 );	break;
			case 2 :	out_Color.rgb = Abs( CalcNormal( 1, uv, duv ) - CalcNormal( 2, uv, duv )) * iDiff;	break;
			case 3 :	out_Color.rgb = Abs( CalcNormal( 2, uv, duv ) - CalcNormal( 3, uv, duv )) * iDiff;	break;
			case 4 :	out_Color.rgb = Abs( CalcNormal( 2, uv, duv ) - CalcNormal( 4, uv, duv )) * iDiff;	break;
			case 5 :	out_Color.rgb = Abs( CalcNormal( 2, uv, duv ) - CalcNormal( 5, uv, duv )) * iDiff;	break;
			case 6 :	out_Color.rgb = Abs( CalcNormal( 4, uv, duv ) - CalcNormal( 5, uv, duv )) * iDiff;	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
