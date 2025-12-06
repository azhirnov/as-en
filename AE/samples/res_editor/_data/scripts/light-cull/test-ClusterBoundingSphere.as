// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Visualize cluster bounding spheres.

	related:
	* [accuracy test](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/screenshot-test/Frustum-2.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define CREATE_CLUSTERS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( EPixelFormat::RGBA16F, dim );
		RC<Image>			ds				= Image( EPixelFormat::Depth32F, dim );
		RC<FPVCamera>		camera			= FPVCamera();
		RC<Scene>			scene1			= Scene();
		RC<Scene>			scene2			= Scene();

		RC<DynamicUInt3>	clusters_count	= DynamicUInt3();
		RC<Buffer>			clusters		= Buffer();
		RC<DynamicUInt>		draw_mode		= DynamicUInt();

		clusters.ArrayLayout(
			"Cluster",
			"	float4		sp;"s +
			"	float3		points [8];" +
			"	uint		visible;",
			clusters_count.Volume()
		);

		// setup camera
		{
			camera.ClipPlanes( 0.5f, 200.f );
			camera.FovY( 60.f );

			const float	s = 4.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position(float3(0.0, -20.0, -10.0));
			camera.Dimension( dim );
			scene1.Set( camera );
			scene2.Set( camera );
		}

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
				4, 0, 2,	4, 2, 6,	// left
				3, 7, 6,	3, 6, 2,	// top
				0, 4, 5,	0, 5, 1		// bottom
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

		Slider( clusters_count,	"ClusterDim",		uint3(1),	uint3(40,20,64),	uint3(6,4,32) );
		Slider( draw_mode,		"Spheres",			0,			1,					1 );		// boxes, spheres

		RC<DynamicUInt2>	tile_size = dim.XY().Div( clusters_count.XY() );
		Label( tile_size,		"TileSize" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "CREATE_CLUSTERS" );
			pass.Set( camera );
			pass.ArgOut(	"un_Clusters",		clusters );
			pass.Slider(	"iProjMode",		0,		2,			1 );			// linear view-space,  log view-space,  linear in depth buffer
			pass.Slider(	"iNearPlane",		0.01,	1.0,		1.0 );
			pass.Slider(	"iFarPlane",		100.0,	1000.0,		100.0 );
			pass.Slider(	"iUseFrustum",		0,		1 );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.Constant(	"iResolution",		dim );
			pass.LocalSize( 8, 8, 1 );
			pass.DispatchThreads( clusters_count );
		}{
			RC<SceneGraphicsPass>	draw = scene1.AddGraphicsPass( "draw boxes" );
			draw.AddPipeline( "*-draw.ppln" );
			draw.Output( "out_Color",	rt,		RGBA32f(0.f) );
			draw.Output(				ds,		DepthStencil(1.f, 0) );
			draw.Constant( "DRAW_BOXES", 0 );
			draw.EnableIfEqual( draw_mode, 0 );
		}{
			RC<SceneGraphicsPass>	draw = scene2.AddGraphicsPass( "draw spheres" );
			draw.AddPipeline( "*-draw.ppln" );
			draw.Output( "out_Color",	rt,		RGBA32f(0.f) );
			draw.Output(				ds,		DepthStencil(1.f, 0) );
			draw.EnableIfEqual( draw_mode, 1 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CREATE_CLUSTERS
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"


	float2x2  ClusterCoords (const int2 clusterIdx, const int2 clusterCount, const float z)
	{
		// frustum width and height
		float	h	= 2.0 * z * Tan( un_PerPass.camera.fov.y * 0.5 );
		float	w	= h * float(iResolution.x) / float(iResolution.y);

		float2	cl_size	= float2(w,h) / float2(clusterCount.xy);

		// bounding rect
		float2	min		= float2(w,h) * -0.5 + float2(clusterIdx.xy) * cl_size;
		float2	max		= min + cl_size;

		return float2x2( min, max );
	}


	// returns {near, far} Z for cluster
	float2  ZProjection (int clusterZ, int clusterCount)
	{
		float	z_near	= iNearPlane;	// un_PerPass.camera.clipPlanes[0];
		float	z_far	= iFarPlane;	// un_PerPass.camera.clipPlanes[1];

		switch ( iProjMode )
		{
			case 0 :	// linear view-space
			{
				float	cl_near_z	= z_near + float(clusterZ) * (z_far - z_near) / float(clusterCount);
				float	cl_far_z	= z_near + float(clusterZ + 1) * (z_far - z_near) / float(clusterCount);
				return	float2( cl_near_z, cl_far_z );
			}
			case 1 :	// log view-space
			{
				float	cl_near_z	= z_near * Pow( z_far / z_near,  float(clusterZ) / float(clusterCount) );
				float	cl_far_z	= z_near * Pow( z_far / z_near,  float(clusterZ+1) / float(clusterCount) );
				return	float2( cl_near_z, cl_far_z );
			}
			case 2 :	// linear in depth buffer
			{
				float4x4	proj		= f4x4_Perspective( un_PerPass.camera.fov.y, float2(iResolution), float2(z_near, z_far) );
				float		cl_near_z	= float(clusterZ)     / float(clusterCount);
				float		cl_far_z	= float(clusterZ + 1) / float(clusterCount);
							cl_near_z	= FastUnProjectZ( proj, cl_near_z );
							cl_far_z	= FastUnProjectZ( proj, cl_far_z );
				return	float2( cl_near_z, cl_far_z );
			}
		}
	}


	void  GetClusterCorners_v1 (const int3 clusterIdx, const int3 clusterCount, out float3 outPoints[8])
	{
		float2		z_near_far	= ZProjection( clusterIdx.z, clusterCount.z );
		float		cl_near_z	= z_near_far[0];
		float		cl_far_z	= z_near_far[1];
		float		cl_center_z	= (cl_near_z + cl_far_z) * 0.5;

		float2x2	cl_near_xy	= ClusterCoords( clusterIdx.xy, clusterCount.xy, cl_near_z );
		float2x2	cl_far_xy	= ClusterCoords( clusterIdx.xy, clusterCount.xy, cl_far_z );
		float2x2	cl_cen_xy	= ClusterCoords( clusterIdx.xy, clusterCount.xy, cl_center_z );

		//  2 - 3        6 - 7
		//  | / |-near   | \ |-far
		//  0 - 1        4 - 5

		outPoints[0] = float3( cl_near_xy[0].x, cl_near_xy[1].y,	cl_near_z );
		outPoints[1] = float3( cl_near_xy[1],						cl_near_z );
		outPoints[2] = float3( cl_near_xy[0],						cl_near_z );
		outPoints[3] = float3( cl_near_xy[1].x, cl_near_xy[0].y,	cl_near_z );

		outPoints[4] = float3( cl_far_xy[0].x, cl_far_xy[1].y,		cl_far_z );
		outPoints[5] = float3( cl_far_xy[1],						cl_far_z );
		outPoints[6] = float3( cl_far_xy[0],						cl_far_z );
		outPoints[7] = float3( cl_far_xy[1].x, cl_far_xy[0].y,		cl_far_z );
	}


	// use linear interpolation from frustum corner points
	void  GetClusterCorners_v2 (const int3 clusterIdx, const int3 clusterCount, out float3 outPoints[8])
	{
		float2		z_near_far	= ZProjection( clusterIdx.z, clusterCount.z );

		float2		uv0			= float2(clusterIdx.xy) / float2(clusterCount.xy);
		float2		uv1			= float2(clusterIdx.xy + 1) / float2(clusterCount.xy);
		float2		z_factor	= (z_near_far - iNearPlane) / (iFarPlane - iNearPlane);

		Frustum		main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, float2(iNearPlane, iFarPlane) );

		float3		corners [8];
		Frustum_ToCornerPoints( main_fr, OUT corners );

		#define CUBELERP( _uv_, _zfactor_ )\
			(Lerp(	BiLerp( corners[2], corners[3], corners[0], corners[1], (_uv_) ), \
					BiLerp( corners[6], corners[7], corners[4], corners[5], (_uv_) ), (_zfactor_) ))

		outPoints[0] = CUBELERP( float2(uv0.x, uv0.y), z_factor.x );
		outPoints[1] = CUBELERP( float2(uv1.x, uv0.y), z_factor.x );
		outPoints[2] = CUBELERP( float2(uv0.x, uv1.y), z_factor.x );
		outPoints[3] = CUBELERP( float2(uv1.x, uv1.y), z_factor.x );

		outPoints[4] = CUBELERP( float2(uv0.x, uv0.y), z_factor.y );
		outPoints[5] = CUBELERP( float2(uv1.x, uv0.y), z_factor.y );
		outPoints[6] = CUBELERP( float2(uv0.x, uv1.y), z_factor.y );
		outPoints[7] = CUBELERP( float2(uv1.x, uv1.y), z_factor.y );
	}


	void  Main ()
	{
		const int3		cluster_idx		= GetGlobalCoord();
		const int3		cluster_count	= int3(iClusterCount);
		const int		idx				= VecToIndex( cluster_idx, cluster_count );

		if ( AnyGreaterEqual( cluster_idx, cluster_count ))
			return;

		float3	corners[8];

		if ( iUseFrustum == 0 )
			GetClusterCorners_v1( cluster_idx, cluster_count, OUT corners );
		else
			GetClusterCorners_v2( cluster_idx, cluster_count, OUT corners );

		Sphere		sp;
		sp.center	= (corners[0] + corners[7]) * 0.5;
		float	r0	= Distance( sp.center, corners[1] );
		float	r1	= Distance( sp.center, corners[6] );
		sp.radius	= Max( r0, r1 );

		un_Clusters.elements[ idx ].sp		= float4( sp.center, sp.radius );
		un_Clusters.elements[ idx ].points	= corners;
		un_Clusters.elements[ idx ].visible = 1;
	}

#endif
//-----------------------------------------------------------------------------
