// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Forward+ rendering with clustered lights.

	related:
	* [project lights to clusters](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-LightVolToClusters.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define PUT_LIGHTS
#	define BUILD_LIGHT_LIST
#	define APPLY_LIGHTING
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	hdr_fmt			= EPixelFormat::RGBA16F;
		RC<DynamicDim>		rt_dim			= SurfaceSize();
		RC<Image>			rt				= Image( hdr_fmt, rt_dim );							rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( EPixelFormat::RGB10_A2_UNorm, rt_dim );	rt_col.Name( "Albedo" );
		RC<Image>			rt_rad			= Image( hdr_fmt, rt_dim );							rt.Name( "Radiance" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, rt_dim );	rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), rt_dim );			ds.Name( "Depth" );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			lights_buf		= Buffer();
		RC<Buffer>			params_buf		= Buffer();
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_lights	= Scene();
		RC<Scene>			scene_cones		= Scene();
		RC<Scene>			empty_scene		= Scene();
		RC<FPVCamera>		camera			= FPVCamera();

		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt>		light_count		= DynamicUInt();
		RC<DynamicUInt>		view_mode		= DynamicUInt();
		RC<DynamicUInt>		tech			= DynamicUInt();
		RC<DynamicFloat>	light_scale		= DynamicFloat();

		RC<DynamicInt3>		select_cluster	= DynamicInt3();
		RC<DynamicUInt3>	clusters_count	= DynamicUInt3();
		RC<DynamicUInt>		clusters_total	= clusters_count.Volume();
		RC<Buffer>			clusters_buf	= Buffer();

		const bool			has_sg_size_ctrl	= GetFeatureSet().hasSubgroupSizeControl();
		const uint			max_lights_per_tile	= 32;
		const uint			warp_size			= has_sg_size_ctrl ? GetFeatureSet().getMaxSubgroupSize() : GetSubgroupSize();
		const uint			warps_per_wg		= 4;
		const uint3			max_clusters		= uint3(40,20,64);
		const uint			local_size			= warp_size * warps_per_wg;

		// setup buffer layouts
		{
			obj_buf.ArrayLayout(
				"ObjectTransform",
				"	float3	position;"
				"	float3	scale;"
				"	uint	color;",
				obj_count );

			lights_buf.ArrayLayout(
				"LightObject",
				"	float3	position;"
				"	float	angle;"
				"	float3	dir;"
				"	float	height;"
				"	float2	atten;"		// linear, quadratic
				"	uint	color;"
				"	float	brightness;"
				"	uint	visible;",	// atomic
				light_count );

			clusters_buf.ArrayLayout(
				"ClusterLightList",
				"	uint	count;"		// atomic
				"	uint	indices["s+(max_lights_per_tile)+"];",
				clusters_total
			);

			params_buf.UseLayout(
				"Params",
				"	uint2	assignedLights_maxLightsPerCluster;"	// atomic
			);
		}

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 500.f );
			camera.FovY( 60.f );
			camera.ReverseZ( true );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( rt_dim );

			scene.Set( camera );
			scene_lights.Set( camera );
			scene_cones.Set( camera );
		}

		// create scene with buildings
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCube();

			RC<Buffer>		geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= mesh.IndexCount();
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( obj_count );
				geometry.Draw( cmd );

				scene.Add( geometry );
			}{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= 1;
				cmd.IndexBuffer( geom_data, "indices" );
				geometry.Draw( cmd );

				empty_scene.Add( geometry );
			}
		}

		// create scene with lights
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_LightObjs",	lights_buf );
			{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 4;
				cmd.InstanceCount( light_count );
				cmd.PipelineHint( "light_bulb" );
				geometry.Draw( cmd );
			}{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 5;
				cmd.InstanceCount( light_count );
				cmd.PipelineHint( "light_cone" );
				geometry.Draw( cmd );
			}
			scene_lights.Add( geometry );
		}

		// create scene with light cones
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCone( 12 );

			RC<Buffer>		geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_LightObjs",	lights_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( light_count );
			geometry.Draw( cmd );

			scene_cones.Add( geometry );
		}

		Slider( obj_count,		"ObjCount",		100,		400,				100 );
		Slider( light_count,	"LightCount",	100,		1024,				200 );
		Slider( view_mode,		"View",			0,			4,					0 );	// 0 - combined, 1 - color, 2 - light, 3 - overdraw, 4 - dbg cones
		Slider( clusters_count,	"ClusterDim",	uint3(1),	max_clusters,		uint3(16,8,24) );
		Slider( select_cluster,	"SelectCluster",int3(-1),	int3(max_clusters),	int3(-1) );
		Slider( light_scale,	"LightScale",	1.0,		100.0,				10.0 );
		Slider( tech,			"Technique",	0,			2,					1 );	// forward+, forward+ with DPP, deferred

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",		obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			RC<ComputePass>			pass = ComputePass( "", "PUT_LIGHTS; FIELD_VISIBLE" );
			pass.ArgInOut( "un_Lights",			lights_buf );
			pass.Slider(	"iMaxHeight",		1.0,	20.0,	20.0 );
			pass.LocalSize( 64 );
			pass.DispatchThreads( light_count );
		}{
			ClearBuffer( params_buf, 0 );
			ClearBuffer( clusters_buf, 0 );
		}{
			const uint	warp_size2 = warp_size <= 32 ? warp_size : 32;
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST" );	// v3.1
			pass.Set( camera );
			pass.ArgInOut(	"un_Lights",		lights_buf );
			pass.ArgOut(	"un_Clusters",		clusters_buf );
			pass.ArgInOut(	"un_Params",		params_buf );
			pass.Slider(	"iCullMode",		0,		2,		2 );
			pass.Slider( 	"iCullError",		-0.5f,	0.5f,	0.f );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.Constant(	"iLightCount",		light_count );
			pass.Constant(	"iSelectCluster",	select_cluster );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size2 );
			pass.LocalSize( warp_size2, 1, warps_per_wg );
			pass.DispatchGroups( clusters_count.DivCeil(uint3( 1, 1, warps_per_wg )));
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "forward+ dpp" );
			pass.AddPipeline( "samples/StreetLights/DepthPrePass.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/DepthPrePass.as)
			pass.Output(						ds,			DepthStencil(0.0, 0) );
			pass.EnableIfEqual( tech, 1 );
		}{
			RC<SceneGraphicsPass>	pass = empty_scene.AddGraphicsPass( "clear depth" );
			pass.AddPipeline( "samples/StreetLights/DepthPrePass.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/DepthPrePass.as)
			pass.Output(						ds,			DepthStencil(0.0, 0) );
			pass.EnableIfEqual( tech, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "forward+ opaque" );
			pass.AddPipeline( "*-opaque.ppln" );
			pass.Output(	"out_Color",		rt_rad,		RGBA32f(0.0) );			// radiance
			pass.Output(	"out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(						ds );
			pass.ArgIn(		"un_Lights",		lights_buf );
			pass.ArgIn(		"un_Clusters",		clusters_buf );
			pass.Constant(	"iLightScale",		light_scale );
			pass.Constant(	"iLightCount",		light_count );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.Constant(	"iView",			view_mode );
			pass.EnableIfLess( tech, 2 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "deferred opaque" );
			pass.AddPipeline( "samples/StreetLights/Opaque.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/Opaque.as)
			pass.Output(	"out_Color",		rt_col,		RGBA32f(0.0) );	// albedo
			pass.Output(	"out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(						ds,			DepthStencil(0.0, 0) );
			pass.EnableIfEqual( tech, 2 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "APPLY_LIGHTING" );
			pass.Set( camera );
			pass.Output(	"out_Color",		rt_rad,		RGBA32f(0.0) );
			pass.ArgIn(		"un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn(		"un_Depth",			ds,			Sampler_NearestClamp );
			pass.ArgIn(		"un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Lights",		lights_buf );
			pass.ArgIn(		"un_Clusters",		clusters_buf );
			pass.Constant(	"iLightScale",		light_scale );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.Constant(	"iLightCount",		light_count );
			pass.Constant(	"iView",			view_mode );
			pass.EnableIfEqual( tech, 2 );
		}{
			RC<SceneGraphicsPass>	pass = scene_cones.AddGraphicsPass( "dbg cones" );
			pass.AddPipeline( "TiledDeferred-dbg.ppln" );
			pass.Output(	"out_Color",		rt_rad );
			pass.Output(						ds,			DepthStencil(0.0, 0) );
			pass.Slider(	"iMode",			0,	2,	1 );
			pass.EnableIfEqual( view_mode, 4 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_Radiance",		rt_rad,		Sampler_NearestClamp );
			pass.ArgIn(		"un_Params",		params_buf );
			pass.Constant(	"iView",			view_mode );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.Constant(	"iSelectCluster",	select_cluster );
		}{
			RC<SceneGraphicsPass>	pass = scene_lights.AddGraphicsPass( "translucent" );
			pass.AddPipeline( "samples/StreetLights/LightBulb.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightBulb.as)
			pass.AddPipeline( "samples/StreetLights/LightCone.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightCone.as)
			pass.Output(	"out_Color",		rt );
			pass.OutputLS(						ds,			EAttachmentLoadOp::Load, EAttachmentStoreOp::None );	// read only
			pass.EnableIfLess( view_mode, 3 );
		}

		RC<DynamicUInt2>	params = DynamicUInt2();
		ReadBuffer( params, params_buf, "assignedLights_maxLightsPerCluster" );

		RC<DynamicFloat>	avr_lights = params.X().ToFloat().Div( clusters_total.ToFloat() );

		Label( params.X(), 	"Assigned Lights" );
		Label( params.Y(), 	"Max Lights per Cluster" );
		Label( avr_lights, 	"Avg Lights per Cluster" );

		RC<DynamicUInt2>	tile_size = rt_dim.XY().Div( clusters_count.XY() );
		Label( tile_size,	"Cluster XY dim (px)" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(PUT_OBJECTS) or defined(PUT_LIGHTS)

	#include "samples/StreetLights.as"

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST
	#include "SDF.glsl"
	#include "Cone.glsl"
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "Quaternion.glsl"
	#include "Intersectors.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	// see [FrustumCullingRevZ](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/geom-cull/FrustumCullingRevZ.as),
	// [LightVolToClusters](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-LightVolToClusters.as)
	bool  SpotLightVisible (sg_uniform const Frustum clusterFr, sg_uniform const Sphere clusterSp, const LightObject light)
	{
		float3		wpos		= light.position - un_PerPass.camera.pos;
		float3		vpos		= float3x3(un_PerPass.camera.view) * wpos;
		float3		vdir		= Normalize( float3x3(un_PerPass.camera.view) * light.dir );
		float		cull_err	= Clamp( 1.0 - iCullError, 0.0, 2.0 );

		Cone		vs_cone		= Cone_Create( vpos, vdir, light.angle * cull_err, light.height * cull_err );	// view space

		switch ( iCullMode )
		{
			case 0 :	return Frustum_IsConeVisible_v2( clusterFr, vs_cone );
			case 1 :	return Cone_Sphere_Intersects( vs_cone, clusterSp );
			case 2 :
			{
				Quat	rot	= QFrom2Normals( float3(0.0, -1.0, 0.0),	// origin direction (negative because of SDF_Cone implementation)
											 vs_cone.dir );				// new direction

				float3	pos	= clusterSp.center - vs_cone.origin;
						pos	= SDF_Rotate( pos, rot );

				float	d	= SDF_Cone( pos, SinCos( vs_cone.halfAngle ), vs_cone.height );
				return	d < clusterSp.radius;
			}
		}
	}


	void  Main () [[maximally_reconverges]]
	{
		sg_uniform const int3		cluster_id	= int3( GetGroupCoord().xy, GetGlobalCoord().z );
		sg_uniform const int3		cluster_cnt	= int3(iClusterCount);
		sg_uniform const bool		selected	= AllEqual( cluster_id, iSelectCluster );

		// uniform
		if ( AnyGreaterEqual( cluster_id, cluster_cnt ))
			return;

		sg_uniform const uint		cluster_idx	= uint( VecToIndex( cluster_id, cluster_cnt ));

		sg_uniform const Frustum	main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		sg_uniform const float2		z_lerp		= Cluster_ZProjectionLerp_Log( float(cluster_id.z), float(cluster_cnt.z), un_PerPass.camera.clipPlanes );
		sg_uniform const Frustum	cluster_fr	= Frustum_ToCluster( main_fr, cluster_id.xy, cluster_cnt.xy, z_lerp );
		sg_uniform Sphere			cluster_sp;

		// cluster geometry
		{
			float3	corners [8];
			Frustum_ToCornerPoints( cluster_fr, OUT corners );

			cluster_sp = FrustumCornerPoints_ToSphere( corners );
		}

		sg_uniform const uint		sg_size		= gl.subgroup.Size;		// must be <= 32
		sg_uniform const uint		num_iters	= DivCeil( iLightCount, sg_size );
		uint						local_count = 0;

		// uniform
		// loop must have uniform control flow to use subgroup ops inside
		for (uint k = 0; k < num_iters; ++k)
		{
			const uint	light_id	= k * sg_size + gl.subgroup.Index;
			uint		mask		= 0;

			// non-uniform
			if ( light_id < iLightCount )
			{
				if ( SpotLightVisible( cluster_fr, cluster_sp, un_Lights.elements[ light_id ] ))
				{
					mask = 1u << (light_id % 32);
					++local_count;

					if ( selected )
						gl.AtomicOr( INOUT un_Lights.elements[ light_id ].visible, 1 );
				}
			}

			// full subgroup calculate bits for single 'word'
			sg_uniform uint	word_mask = gl.subgroup.Or( mask );

			if ( gl.subgroup.Elect() )
			{
				sg_uniform const uint	word = light_id / 32;
				un_Clusters.elements[ cluster_idx ].indices[ word ] |= word_mask;
			}
		}

	  #ifndef PERF_TEST
		sg_uniform uint	count = gl.subgroup.Add( local_count );

		if ( gl.subgroup.Elect() )
		{
			un_Clusters.elements[ cluster_idx ].count = count;

			gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerCluster[0], count );
			gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerCluster[1], count );
		}
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef APPLY_LIGHTING
	#include "PBR.glsl"
	#include "Cone.glsl"
	#include "Normal.glsl"
	#include "Matrix.glsl"
	#include "ColorSpace.glsl"
	#include "LightClusters.glsl"

	struct GBuffer
	{
		float3	albedo;
		float	metalness;
		float3	normal;
		float	roughness;
		float3	wpos;
		float3	viewDir;
	};


	// same as [DeferredShading](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredShading-Volume.as)
	float3  ApplyLight (const GBuffer gbuf, const LightObject light)
	{
		float3	wpos		= light.position - un_PerPass.camera.pos;
		Cone	cone		= Cone_Create( wpos, light.dir, light.angle, light.height );
		float	range		= Cone_SlantLength( cone );
		float	outer_cos	= Cos( cone.halfAngle );
		float3	light_dir	= -light.dir;

		float3	dir_to_l	= Normalize( wpos - gbuf.wpos );
		float	cos_theta	= Dot( dir_to_l, light_dir );
		float	dist		= Distance( wpos, gbuf.wpos );

		// uniform
		if ( gl.subgroup.All( dist > range or cos_theta < outer_cos ))
			return float3(0.0);

		LightingResult	res = CookTorrance( gbuf.albedo, gbuf.albedo, dir_to_l, gbuf.viewDir, gbuf.normal, gbuf.roughness, gbuf.metalness );

		float	atten	= SpotAttenuation( float3(1.0, light.atten), cos_theta, dist, float2(1.0, outer_cos) );
		float3	col		= res.diffuse + res.specular;
		float3	l_col	= unpackUnorm4x8( light.color ).rgb * light.brightness;

		col *= l_col * atten;
		return col;
	}


	float4  Lighting (const GBuffer gbuf, float viewZ)
	{
		sg_uniform const uint	word_count	= DivCeil( iLightCount, 32 );
		float					cluster_z	= Cluster_InvZProjection_Log( viewZ, float(iClusterCount.z), un_PerPass.camera.clipPlanes );
								cluster_z	= Clamp( cluster_z, 0.0, float(iClusterCount.z-1) );
		float2					cluster_xy	= Min( (gl.FragCoord.xy * un_PerPass.invResolution) * float2(iClusterCount.xy), float2(iClusterCount.xy-1) );
		const uint3				cluster_id	= uint3( cluster_xy, cluster_z );
		const uint				cluster_idx	= VecToIndex( cluster_id, iClusterCount );

		float3	color	= float3(0.0);
		float	count	= 0.0;

		// uniform
		for (sg_uniform uint word_idx = 0; word_idx < word_count; ++word_idx)
		{
			const uint		mask		= un_Clusters.elements[ cluster_idx ].indices[ word_idx ];
			sg_uniform uint	merged_mask = gl.subgroup.BroadcastFirst( gl.subgroup.Or( mask ));

			// uniform
			for (; merged_mask != 0;)
			{
				sg_uniform uint			light_id	= ExtractLowBitIndex( INOUT merged_mask ) + word_idx * 32;
				sg_uniform LightObject	light		= un_Lights.elements[ light_id ];

				color += ApplyLight( gbuf, light );
				count += 1.0;
			}
		}
		return float4( color, count );
	}


	void Main () [[maximally_reconverges]]
	{
		const int2		coord	= int2(gl.FragCoord.xy);
		const float2	uv 		= gl.FragCoord.xy * un_PerPass.invResolution;

		float			depth	= gl.texture.Fetch( un_Depth, coord, 0 ).r;	// reverse Z
		float			viewZ	= FastUnProjectZ( un_PerPass.camera.proj, depth );
		float3			ambient	= RemoveSRGBCurve( float3(54.0, 61.0, 75.0) / 255.0 );

		GBuffer			gbuf;
		gbuf.albedo		= gl.texture.Fetch( un_Albedo, coord, 0 ).rgb;
		gbuf.normal		= ToSNorm( gl.texture.Fetch( un_Normal, coord, 0 ).rgb );	// world space
		gbuf.wpos		= UnProjectNDC( un_PerPass.camera.invViewProj, float3( ToSNorm(uv), depth ));
		gbuf.roughness	= 0.5;
		gbuf.metalness	= 0.8;
		gbuf.viewDir	= ViewDir( un_PerPass.camera.invViewProj, uv );

		float4	light_and_count = Lighting( gbuf, viewZ );

		switch ( iView )
		{
			case 0 :
				out_Color.rgb = ApplySRGBCurve( gbuf.albedo * (light_and_count.rgb * iLightScale + ambient) );
				out_Color.a   = 1.0;
				break;

			case 1 :
				out_Color = float4( gbuf.albedo, 1.0 );
				break;

			case 2 :
				out_Color = float4( light_and_count.rgb, 1.0 );
				if ( Any( IsNaN( light_and_count.rgb )))
					out_Color = float4(1.0, 0.0, 1.0, 1.0);
				break;

			case 3 :
				out_Color = float4( gbuf.albedo, light_and_count.a );
				break;

			case 4 :
				out_Color = float4( gbuf.albedo * 0.5, 1.0 );
				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Color.glsl"

	void Main ()
	{
		float2	uv			= gl.FragCoord.xy * un_PerPass.invResolution;
		float4	color		= gl.texture.Fetch( un_Radiance, int2(gl.FragCoord.xy), 0 );	// depends on iView
		float2	tile_cnt	= float2(iClusterCount.xy);

		switch ( iView )
		{
			case 0 :	// albedo * light
			case 1 :	// albedo
			case 2 :	// light
				out_Color = color;  break;

			case 3 :
			{
				float3	albedo	= color.rgb;
				float	count	= color.a;
				float4	color2	= Rainbow2( 1.0 - count / Max( float(un_Params.assignedLights_maxLightsPerCluster[1]), 1.0 ));
				if ( count == 0.0 )
					color2 = float4(0.0);

				out_Color = float4( Lerp( albedo, color2.rgb, 0.7 ), count );

				// cluster border
				float2	t = Fract( uv * float2(tile_cnt) ) / float2(tile_cnt) * un_PerPass.resolution.xy;
				if ( t.x < 1.25 or t.y < 1.25 )
					out_Color.rgb *= 0.2;
				break;
			}

			case 4 :
			{
				out_Color = color;

				// cluster border
				float2	tile	= uv * float2(tile_cnt);
				float2	t		= Fract( tile ) / float2(tile_cnt) * un_PerPass.resolution.xy;

				if ( AllEqual( int2(tile), iSelectCluster.xy ) or AllEqual( int2(tile-0.01), iSelectCluster.xy ))
				{
					if ( t.x < 1.25 or t.y < 1.25 )
					{
						out_Color.rgb *= 0.2;
						out_Color.rgb = LengthSq(out_Color.rgb) < 0.01 ? 1.0 - out_Color.rgb : out_Color.rgb;
					}
				}
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
