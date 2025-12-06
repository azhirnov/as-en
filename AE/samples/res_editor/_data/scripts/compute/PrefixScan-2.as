// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Move random distributed IDs to the left.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_IDs
#	define PREFIX_SCAN
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		id_buf				= Buffer();
		RC<DynamicUInt>	row_count			= DynamicUInt();
		const bool		has_subgroup_size	= GetFeatureSet().hasSubgroupSizeControl();
		const uint		local_size			= has_subgroup_size ? GetFeatureSet().getMaxSubgroupSize() : GetSubgroupSize();
		const uint		col_count			= local_size * 4;
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );

		id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count.Mul(2) );

		Slider( row_count,	"Count",	1,	32,		1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut( "un_IdBuf",		id_buf );
			pass.Slider( "iDensity",		0.0,	1.0,	0.5 );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "PREFIX_SCAN" );
			pass.ArgInOut( "un_IdBuf",		id_buf );
			pass.LocalSize( local_size );
			pass.DispatchGroups( row_count );
			if ( has_subgroup_size ) pass.SubgroupSize( local_size );
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
		const uint	off		= GetGlobalIndexSize();
		float		x		= DHash11( GetGlobalCoordUNorm().x * 100.0 );

		un_IdBuf.elements[idx].id = (x < iDensity ? idx : -1);
		un_IdBuf.elements[idx + off].id = -1;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PREFIX_SCAN
	#include "InvocationID.glsl"

	void  Main ()
	{
		const uint	src_idx	= GetGlobalIndex() * 4;
		const uint	off		= GetGlobalIndexSize() * 4;
		const uint	dst_off = (GetGlobalIndex() & ~(gl.subgroup.Size-1)) * 4 + off;

		int			src0_id	= un_IdBuf.elements[ src_idx + 0 ].id;
		int			src1_id	= un_IdBuf.elements[ src_idx + 1 ].id;
		int			src2_id	= un_IdBuf.elements[ src_idx + 2 ].id;
		int			src3_id	= un_IdBuf.elements[ src_idx + 3 ].id;

		const uint	cnt		= uint(src0_id >= 0) + uint(src1_id >= 0) + uint(src2_id >= 0) + uint(src3_id >= 0);

		// builtin prefix sum
		uint		i		= gl.subgroup.ExclusiveAdd( cnt );

		if ( src0_id >= 0 )	un_IdBuf.elements[ dst_off + i++ ].id = src0_id;
		if ( src1_id >= 0 )	un_IdBuf.elements[ dst_off + i++ ].id = src1_id;
		if ( src2_id >= 0 )	un_IdBuf.elements[ dst_off + i++ ].id = src2_id;
		if ( src3_id >= 0 )	un_IdBuf.elements[ dst_off + i++ ].id = src3_id;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "ColorSpace.glsl"
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
			int		id	= un_IdBuf.elements[ idx + off ].id;

			if ( id >= 0 )
				color = Rainbow( float(id) / idCount );
		}

		if ( row != row1 )
			color = float4(0.8);

		if ( Abs(y0) < 0.01 )
			color = float4(0.8);

		out_Color = ApplySRGBCurve( color );
	}

#endif
//-----------------------------------------------------------------------------
