// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Project omni and spot light into clusters (3d tiles).
	Used in clustered deferred shading.

	related:
	* [view clusters in 3D](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-ClusterBoundingSphere.as)
	* [frustum culling](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/geom-cull/FrustumCulling.as)
	* [Cluster Forward](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/wip-ClusterForward.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_PARAMS
#	define PROJ_TO_CLUSTERS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	RC<Image>			rt;
	RC<Image>			ds;
	RC<FPVCamera>		camera;
	RC<Buffer>			clusters;
	RC<Buffer>			params_buf;

	RC<DynamicUInt>		light_type;
	RC<DynamicUInt3>	clusters_count;
	RC<DynamicFloat>	cluster_alpha;


	void  DrawSphere ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create sphere
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphere( 3 );

			RC<Buffer>	sphere = mesh.ToBuffer();
			sphere.LayoutName( "GeometrySBlock" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	sphere );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = mesh.IndexCount();
			cmd.IndexBuffer( sphere, "indices" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "*-drawLight.ppln" );
			draw.Output( "out_Color",	rt, RGBA32f(0.0) );
			draw.Output(				ds, DepthStencil( 1.f, 0 ));
			draw.ArgIn(	"un_Params",	params_buf );
			draw.EnableIfEqual( light_type, 0 );
		}
	}


	void  DrawCone ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create cone
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCone( 32 );

			RC<Buffer>	cone = mesh.ToBuffer();
			cone.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = mesh.IndexCount();
			cmd.IndexBuffer( cone, "indices" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Geometry",	cone );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw cone" );
			draw.AddPipeline( "*-drawLight.ppln" );
			draw.Output( "out_Color",	rt, RGBA32f(0.0) );
			draw.Output(				ds, DepthStencil( 1.f, 0 ));
			draw.ArgIn(	"un_Params",	params_buf );
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
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphere( 3 );

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometrySBlock" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn(	"un_Clusters",	clusters );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= mesh.IndexCount();
			cmd.IndexBuffer(	geom_data,	"indices" );
			cmd.InstanceCount( clusters_count.Volume() );
			geometry.Draw( cmd );

			scene2.Add( geometry );
		}

		// create frustum
		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				geom_data	= Buffer();

			// 2 - 3 -- near   6 - 7
			// | / |           | \ |
			// 0 - 1    far -- 4 - 5
			array<float3>		positions;	positions.resize( 8 );	// near[4], far[4]
			array<uint>			indices;	GetFrustumIndices( OUT indices );

			geom_data.FloatArray( "position",	positions );
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

		Slider( draw_mode,	"ClusterMode",	0,	3,	1 );	// none, boxes, spheres, boxes and spheres

		{
			RC<SceneGraphicsPass>	draw = scene1.AddGraphicsPass( "draw clusters" );
			draw.AddPipeline( "*-drawClusters.ppln" );
			draw.Output( "out_Color",		rt );
			draw.Output(					ds );
			draw.Constant( "DRAW_BOXES",	0 );
			draw.Constant( "iClusterAlpha",	cluster_alpha );
			draw.EnableIfAnyBit( draw_mode, 1 );
		}{
			RC<SceneGraphicsPass>	draw = scene2.AddGraphicsPass( "draw clusters" );
			draw.AddPipeline( "*-drawClusters.ppln" );
			draw.Output( "out_Color",		rt );
			draw.Output(					ds );
			draw.Constant( "iClusterAlpha",	cluster_alpha );
			draw.EnableIfAnyBit( draw_mode, 2 );
		}
	}


	void ASmain ()
	{
		// initialize
		RC<DynamicDim>	dim = SurfaceSize();

		@rt				= Image( EPixelFormat::RGBA8_UNorm, dim );	rt.Name( "RT-Color" );
		@ds				= Image( EPixelFormat::Depth32F, dim );		ds.Name( "RT-Depth" );
		@clusters		= Buffer();
		@params_buf		= Buffer();
		@light_type		= DynamicUInt();
		@clusters_count	= DynamicUInt3();
		@cluster_alpha	= DynamicFloat();

		clusters.ArrayLayout(
			"Cluster",
			"	float4		sp;"
			"	float3		points [8];"
			"	uint		visible;",
			clusters_count.Volume()
		);

		params_buf.UseLayout(
			"Params",
			"	float3		lightPos;"
			"	float		sphereRadius;"
			"	float3		coneDir;"
			"	float		coneAngle;"
			"	float		coneHeight;"
			"	float2		clipPlanes;"
			"	float4		frustumPlanes [6];"
			"	uint		visibleClusters;"
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

		//	camera.Position(float3(0.0, 0.0, -2.0));
			camera.Dimension( dim );
		}

		Slider( clusters_count,	"ClusterDim",		uint3(1),	uint3(40,20,64),	uint3(18,11,32) );
		Slider( light_type,		"LightType",		0,			1,					1 );
		Slider( cluster_alpha,	"Alpha",			0.0,		1.0,				0.2 );

		RC<DynamicUInt2>	tile_size	= dim.XY().Div( clusters_count.XY() );
		RC<DynamicUInt>		total_cl	= clusters_count.Volume();

		Label( tile_size,	"TileSize" );
		Label( total_cl,	"ClusterCount" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_PARAMS" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Params",		params_buf );
			pass.Slider(	"iLPos",			float3(-0.1, -0.1, 0.0),	float3(1.1, 1.1, 1.0),	float3(0.5, 0.5, 0.1) );
			pass.Slider(	"iLParams",			float4(0.0),				float4(1.0),			float4(0.2, 0.7, 0.0, 0.0) );
			pass.Slider(	"iClipPlanes",		float2(0.01, 100.0),		float2(1.0, 1000.0),	float2(1.0, 100.0) );
			pass.Constant(	"iLightType",		light_type );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "PROJ_TO_CLUSTERS" );
			pass.Set( camera );
			pass.ArgOut(	"un_Clusters",		clusters );
			pass.ArgInOut(	"un_Params",		params_buf );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.Constant(	"iLightType",		light_type );
			pass.Constant(	"iResolution",		dim );
			pass.Slider(	"iEnableVisTest",	0,		1,		1 );
			pass.Slider(	"iVisTestMode",		0,		2,		0 );
			pass.Slider(	"iDistErr",			0.0,	1.0,	0.0 );
			pass.LocalSize( 8, 8, 1 );
			pass.DispatchThreads( clusters_count );
		}

		DrawSphere();
		DrawCone();
		DrawClusters();

		RC<DynamicUInt>		vis_clusters = DynamicUInt();
		ReadBuffer( vis_clusters, params_buf, "visibleClusters" );
		Label( vis_clusters,	"VisibleClusters" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT_PARAMS
	#include "Frustum.glsl"

	void  SetupOmniLight (const float3 lightPos, const float2 size)
	{
		un_Params.lightPos		= lightPos;
		un_Params.sphereRadius	= Max( iLParams.x, 0.001 ) * Min( size.x, size.y ) * 0.5;
		un_Params.coneDir		= float3(0.0);
		un_Params.coneAngle		= 0.0;
		un_Params.coneHeight	= 0.0;
	}


	void  SetupSpotLight (const float3 lightPos, const float2 size)
	{
		const float3	dir		= SphericalToCartesian( float2( float_Pi2 * iLParams.z, float_Pi2 * iLParams.w ));
		const float		angle	= float_Pi * iLParams.y;
		const float		height	= Max( iLParams.x, 0.001 ) * Min( size.x, size.y ) * 0.5;

		un_Params.lightPos		= lightPos;
		un_Params.sphereRadius	= 0.0;
		un_Params.coneDir		= dir;
		un_Params.coneAngle		= angle;
		un_Params.coneHeight	= height;
	}


	void  Main ()
	{
		Frustum		frustum	= Frustum_FromMatrix( un_PerPass.camera.proj, iClipPlanes );	// view space

		un_Params.frustumPlanes		= frustum.planes;
		un_Params.clipPlanes		= iClipPlanes;
		un_Params.visibleClusters	= 0;

		float3	corners [4];
		Frustum_ZSlicePoints( frustum, iLPos.z, OUT corners );

		float3	light_pos	= BiLerp( corners[0], corners[1], corners[2], corners[3], iLPos.xy );
		float2	size;
				size.x		= Min( Distance( corners[0], corners[1] ), Distance( corners[2], corners[3] ));
				size.y		= Min( Distance( corners[0], corners[2] ), Distance( corners[1], corners[3] ));

		switch ( iLightType )
		{
			case 0 :	SetupOmniLight( light_pos, size );	break;
			case 1 :	SetupSpotLight( light_pos, size );	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PROJ_TO_CLUSTERS
	#include "SDF.glsl"
	#include "Cone.glsl"
	#include "Sphere.glsl"
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"
	#include "Intersectors.glsl"


	float2  ZProjectionLerp (int clusterZ, int clusterCount)
	{
		float	z_near		= un_Params.clipPlanes.x;	// un_PerPass.camera.clipPlanes[0];
		float	z_far		= un_Params.clipPlanes.y;	// un_PerPass.camera.clipPlanes[1];
		float	cl_near_z	= z_near * Pow( z_far / z_near,  float(clusterZ) / float(clusterCount) );
		float	cl_far_z	= z_near * Pow( z_far / z_near,  float(clusterZ+1) / float(clusterCount) );
		float2	z_factor	= (float2( cl_near_z, cl_far_z ) - z_near) / (z_far - z_near);
		return	z_factor;
	}


	Frustum  CreateClusterFrustum (const Frustum mainFrustum, const int3 clusterIdx, const int3 clusterCount)
	{
		float2		z_factor	= ZProjectionLerp( clusterIdx.z, clusterCount.z );
		Frustum		cluster_fr	= Frustum_ToCluster( mainFrustum, clusterIdx.xy, clusterCount.xy, z_factor );
		return cluster_fr;
	}


	bool  PointLight (const Frustum frustum, const Sphere frustumSphere)
	{
		const Sphere	sp	= Sphere_Create( un_Params.lightPos, un_Params.sphereRadius );

		switch ( iVisTestMode )
		{
			case 0 :	return Frustum_IsVisible( frustum, sp );
			case 1 :	return Sphere_AABB_Intersects( sp, Frustum_ToAABB( frustum ));
			case 2 :	return Sphere_Intersects( sp, frustumSphere );
		}
	}


	bool  SpotLight (const Frustum frustum, const Sphere frustumSphere)
	{
		const Cone	cone = Cone_Create( un_Params.lightPos, un_Params.coneDir, un_Params.coneAngle, un_Params.coneHeight );  // view space

		switch ( iVisTestMode )
		{
			case 0 :	return Frustum_IsConeVisible_v2( frustum, cone );
			case 1 :	return Cone_Sphere_Intersects( cone, frustumSphere );
			case 2 :
			{
				Quat	rot	= QFrom2Normals( float3(0.0, -1.0, 0.0),	// origin direction (negative because of SDF_Cone implementation)
											 cone.dir );				// new direction

				float3	pos	= frustumSphere.center - cone.origin;
						pos	= SDF_Rotate( pos, rot );

				float	d	= SDF_Cone( pos, SinCos( cone.halfAngle ), cone.height );
				return	d < frustumSphere.radius;
			}
		}
	}


	void  Main ()
	{
		const int3	cluster_idx		= GetGlobalCoord();
		const int3	cluster_count	= int3(iClusterCount);
		const int	idx				= VecToIndex( cluster_idx, cluster_count );

		if ( AnyGreaterEqual( cluster_idx, cluster_count ))
			return;

		Frustum		main_frustum	= Frustum_Create( un_Params.frustumPlanes );	// view space
		Frustum		cl_frustum		= CreateClusterFrustum( main_frustum, cluster_idx, cluster_count );
		Sphere		sp;

		// cluster geometry
		{
			float3	corners [8];
			Frustum_ToCornerPoints( cl_frustum, OUT corners );

			sp = FrustumCornerPoints_ToSphere( corners );
			sp.radius *= Saturate( 1.0 - iDistErr );

			un_Clusters.elements[ idx ].sp		= float4( sp.center, sp.radius );
			un_Clusters.elements[ idx ].points	= corners;
		}

		// test visibility
		{
			bool	visible = false;
			switch ( iLightType )
			{
				case 0 :	visible = PointLight( cl_frustum, sp );	break;
				case 1 :	visible = SpotLight( cl_frustum, sp );	break;
				// TODO: area light
			}

			un_Clusters.elements[ idx ].visible = (visible or iEnableVisTest == 0 ? 1 : 0);

			if ( visible )
				gl.AtomicAdd( INOUT un_Params.visibleClusters, 1 );
		}
	}

#endif
//-----------------------------------------------------------------------------
