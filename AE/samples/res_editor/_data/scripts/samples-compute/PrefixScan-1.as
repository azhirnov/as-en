// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Move random distributed IDs to the left.
	Limited to subgroup size (16 - 64 threads depends on GPU).
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
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		id_buf		= Buffer();
		RC<DynamicUInt>	count		= DynamicUInt();
		const uint		local_size	= 32;	// TODO: get subgroup size
		RC<DynamicUInt>	id_count	= count.Mul( local_size );

		id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count.Mul(2) );

		Slider( count,	"Count",	1,	32,		20 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut( "un_IdBuf",		id_buf );
			pass.Slider( "iDensity",		0.0,	1.0,	0.5 );
			pass.LocalSize( local_size );
			pass.DispatchGroups( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "PREFIX_SCAN" );
			pass.ArgInOut( "un_IdBuf",		id_buf );
			pass.LocalSize( local_size );
			pass.DispatchGroups( count );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.ArgIn(  "un_IdBuf",		id_buf );
			pass.Output( "out_Color",		rt );
			pass.Constant( "idCount",		id_count );
			pass.Constant( "iRows",			count );
			pass.Constant( "iColSize",		local_size );
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
		const int	idx	= GetGlobalIndex();
		float		x	= DHash11( GetGlobalCoordUNorm().x * 100.0 );

		un_IdBuf.elements[idx].id = (x < iDensity ? idx : -1);
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
		
		un_IdBuf.elements[ src_idx + off ].id = -1;

		uint		dst_idx = (src_idx & ~(gl.subgroup.Size-1)) + gl.subgroup.ExclusiveAdd( uint(src_id >= 0) );
		
		gl.subgroup.ExecutionBarrier();

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
