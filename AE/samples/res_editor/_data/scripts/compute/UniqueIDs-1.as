// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Each warp will find set of unique IDs and store it to the dst buffer.
	This is similar as some GPUs handle non-uniform indices, it also known as waterfall loop.

	Warning: version with 'subgroupElect()' (mode=0) works only on NV RTX because of independent thread scheduling (?)

	https://github.com/KhronosGroup/Vulkan-Guide/blob/main/chapters/extensions/VK_KHR_shader_subgroup_uniform_control_flow.adoc
	https://docs.vulkan.org/features/latest/features/proposals/VK_KHR_shader_maximal_reconvergence.html
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_IDs
#	define WARERFALL_LOOP	2
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		id_buf				= Buffer();
		RC<DynamicUInt>	max_id				= DynamicUInt();
		RC<DynamicUInt>	row_count			= DynamicUInt();
		const bool		has_subgroup_size	= GetFeatureSet().hasSubgroupSizeControl();
		const uint		local_size			= has_subgroup_size ? GetFeatureSet().getMaxSubgroupSize() : GetSubgroupSize();
		const uint		col_count			= local_size;
		RC<DynamicUInt>	id_count			= row_count.Mul( col_count );
		RC<DynamicUInt>	mode				= DynamicUInt();

		id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count.Mul(2) );

		Slider( row_count,	"Count",	1,	32,		20 );
		Slider( max_id,		"MaxID",	8,	64,		16 );
		Slider( mode,		"Mode",		0,	1,		1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut(	"un_IdBuf",		id_buf );
			pass.Slider(	"iRndID",		0.0,	1.0,	0.5 );
			pass.Constant(	"iMaxID",		max_id );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "WARERFALL_LOOP=1" );
			pass.ArgInOut(	"un_IdBuf",		id_buf );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
			if ( has_subgroup_size ) pass.SubgroupSize( local_size );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "WARERFALL_LOOP=2" );
			pass.ArgInOut(	"un_IdBuf",		id_buf );
			pass.LocalSize( col_count );
			pass.DispatchGroups( row_count );
			if ( has_subgroup_size ) pass.SubgroupSize( local_size );
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.ArgIn(		"un_IdBuf",		id_buf );
			pass.Output(	"out_Color",	rt );
			pass.Constant(	"iMaxID",		max_id );
			pass.Constant(	"idCount",		id_count );
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
		const uint	idx		= GetGlobalIndex();
		const uint	off		= GetGlobalIndexSize();
		float		x		= DHash11( (GetGlobalCoordUNorm().x + iRndID) * 100.0 );
		int			id		= int(x * iMaxID);

		un_IdBuf.elements[idx].id = id;
		un_IdBuf.elements[idx + off].id = -1;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef WARERFALL_LOOP
	#include "InvocationID.glsl"

	WGShared uint	s_Pos;

	void  Main ()
	{
		// initialize shared variable
		{
			if ( gl.subgroup.Index == 0 )
				s_Pos = 0;

			gl.subgroup.memoryBarrier.Shared();	// release
			gl.subgroup.ExecutionBarrier();
			gl.subgroup.memoryBarrier.Shared();	// acquire
		}

		const int	src_idx	= GetGlobalIndex();
		const int	off		= GetGlobalIndexSize();
		int			src_id	= un_IdBuf.elements[ src_idx ].id;

		if ( src_id < 0 )
			return;

		// find unique IDs using waterfall loop
	#if WARERFALL_LOOP == 1
		for (;;)
		{
			// get unique ID per subgroup
			int		id = gl.subgroup.BroadcastFirst( src_id );

			// if current lane equal to unique ID
			//[[branch]]  // by default all 'if' is a branch
			if ( id == src_id )
			//if ( gl.subgroup.Any( id == src_id ))		// NV will works as other GPUs
			{
				// only one lane which is equal to unique ID
				if ( gl.subgroup.Elect() )
				{
					uint	pos = gl.AtomicAdd( INOUT s_Pos, 1 );

					// select column
					pos += (src_idx & ~(gl.subgroup.Size-1));

					un_IdBuf.elements[ pos + off ].id = src_id;
				}
				break;
			}
		}
	#else
		for (;;)
		{
			// get unique ID per subgroup
			int		id = gl.subgroup.BroadcastFirst( src_id );

			// fix for AMD, used instead of Elect() in branch
			uint4	active_threads	= gl.subgroup.Ballot( id == src_id );
			uint	first_thread	= gl.subgroup.BallotFindLSB( active_threads );

			// only one lane which is equal to unique ID
			if ( gl.subgroup.Index == first_thread )
			{
				uint	pos = gl.AtomicAdd( INOUT s_Pos, 1 );

				// select column
				pos += (src_idx & ~(gl.subgroup.Size-1));

				un_IdBuf.elements[ pos + off ].id = src_id;
			}

			if ( id == src_id )
				break;
		}
	#endif
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
				color = Rainbow( float(id) / float(iMaxID) );
		}

		if ( row != row1 )
			color = float4(0.8);

		if ( Abs(y0) < 0.01 )
			color = float4(0.8);

		out_Color = color;
	}

#endif
//-----------------------------------------------------------------------------
