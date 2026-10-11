// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	SDF to Surface Nets (simplified Dual Contouring)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define RAYMARCH
#	define SDF_TO_VOXELS	1
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
		RC<DynamicUInt>		grid_pot	= DynamicUInt();
		RC<DynamicUInt3>	grid_size	= grid_pot.Exp2().XXX();
		RC<DynamicUInt>		vox_count	= grid_size.Volume();

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
			"	uint	packedPos;"			// 3x 8 bit - used only for selection, 8 bit - sign mask
			"	int		idxCount;"
			"	int		indices [18];"
			"	float4	pos;",				// [0..1]	// TODO: should be in separate array
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
			cmd.vertexCount		= 18;	// 9 tris
			cmd.InstanceCount( vox_count );
			geometry.Draw( cmd );

			scene.Add( geometry, float3(0.0, 0.0, 4.0) );
		}

		Slider( min_den,	"MinDensity",	0.0,	0.5,	0.0 );
		Slider( grid_pot,	"GridSizePOT",	3,		7,		5 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "RAYMARCH" );
			pass.Set( camera );
			pass.Output(	"out_Color",	rt1 );
			pass.Constant(	"iMinDensity",	min_den );
		}{
			RC<ComputePass>		pass = ComputePass( "", "SDF_TO_VOXELS=1" );
			pass.ArgInOut(	"un_Voxels",	voxels );
			pass.Constant(	"iMinDensity",	min_den );
			pass.Constant(	"iGridSize",	grid_size );
			pass.LocalSize( local_size );
			pass.DispatchThreads( grid_size );
		}{
			RC<ComputePass>		pass = ComputePass( "", "SDF_TO_VOXELS=2" );
			pass.ArgInOut(	"un_Voxels",	voxels );
			pass.Constant(	"iGridSize",	grid_size );
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
#if SDF_TO_VOXELS == 1
	#include "InvocationID.glsl"


	uint  CornerIndex (uint x, uint y, uint z)	{ return x | (y<<1) | (z<<2); }
	int3  CornerFromIndex (uint i)				{ return int3( uint3(i, i>>1, i>>2) & uint3(1) ); }


	float3  CellCornerToWorld (int3 cell, int3 corner, float3 cellSize)
	{
		// corner components are 0/1. No +0.5: cell (0..N-1) covers ToSNorm([0,1]).
		return ToSNorm( float3(cell + corner) * cellSize );
	}

	// Must match CornerFromIndex bit packing, not face-winding order.
	const uint2	c_EdgeCorners [12] = {
		// edges along x
		uint2( 0, 1 ),	// (0,0,0)-(1,0,0)
		uint2( 2, 3 ),	// (0,1,0)-(1,1,0)
		uint2( 4, 5 ),	// (0,0,1)-(1,0,1)
		uint2( 6, 7 ),	// (0,1,1)-(1,1,1)
		// edges along y
		uint2( 0, 2 ),	// (0,0,0)-(0,1,0)
		uint2( 1, 3 ),	// (1,0,0)-(1,1,0)
		uint2( 4, 6 ),	// (0,0,1)-(0,1,1)
		uint2( 5, 7 ),	// (1,0,1)-(1,1,1)
		// edges along z
		uint2( 0, 4 ),	// (0,0,0)-(0,0,1)
		uint2( 1, 5 ),	// (1,0,0)-(1,0,1)
		uint2( 2, 6 ),	// (0,1,0)-(0,1,1)
		uint2( 3, 7 )	// (1,1,0)-(1,1,1)
	};


	void  Main ()
	{
		const int3		cell_coord	= GetGlobalCoord();

		if ( AnyGreaterEqual( cell_coord, iGridSize ))
			return;

		const float3	inv_size	= 1.0 / float3(iGridSize);
		const int		idx			= GetGlobalIndex();

		un_Voxels.elements[ idx ].idxCount	= 0;
		for (int i = 0; i < 18; ++i)
			un_Voxels.elements[ idx ].indices[i] = -1;


		// sample corners
		float	dist [8];
		float3	corner_wpos [8];
		uint	sign_mask = 0;

		for (uint i = 0; i < 8; ++i)
		{
			int3	c		= CornerFromIndex( i );
			corner_wpos[i]	= CellCornerToWorld( cell_coord, c, inv_size );
			dist[i]			= Density( corner_wpos[i] );
			// <= 0 counts as inside so a surface through a corner is not dropped
			if ( dist[i] <= 0.0 )
				sign_mask |= (1u << i);
		}

		un_Voxels.elements[ idx ].packedPos =
			(uint(cell_coord.x) & 0xFF) |
			((uint(cell_coord.y) & 0xFF) << 8) |
			((uint(cell_coord.z) & 0xFF) << 16) |
			(sign_mask << 24);


		// Early reject if all same sign (no surface)
		{
			uint	neg_cnt = 0;
			uint	pos_cnt = 0;

			for (int i = 0; i < 8; ++i)
			{
				neg_cnt += uint(dist[i] <= 0.0);
				pos_cnt += uint(dist[i] > 0.0);
			}

			if ( neg_cnt == 0 or pos_cnt == 0 )
			{
				un_Voxels.elements[idx].pos = float4(0.0);
				return;
			}
		}

		// Gather intersection points on the 12 cube edges
		const uint	max_points	= 12;
		float3		points [max_points];
		float3		avg	= float3(0.0);
		uint		k = 0;

		for (uint i = 0; i < max_points; ++i)
		{
			uint	a	= c_EdgeCorners[i].x;
			uint	b	= c_EdgeCorners[i].y;
			float	da	= dist[a];
			float	db	= dist[b];

			if ( (da <= 0.0) != (db <= 0.0) )
			{
				// linear interpolation along edge
				float	t	= Saturate( da / (da - db) );
				float3	pa	= corner_wpos[a];
				float3	pb	= corner_wpos[b];
				float3	p	= Lerp( pa, pb, t );

				points[k] = p;
				avg += p;
				++k;
			}
		}

		// If somehow no edges crossed, invalid
		if ( k == 0 )
		{
			un_Voxels.elements[idx].pos = float4(0.0, 0.0, 0.0, 0.1);
			return;
		}

		avg /= float(k);

		// clamp to cell bounds to avoid crazy vertices
		float3	cell_min = CellCornerToWorld( cell_coord, int3(0), inv_size );
		float3	cell_max = CellCornerToWorld( cell_coord, int3(1), inv_size );
		avg = Clamp( avg, cell_min, cell_max );

		un_Voxels.elements[idx].pos = float4(avg, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#if SDF_TO_VOXELS == 2
	#include "InvocationID.glsl"

	int		g_IdxCount;
	int		g_Indices [3*2*3];	// 3 faces x 2 triangles x 3 indices


	int  CellIndex (int3 coord)
	{
		if ( AnyLess( coord, int3(0) ))
			return -1;

		if ( AnyGreaterEqual( coord, iGridSize ))
			return -1;

		int		idx	= coord.x + coord.y * iGridSize.x + coord.z * iGridSize.x * iGridSize.y;
		float4	v	= un_Voxels.elements[idx].pos;

		if ( v.w < 0.5 )
			return -1;

		return	idx;
	}


	void  EmitTri (int a, int b, int c)
	{
		if ( g_IdxCount+3 > g_Indices.length() )
			return;

		g_Indices[ g_IdxCount+0 ] = a;
		g_Indices[ g_IdxCount+1 ] = b;
		g_Indices[ g_IdxCount+2 ] = c;

		g_IdxCount += 3;
	}

	// Quad around the primal edge of this cell that runs toward corner (1,1,1)
	// along `edge_axis`. i1 = +axis_a, i2 = +axis_b, i3 = +both.
	// axis_a × axis_b must equal the edge direction so CCW faces +axis.
	void  TryEmitEdgeQuad (int3 cell_coord, int3 axis_a, int3 axis_b, uint corner_min, uint corner_max)
	{
		int	i0	= CellIndex( cell_coord );
		int	i1	= CellIndex( cell_coord + axis_a );
		int	i2	= CellIndex( cell_coord + axis_b );
		int	i3	= CellIndex( cell_coord + axis_a + axis_b );

		// need the 4 dual vertices that share this edge
		if ( i0 < 0 or i1 < 0 or i2 < 0 or i3 < 0 )
			return;

		uint	signs	= un_Voxels.elements[i0].packedPos >> 24;
		bool	inside0	= (signs & (1u << corner_min)) != 0;
		bool	inside1	= (signs & (1u << corner_max)) != 0;

		// no isosurface on this edge → no quad (this was the main hole/junk source)
		if ( inside0 == inside1 )
			return;

		// inside on the min end ⇒ surface faces +axis ⇒ keep CCW for +axis
		if ( inside0 )
		{
			EmitTri( i0, i1, i3 );
			EmitTri( i0, i3, i2 );
		}
		else
		{
			EmitTri( i0, i3, i1 );
			EmitTri( i0, i2, i3 );
		}
	}


	void  Main ()
	{
		const int3	cell_coord	= GetGlobalCoord();
		const int	idx			= GetGlobalIndex();

		if ( CellIndex( cell_coord ) < 0 )
			return;

		g_IdxCount = 0;

		// Each cell owns the 3 edges that touch its max corner (1,1,1).
		// The 4 cells that share that edge form the Surface Nets quad.

		// X-edge (0,1,1)-(1,1,1) = corners 6-7.  axis_a=Y, axis_b=Z  (Y×Z = X)
		TryEmitEdgeQuad( cell_coord, int3(0,1,0), int3(0,0,1), 6u, 7u );

		// Y-edge (1,0,1)-(1,1,1) = corners 5-7.  axis_a=Z, axis_b=X  (Z×X = Y)
		TryEmitEdgeQuad( cell_coord, int3(0,0,1), int3(1,0,0), 5u, 7u );

		// Z-edge (1,1,0)-(1,1,1) = corners 3-7.  axis_a=X, axis_b=Y  (X×Y = Z)
		TryEmitEdgeQuad( cell_coord, int3(1,0,0), int3(0,1,0), 3u, 7u );

		un_Voxels.elements[ idx ].idxCount = g_IdxCount;

		for (int i = 0; i < g_IdxCount; ++i) {
			un_Voxels.elements[ idx ].indices[i] = g_Indices[i];
		}
	}

#endif
//-----------------------------------------------------------------------------
