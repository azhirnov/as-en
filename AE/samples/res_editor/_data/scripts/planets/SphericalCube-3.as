// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Show distortion near to cube edge.
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
			camera.RotationScale( 0.25f );

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
		Slider( proj_type, "iProj",  0, 6,  2 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "LOD=" + lod );
			pass.ArgInOut(	"un_OutImage",	cubemap_view );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );
			pass.Constant(	"iProj",				proj_type );
			pass.Slider(	"iDistanceOnSphere",	0, 1 );

			GenMipmaps( cubemap_view );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "sphere/SphericalCube-3.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/sphere/SphericalCube-3.as)
			draw.Output( "out_Color", rt, RGBA32f(0.0) );
			draw.Output( ds, DepthStencil(1.f, 0) );
			draw.Constant( "iProj",		proj_type );
			draw.Slider( "iRadius",		0.0f,	2.5f,	1.f );	// used to check circle distortion
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"


	int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

	float3  ProjectToSphere (const float2 snormCoord)
	{
		switch ( iProj )
		{
			case 0 :	return CM_CubeSC_Forward(		snormCoord, FaceIdx() );
			case 1 :	return CM_IdentitySC_Forward(	snormCoord, FaceIdx() );
			case 2 :	return CM_TangentialSC_Forward(	snormCoord, FaceIdx() );
			case 3 :	return CM_EverittSC_Forward(	snormCoord, FaceIdx() );
			case 4 :	return CM_5thPolySC_Forward(	snormCoord, FaceIdx() );
			case 5 :	return CM_COBE_SC_Forward(		snormCoord, FaceIdx() );
			case 6 :	return CM_ArvoSC_Forward(		snormCoord, FaceIdx() );
		}
		return float3(0.0);
	}

	float  Distance2 (const float3 lhs, const float3 rhs)
	{
		if ( iDistanceOnSphere == 0 )
			return Distance( lhs, rhs );
		else
			return DistanceOnSphere( lhs, rhs );	// radius = 1.0
	}

	void  Main ()
	{
		const float		lod				= float(LOD);
		const float2	size			= float2(GetGlobalSize().xy);
		const float2	ncoord			= GetGlobalCoordSNorm().xy;
		const float3	pos_on_sphere	= ProjectToSphere( ncoord );	// == normal
		float4			color			= float4(0.0);

		color.b = AA_QuadGrid( size * ncoord, lod/size, 12.5 ) * 0.6;

		const float2	scale	= float2(lod);
		float			dist	= float_max;

		for (int y = -1; y <= 1; ++y)
		for (int x = -1; x <= 1; ++x)
		{
			const float2	pos_on_face	= Floor( scale * ncoord ) + float2(x,y) + 0.5;
			const float3	obj_pos		= ProjectToSphere( pos_on_face / scale );
			const float		d			= Distance2( pos_on_sphere, obj_pos ) - 0.06;

			dist = Min( dist, d );
		}

		color.g = Saturate( 1.0 - SmoothStep( dist, -0.005, 0.005 ));
		color.b *= (1.0 - color.g);

		gl.image.Store( un_OutImage, GetGlobalCoord(), color );
	}

#endif
//-----------------------------------------------------------------------------
