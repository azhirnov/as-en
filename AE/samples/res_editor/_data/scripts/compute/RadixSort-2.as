// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Optimized radix sort.
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
		RC<FeatureSet>	fs					= GetFeatureSet();
		const uint		local_size			= 16; //GetSubgroupSize();

		const uint		local_size_bits		= HighBitIndex( local_size );
		const uint		col_count			= local_size;
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );
		RC<DynamicUInt>	max_id				= DynamicUInt();

		id_buf.ArrayLayout(
			"IdBuffer",
			"	uint		id;",
			id_count.Mul(2) );

		Slider( row_count,	"Count",	1,			32,		1 );
		Slider( max_id,		"MaxID",	local_size,	1024,	678 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut(	"un_IdBuf",		id_buf );
			pass.Slider(	"iDensity",		0.0,	1.0,	0.5 );
			pass.Slider(	"iSeed",		0.0,	1.0,	0.0 );
			pass.Constant(	"idCount",		id_count );
			pass.Constant(	"iMaxID",		max_id );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "RADIX_SORT; WG_SIZE="+local_size+"; BIT_COUNT="+local_size_bits );
			pass.ArgInOut(	"un_IdBuf",		id_buf );
			pass.Slider(	"iOptimized",	0,	1,	1 );
			pass.Slider(	"iSteps",		0,	6,	1 );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
			pass.MinSubgroupSize( local_size );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.ArgIn(		"un_IdBuf",		id_buf );
			pass.Output(	"out_Color",	rt );
			pass.Constant(	"idCount",		id_count );
			pass.Constant(	"iMaxID",		max_id );
			pass.Constant(	"iRows",		row_count );
			pass.Constant(	"iColSize",		col_count );
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
		const uint	off		= GetGlobalIndexSize();
		float		x		= DHash11( GetGlobalCoordUNorm().x * 111.0 );
		uint		val		= HEHash11i( idx + uint(2946255.0 * iSeed) ) % iMaxID;

		un_IdBuf.elements[idx].id = (x < iDensity ? val : ~0u);
		un_IdBuf.elements[idx + off].id = ~0u;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RADIX_SORT
	#include "Math.glsl"
	#include "InvocationID.glsl"

	WGShared uint	s_Temp [WG_SIZE*2];
	WGShared uint	s_DstOffset [WG_SIZE];	// atomic

// optimization, iteration 1
#if 1
	WGShared uint	s_Histogram [WG_SIZE];	// atomic

	void  RadixPass_v1 (const uint bitShift, const uint readOffset, const uint writeOffset)
	{
		const uint	local_id	= GetLocalIndex();
		const uint	mask		= WG_SIZE - 1;

		// clear histogram
		{
			s_Histogram[ local_id ] = 0;

			gl.subgroup.Barrier();	// 1-5cy
		}

		// build histogram
		uint	current_key;
		uint	bucket;
		{
			uint	src_idx		= readOffset + local_id;
					current_key = s_Temp[ src_idx ];
					bucket		= (current_key >> bitShift) & mask;

			gl.AtomicAdd( INOUT s_Histogram[bucket], 1 );	// 10-30cy on NV (L1 cache), much slower on mobile (L2 cache)
			gl.subgroup.Barrier();							// 1-5cy
		}

		// exclusive prefix sum over histogram
		s_DstOffset[ local_id ] = gl.subgroup.ExclusiveAdd( s_Histogram[ local_id ]);	// 1-3cy
		gl.subgroup.Barrier();	// 1-5cy

		// scatter:
		// calculate offset for same 'key & mask'

		#if 0
			// same algorithm as in [UniqueIDs](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/compute/UniqueIDs-1.as)
			// may degradate to single thread if all buckets are unique
			for (;;)
			{
				uint	id				= gl.subgroup.BroadcastFirst( bucket );
				uint4	active_threads	= gl.subgroup.Ballot( id == bucket );
				uint	rank			= gl.subgroup.BallotExclusiveBitCount( active_threads );

				if ( id == bucket )
				{
					// write back
					uint	dst_idx = writeOffset + s_DstOffset[ bucket ] + rank;
					s_Temp[ dst_idx ] = current_key;
					break;
				}
			}
		#else
			uint	offset = gl.AtomicAdd( INOUT s_DstOffset[ bucket ], 1 );	// 10-30cy

			s_Temp[ writeOffset + offset ] = current_key;
		#endif
	}
#endif

// optimization, iteration 2
#if 1
	void  RadixPass_v2 (const uint bitShift, const uint readOffset, const uint writeOffset)
	{
		const uint	local_id	= GetLocalIndex();
		const uint	mask		= WG_SIZE - 1;
		const uint	src_idx		= readOffset + local_id;
		const uint	current_key = s_Temp[ src_idx ];
		const uint	bucket		= (current_key >> bitShift) & mask;

		// build histogram
		// see HistogramPass_v3 in [Histogram](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/compute/Histogram-1.as)
		uint	histogram;
		uint	rank;
		{
			uint	bit0_mask		= gl.subgroup.Ballot( HasBit( bucket, 0 )).x;	// 1-3cy
			uint	bit1_mask		= gl.subgroup.Ballot( HasBit( bucket, 1 )).x;
			uint	bit2_mask		= gl.subgroup.Ballot( HasBit( bucket, 2 )).x;
			uint	bit3_mask		= gl.subgroup.Ballot( HasBit( bucket, 3 )).x;

			// number of threads with same 'bucket == local_id'
			uint	lid_mask		= ToBitMask( WG_SIZE );
					lid_mask		&= HasBit( local_id, 0 ) ? bit0_mask : ~bit0_mask;
					lid_mask		&= HasBit( local_id, 1 ) ? bit1_mask : ~bit1_mask;
					lid_mask		&= HasBit( local_id, 2 ) ? bit2_mask : ~bit2_mask;
					lid_mask		&= HasBit( local_id, 3 ) ? bit3_mask : ~bit3_mask;

					histogram		= BitCount( lid_mask );

			// number of threads with same 'bucket' with lower ID than current thread
			uint	bucket_mask		= ExclusiveBitMask( local_id );
					bucket_mask		&= HasBit( bucket, 0 ) ? bit0_mask : ~bit0_mask;
					bucket_mask		&= HasBit( bucket, 1 ) ? bit1_mask : ~bit1_mask;
					bucket_mask		&= HasBit( bucket, 2 ) ? bit2_mask : ~bit2_mask;
					bucket_mask		&= HasBit( bucket, 3 ) ? bit3_mask : ~bit3_mask;

					rank			= BitCount( bucket_mask );
		}

	#if 0
		// exclusive prefix sum over histogram
		s_DstOffset[ local_id ] = gl.subgroup.ExclusiveAdd( histogram );	// scan 1-3cy, write is free
		gl.subgroup.Barrier();	// 1-5cy

		uint	offset	= s_DstOffset[ bucket ];	// read 20-30cy, x2 on bank conflict
				offset	+= rank;
	#else

		// exclusive prefix sum over histogram
		uint	lid_offset = gl.subgroup.ExclusiveAdd( histogram );		// 1-3cy

		uint	offset = gl.subgroup.Shuffle( lid_offset, bucket );		// 1-3cy
				offset	+= rank;
	#endif

		s_Temp[ writeOffset + offset ] = current_key;
	}
#endif


	void  Main ()
	{
		const uint	wg_offset		= GetGroupIndex() * WG_SIZE;
		const uint	lid				= GetLocalIndex();
		uint		read_offset		= 0;
		uint		write_offset	= WG_SIZE;

		switch ( iOptimized )
		{
			case 0 :
			{
				s_Temp[ read_offset + lid ] = un_IdBuf.elements[ wg_offset + lid ].id;

				RadixPass_v1( 0, read_offset, write_offset );

				for (uint shift = BIT_COUNT, step = 0; shift < 32 and step < iSteps; shift += BIT_COUNT)
				{
					Swap( read_offset, write_offset );

					RadixPass_v1( shift, read_offset, write_offset );
				}

				gl.subgroup.Barrier();
				un_IdBuf.elements[ GetGlobalIndexSize() + wg_offset + lid ].id = s_Temp[ write_offset + lid ];
				break;
			}

			case 1 :
			{
				s_Temp[ read_offset + lid ] = un_IdBuf.elements[ wg_offset + lid ].id;

				RadixPass_v2( 0, read_offset, write_offset );

				for (uint shift = BIT_COUNT, step = 0; shift < 32 and step < iSteps; shift += BIT_COUNT)
				{
					Swap( read_offset, write_offset );

					RadixPass_v2( shift, read_offset, write_offset );
				}

				gl.subgroup.Barrier();
				un_IdBuf.elements[ GetGlobalIndexSize() + wg_offset + lid ].id = s_Temp[ write_offset + lid ];
				break;
			}
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

			if ( id < iMaxID )
				color = Rainbow( float(id) / iMaxID );
			else
			if ( id == ~0u )
				color = float4(0.2);
		}

		if ( row != row1 )
			color = float4(0.8);

		if ( Abs(y0) < 0.01 )
			color = float4(0.8);

		out_Color = color;
	}

#endif
//-----------------------------------------------------------------------------
