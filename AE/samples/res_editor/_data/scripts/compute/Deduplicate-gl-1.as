// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compaction and deduplication IDs in large buffer.
	IDs in range limited by 'max_values' constant.
	Used bit mask for all unique values.
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
		RC<Buffer>		bit_buf				= Buffer();
		RC<FeatureSet>	fs					= GetFeatureSet();
		const bool		has_sg_size_ctrl	= fs.hasSubgroupSizeControl();
		const uint		warp_size			= has_sg_size_ctrl ? fs.getMaxSubgroupSize() : GetSubgroupSize();
		const uint		warps_per_wg 		= 4;
		const uint		col_count 			= warp_size * warps_per_wg;
		const uint 		max_values			= 1024;
		RC<DynamicUInt>	row_count			= DynamicUInt();
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );
		RC<DynamicUInt>	enable_dedup		= DynamicUInt();

		id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count );

		src_buf.ArrayLayout(
			"SrcIdBuffer",
			// element
			"	int		id;",
			// static
			"	uint	count;",	// atomic
			id_count );

		bit_buf.ArrayLayout(
			"BitfieldBuffer",
			"	uint	mask;",		// atomic
			max_values / 32 );

		Slider( row_count,		"Count",	1,	32,		1 );
		Slider( enable_dedup,	"Enable",	0,	1,		1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut(	"un_IdBuf",			id_buf );
			pass.Slider(	"iDensity",			0.0,	1.0,				0.5 );
			pass.Slider(	"iRange",			10.0,	float(max_values),	32.0 );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "DEDUP; CAP_IN_WORDS="s+(max_values/32) );
			pass.ArgOut(	"un_SrcIdBuf",		src_buf );
			pass.ArgIn(		"un_IdBuf",			id_buf );
			pass.ArgOut(	"un_BitBuf",		bit_buf );
			pass.Slider(	"iVariant",			0,	1 );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
			pass.AddFlag( EPassFlags::ZeroContentOfArgOut );
			pass.EnableIfEqual( enable_dedup, 1 );
		}{
			RC<Postprocess>		pass = Postprocess();
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
		int			val		= int(DHash11( GetGlobalCoordUNorm().x * 222.0 ) * iRange);

		un_IdBuf.elements[idx].id = (x < iDensity ? val : -1);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DEDUP
	#include "Atomic.glsl"
	#include "InvocationID.glsl"


	const uint 	c_WordCapacity = CAP_IN_WORDS;


	void  Deduplicate_v1 ()
	{
		int		src_id	= un_IdBuf.elements[ GetGlobalIndex() ].id;
		uint	word 	= src_id / 32;

		if ( word < c_WordCapacity )
		{
			uint 	new_bit	 = 1u << (src_id % 32);
			uint	old_bits = gl.AtomicOr( INOUT un_BitBuf.elements[ word ].mask, new_bit );
			bool	is_new 	 = (old_bits & new_bit) == 0;

			if ( is_new )
			{
				uint	pos;
				SubgroupAtomicInc( OUT pos, INOUT un_SrcIdBuf.count );

				un_SrcIdBuf.elements[ pos ].id = src_id;
			}
		}
	}


	WGShared uint 	s_UsedBits [c_WordCapacity];

	// optimized
	void  Deduplicate_v2 ()
	{
		// initialize
		int	lid = GetLocalIndex();
		if ( lid < c_WordCapacity )
		{
			s_UsedBits[lid] = 0;
		}
		gl.WorkgroupBarrier();


		int		src_id	= un_IdBuf.elements[ GetGlobalIndex() ].id;
		uint	word 	= src_id / 32;

		if ( word < c_WordCapacity )
		{
			uint 	new_bit	 	= 1u << (src_id % 32);
			uint	old_bits 	= gl.AtomicOr( INOUT s_UsedBits[word], new_bit );
			bool	wg_unique	= (old_bits & new_bit) == 0;
			bool	is_new		= false;

			// unique per WG, try global bitfield
			if ( wg_unique )
			{
				old_bits = gl.AtomicOr( INOUT un_BitBuf.elements[ word ].mask, new_bit );
				is_new 	 = (old_bits & new_bit) == 0;
			}

			// single atomic per subgroup
			if ( is_new )
			{
				uint	pos;
				SubgroupAtomicInc( OUT pos, INOUT un_SrcIdBuf.count );

				un_SrcIdBuf.elements[ pos ].id = src_id;
			}
		}
	}


	void  Main ()
	{
		switch ( iVariant )
		{
			case 0 :	Deduplicate_v1(); break;
			case 1 :	Deduplicate_v2(); break;
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
		uint 	idx 	= col + row * iColSize;
		uint 	row2 	= idx / iColSize;
		uint	count	= un_SrcIdBuf.count;
		float4	color	= float4(0.0);

		if ( idx < count and row == row2 )
		{
			int	id = un_SrcIdBuf.elements[ idx ].id;
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