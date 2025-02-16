// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Visualize texture distortion.
	All quad in texture has the same size, but size on sphere depends on projection type.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	ds				= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>	scene			= Scene();

		RC<Image>	cubemap			= Image( EPixelFormat::RGBA8_UNorm, uint2(1024), ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>	cubemap_view	= cubemap.CreateView( EImage::Cube );

		const uint	lod				= 4;

		// setup camera
		{
			RC<OrbitalCamera>	camera = OrbitalCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );
			camera.Offset( 3.f );
			camera.OffsetScale( 10.0f );

			scene.Set( camera );
		}

		// setup sphere
		{
			RC<Buffer>				geom_data	= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;
			GetSphericalCube( lod, OUT positions, OUT indices );

			geom_data.FloatArray(	"positions",	positions );
			geom_data.UIntArray(	"indices",		indices );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( geom_data, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn( "un_CubeMap",	cubemap_view, Sampler_LinearMipmapClamp );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "LOD=" + lod );
			pass.ArgInOut(	"un_OutImage",	cubemap_view );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );

			GenMipmaps( cubemap_view );
		}
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "sphere/SphericalCube-2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/sphere/SphericalCube-2.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );
			draw.Slider( "iProj",	0,	6,	1  );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Hash.glsl"
	#include "Noise.glsl"
	#include "SDF.glsl"


	int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

	void  Main ()
	{
		const float		lod		= float(LOD) * 2.f;
		const float2	ncoord	= GetGlobalCoordUNorm().xy;

		float2	pos		= Floor( ncoord * lod );
		float4	color	= RainbowWrap( (pos.x * 2.0 / lod) + (pos.y * 0.25) );

		gl.image.Store( un_OutImage, GetGlobalCoord(), color );
	}

#endif
//-----------------------------------------------------------------------------
