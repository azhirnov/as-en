// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compare performance of light list building for tiles.

	ListMode switch:
	1.0 - one tile per thread, check intersection with all lights
	1.1 - one tile per warp, same as 1.0, better thread coherency in warp

	2.0 - one light per thread, project light volume to screen and check intersection with tiles in range
	2.1 - one light per warp, same as 2.0, better thread coherency in warp

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
//#	define PERF_TEST
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		rt_dim			= SurfaceSize();
		RC<Image>			rt_lights		= Image( EPixelFormat::RGBA8_UNorm, rt_dim );
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, rt_dim );
		RC<Image>			ds				= Image( Supported_DepthFormat(), rt_dim );

		RC<FPVCamera>		camera			= FPVCamera();
		RC<Scene>			scene			= Scene();
		RC<Buffer>			lights_buf		= Buffer();
		RC<Buffer>			tiles_buf		= Buffer();
		RC<Buffer>			params_buf		= Buffer();

		RC<DynamicUInt2>	tile_count		= DynamicUInt2();
		RC<DynamicUInt>		tile_total		= tile_count.Area();
		RC<DynamicUInt>		view_mode		= DynamicUInt();
		RC<DynamicUInt>		list_mode		= DynamicUInt();
		RC<DynamicUInt>		subgroup_opt	= DynamicUInt();
		RC<DynamicUInt>		list_ver		= list_mode.Mul( 10 ).Add( subgroup_opt );

		RC<DynamicUInt3>	grid_pot		= DynamicUInt3();
		RC<DynamicUInt3>	grid_dim		= grid_pot.Exp2();
		RC<DynamicUInt>		light_count		= grid_dim.Volume();

		RC<DynamicUInt>		repeat			= DynamicUInt();
		RC<DynamicFloat>	time			= DynamicFloat();
		RC<DynamicFloat>	time_ms			= time.Mul( 1000.0f );

		// 1024 lights = uint[32] for bitfield, otherwise some lights will be missed
		const uint			max_lights_per_tile	= 32*4;
		const bool			perf_test			= false;
		RC<FeatureSet>		fs					= GetFeatureSet();
		const bool			has_sg_size_ctrl	= fs.hasSubgroupSizeControl();
		const uint			warp_size			= has_sg_size_ctrl ? fs.getMaxSubgroupSize() : GetSubgroupSize();
		const uint			warps_per_wg		= 4;
		const uint			local_size			= warp_size * warps_per_wg;

		Slider( grid_pot,		"LightGridPOT",		uint3(1),	uint3(6), 		uint3(3) );
		Slider( tile_count,		"TileCount",		uint2(4),	uint2(128),	uint2(20,12) );
		Slider( view_mode,		"View",				0,			2,			2 );
		Slider( list_mode,		"ListMode",			1,			4 );
		Slider( subgroup_opt,	"SubgroupOpt",		0,			1 );
		Slider( repeat,			"Repeat",			1,			32 );

		Label( tile_total,	"Total Tiles" );
		Label( light_count,	"Total Lights" );

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -3.f));

			scene.Set( camera );
		}

		// setup buffer layouts
		{
			lights_buf.ArrayLayout(
				"OmniLightData",
				"	float3 	pos;"
				"	float	radius;"
				"	uint 	color;",
				light_count );

			tiles_buf.ArrayLayout(
				"TileLightList",
				"	uint	count;"		// atomic
				"	uint	indices["s+(max_lights_per_tile)+"];",	// bitfield for 'list_ver >= 30'
				tile_total );

			params_buf.UseLayout(
				"Params",
				"	uint2	assignedLights_maxLightsPerTile;"	// atomic
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

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_LIGHTS" );
			pass.ArgOut(	"un_OmniLights",	lights_buf );
			pass.Constant(	"iGridDim",			grid_dim );
			pass.Slider( 	"iRadiusPosScale",	float2(0.2),	float2(4.0),	float2(1.0) );
			pass.LocalSize( 8, 8, 8 );
			pass.DispatchThreads( grid_dim );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw lights" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color", 	rt_lights, 	RGBA32f(0.f) );
			pass.Output( 				ds, 		DepthStencil( 1.f, 0 ));
		}{
			ClearBuffer( params_buf, 0 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V10" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )	pass.Constant(	"PERF_TEST", 1 );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( tile_count );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 10 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V11" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )		pass.Constant(	"PERF_TEST", 1 );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
			pass.LocalSize( warp_size, warps_per_wg );
			pass.DispatchGroups( tile_count.DivCeil(uint2( 1, warps_per_wg )));
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 11 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V20" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
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
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )		pass.Constant(	"PERF_TEST", 1 );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
			pass.LocalSize( warp_size * warps_per_wg );
			pass.DispatchGroups( light_count.DivCeil( warps_per_wg ));
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 21 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V30" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )	pass.Constant(	"PERF_TEST", 1 );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( tile_count );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 30 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			const uint	warp_size2 = warp_size <= 32 ? warp_size : 32;
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V31" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )		pass.Constant(	"PERF_TEST", 1 );
			if ( has_sg_size_ctrl )	pass.SubgroupSize( warp_size2 );
			pass.LocalSize( warp_size2, warps_per_wg );
			pass.DispatchGroups( tile_count.DivCeil(uint2( 1, warps_per_wg )));
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 31 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V40" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( perf_test )	pass.Constant(	"PERF_TEST", 1 );
			pass.LocalSize( local_size );
			pass.DispatchThreads( light_count );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfGreater( list_ver, 40-1 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",			rt,			RGBA32f(0.0) );
			pass.ArgIn(		"un_LightVolumesRT",	rt_lights,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Tiles",				tiles_buf );
			pass.ArgIn(		"un_Lights",			lights_buf );
			pass.ArgIn(		"un_Params",			params_buf );
			pass.Slider(	"iBlend",				0.0,	1.0,	0.8 );
			pass.Constant(	"iTileCount",			tile_count );
			pass.Constant(	"iLightCount",			light_count );
			pass.Constant(	"iView",				view_mode );
			pass.Constant(	"iListVersion",			list_ver );
		}

		if ( not perf_test )
		{
			RC<DynamicUInt2>	params = DynamicUInt2();
			ReadBuffer( params, params_buf, "assignedLights_maxLightsPerTile" );

			RC<DynamicFloat>	avr_lights = params.X().ToFloat().Div( tile_total.ToFloat() );

			Label( params.X(), 	"Assigned Lights" );
			Label( params.Y(), 	"Max Lights per Tile" );
			Label( avr_lights, 	"Avg Lights per Tile" );
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

	void Main ()
	{
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

	void  Main ()
	{
		const int2		tile_id		= GetGlobalCoord().xy;
		const int2		tile_cnt	= int2(iTileCount);

		if ( AnyGreaterEqual( tile_id, tile_cnt ))
			return;

		const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );
		const uint		capacity	= un_Tiles.elements[ tile_idx ].indices.length();

		Frustum			main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		Frustum			tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );

		uint			count		= 0;

		for (uint i = 0; i < iLightCount; ++i)
		{
			const OmniLightData		light	= un_Lights.elements[i];
			const float3			vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
			const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space

			if ( Frustum_IsVisible( tile_fr, sp ))
			{
				if ( count < capacity )
					un_Tiles.elements[ tile_idx ].indices[ count ] = i;

				++count;
			}
		}

		count = Min( count, capacity );
		un_Tiles.elements[ tile_idx ].count = count;

	  #ifndef PERF_TEST
		gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
		gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], count );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V11
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"

	void  Main () [[maximally_reconverges]]
	{
		sg_uniform const int2		tile_id		= int2( GetGroupCoord().x, GetGlobalCoord().y );
		sg_uniform const int2		tile_cnt	= int2(iTileCount);

		// uniform
		if ( AnyGreaterEqual( tile_id, tile_cnt ))
			return;

		sg_uniform const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );
		sg_uniform const uint		capacity	= un_Tiles.elements[0].indices.length();

		sg_uniform const Frustum	main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		sg_uniform const Frustum	tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );

		sg_uniform uint				base		= 0;	// filled slots
		sg_uniform const uint		sg_size		= gl.subgroup.Size;
		sg_uniform const uint		num_iters	= DivCeil( iLightCount, sg_size );

		// uniform
		// loop must have uniform control flow to use subgroup ops inside
		for (uint k = 0; k < num_iters; ++k)
		{
			const uint	i		= k * sg_size + gl.subgroup.Index;
			uint		valid	= 0;

			// non-uniform
			if ( i < iLightCount )
			{
				const OmniLightData	light	= un_Lights.elements[i];
				const float3		vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
				const Sphere		sp		= Sphere_Create( vpos, light.radius );	// view space

				valid = Frustum_IsVisible( tile_fr, sp ) ? 1u : 0u;
			}

			const uint	offset	= gl.subgroup.ExclusiveAdd( valid );
			const uint	added	= gl.subgroup.Add( valid );
			const uint	pos		= base + offset;

			if ( valid != 0 and pos < capacity )
				un_Tiles.elements[ tile_idx ].indices[ pos ] = i;

			base += added;
		}

		if ( gl.subgroup.Elect() )
		{
			uint	count = Min( base, capacity );
			un_Tiles.elements[ tile_idx ].count = count;

		  #ifndef PERF_TEST
			gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
			gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], count );
		  #endif
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V20
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const uint		light_idx	= uint(GetGlobalIndex());

		if ( light_idx >= iLightCount )
			return;

		const OmniLightData	light	= un_Lights.elements[ light_idx ];
		const float3		vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
		const Sphere		sp		= Sphere_Create( vpos, light.radius );	// view space
		const float			z_near	= un_PerPass.camera.clipPlanes.x;

		if ( vpos.z + light.radius < z_near )
			return;

		const int2		tile_cnt	= int2(iTileCount);
		int2			tile_min	= int2(0);
		int2			tile_max	= tile_cnt;

		Rect			ndc_rect;

		// returns 'false' if culled by near plane
		if ( Sphere_FastProject2( sp, un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1], z_near, OUT ndc_rect ))
		{
			const Rect	uv_rect	= ToUNorm( ndc_rect );
			tile_min = Max( int2( Floor( Rect_Min( uv_rect ) * float2(tile_cnt) )), int2(0) );
			tile_max = Min( int2( Ceil(  Rect_Max( uv_rect ) * float2(tile_cnt) )), tile_cnt );
		}

		if ( AnyGreaterEqual( tile_min, tile_max ))
			return;

		Frustum		main_fr			= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		uint		count			= 0;
		uint		max_per_tile	= 0;

		for (int y = tile_min.y; y < tile_max.y; ++y)
		for (int x = tile_min.x; x < tile_max.x; ++x)
		{
			const Frustum	tile_fr		= Frustum_ToTile( main_fr, int2(x, y), tile_cnt );

			if ( ! Frustum_IsVisible( tile_fr, sp ))
				continue;

			const uint		tile_idx	= uint( x + y * tile_cnt.x );
			const uint		capacity	= un_Tiles.elements[0].indices.length();
			const uint		pos			= gl.AtomicAdd( INOUT un_Tiles.elements[ tile_idx ].count, 1 );

			if ( pos < capacity )
			{
				un_Tiles.elements[ tile_idx ].indices[ pos ] = light_idx;
				++count;
				max_per_tile = Max( max_per_tile, pos + 1 );
			}
		}

	  #ifndef PERF_TEST
		gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
		gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], max_per_tile );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V21
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"

	void  Main () [[maximally_reconverges]]
	{
		sg_uniform const uint		light_idx	= gl.subgroup.BroadcastFirst( GetGlobalIndex() / gl.subgroup.Size );

		// uniform control flow
		if ( light_idx >= iLightCount )
			return;

		sg_uniform const OmniLightData	light	= un_Lights.elements[ light_idx ];
		sg_uniform const float3			vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
		sg_uniform const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space
		sg_uniform const float			z_near	= un_PerPass.camera.clipPlanes.x;

		// uniform control flow
		if ( vpos.z + light.radius < z_near )
			return;

		sg_uniform const int2	tile_cnt	= int2(iTileCount);
		sg_uniform int2			tile_min	= int2(0);
		sg_uniform int2			tile_max	= tile_cnt;
		sg_uniform Rect			ndc_rect;

		// uniform
		if ( Sphere_FastProject2( sp, un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1], z_near, OUT ndc_rect ))
		{
			const Rect	uv_rect	= ToUNorm( ndc_rect );
			tile_min = Max( int2( Floor( Rect_Min( uv_rect ) * float2(tile_cnt) )), int2(0) );
			tile_max = Min( int2( Ceil(  Rect_Max( uv_rect ) * float2(tile_cnt) )), tile_cnt );
		}

		tile_min = gl.subgroup.BroadcastFirst( tile_min );
		tile_max = gl.subgroup.BroadcastFirst( tile_max );

		// uniform control flow
		if ( AnyGreaterEqual( tile_min, tile_max ))
			return;

		sg_uniform const int2	tile_size		= tile_max - tile_min;
		sg_uniform const uint	tile_n			= uint( tile_size.x * tile_size.y );
		sg_uniform Frustum		main_fr			= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		uint					count			= 0;
		uint					max_per_tile	= 0;

		// non-uniform
		for (uint i = gl.subgroup.Index; i < tile_n; i += gl.subgroup.Size)
		{
			const int2		tile_id		= tile_min + int2(IndexToVec2( i, uint(tile_size.x) ));
			const Frustum	tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );

			// non-uniform
			if ( ! Frustum_IsVisible( tile_fr, sp ))
				continue;

			const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );
			const uint		capacity	= un_Tiles.elements[0].indices.length();
			const uint		pos			= gl.AtomicAdd( INOUT un_Tiles.elements[ tile_idx ].count, 1 );

			// non-uniform
			if ( pos < capacity )
			{
				un_Tiles.elements[ tile_idx ].indices[ pos ] = light_idx;
				++count;
				max_per_tile = Max( max_per_tile, pos + 1 );
			}
		}

	  #ifndef PERF_TEST
		count 		 = gl.subgroup.Add( count );
		max_per_tile = gl.subgroup.Max( max_per_tile );

		if ( gl.subgroup.Elect() )
		{
			gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
			gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], max_per_tile );
		}
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V30
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const int2		tile_id		= GetGlobalCoord().xy;
		const int2		tile_cnt	= int2(iTileCount);

		if ( AnyGreaterEqual( tile_id, tile_cnt ))
			return;

		const uint		max_words	= un_Tiles.elements[0].indices.length();
		const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );

		Frustum			main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		Frustum			tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );

		uint			count		= 0;

		for (uint i = 0; i < iLightCount; ++i)
		{
			const OmniLightData		light	= un_Lights.elements[i];
			const float3			vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
			const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space
			const uint				word	= i / 32;
			const uint				mask	= 1u << (i % 32);

			if ( Frustum_IsVisible( tile_fr, sp ) and word < max_words )
			{
				un_Tiles.elements[ tile_idx ].indices[ word ] |= mask;

			  #ifndef PERF_TEST
				++count;
			  #endif
			}
		}

	  #ifndef PERF_TEST
		un_Tiles.elements[ tile_idx ].count = count;

		gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
		gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], count );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V31
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"

	void  Main () [[maximally_reconverges]]
	{
		sg_uniform const int2		tile_id		= int2( GetGroupCoord().x, GetGlobalCoord().y );
		sg_uniform const int2		tile_cnt	= int2(iTileCount);

		// uniform
		if ( AnyGreaterEqual( tile_id, tile_cnt ))
			return;

		sg_uniform const uint		max_words	= un_Tiles.elements[0].indices.length();
		sg_uniform const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );

		sg_uniform const Frustum	main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		sg_uniform const Frustum	tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );

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
				const OmniLightData	light	= un_Lights.elements[i];
				const float3		vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
				const Sphere		sp		= Sphere_Create( vpos, light.radius );	// view space

				if ( Frustum_IsVisible( tile_fr, sp ))
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
					un_Tiles.elements[ tile_idx ].indices[ word ] |= word_mask;
			}
		}

	  #ifndef PERF_TEST
		sg_uniform uint	count = gl.subgroup.Add( local_count );

		if ( gl.subgroup.Elect() )
		{
			un_Tiles.elements[ tile_idx ].count = count;

			gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
			gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], count );
		}
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST_V40
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const uint		light_idx		= uint(GetGlobalIndex());
		const uint		capacity		= un_Tiles.elements[0].indices.length();
		const uint		max_bit_count	= capacity * 32;

		if ( light_idx >= iLightCount or light_idx >= max_bit_count )
			return;

		const OmniLightData	light	= un_Lights.elements[ light_idx ];
		const float3		vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
		const Sphere		sp		= Sphere_Create( vpos, light.radius );	// view space
		const float			z_near	= un_PerPass.camera.clipPlanes.x;

		if ( vpos.z + light.radius < z_near )
			return;

		const int2		tile_cnt	= int2(iTileCount);
		int2			tile_min	= int2(0);
		int2			tile_max	= tile_cnt;
		Rect			ndc_rect;

		// returns 'false' if culled by near plane
		if ( Sphere_FastProject2( sp, un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1], z_near, OUT ndc_rect ))
		{
			const Rect	uv_rect	= ToUNorm( ndc_rect );
			tile_min = Max( int2( Floor( Rect_Min( uv_rect ) * float2(tile_cnt) )), int2(0) );
			tile_max = Min( int2( Ceil(  Rect_Max( uv_rect ) * float2(tile_cnt) )), tile_cnt );
		}

		if ( AnyGreaterEqual( tile_min, tile_max ))
			return;

		const uint		max_words	= un_Tiles.elements[0].indices.length();
		const Frustum	main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );	// view space
		const uint		word		= light_idx / 32;
		const uint		mask		= 1u << (light_idx % 32);
		uint			assigned	= 0;
		uint			max_per_tile= 0;

		if ( word >= max_words )
			return;

		for (int y = tile_min.y; y < tile_max.y; ++y)
		for (int x = tile_min.x; x < tile_max.x; ++x)
		{
			const Frustum	tile_fr		= Frustum_ToTile( main_fr, int2(x, y), tile_cnt );
			const uint		tile_idx	= uint( x + y * tile_cnt.x );

			if ( ! Frustum_IsVisible( tile_fr, sp ))
				continue;

			gl.AtomicOr( INOUT un_Tiles.elements[ tile_idx ].indices[ word ], mask );

		  #ifndef PERF_TEST
			uint	pos	 = gl.AtomicAdd( INOUT un_Tiles.elements[ tile_idx ].count, 1 );
			max_per_tile = Max( max_per_tile, pos + 1 );
			++assigned;
		  #endif
		}

	  #ifndef PERF_TEST
		gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], assigned );
		gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], max_per_tile );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Math.glsl"
	#include "Color.glsl"

	uint  TileBitCount (const uint tile_idx)
	{
		const uint	word_n	= un_Tiles.elements[0].indices.length();
		uint		n		= 0;

		for (uint w = 0; w < word_n; ++w) {
			n += BitCount( un_Tiles.elements[ tile_idx ].indices[ w ] );
		}
		return n;
	}

	void  Main ()
	{
		out_Color = gl.texture.Fetch( un_LightVolumesRT, int2(gl.FragCoord.xy), 0 );

		if ( iView == 0 )
			return;

		const float2	uv			= gl.FragCoord.xy * un_PerPass.invResolution;
		const int2		tile_cnt	= int2(iTileCount);
		const int2		tile_id		= Clamp( int2( uv * float2(tile_cnt) ), int2(0), tile_cnt - 1 );
		const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );
		const bool		use_bits	= iListVersion >= 30;
		const uint		count		= use_bits ?
										TileBitCount( tile_idx ) :
										Min( un_Tiles.elements[ tile_idx ].count, un_Tiles.elements[0].indices.length() );

		switch ( iView )
		{
			case 1 :
			{
				float3	color = float3(0.0);
				if ( use_bits )
				{
					const uint	word_n	= un_Tiles.elements[0].indices.length();
					for (uint w = 0; w < word_n; ++w)
					{
						uint	bits = un_Tiles.elements[ tile_idx ].indices[w];

						for (uint b = 0; bits != 0 and b < 32; ++b)
						{
							if ( (bits & 1u) != 0 )
							{
								const uint	light_idx = w * 32 + b;
								if ( light_idx < iLightCount )
									color += unpackUnorm4x8( un_Lights.elements[ light_idx ].color ).rgb * 0.1;
							}
							bits >>= 1;
						}
					}
				}
				else
				{
					for (uint i = 0; i < count; ++i)
					{
						const uint	light_idx	= un_Tiles.elements[ tile_idx ].indices[i];
						color += unpackUnorm4x8( un_Lights.elements[ light_idx ].color ).rgb * 0.1;
					}
				}

				out_Color.rgb = Lerp( out_Color.rgb, color, iBlend );
				break;
			}
			case 2 :
			{
				float4	color = Rainbow2( 1.0 - float(count) / Max( float(un_Params.assignedLights_maxLightsPerTile[1]), 1.0 ));
				out_Color = Lerp( out_Color, color, iBlend );
				if ( count == 0 )
					out_Color = float4(0.0);
				break;
			}
		}

		// tile border
		float2	t = Fract( uv * float2(tile_cnt) ) / float2(tile_cnt) * un_PerPass.resolution.xy;
		if ( t.x < 1.25 or t.y < 1.25 )
			out_Color.rgb *= 0.2;
	}

#endif
//-----------------------------------------------------------------------------
