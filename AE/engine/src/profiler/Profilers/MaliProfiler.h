// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for Mali GPU hardware performance counters.
	(x.x.x) - link to counter description in docs.
	By default it it link to 5th Gen performance counters guide, special key like V (Valhall), B (Bifrost) used for links to other architectures.

	[Performance counters description](https://github.com/azhirnov/cpu-gpu-arch/blob/main/gpu/ARM-Mali_PC.md)
*/

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class MaliProfilerClient;


	//
	// Mali GPU Profiler
	//

	class MaliProfiler
	{
	// types
	public:
		enum class ECounter : ushort
		{
		#define AE_MALI_COUNTERS( _visit_ )\
			_visit_( GPUActiveCy )				/* cycles	| GPU active cycles								(3.1.1)							*/\
			_visit_( GPUIRQActiveCy )			/* cycles	| GPU interrupt pending cycles					(3.1.6)							*/\
			_visit_( FragQueueJob )				/* jobs		| Fragment jobs																	*/\
			_visit_( FragQueueTask )			/* tasks	| Fragment tasks																*/\
			_visit_( FragQueueActiveCy )		/* cycles	| Fragment queue active cycles, Main phase queue active cycles			(3.1.3)	*/\
			_visit_( NonFragQueueJob )			/* jobs		| Non-fragment jobs																*/\
			_visit_( NonFragQueueTask )			/* tasks	| Non-fragment tasks															*/\
			_visit_( NonFragQueueActiveCy )		/* cycles	| Non-fragment queue active cycles, Binning phase queue active cycles	(3.1.2)	*/\
			_visit_( ResQueueJob )				/* jobs		| Reserved queue jobs															*/\
			_visit_( ResQueueTask )				/* tasks	| Reserved queue tasks															*/\
			_visit_( ResQueueActiveCy )			/* cycles	| Reserved active cycles														*/\
			_visit_( ExtBusWrBt )				/* beats	| Output external write beats													*/\
			_visit_( ExtBusRdBt )				/* beats	| Output external read beats													*/\
			_visit_( ExtBusRdStallCy )			/* cycles	| Output external read stall cycles												*/\
			_visit_( ExtBusWrStallCy )			/* cycles	| Output external write stall cycles											*/\
			_visit_( FragActiveCy )				/* cycles	| Fragment active cycles														*/\
			_visit_( FragRdPrim )				/* primitives| Fragment primitives loaded													*/\
			_visit_( FragThread )				/* threads	| Fragment threads																*/\
			_visit_( FragHelpThread )			/* threads	| Fragment helper threads														*/\
			_visit_( FragRastQd )				/* quads	| Rasterized fine quads															*/\
			_visit_( FragEZSTestQd )			/* quads	| Early ZS tested quads															*/\
			_visit_( FragEZSKillQd )			/* quads	| Early ZS killed quads															*/\
			_visit_( FragLZSTestTd )			/* threads	| Late ZS tested threads														*/\
			_visit_( FragLZSKillTd )			/* threads	| Late ZS killed threads														*/\
			_visit_( FragTile )					/* tiles	| Tile count																	*/\
			_visit_( FragTileKill )				/* tiles	| Killed unchanged tiles														*/\
			_visit_( NonFragActiveCy )			/* cycles	| Non-fragment active cycles													*/\
			_visit_( NonFragThread )			/* threads	| Non-fragment threads															*/\
			_visit_( CoreActiveCy )				/* cycles	| Execution core active cycles, (sum of all cores?)								*/\
			_visit_( EngInstr )					/* instructions | Executed instructions														*/\
			_visit_( LSIssueCy )				/* cycles	| Load/store unit issue cycles						(9.1.1)						*/\
			_visit_( TexInstr )					/* requests	| Texture instructions															*/\
			_visit_( TexFiltIssueCy )			/* cycles	| Texture filtering cycles							(8.1.1)						*/\
			_visit_( LSRdHitCy )				/* requests	| Load/store unit read hits														*/\
			_visit_( LSWrHitCy )				/* requests	| Load/store unit write hits													*/\
			_visit_( GeomTrianglePrim )			/* primitives| Triangle primitives															*/\
			_visit_( GeomPointPrim )			/* primitives| Point primitives																*/\
			_visit_( GeomLinePrim )				/* primitives| Line primitives																*/\
			_visit_( GeomFrontFacePrim )		/* primitives| Visible front-facing primitives												*/\
			_visit_( GeomBackFacePrim )			/* primitives| Visible back-facing primitives												*/\
			_visit_( GeomVisiblePrim )			/* primitives| Visible primitives								(4.1.3)						*/\
			_visit_( GeomFaceXYPlaneCullPrim )	/* primitives| Facing or XY plane test culled primitives									*/\
			_visit_( GeomZPlaneCullPrim )		/* primitives| Z plane culled primitives													*/\
			_visit_( TilerActiveCy )			/* cycles	| Tiler active cycles								(3.1.4)						*/\
			_visit_( GPUIRQUtil )				/* %		| Interrupt pending utilization						(3.2.5)						*/\
			_visit_( FragQueueUtil )			/* %		| Fragment queue utilization, Main phase queue utilization			(3.2.2)		*/\
			_visit_( NonFragQueueUtil )			/* %		| Non-fragment queue utilization, Binning phase queue utilization	(3.2.1)		*/\
			_visit_( ExtBusRdBy )				/* bytes	| Output external read bytes						(3.3.1)						*/\
			_visit_( ExtBusWrBy )				/* bytes	| Output external write bytes						(3.3.2)						*/\
			_visit_( ExtBusRdStallRate )		/* %		| Output external read stall rate					(3.4.1)						*/\
			_visit_( ExtBusWrStallRate )		/* %		| Output external write stall rate					(3.4.2)						*/\
			_visit_( TilerUtil )				/* %		| Tiler utilization									(3.2.3)						*/\
			_visit_( GeomTotalPrim )			/* primitives| Total input primitives							(4.1.1)						*/\
			_visit_( GeomVisibleRate )			/* %		| Visible primitives rate							(4.2.1)						*/\
			_visit_( GeomTotalCullPrim )		/* Total culled primitives							(4.1.2)					*/\
			_visit_( GeomFaceXYPlaneCullRate )	/* Facing or XY plane test cull						(4.2.2)					*/\
			_visit_( GeomZPlaneCullRate )		/* Z plane test cull rate							(4.2.3)					*/\
			_visit_( NonFragUtil )				/* Non-fragment utilization							(5.3.2)					*/\
			_visit_( NonFragThroughputCy )		/* Average cycles per non-fragment thread			(5.2.1)					*/\
			_visit_( FragUtil )					/* Fragment utilization								(5.3.3)					*/\
			_visit_( FragThroughputCy )			/* Average cycles per fragment thread				(5.2.2)					*/\
			_visit_( FragHelpTdRate )\
			_visit_( FragEZSTestRate )			/* Early ZS tested quad percentage					(4.5.1)					*/\
			_visit_( FragEZSKillRate )			/* Early ZS killed quad percentage					(4.5.3)					*/\
			_visit_( FragLZSTestRate )			/* Late ZS tested thread percentage					(4.5.5 ?)				*/\
			_visit_( FragLZSKillRate )			/* Late ZS killed thread percentage					(4.5.6 ?)				*/\
			_visit_( FragOverdraw )				/* Fragments per pixel								(4.4.3)					*/\
			_visit_( FragTileKillRate )			/* Unchanged tile kill rate, transaction elimination(6.3.5)					*/\
			_visit_( CoreUtil )					/* Execution core utilization						(5.3.5)					*/\
			_visit_( ALUUtil )					/* Arithmetic unit utilization						(6.1.1)					*/\
			_visit_( TexSample )				/* Texture samples															*/\
			_visit_( TexCPI )					/* Texture filtering cycles per instruction			(8.1.2)					*/\
			_visit_( TexUtil )					/* Texture unit utilization							(6.1.3)					*/\
			_visit_( TexIssueCy )				/* Texture unit issue cycles												*/\
			_visit_( LSUtil )					/* Load/store unit utilization						(6.1.4)					*/\
			_visit_( GPUPix )					/* total number of pixels that are shaded, assumes that all pixels in task are shaded (32x32, 5Gen: 64x64) (4.4.1)	*/\
			_visit_( GPUCyPerPix )				/* Average cycles per pixel							(4.4.2)					*/\
			_visit_( FragQueueWaitRdCy )		/* Fragment queue job descriptor read wait cycles							*/\
			_visit_( FragQueueWaitIssueCy )		/* Fragment queue job issue wait cycles										*/\
			_visit_( FragQueueWaitDepCy )		/* Fragment queue job dependency wait cycles								*/\
			_visit_( FragQueueWaitFinishCy )	/* Fragment queue job finish wait cycles									*/\
			_visit_( NonFragQueueWaitRdCy )		/* Non-fragment queue job descriptor read wait cycles						*/\
			_visit_( NonFragQueueWaitIssueCy )	/* Non-fragment queue job issue wait cycles									*/\
			_visit_( NonFragQueueWaitDepCy )	/* Non-fragment queue job dependency wait cycles							*/\
			_visit_( NonFragQueueWaitFinishCy )	/* Non-fragment queue job finish wait cycles								*/\
			_visit_( ResQueueWaitRdCy )			/* Reserved queue job descriptor read wait cycles							*/\
			_visit_( ResQueueWaitIssueCy )		/* Reserved queue job issue wait cycles										*/\
			_visit_( ResQueueWaitDepCy )		/* Reserved queue job dependency wait cycles								*/\
			_visit_( ResQueueWaitFinishCy )		/* Reserved queue job finish wait cycles									*/\
			_visit_( MMUL2Hit )					/* MMU L2 lookup TLB hits													*/\
			_visit_( MMUL2Rd )					/* MMU L2 table read requests												*/\
			_visit_( MMULookup )				/* MMU lookup requests														*/\
			_visit_( L2CacheLookup )\
			_visit_( L2CacheRdLookup )\
			_visit_( L2CacheWrLookup )\
			_visit_( FragFPKActiveCy )			/* Forward pixel kill buffer active cycles									*/\
			_visit_( LSRdCy )					/* Load/store unit read issues												*/\
			_visit_( LSWrCy )					/* Load/store unit write issues												*/\
			_visit_( LSAtomic )					/* Load/store unit atomic issues					(9.1.6)					*/\
			_visit_( TilerPosCacheHit )			/* Position cache hit requests												*/\
			_visit_( TilerPosCacheMiss )		/* Position cache miss requests												*/\
			_visit_( FragFPKBUtil )				/* Fragment FPK buffer utilization					(5.3.4)					*/\
			_visit_( FragQueueWaitFlushCy )		/* Fragment queue cache flush wait cycles									*/\
			_visit_( NonFragQueueWaitFlushCy )	/* Non-fragment queue cache flush wait cycles								*/\
			_visit_( ResQueueWaitFlushCy )		/* Non-fragment queue cache flush wait cycles								*/\
			_visit_( L2CacheFlush )				/* L2 cache flush requests													*/\
			_visit_( GeomSampleCullPrim )		/* Sample test culled primitives											*/\
			_visit_( TilerRdBt )				/* Output internal read beats												*/\
			_visit_( TilerWrBt )\
			_visit_( GeomPosShadTask )			/* Tiler position shading requests											*/\
			_visit_( TilerPosShadStallCy )		/* Tiler position shading stall cycles										*/\
			_visit_( TilerPosShadFIFOFullCy )	/* Tiler position FIFO full cycles											*/\
			_visit_( TilerVarCacheHit )			/* Varying cache hits														*/\
			_visit_( TilerVarCacheMiss )		/* Varying cache misses														*/\
			_visit_( GeomVarShadTask )			/* Tiler varying shading requests											*/\
			_visit_( TilerVarShadStallCy )		/* Tiler varying shading stall cycles										*/\
			_visit_( FragRastPrim )				/* Rasterized primitives													*/\
			_visit_( FragWarp )					/* Fragment warps									(5.1.2)					*/\
			_visit_( FragPartWarp )\
			_visit_( FragEZSUpdateQd )			/* Early ZS updated quads													*/\
			_visit_( FragLZSTestQd )			/* Late ZS tested quads														*/\
			_visit_( FragLZSKillQd )			/* Late ZS killed quads														*/\
			_visit_( FragOpaqueQd )				/* Occluding quads															*/\
			_visit_( NonFragTask )\
			_visit_( NonFragWarp )				/* Non-fragment warps								(5.1.1)					*/\
			_visit_( EngActiveCy )\
			_visit_( EngDivergedInstr )\
			_visit_( EngStarveCy )\
			_visit_( TexQuads )					/* Texture quads															*/\
			_visit_( TexQuadPass )\
			_visit_( TexQuadPassDescMiss )\
			_visit_( TexQuadPassMip )\
			_visit_( TexQuadPassTri )\
			_visit_( TexCacheFetch )\
			_visit_( TexCacheCompressFetch )\
			_visit_( TexCacheLookup )\
			_visit_( LSFullRd )					/* Load/store unit full read issues					(9.1.2)					*/\
			_visit_( LSPartRd )					/* Load/store unit partial read issues				(9.1.3)					*/\
			_visit_( LSFullWr )					/* Load/store unit full write issues				(9.1.4)					*/\
			_visit_( LSPartWr )					/* Load/store unit partial write issues				(9.1.5)					*/\
			_visit_( VarInstr )					/* Varying unit instructions												*/\
			_visit_( Var32IssueSlot )			/* 32-bit interpolation slots												*/\
			_visit_( Var16IssueSlot )			/* 16-bit interpolation slots												*/\
			_visit_( AttrInstr )				/* Attribute instructions													*/\
			_visit_( SCBusFFEL2RdBt )			/* Fragment front-end read beats from L2 cache								*/\
			_visit_( SCBusFFEExtRdBt )			/* Fragment front-end read beats from external memory						*/\
			_visit_( SCBusLSL2RdBt )			/* Load/store unit read beats from L2 cache									*/\
			_visit_( SCBusLSExtRdBt )			/* Load/store unit read beats from external memory							*/\
			_visit_( SCBusTexL2RdBt )			/* Texture unit read beats from L2 cache									*/\
			_visit_( SCBusTexExtRdBt )			/* Texture unit read beats from external memory								*/\
			_visit_( SCBusOtherL2RdBt )			/* Miscellaneous read beats from L2 cache									*/\
			_visit_( SCBusLSWBWrBt )			/* Load/store unit write-back write beats									*/\
			_visit_( SCBusTileWrBt )			/* Tile unit write beats to L2 memory system								*/\
			_visit_( SCBusLSOtherWrBt )			/* Load/store unit other write beats										*/\
			_visit_( MMUL3Rd )					/* MMU L3 table read requests												*/\
			_visit_( MMUL3Hit )					/* MMU L3 lookup TLB hits													*/\
			_visit_( MMUS2Lookup )				/* MMU stage 2 lookup requests												*/\
			_visit_( MMUS2L3Rd )				/* MMU stage 2 L3 lookup requests											*/\
			_visit_( MMUS2L2Rd )				/* MMU stage 2 L2 lookup requests											*/\
			_visit_( MMUS2L3Hit )				/* MMU stage 2 L3 lookup TLB hits											*/\
			_visit_( MMUS2L2Hit )				/* MMU stage 2 L2 lookup TLB hits											*/\
			_visit_( L2CacheRd )				/* Input internal read requests												*/\
			_visit_( L2CacheRdStallCy )			/* Input internal read stall cycles											*/\
			_visit_( L2CacheWr )				/* Input internal write requests											*/\
			_visit_( L2CacheWrStallCy )			/* Input internal write stall cycles										*/\
			_visit_( L2CacheSnp )				/* Input internal snoop requests											*/\
			_visit_( L2CacheSnpStallCy )		/* Input internal snoop stall cycles										*/\
			_visit_( L2CacheL1Rd )				/* Output internal read requests											*/\
			_visit_( L2CacheL1RdStallCy )		/* Output internal read stall cycles										*/\
			_visit_( L2CacheL1Wr )				/* Output internal write requests											*/\
			_visit_( L2CacheSnpLookup )			/* Input external snoop lookup requests										*/\
			_visit_( ExtBusRd )					/* Output external read transactions										*/\
			_visit_( ExtBusRdNoSnoop )			/* Output external ReadNoSnoop transactions									*/\
			_visit_( ExtBusRdUnique )			/* Output external ReadUnique transactions									*/\
			_visit_( ExtBusRdOTQ1 )				/* Output external outstanding reads 0-25%									*/\
			_visit_( ExtBusRdOTQ2 )				/* Output external outstanding reads 25-50%									*/\
			_visit_( ExtBusRdOTQ3 )				/* Output external outstanding reads 50-75%									*/\
			_visit_( ExtBusRdLat0 )				/* Output external read latency 0-127 cycles				(3.5.1)			*/\
			_visit_( ExtBusRdLat128 )			/* Output external read latency 128-191 cycles				(3.5.2)			*/\
			_visit_( ExtBusRdLat192 )			/* Output external read latency 192-255 cycles				(3.5.3)			*/\
			_visit_( ExtBusRdLat256 )			/* Output external read latency 256-319 cycles				(3.5.4)			*/\
			_visit_( ExtBusRdLat320 )			/* Output external read latency 320-383 cycles				(3.5.5)			*/\
			_visit_( ExtBusWr )					/* Output external write transactions										*/\
			_visit_( ExtBusWrNoSnoopFull )		/* Output external WriteNoSnoopFull transactions							*/\
			_visit_( ExtBusWrNoSnoopPart )		/* Output external WriteNoSnoopPartial transactions							*/\
			_visit_( ExtBusWrSnoopFull )		/* Output external WriteSnoopFull transactions								*/\
			_visit_( ExtBusWrSnoopPart )		/* Output external WriteSnoopPartial transactions							*/\
			_visit_( ExtBusWrOTQ1 )				/* Output external outstanding writes 0-25%									*/\
			_visit_( ExtBusWrOTQ2 )				/* Output external outstanding writes 25-50%								*/\
			_visit_( ExtBusWrOTQ3 )				/* Output external outstanding writes 50-75%								*/\
			_visit_( L2CacheIncSnp )			/* Input external snoop transactions										*/\
			_visit_( L2CacheIncSnpStallCy )		/* Input external snoop stall cycles										*/\
			_visit_( L2CacheRdMissRate )		/* L2 cache read miss rate													*/\
			_visit_( L2CacheWrMissRate )		/* L2 cache write miss rate													*/\
			_visit_( ExtBusRdLat384 )			/* Output external read latency 384+ cycles					(3.5.6)			*/\
			_visit_( ExtBusRdOTQ4 )				/* Output external outstanding reads 75-100%								*/\
			_visit_( ExtBusWrOTQ4 )				/* Output external outstanding writes 75-100%								*/\
			_visit_( GeomSampleCullRate )		/* Sample coverage test cull rate							(4.2.4)			*/\
			_visit_( GeomPosShadThread )		/* Position shader thread invocations						(4.3.1)			*/\
			_visit_( GeomPosShadThreadPerPrim )	/* Position threads per input primitive, should be < 1.5	(4.3.3)			*/\
			_visit_( TilerPosCacheHitRate )		/* Position cache hit rate													*/\
			_visit_( GeomVarShadThread )		/* Varying shader thread invocations						(4.3.2)			*/\
			_visit_( GeomVarShadThreadPerPrim )	/* Varying threads per input primitive						(4.3.4)			*/\
			_visit_( TilerVarCacheHitRate )\
			_visit_( FragOpaqueQdRate )			/* Occluding quad percentage												*/\
			_visit_( FragTransparentQd )		/* Non-occluding quads														*/\
			_visit_( FragShadedQd )				/* Shaded coarse quads														*/\
			_visit_( FragPartWarpRate )\
			_visit_( FragEZSUpdateRate )		/* Early ZS updated quad percentage							(4.5.2)			*/\
			_visit_( FragFPKKillQd )			/* Forward pixel kill killed quads											*/\
			_visit_( FragFPKKillRate )			/* Forward pixel kill killed quad percentage				(4.5.4)			*/\
			_visit_( EngDivergedInstrRate )		/* Warp divergence percentage								(6.2.2)			*/\
			_visit_( TexCacheUtil )\
			_visit_( TexMipInstrRate )\
			_visit_( TexCacheCompressFetchRate )\
			_visit_( TexTriInstrRate )\
			_visit_( Var32IssueCy )				/* 32-bit interpolation active cycles						(7.1.3)			*/\
			_visit_( Var16IssueCy )				/* 16-bit interpolation active cycles						(7.1.2)			*/\
			_visit_( VarIssueCy )				/* Varying unit issue cycles								(7.1.1)			*/\
			_visit_( VarUtil )					/* Varying unit utilization									(6.1.2)			*/\
			_visit_( SCBusFFEL2RdBy )			/* Front-end unit read bytes from L2 cache					(11.1.1)		*/\
			_visit_( SCBusFFEExtRdBy )			/* Front-end unit read bytes from external memory			(11.2.1)		*/\
			_visit_( SCBusLSL2RdBy )			/* Load/store unit read bytes from L2 cache					(11.1.2)		*/\
			_visit_( SCBusLSL2RdByPerRd )		/* Load/store unit bytes read from L2 per access cycle		(9.2.1)			*/\
			_visit_( SCBusLSExtRdBy )			/* Load/store unit read bytes from external memory			(11.2.2)		*/\
			_visit_( SCBusLSExtRdByPerRd )		/* Load/store unit bytes read from external memory per access cycle	(9.2.2)	*/\
			_visit_( SCBusTexL2RdBy )			/* Texture unit read bytes from L2 cache					(11.1.3)		*/\
			_visit_( SCBusTexL2RdByPerRd )		/* Texture unit bytes read from L2 per texture cycle		(8.2.1)			*/\
			_visit_( SCBusTexExtRdBy )			/* Texture unit read bytes from external memory				(11.2.3)		*/\
			_visit_( SCBusTexExtRdByPerRd )		/* Texture unit bytes read from external memory per texture cycle	(8.2.2)	*/\
			_visit_( SCBusLSWrBt )				/* Load/store unit write beats to L2 memory system							*/\
			_visit_( SCBusLSWrBy )				/* Load/store unit write bytes								(11.3.1)		*/\
			_visit_( SCBusLSWrByPerWr )			/* Load/store unit bytes written to L2 per access cycle		(9.2.3)			*/\
			_visit_( SCBusTileWrBy )			/* Tile unit write bytes									(11.3.2)		*/\
			_visit_( CoreAllRegsWarp )			/* Warps using more than 32 registers										*/\
			_visit_( CoreFullWarp )				/* Full warps																*/\
			_visit_( CoreAllRegsWarpRate )		/* All registers warp rate									(6.2.3)			*/\
			_visit_( CoreFullWarpRate )			/* Full warp rate											(6.3.3)			*/\
			_visit_( TexMipInstr )\
			_visit_( TexCompressInstr )\
			_visit_( Tex3DInstr )\
			_visit_( TexTriInstr )\
			_visit_( TexCoordStallCy )\
			_visit_( TexDataStallCy )\
			_visit_( TexPartDataStallCy )\
			_visit_( SCBusOtherWrBt )\
			_visit_( TexCompressInstrRate )\
			_visit_( Tex3DInstrRate )\
			_visit_( SCBusOtherWrBy )\
			_visit_( FragRastPartQd )			/* Partial rasterized fine quads											*/\
			_visit_( EngFMAInstr )				/* Arithmetic FMA instructions												*/\
			_visit_( EngCVTInstr )				/* Arithmetic CVT instructions												*/\
			_visit_( EngSFUInstr )				/* Arithmetic SFU instructions												*/\
			_visit_( EngICacheMiss )			/* Instruction cache misses													*/\
			_visit_( EngSWBlendInstr )\
			_visit_( TexInBt )\
			_visit_( TexDescStallCy )			/* Texture descriptor stall cycles											*/\
			_visit_( TexDataFetchStallCy )\
			_visit_( TexFiltStallCy )\
			_visit_( TexFullBiFiltCy )			/* Texture filtering cycles using full bilinear		(V: 8.1.2)				*/\
			_visit_( TexFullTriFiltCy )			/* Texture filtering cycles using full trilinear	(V: 8.1.3)				*/\
			_visit_( TexOutMsg )\
			_visit_( TexOutBt )\
			_visit_( FragRastPartQdRate )		/* Partial coverage rate							(6.3.1)					*/\
			_visit_( EngFMAPipeUtil )			/* FMA pipe utilization														*/\
			_visit_( EngCVTPipeUtil )			/* CVT pipe utilization														*/\
			_visit_( EngSFUPipeUtil )			/* SFU pipe utilization														*/\
			_visit_( EngArithInstr )			/* Arithmetic instruction issue cycles										*/\
			_visit_( EngSWBlendRate )			/* Shader blend percentage							(6.2.4)					*/\
			_visit_( TexInBusUtil )				/* Texture input bus utilization											*/\
			_visit_( TexOutBusUtil )			/* Texture output bus utilization											*/\
			_visit_( TexFiltFullRate )			/* Texture full speed filtering percentage									*/\
			_visit_( AnyActiveCy )\
			_visit_( AnyUtil )\
			_visit_( CSFMCUActiveCy )			/* MCU active cycles								(3.1.5)					*/\
			_visit_( GPUQueueActiveCy )\
			_visit_( GPUIRQ )\
			_visit_( L2CacheFlushCy )\
			_visit_( VertQueuedCy )\
			_visit_( VertQueueJob )\
			_visit_( VertQueueTask )\
			_visit_( VertQueueTotalActiveCy )\
			_visit_( VertQueueIRQActiveCy )\
			_visit_( VertQueueAssignStallCy )\
			_visit_( TilerQueueDrainStallCy )\
			_visit_( CompQueuedCy )				/* Compute queue active cycles						(3.1.4)					*/\
			_visit_( CompQueueJob )\
			_visit_( CompQueueTask )\
			_visit_( CompQueueTotalActiveCy )\
			_visit_( CompQueueIRQActiveCy )\
			_visit_( CompQueueAssignStallCy )\
			_visit_( CompQueueDrainStallCy )\
			_visit_( FragQueuedCy )\
			_visit_( FragQueueTotalActiveCy )\
			_visit_( FragQueueIRQActiveCy )\
			_visit_( FragQueueAssignStallCy )\
			_visit_( CSFCEUActiveCy )\
			_visit_( CSFLSUActiveCy )\
			_visit_( CSFCS0ActiveCy )\
			_visit_( CS0WaitStallCy )\
			_visit_( CSFCS1ActiveCy )\
			_visit_( CS1WaitStallCy )\
			_visit_( CSFCS2ActiveCy )\
			_visit_( CS2WaitStallCy )\
			_visit_( CSFCS3ActiveCy )\
			_visit_( CS3WaitStallCy )\
			_visit_( L2CacheEvict )\
			_visit_( L2CacheCleanUnique )\
			_visit_( VertQueueActiveCy )\
			_visit_( VertQueueUtil )\
			_visit_( CompQueueActiveCy )\
			_visit_( CompQueueUtil )			/* Compute queue utilization						(3.2.3)			*/\
			_visit_( CSFMCUUtil )				/* Microcontroller utilization						(3.2.4)			*/\
			_visit_( CSFLSUUtil )\
			_visit_( CSFCEUUtil )\
			_visit_( GPUActive2Cy )\
			_visit_( EngNarrowInstr )			/* Narrow arithmetic instructions (8/16 bit)						*/\
			_visit_( FragRastCoarseQd )\
			_visit_( RTUTri )\
			_visit_( RTUBox )\
			_visit_( RTUTriBin1 )				/* Ray tracing triangle batches with 1-4 rays		(10.4.4)		*/\
			_visit_( RTUTriBin5 )				/* Ray tracing triangle batches with 5-8 rays		(10.4.3)		*/\
			_visit_( RTUTriBin9 )				/* Ray tracing triangle batches with 9-12 rays		(10.4.2)		*/\
			_visit_( RTUTriBin13 )				/* Ray tracing triangle batches with 13-16 rays		(10.4.1)		*/\
			_visit_( RTUBoxBin1 )				/* Ray tracing box nodes with 1-4 rays				(10.3.4)		*/\
			_visit_( RTUBoxBin5 )				/* Ray tracing box nodes with 5-8 rays				(10.3.3)		*/\
			_visit_( RTUBoxBin9 )				/* Ray tracing box nodes with 9-12 rays				(10.3.2)		*/\
			_visit_( RTUBoxBin13 )				/* Ray tracing box nodes with 13-16 rays			(10.3.1)		*/\
			_visit_( RTUOpaqueHit )				/* Ray tracing opaque triangle hits					(10.2.2)		*/\
			_visit_( RTUNonOpaqueHit )			/* Ray tracing non-opaque triangle hits				(10.2.3)		*/\
			_visit_( RTUFirstHitTerm )			/* Ray tracing first hit terminations				(10.2.5)		*/\
			_visit_( RTUMiss )					/* Ray tracing ray misses							(10.2.4)		*/\
			_visit_( RTURay )					/* Ray tracing started rays							(10.2.1)		*/\
			_visit_( RTUBoxIssueCy )			/* Ray tracing box tester issue cycles				(10.1.1)		*/\
			_visit_( RTUTriIssueCy )			/* Ray tracing triangle tester issue cycles			(10.1.2)		*/\
			_visit_( GeomFaceCullPrim )\
			_visit_( GeomPlaneCullPrim )\
			_visit_( GeomFaceCullRate )\
			_visit_( GeomPlaneCullRate )\
			_visit_( FragShadRate )				/* Fragment shading rate							(4.6.1)	(6.3.4)	*/\
			_visit_( CoreFragWarpOcc )			/* Fragment warp occupancy %						(6.3.2)			*/\
			_visit_( EngNarrowInstrRate )		/* Narrow arithmetic percentage						(6.2.1)			*/\
			_visit_( RTUUtil )					/* Ray tracing unit utilization						(6.1.5)			*/\
			_visit_( BinningQueuedCy )			/* Binning phase queue active cycles				(3.1.2)			*/\
			_visit_( BinningQueueJob )\
			_visit_( BinningQueueTask )\
			_visit_( BinningQueueIRQActiveCy )\
			_visit_( BinningQueueAssignStallCy )\
			_visit_( MainQueuedCy )				/* Main phase queue active cycles					(3.1.3)			*/\
			_visit_( MainQueueJob )\
			_visit_( MainQueueTask )\
			_visit_( MainQueueIRQActiveCy )\
			_visit_( MainQueueAssignStallCy )\
			_visit_( MainActiveCy )\
			_visit_( CompOrBinningActiveCy )\
			_visit_( TexL1CacheLoadCy )\
			_visit_( TexCacheSimpleLoadCy )\
			_visit_( TexL1CacheOutputCy )\
			_visit_( TexL1CacheLookupCy )\
			_visit_( TexOutSingleMsg )\
			_visit_( TexCacheLookupCy )\
			_visit_( TexCacheComplexLoadCy )\
			_visit_( TexIndexCy )\
			_visit_( TexClkStarvedCy )\
			_visit_( TexClkActiveCy )\
			_visit_( GeomScissorCullPrim )\
			_visit_( GeomVisibleDVSPrim )\
			_visit_( MainQueueActiveCy )\
			_visit_( MainQueueUtil )\
			_visit_( BinningQueueActiveCy )\
			_visit_( BinningQueueUtil )\
			_visit_( GeomScissorCullRate )\
			_visit_( CompOrBinningUtil )\
			_visit_( MainUtil )\
			_visit_( CSDoorbellIRQCy )\
			_visit_( MainQueueTotalActiveCy )\
			_visit_( BinningQueueTotalActiveCy )\
			_visit_( CSFCS4ActiveCy )\
			_visit_( CS4WaitStallCy )\
			_visit_( CSFCS5ActiveCy )\
			_visit_( CS5WaitStallCy )\
			_visit_( FragPrim )\
			_visit_( FragPrepassCullPrim )\
			_visit_( FragPrepassPrim )\
			_visit_( FragPrepassUncullPrim )\
			_visit_( FragPrepassEZSUpdateQd )\
			_visit_( FragPrepassTestQd )\
			_visit_( FragPrepassKillQd )\
			_visit_( FragPrepassWarp )\
			_visit_( FragMainPassStallCy )\
			_visit_( DefVertWarp )\
			_visit_( EngTexBackpressureCy )\
			_visit_( EngVarBackpressureCy )\
			_visit_( EngBlendBackpressureCy )\
			_visit_( EngZSBackpressureCy )\
			_visit_( EngLSBackpressureCy )\
			_visit_( EngAttrBackpressureCy )\
			_visit_( EngSlot1IssueCy )\
			_visit_( EngSlotAnyIssueCy )\
			_visit_( GeomPosShadPartTask )\
			_visit_( GeomVarShadPartTask )\
			_visit_( FragPrepassKillRate )\
			_visit_( FragMainPassStallRate )\
			_visit_( FragInputPrim )\
			_visit_( FragPrepassPrimRate )\
			_visit_( FragPrepassCullPrimRate )\
			_visit_( FragPrepassUncullPrimRate )\
			_visit_( FragPrepassWarpRate )\
			_visit_( FragPrepassThread )\
			_visit_( FragMainThread )\
			_visit_( EngSlot0IssueCy )\
			_visit_( EngAttrBackpressureRate )\
			_visit_( EngBlendBackpressureRate )\
			_visit_( EngLSBackpressureRate )\
			_visit_( EngTexBackpressureRate )\
			_visit_( EngVarBackpressureRate )\
			_visit_( EngZSBackpressureRate )

			#define DECL_COUNTER( _name_ )		_name_,
			AE_MALI_COUNTERS( DECL_COUNTER )
			#undef DECL_COUNTER

			_LastCounter,

			// custom //
			ExtBusTotalBy = _LastCounter,		// ExtBusRdBy + ExtBusWrBy
			ExtMemEnergy,						// (ExtBusRdBy + ExtBusWrBy) * power consumption, in joules
			CoreEnergy,							// CoreActiveCy * power consumption, in joules
			TotalEnergy,						// ExtMemEnergy + CoreEnergy, in joules
			PerCoreActiveCy,					// CoreActiveCy

			_Count
		};

		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, double >;

		struct HWInfo
		{
			ubyte		shaderCoreCount		= 0;
			ubyte		execEngineCount		= 0;	// per core
			ushort		busWidth			= 0;	// bits
			ushort		l2Slices			= 0;
			Bytes32u	l2SliceSize;
			ushort		tileSize			= 0;	// pixels
			ushort		warpSize			= 0;
		};

	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		MaliProfiler ()												__NE___;
		~MaliProfiler ()											__NE___;

		ND_ bool  Initialize (ECounterSet counterSet)				__NE___;
			void  Deinitialize ()									__NE___;
		ND_ bool  IsInitialized ()									C_NE___;

		ND_ ECounterSet	EnabledCounterSet ()						C_NE___;
		ND_ HWInfo		GetHWInfo ()								C_NE___;

			void  Sample (OUT Counters_t &, INOUT float &invdt)		C_NE___;


	  #ifndef AE_ENABLE_MALI_HWCPIPE
		ND_ bool  InitClient (RC<MaliProfilerClient>)				__NE___;
	  #endif
	};


} // AE::Profiler
