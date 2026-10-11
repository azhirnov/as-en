// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define PUT_LIGHTS
#	define INIT_MINMAX_DEPTH
#	define CALC_MINMAX_DEPTH
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
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, rt_dim );	rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), rt_dim );			ds.Name( "Depth" );
		RC<Image>			minmax_depth	= Image( EPixelFormat::RG16F, rt_dim, MipmapLevel(~0) );	minmax_depth.Name( "MinMax Depth" );	// TODO: only HiZ ?
		RC<Image>			rt_light		= Image( hdr_fmt, rt_dim );							rt_light.Name( "LightBuf" );
		RC<Buffer>			obj_buf			= Buffer();											obj_buf.Name( "Buildings buf" );
		RC<Buffer>			lights_buf		= Buffer();											lights_buf.Name( "Lights buf" );
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_lights	= Scene();
		RC<Scene>			scene_cones		= Scene();
		RC<Buffer>			tiles_buf		= Buffer();
		RC<Buffer>			params_buf		= Buffer();
		RC<FPVCamera>		camera 			= FPVCamera();

		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt>		light_count		= DynamicUInt();
		RC<DynamicUInt>		view_mode		= DynamicUInt();

		RC<DynamicInt2>		select_tile		= DynamicInt2();
		RC<DynamicUInt2>	tile_count		= DynamicUInt2();
		RC<DynamicUInt>		tile_total		= tile_count.Area();

		const uint			max_light_count		= 1024;
		const uint			max_lights_per_tile	= max_light_count / 32;

		const bool			has_sg_size_ctrl	= GetFeatureSet().hasSubgroupSizeControl();
		const uint			warp_size			= has_sg_size_ctrl ? GetFeatureSet().getMaxSubgroupSize() : GetSubgroupSize();
		const uint			warps_per_wg		= 4;
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

			tiles_buf.ArrayLayout(
				"TileLightList",
				"	uint	count;"		// atomic, only for debugging
				"	uint	indices["s+(max_lights_per_tile)+"];",	// index or bitfield
				tile_total );

			params_buf.UseLayout(
				"Params",
				"	uint2	assignedLights_maxLightsPerTile;"	// atomic
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

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( obj_count );
			cmd.PipelineHint( "opaque.GEqual" );	// reverseZ
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
		Slider( light_count,	"LightCount",	100,		max_light_count,	200 );
		Slider( view_mode,		"View",			0,			4,					0 );	// 0 - combined, 1 - color, 2 - light, 3 - lights per tile, 4 - dbg cone
		Slider( tile_count,		"TileCount",	uint2(4),	uint2(128),			uint2(20,12) );
		Slider( select_tile,	"SelectTile",	int2(-1),	int2(20,12),		int2(-1) );

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",	obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			RC<ComputePass>			pass = ComputePass( "", "PUT_LIGHTS; FIELD_VISIBLE" );
			pass.ArgInOut(	"un_Lights",	lights_buf );
			pass.Slider(	"iMaxHeight",	1.0,	20.0,	20.0 );
			pass.LocalSize( 64 );
			pass.DispatchThreads( light_count );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/StreetLights/Opaque.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/Opaque.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );	// albedo
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(0.0, 0) );
		}

		// Min/Max Depth
		{
			RC<Postprocess>		pass = Postprocess( "", "INIT_MINMAX_DEPTH" );
			pass.Output( "out_Color",	minmax_depth );
			pass.ArgIn(	 "un_Depth",	ds,		Sampler_NearestClamp );
		}{
			RC<RasterMip>		pass = RasterMip( "", "CALC_MINMAX_DEPTH" );
			pass.Variable( "un_InImage",	"out_Color",	minmax_depth,	Sampler_NearestClamp );
		}

		{
			ClearBuffer( params_buf, 0 );
			ClearBuffer( tiles_buf, 0 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "BUILD_LIGHT_LIST" );	// v3.1
			pass.Set( camera );
			pass.ArgInOut(	"un_Lights",		lights_buf );
			pass.ArgOut(	"un_Tiles",			tiles_buf );
			pass.ArgInOut(	"un_Params",		params_buf );
			pass.ArgIn(		"un_MinMaxDepth",	minmax_depth,	Sampler_NearestClamp );
			pass.Slider(	"iCullMode",		0,		2,		2 );
			pass.Slider(	"iDepthTest",		0,		1,		1 );
			pass.Slider( 	"iCullError",		-0.5f,	0.5f,	0.f );
			pass.Constant(	"iTile",			select_tile );
			pass.Constant(	"iTileCount",		tile_count );
			pass.Constant(	"iLightCount",		light_count );
			pass.Constant(	"iScreenDim",		rt_dim );
			if ( has_sg_size_ctrl )	pass.SubgroupSize( 32 );
			pass.LocalSize( 32, warps_per_wg );
			pass.DispatchGroups( tile_count.DivCeil(uint2( 1, warps_per_wg )));
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			RC<Postprocess>		pass = Postprocess( "", "APPLY_LIGHTING" );
			pass.Set( camera );
			pass.Output(	"out_Light",		rt_light,		RGBA32f(0.0) );
			pass.ArgIn(		"un_Albedo",		rt_col,			Sampler_NearestClamp );
			pass.ArgIn(		"un_Depth",			ds,				Sampler_NearestClamp );
			pass.ArgIn(		"un_Normal",		rt_norm,		Sampler_NearestClamp );
			pass.ArgIn(		"un_Lights",		lights_buf );
			pass.ArgIn(		"un_Tiles",			tiles_buf );
			pass.Constant(	"iTileCount",		tile_count );
			pass.Constant(	"iLightCount",		light_count );
		}{
			RC<SceneGraphicsPass>	pass = scene_cones.AddGraphicsPass( "dbg cones" );
			pass.AddPipeline( "*-dbg.ppln" );
			pass.Output( "out_Color",			rt_light,		RGBA32f(0.0) );
			pass.Output(						ds,				DepthStencil(0.0, 0) );
			pass.Slider( "iMode",				0,	2,	2 );
			pass.EnableIfEqual( view_mode, 4 );
		}{
			RC<Postprocess>			pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_Albedo",		rt_col,			Sampler_NearestClamp );
			pass.ArgIn(		"un_LightBuf",		rt_light,		Sampler_NearestClamp );
			pass.ArgIn(		"un_Params",		params_buf );
			pass.ArgIn(		"un_Tiles",			tiles_buf );
			pass.Slider(	"iLightScale",		1.0,	100.0,	10.0 );
			pass.Constant(	"iTile",			select_tile );
			pass.Constant(	"iView",			view_mode );
			pass.Constant(	"iTileCount",		tile_count );
		}{
			RC<SceneGraphicsPass>	pass = scene_lights.AddGraphicsPass( "translucent" );
			pass.AddPipeline( "samples/StreetLights/LightBulb.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightBulb.as)
			pass.AddPipeline( "samples/StreetLights/LightCone.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightCone.as)
			pass.Output( "out_Color",			rt );
			pass.OutputLS(						ds,			EAttachmentLoadOp::Load, EAttachmentStoreOp::None );	// read only
			pass.EnableIfLess( view_mode, 3 );
		}

		RC<DynamicUInt2>	params = DynamicUInt2();
		ReadBuffer( params, params_buf, "assignedLights_maxLightsPerTile" );

		RC<DynamicFloat>	avr_lights = params.X().ToFloat().Div( tile_total.ToFloat() );

		Label( params.X(), 	"Assigned Lights" );
		Label( params.Y(), 	"Max Lights per Tile" );
		Label( avr_lights, 	"Avg Lights per Tile" );

		RC<DynamicUInt2>	tile_size = rt_dim.XY().Div( tile_count );
		Label( tile_size,	"Tile dim (px)" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(PUT_OBJECTS) or defined(PUT_LIGHTS)

	#include "samples/StreetLights.as"

#endif
//-----------------------------------------------------------------------------
#ifdef BUILD_LIGHT_LIST
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "InvocationID.glsl"

	int2  NdcToTile (float2 ndc) {
		return int2( ToUNorm( ndc ) * float2(iTileCount) );
	}

	int2  NdcToCeilTile (float2 ndc) {
		return int2(Ceil( ToUNorm( ndc ) * float2(iTileCount) ));
	}

	RectI  NdcToTile (Rect ndc) {
		return Rect_Create( NdcToTile(		Rect_Min( ndc )),
							NdcToCeilTile(	Rect_Max( ndc )) );
	}

	bool  SphereOverlapsViewDepthRange (const Sphere sphere, const float minTileDepth, const float maxTileDepth)
	{
		float	sphere_center_depth	= sphere.center.z;
		float	min_sphere_depth	= sphere_center_depth - sphere.radius;
		float	max_sphere_depth	= sphere_center_depth + sphere.radius;
		return	max_sphere_depth >= minTileDepth and
				min_sphere_depth <= maxTileDepth;
	}


	// see [FrustumCullingRevZ](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/geom-cull/FrustumCullingRevZ.as),
	// [LightVolToTile](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-LightVolToTile.as)
	bool  SpotLightVisible (sg_uniform const Frustum tileFr, const int2 tileId, const LightObject light, const float2 minMaxDepth)
	{
		float3		wpos		= light.position - un_PerPass.camera.pos;
		float3		vpos		= float3x3(un_PerPass.camera.view) * wpos;
		float3		vdir		= Normalize( float3x3(un_PerPass.camera.view) * light.dir );
		float		cull_err	= Clamp( 1.0 - iCullError, 0.0, 2.0 );

		Cone		vs_cone		= Cone_Create( vpos, vdir, light.angle * cull_err, light.height * cull_err );	// view space
		Sphere		vs_sp		= Cone_ToBoundingSphere( vs_cone );												// view space

		float		p00			= un_PerPass.camera.proj[0][0];
		float		p11			= un_PerPass.camera.proj[1][1];
		float		z_near		= un_PerPass.camera.clipPlanes.x;

		if ( iDepthTest != 0 and !SphereOverlapsViewDepthRange( vs_sp, minMaxDepth.x, minMaxDepth.y ))
			return false;

		switch ( iCullMode )
		{
			case 0 :	// project bounding sphere
			{
				Rect	aabb_ndc;
				if ( Sphere_FastProject2( vs_sp, p00, p11, z_near, OUT aabb_ndc ))
				{
					RectI	tile_rect = NdcToTile( aabb_ndc );
					return Rect_IsInside( tile_rect, tileId );
				}
				return true;  // too large or too close
			}

			case 1 :	// is cone visible for tile frustum
			{
				return Frustum_IsVisible( tileFr, vs_cone );
			}

			case 2 :	// unite 2 previous
			{
				Rect	aabb_ndc;
				bool	inside1 = true;	// if too large or too close
				if ( Sphere_FastProject2( vs_sp, p00, p11, z_near, OUT aabb_ndc ))
				{
					RectI	tile_rect 	= NdcToTile( aabb_ndc );
							inside1		= Rect_IsInside( tile_rect, tileId );
				}
				bool	inside2 = Frustum_IsVisible( tileFr, vs_cone );
				return	inside1 and inside2;
			}
		}
	}


	bool  ReadTileDepthRange (const int2 tileId, out float2 minMaxDepth)
	{
		const int2		pixel_min	= int2(Floor(float2(tileId) * float2(iScreenDim) / float2(iTileCount)));
		const int2		pixel_max	= int2(Ceil(float2(tileId + 1) * float2(iScreenDim) / float2(iTileCount)));

		const float2	tile_dim	= Round( float2(iScreenDim) / float2(iTileCount) );
		const float		min_dim		= Max( Min( tile_dim.x, tile_dim.y ), 1.0 );
		const uint		mip			= uint(Max( Floor( Log2( min_dim )), 0.0 ));
		const int		scale		= 1 << int(mip);

		int2			min_coord	= pixel_min / scale;
		int2			max_coord	= (pixel_max + scale - 1) / scale;

		const int2		mip_dim		= gl.texture.GetSize( un_MinMaxDepth, int(mip) );

		min_coord = Clamp( min_coord, int2(0),   mip_dim );
		max_coord = Clamp( max_coord, min_coord, mip_dim );

		float	min_depth =  float_max;
		float	max_depth = -float_max;

		for (int y = min_coord.y; y < max_coord.y; ++y)
		{
			for (int x = min_coord.x; x < max_coord.x; ++x)
			{
				float2 depth_range = gl.texture.Fetch( un_MinMaxDepth, int2(x,y), int(mip) ).rg;

				min_depth = Min( min_depth, depth_range.x );
				max_depth = Max( max_depth, depth_range.y );
			}
		}

		minMaxDepth.y = FastUnProjectZ( un_PerPass.camera.proj, min_depth );
		minMaxDepth.x = FastUnProjectZ( un_PerPass.camera.proj, max_depth );

		minMaxDepth.x = Max( minMaxDepth.x, un_PerPass.camera.clipPlanes.x );
		minMaxDepth.y = Max( minMaxDepth.x, minMaxDepth.y );

		return min_depth <= max_depth;
	}


	void  Main () [[maximally_reconverges]]
	{
		dbg_EnableTraceRecording( AllEqual( int2( GetGroupCoord().x, GetGlobalCoord().y ), int2(7,4) ));

		sg_uniform const int2		tile_id		= int2( GetGroupCoord().x, GetGlobalCoord().y );
		sg_uniform const int2		tile_cnt	= int2(iTileCount);
		sg_uniform const bool		selected	= AllEqual( tile_id, iTile );

		// uniform
		if ( AnyGreaterEqual( tile_id, tile_cnt ))
			return;

		sg_uniform const uint		tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );

		sg_uniform const Frustum	main_fr		= Frustum_FromMatrix( un_PerPass.camera.proj, un_PerPass.camera.clipPlanes );		// view space
		sg_uniform const Frustum	tile_fr		= Frustum_ToTile( main_fr, tile_id, tile_cnt );										// view space

		sg_uniform const uint		sg_size		= gl.subgroup.Size;		// must be <= 32
		sg_uniform const uint		num_iters	= DivCeil( iLightCount, sg_size );
		sg_uniform float2			min_max_depth;
		uint						local_count = 0;

		if ( ! ReadTileDepthRange( tile_id, min_max_depth ))
			return;

		min_max_depth = gl.subgroup.BroadcastFirst( min_max_depth );

		// uniform
		// loop must have uniform control flow to use subgroup ops inside
		for (uint k = 0; k < num_iters; ++k)
		{
			const uint	light_id	= k * sg_size + gl.subgroup.Index;
			uint		mask		= 0;

			// non-uniform
			if ( light_id < iLightCount )
			{
				if ( SpotLightVisible( tile_fr, tile_id, un_Lights.elements[ light_id ], min_max_depth ))
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
				un_Tiles.elements[ tile_idx ].indices[ word ] |= word_mask;
			}
		}

		sg_uniform uint	count = gl.subgroup.Add( local_count );

		if ( gl.subgroup.Elect() )
		{
			un_Tiles.elements[ tile_idx ].count = count;

			gl.AtomicAdd( INOUT un_Params.assignedLights_maxLightsPerTile[0], count );
			gl.AtomicMax( INOUT un_Params.assignedLights_maxLightsPerTile[1], count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef APPLY_LIGHTING
	#include "PBR.glsl"
	#include "Cone.glsl"
	#include "Matrix.glsl"
	#include "CodeTemplates.glsl"

	struct GBuffer
	{						// TODO:
		float3	albedo;		// half3
		float	metalness;	// half, don't pack
		float3	normal;		// half3
		float	roughness;	// half, don't pack
		float3	wpos;
		float3	viewDir;	// half3 ?
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


	void  Main () [[maximally_reconverges]]
	{
		const int2		coord		= int2(gl.FragCoord.xy);
		const float2	uv			= gl.FragCoord.xy * un_PerPass.invResolution;

		const uint2		tile_id		= uint2( float2(iTileCount) * uv );
		const uint		tile_idx	= tile_id.x + tile_id.y * iTileCount.x;

		const uint		word_count	= DivCeil( iLightCount, 32 );

		float			depth		= gl.texture.Fetch( un_Depth, coord, 0 ).r;	// reverse Z
		float3			color		= float3(0.0);
		float			count		= 0.0;

		// uniform
		if ( gl.subgroup.All( depth == 0.0 ))
		{
			out_Light = float4(0.0);
			return;
		}

		GBuffer			gbuf;
		gbuf.albedo		= gl.texture.Fetch( un_Albedo, coord, 0 ).rgb;
		gbuf.normal		= ToSNorm( gl.texture.Fetch( un_Normal, coord, 0 ).rgb );	// world space
		gbuf.wpos		= UnProjectNDC( un_PerPass.camera.invViewProj, float3( ToSNorm(uv), depth ));
		gbuf.roughness	= 0.5;
		gbuf.metalness	= 0.8;
		gbuf.viewDir	= ViewDir( un_PerPass.camera.invViewProj, uv );

		// uniform
		for (sg_uniform uint word_idx = 0; word_idx < word_count; ++word_idx)
		{
			const uint		mask = un_Tiles.elements[ tile_idx ].indices[ word_idx ];

			// 10% faster if tile doesn't match with HW tile size on NV, Mali, Apple
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

		out_Light = float4(color, count);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Color.glsl"
	#include "ColorSpace.glsl"
	#include "ToneMapping.glsl"

	uint  TileBitCount (const uint tile_idx)
	{
		const uint	word_n	= un_Tiles.elements[0].indices.length();
		uint		n		= 0;

		for (uint w = 0; w < word_n; ++w) {
			n += BitCount( un_Tiles.elements[ tile_idx ].indices[ w ] );
		}
		return n;
	}

	void Main ()
	{
		float2	uv		= gl.FragCoord.xy * un_PerPass.invResolution;
		float3	light	= gl.texture.Fetch( un_LightBuf, int2(gl.FragCoord.xy), 0 ).rgb * iLightScale;
		float3	albedo	= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float3	ambient	= RemoveSRGBCurve( float3(54.0, 61.0, 75.0) / 255.0 );
		int2	tile_cnt= int2(iTileCount);

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
				int2	tile_id		= Clamp( int2( uv * float2(tile_cnt) ), int2(0), tile_cnt - 1 );
				uint	tile_idx	= uint( tile_id.x + tile_id.y * tile_cnt.x );
				uint	count		= TileBitCount( tile_idx );
				float4	color		= Rainbow2( 1.0 - float(count) / Max( float(un_Params.assignedLights_maxLightsPerTile[1]), 1.0 ));
				if ( count == 0 )
					color = float4(0.0);

				out_Color = float4( Lerp( albedo, color.rgb, 0.7 ), float(count) );

				// tile border
				float2	t = Fract( uv * float2(tile_cnt) ) / float2(tile_cnt) * un_PerPass.resolution.xy;
				if ( t.x < 1.25 or t.y < 1.25 )
					out_Color.rgb *= 0.2;
				break;
			}

			case 4 :
			{
				out_Color = ApplySRGBCurve( float4( albedo * (light + ambient), 1.0 ));

				// tile border
				float2	tile	= uv * float2(tile_cnt);
				float2	t		= Fract( tile ) / float2(tile_cnt) * un_PerPass.resolution.xy;

				if ( AllEqual( int2(tile), iTile ) or AllEqual( int2(tile-0.01), iTile ))
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
#ifdef INIT_MINMAX_DEPTH

	void  Main ()
	{
		float	d = gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;
		out_Color.rg = float2(d, d);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CALC_MINMAX_DEPTH
	#include "InvocationID.glsl"

	// example: downsample 5x5 to 2x2
	//
	// |0|1|2|3|4|
	//  | 0 | 1 |

	void  Main ()
	{
		uint2	dim		= pc.srcResolution; // gl.texture.GetSize( un_InImage, 0 );
		float2	fdim	= float2(dim);
		float2	inv_res	= iInvResolution;

		float2	uv0		= (GetGlobalCoordUF().xy * inv_res) * fdim;
		float2	uv1		= ((GetGlobalCoordUF().xy + 1.0) * inv_res) * fdim;

		float	min_d	= float_max;
		float	max_d	= 0.0;

		int2	c0		= Max( int2(uv0), int2(0) );
		int2	c1		= Min( int2(uv1), int2(dim-1) );

		for (int y = c0.y; y <= c1.y; ++y)
		for (int x = c0.x; x <= c1.x; ++x)
		{
			float2	d = gl.texture.Fetch( un_InImage, int2(x,y), 0 ).rg;
			min_d = Min( min_d, d.x );
			max_d = Max( max_d, d.y );
		}

		out_Color.rg = float2( min_d, max_d );
	}

#endif
//-----------------------------------------------------------------------------
