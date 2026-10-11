// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compaction and deduplication IDs per subgroup.
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
		const uint		col_count			= has_sg_size_ctrl ? fs.getMaxSubgroupSize() : GetSubgroupSize();
		RC<DynamicUInt>	row_count			= DynamicUInt();
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );

		id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count );

		src_buf.ArrayLayout(
			"SrcIdBuffer",
			"	int		src_id;",
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

			RC<ComputePass>		pass = ComputePass( "", "DEDUP" );
			pass.ArgInOut(	"un_RowSize",		row_size_buf );
			pass.ArgOut(	"un_SrcIdBuf",		src_buf );
			pass.ArgIn(		"un_IdBuf",			id_buf );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( col_count );
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

	void  Main () [[maximally_reconverges]]
	{
		const int	src_idx	= GetGlobalIndex();
		int			src_id	= un_IdBuf.elements[ src_idx ].id;

		if ( src_id < 0 )
			return;

		uint	count = 1;
		for (;;)
		{
			sg_uniform int	id = gl.subgroup.BroadcastFirst( src_id );

		  #if 0
			if ( gl.subgroup.Elect() )
		  #else
			// fix for NV with independent thread scheduling
			uint4	active_threads	= gl.subgroup.Ballot( id == src_id );
			uint	first_thread	= gl.subgroup.BallotFindLSB( active_threads );
			if ( gl.subgroup.Index == first_thread )
		  #endif
			{
				uint	slot = GetGroupIndex() * gl.subgroup.Size + (count - 1);
				un_SrcIdBuf.elements[slot].src_id = id;
			}

			if ( id == src_id || count >= gl.subgroup.Size )
				break;

			++count;
		}

		uint	max_count = gl.subgroup.Max( count );

		if ( gl.subgroup.Elect() )
		{
			un_RowSize.elements[ GetGroupIndex() ].count = max_count;
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
			int	id = un_SrcIdBuf.elements[ col + row * iColSize ].src_id;
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