// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	How to blend in distortion map.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define DRAW_CUBEMAP
#	define APPLY_DISTORTION
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>		color_rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize(), MipmapLevel(~0) );
		RC<Image>		dist_map	= Image( EPixelFormat::RGB10_A2_UNorm, SurfaceSize() );
		RC<Image>		noise3d		= Image( EImageType::Float_3D, "res/tex/tile-noise-3d.aeimg" );

		const string	cm_addr		= "res/humus/Teide/";	const string  cm_ext = ".jpg";	const uint2	cm_dim (2048);

		RC<Image>		cubemap		= Image( EPixelFormat::RGBA8_UNorm, cm_dim, ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>		cubemap_view= cubemap.CreateView( EImage::Cube );

		RC<Scene>		scene		= Scene();
		RC<FPVCamera>	camera		= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 70.f );

			const float	s = 1.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
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

		// create geometry
		{
			const uint	segments = 32;
			RC<Mesh>	mesh	 = Mesh();

			mesh.SetAttributes( EAttribute::Position | EAttribute::Normal | EAttribute::SubMeshID );
			mesh.AddCube( Transform().Position( 0.0, 1.1, 0.0 ).Scale( 100.0, 0.1, 100.0 ));
			mesh.AddCylinder( segments, Transform().Position(  2.0, -1.0, 6.0 ).Scale( 1.0, 2.0, 1.0 ) );
			mesh.AddCylinder( segments, Transform().Position( -2.0, -1.0, 6.0 ).Scale( 1.0, 2.0, 1.0 ) );

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn( "un_Noise",		noise3d,	Sampler_LinearMipmapRepeat );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "DRAW_CUBEMAP" );
			pass.Set( camera );
			pass.Output( "out_Color",	color_rt,		RGBA32f(0.0) );
			pass.ArgIn(  "un_Cubemap",	cubemap_view,	Sampler_LinearMipmapRepeat );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "distortion" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Dist",	dist_map,		RGBA32f(0.0) );
			pass.Slider( "iNormScale",	0.1,			1.0,	1.0 );
		}{
			GenMipmaps( color_rt );

			RC<Postprocess>		pass = Postprocess( "", "APPLY_DISTORTION" );
			pass.Output( "out_Color",	rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_Color",	color_rt,		Sampler_LinearClamp );
			pass.ArgIn(  "un_DistMap",	dist_map,		Sampler_LinearClamp );
			pass.Slider( "iDistScale",	0.0,	0.2,	0.08 );
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
#ifdef APPLY_DISTORTION
	#include "Math.glsl"

	void  Main ()
	{
		float2	uv		= (gl.FragCoord.xy - 0.5) * un_PerPass.invResolution;
		float4	texel	= gl.texture.Sample( un_DistMap, uv );
		float3	dist	= Normalize( ToSNorm( texel.rgb )) * texel.a;

		uv += dist.xy * iDistScale;

		out_Color = gl.texture.SampleLod( un_Color, uv, 0.0 );
	}

#endif
//-----------------------------------------------------------------------------
