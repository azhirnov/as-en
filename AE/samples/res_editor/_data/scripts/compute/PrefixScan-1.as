// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Move random distributed IDs to the left.
	Limited to subgroup size (16 - 64 threads depends on GPU).

	Bug in Intel driver: subgroup size in vulkan - 32, but gl.subgroup.Size = 16. Can be fixed by subgroup size control.
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
		const uint		col_count			= local_size;
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );

		id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count.Mul(2) );

		Slider( row_count,	"Count",	1,	32,		20 );

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
			pass.LocalSize( col_count );
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
		const int	src_idx	= GetGlobalIndex();
		const int	off		= GetGlobalIndexSize();
		int			src_id	= un_IdBuf.elements[ src_idx ].id;

		// builtin prefix sum
		uint		dst_idx = gl.subgroup.ExclusiveAdd( uint(src_id >= 0) );

		// select column
		dst_idx += (src_idx & ~(gl.subgroup.Size-1));

		if ( src_id >= 0 )
			un_IdBuf.elements[ dst_idx + off ].id = src_id;
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
			int		id	= un_IdBuf.elements[ idx + off ].id;

			if ( id >= 0 )
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
