// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compaction and deduplication IDs per workgroup.
	IDs in range 0..2^32
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
# 	include <glsl.h>
# 	define INIT_IDs
# 	define DEDUP
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		row_size_buf		= Buffer();
		RC<Buffer>		id_buf				= Buffer();
		RC<Buffer>		src_buf				= Buffer();
		RC<FeatureSet>	fs					= GetFeatureSet();
		const bool		has_sg_size_ctrl	= fs.hasSubgroupSizeControl();
		const uint		warp_size			= has_sg_size_ctrl ? fs.getMaxSubgroupSize() : GetSubgroupSize();
		const uint		warps_per_wg 		= 4;
		const uint		col_count 			= warp_size * warps_per_wg;
		RC<DynamicUInt>	row_count			= DynamicUInt();
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );

		id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count );

		src_buf.ArrayLayout(
			"SrcIdBuffer",
			"	int		id;",
			id_count );

		row_size_buf.ArrayLayout(
			"RowSizeBuffer",
			"	uint	count;",
			row_count );

		Slider( row_count,	"Count",	1,	32,		1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut(	"un_IdBuf",			id_buf );
			pass.Slider(	"iDensity",			0.0,	1.0,		0.5 );
			pass.Slider(	"iRange",			10.0,	64.0,		32.0 );
			pass.Slider(	"iIdOffsetPOT",		0,		32-6,		0 );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
		}{
			ClearBuffer( row_size_buf, 0 );
			ClearBuffer( src_buf, 0 );

			RC<ComputePass>		pass = ComputePass( "", "DEDUP; SG_COUNT="s+warps_per_wg );
			pass.ArgInOut(	"un_RowSize",		row_size_buf );
			pass.ArgOut(	"un_SrcIdBuf",		src_buf );
			pass.ArgIn(		"un_IdBuf",			id_buf );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.ArgIn(		"un_RowSize",		row_size_buf );
			pass.ArgIn(		"un_SrcIdBuf",		src_buf );
			pass.ArgIn(		"un_IdBuf",			id_buf );
			pass.Output(	"out_Color",		rt );
			pass.Constant(	"iRows",			row_count );
			pass.Constant(	"iColSize",			col_count );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT_IDs
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const uint	idx		= GetGlobalIndex();
		float		x		= DHash11( GetGlobalCoordUNorm().x * 100.0 );
		int			val		= int(DHash11( GetGlobalCoordUNorm().x * 222.0 ) * iRange) << iIdOffsetPOT;

		un_IdBuf.elements[idx].id = (x < iDensity ? val : -1);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DEDUP
	#include "InvocationID.glsl"

	WGShared int  	s_Unique 	[gl.WorkGroupSize.x];
	WGShared uint 	s_SgCount 	[SG_COUNT];
	WGShared uint 	s_KeepCount	[SG_COUNT];
	WGShared uint 	s_SgBase 	[SG_COUNT];


	void  Main ()
	{
		const uint	wg_size	= gl.WorkGroupSize.x;
		const uint	wg		= GetGroupIndex();
		const uint	lid		= GetGlobalIndex() - wg * wg_size;
		const uint	sg_size	= gl.subgroup.Size;
		const uint	lane	= gl.subgroup.Index;
		const uint	sg		= lid / sg_size;
		int			src_id	= un_IdBuf.elements[ GetGlobalIndex() ].id;

		// Phase 1: same extract-and-retire loop, but nobody returns.
		// remaining is subgroup-uniform, so the while condition is uniform.
		sg_uniform 	uint4	remaining	= gl.subgroup.Ballot( src_id >= 0 );
		uint				found		= 0;

		// uniform control flow
		while ( gl.subgroup.BallotBitCount(remaining) > 0 )
		{
			uint			first	= gl.subgroup.BallotFindLSB( remaining );
			sg_uniform int	id		= gl.subgroup.Broadcast( src_id, first );

			// non-uniform
			if ( lane == first )
				s_Unique[ sg * sg_size + found ] = id;

			remaining = remaining & ~gl.subgroup.Ballot( src_id == id );
			++found;
		}

		if ( lane == 0 )
			s_SgCount[sg] = found;

		gl.WorkgroupBarrier();


		// Phase 2: drop ids already emitted by an earlier subgroup.
		sg_uniform uint	sg_count	= s_SgCount[ sg ];
		bool			keep		= lane < sg_count;
		int				uid			= keep ? s_Unique[ sg * sg_size + lane ] : -1;

		// non-uniform
		if ( keep )
		{
			for (uint ps = 0; ps < sg and keep; ++ps)
			{
				uint 	base 	= ps * sg_size;
				uint	pc  	= s_SgCount[ ps ];

				for (uint j = 0; j < pc; ++j)
				{
					if ( s_Unique[ base + j ] == uid )
					{
						keep = false;
						break;
					}
				}
			}
		}

		sg_uniform uint4 	kb = gl.subgroup.Ballot( keep );
		if ( lane == 0 )
			s_KeepCount[sg] = gl.subgroup.BallotBitCount( kb );

		gl.WorkgroupBarrier();                   // do not overwrite 's_SgCount' before this


		if ( lid == 0 )
		{
			uint 	sum = 0;
			uint 	nsg = wg_size / sg_size;

			for (uint i = 0; i < nsg; ++i)
			{
				s_SgBase[i]	= sum;
				sum 		+= s_KeepCount[i];
			}
			un_RowSize.elements[ wg ].count = sum;
		}
		gl.WorkgroupBarrier();

		if ( keep )
		{
			uint slot = wg * wg_size + s_SgBase[sg] + gl.subgroup.BallotExclusiveBitCount( kb );
			un_SrcIdBuf.elements[ slot ].id = uid;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	float4  DrawInputData (float x, float x1, float y, float y1)
	{
		uint	row		= uint( (1.0 + y) * iRows );
		uint	row1	= uint( (1.0 + y1) * iRows );
		uint	col		= Clamp( uint( x * iColSize ), 0, iColSize-1 );
		uint	col1	= uint( x1 * iColSize );
		float4	color	= float4(0.0);

		if ( row < iRows )
		{
			uint	idx	= col + row * iColSize;
			int		id	= un_IdBuf.elements[ idx ].id;

			if ( id >= 0 )
				color.rgb = DHash31( float(id) );
		}

		if ( col != col1 )
			color = float4(0.2);

		if ( row != row1 )
			color = float4(0.8);

		return color;
	}


	float4  DrawHistogram (float x0, float x1, float y0, float y1)
	{
		uint	col		= uint( x0 * iColSize );
		uint	col1	= uint( x1 * iColSize );
		uint	row		= Clamp( uint( y0 * iRows ), 0, iRows-1 );
		uint	row1	= uint( y1 * iRows );
		uint	count	= un_RowSize.elements[ row ].count;
		float4	color	= float4(0.0);

		if ( col < count )
		{
			int	id = un_SrcIdBuf.elements[ col + row * iColSize ].id;
			color.rgb = DHash31( float(id) );
		}

		if ( col != col1 )
			color = float4(0.2);

		if ( row != row1 )
			color = float4(0.8);

		return color;
	}

	void  Main ()
	{
		float2	uv0		= GetGlobalCoordUNorm().xy;
		float2	uv1		= GetGlobalCoordUNorm( int3(1) ).xy;
		float	y		= ToSNorm( uv0.y );
		float	y1		= ToSNorm( uv1.y );

		if ( y > 0.0 )
		{
			out_Color = DrawHistogram( uv0.x, uv1.x, y, y1 );
		}
		else
			out_Color = DrawInputData( uv0.x, uv1.x, y, y1 );

		if ( Abs(y) < 0.01 )
			out_Color = float4(0.8);
	}

#endif
//-----------------------------------------------------------------------------