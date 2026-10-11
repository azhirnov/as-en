// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	reference: https://youtu.be/il-TXbn5iMA
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define RAYMARCH
#	define SDF_TO_VOXELS
#	define UNPACK_SELECTED
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt1			= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			rt2			= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			steps_cnt	= Image( EPixelFormat::R32U,		SurfaceSize() );
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			ds			= Image( EPixelFormat::Depth32F,	SurfaceSize() );
		RC<Image>			sdf_data	= Image( EPixelFormat::R16F,		uint3(256) );		// 128^3 - 4 MB, 256^3 - 32 MB
		RC<FPVCamera>		camera		= FPVCamera();
		RC<Buffer>			voxels		= Buffer();
		RC<Scene>			scene		= Scene();
		RC<Buffer>			select		= Buffer();
		RC<DynamicFloat>	min_den		= DynamicFloat();
		const uint3			local_size	= uint3( 8, 8, 8 );	// 512
		RC<DynamicUInt>		sdf_vox_pot	= DynamicUInt();	// 1: 8 texels per voxel
		RC<DynamicUInt>		sdf_per_vox	= sdf_vox_pot.Exp2();
		RC<DynamicUInt>		grid_pot	= DynamicUInt();
		RC<DynamicUInt3>	grid_size	= grid_pot.Exp2().XXX();
		RC<DynamicUInt>		vox_count	= grid_size.Volume();
		RC<DynamicUInt>		max_steps	= DynamicUInt();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 10.f );
			camera.FovY( 70.f );

			const float	s = 0.6f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		voxels.ArrayLayout(
			"Voxel",
			"	uint	packedUVW;"		// ~0u - empty, ~0u-1 - opaque
			"	uint	packedPos;",	// used only for selection
			"uint	count;",			// atomic
			vox_count
		);

		select.UseLayout(
			"MouseSelectionData",
			"	uint	packed;"	// atomic
			"	float	depth;"
			"	uint	instanceId;"
			"	uint3	coord;"
		);

		// create geometry
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount		= 6*6;	// 6 verts * 6 faces (full cube)
			cmd.InstanceCount( vox_count );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( min_den,		"MinDensity",		0.0,	0.5,	0.0 );
		Slider( grid_pot,		"GridSizePOT",		3,		6,		4 );
		Slider( sdf_vox_pot,	"TexelsPerQuadPOT",	1,		4,		3 );
		Slider( max_steps,		"MaxSteps",			1,		32,		8 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "RAYMARCH" );
			pass.Set( camera );
			pass.Output(	"out_Color",	rt1 );
			pass.Constant(	"iMinDensity",	min_den );
		}{
			ClearBuffer( voxels, 0 );

			RC<ComputePass>		pass = ComputePass( "", "SDF_TO_VOXELS" );
			pass.ArgInOut(	"un_Voxels",			voxels );
			pass.ArgInOut(	"un_SDFCache",			sdf_data );
			pass.Constant(	"iMinDensity",			min_den );
			pass.Constant(	"iGridSize",			grid_size );
			pass.Constant(	"iCacheDim",			sdf_data.Dimension3() );
			pass.Constant(	"iTexelsPerVoxelPOT",	sdf_vox_pot );
			pass.LocalSize( local_size );
			pass.DispatchThreads( grid_size );
		}{
			ClearImage( steps_cnt, RGBA32u(0) );

			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw voxels" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",		rt2,		RGBA32f(0.f) );
			pass.Output(						ds,			DepthStencil(1.f, 0) );
			pass.ArgInOut(	"un_StepsCount",	steps_cnt );	// atomic
			pass.ArgIn(		"un_Voxels",		voxels );
			pass.ArgIn(		"un_SDFCache",		sdf_data,	Sampler_LinearClamp );
			pass.ArgInOut(	"un_Select",		select );
			pass.Constant(	"iPixStep",			sdf_per_vox );
			pass.Constant(	"iInvDim",			float3(1.0) / float3(sdf_data.Dimension3()) );
			pass.Constant(	"iGridSize",		grid_size );
			pass.Constant(	"iMinDensity",		min_den );
			pass.Constant(	"iMaxSteps",		max_steps );
			pass.Slider(	"iColorMode",		0,	3,	0 );
			pass.Slider(	"iTraceMode",		0,	1 );
			pass.Slider(	"iLightDir",		float2(-1.0),	float2(1.0),	float2(0.0, 1.0) );
		}{
			RC<ComputePass>		pass = ComputePass( "", "UNPACK_SELECTED" );
			pass.ArgInOut(	"un_Select",	select );
			pass.ArgIn(		"un_Voxels",	voxels );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_RayMarch",		rt1,		Sampler_NearestClamp );
			pass.ArgIn(		"un_Voxels",		rt2,		Sampler_NearestClamp );
			pass.ArgIn(		"un_StepsCount",	steps_cnt,	Sampler_NearestClamp );
			pass.Slider(	"iBlend",			0.0,	1.0,	0.8 );
			pass.Slider(	"iDbgMode",			0,		1 );
			pass.Constant(	"iMaxSteps",		max_steps );
		}

		RC<DynamicUInt>		selected_inst	= DynamicUInt();
		RC<DynamicUInt3>	selected_coord	= DynamicUInt3();

		ReadBuffer( selected_inst,	select, "instanceId" );
		ReadBuffer( selected_coord,	select, "coord" );

		Label( selected_inst,	"Selected Instance" );
		Label( selected_coord,	"Selected coord" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(RAYMARCH) or defined(SDF_TO_VOXELS) or defined(UNPACK_SELECTED)

	#include "voxels/Base.as"

#endif
//-----------------------------------------------------------------------------
#ifdef SDF_TO_VOXELS
	#include "Atomic.glsl"
	#include "InvocationID.glsl"

	void  StoreSDF (float3 pos, int3 posInCache)
	{
		float	d = SDFScene( ToSNorm( pos ));
		gl.image.Store( un_SDFCache, posInCache, float4(d) );
	}

	void  Main ()
	{
		const int3	coord	= GetGlobalCoord();
		const int3	size	= iGridSize;

		if ( AnyGreaterEqual( coord, size ))
			return;

		const float3	inv_size	= 1.0 / float3(size);
		const int		idx			= GetGlobalIndex();

		un_Voxels.elements[ idx ].packedPos = (coord.x & 0xFF) | ((coord.y & 0xFF) << 8) | ((coord.z & 0xFF) << 16);

		// fast path
		{
			float	d		= Density( coord, size, inv_size ) * 0.5;
			float	max_d	= MaxOf( inv_size );

			if ( Abs(d) > max_d )
			{
				un_Voxels.elements[ idx ].packedUVW = ~0u;		// empty
				return;
			}
		}

		uint	pos;
		SubgroupAtomicInc( OUT pos, INOUT un_Voxels.count );

		const uint3	cache_dim	= iCacheDim >> iTexelsPerVoxelPOT;

		if ( pos >= cache_dim.x * cache_dim.y * cache_dim.z )
		{
			un_Voxels.elements[ idx ].packedUVW = ~0u - 1;		// opaque	// TODO: special value for overflow?
			return;
		}

		const uint3	pos_in_cache = IndexToVec3( pos, cache_dim ) << iTexelsPerVoxelPOT;

		un_Voxels.elements[ idx ].packedUVW = pos_in_cache.x | (pos_in_cache.y << 8) | (pos_in_cache.z << 16);

		const uint	texels_per_voxel	= 1u << iTexelsPerVoxelPOT;
		const float	inv_tex				= 1.0 / float(texels_per_voxel - 1);

		for (uint z = 0; z < texels_per_voxel; ++z)
		for (uint y = 0; y < texels_per_voxel; ++y)
		for (uint x = 0; x < texels_per_voxel; ++x)
		{
			float3	pos = (float3(coord) + float3(x,y,z) * inv_tex) * inv_size;
			StoreSDF( pos, int3(pos_in_cache) + int3(x,y,z) );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Math.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	uv		= gl.FragCoord.xy * un_PerPass.invResolution;
		float4	col0	= gl.texture.Sample( un_RayMarch, uv );
		float4	col1	= gl.texture.Sample( un_Voxels, uv );

		float	overdraw = 4.0;
		float	steps	= float(gl.texture.Sample( un_StepsCount, uv ).r) / (iMaxSteps * overdraw);

		out_Color = Lerp( col0, col1, iBlend );

		switch ( iDbgMode )
		{
			case 1 :	out_Color = steps == 0.0 ? float4(0.0) : Rainbow2( 1.0 - Saturate( steps ));  break;
		}
	}

#endif
//-----------------------------------------------------------------------------
