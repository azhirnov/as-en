// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compare performance of light list building for clusters.

	ListMode switch:
	1.0 - one cluster per thread, check intersection with all lights
	1.1 - one cluster per warp, same as 1.0, better thread coherency in warp

	2.0 - one light per thread, intersect light with clusters
	2.1 - one light per warp, same as 2, better thread coherency in warp

	3.0 - same as 1.0, but used bitfields instead of light indices
	4.0 - same as 2.0, but used bitfields instead of light indices
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_LIGHTS
#	define BUILD_LIGHT_LIST_V10		// 1.0
#	define BUILD_LIGHT_LIST_V11		// 1.1 - subgroup opt
#	define BUILD_LIGHT_LIST_V20		// 2.0
#	define BUILD_LIGHT_LIST_V21		// 2.1 - subgroup opt
#	define BUILD_LIGHT_LIST_V30		// 3.0
#	define BUILD_LIGHT_LIST_V31		// 3.1 - subgroup opt
#	define BUILD_LIGHT_LIST_V40		// 4.0
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		rt_dim			= SurfaceSize();
		RC<Image>			rt_lights		= Image( EPixelFormat::RGBA8_UNorm, rt_dim );
		RC<Image>			rt_clusters		= Image( EPixelFormat::RGBA8_UNorm, rt_dim );
		RC<Image>			rt_woit1		= Image( EPixelFormat::RGBA16F,		rt_dim );
		RC<Image>			rt_woit2		= Image( EPixelFormat::R16F,		rt_dim );
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, rt_dim );
		RC<Image>			ds				= Image( Supported_DepthFormat(), rt_dim );

		RC<FPVCamera>		camera			= FPVCamera();
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_cl		= Scene();
		RC<Buffer>			lights_buf		= Buffer();
		RC<Buffer>			cluster_buf		= Buffer();
		RC<Buffer>			counter_buf		= Buffer();
		RC<Buffer>			params_buf		= Buffer();

		RC<DynamicUInt3>	clusters_count	= DynamicUInt3();
		RC<DynamicUInt>		clusters_total	= clusters_count.Volume();
		RC<DynamicUInt>		view_mode		= DynamicUInt();
		RC<DynamicUInt>		list_mode		= DynamicUInt();
		RC<DynamicUInt>		subgroup_opt	= DynamicUInt();
		RC<DynamicUInt>		list_ver		= list_mode.Mul( 10 ).Add( subgroup_opt );
		RC<DynamicUInt>		lock_frustum	= DynamicUInt();

		RC<DynamicUInt3>	grid_pot		= DynamicUInt3();
		RC<DynamicUInt3>	grid_dim		= grid_pot.Exp2();
		RC<DynamicUInt>		light_count		= grid_dim.Volume();

		RC<DynamicUInt>		repeat			= DynamicUInt();
		RC<DynamicFloat>	time			= DynamicFloat();
		RC<DynamicFloat>	time_ms			= time.Mul( 1000.0f );

		const bool			perf_test			= false;
		const bool			has_sg_size_ctrl	= GetFeatureSet().hasSubgroupSizeControl();
		const uint			max_lights_per_cl	= 32;
		const uint			warp_size			= has_sg_size_ctrl ? GetFeatureSet().getMaxSubgroupSize() : GetSubgroupSize();
		const uint			warps_per_wg		= 4;
		const uint3			max_clusters		= uint3(40,20,64);
		const uint			local_size			= warp_size * warps_per_wg;

		Slider( grid_pot,		"LightGridPOT",		uint3(1),	uint3(6), 		uint3(3) );
		Slider( clusters_count,	"ClusterCount",		uint3(1),	max_clusters,	uint3(18,11,32) );
		Slider( view_mode,		"View",				0,			2,			2 );
		Slider( list_mode,		"ListMode",			1,			4 );
		Slider( subgroup_opt,	"SubgroupOpt",		0,			1 );
		Slider( lock_frustum,	"LockFrustum",		0,			1,			0 );
		Slider( repeat,			"Repeat",			1,			32 );

		Label( clusters_total,	"Total Clusters" );
		Label( light_count,		"Total Lights" );

		// setup camera
		{
			camera.ClipPlanes( 0.5f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -4.7f));
			camera.Dimension( rt_dim );

			scene.Set( camera );
			scene_cl.Set( camera );
		}

		// setup buffer layouts
		{
			lights_buf.ArrayLayout(
				"OmniLightData",
				"	float3 	pos;"
				"	float	radius;"
				"	uint 	color;",
				light_count );

			cluster_buf.ArrayLayout(
				"ClusterLightList",
				"	uint	count;"		// atomic
				"	uint	indices["s+(max_lights_per_cl)+"];",
				clusters_total );

			counter_buf.UseLayout(
				"Counters",
				"	uint2		assignedLights_maxLightsPerCluster;"	// atomic
			);

			params_buf.UseLayout(
				"Params",
				"	float4x4	cameraProj;"
				"	float3x3	cameraView;"
				"	float2		cameraClipPlanes;"
				"	float3		cameraPos;"
				"	float3		cameraCornerPoints[8];"		// world space
			);
		}

		// create sphere
		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Mesh>				mesh		= Mesh();

			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphere( 3 );

			RC<Buffer>	sphere = mesh.ToBuffer();
			sphere.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = mesh.IndexCount();
			cmd.InstanceCount( light_count );
			cmd.IndexBuffer( sphere, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",		sphere );
			geometry.ArgIn( "un_OmniLights",	lights_buf );

			scene.Add( geometry );
		}

		// create cluster box (frustum)
		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				geom_data	= Buffer();

			array<float3>		positions;	positions.resize( 8 );
			array<uint>			indices;	GetFrustumIndices( OUT indices );

			geom_data.FloatArray( "position",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.layer			= ERenderLayer::Translucent;
			cmd.indexCount		= indices.size();
			cmd.IndexBuffer(	geom_data,	"indices" );
			cmd.InstanceCount( clusters_total );
			geometry.Draw( cmd );

			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn(	"un_Clusters",	cluster_buf );
			geometry.ArgIn(	"un_Params",	params_buf );
			geometry.ArgIn(	"un_Counters",	counter_buf );

			scene_cl.Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount	= 4;
			cmd.layer		= ERenderLayer::PostProcess;
			geometry.Draw( cmd );
			scene_cl.Add( geometry );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_LIGHTS" );
			pass.Set( camera );
			pass.ArgOut(	"un_OmniLights",	lights_buf );
			pass.ArgInOut(	"un_Params",		params_buf );
			pass.Constant(	"iGridDim",			grid_dim );
			pass.Constant(	"iLockFrustum",		lock_frustum );
			pass.Slider( 	"iRadiusPosScale",	float2(0.2),	float2(4.0),	float2(1.0) );
			pass.LocalSize( 8, 8, 8 );
			pass.DispatchThreads( grid_dim );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw lights" );
			pass.AddPipeline( "perf-TileLightList-draw.ppln" );
			pass.Output( "out_Color", 	rt_lights, 	RGBA32f(0.f) );
			pass.Output( 				ds, 		DepthStencil( 1.f, 0 ));
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V10" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgIn(		"un_Params",	params_buf );
			pass.ArgOut(	"un_Clusters",	cluster_buf );
			pass.ArgOut(	"un_Counters",	counter_buf );
			pass.Constant(	"iClusterCount",clusters_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )	pass.Constant(	"PERF_TEST", 1 );
			pass.LocalSize( 8, 8, 4 );
			pass.DispatchThreads( clusters_count );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 10 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V11" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgIn(		"un_Params",	params_buf );
			pass.ArgOut(	"un_Clusters",	cluster_buf );
			pass.ArgOut(	"un_Counters",	counter_buf );
			pass.Constant(	"iClusterCount",clusters_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )		pass.Constant(	"PERF_TEST", 1 );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
			pass.LocalSize( warp_size, 1, warps_per_wg );
			pass.DispatchGroups( clusters_count.DivCeil(uint3( 1, 1, warps_per_wg )));
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 11 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V20" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgIn(		"un_Params",	params_buf );
			pass.ArgOut(	"un_Clusters",	cluster_buf );
			pass.ArgOut(	"un_Counters",	counter_buf );
			pass.Constant(	"iClusterCount",clusters_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )	pass.Constant(	"PERF_TEST", 1 );
			pass.LocalSize( local_size );
			pass.DispatchThreads( light_count );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 20 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V21" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgIn(		"un_Params",	params_buf );
			pass.ArgOut(	"un_Clusters",	cluster_buf );
			pass.ArgOut(	"un_Counters",	counter_buf );
			pass.Constant(	"iClusterCount",clusters_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )		pass.Constant(	"PERF_TEST", 1 );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
			pass.LocalSize( local_size );
			pass.DispatchGroups( light_count.DivCeil( warps_per_wg ));
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 21 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V30" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgIn(		"un_Params",	params_buf );
			pass.ArgOut(	"un_Clusters",	cluster_buf );
			pass.ArgOut(	"un_Counters",	counter_buf );
			pass.Constant(	"iClusterCount",clusters_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )	pass.Constant(	"PERF_TEST", 1 );
			pass.LocalSize( 8, 8, 4 );
			pass.DispatchThreads( clusters_count );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 30 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			const uint	warp_size2 = warp_size <= 32 ? warp_size : 32;
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V31" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgIn(		"un_Params",	params_buf );
			pass.ArgOut(	"un_Clusters",	cluster_buf );
			pass.ArgOut(	"un_Counters",	counter_buf );
			pass.Constant(	"iClusterCount",clusters_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )		pass.Constant(	"PERF_TEST", 1 );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size2 );
			pass.LocalSize( warp_size2, 1, warps_per_wg );
			pass.DispatchGroups( clusters_count.DivCeil(uint3( 1, 1, warps_per_wg )));
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 31 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V40" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgIn(		"un_Params",	params_buf );
			pass.ArgOut(	"un_Clusters",	cluster_buf );
			pass.ArgOut(	"un_Counters",	counter_buf );
			pass.Constant(	"iClusterCount",clusters_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )	pass.Constant(	"PERF_TEST", 1 );
			pass.LocalSize( local_size );
			pass.DispatchThreads( light_count );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfGreater( list_ver, 40-1 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<SceneGraphicsPass>	pass = scene_cl.AddGraphicsPass( "draw clusters" );
			{
				pass.AddPipeline( "*-drawClusters.ppln" );
				pass.Output( "out_Color",				rt_woit1,		RGBA32f(0.f) );
				pass.Output( "out_Weights",				rt_woit2,		RGBA32f(1.f) );
				pass.Output(							ds, 			DepthStencil( 1.f, 0 ));
				pass.Slider( "iZSlice",					-1,	max_clusters.z,	-1 );
				pass.Constant( "iListVersion",			list_ver );
				pass.Constant( "iClusterCount",			clusters_count );
				pass.Constant( "iMaxLightsPerCluster",	max_lights_per_cl );
				pass.Layer( ERenderLayer::Translucent );
			}
			pass.NextSubpass( "resolve" );
			{
				pass.AddPipeline( "*-resolve.ppln" );
				pass.Output(	"out_Color2",		rt_clusters );
				pass.Input(		"un_TexColor",		rt_woit1,	"out_Color" );
				pass.Input(		"un_TexWeights",	rt_woit2,	"out_Weights" );
				pass.Layer( ERenderLayer::PostProcess );
			}
			pass.EnableIfEqual( view_mode, 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",			rt,				RGBA32f(0.0) );
			pass.ArgIn(		"un_LightVolumesRT",	rt_lights,		Sampler_NearestClamp );
			pass.ArgIn(		"un_ClusterRT",			rt_clusters,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Clusters",			cluster_buf );
			pass.ArgIn(		"un_Params",			params_buf );
			pass.ArgIn(		"un_Counters",			counter_buf );
			pass.Slider(	"iBlend",				0.0,	1.0,	0.8 );
			pass.Constant(	"iClusterCount",		clusters_count );
			pass.Constant(	"iLightCount",			light_count );
			pass.Constant(	"iView",				view_mode );
			pass.Constant(  "iListVersion",			list_ver );
			pass.Constant(  "iMaxLightsPerCluster",	max_lights_per_cl );
		}

		if ( not perf_test )
		{
			RC<DynamicUInt2>	counters = DynamicUInt2();
			ReadBuffer( counters, counter_buf, "assignedLights_maxLightsPerCluster" );

			RC<DynamicFloat>	avr_lights	= counters.X().ToFloat().Div( clusters_total.ToFloat() );
			RC<DynamicFloat>	occupancy	= avr_lights.Div( float(warp_size) / 100.f );

			Label( counters.X(), 	"Assigned Lights" );
			Label( counters.Y(), 	"Max Lights per Cluster" );
			Label( avr_lights, 		"Avg Lights per Cluster" );
			Label( occupancy,		"Warp occupancy (%)" );
		}
		Label( time_ms,		"LightList (ms)" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_LIGHTS
	#include "InvocationID.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Frustum.glsl"

	void Main ()
	{
		if ( GetGlobalIndex() == 0 and (iLockFrustum == 0 or un_PerPass.frame == 0) )
		{
			un_Params.cameraClipPlanes 	= un_PerPass.camera.clipPlanes;
			un_Params.cameraPos			= un_PerPass.camera.pos;

			un_Params.cameraProj		= un_PerPass.camera.proj;
			un_Params.cameraView		= float3x3(un_PerPass.camera.view);

			Frustum	fr = Frustum_FromMatrix( un_PerPass.camera.viewProj, un_PerPass.camera.clipPlanes );	// world space frustum
			Frustum_ToCornerPoints( fr, OUT un_Params.cameraCornerPoints );
		}

		const int3		ipos	= GetGlobalCoord();

		if ( AnyGreaterEqual( ipos, iGridDim ))
			return;

		const int		idx		= ipos.x + ipos.y * iGridDim.x + ipos.z * iGridDim.x * iGridDim.y;
		const float		scale2	= 18.0 / float(iGridDim.x);
		const float		r_scale	= 0.2 * iRadiusPosScale.x;
		const float		p_scale	= 0.2 * iRadiusPosScale.y;
		const float3	pos		= (float3(ipos - int3(iGridDim) / 2) * scale2 + ToSNorm(DHash33( float3(ipos).zxy ))) * p_scale;
		const float		size	= 0.1 + DHash13( float3(ipos) ) * r_scale;
		const float3	color	= DHash33( float3(ipos).yxz * 33.33 + 11.11 );

		un_OmniLights.elements[idx].pos		= pos;
		un_OmniLights.elements[idx].radius	= size;
		un_OmniLights.elements[idx].color	= packUnorm4x8(float4( color, 1.0 ));
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V10
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	void  Main ()
	{
		const int3		cluster_id	= GetGlobalCoord();
		const int3		cluster_cnt	= int3(iClusterCount);

		if ( AnyGreaterEqual( cluster_id, cluster_cnt ))
			return;

		const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));
		const uint		capacity	= un_Clusters.elements[0].indices.length();

		Frustum			main_fr		= Frustum_FromMatrix( un_Params.cameraProj, un_Params.cameraClipPlanes );	// view space
		const float2	z_lerp		= Cluster_ZProjectionLerp_Log( float(cluster_id.z), float(cluster_cnt.z), un_Params.cameraClipPlanes );
		Frustum			cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );

		uint			count		= 0;

		for (uint i = 0; i < iLightCount; ++i)
		{
			const OmniLightData		light	= un_Lights.elements[i];
			const float3			vpos	= un_Params.cameraView * (light.pos - un_Params.cameraPos);
			const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space

			if ( Frustum_IsVisible( cluster_fr, sp ))
			{
				if ( count < capacity )
					un_Clusters.elements[ cluster_idx ].indices[ count ] = i;

				++count;
			}
		}

		count = Min( count, capacity );
		un_Clusters.elements[ cluster_idx ].count = count;

	  #ifndef PERF_TEST
		gl.AtomicAdd( INOUT un_Counters.assignedLights_maxLightsPerCluster[0], count );
		gl.AtomicMax( INOUT un_Counters.assignedLights_maxLightsPerCluster[1], count );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V11
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	void  Main () [[maximally_reconverges]]
	{
		sg_uniform const int3		cluster_id	= int3( GetGroupCoord().xy, GetGlobalCoord().z );
		sg_uniform const int3		cluster_cnt	= int3(iClusterCount);

		// uniform
		if ( AnyGreaterEqual( cluster_id, cluster_cnt ))
			return;

		sg_uniform const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));
		sg_uniform const uint		capacity	= un_Clusters.elements[0].indices.length();

		sg_uniform const Frustum	main_fr		= Frustum_FromMatrix( un_Params.cameraProj, un_Params.cameraClipPlanes );	// view space
		sg_uniform const float2		z_lerp		= Cluster_ZProjectionLerp_Log( float(cluster_id.z), float(cluster_cnt.z), un_Params.cameraClipPlanes );
		sg_uniform const Frustum	cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );

		sg_uniform uint				base		= 0;	// filled slots
		sg_uniform const uint		sg_size		= gl.subgroup.Size;
		sg_uniform const uint		num_iters	= (iLightCount + sg_size - 1) / sg_size;

		// uniform
		// loop must have uniform control flow to use subgroup ops inside
		for (uint k = 0; k < num_iters and base < capacity; ++k)
		{
			const uint	i		= k * sg_size + gl.subgroup.Index;
			uint		valid	= 0;

			// non-uniform
			if ( i < iLightCount )
			{
				const OmniLightData		light	= un_Lights.elements[i];
				const float3			vpos	= un_Params.cameraView * (light.pos - un_Params.cameraPos);
				const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space

				valid = Frustum_IsVisible( cluster_fr, sp ) ? 1u : 0u;
			}

			const uint		offset	= gl.subgroup.ExclusiveAdd( valid );
			sg_uniform uint	added	= gl.subgroup.Add( valid );
			const uint		pos		= base + offset;

			// non-uniform
			if ( valid != 0 and pos < capacity )
				un_Clusters.elements[ cluster_idx ].indices[ pos ] = i;

			base += added;
		}

		if ( gl.subgroup.Elect() )
		{
			uint	count = Min( base, capacity );
			un_Clusters.elements[ cluster_idx ].count = count;

		  #ifndef PERF_TEST
			gl.AtomicAdd( INOUT un_Counters.assignedLights_maxLightsPerCluster[0], count );
			gl.AtomicMax( INOUT un_Counters.assignedLights_maxLightsPerCluster[1], count );
		  #endif
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V20
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	void  Main ()
	{
		const uint		light_idx	= uint(GetGlobalIndex());

		if ( light_idx >= iLightCount )
			return;

		const OmniLightData	light	= un_Lights.elements[ light_idx ];
		const float3		vpos	= un_Params.cameraView * (light.pos - un_Params.cameraPos);
		const Sphere		sp		= Sphere_Create( vpos, light.radius );	// view space
		const float2		clip	= un_Params.cameraClipPlanes;
		const float			z_near	= clip.x;

		if ( vpos.z + light.radius < z_near )
			return;

		const int3		cluster_cnt	= int3(iClusterCount);
		int3			cluster_min	= int3(0);
		int3			cluster_max	= cluster_cnt;

		Rect			ndc_rect;

		if ( Sphere_FastProject2( sp, un_Params.cameraProj[0][0], un_Params.cameraProj[1][1], z_near, OUT ndc_rect ))
		{
			const Rect	uv_rect	= ToUNorm( ndc_rect );
			cluster_min.xy = Max( int2( Floor( Rect_Min( uv_rect ) * float2(cluster_cnt.xy) )), int2(0) );
			cluster_max.xy = Min( int2( Ceil(  Rect_Max( uv_rect ) * float2(cluster_cnt.xy) )), cluster_cnt.xy );
		}

		const float		z_min	= Max( vpos.z - light.radius, z_near );
		const float		z_max	= Max( vpos.z + light.radius, z_near );
		cluster_min.z = Max( int(Cluster_InvZProjection_Log( z_min, float(cluster_cnt.z), clip )), 0 );
		cluster_max.z = Min( int(Cluster_InvZProjection_Log( z_max, float(cluster_cnt.z), clip )) + 1, cluster_cnt.z );

		if ( AnyGreaterEqual( cluster_min, cluster_max ))
			return;

		const uint		capacity		= un_Clusters.elements[0].indices.length();
		const Frustum	main_fr			= Frustum_FromMatrix( un_Params.cameraProj, clip );	// view space
		uint			count			= 0;
		uint			max_per_cluster	= 0;

		for (int z = cluster_min.z; z < cluster_max.z; ++z)
		for (int y = cluster_min.y; y < cluster_max.y; ++y)
		for (int x = cluster_min.x; x < cluster_max.x; ++x)
		{
			const int3		cluster_id	= int3( x, y, z );
			const float2	z_lerp		= Cluster_ZProjectionLerp_Log( float(z), float(cluster_cnt.z), clip );
			const Frustum	cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );

			if ( ! Frustum_IsVisible( cluster_fr, sp ))
				continue;

			const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));
			const uint		pos			= gl.AtomicAdd( INOUT un_Clusters.elements[ cluster_idx ].count, 1 );

			if ( pos < capacity )
			{
				un_Clusters.elements[ cluster_idx ].indices[ pos ] = light_idx;
				++count;
				max_per_cluster = Max( max_per_cluster, pos + 1 );
			}
		}

	  #ifndef PERF_TEST
		gl.AtomicAdd( INOUT un_Counters.assignedLights_maxLightsPerCluster[0], count );
		gl.AtomicMax( INOUT un_Counters.assignedLights_maxLightsPerCluster[1], max_per_cluster );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V21
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	void  Main () [[maximally_reconverges]]
	{
		sg_uniform const uint		light_idx	= gl.subgroup.BroadcastFirst( GetGlobalIndex() / gl.subgroup.Size );

		// uniform
		if ( light_idx >= iLightCount )
			return;

		sg_uniform const OmniLightData	light	= un_Lights.elements[ light_idx ];
		sg_uniform const float3			vpos	= un_Params.cameraView * (light.pos - un_Params.cameraPos);
		sg_uniform const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space
		sg_uniform const float2			clip	= un_PerPass.camera.clipPlanes;
		sg_uniform const float			z_near	= clip.x;

		// uniform
		if ( vpos.z + light.radius < z_near )
			return;

		sg_uniform const int3	cluster_cnt	= int3(iClusterCount);
		sg_uniform int3			cluster_min	= int3(0);
		sg_uniform int3			cluster_max	= cluster_cnt;
		sg_uniform Rect			ndc_rect;

		// uniform
		if ( Sphere_FastProject2( sp, un_Params.cameraProj[0][0], un_Params.cameraProj[1][1], z_near, OUT ndc_rect ))
		{
			const Rect	uv_rect	= ToUNorm( ndc_rect );
			cluster_min.xy = Max( int2( Floor( Rect_Min( uv_rect ) * float2(cluster_cnt.xy) )), int2(0) );
			cluster_max.xy = Min( int2( Ceil(  Rect_Max( uv_rect ) * float2(cluster_cnt.xy) )), cluster_cnt.xy );
		}

		sg_uniform const float	z_min	= Max( vpos.z - light.radius, z_near );
		sg_uniform const float	z_max	= Max( vpos.z + light.radius, z_near );
		cluster_min.z = Max( int(Cluster_InvZProjection_Log( z_min, float(cluster_cnt.z), clip )), 0 );
		cluster_max.z = Min( int(Cluster_InvZProjection_Log( z_max, float(cluster_cnt.z), clip )) + 1, cluster_cnt.z );

		cluster_min = gl.subgroup.BroadcastFirst( cluster_min );
		cluster_max = gl.subgroup.BroadcastFirst( cluster_max );

		// uniform
		if ( AnyGreaterEqual( cluster_min, cluster_max ))
			return;

		sg_uniform const int3	cluster_size	= cluster_max - cluster_min;
		sg_uniform const uint	cluster_n		= uint( cluster_size.x * cluster_size.y * cluster_size.z );
		const uint				capacity		= un_Clusters.elements[0].indices.length();
		sg_uniform Frustum		main_fr			= Frustum_FromMatrix( un_Params.cameraProj, clip );	// view space
		uint					count			= 0;
		uint					max_per_cluster	= 0;

		// uniform
		for (uint i = gl.subgroup.Index; i < cluster_n; i += gl.subgroup.Size)
		{
			const int3		cluster_id	= cluster_min + IndexToVec3( int(i), cluster_size );
			const float2	z_lerp		= Cluster_ZProjectionLerp_Log( float(cluster_id.z), float(cluster_cnt.z), clip );
			const Frustum	cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );
			const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));

			// non-uniform
			if ( ! Frustum_IsVisible( cluster_fr, sp ))
				continue;

			uint	pos = gl.AtomicAdd( INOUT un_Clusters.elements[ cluster_idx ].count, 1 );

			// non-uniform
			if ( pos < capacity )
			{
				un_Clusters.elements[ cluster_idx ].indices[ pos ] = light_idx;
				++count;
				max_per_cluster = Max( max_per_cluster, pos + 1 );
			}
		}

	  #ifndef PERF_TEST
		count 			= gl.subgroup.Add( count );
		max_per_cluster	= gl.subgroup.Max( max_per_cluster );

		if ( gl.subgroup.Elect() )
		{
			gl.AtomicAdd( INOUT un_Counters.assignedLights_maxLightsPerCluster[0], count );
			gl.AtomicMax( INOUT un_Counters.assignedLights_maxLightsPerCluster[1], max_per_cluster );
		}
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V30
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	void  Main ()
	{
		const int3		cluster_id	= GetGlobalCoord();
		const int3		cluster_cnt	= int3(iClusterCount);

		if ( AnyGreaterEqual( cluster_id, cluster_cnt ))
			return;

		const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));

		Frustum			main_fr		= Frustum_FromMatrix( un_Params.cameraProj, un_Params.cameraClipPlanes );	// view space
		const float2	z_lerp		= Cluster_ZProjectionLerp_Log( float(cluster_id.z), float(cluster_cnt.z), un_Params.cameraClipPlanes );
		Frustum			cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );
		const uint		max_words	= un_Clusters.elements[0].indices.length();

		uint			count		= 0;

		for (uint i = 0; i < iLightCount; ++i)
		{
			const OmniLightData		light	= un_Lights.elements[i];
			const float3			vpos	= un_Params.cameraView * (light.pos - un_Params.cameraPos);
			const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space
			const uint				word	= i / 32;
			const uint				mask	= 1u << (i % 32);

			if ( Frustum_IsVisible( cluster_fr, sp ) and word < max_words )
			{
				un_Clusters.elements[ cluster_idx ].indices[ word ] |= mask;

			  #ifndef PERF_TEST
				++count;
			  #endif
			}
		}

	  #ifndef PERF_TEST
		un_Clusters.elements[ cluster_idx ].count = count;

		gl.AtomicAdd( INOUT un_Counters.assignedLights_maxLightsPerCluster[0], count );
		gl.AtomicMax( INOUT un_Counters.assignedLights_maxLightsPerCluster[1], count );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V31
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	void  Main () [[maximally_reconverges]]
	{
		sg_uniform const int3		cluster_id	= int3( GetGroupCoord().xy, GetGlobalCoord().z );
		sg_uniform const int3		cluster_cnt	= int3(iClusterCount);

		// uniform
		if ( AnyGreaterEqual( cluster_id, cluster_cnt ))
			return;

		sg_uniform const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));

		sg_uniform const Frustum	main_fr		= Frustum_FromMatrix( un_Params.cameraProj, un_Params.cameraClipPlanes );	// view space
		sg_uniform const float2		z_lerp		= Cluster_ZProjectionLerp_Log( float(cluster_id.z), float(cluster_cnt.z), un_Params.cameraClipPlanes );
		sg_uniform const Frustum	cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );
		sg_uniform const uint		max_words	= un_Clusters.elements[0].indices.length();

		sg_uniform const uint		sg_size		= gl.subgroup.Size;		// must be <= 32
		sg_uniform const uint		num_iters	= DivCeil( iLightCount, sg_size );
		uint						local_count = 0;

		// uniform
		// loop must have uniform control flow to use subgroup ops inside
		for (uint k = 0; k < num_iters; ++k)
		{
			const uint	i		= k * sg_size + gl.subgroup.Index;
			uint		mask	= 0;

			// non-uniform
			if ( i < iLightCount )
			{
				const OmniLightData		light	= un_Lights.elements[i];
				const float3			vpos	= un_Params.cameraView * (light.pos - un_Params.cameraPos);
				const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space

				if ( Frustum_IsVisible( cluster_fr, sp ) )
				{
					mask = 1u << (i % 32);
				  #ifndef PERF_TEST
					++local_count;
				  #endif
				}
			}

			// full subgroup calculate bits for single 'word'
			sg_uniform uint	word_mask = gl.subgroup.Or( mask );

			if ( gl.subgroup.Elect() )
			{
				sg_uniform const uint	word = i / 32;
				if ( word < max_words )
					un_Clusters.elements[ cluster_idx ].indices[ word ] |= word_mask;
			}
		}

	  #ifndef PERF_TEST
		sg_uniform uint	count = gl.subgroup.Add( local_count );

		if ( gl.subgroup.Elect() )
		{
			un_Clusters.elements[ cluster_idx ].count = count;

			gl.AtomicAdd( INOUT un_Counters.assignedLights_maxLightsPerCluster[0], count );
			gl.AtomicMax( INOUT un_Counters.assignedLights_maxLightsPerCluster[1], count );
		}
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V40
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	void  Main ()
	{
		const uint		light_idx		= uint(GetGlobalIndex());
		const uint		capacity		= un_Clusters.elements[0].indices.length();
		const uint		max_bit_count	= capacity * 32;

		if ( light_idx >= iLightCount or light_idx >= max_bit_count )
			return;

		const OmniLightData	light	= un_Lights.elements[ light_idx ];
		const float3		vpos	= un_Params.cameraView * (light.pos - un_Params.cameraPos);
		const Sphere		sp		= Sphere_Create( vpos, light.radius );	// view space
		const float2		clip	= un_Params.cameraClipPlanes;
		const float			z_near	= clip.x;

		if ( vpos.z + light.radius < z_near )
			return;

		const int3		cluster_cnt	= int3(iClusterCount);
		int3			cluster_min	= int3(0);
		int3			cluster_max	= cluster_cnt;

		Rect			ndc_rect;

		if ( Sphere_FastProject2( sp, un_Params.cameraProj[0][0], un_Params.cameraProj[1][1], z_near, OUT ndc_rect ))
		{
			const Rect	uv_rect	= ToUNorm( ndc_rect );
			cluster_min.xy = Max( int2( Floor( Rect_Min( uv_rect ) * float2(cluster_cnt.xy) )), int2(0) );
			cluster_max.xy = Min( int2( Ceil(  Rect_Max( uv_rect ) * float2(cluster_cnt.xy) )), cluster_cnt.xy );
		}

		const float		z_min	= Max( vpos.z - light.radius, z_near );
		const float		z_max	= Max( vpos.z + light.radius, z_near );
		cluster_min.z = Max( int(Cluster_InvZProjection_Log( z_min, float(cluster_cnt.z), clip )), 0 );
		cluster_max.z = Min( int(Cluster_InvZProjection_Log( z_max, float(cluster_cnt.z), clip )) + 1, cluster_cnt.z );

		if ( AnyGreaterEqual( cluster_min, cluster_max ))
			return;

		const uint		max_words		= un_Clusters.elements[0].indices.length();
		const Frustum	main_fr			= Frustum_FromMatrix( un_Params.cameraProj, clip );	// view space
		const uint		word			= light_idx / 32;
		const uint		mask			= 1u << (light_idx % 32);
		uint			assigned		= 0;
		uint			max_per_cluster	= 0;

		if ( word >= max_words )
			return;

		for (int z = cluster_min.z; z < cluster_max.z; ++z)
		for (int y = cluster_min.y; y < cluster_max.y; ++y)
		for (int x = cluster_min.x; x < cluster_max.x; ++x)
		{
			const int3		cluster_id	= int3( x, y, z );
			const float2	z_lerp		= Cluster_ZProjectionLerp_Log( float(z), float(cluster_cnt.z), clip );
			const Frustum	cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );
			const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));

			if ( ! Frustum_IsVisible( cluster_fr, sp ))
				continue;

			gl.AtomicOr( INOUT un_Clusters.elements[ cluster_idx ].indices[ word ], mask );

		  #ifndef PERF_TEST
			uint	pos	 = gl.AtomicAdd( INOUT un_Clusters.elements[ cluster_idx ].count, 1 );
			max_per_cluster = Max( max_per_cluster, pos + 1 );
			++assigned;
		  #endif
		}

	  #ifndef PERF_TEST
		gl.AtomicAdd( INOUT un_Counters.assignedLights_maxLightsPerCluster[0], assigned );
		gl.AtomicMax( INOUT un_Counters.assignedLights_maxLightsPerCluster[1], max_per_cluster );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Math.glsl"
	#include "Color.glsl"

	uint  ClusterBitCount (const uint clusterIdx)
	{
		const uint	word_n	= un_Clusters.elements[0].indices.length();
		uint		n		= 0;

		for (uint w = 0; w < word_n; ++w) {
			n += BitCount( un_Clusters.elements[ clusterIdx ].indices[ w ] );
		}
		return n;
	}

	uint  MaxLightsPerClusterColumn ()
	{
		const bool		use_bits	= iListVersion >= 30;
		const int2		cluster_xy	= int2(gl.FragCoord.xy * un_PerPass.invResolution * float2(iClusterCount.xy));
		uint			max_count	= 0;

		for (int cluster_z = 0; cluster_z < iClusterCount.z; ++cluster_z)
		{
			const int3		cluster_id	= int3( cluster_xy, cluster_z );
			const int		cluster_idx	= VecToIndex( cluster_id, int3(iClusterCount) );
			const uint		count		= use_bits ?
											ClusterBitCount( cluster_idx ) :
											Min( un_Clusters.elements[ cluster_idx ].count, un_Clusters.elements[0].indices.length() );
			max_count = Max( max_count, count );
		}
		return max_count;
	}


	void  Main ()
	{
		out_Color = gl.texture.Fetch( un_LightVolumesRT, int2(gl.FragCoord.xy), 0 );

		switch ( iView )
		{
			case 0 :	return;
			case 1 :
			{
				float4	heat = gl.texture.Fetch( un_ClusterRT, int2(gl.FragCoord.xy), 0 );
				out_Color = Lerp( out_Color, heat, iBlend );
				break;
			}
			case 2 :
			{
				uint	count = MaxLightsPerClusterColumn();
				float4	heat  = Rainbow2( 1.0 - float(count) / float(iMaxLightsPerCluster));
				if ( count == 0 )
					heat = float4(0.0);

				out_Color = Lerp( out_Color, heat, iBlend );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
