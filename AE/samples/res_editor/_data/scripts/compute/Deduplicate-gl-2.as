// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compaction and deduplication IDs in large buffer.
	* First pass: local deduplication from 'Deduplicate-wg.as'
	* Second pass:
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
# 	include <glsl.h>
# 	define INIT_IDs
# 	define LOCAL_DEDUP
# 	define GLOBAL_DEDUP
# 	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		chunk_size_buf		= Buffer();		// unique count per chunk (after local dedup)
		RC<Buffer>		src_id_buf			= Buffer();		// source ids
		RC<Buffer>		local_comp_ids		= Buffer();		// locally deduplicated ids, compacted per chunk
		RC<Buffer>		hash_buf			= Buffer();		// global hash table, -1 = empty
		RC<Buffer>		gl_comp_ids			= Buffer();		// globally unique ids, compacted
		RC<Buffer>		counter_buf			= Buffer();		// total unique count
		RC<FeatureSet>	fs					= GetFeatureSet();
		const bool		has_sg_size_ctrl	= fs.hasSubgroupSizeControl();
		const uint		warp_size			= has_sg_size_ctrl ? fs.getMaxSubgroupSize() : GetSubgroupSize();
		const uint		warps_per_wg 		= 4;
		const uint		chunk_size			= warp_size * warps_per_wg;		// elements per chunk (= workgroup)
		RC<DynamicUInt>	row_count			= DynamicUInt();
		RC<DynamicUInt>	chunk_count			= DynamicUInt();
		RC<DynamicUInt>	total_chunks		= row_count.Mul( chunk_count );
		RC<DynamicUInt>	id_count			= total_chunks.Mul( chunk_size );
		RC<DynamicUInt>	hash_size_pot		= DynamicUInt();
		RC<DynamicUInt>	hash_size			= hash_size_pot.Exp2();

		src_id_buf.ArrayLayout(
			"IdBuffer",
			"	int		id;",
			id_count );

		local_comp_ids.ArrayLayout(
			"LocalCompactIdBuffer",
			"	int		id;",
			id_count );

		chunk_size_buf.ArrayLayout(
			"ChunkSizeBuffer",
			"	uint	count;",
			total_chunks );

		hash_buf.ArrayLayout(
			"HashTable",
			"	int		id;",
			hash_size );

		gl_comp_ids.ArrayLayout(
			"GlobalCompactIdBuffer",
			"	int		id;",
			id_count );		// upper bound: all ids unique

		counter_buf.UseLayout(
			"CounterBuffer",
			"	uint	count;"			// atomic
			"	uint	failCount;" );	// atomic

		Slider( row_count,		"Rows",			1,	32,		4 );
		Slider( chunk_count,	"ChunksPerRow",	1,	28,		16 );
		Slider( hash_size_pot,	"HashSizePOT",	10,	26,		18 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_IDs" );
			pass.ArgOut(	"un_SrcIds",		src_id_buf );
			pass.Slider(	"iDensity",			0.0,	1.0,		0.5 );
			pass.Slider(	"iRangePOT",		8,		32,			20 );
			pass.LocalSize( chunk_size );
			pass.DispatchGroups( total_chunks );
		}{
			ClearBuffer( chunk_size_buf, 0 );
			ClearBuffer( counter_buf, 0 );
			ClearBuffer( hash_buf, uint(-1) );

			RC<ComputePass>		pass = ComputePass( "", "LOCAL_DEDUP; SG_COUNT="s+warps_per_wg );
			pass.ArgInOut(	"un_ChunkSize",			chunk_size_buf );
			pass.ArgOut(	"un_LocalCompactIds",	local_comp_ids );
			pass.ArgIn(		"un_SrcIds",			src_id_buf );
			pass.LocalSize( chunk_size );
			pass.DispatchGroups( total_chunks );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
		}{
			RC<ComputePass>		pass = ComputePass( "", "GLOBAL_DEDUP" );
			pass.ArgIn(		"un_ChunkSize",			chunk_size_buf );
			pass.ArgIn(		"un_LocalCompactIds",	local_comp_ids );
			pass.ArgInOut(	"un_Hash",				hash_buf );
			pass.ArgOut(	"un_GlobalCompactIds",	gl_comp_ids );
			pass.ArgInOut(	"un_Counter",			counter_buf );
			pass.Slider(	"iMaxProbePOT",			1,	10,		10 );
			pass.Constant(	"iHashSizePOT",			hash_size_pot );
			pass.Constant(	"iChunkSize",			chunk_size );
			pass.LocalSize( chunk_size );
			pass.DispatchGroups( total_chunks );
			if ( has_sg_size_ctrl ) pass.SubgroupSize( warp_size );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.ArgIn(		"un_SrcIds",			src_id_buf );
			pass.ArgIn(		"un_GlobalCompactIds",	gl_comp_ids );
			pass.ArgIn(		"un_Counter",			counter_buf );
			pass.Output(	"out_Color",			rt );
			pass.Constant(	"iRows",				row_count );
			pass.Constant(	"iColSize",				id_count.Div( row_count ) );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}

		RC<DynamicUInt>		fail_count = DynamicUInt();
		ReadBuffer( fail_count, counter_buf, "failCount" );
		Label( fail_count, 			"Fail count" );
		Label( hash_size.Div(1024), "Hash buf size (KB)" );

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
		float		scale 	= (iRangePOT >= 32 ? ~0u : (1u << iRangePOT));
		int			val		= int(DHash11( GetGlobalCoordUNorm().x * 22.2222 ) * scale);
					val		= Max( val, 0 );

		un_SrcIds.elements[idx].id = (x < iDensity ? val : -1);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef LOCAL_DEDUP
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
		int			src_id	= un_SrcIds.elements[ GetGlobalIndex() ].id;

		// Phase 1: extract unique ids within subgroup.
		sg_uniform 	uint4	remaining	= gl.subgroup.Ballot( src_id >= 0 );
		uint				found		= 0;

		while ( gl.subgroup.BallotBitCount(remaining) > 0 )
		{
			uint			first	= gl.subgroup.BallotFindLSB( remaining );
			sg_uniform int	id		= gl.subgroup.Broadcast( src_id, first );

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

		gl.WorkgroupBarrier();		// do not overwrite 's_SgCount' before this


		if ( lid == 0 )
		{
			uint 	sum = 0;
			uint 	nsg = wg_size / sg_size;

			for (uint i = 0; i < nsg; ++i)
			{
				s_SgBase[i]	= sum;
				sum 		+= s_KeepCount[i];
			}
			un_ChunkSize.elements[ wg ].count = sum;
		}
		gl.WorkgroupBarrier();

		if ( keep )
		{
			uint slot = wg * wg_size + s_SgBase[sg] + gl.subgroup.BallotExclusiveBitCount( kb );
			un_LocalCompactIds.elements[ slot ].id = uid;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GLOBAL_DEDUP
	#include "Atomic.glsl"
	#include "InvocationID.glsl"

	const int  HashInsert_NewSlot 			= 1;
	const int  HashInsert_AlreadyPresent 	= 2;
	const int  HashInsert_TableFull 		= 3;

	uint  HashInt (int id)
	{
		uint h = uint(id);
		h ^= h >> 16;	h *= 0x7FEB352Du;
		h ^= h >> 15;	h *= 0x846CA68Bu;
		h ^= h >> 16;
		return h;
	}

	int  HashInsert (int id, uint mask)
	{
		const uint	max_probe	= 1u << iMaxProbePOT;		// sanity limit; increase hash size if hit
		uint		slot		= HashInt( id ) & mask;

		for (uint probe = 0; probe < max_probe; ++probe)
		{
			int old = gl.AtomicCompSwap( INOUT un_Hash.elements[slot].id, -1, id );

			if ( old == -1 )	return HashInsert_NewSlot;
			if ( old == id )	return HashInsert_AlreadyPresent;

			slot = (slot + 1) & mask;			// linear probing
		}
		return HashInsert_TableFull;	// table (nearly) full — increase iHashSizePOT
	}


	void  Main ()
	{
		const uint	wg_size		= gl.WorkGroupSize.x;
		const uint	chunk		= GetGroupIndex();
		const uint	lid			= GetLocalIndex();
		const uint 	hash_mask	= (1u << iHashSizePOT) - 1;  // == un_Hash.elements.length()-1
		const uint	lane		= gl.subgroup.Index;
		const uint	count		= un_ChunkSize.elements[ chunk ].count;	// locally-unique count in this chunk

		// read only the compacted, locally-unique prefix of the chunk
		int		uid		= (lid < count) ? un_LocalCompactIds.elements[ chunk * iChunkSize + lid ].id : -1;
		int		hi_res	= (uid >= 0) ? HashInsert( uid, hash_mask ) : HashInsert_AlreadyPresent;
		bool	is_new	= (hi_res == HashInsert_NewSlot);
		bool	is_full	= (hi_res == HashInsert_TableFull);

		// compact into global output
		{
			uint	slot = 0;
			SubgroupAtomicAdd( OUT slot, INOUT un_Counter.count, uint(is_new) );

			if ( is_new )
				un_GlobalCompactIds.elements[ slot ].id = uid;
		}

		// fail counter
		SubgroupAtomicCounter( INOUT un_Counter.failCount, uint(is_full) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	float4  DrawInputData (float x, float x1, float y, float y1)
	{
		uint	row		= uint( (1.0 + y) * iRows );
		uint	row1	= uint( (1.0 + y1) * iRows );
		uint	col		= Clamp( uint(x * iColSize), 0, iColSize-1 );
		uint	col1	= uint( x1 * iColSize );
		float4	color	= float4(0.0);

		if ( row < iRows )
		{
			uint	idx	= col + row * iColSize;
			int		id	= un_SrcIds.elements[ idx ].id;

			if ( id >= 0 )
				color.rgb = DHash31( float(id & 0xFFFFFF) );
		}

		if ( col != col1 )
			color = float4(0.2);

		if ( row != row1 )
			color = float4(0.8);

		return color;
	}


	float4  DrawGlobalOutput (float x0, float x1, float y0, float y1)
	{
		uint	col		= uint( x0 * iColSize );
		uint	col1	= uint( x1 * iColSize );
		uint	row		= Clamp( uint(y0 * iRows), 0, iRows-1 );
		uint	row1	= uint( y1 * iRows );
		uint	count	= un_Counter.count;
		float4	color	= float4(0.0);

		// output is a single compacted 1D array; wrap it across rows for display
		uint	idx = col + row * iColSize;

		if ( idx < count )
		{
			int	id = un_GlobalCompactIds.elements[ idx ].id;
			color.rgb = DHash31( float(id & 0xFFFFFF) );
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
			out_Color = DrawGlobalOutput( uv0.x, uv1.x, y, y1 );
		}
		else
			out_Color = DrawInputData( uv0.x, uv1.x, y, y1 );

		if ( Abs(y) < 0.01 )
			out_Color = float4(0.8);
	}

#endif
//-----------------------------------------------------------------------------
