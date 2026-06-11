// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Screen-space distortion.
	Like in Doom:
	https://www.adriancourreges.com/blog/2016/09/09/doom-2016-graphics-study/
	https://simoncoenen.com/blog/programming/graphics/DoomEternalStudy
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define DRAW_CUBEMAP
#	define GEN_DISTORTION
#	define APPLY_DISTORTION
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>		color_rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize(), MipmapLevel(~0) );
		RC<Image>		dist_map	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/4 );

		const string	cm_addr		= "res/humus/Yokohama3/";	const string  cm_ext = ".jpg";	const uint2	cm_dim (2048);

		RC<Image>		cubemap		= Image( EPixelFormat::RGBA8_UNorm, cm_dim, ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>		cubemap_view= cubemap.CreateView( EImage::Cube );

		RC<FPVCamera>	camera		= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 10.f );
			camera.FovY( 70.f );

			const float	s = 0.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// load cubemap
		{
			cubemap.LoadLayer( cm_addr+ "posx" +cm_ext, 0, ImageLoadOpFlags::GenMipmaps );	// -Z
			cubemap.LoadLayer( cm_addr+ "negx" +cm_ext, 1, ImageLoadOpFlags::GenMipmaps );	// +Z
			cubemap.LoadLayer( cm_addr+ "posy" +cm_ext, 2, ImageLoadOpFlags::GenMipmaps );	// +Y	- up
			cubemap.LoadLayer( cm_addr+ "negy" +cm_ext, 3, ImageLoadOpFlags::GenMipmaps );	// -Y	- down
			cubemap.LoadLayer( cm_addr+ "posz" +cm_ext, 4, ImageLoadOpFlags::GenMipmaps );	// -X
			cubemap.LoadLayer( cm_addr+ "negz" +cm_ext, 5, ImageLoadOpFlags::GenMipmaps );	// +X
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "DRAW_CUBEMAP" );
			pass.Set( camera );
			pass.Output( "out_Color",	color_rt,		RGBA32f(0.0) );
			pass.ArgIn(  "un_Cubemap",	cubemap_view,	Sampler_LinearMipmapRepeat );

			GenMipmaps( color_rt );
		}{
			RC<Postprocess>		pass = Postprocess( "", "GEN_DISTORTION" );
			pass.Output( "out_Color",	dist_map,		RGBA32f(0.0) );
		}{
			RC<Postprocess>		pass = Postprocess( "", "APPLY_DISTORTION" );
			pass.Output( "out_Color",	rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_Color",	color_rt,		Sampler_LinearClamp );
			pass.ArgIn(  "un_DistMap",	dist_map,		Sampler_LinearClamp );
			pass.Slider( "iDistScale",	0.0,	0.1,	0.01 );
		}

		DbgView( dist_map, DbgViewFlags::NoCopy );
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DRAW_CUBEMAP
	#include "Ray.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		float	z_near	= un_PerPass.camera.clipPlanes.x;

		Ray		ray		= Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), z_near, uv );

		ray.dir.y = -ray.dir.y;

		out_Color = gl.texture.Sample( un_Cubemap, ray.dir );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_DISTORTION
	#include "Waves.glsl"
	#include "Normal.glsl"
	#include "InvocationID.glsl"

	ND_ float  WaveAnim (const float2 snorm, const float timeBias)
	{
		const float	wave_width	= 0.2;
		float		wave_r		= 1.5 * Fract( (un_PerPass.time + timeBias) * 0.5 );
		float		wave		= SmoothBumpStep( Length(snorm), wave_r, wave_r + wave_width );
		return wave * 0.1;
	}

	ND_ float3  GetPosition (const float2 snorm) {
		return float3( snorm, Max( WaveAnim( snorm, 0.f ), WaveAnim( snorm, 0.5f )) );
	}

	ND_ float3  GetPosition (const float2 snorm, const float2 offset) {
		return GetPosition( snorm + offset );
	}

	ND_ float3  GetNormal (const float2 snorm)
	{
		float3	norm;
		SmoothNormal3x3f( OUT norm, GetPosition, snorm, 1.0/64.0 );
		return norm;
	}

	void  Main ()
	{
		// XY - distortion
		// Z - occlusion ???

		out_Color = float4( ToUNorm( -GetNormal( GetGlobalCoordSNorm().xy )), 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef APPLY_DISTORTION
	#include "Math.glsl"

	void  Main ()
	{
		float2	uv		= (gl.FragCoord.xy - 0.5) * un_PerPass.invResolution;
		float3	dist	= ToSNorm( gl.texture.Sample( un_DistMap, uv ).rgb );

		uv += dist.xy * iDistScale;

		out_Color = gl.texture.SampleLod( un_Color, uv, 0.0 );
	}

#endif
//-----------------------------------------------------------------------------
