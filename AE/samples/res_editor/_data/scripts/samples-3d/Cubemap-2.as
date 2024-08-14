// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Generate cubemap in compute shader.
	The cubemap is updated one tile per frame.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>			ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );

		const int2			tile_size		= int2(128);
		const int			tile_count_pot	= 5;	// 2^X
		const int2			cubemap_dim		= tile_size << tile_count_pot;

		RC<Image>			cubemap			= Image( EPixelFormat::RGBA8_UNorm, uint2(cubemap_dim), ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>			cubemap_view	= cubemap.CreateView( EImage::Cube );

		RC<Scene>			scene			= Scene();
		RC<Buffer>			cbuf			= Buffer();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 10.f );
			camera.FovY( 50.f );

			const float	s = 0.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// setup constants
		{
			cbuf.Int(	"tileSize",		tile_size );
			cbuf.Int(	"dim",			cubemap_dim );
			cbuf.Int(	"frameToTile",	tile_count_pot );
		}

		// setup skybox
		{
			RC<SphericalCube>	skybox = SphericalCube();

			skybox.ArgIn( "un_CubeMap", cubemap_view, Sampler_LinearMipmapRepeat );
			skybox.DetailLevel( 0, 9 );

			scene.Add( skybox );
		}

		// render loop
		{
			RC<ComputePass>		update_skybox = ComputePass();
			update_skybox.ArgIn(	"un_CBuf",		cbuf );
			update_skybox.ArgInOut(	"un_OutImage",	cubemap_view );
			update_skybox.LocalSize( 8, 8 );
			update_skybox.DispatchThreads( uint3( tile_size, cubemap_view.ArrayLayers() ));	// 1 tile per frame

			// TODO: optimize?
			GenMipmaps( cubemap_view );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/Cubemap.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Cubemap.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "GlobalIndex.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Noise.glsl"

	FBM_NOISE_Hash( PerlinNoise )
	TURBULENCE_FBM_Hash( PerlinNoiseFBM )


	ND_ int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

	ND_ int3  GetGlobalCoord2 ()
	{
		int3	coord = GetGlobalCoord();
		coord.x += un_CBuf.tileSize.x * int(un_PerPass.frame &  ((1u << un_CBuf.frameToTile) - 1));
		coord.y += un_CBuf.tileSize.y * int(un_PerPass.frame >> un_CBuf.frameToTile);
		return coord;
	}

	ND_ float3  PosOnSphere ()
	{
		float2	ncoord		= ToSNorm( (float2(GetGlobalCoord2().xy) + 0.5) / float2(un_CBuf.dim) );
		float3	sphere_pos	= CM_TangentialSC_Forward( ncoord, FaceIdx() );
		return sphere_pos;
	}

	void  Main ()
	{
		const int3	coord	= GetGlobalCoord2();
		float3		pos		= PosOnSphere();

		pos *= 8.0;
		pos += Turbulence_PerlinNoiseFBM( pos, 2.0, 0.6, 7 );

		float		hash	= DHash13( Voronoi( pos, float2(3.9672) ).icenter );

		gl.image.Store( un_OutImage, coord, Rainbow(hash) );
	}

#endif
//-----------------------------------------------------------------------------
