// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Forward+ rendering with clustered lights.

	related:
	* [project lights to clusters](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-LightVolToClusters.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_PARAMS
#	define PUT_OBJECTS
#	define PUT_LIGHTS
#	define PROJ_TO_CLUSTERS
#	define RESOLVE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	hdr_fmt			= EPixelFormat::RGBA16F;
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( hdr_fmt, dim );						rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( hdr_fmt, dim );						rt_col.Name( "HDR" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, dim );	rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );		ds.Name( "Depth" );
		RC<Image>			rt_ovd			= Image( EPixelFormat::R16F, dim );				rt_ovd.Name( "Overdraw" );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			lights_buf		= Buffer();
		RC<Buffer>			params_buf		= Buffer();
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_lights	= Scene();
		RC<Scene>			scene_cl		= Scene();

		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt>		light_count		= DynamicUInt();
		RC<DynamicUInt>		dbg_mode		= DynamicUInt();
		RC<DynamicUInt>		view_mode		= DynamicUInt();

		RC<DynamicUInt3>	clusters_count	= DynamicUInt3();
		RC<Buffer>			clusters_buf	= Buffer();
		const uint			warp_size		= GetSubgroupSize();
		const uint			warps_per_wg	= 4;

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
			"	float	brightness;",
			light_count );

		clusters_buf.ArrayLayout(
			"Cluster",
			"	uint	count;"
			"	uint	offset;",
			clusters_count.Volume()
		);

		params_buf.UseLayout(
			"Params",
			"	CameraData	camera;"
			"	uint		maxClusterTestsPerLight;"	// atomic
			"	uint		maxClustersPerLight;"		// atomic		// TODO: histogramm
		);

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 1.0f );
			camera.FovY( 60.f );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
			scene_lights.Set( camera );
		}

		// create scene with buildings
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCube();

			RC<Buffer>		geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( obj_count );
			cmd.PipelineHint( "opaque.LEqual" );
			geometry.Draw( cmd );

			scene.Add( geometry );
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

			scene_cl.Add( geometry );
		}

		Slider( obj_count,		"ObjCount",		100,	400,		100 );
		Slider( light_count,	"LightCount",	100,	1000,		200 );
		Slider( dbg_mode,		"DbgClusters",	0,		1 );
		Slider( view_mode,		"View",			0,		3,			0 );	// 0 - combined, 1 - color, 2 - light, 3 - overdraw
		Slider( clusters_count,	"ClusterDim",		uint3(1),	uint3(40,20,64),	uint3(6,4,32) );

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "INIT_PARAMS" );
			pass.Set( camera );
			pass.ArgInOut( "un_Params",			params_buf );
			pass.Constant( "iDbgMode",			dbg_mode );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",		obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			RC<ComputePass>			pass = ComputePass( "", "PUT_LIGHTS" );
			pass.ArgInOut( "un_Lights",			lights_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( light_count );
		}

		// lights to clusters
		// pass1: count max lights per cluster
		{
			ClearBuffer( clusters_buf, 0 );

			RC<ComputePass>			pass = ComputePass( "", "PROJ_TO_CLUSTERS" );
			pass.ArgInOut(	"un_Params",		params_buf );
			pass.ArgIn(		"un_Lights",		lights_buf );
			pass.ArgInOut(	"un_Clusters",		clusters_buf );
			pass.Constant(	"iClusterCount",	clusters_count );
			pass.SubgroupSize( warp_size );
			pass.LocalSize( warp_size * warps_per_wg );
			pass.DispatchGroups( light_count.DivCeil(warps_per_wg) );	// single light per warp
			pass.AddFlag( EPassFlags::Enable_ShaderAsserts );
		}
		// pass2: build light indices list
		{
		}
		// pass3: sort light indices
		{
		}

		{
		//	RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "depth pre-pass" );
		//	pass.AddPipeline( "samples/StreetLights/DPP.as" );			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/DPP.as)
		//	pass.Output( ds,	DepthStencil(0.0, 0) );		// write
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "forward+" );
			pass.AddPipeline( "samples/StreetLights/ForwardPlus.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/ForwardPlus.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output( "out_Overdraw",	rt_ovd,		RGBA32f(0.0) );
			pass.Output(					ds );						// read/write
			pass.ColorSelector( "iAmbient",		RGBA8u(54, 61, 75, 255) );
			pass.EnableIfEqual( dbg_mode,	0 );
		}{
		//	RC<SceneGraphicsPass>	draw = scene1.AddGraphicsPass( "draw clusters" );
		//	pass.AddPipeline( "samples/DrawClusters.as" );				// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DrawClusters.as)
		//	draw.Output( "out_Color",		rt );
		//	draw.Output(					ds );
		//	draw.Slider( "iClusterAlpha",	0.0,	1.0,	0.5 );
		//	pass.EnableIfEqual( dbg_mode,	1 );
		}{
			RC<Postprocess>			pass = Postprocess( "", "RESOLVE" );
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn(		"un_LightBuf",		rt_light,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Overdraw",		rt_ovd,		Sampler_NearestClamp );
			pass.Slider(	"iLightScale",		1.0,		100.0,	10.0 );
			pass.Constant(	"iView",			view_mode );
			pass.Constant(	"iMaxOverdraw",		light_count );
		}{
			RC<SceneGraphicsPass>	pass = scene_lights.AddGraphicsPass( "translucent" );
			pass.AddPipeline( "samples/StreetLights/LightBulb.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightBulb.as)
			pass.AddPipeline( "samples/StreetLights/LightCone.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightCone.as)
			pass.Output( "out_Color",		rt );
			pass.OutputLS(					ds,			EAttachmentLoadOp::Load, EAttachmentStoreOp::None );
			pass.Slider( "iLightSize",		0.01,	1.0,	1.0 );
			pass.EnableIfLess( view_mode, 3 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT_PARAMS

	void  Main ()
	{
		if ( iDbgMode == 0 )
		{
			un_Params.camera	= un_PerPass.camera;
		}

		un_Params.maxClusterTestsPerLight	= 0;
		un_Params.maxClustersPerLight		= 0;
	}

#endif
//-----------------------------------------------------------------------------
#if defined(PUT_OBJECTS) or defined(PUT_LIGHTS)

	#define GROUND_Y	2.0

#endif
//-----------------------------------------------------------------------------
#ifdef PUT_OBJECTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		ObjectTransform		obj;
		const uint			idx				= GetGlobalIndex();
		const uint			street_cnt		= 4;
		const uint			street_idx		= idx % street_cnt;
		const uint			building_idx	= idx / street_cnt;
		const float2		uv				= float2( street_idx, building_idx ) / float2( street_cnt, 1 );

		if ( idx == 0 )
		{
			// floor
			obj.position	= float3(0.0, GROUND_Y + 1.0, 0.0);
			obj.scale		= float2(1000.0, 1.0).xyx;
			obj.color		= packUnorm4x8( float4(0.2) );
			un_Objects.elements[idx] = obj;
			return;
		}

		if ( idx >= un_Objects.elements.length() )
			return;

		obj.position.x	= (ToSNorm( uv.x ) + 0.25) * 20.0;
		obj.position.y	= GROUND_Y;
		obj.position.z	= uv.y - 5.0;

		obj.scale.x		= 0.5 + DHash12( uv.yx * 111.0 ) * 1.0;
		obj.scale.y		= 0.8 + DHash12( uv * 444.0 ) * 5.0;
		obj.scale.z		= 2.0;

		obj.position.y	-= obj.scale.y;
		obj.position.z	*= obj.scale.z * 2.0;

		obj.color		= packUnorm4x8( float4( DHash32( uv * 333.0 ), 1.0 ));

		un_Objects.elements[idx] = obj;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PUT_LIGHTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "Transform.glsl"

	void  GenLight (out LightObject light, const uint idx)
	{
		const uint		street_cnt		= 4u * 2;	// 2 light lines per street
		const uint		street_idx		= idx % street_cnt;
		const uint		building_idx	= idx / street_cnt;
		const float2	uv				= float2( street_idx, building_idx ) / float2( street_cnt, 1.0 );

		light.position.x	= (ToSNorm( uv.x ) + 0.125) * 20.0 + ToSNorm( DHash12( uv.yx * 123.0 )) * 0.5;
		light.position.y	= -1.5 + DHash12( uv * 77.0 );
		light.position.z	= uv.y * 2.0 - 5.0;

		light.dir			= Normalize( float3(0.0, 1.0, 0.0) + float3( ToSNorm( DHash22( uv * 222.0 )), 0.0 ).xzy );

		light.height		= Abs( (GROUND_Y + 2.0 - light.position.y) / light.dir.y );
		light.angle			= float_Pi * 0.5 * (DHash12( uv * 67.2 ) * 0.5 + 0.5);

		light.color			= packUnorm4x8( Rainbow( DHash12( uv * 333.0 ) * 0.2 + 0.1 ));
		light.brightness	= 1.0 + DHash12( uv * 99.0 );
		light.atten			= float2( 0.027, 0.055 );

		un_Lights.elements[ idx ] = light;
	}


	void Main ()
	{
		const uint	idx = GetGlobalIndex();

		if ( idx >= un_Lights.elements.length() )
			return;

		LightObject light;
		GenLight( OUT light, idx );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PROJ_TO_CLUSTERS
	#include "Cone.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"
	#include "LightClusters.glsl"

	#define camera		un_Params.camera


	void  Main ()
	{
		// read light geometry
		sg_uniform const uint			light_idx	= gl.subgroup.BroadcastFirst( GetGlobalIndex() / gl.subgroup.Size );
		sg_uniform const LightObject	light_obj	= un_Lights.elements[ light_idx ];
		sg_uniform const Cone			cone		= Cone_Create(	float3x3(camera.view) * light_obj.position,  // view space
																	float3x3(camera.view) * light_obj.dir,
																	light_obj.angle, light_obj.height );

		// project to clusters
		sg_uniform uint3	cl_min, cl_max;
		sg_uniform uint		cl_count;
		{
			// calculate bounding sphere
			Sphere		sphere		= Cone_ToBoundingSphere( cone );  // view space

			// project to NDC
			Rect		scr_rect	= Rect_ToUNorm( Sphere_FastProject( sphere, camera.proj[0][0], camera.proj[1][1] ));
			float		z_min		= sphere.center.z - sphere.radius;	// view space
			float		z_max		= sphere.center.z + sphere.radius;

			float3		f_cluster_count	= float3(iClusterCount);
			float		z_near			= camera.clipPlanes.x;
			float		z_far			= camera.clipPlanes.y;

			if ( sphere.center.z - sphere.radius < z_near )
			{
				cl_min.xy = uint2(0);
				cl_max.xy = iClusterCount.xy;
			}
			else
			{
				cl_min.xy = uint2(Max( Floor( Rect_Min(scr_rect) * f_cluster_count.xy ), float2(0.0) ));
				cl_max.xy = uint2( Ceil(  Rect_Max(scr_rect) * f_cluster_count.xy ));
			}

			cl_min.z = uint( Max( Cluster_InvZProjection_Log( z_min, f_cluster_count.z, camera.clipPlanes ), 0.0 ));
			cl_max.z = uint( Max( Cluster_InvZProjection_Log( z_max, f_cluster_count.z, camera.clipPlanes ), 0.0 ));

			cl_min	= gl.subgroup.BroadcastFirst( Min( cl_min, iClusterCount-1 ));
			cl_max	= gl.subgroup.BroadcastFirst( Min( cl_max, iClusterCount - cl_min ));

			sg_uniform uint3	cl_size	= cl_max - cl_min;

			cl_count = cl_size.x * cl_size.y * cl_size.z;
			if ( cl_count == 0 )
				return;

			if ( gl.subgroup.Elect() )
				AtomicMax( INOUT un_Params.maxClusterTestsPerLight, cl_count );
		}


		// parallel work to threads in warp
		Frustum		main_frustum	= Frustum_Create( un_Params.frustumPlanes );	// view space
		uint		isec_clusters	= 0;

		for (uint i = gl.subgroup.Index; i < cl_count; i += gl.subgroup.Size)
		{
			uint3		cluster_idx		= IndexToVec3( i, iClusterCount ) + cl_min;	// non-uniform	// TODO: use fp or POT version
			uint		cl_idx			= VecToIndex( cluster_idx );

			Frustum		cl_frustum		= CreateClusterFrustum( main_frustum, cluster_idx, iClusterCount );
			bool		visible			= Frustum_IsConeVisible_v2( cl_frustum, cone );

			if ( visible							and
				 AllLess( cluster_idx, iClusterCount ))
			{
				uint	pos = AtomicAdd( INOUT un_Clusters.elements[ cl_idx ].count, 1 );

				ASSERT( pos < un_Clusters.elements[ cl_idx ].indices.length() );

				if ( pos < un_Clusters.elements[ cl_idx ].indices.length() )
				{
					un_Clusters.elements[ cl_idx ].indices[ pos ] = light_idx;
					++isec_clusters;
				}
			}
		}

		// stats
		isec_clusters = gl.subgroup.Add( isec_clusters );
		if ( gl.subgroup.Elect() )
			AtomicMax( INOUT un_Params.maxClustersPerLight, isec_clusters );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RESOLVE
	#include "Color.glsl"
	#include "ColorSpace.glsl"
	#include "ToneMapping.glsl"

	void Main ()
	{
		float3	light	= gl.texture.Fetch( un_LightBuf, int2(gl.FragCoord.xy), 0 ).rgb * iLightScale;
		float3	albedo	= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float3	ambient	= RemoveSRGBCurve( iAmbient.rgb );

		switch ( iView )
		{
			case 0 :
				out_Color = ApplySRGBCurve( float4( albedo * (light + ambient), 1.0 ));
				break;

			case 1 :
				out_Color = float4( albedo, 1.0 );
				break;

			case 2 :
				out_Color = float4( light, 1.0 );
				if ( Any( IsNaN( light )))
					out_Color = float4(1.0, 0.0, 1.0, 1.0);
				break;

			case 3 :
			{
				float	cnt = gl.texture.Fetch( un_Overdraw, int2(gl.FragCoord.xy), 0 ).r * iLightScale;
				out_Color = Rainbow( cnt / iMaxOverdraw );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
