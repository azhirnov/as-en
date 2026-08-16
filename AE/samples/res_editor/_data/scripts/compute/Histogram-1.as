// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Calculate number of unique IDs per subgroup.
	IDs in range 0..SubgroupSize-1
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_IDs
#	define HISTOGRAM
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		his_buf				= Buffer();
		RC<Buffer>		id_buf				= Buffer();
		RC<FeatureSet>	fs					= GetFeatureSet();
		const uint		local_size			= 16; //GetSubgroupSize();

		const uint		local_size_bits		= HighBitIndex( local_size );
		const uint		col_count			= local_size;
		const uint		col_height			= local_size;
		const uint		his_size			= col_count;

		id_buf.ArrayLayout(
			"IdBuffer",
			"	uint		id;",
			col_count );

		his_buf.ArrayLayout(
			"HistogramBuffer",
			"	uint		count;"s +
			"	uint		errors;",
			his_size*2 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut(	"un_IdBuf",			id_buf );
			pass.Slider(	"iDensity",			0.0,	1.0,	0.5 );
			pass.Constant(	"iColSize",			col_count );
			pass.LocalSize( col_count );
			pass.DispatchGroups( 1 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "HISTOGRAM; WG_SIZE="+local_size+"; BIT_COUNT="+local_size_bits );
			pass.ArgInOut(	"un_Histogram",		his_buf );
			pass.ArgIn(		"un_IdBuf",			id_buf );
			pass.Constant(	"iColHeight",		col_height );
			pass.LocalSize( col_count );
			pass.DispatchGroups( 1 );
			pass.MinSubgroupSize( local_size );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.ArgIn(		"un_Histogram",		his_buf );
			pass.ArgIn(		"un_IdBuf",			id_buf );
			pass.Output(	"out_Color",		rt );
			pass.Slider(	"iCmp",				0,	1,		1 );
			pass.Constant(	"iHisSize",			his_size );
			pass.Constant(	"iRows",			1 );
			pass.Constant(	"iColSize",			col_count );
			pass.Constant(	"iColHeight",		col_height );
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
		float		x		= DHash11( GetGlobalCoordUNorm().x * 100.0 );
		uint		val		= HEHash11i( idx ) % iColSize;

		un_IdBuf.elements[idx].id = (x < iDensity ? val : ~0u);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef HISTOGRAM
	#include "Hash.glsl"
	#include "InvocationID.glsl"


	void  HistogramPass_v1 (const uint dstOffset, const uint idx)
	{
		uint	id = un_IdBuf.elements[ idx ].id & (WG_SIZE - 1);

		gl.AtomicAdd( INOUT un_Histogram.elements[ id + dstOffset ].count, 1 );
	}


	void  HistogramPass_v2 (const uint dstOffset, const uint idx)
	{
		uint	bucket			= un_IdBuf.elements[ idx ].id & (WG_SIZE - 1);
		uint	err				= 0;
		uint	active_threads	= gl.subgroup.Ballot( true ).x;

		uint	bit0_mask		= gl.subgroup.Ballot( HasBit( bucket, 0 )).x;
		uint	bit1_mask		= gl.subgroup.Ballot( HasBit( bucket, 1 )).x;
		uint	bit2_mask		= gl.subgroup.Ballot( HasBit( bucket, 2 )).x;
		uint	bit3_mask		= gl.subgroup.Ballot( HasBit( bucket, 3 )).x;

		uint	bucket_mask		= ToBitMask( WG_SIZE );
				bucket_mask		&= HasBit( bucket, 0 ) ? bit0_mask : ~bit0_mask;
				bucket_mask		&= HasBit( bucket, 1 ) ? bit1_mask : ~bit1_mask;
				bucket_mask		&= HasBit( bucket, 2 ) ? bit2_mask : ~bit2_mask;
				bucket_mask		&= HasBit( bucket, 3 ) ? bit3_mask : ~bit3_mask;

		if ( BitCount( bucket_mask ) == 0 )
			err |= 1;

		if ( ! HasBit( bucket_mask, idx ))
			err |= 2;

		if ( BitCount( active_threads ) != WG_SIZE )
			err |= 4;

		uint	histogram		= BitCount( bucket_mask );
		uint	first_thread	= LowBitIndex( bucket_mask );

		if ( gl.subgroup.Index == first_thread )
			un_Histogram.elements[ bucket + dstOffset ].count = histogram;

		un_Histogram.elements[ idx + dstOffset ].errors = err;
	}


	void  HistogramPass_v3 (const uint dstOffset, const uint idx)
	{
		uint	active_threads	= gl.subgroup.Ballot( true ).x;
		if ( BitCount( active_threads ) != WG_SIZE )
		{
			un_Histogram.elements[ idx + dstOffset ].errors = 1;
			un_Histogram.elements[ idx + dstOffset ].count = 0;
			return;  // error
		}

		uint	bucket			= un_IdBuf.elements[ idx ].id & (WG_SIZE - 1);

		uint	bit0_mask		= gl.subgroup.Ballot( HasBit( bucket, 0 )).x;
		uint	bit1_mask		= gl.subgroup.Ballot( HasBit( bucket, 1 )).x;
		uint	bit2_mask		= gl.subgroup.Ballot( HasBit( bucket, 2 )).x;
		uint	bit3_mask		= gl.subgroup.Ballot( HasBit( bucket, 3 )).x;

		uint	bucket_mask		= ToBitMask( WG_SIZE );
				bucket_mask		&= HasBit( idx, 0 ) ? bit0_mask : ~bit0_mask;
				bucket_mask		&= HasBit( idx, 1 ) ? bit1_mask : ~bit1_mask;
				bucket_mask		&= HasBit( idx, 2 ) ? bit2_mask : ~bit2_mask;
				bucket_mask		&= HasBit( idx, 3 ) ? bit3_mask : ~bit3_mask;

		uint	histogram		= BitCount( bucket_mask );

		un_Histogram.elements[ idx + dstOffset ].count = histogram;
	}


	void  Main ()
	{
		const uint	idx		= GetLocalIndex();
		const uint	off		= GetGlobalIndexSize();

		un_Histogram.elements[ idx ].count = 0;
		un_Histogram.elements[ idx ].errors = 0;

		un_Histogram.elements[ idx + off ].count = 0;
		un_Histogram.elements[ idx + off ].errors = 0;

		gl.subgroup.Barrier();

		HistogramPass_v1( 0, idx );
		//HistogramPass_v2( off, idx );
		HistogramPass_v3( off, idx );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "InvocationID.glsl"

	float4  DrawInputData (float x, float y)
	{
		uint	col		= Clamp( uint( x * iColSize ), 0, iColSize-1 );
		float4	color	= float4(0.0);

		if ( col < iColSize )
		{
			uint	idx	= col;
			uint	id	= un_IdBuf.elements[ idx ].id;

			if ( id < iColSize )
				color = Rainbow( float(id) / iColSize );
		}

		return color;
	}


	float4  DrawHistogram (uint off, float x0, float x1, float y)
	{
		uint	col		= Clamp( uint( x0 * iColSize ), 0, iColSize-1 );
		uint	col1	= uint( x1 * iColSize );
		float4	color	= float4(0.0);

		if ( col < iHisSize )
		{
			uint	id	= col;
			float	h	= float(un_Histogram.elements[ col + off ].count) / float(iColHeight);
			uint	err	= un_Histogram.elements[ col + off ].errors;

			y = 1.0 - y;
			if ( y < h )
				color = Rainbow( float(id) / iColSize );
			else
			{
				switch ( err )
				{
					case 1 :	color = float4(0.2, 0.0, 0.0, 1.0);		break;
					case 2 :	color = float4(0.0, 0.2, 0.0, 1.0);		break;
					case 3 :	color = float4(0.0, 0.0, 0.2, 1.0);		break;
				}
			}
		}

		if ( col != col1 )
			color = float4(0.8);

		return color;
	}


	void  Main ()
	{
		float	x0		= GetGlobalCoordUNorm().x;
		float	x1		= GetGlobalCoordUNorm( int3(1) ).x;
		float	y		= GetGlobalCoordSNorm().y;

		if ( y > 0.0 )
		{
			uint	off = iCmp == 0 ? 0 : iColSize;
			out_Color = DrawHistogram( off, x0, x1, y );
		}
		else
			out_Color = DrawInputData( x0, y );

		if ( Abs(y) < 0.01 )
			out_Color = float4(0.8);
	}

#endif
//-----------------------------------------------------------------------------
