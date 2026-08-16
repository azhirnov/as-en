// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	SDF to Dual Contouring

	reference:
	https://www.boristhebrave.com/2018/04/15/dual-contouring-tutorial/
	https://www.cs.wustl.edu/~taoju/research/interfree_paper_final.pdf
	https://www.cs.rice.edu/~jwarren/papers/dualcontour.pdf
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
			"	uint	packedPos;"			// 3x 8 bit
			"	int		idxCount;"
			"	int		indices [18];"
			"	float4	pos;",				// [0..1]
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
			cmd.vertexCount		= 18;	// 9 tris
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


	GEN_SDF_NORMAL_6sp_FN( SDFNormal, SDFScene )

	uint  CornerIndex (uint x, uint y, uint z)	{ return x | (y<<1) | (z<<2); }
	int3  CornerFromIndex (uint i)				{ return int3( uint3(i, i>>1, i>>2) & uint3(1) ); }


	float3  CellCornerToWorld (int3 cell, int3 corner, float3 cellSize)
	{
		// corner components are 0/1
		return ToSNorm( (float3(cell + corner) + 0.5) * cellSize );
	}


	// Solve 3x3 linear system A x = b (Gaussian elimination), small & simple.
	// Returns false if near-singular.
	bool  Solve3 (const float3x3 A, const float3 b, out float3 x)
	{
		// Augment
		float3x3	M = A;
		float3		B = b;

		// Pivot 0
		float	p0 = Abs( M[0][0] );
		int		r0 = 0;
		if ( Abs(M[1][0]) > p0 )	{ p0 = Abs(M[1][0]);  r0 = 1; }
		if ( Abs(M[2][0]) > p0 )	{ p0 = Abs(M[2][0]);  r0 = 2; }
		if ( p0 < 1e-10 )			return false;

		if ( r0 != 0 ) // swap rows r0 <-> 0
		{
			Swap( M[0], M[r0] );
			Swap( B[0], B[r0] );
		}

		// Eliminate below row0
		for (int r = 1; r < 3; ++r)
		{
			float f = M[r][0] / M[0][0];
			M[r] -= f * M[0];
			B[r] -= f * B[0];
		}

		// Pivot 1
		float	p1 = Abs( M[1][1] );
		int		r1 = 1;
		if ( Abs(M[2][1]) > p1 )	{ p1 = abs(M[2][1]); r1 = 2; }
		if ( p1 < 1e-10 )			return false;

		if ( r1 != 1 )
		{
			float3	t  = M[1];  M[1] = M[2];  M[2] = t;
			float	tb = B[1];  B[1] = B[2];  B[2] = tb;
		}

		// Eliminate below row1
		{
			float f = M[2][1] / M[1][1];
			M[2] -= f * M[1];
			B[2] -= f * B[1];
		}

		if ( Abs(M[2][2]) < 1e-10 )
			return false;

		// Back substitution
		x.z = B[2] / M[2][2];
		x.y = (B[1] - M[1][2]*x.z) / M[1][1];
		x.x = (B[0] - M[0][1]*x.y - M[0][2]*x.z) / M[0][0];
		return true;
	}


	const uint2	c_EdgeCorners [12] = {
		// edges along x
		uint2( 0, 1 ),	// CornerIndex(0,0,0), CornerIndex(1,0,0) ),
		uint2( 1, 2 ),	// CornerIndex(0,1,0), CornerIndex(1,1,0) ),
		uint2( 2, 3 ),	// CornerIndex(0,0,1), CornerIndex(1,0,1) ),
		uint2( 3, 0 ),	// CornerIndex(0,1,1), CornerIndex(1,1,1) ),
		// edges along y
		uint2( 4, 5 ),	// CornerIndex(0,0,0), CornerIndex(0,1,0) ),
		uint2( 5, 6 ),	// CornerIndex(1,0,0), CornerIndex(1,1,0) ),
		uint2( 6, 7 ),	// CornerIndex(0,0,1), CornerIndex(0,1,1) ),
		uint2( 7, 4 ),	// CornerIndex(1,0,1), CornerIndex(1,1,1) ),
		// edges along z
		uint2( 0, 4 ),	// CornerIndex(0,0,0), CornerIndex(0,0,1) ),
		uint2( 1, 5 ),	// CornerIndex(1,0,0), CornerIndex(1,0,1) ),
		uint2( 2, 6 ),	// CornerIndex(0,1,0), CornerIndex(0,1,1) ),
		uint2( 3, 7 )	// CornerIndex(1,1,0), CornerIndex(1,1,1) )
	};


	void  Main ()
	{
		const int3		cell_coord	= GetGlobalCoord();
		const int3		size		= GetGlobalSize();
		const float3	inv_size	= 1.0 / float3(size);
		const int		idx			= GetGlobalIndex();

		un_Voxels.elements[ idx ].packedPos = (cell_coord.x & 0xFF) | ((cell_coord.y & 0xFF) << 8) | ((cell_coord.z & 0xFF) << 16);
		un_Voxels.elements[ idx ].idxCount	= 0;


		// sample corners
		float	dist [8];
		float3	corner_wpos [8];

		for (uint i = 0; i < 8; ++i)
		{
			int3	c		= CornerFromIndex( i );
			corner_wpos[i]	= CellCornerToWorld( cell_coord, c, inv_size );
			dist[i]			= SDFScene( corner_wpos[i] ) - iMinDensity;
		}


		// Early reject if all same sign (no surface)
		{
			uint	neg_cnt = 0;
			uint	pos_cnt = 0;

			for (int i = 0; i < 8; ++i)
			{
				neg_cnt += uint(dist[i] < 0.0);
				pos_cnt += uint(dist[i] > 0.0);
			}

			if ( neg_cnt == 0 or pos_cnt == 0 )
			{
				un_Voxels.elements[idx].pos = float4(0.0);
				return;
			}
		}


		// Gather intersection points + normals
		const uint	max_points	= 12;
		float3		points [max_points];
		float3		normals [max_points];
		uint		k = 0;

		for (uint i = 0; i < max_points; ++i)
		{
			uint	a	= c_EdgeCorners[i].x & 7;
			uint	b	= c_EdgeCorners[i].y & 7;
			float	da	= dist[a];
			float	db	= dist[b];

			//if ( (da > 0.0) != (db > 0.0) )	// TODO: always false on NV
			{
				// linear interpolation along edge
				float	t	= Saturate( da / (da - db) );
				float3	pa	= corner_wpos[a];
				float3	pb	= corner_wpos[b];
				float3	p	= Lerp( pa, pb, t );

				points[k]  = p;
				normals[k] = SDFNormal( p );
				++k;
			}
		}


		// If somehow no edges crossed, invalid
		if ( k == 0 )
		{
			un_Voxels.elements[idx].pos = float4(0.0, 0.0, 0.0, 0.1);
			return;
		}


		// QEF: minimize sum_i (n_i dot (x - p_i))^2
		// This yields A x = b, where:
		// A = sum (n n^T), b = sum (n n^T p) = sum n * dot(n, p)
		float3x3	A	= float3x3(0.0);
		float3		b	= float3(0.0);
		float3		avg	= float3(0.0);

		for (uint i = 0; i < k; ++i)
		{
			float3	n = normals[i];
			float3	p = points[i];
			avg += p;

			// outer product n*n^T
			A += float3x3(
				n.x * n.x, n.x * n.y, n.x * n.z,
				n.y * n.x, n.y * n.y, n.y * n.z,
				n.z * n.x, n.z * n.y, n.z * n.z
			);
			b += n * Dot( n, p );
		}
		avg /= float(k);


		// Regularization to avoid singularities
		A[0][0] += 1e-6;
		A[1][1] += 1e-6;
		A[2][2] += 1e-6;

		float3	x;
		bool	ok = Solve3( A, b, OUT x );


		// if solve fails, use average of intersection points
		float3	v = ok ? x : avg;

		// clamp to cell bounds to avoid crazy vertices
		float3	cell_min = CellCornerToWorld( cell_coord, int3(0), inv_size );
		float3	cell_max = CellCornerToWorld( cell_coord, int3(1), inv_size );
		v = Clamp( v, cell_min, cell_max );

		un_Voxels.elements[idx].pos = float4(v, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#if SDF_TO_VOXELS == 2
	#include "InvocationID.glsl"

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


	float  SdfAtGridPoint (int3 gp, float3 cellSize)
	{
		float3	p = ToSNorm( (float3(gp) + 0.5) * cellSize );
		return	SDFScene( p );
	}


	int		p_IdxCount = 0;
	int		p_Indices [3*2*3];	// 3 faces x 2 triangles x 3 indices

	void  EmitTri (int a, int b, int c)
	{
		if ( p_IdxCount+3 >= p_Indices.length() )
			return;

		p_Indices[ p_IdxCount+0 ] = a;
		p_Indices[ p_IdxCount+1 ] = b;
		p_Indices[ p_IdxCount+2 ] = c;

		p_IdxCount += 3;
	}


	void  Main ()
	{
		const int3		cell_coord	= GetGlobalCoord();
		const float3	inv_size	= 1.0 / float3(GetGlobalSize());

		if ( CellIndex( cell_coord ) < 0 )
			return;

		// +X face
		if ( cell_coord.x + 1 < iGridSize.x )
		{
			/*int3	gp0		= cell_coord + int3(1,0,0);
			float	f00		= SdfAtGridPoint( gp0 + int3(0,0,0), inv_size );
			float	f01		= SdfAtGridPoint( gp0 + int3(0,1,0), inv_size );
			float	f10		= SdfAtGridPoint( gp0 + int3(0,0,1), inv_size );
			float	f11		= SdfAtGridPoint( gp0 + int3(0,1,1), inv_size );
			bool	any_neg	= (f00 < 0.0) or (f01 < 0.0) or (f10 < 0.0) or (f11 < 0.0);
			bool	any_pos	= (f00 > 0.0) or (f01 > 0.0) or (f10 > 0.0) or (f11 > 0.0);

			if ( any_neg and any_pos )*/
			{
				int	i0	= CellIndex( cell_coord );
				int	i1	= CellIndex( cell_coord + int3(0,1,0) );
				int	i2	= CellIndex( cell_coord + int3(0,0,1) );
				int	i3	= CellIndex( cell_coord + int3(0,1,1) );

				if ( i0 >= 0 and i1 >= 0 and i2 >= 0 and i3 >= 0 )
				{
					EmitTri( i0, i1, i3 );
					EmitTri( i0, i3, i2 );
				}
			}
		}

		// +Y face
		if ( cell_coord.y + 1 < iGridSize.y )
		{
			/*int3	gp0		= cell_coord + int3(0,1,0);
			float	f00		= SdfAtGridPoint( gp0 + int3(0,0,0), inv_size );
			float	f01		= SdfAtGridPoint( gp0 + int3(1,0,0), inv_size );
			float	f10		= SdfAtGridPoint( gp0 + int3(0,0,1), inv_size );
			float	f11		= SdfAtGridPoint( gp0 + int3(1,0,1), inv_size );
			bool	any_neg	= (f00 < 0.0) or (f01 < 0.0) or (f10 < 0.0) or (f11 < 0.0);
			bool	any_pos	= (f00 > 0.0) or (f01 > 0.0) or (f10 > 0.0) or (f11 > 0.0);

			if ( any_neg and any_pos )*/
			{
				int	i0	= CellIndex( cell_coord );
				int	i1	= CellIndex( cell_coord + int3(1,0,0) );
				int	i2	= CellIndex( cell_coord + int3(0,0,1) );
				int	i3	= CellIndex( cell_coord + int3(1,0,1) );

				if ( i0 >= 0 and i1 >= 0 and i2 >= 0 and i3 >= 0 )
				{
					EmitTri( i0, i3, i1 );
					EmitTri( i0, i2, i3 );
				}
			}
		}

		// +Z face
		if ( cell_coord.z + 1 < iGridSize.z )
		{
			/*int3	gp0		= cell_coord + int3(0,0,1);
			float	f00		= SdfAtGridPoint( gp0 + int3(0,0,0), inv_size );
			float	f01		= SdfAtGridPoint( gp0 + int3(1,0,0), inv_size );
			float	f10		= SdfAtGridPoint( gp0 + int3(0,1,0), inv_size );
			float	f11		= SdfAtGridPoint( gp0 + int3(1,1,0), inv_size );
			bool	any_neg	= (f00 < 0.0) or (f01 < 0.0) or (f10 < 0.0) or (f11 < 0.0);
			bool	any_pos	= (f00 > 0.0) or (f01 > 0.0) or (f10 > 0.0) or (f11 > 0.0);

			if ( any_neg and any_pos )*/
			{
				int	i0	= CellIndex( cell_coord );
				int	i1	= CellIndex( cell_coord + int3(1,0,0) );
				int	i2	= CellIndex( cell_coord + int3(0,1,0) );
				int	i3	= CellIndex( cell_coord + int3(1,1,0) );

				if ( i0 >= 0 and i1 >= 0 and i2 >= 0 and i3 >= 0 )
				{
					EmitTri( i0, i1, i3 );
					EmitTri( i0, i3, i2 );
				}
			}
		}

		int	idx = GetGlobalIndex();

		un_Voxels.elements[ idx ].idxCount	= p_IdxCount;
		un_Voxels.elements[ idx ].indices	= p_Indices;
	}

#endif
//-----------------------------------------------------------------------------
