// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
		RC<Image>	rt = Image( EPixelFormat::RGBA16F, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();

			pass.ArgOut( "un_OutImage", rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );

			pass.Slider( "iScale",	0,	8,	3 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "SDF.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	float2  GetUV (int dx)
	{
		float2	uv = GetGlobalCoordUNorm( int3(dx,0,0) ).xy;
		uv.x = uv.x * 2.0 - 0.5;
		return uv;
	}

	float3  ApplySRGBCurve_v2 (float3 v)
	{
		v = Max( v, float3(0.0) );	// to avoid NaN
		float3	a = 12.92 * v;
		float3	b = 1.055 * Pow( v, float3(1.0/2.4) ) - 0.055;
		return Lerp( b, a, LessF( v, 0.0031308 ));
	}

	float3  RemoveSRGBCurve_v2 (float3 v)
	{
		v = Max( v, float3(0.0) );	// to avoid NaN
		float3 a = v / 12.92;
		float3 b = Pow( (v + 0.055) / 1.055, float3(2.4) );
		return Lerp( b, a, LessF( v, 0.04045 ));
	}

	float3  ApplySRGBCurve_v3 (float3 v)
	{
		v = Max( v, float3(0.0) );	// to avoid NaN
		float3	a = 12.92 * v;
		float3	b = 1.13005 * Sqrt(v - 0.00228) - 0.13448 * v + 0.005719;
		return Lerp( b, a, LessF( v, 0.0031308 ));
	}

	float3  RemoveSRGBCurve_v3 (float3 v)
	{
		v = Max( v, float3(0.0) );	// to avoid NaN
		float3 a = v / 12.92;
		float3 b = -7.43605 * v - 31.24297 * Sqrt(-0.53792 * v + 1.279924) + 35.34864;
		return Lerp( b, a, LessF( v, 0.04045 ));
	}


	void  Main ()
	{
		float4			col		= float4(0.0);
		const float		y_max	= 6.0;
		const float		y		= Floor( GetGroupCoordUNorm().y * y_max );
		const float		scale	= Exp10( float(iScale) );
		float			uv		= GetUV( 0 ).x;

		switch ( int(y) )
		{
			case 0 : {
				float	uv2 = ApplySRGBCurve( uv );
						uv2 = RemoveSRGBCurve( uv2 );
				col.g = Abs( uv - uv2 ) * scale;
				col.r = IsFinite( uv2 ) ? 0.0 : 1.0;
				break;
			}
			case 1 : {
				float	uv2 = ApplySRGBCurve_Fast( uv );
						uv2 = RemoveSRGBCurve_Fast( uv2 );
				col.g = Abs( uv - uv2 ) * scale;
				col.r = IsFinite( uv2 ) ? 0.0 : 1.0;
				break;
			}
			case 2 : {
				float	uv2 = ApplyREC709Curve( uv );
						uv2 = RemoveREC709Curve( uv2 );
				col.g = Abs( uv - uv2 ) * scale;
				col.r = IsFinite( uv2 ) ? 0.0 : 1.0;
				break;
			}
			case 3 : {
				float	uv2 = ApplyREC2084Curve( float3(uv) ).x;
						uv2 = RemoveREC2084Curve( float3(uv2) ).x;
				col.g = Abs( uv - uv2 ) * scale;
				col.r = IsFinite( uv2 ) ? 0.0 : 1.0;
				break;
			}
			case 4 : {
				float	uv2 = ApplySRGBCurve_v2( float3(uv) ).x;
						uv2 = RemoveSRGBCurve_v2( float3(uv2) ).x;
				col.g = Abs( uv - uv2 ) * scale;
				col.r = IsFinite( uv2 ) ? 0.0 : 1.0;
				break;
			}
			case 5 : {
				float	uv2 = ApplySRGBCurve_v3( float3(uv) ).x;
						uv2 = RemoveSRGBCurve_v3( float3(uv2) ).x;
				col.g = Abs( uv - uv2 ) * scale;
				col.r = IsFinite( uv2 ) ? 0.0 : 1.0;
				break;
			}
		}

		if ( Fract(uv) > Fract(GetUV(1).x) )
			col = float4(1.0);

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
