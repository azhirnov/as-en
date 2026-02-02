// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Compare gl.texture.Sample() with SwSampling().
	Use bilinear, triliniar and anisotropy filtering.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GENTEX
#	define RAYTRACE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>		tex		= Image( EPixelFormat::sRGB8_A8, uint2(128), MipmapLevel(~0) );
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 1000.f );
			camera.FovY( 70.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GENTEX" );
			pass.Output( "out_Color",	tex );

			GenMipmaps( tex );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RAYTRACE" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_Linear",		tex,		Sampler_LinearRepeat );
			pass.ArgIn(  "un_LinearMipmap",	tex,		Sampler_LinearMipmapRepeat );
			pass.ArgIn(  "un_Anisotropy",	tex,		Sampler_Anisotropy16Repeat );
			pass.Slider( "iMode",			0,			6 );
			pass.Slider( "iView",			0,			2 );
			pass.Slider( "iEnableBias",		0,			1,		1 );
			pass.Slider( "iBias",			-3.0,		3.0,	0.0 );	// limits: [-maxSamplerLodBias, maxSamplerLodBias]
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GENTEX
	#include "InvocationID.glsl"

	void Main ()
	{
		uint2	pos = uint2(GetGlobalCoord().xy) / 32;

		if ( ((pos.x + pos.y) & 1) == 0 )
			out_Color = float4(1.0);
		else
			out_Color = float4(0.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RAYTRACE
	#include "Ray.glsl"
	#include "Color.glsl"
	#include "TexSampling.glsl"
	#include "Intersectors.glsl"

	void Main ()
	{
		float2		uv			= gl.FragCoord.xy * un_PerPass.invResolution;
		int			mode		= int(iMode-1);
		float		fov			= ToRad( 60.0 );
		float		ratio		= un_PerPass.resolution.x / un_PerPass.resolution.y;
		const int	max_mode	= iMode_max-1;

		// split screen
		if ( iMode == 0 )
		{
			mode = Min( int(uv.x * iMode_max), max_mode );
			uv.x = Fract( uv.x * iMode_max );
			ratio /= iMode_max;
		}

		bool	isec;
		{
			Ray	ray = Ray_Perspective( un_PerPass.camera.pos, fov, ratio, 1.0, uv );

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

			Ray_SetLength( ray, t );

			uv = ray.pos.xz * 2.0;
		}

		float	bias = iBias;

		if ( iEnableBias == 0 )
			bias = 0.0;

		switch ( iView )
		{
			case 0 :
				switch ( mode ) {
					case 0 :	out_Color = gl.texture.Sample( un_Linear, uv, bias );			break;
					case 1 :	out_Color = SwSampling( un_Linear, uv, bias );					break;
					case 2 :	out_Color = gl.texture.Sample( un_LinearMipmap, uv, bias );		break;
					case 3 :	out_Color = SwSampling( un_LinearMipmap, uv, bias );			break;

					case 4 : {
						float	lod = gl.texture.QueryLod( un_LinearMipmap, uv * Exp2(bias) ).y;
						out_Color = gl.texture.SampleLod( un_LinearMipmap, uv, lod );
						break;
					}
					case 5 : {
						float	lod = SwQueryLod( un_LinearMipmap, uv, bias ).y;
						out_Color = gl.texture.SampleLod( un_LinearMipmap, uv, lod );
						break;
					}
				}
				break;

			case 1 :
			case 2 :
			{
				float2	lod;
				switch ( mode ) {
					case 0 :	lod = gl.texture.QueryLod( un_Linear, uv * Exp2(bias) );		break;
					case 1 :	lod = SwQueryLod( un_Linear, uv, bias );						break;
					case 2 :
					case 4 :	lod = gl.texture.QueryLod( un_LinearMipmap, uv * Exp2(bias) );	break;
					case 3 :
					case 5 :	lod = SwQueryLod( un_LinearMipmap, uv, bias );					break;
				}
				float	level = (iView == 1 ? lod.x : lod.y);

				float4	c0	= Rainbow( Floor(level) / 7.0 );
				float4	c1	= Rainbow( Ceil(level) / 7.0 );

				out_Color = Lerp( c0, c1, Fract(level) );
				break;
			}
		}

		if ( ! isec )
			out_Color = Rainbow( float(mode) / max_mode ) * 0.2;

		// split screen
		if ( iMode == 0 )
		{
			int	mode1 = Min( int((gl.FragCoord.x + 1.0) * un_PerPass.invResolution.x * iMode_max), max_mode );
			if ( mode != mode1 )
				out_Color = float4(0.0);
		}
	}

#endif
//-----------------------------------------------------------------------------
