// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Interpolation correction in compute shader.
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

		RC<Image>	cubemap			= Image( EPixelFormat::RGBA32F, uint2(1024), ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>	cubemap_view	= cubemap.CreateView( EImage::Cube );

		const uint	lod				= 6;

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

		RC<DynamicInt>	proj_type = DynamicInt();
		Slider( proj_type, "iProj",  1, 6,  1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "LOD=" + lod );
			pass.ArgInOut(	"un_OutImage",	cubemap_view );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );
			pass.Constant(	"iProj",		proj_type );
			pass.Slider(	"iCorrection",	0,	1,	1 );

			GenMipmaps( cubemap_view );
		}
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "sphere/SphericalCube-4.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/sphere/SphericalCube-4.as)
			draw.Output(	"out_Color", rt, RGBA32f(0.2) );
			draw.Output( ds, DepthStencil(1.f, 0) );
			draw.Constant(	"iProj",	proj_type );
			draw.Slider(	"iCmp",		0,		2,			2 );
			draw.Slider(	"iScale",	1.f,	1000.f,		310.f );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "CubeMap.glsl"

	int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

	float3  ProjectToSphere1 (const float2 snormCoord)
	{
		switch ( iProj )
		{
			case 1 :	return CM_IdentitySC_Forward(	snormCoord, FaceIdx() );
			case 2 :	return CM_TangentialSC_Forward(	snormCoord, FaceIdx() );
			case 3 :	return CM_EverittSC_Forward(	snormCoord, FaceIdx() );
			case 4 :	return CM_5thPolySC_Forward(	snormCoord, FaceIdx() );
			case 5 :	return CM_COBE_SC_Forward(		snormCoord, FaceIdx() );
			case 6 :	return CM_ArvoSC_Forward(		snormCoord, FaceIdx() );
		}
		return float3(0.0);
	}

	float3  ProjectToSphere2 (const float2 snormCoord)
	{
		const float		lod = float(LOD);
		const float2	min	= Floor( snormCoord * lod ) / lod;
		const float2	max	= Ceil( snormCoord * lod ) / lod;
		float3			p0	= ProjectToSphere1( float2( min.x, min.y ));
		float3			p1	= ProjectToSphere1( float2( max.x, min.y ));
		float3			p2	= ProjectToSphere1( float2( min.x, max.y ));
		float3			p3	= ProjectToSphere1( float2( max.x, max.y ));
		float2			uv	= (snormCoord - min) / (max - min);

		return Normalize( BiLerp( p0, p1, p2, p3, uv ));
	}

	float3  ProjectToSphere (const float2 snormCoord)
	{
		if ( iCorrection == 0 or iProj == 1 )
			return ProjectToSphere1( snormCoord );
		else
			return ProjectToSphere2( snormCoord );
	}

	void  Main ()
	{
		const float2	size	= float2(GetGlobalSize().xy);
		const float2	ncoord	= GetGlobalCoordSNorm().xy;
		float3			norm	= ProjectToSphere( ncoord );

		gl.image.Store( un_OutImage, GetGlobalCoord(), float4(norm, 1.0) );
	}

#endif
//-----------------------------------------------------------------------------
