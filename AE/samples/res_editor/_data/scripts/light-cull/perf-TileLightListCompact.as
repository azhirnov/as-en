// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Dynamically allocate space for light list
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_LIGHTS
#	define BUILD_LIGHT_LIST_V10
#	define BUILD_LIGHT_LIST_V11
#	define VIEW
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
		RC<Buffer>			light_list_buf	= Buffer();
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
		RC<DynamicUInt>		max_list_size	= tile_total.Mul( 32 * 32 );

		RC<DynamicUInt>		repeat			= DynamicUInt();
		RC<DynamicFloat>	time			= DynamicFloat();
		RC<DynamicFloat>	time_ms			= time.Mul( 1000.0f );

		const bool			has_sg_size_ctrl	= GetFeatureSet().hasSubgroupSizeControl();
		const uint			max_lights_per_tile	= 32;
		const uint			warp_size			= has_sg_size_ctrl ? GetFeatureSet().getMaxSubgroupSize() : GetSubgroupSize();
		const uint			warps_per_wg		= 4;
		const uint			local_size			= warp_size * warps_per_wg;

		Slider( grid_pot,		"LightGridPOT",		uint3(1),	uint3(6), 		uint3(3) );
		Slider( tile_count,		"TileCount",		uint2(4),	uint2(128),	uint2(20,12) );
		Slider( view_mode,		"View",				0,			2,		2 );
		Slider( list_mode,		"ListMode",			1,			1 );
		Slider( subgroup_opt,	"SubgroupOpt",		0,			1 );
		Slider( repeat,			"Repeat",			1,			32 );

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
				"	uint	offset;"
				"	uint	count;",
				tile_total );

			light_list_buf.ArrayLayout(
				"LightList",
				// element
				"	uint	idx;",
				// static
				"	uint	count;",
				max_list_size );

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
			pass.AddPipeline( "perf-TileLightList-draw.ppln" );
			pass.Output( "out_Color", 	rt_lights, 	RGBA32f(0.f) );
			pass.Output( 				ds, 		DepthStencil( 1.f, 0 ));
		}{
			ClearBuffer( params_buf, 0 );
			ClearBuffer( light_list_buf, /*offset*/0, /*size*/4, /*value*/0 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST_V10" );
			pass.Set( camera );
			pass.ArgIn(		"un_Lights",	lights_buf );
			pass.ArgOut(	"un_Tiles",		tiles_buf );
			pass.ArgOut(	"un_LightList",	light_list_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
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
			pass.ArgOut(	"un_LightList",	light_list_buf );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Constant(	"iLightCount",	light_count );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
			pass.LocalSize( warp_size, warps_per_wg );
			pass.DispatchGroups( tile_count.DivCeil(uint2( 1, warps_per_wg )));
			pass.Repeat( repeat );
			pass.MeasureTime( time );
			pass.EnableIfEqual( list_ver, 11 );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",			rt,			RGBA32f(0.0) );
			pass.ArgIn(		"un_LightVolumesRT",	rt_lights,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Tiles",				tiles_buf );
			pass.ArgIn(		"un_LightList",			light_list_buf );
			pass.ArgIn(		"un_Lights",			lights_buf );
			pass.ArgIn(		"un_Params",			params_buf );
			pass.Slider(	"iBlend",				0.0,	1.0,	0.8 );
			pass.Slider(	"iColorScale",			0,		10,		4 );
			pass.Constant(	"iTileCount",			tile_count );
			pass.Constant(	"iLightCount",			light_count );
			pass.Constant(	"iView",				view_mode );
		}

		RC<DynamicUInt2>	params = DynamicUInt2();
		ReadBuffer( params, params_buf, "assignedLights_maxLightsPerTile" );

		RC<DynamicUInt >	list_size = DynamicUInt();
		ReadBuffer( list_size, light_list_buf, "count" );

		Label( tile_total,		"Total tiles" );
		Label( light_count,		"Total lights" );

		Label( list_size,		"Light list size" );
		Label( max_list_size,	"Max light list size" );

		Label( params.X(), 		"Assigned Lights" );
		Label( params.Y(),		"Max Lights per Tile" );
		Label( time_ms,			"Light list (ms)" );

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

		// tile frustum in view space
		const Frustum	main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );
		const Frustum	tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );

		uint			count		= 0;

		// allocate space
		for (uint i = 0; i < iLightCount; ++i)
		{
			const OmniLightData		light	= un_Lights.elements[i];
			const float3			vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
			const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space

			count += Frustum_IsVisible( tile_fr, sp ) ? 1u : 0u;
		}

		if ( count == 0 )
			return;

		uint	pos = gl.AtomicAdd( INOUT un_LightList.count, count );
		if ( pos + count >= un_LightList.elements.length() )
			return;

		un_Tiles.elements[ tile_idx ].offset = pos;
		un_Tiles.elements[ tile_idx ].count	 = count;

		// fill light list
		for (uint i = 0; i < iLightCount; ++i)
		{
			const OmniLightData		light	= un_Lights.elements[i];
			const float3			vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
			const Sphere			sp		= Sphere_Create( vpos, light.radius );

			if ( Frustum_IsVisible( tile_fr, sp ))
			{
				un_LightList.elements[ pos ].idx = i;
				++pos;
			}
		}

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

		// tile frustum in view space
		sg_uniform const Frustum	main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );
		sg_uniform const Frustum	tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );

		sg_uniform const uint		sg_size		= gl.subgroup.Size;
		sg_uniform const uint		num_iters	= DivCeil( iLightCount, sg_size );

		uint	local_count = 0;


		// allocate space //

		// uniform
		for (uint k = 0; k < num_iters; ++k)
		{
			const uint	i = k * sg_size + gl.subgroup.Index;

			// non-uniform
			if ( i < iLightCount )
			{
				const OmniLightData		light	= un_Lights.elements[i];
				const float3			vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
				const Sphere			sp		= Sphere_Create( vpos, light.radius );	// view space

				local_count += Frustum_IsVisible( tile_fr, sp ) ? 1u : 0u;
			}
		}

		sg_uniform uint			base	= 0;	// filled slots
		sg_uniform const uint	count	= gl.subgroup.Add( local_count );
		sg_uniform uint			base_offset;

		// uniform
		if ( count == 0 )
			return;

		if ( gl.subgroup.Elect() )
		{
			base_offset = gl.AtomicAdd( INOUT un_LightList.count, count );

			if ( base_offset + count < un_LightList.elements.length() )
			{
				un_Tiles.elements[ tile_idx ].offset = base_offset;
				un_Tiles.elements[ tile_idx ].count	 = count;
			}
		}
		base_offset = gl.subgroup.BroadcastFirst( base_offset );

		// uniform
		if ( base_offset + count >= un_LightList.elements.length() )
			return;


		// fill light list //

		// uniform
		for (uint k = 0; k < num_iters; ++k)
		{
			const uint	i		= k * sg_size + gl.subgroup.Index;
			uint		valid	= 0;

			// non-uniform
			if ( i < iLightCount )
			{
				const OmniLightData		light	= un_Lights.elements[i];
				const float3			vpos	= float3x3(un_PerPass.camera.view) * (light.pos - un_PerPass.camera.pos);
				const Sphere			sp		= Sphere_Create( vpos, light.radius );

				valid = Frustum_IsVisible( tile_fr, sp ) ? 1u : 0u;
			}

			const uint		offset	= gl.subgroup.ExclusiveAdd( valid );
			sg_uniform uint	added	= gl.subgroup.Add( valid );
			const uint		pos		= base_offset + base + offset;

			// non-uniform
			if ( valid != 0 )
				un_LightList.elements[ pos ].idx = i;

			base += added;
		}

	  #ifndef PERF_TEST
		if ( gl.subgroup.Elect() )
		{
			gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
			gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], count );
		}
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Math.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		out_Color = gl.texture.Fetch( un_LightVolumesRT, int2(gl.FragCoord.xy), 0 );

		if ( iView == 0 )
			return;

		const float2	uv			= gl.FragCoord.xy * un_PerPass.invResolution;
		const int2		tile_cnt	= int2(iTileCount);
		const int2		tile_id		= Clamp( int2( uv * float2(tile_cnt) ), int2(0), tile_cnt - 1 );
		const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );

		const TileLightList  light_list = un_Tiles.elements[ tile_idx ];

		switch ( iView )
		{
			case 1 :
			{
				float3	color = float3(0.0);
				for (uint i = 0; i < light_list.count; ++i)
				{
					uint	light_idx = un_LightList.elements[ light_list.offset + i ].idx;
					color += unpackUnorm4x8( un_Lights.elements[ light_idx ].color ).rgb;
				}
				color /= Exp2( float(iColorScale) );
				out_Color.rgb = Lerp( out_Color.rgb, color, iBlend );
				break;
			}
			case 2 :
			{
				float4	color = Rainbow2( 1.0 - float(light_list.count) / Max( float(un_Params.assignedLights_maxLightsPerTile[1]), 1.0 ));
				out_Color = Lerp( out_Color, color, iBlend );
				if ( light_list.count == 0 )
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
