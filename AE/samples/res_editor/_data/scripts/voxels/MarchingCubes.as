// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	SDF to Marching Cubes

	reference:
	https://dl.acm.org/doi/pdf/10.1145/37402.37422
	https://www.dkgrdatasystems.com/news/marching-cubes
	https://www.boristhebrave.com/2018/04/15/marching-cubes-3d-tutorial/
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
		RC<Buffer>			cbuf		= Buffer();
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
			"	uint	tableOffset;"
			"	float3	vpos [12];",	// [0..1]
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
			cmd.vertexCount		= 5*3;	// 5 tris
			cmd.instanceCount	= Area( grid_size );
			geometry.Draw( cmd );

			scene.Add( geometry, float3(0.0, 0.0, 4.0) );
		}

		// init buffers
		{
			array<uint>		edge_table;	// [256]
			array<int>		tri_table;	// [256][16]
			array<int3>		uvw;		// [8]
			GetMarchingCubeTable( OUT edge_table, OUT tri_table, OUT uvw );

			cbuf.UIntArray(	"edgeTable",		edge_table );
			cbuf.IntArray(	"triangleTable",	tri_table );
			cbuf.IntArray(	"uvw",				uvw );
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
			pass.ArgIn(		"un_CBuf",		cbuf );
			pass.Constant(	"iMinDensity",	min_den );
			pass.LocalSize( local_size );
			pass.DispatchThreads( grid_size );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw voxels" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",	rt2,	RGBA32f(0.f) );
			pass.Output(					ds,		DepthStencil(1.f, 0) );
			pass.ArgIn(		"un_Voxels",	voxels );
			pass.ArgIn(		"un_CBuf",		cbuf );
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
			pass.Slider(	"iBlend",		0.0,	1.0,	0.5 );
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


	float	p_Density [2*2*2];

	float  LocalDensity (int3 coord)
	{
		coord = Clamp( coord, 0, 1 );
		return p_Density[ coord.x + coord.y*2 + coord.z*2*2 ];
	}


	void  SetVertex (uint i0, uint i1, out float3 pos)
	{
		int3	uvw0	= un_CBuf.uvw[i0];
		int3	uvw1	= un_CBuf.uvw[i1];

		float	den0	= LocalDensity( uvw0 );
		float	den1	= LocalDensity( uvw1 );

		float	diff	= den1 - den0;

		if ( Abs(diff) > 1.0e-5 )
		{
			float	f = Saturate( (iMinDensity - den0) / (den1 - den0) );
			pos	= Lerp( float3(uvw0), float3(uvw1), f );
		}
		else
			pos = Lerp( float3(uvw0), float3(uvw1), 0.5 );
	}


	void  Main ()
	{
		int3	coord		= GetGlobalCoord();
		int3	size		= GetGlobalSize();
		float3	inv_size	= 1.0 / float3(size);

		// cache density
		for (int z = 0; z < 2; ++z)
		for (int y = 0; y < 2; ++y)
		for (int x = 0; x < 2; ++x)
		{
			p_Density[ x + y*2 + z*2*2 ] = Density( int3(x,y,z) + coord, size, inv_size );
		}

		// choose triangle type
		uint	tri_type = 0;
		{
			for (uint i = 0; i < 8; ++i)
			{
				if ( LocalDensity( un_CBuf.uvw[i] ) > iMinDensity )
					tri_type |= 1u << i;
			}
		}

		// calculate vertex positions
		float3	tmp_pos [12];
		{
			const uint	edge = un_CBuf.edgeTable[ tri_type ];
			if ( HasBit( edge, 0 ))		SetVertex( 0, 1, OUT tmp_pos[0] );
			if ( HasBit( edge, 1 ))		SetVertex( 1, 2, OUT tmp_pos[1] );
			if ( HasBit( edge, 2 ))		SetVertex( 2, 3, OUT tmp_pos[2] );
			if ( HasBit( edge, 3 ))		SetVertex( 3, 0, OUT tmp_pos[3] );
			if ( HasBit( edge, 4 ))		SetVertex( 4, 5, OUT tmp_pos[4] );
			if ( HasBit( edge, 5 ))		SetVertex( 5, 6, OUT tmp_pos[5] );
			if ( HasBit( edge, 6 ))		SetVertex( 6, 7, OUT tmp_pos[6] );
			if ( HasBit( edge, 7 ))		SetVertex( 7, 4, OUT tmp_pos[7] );
			if ( HasBit( edge, 8 ))		SetVertex( 0, 4, OUT tmp_pos[8] );
			if ( HasBit( edge, 9 ))		SetVertex( 1, 5, OUT tmp_pos[9] );
			if ( HasBit( edge, 10 ))	SetVertex( 2, 6, OUT tmp_pos[10] );
			if ( HasBit( edge, 11 ))	SetVertex( 3, 7, OUT tmp_pos[11] );
		}

		int	idx = GetGlobalIndex();

		un_Voxels.elements[ idx ].packedPos		= (coord.x & 0xFF) | ((coord.y & 0xFF) << 8) | ((coord.z & 0xFF) << 16);
		un_Voxels.elements[ idx ].tableOffset	= tri_type * 16;
		un_Voxels.elements[ idx ].vpos			= tmp_pos;
	}

#endif
//-----------------------------------------------------------------------------
