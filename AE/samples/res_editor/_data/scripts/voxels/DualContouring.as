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
			"	uint	packedPos;"			// 3x 8 bit	- used only for selection
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

	bool  EdgeCrosses (float da, float db)
	{
		// NV-safe: named bools, treat 0 as outside
		const bool	in_a = da < 0.0;
		const bool	in_b = db < 0.0;
		return in_a != in_b;
	}

#endif
//-----------------------------------------------------------------------------
#if SDF_TO_VOXELS == 1
	#include "InvocationID.glsl"

	const uint	max_points	= 12;

	GEN_SDF_NORMAL_6sp_FN( SDFNormal, Density )

	uint  CornerIndex (uint x, uint y, uint z)	{ return x | (y<<1) | (z<<2); }
	int3  CornerFromIndex (uint i)				{ return int3( uint3(i, i>>1, i>>2) & uint3(1) ); }

	float3  CellCornerToWorld (int3 cell, int3 corner, float3 cellSize)
	{
		// cell c occupies [c, c+1] in grid space, mapped to [-1, 1]
		return ToSNorm( float3(cell + corner) * cellSize );
	}

#if 0
	// GLSL mat3 is column-major: M[c][r]. Work in row-major temporaries.
	bool  Solve3 (const float3x3 A, const float3 b, out float3 x)
	{
		float	M[3][3];
		float	B[3] = { b.x, b.y, b.z };

		for (int r = 0; r < 3; ++r)
		for (int c = 0; c < 3; ++c)
			M[r][c] = A[c][r];

		for (int col = 0; col < 3; ++col)
		{
			int		piv = col;
			float	best = Abs( M[col][col] );
			for (int r = col + 1; r < 3; ++r)
			{
				float a = Abs( M[r][col] );
				if ( a > best ) { best = a; piv = r; }
			}
			if ( best < 1e-10 )
				return false;

			if ( piv != col )
			{
				for (int c = 0; c < 3; ++c)
				{
					float t = M[col][c];
					M[col][c] = M[piv][c];
					M[piv][c] = t;
				}
				float tb = B[col]; B[col] = B[piv]; B[piv] = tb;
			}

			float diag = M[col][col];
			for (int r = col + 1; r < 3; ++r)
			{
				float f = M[r][col] / diag;
				for (int c = col; c < 3; ++c)
					M[r][c] -= f * M[col][c];
				B[r] -= f * B[col];
			}
		}

		if ( Abs(M[2][2]) < 1e-10 )
			return false;

		x.z = B[2] / M[2][2];
		x.y = (B[1] - M[1][2] * x.z) / M[1][1];
		x.x = (B[0] - M[0][1] * x.y - M[0][2] * x.z) / M[0][0];
		return true;
	}

	float3  QEF (in float3 points[max_points], in float3 normals[max_points], uint k, int3 cell_coord, float3 inv_size)
	{
		// QEF: min sum (n_i · (x - p_i))^2   →   A x = b
		// Solve for offset from the mass point (much more stable).
		float3x3	A	= float3x3(0.0);
		float3		b	= float3(0.0);
		float3		avg	= float3(0.0);

		for (uint i = 0; i < k; ++i)
		{
			float3	n = normals[i];
			float3	p = points[i];
			avg += p;
			A[0] += n * n.x;
			A[1] += n * n.y;
			A[2] += n * n.z;
			b += n * Dot( n, p );
		}
		avg /= float(k);

		A[0][0] += 1e-6;
		A[1][1] += 1e-6;
		A[2][2] += 1e-6;

		float3	d;
		bool	ok	= Solve3( A, b - (A * avg), OUT d );
		float3	v	= ok ? (avg + d) : avg;

		float3	cell_min = CellCornerToWorld( cell_coord, int3(0), inv_size );
		float3	cell_max = CellCornerToWorld( cell_coord, int3(1), inv_size );

		v = Clamp( v, cell_min, cell_max );
		return v;
	}

#else

	void  JacobiRotate (inout float M[3][3], inout float V[3][3], int p, int q)
	{
		float	apq = M[p][q];
		if ( Abs(apq) < 1e-12 )
			return;

		float	app	= M[p][p];
		float	aqq	= M[q][q];
		float	tau	= (aqq - app) / (2.0 * apq);
		float	st	= tau >= 0.0 ? 1.0 : -1.0;
		float	t	= st / (Abs(tau) + Sqrt(1.0 + tau * tau));
		float	c	= 1.0 / Sqrt(1.0 + t * t);
		float	s	= t * c;

		M[p][p] = app - t * apq;
		M[q][q] = aqq + t * apq;
		M[p][q] = M[q][p] = 0.0;

		for (int r = 0; r < 3; ++r)
		{
			if ( r == p or r == q )
				continue;
			float	mrp = M[r][p];
			float	mrq = M[r][q];
			M[r][p] = M[p][r] = c * mrp - s * mrq;
			M[r][q] = M[q][r] = s * mrp + c * mrq;
		}

		for (int r = 0; r < 3; ++r)
		{
			float	vrp = V[r][p];
			float	vrq = V[r][q];
			V[r][p] = c * vrp - s * vrq;
			V[r][q] = s * vrp + c * vrq;
		}
	}

	// A is SPD. Truncate small eigenvalues so tangent directions stay at `avg`
	// (mass point). That is what makes a sphere/cylinder C0-smooth; keeping
	// large eigenvalues is what keeps sharp DC features.
	// Raise c_RelThresh if you still see grid creases; lower it to sharpen.
	float3  QefMinimize (float3x3 A, float3 b, float3 avg)
	{
		const float	c_RelThresh = 0.05;

		float	M[3][3];
		float	V[3][3];
		for (int r = 0; r < 3; ++r)
		for (int c = 0; c < 3; ++c)
		{
			M[r][c] = A[c][r];			// column-major → row-major
			V[r][c] = float(r == c);
		}

		for (int it = 0; it < 8; ++it)
		{
			JacobiRotate( M, V, 0, 1 );
			JacobiRotate( M, V, 0, 2 );
			JacobiRotate( M, V, 1, 2 );
		}

		float	e0 = M[0][0], e1 = M[1][1], e2 = M[2][2];
		float	emax	= Max( Abs(e0), Max( Abs(e1), Abs(e2) ) );
		float	thresh	= Max( emax * c_RelThresh, 1e-10 );

		float3	rhs	= b - (A * avg);
		float3	d	= float3(0.0);

		// V's columns are eigenvectors
		if ( Abs(e0) >= thresh ) { float3 u = float3(V[0][0], V[1][0], V[2][0]);  d += u * (Dot(u, rhs) / e0); }
		if ( Abs(e1) >= thresh ) { float3 u = float3(V[0][1], V[1][1], V[2][1]);  d += u * (Dot(u, rhs) / e1); }
		if ( Abs(e2) >= thresh ) { float3 u = float3(V[0][2], V[1][2], V[2][2]);  d += u * (Dot(u, rhs) / e2); }

		return avg + d;
	}

	float3  QEF (in float3 points[max_points], in float3 normals[max_points], uint k, int3 cell_coord, float3 inv_size)
	{
		float3x3	A	= float3x3(0.0);
		float3		b	= float3(0.0);
		float3		avg	= float3(0.0);

		for (uint i = 0; i < k; ++i)
		{
			float3	n	= normals[i];
			float	nl	= Length( n );
			if ( nl < 1e-8 )
				continue;
			n /= nl;

			float3	p = points[i];
			avg += p;
			A[0] += n * n.x;
			A[1] += n * n.y;
			A[2] += n * n.z;
			b += n * Dot( n, p );
		}
		avg /= float(k);

		float3	cell_min	= CellCornerToWorld( cell_coord, int3(0), inv_size );
		float3	cell_max	= CellCornerToWorld( cell_coord, int3(1), inv_size );
		float3	v			= QefMinimize( A, b, avg );

		// NEVER box-clamp: snapping to a cell face is a visible ridge.
		// Mass point is always inside the cell (convex hull of edge hits).
		if ( AnyLess( v, cell_min ) or AnyGreater( v, cell_max ) )
			v = avg;

		// one Newton step onto the isosurface — rejects if it would leave the cell
		{
			float3	n	= SDFNormal( v );
			float	nl	= Length( n );
			if ( nl > 1e-8 )
			{
				n /= nl;
				float3	v2 = v - n * Density( v );
				if ( AllGreaterEqual( v2, cell_min ) and AllLessEqual( v2, cell_max ) )
					v = v2;
			}
		}
		return v;
	}

#endif


	// Must match CornerIndex(x,y,z) = x | (y<<1) | (z<<2)
	const uint2	c_EdgeCorners [max_points] = {
		// X
		uint2( 0, 1 ),	// (0,0,0)-(1,0,0)
		uint2( 2, 3 ),	// (0,1,0)-(1,1,0)
		uint2( 4, 5 ),	// (0,0,1)-(1,0,1)
		uint2( 6, 7 ),	// (0,1,1)-(1,1,1)
		// Y
		uint2( 0, 2 ),	// (0,0,0)-(0,1,0)
		uint2( 1, 3 ),	// (1,0,0)-(1,1,0)
		uint2( 4, 6 ),	// (0,0,1)-(0,1,1)
		uint2( 5, 7 ),	// (1,0,1)-(1,1,1)
		// Z
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

		un_Voxels.elements[ idx ].packedPos	= (cell_coord.x & 0xFF) | ((cell_coord.y & 0xFF) << 8) | ((cell_coord.z & 0xFF) << 16);
		un_Voxels.elements[ idx ].idxCount	= 0;
		un_Voxels.elements[ idx ].pos		= float4(0.0);

		float	dist [8];
		float3	corner_wpos [8];

		for (uint i = 0; i < 8; ++i)
		{
			int3	c		= CornerFromIndex( i );
			corner_wpos[i]	= CellCornerToWorld( cell_coord, c, inv_size );
			dist[i]			= Density( corner_wpos[i] );
		}

		{
			uint	neg_cnt = 0;
			uint	pos_cnt = 0;
			for (int i = 0; i < 8; ++i)
			{
				neg_cnt += uint(dist[i] <  0.0);
				pos_cnt += uint(dist[i] >= 0.0);
			}
			if ( neg_cnt == 0 or pos_cnt == 0 )
				return;
		}

		float3	points [max_points];
		float3	normals [max_points];
		uint	k = 0;

		for (uint i = 0; i < max_points; ++i)
		{
			uint	a	= c_EdgeCorners[i].x;
			uint	b	= c_EdgeCorners[i].y;
			float	da	= dist[a];
			float	db	= dist[b];

			if ( ! EdgeCrosses( da, db ) )
				continue;

			float	t	= da / (da - db);			// da != db because signs differ
					t	= Clamp( t, 0.0, 1.0 );
			float3	p	= Lerp( corner_wpos[a], corner_wpos[b], t );

			points[k]  = p;
			normals[k] = SDFNormal( p );
			++k;
		}

		if ( k == 0 )
			return;

		un_Voxels.elements[ idx ].pos = float4(QEF( points, normals, k, cell_coord, inv_size ), 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#if SDF_TO_VOXELS == 2
	#include "InvocationID.glsl"

	int		g_IdxCount;
	int		g_Indices [18];


	int  CellIndex (int3 coord)
	{
		if ( AnyLess( coord, int3(0) ) or AnyGreaterEqual( coord, iGridSize ) )
			return -1;

		int		idx	= coord.x + coord.y * iGridSize.x + coord.z * iGridSize.x * iGridSize.y;
		if ( un_Voxels.elements[ idx ].pos.w < 0.5 )
			return -1;

		return idx;
	}

	float  SdfAtGridPoint (int3 gp, float3 cellSize)
	{
		return Density( ToSNorm( float3(gp) * cellSize ) );
	}

	void  EmitTri (int a, int b, int c)
	{
		if ( g_IdxCount + 3 > g_Indices.length() )	// was >=  → dropped the last tri
			return;

		g_Indices[ g_IdxCount+0 ] = a;
		g_Indices[ g_IdxCount+1 ] = b;
		g_Indices[ g_IdxCount+2 ] = c;
		g_IdxCount += 3;
	}

#if 0
	void  EmitQuad (int i00, int i10, int i01, int i11, bool flip)
	{
		if ( flip )
		{
			EmitTri( i00, i11, i10 );
			EmitTri( i00, i01, i11 );
		}
		else
		{
			EmitTri( i00, i10, i11 );
			EmitTri( i00, i11, i01 );
		}
	}
#else

	void  EmitQuad (int i00, int i10, int i01, int i11, bool flip)
	{
		float3	p00 = un_Voxels.elements[i00].pos.xyz;
		float3	p10 = un_Voxels.elements[i10].pos.xyz;
		float3	p01 = un_Voxels.elements[i01].pos.xyz;
		float3	p11 = un_Voxels.elements[i11].pos.xyz;

		float3	na0 = Cross( p10 - p00, p11 - p00 );
		float3	na1 = Cross( p11 - p00, p01 - p00 );
		float3	nb0 = Cross( p01 - p00, p10 - p00 );
		float3	nb1 = Cross( p10 - p11, p01 - p11 );

		float	la0 = Length( na0 ), la1 = Length( na1 );
		float	lb0 = Length( nb0 ), lb1 = Length( nb1 );

		float	scoreA	= (la0 > 1e-12 and la1 > 1e-12) ? Dot(na0, na1) / (la0 * la1) : -2.0;
		float	scoreB	= (lb0 > 1e-12 and lb1 > 1e-12) ? Dot(nb0, nb1) / (lb0 * lb1) : -2.0;

		if ( scoreA >= scoreB )
		{
			// diagonal 00-11
			if ( flip )	{ EmitTri( i00, i11, i10 );  EmitTri( i00, i01, i11 ); }
			else		{ EmitTri( i00, i10, i11 );  EmitTri( i00, i11, i01 ); }
		}
		else
		{
			// diagonal 10-01
			if ( flip )	{ EmitTri( i00, i01, i10 );  EmitTri( i10, i01, i11 ); }
			else		{ EmitTri( i00, i10, i01 );  EmitTri( i10, i11, i01 ); }
		}
	}
#endif

	void  Main ()
	{
		const int3		cell_coord	= GetGlobalCoord();
		const float3	inv_size	= 1.0 / float3(iGridSize);

		g_IdxCount = 0;

		if ( CellIndex( cell_coord ) < 0 )
			return;

		// Dual contouring: one quad per primal EDGE that changes sign.
		// The 4 cells that share that edge contribute their QEF vertices.

		// X-edge (x,y+1,z+1) → (x+1,y+1,z+1)
		if ( cell_coord.x + 1 < iGridSize.x )
		{
			float	d0 = SdfAtGridPoint( cell_coord + int3(0,1,1), inv_size );
			float	d1 = SdfAtGridPoint( cell_coord + int3(1,1,1), inv_size );

			if ( EdgeCrosses( d0, d1 ))
			{
				int	i0 = CellIndex( cell_coord );
				int	i1 = CellIndex( cell_coord + int3(0,1,0) );
				int	i2 = CellIndex( cell_coord + int3(0,0,1) );
				int	i3 = CellIndex( cell_coord + int3(0,1,1) );

				if ( i0 >= 0 and i1 >= 0 and i2 >= 0 and i3 >= 0 )
					EmitQuad( i0, i1, i2, i3, d0 >= 0.0 );
			}
		}

		// Y-edge (x+1,y,z+1) → (x+1,y+1,z+1)
		if ( cell_coord.y + 1 < iGridSize.y )
		{
			float	d0 = SdfAtGridPoint( cell_coord + int3(1,0,1), inv_size );
			float	d1 = SdfAtGridPoint( cell_coord + int3(1,1,1), inv_size );

			if ( EdgeCrosses( d0, d1 ))
			{
				int	i0 = CellIndex( cell_coord );
				int	i1 = CellIndex( cell_coord + int3(1,0,0) );
				int	i2 = CellIndex( cell_coord + int3(0,0,1) );
				int	i3 = CellIndex( cell_coord + int3(1,0,1) );

				if ( i0 >= 0 and i1 >= 0 and i2 >= 0 and i3 >= 0 )
					EmitQuad( i0, i2, i1, i3, d0 >= 0.0 );
			}
		}

		// Z-edge (x+1,y+1,z) → (x+1,y+1,z+1)
		if ( cell_coord.z + 1 < iGridSize.z )
		{
			float	d0 = SdfAtGridPoint( cell_coord + int3(1,1,0), inv_size );
			float	d1 = SdfAtGridPoint( cell_coord + int3(1,1,1), inv_size );

			if ( EdgeCrosses( d0, d1 ))
			{
				int	i0 = CellIndex( cell_coord );
				int	i1 = CellIndex( cell_coord + int3(1,0,0) );
				int	i2 = CellIndex( cell_coord + int3(0,1,0) );
				int	i3 = CellIndex( cell_coord + int3(1,1,0) );

				if ( i0 >= 0 and i1 >= 0 and i2 >= 0 and i3 >= 0 )
					EmitQuad( i0, i1, i2, i3, d0 >= 0.0 );
			}
		}

		int	idx = GetGlobalIndex();
		un_Voxels.elements[ idx ].idxCount	= g_IdxCount;
		un_Voxels.elements[ idx ].indices	= g_Indices;
	}

#endif
//-----------------------------------------------------------------------------
