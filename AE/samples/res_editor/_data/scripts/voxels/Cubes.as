// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple SDF to Cube voxels algorithm.
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
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			ds			= Image( EPixelFormat::Depth32F,	SurfaceSize() );
		RC<FPVCamera>		camera		= FPVCamera();
		RC<Buffer>			voxels		= Buffer();
		RC<Scene>			scene		= Scene();
		RC<Buffer>			select		= Buffer();
		RC<DynamicFloat>	min_den		= DynamicFloat();
		const uint3			local_size	= uint3( 8, 8, 8 );	// 512
		const uint3			grid_size	= uint3(32);

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
			"	uint	packedPos;"		// 3x 8 bit
			"	uint	faceBits;",		// max bit: 6
			Area( grid_size )
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
			cmd.vertexCount		= 6*6;	// 6 verts * 6 faces
			cmd.instanceCount	= Area( grid_size );
			geometry.Draw( cmd );

			scene.Add( geometry, float3(0.0, 0.0, 4.0) );
		}

		Slider( min_den,	"MinDensity",	0.0,	0.5,	0.0 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "RAYMARCH" );
			pass.Set( camera );
			pass.Output(	"out_Color",	rt1 );
			pass.Constant(	"iMinDensity",	min_den );
		}{
			RC<ComputePass>		pass = ComputePass( "", "SDF_TO_VOXELS" );
			pass.ArgInOut(	"un_Voxels",	voxels );
			pass.Constant(	"iMinDensity",	min_den );
			pass.LocalSize( local_size );
			pass.DispatchThreads( grid_size );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw voxels" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",	rt2,	RGBA32f(0.f) );
			pass.Output(					ds,		DepthStencil(1.f, 0) );
			pass.ArgIn(		"un_Voxels",	voxels );
			pass.ArgInOut(	"un_Select",	select );
			pass.Constant(	"iGridSize",	grid_size );
			pass.Slider(	"iColorMode",	0,	2,	1 );
			pass.Slider(	"iLightDir",	float2(-1.0),	float2(1.0),	float2(0.0, 1.0) );
		}{
			RC<ComputePass>		pass = ComputePass( "", "UNPACK_SELECTED" );
			pass.ArgInOut(	"un_Select",	select );
			pass.ArgIn(		"un_Voxels",	voxels );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",	rt );
			pass.ArgIn(		"un_RayMarch",	rt1,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Voxels",	rt2,	Sampler_NearestClamp );
			pass.Slider(	"iBlend",		0.0,	1.0,	0.8 );
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
#if defined(RAYMARCH) or defined(SDF_TO_VOXELS) or defined(UNPACK_SELECTED) or defined(VIEW)

	#include "voxels/Base.as"

#endif
//-----------------------------------------------------------------------------
#ifdef SDF_TO_VOXELS
	#include "CubeMap.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		int3	coord		= GetGlobalCoord();
		int3	size		= GetGlobalSize();
		float3	inv_size	= 1.0 / float3(size);
		uint	face_bits	= 0;
		float	density		= Density( coord, size, inv_size );

		if ( density > iMinDensity )
		{
			for (int i = 0; i < 6; ++i)
			{
				int3	off	= CM_FaceOffsetI( i );
				float	d	= Density( coord + off, size, inv_size );

				if ( d < iMinDensity )
					face_bits |= (1u << i);
			}
		}

		int	idx = GetGlobalIndex();

		un_Voxels.elements[ idx ].packedPos = (coord.x & 0xFF) | ((coord.y & 0xFF) << 8) | ((coord.z & 0xFF) << 16);
		un_Voxels.elements[ idx ].faceBits	= face_bits;

		// TODO: merge workgroup items into single draw call
	}

#endif
//-----------------------------------------------------------------------------
