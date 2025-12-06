// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use trilinear filter with bias as alternative to anisotropy filtering.
	Much usefull for tiling when anisotropy has access out of 2x2x2 cube which cause artifacts.
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
		RC<Image>		rt		= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Image>		tex		= Image( EPixelFormat::sRGB8_A8, uint2(128), MipmapLevel(~0) );
		RC<Image>		dbg_tex	= Image( EPixelFormat::sRGB8_A8, uint2(128), MipmapLevel(~0) );
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
			pass.Slider( "iMode",			0,			3 );
			pass.Slider( "iView",			0,			2 );
			pass.Slider( "iEnableBias",		0,			1,		1 );
			pass.Slider( "iBias",			-3.0,		3.0,	-0.12 );	// limits: [-maxSamplerLodBias, maxSamplerLodBias]
			pass.Slider( "iAngleBias",		0.0,		10.0,	5.0 );
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
		float2	uv		= gl.FragCoord.xy / un_PerPass.resolution.xy;
		int		mode	= int(iMode);
		float	fov		= ToRad( 60.0 );
		float	ratio	= un_PerPass.resolution.x / un_PerPass.resolution.y;

		// split screen
		if ( iMode == 3 )
		{
			mode = Min( int(uv.x * 3.0), 2 );
			uv.x = Fract( uv.x * 3.0 );
			ratio /= 3.0;
		}

		bool	isec;
		{
			Ray	ray = Ray_Perspective( un_PerPass.camera.pos, fov, ratio, 1.0, ToSNorm(uv) );

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

			Ray_SetLength( ray, t );

			uv = ray.pos.xz * 2.0;

			ray = Ray_Perspective( un_PerPass.camera.pos, fov, ratio, 1.0, float2(0.0) );

			uv += Normalize( ray.dir.xz ) * un_PerPass.time * 0.2;
		}

		float	bias = iBias;
		float2	duv  = gl.fwidth( uv );

		bias += Max( duv.x, duv.y ) * iAngleBias;

		if ( iEnableBias == 0 )
			bias = 0.0;

		switch ( iView )
		{
			case 0 :
				switch ( mode ) {
					case 0 :	out_Color = gl.texture.Sample( un_Linear, uv, bias );			break;
					case 1 :	out_Color = gl.texture.Sample( un_LinearMipmap, uv, bias );		break;
					case 2 :	out_Color = gl.texture.Sample( un_Anisotropy, uv, bias );		break;
				}
				break;

			case 1 :
			case 2 :
			{
				float2	lod;
				switch ( mode ) {
					case 0 :	lod = gl.texture.QueryLod( un_Linear, uv );			break;
					case 1 :	lod = gl.texture.QueryLod( un_LinearMipmap, uv );	break;
					case 2 :	lod = gl.texture.QueryLod( un_Anisotropy, uv );		break;
				}
				float	level = (iView == 1 ? lod.x : lod.y) + bias;

				float4	c0	= Rainbow( Floor(level) / 7.0 );
				float4	c1	= Rainbow( Ceil(level) / 7.0 );

				out_Color = Lerp( c0, c1, Fract(level) );
				break;
			}
		}

		if ( ! isec )
			out_Color = float4(0.25);

		if ( gl.dFdx( float(mode) ) != 0 )
			out_Color = float4(0.0);
	}

#endif
//-----------------------------------------------------------------------------
