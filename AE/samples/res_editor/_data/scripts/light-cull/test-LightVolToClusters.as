// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Project omni and spot light into clusters (3d tiles).
	Used in clustered deferred shading.

	related:
	* [view clusters in 3D](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-ClusterBoundingSphere.as)
	* [frustum culling](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/geom-cull/FrustumCulling.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PROJ_TO_CLUSTERS_V1
#	define PROJ_TO_CLUSTERS_V2
#	define VIEW_2D
#	define VIEW_2D_TOP
#	define VIEW_3D
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	RC<Image>			rt;
	RC<Image>			ds;
	RC<FPVCamera>		camera;
	RC<Buffer>			clusters;

	RC<DynamicUInt>		light_type;
	RC<DynamicFloat3>	light_pos;
	RC<DynamicFloat4>	light_params;
	RC<DynamicUInt3>	clusters_count;
	RC<DynamicFloat2>	clip_planes;


	void  DrawSphere ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create sphere
		{
			RC<Buffer>				sphere		= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 3, OUT positions, OUT indices );

			sphere.FloatArray( "positions",	positions );
			sphere.UIntArray(  "indices",	indices );
			sphere.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( sphere, "indices" );
			geometry.Draw( cmd );

			geometry.ArgIn( "un_Geometry",	sphere );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "*-drawLight.ppln" );
			draw.Output( "out_Color",		rt, RGBA32f(0.0) );
			draw.Output(					ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iLightPos",		light_pos );
			draw.Constant( "iLightParams",	light_params );
			draw.Constant( "iClipPlanes",	clip_planes );
			draw.EnableIfEqual( light_type, 0 );
		}
	}


	void  DrawCone ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create cone
		{
			RC<Buffer>				cone		= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			array<float3>	positions;
			array<uint>		indices;
			GetCone( 14, 1.0, 1.0, OUT positions, OUT indices );

			cone.FloatArray( "positions",	positions );
			cone.UIntArray(  "indices",		indices );
			cone.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( cone, "indices" );
			geometry.Draw( cmd );

			geometry.ArgIn( "un_Geometry",	cone );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw cone" );
			draw.AddPipeline( "*-drawLight.ppln" );
			draw.Output( "out_Color",		rt, RGBA32f(0.0) );
			draw.Output(					ds, DepthStencil( 1.f, 0 ));
			draw.Constant( "iLightPos",		light_pos );
			draw.Constant( "iLightParams",	light_params );
			draw.Constant( "iClipPlanes",	clip_planes );
			draw.Constant( "SPOTLIGHT",	1 );
			draw.EnableIfEqual( light_type, 1 );
		}
	}


	void  DrawClusters ()
	{
		RC<Scene>			scene1		= Scene();
		RC<Scene>			scene2		= Scene();
		RC<DynamicUInt>		draw_mode	= DynamicUInt();

		// create sphere
		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				geom_data	= Buffer();

			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 3, OUT positions, OUT indices );

			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.IndexBuffer(	geom_data,	"indices" );
			cmd.InstanceCount( clusters_count.Volume() );
			geometry.Draw( cmd );

			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn(	"un_Clusters",	clusters );

			scene2.Add( geometry );
		}

		// create box
		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				geom_data	= Buffer();

			// 2 - 3 -- near   6 - 7
			// | / |           | \ |
			// 0 - 1    far -- 4 - 5
			array<float3>		positions;	positions.resize( 8 );	// near[4], far[4]
			const array<uint>	indices		= {
				0, 1, 3,	0, 3, 2,	// front
				5, 4, 6,	5, 6, 7,	// back
				1, 5, 7,	1, 7, 3,	// right
				3, 7, 6,	3, 6, 2,	// top
				0, 4, 5,	0, 5, 1,	// bottom
				4, 0, 2,	4, 2, 6		// left
			};
			Assert( indices.size() == 6*2*3 );

			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= indices.size();
			cmd.IndexBuffer(	geom_data,	"indices" );
			cmd.InstanceCount( clusters_count.Volume() );
			geometry.Draw( cmd );

			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn(	"un_Clusters",	clusters );

			scene1.Add( geometry );
		}

		scene1.Set( camera );
		scene2.Set( camera );

		Slider( draw_mode,	"ClusterMode",	0,	2,	2 );	// boxes, spheres

		{
			RC<SceneGraphicsPass>	draw = scene1.AddGraphicsPass( "draw clusters" );
			draw.AddPipeline( "*-drawClusters.ppln" );
			draw.Output( "out_Color",	rt );
			draw.Output(				ds );
			draw.Constant( "DRAW_BOXES", 0 );
			draw.EnableIfEqual( draw_mode, 0 );
		}{
			RC<SceneGraphicsPass>	draw = scene2.AddGraphicsPass( "draw clusters" );
			draw.AddPipeline( "*-drawClusters.ppln" );
			draw.Output( "out_Color",	rt );
			draw.Output(				ds );
			draw.EnableIfEqual( draw_mode, 1 );
		}
	}


	void ASmain ()
	{
		// initialize
		RC<DynamicDim>	dim = SurfaceSize();

		@rt				= Image( EPixelFormat::RGBA8_UNorm, dim );	rt.Name( "RT-Color" );
		@ds				= Image( EPixelFormat::Depth32F, dim );		ds.Name( "RT-Depth" );
		@light_type		= DynamicUInt();
		@light_pos		= DynamicFloat3();
		@light_params	= DynamicFloat4();
		@clusters		= Buffer();
		@clusters_count	= DynamicUInt3();
		@clip_planes	= DynamicFloat2();

		clusters.ArrayLayout(
			"Cluster",
			"	float4		sp;"s +
			"	float3		points [8];" +
			"	uint		visible;",
			clusters_count.Volume()
		);

		// setup camera
		{
			@camera = FPVCamera();

			camera.ClipPlanes( 0.5f, 200.f );
			camera.FovY( 60.f );

			const float	s = 4.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position(float3(0.0, 0.0, -2.0));
			camera.Dimension( dim );
		}

		Slider( clusters_count,	"ClusterDim",		uint3(1),					uint3(40,20,64),		uint3(6,4,32) );
		Slider( light_pos,		"LightPos",			float3(-1.5, -1.5, 0.0),	float3(1.5, 1.5, 1.0),	float3(0.0, 0.0, 0.6) );
		Slider( light_params,	"LParams",			float4(0.0),				float4(1.0),			float4(0.1, 0.1, 0.0, 0.0) );
		Slider( light_type,		"LightType",		0,							1,						0 );
		Slider( clip_planes,	"ClipPlanes",		float2(0.01, 100.0),		float2(1.0, 1000.0),	float2(1.0, 100.0) );

		RC<DynamicUInt2>	tile_size = dim.XY().Div( clusters_count.XY() );
		Label( tile_size,		"TileSize" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "PROJ_TO_CLUSTERS_V1" );
			pass.Set( camera );
			pass.ArgOut(	"un_Clusters",		clusters );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.Constant(	"iResolution",		dim );
			pass.Constant(	"iLightPos",		light_pos );
			pass.Constant(	"iLightType",		light_type );
			pass.Constant(	"iLightParams",		light_params );
			pass.Constant(	"iClipPlanes",		clip_planes );
			pass.LocalSize( 8, 8, 1 );
			pass.DispatchThreads( clusters_count );
		}

		DrawSphere();
		DrawCone();
		DrawClusters();

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PROJ_TO_CLUSTERS_V1
	#include "Cone.glsl"
	#include "Sphere.glsl"
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"


	// returns {near, far} Z for cluster
	float2  ZProjection (int clusterZ, int clusterCount)
	{
		float	z_near		= iClipPlanes.x;	// un_PerPass.camera.clipPlanes[0];
		float	z_far		= iClipPlanes.y;	// un_PerPass.camera.clipPlanes[1];
		float	cl_near_z	= z_near * Pow( z_far / z_near,  float(clusterZ) / float(clusterCount) );
		float	cl_far_z	= z_near * Pow( z_far / z_near,  float(clusterZ+1) / float(clusterCount) );
		return	float2( cl_near_z, cl_far_z );
	}


	Frustum  CreateClusterFrustum (const int3 clusterIdx, const int3 clusterCount)
	{
		Frustum		main_fr		= Frustum_Create( un_PerPass.camera.frustum );
		FrustumRays	main_rays	= Frustum_ToRays( main_fr );

		FrustumRays	tile_rays	= FrustumRays_GetTile( main_rays, clusterIdx.xy, clusterCount.xy );
		float2		z_range		= ZProjection( clusterIdx.z, clusterCount.z );

		return Frustum_FromRays( tile_rays, z_range, un_PerPass.camera.pos );
	}


	void  Main ()
	{
		const int3		cluster_idx		= GetGlobalCoord();
		const int3		cluster_count	= int3(iClusterCount);
		const int		idx				= VecToIndex( cluster_idx, cluster_count );

		if ( AnyGreaterEqual( cluster_idx, cluster_count ))
			return;

		Frustum		frustum	= CreateClusterFrustum( cluster_idx, cluster_count );
		float3		conters [8];

		Frustum_ToCornerPoints( frustum, OUT conters );

		Sphere		sp;
		sp.center	= (conters[0] + conters[7]) * 0.5;
		float	r0	= Distance( sp.center, conters[1] );
		float	r1	= Distance( sp.center, conters[6] );
		sp.radius	= Max( r0, r1 );

		un_Clusters.elements[ idx ].sp		= float4( sp.center, sp.radius );
		un_Clusters.elements[ idx ].points	= conters;

		un_Clusters.elements[ idx ].visible = 1;
	}

#endif
//-----------------------------------------------------------------------------
