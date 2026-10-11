// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Simple radix sort.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_IDs
#	define RADIX_SORT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		id_buf				= Buffer();
		RC<DynamicUInt>	row_count			= DynamicUInt();
		const uint		local_size			= GetSubgroupSize() * 4;
		const uint		local_size_bits		= HighBitIndex( local_size );
		const uint		col_count			= local_size;
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );

		id_buf.ArrayLayout(
			"IdBuffer",
			"	uint		id;",
			id_count.Mul(3) );

		Slider( row_count,	"Count",	1,	32,		1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut( "un_IdBuf",		id_buf );
			pass.Slider( "iDensity",		0.0,	1.0,	0.5 );
			pass.Constant( "idCount",		id_count );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "RADIX_SORT; WG_SIZE="+local_size+"; BIT_COUNT="+local_size_bits );
			pass.ArgInOut( "un_IdBuf",		id_buf );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.ArgIn(  "un_IdBuf",		id_buf );
			pass.Output( "out_Color",		rt );
			pass.Constant( "idCount",		id_count );
			pass.Constant( "iRows",			row_count );
			pass.Constant( "iColSize",		col_count );
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
		const int	idx		= GetGlobalIndex();
		const uint	off1	= GetGlobalIndexSize();
		const uint	off2	= GetGlobalIndexSize()*2;
		float		x		= DHash11( GetGlobalCoordUNorm().x * 100.0 );
		uint		val		= HEHash11i( idx ) % idCount;

		un_IdBuf.elements[idx].id = (x < iDensity ? val : ~0u);
		un_IdBuf.elements[idx + off1].id = ~0u;
		un_IdBuf.elements[idx + off2].id = ~0u;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RADIX_SORT
	#include "Math.glsl"
	#include "InvocationID.glsl"

	WGShared uint	s_Histogram [WG_SIZE];	// atomic
	WGShared uint	s_DstOffset [WG_SIZE];


	void  RadixPass (const uint bitShift, const uint readOffset, const uint writeOffset)
	{
		const uint	local_id	= GetLocalIndex();
		const uint	mask		= WG_SIZE - 1;

		// clear histogram
		{
			s_Histogram[ local_id ] = 0;

			gl.WorkgroupBarrier();
		}

		// build histogram
		uint	current_key;
		uint	bucket;
		{
			uint	src_idx		= readOffset + local_id;
					current_key = un_IdBuf.elements[ src_idx ].id;
					bucket		= (current_key >> bitShift) & mask;

			gl.AtomicAdd( INOUT s_Histogram[bucket], 1 );
			gl.WorkgroupBarrier();
		}

		// exclusive prefix sum over histogram
		if ( local_id == 0 )
		{
			uint	sum = 0;
			for (uint i = 0; i < WG_SIZE; ++i)
			{
				s_DstOffset[i]	= sum;  // exclusive
				sum				+= s_Histogram[i];
			}
		}
		gl.WorkgroupBarrier();

		// scatter pass
		// find offset in same 'key & mask'
		uint	offset_in_bucket = 0;
		for (uint i = 0; i < local_id; ++i)
		{
			uint	src_idx			= readOffset + i;
			uint	other_bucket	= (un_IdBuf.elements[ src_idx ].id >> bitShift) & mask;

			if ( bucket == other_bucket )
				++offset_in_bucket;
		}

		// write back
		uint	dst_idx = writeOffset + s_DstOffset[ bucket ] + offset_in_bucket;
		un_IdBuf.elements[ dst_idx ].id = current_key;
		gl.WorkgroupBarrier();
	}


	void  Main ()
	{
		const uint	wg_offset		= GetGroupIndex() * WG_SIZE;
		uint		read_offset		= wg_offset;
		uint		write_offset	= GetGlobalIndexSize() + wg_offset;

		RadixPass( 0, read_offset, write_offset );

		read_offset = GetGlobalIndexSize()*2 + wg_offset;

		for (uint shift = BIT_COUNT; shift < 32; shift += BIT_COUNT)
		{
			Swap( read_offset, write_offset );

			RadixPass( shift, read_offset, write_offset );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float	x		= GetGlobalCoordUNorm().x;
		float	y0		= GetGlobalCoordSNorm().y;
		float	y1		= GetGlobalCoordSNorm( int3(1) ).y;
		float	a0		= y0 < 0.0 ? 1.0 + y0 : y0;
		float	a1		= y1 < 0.0 ? 1.0 + y1 : y1;

		uint	off		= y0 < 0.0 ? 0 : idCount;
		uint	row		= uint( a0 * iRows );
		uint	row1	= uint( a1 * iRows );
		uint	col		= Clamp( uint( x * iColSize ), 0, iColSize-1 );
		float4	color	= float4(0.0);

		if ( row < iRows )
		{
			uint	idx	= col + row * iColSize;
			uint	id	= un_IdBuf.elements[ idx + off ].id;

			if ( id < idCount )
				color = Rainbow( float(id) / idCount );
		}

		if ( row != row1 )
			color = float4(0.8);

		if ( Abs(y0) < 0.01 )
			color = float4(0.8);

		out_Color = color;
	}

#endif
//-----------------------------------------------------------------------------
