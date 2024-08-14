// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for Mali GPU hardware performance counters.
	(x.x.x) - link to counter description in docs.
	By default it it link to 5th Gen performance counters guide, special key like V (Valhall), B (Bifrost) used for links to other architectures.

	[Performance counters description](https://github.com/azhirnov/cpu-gru-arch/blob/main/gpu/ARM-Mali_PC.md)
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
			GPUActiveCy,				// 0	//													(3.1.1)
			GPUIRQActiveCy,						// GPU interrupt pending cycles						(3.1.6)
			FragQueueJob,
			FragQueueTask,
			FragQueueActiveCy,					// Fragment queue active cycles, Main phase queue active cycles			(3.1.3)
			NonFragQueueJob,
			NonFragQueueTask,
			NonFragQueueActiveCy,				// Non-fragment queue active cycles, Binning phase queue active cycles	(3.1.2)
			ResQueueJob,
			ResQueueTask,
			ResQueueActiveCy,			// 10
			ExtBusWrBt,							// Output external write beats
			ExtBusRdBt,							// Output external read beats
			ExtBusRdStallCy,					// Output external read stall cycles
			ExtBusWrStallCy,					// Output external write stall cycles
			FragActiveCy,						// Fragment active cycles
			FragRdPrim,							// Fragment primitives loaded
			FragThread,							// Fragment threads
			FragHelpThread,
			FragRastQd,							// Rasterized fine quads
			FragEZSTestQd,				// 20	// Early ZS tested quads
			FragEZSKillQd,						// Early ZS killed quads
			FragLZSTestTd,
			FragLZSKillTd,
			FragTile,							// Tile count ???
			FragTileKill,						// Killed unchanged tiles
			NonFragActiveCy,					// Non-fragment active cycles
			NonFragThread,						// Non-fragment threads
			CoreActiveCy,						// Execution core active cycles
			EngInstr,
			LSIssueCy,					// 30	// Load/store unit issue cycles						(9.1.1)
			TexInstr,
			TexFiltIssueCy,						// Texture filtering cycles							(8.1.1)
			LSRdHitCy,
			LSWrHitCy,
			GeomTrianglePrim,					// Triangle primitives
			GeomPointPrim,						// Point primitives
			GeomLinePrim,						// Line primitives
			GeomFrontFacePrim,					// Visible front-facing primitives
			GeomBackFacePrim,					// Visible back-facing primitives
			GeomVisiblePrim,			// 40	// Visible primitives								(4.1.3)
			GeomFaceXYPlaneCullPrim,			// Facing or XY plane test culled primitives
			GeomZPlaneCullPrim,					// Z plane culled primitives
			TilerActiveCy,						// Tiler active cycles								(3.1.4)
			GPUIRQUtil,							// Interrupt pending utilization					(3.2.5)
			FragQueueUtil,						// Fragment queue utilization, Main phase queue utilization			(3.2.2)
			NonFragQueueUtil,					// Non-fragment queue utilization, Binning phase queue utilization	(3.2.1)
			ExtBusRdBy,							// Output external read bytes						(3.3.1)
			ExtBusWrBy,							// Output external write bytes						(3.3.2)
			ExtBusRdStallRate,					// Output external read stall rate					(3.4.1)
			ExtBusWrStallRate,			// 50	// Output external write stall rate					(3.4.2)
			TilerUtil,							// Tiler utilization								(3.2.3)
			GeomTotalPrim,						// Total input primitives							(4.1.1)
			GeomVisibleRate,					// Visible primitives rate							(4.2.1)
			GeomTotalCullPrim,					// Total culled primitives							(4.1.2)
			GeomFaceXYPlaneCullRate,			// Facing or XY plane test cull						(4.2.2)
			GeomZPlaneCullRate,					// Z plane test cull rate							(4.2.3)
			NonFragUtil,						// Non-fragment utilization							(5.3.2)
			NonFragThroughputCy,				// Average cycles per non-fragment thread			(5.2.1)
			FragUtil,							// Fragment utilization								(5.3.3)
			FragThroughputCy,			// 60	// Average cycles per fragment thread				(5.2.2)
			FragHelpTdRate,
			FragEZSTestRate,					// Early ZS tested quad percentage					(4.5.1)
			FragEZSKillRate,					// Early ZS killed quad percentage					(4.5.3)
			FragLZSTestRate,					// Late ZS tested thread percentage					(4.5.5 ?)
			FragLZSKillRate,					// Late ZS killed thread percentage					(4.5.6 ?)
			FragOverdraw,						// Fragments per pixel								(4.4.3)
			FragTileKillRate,					// Unchanged tile kill rate							(6.3.5)
			CoreUtil,							// Execution core utilization						(5.3.5)
			ALUUtil,							// Arithmetic unit utilization						(6.1.1)
			TexSample,					// 70	// Texture samples
			TexCPI,								// Texture filtering cycles per instruction			(8.1.2)
			TexUtil,							// Texture unit utilization							(6.1.3)
			TexIssueCy,							// Texture unit issue cycles
			LSUtil,								// Load/store unit utilization						(6.1.4)
			GPUPix,								// total number of pixels that are shaded, assumes that all pixels in task are shaded (32x32, 5Gen: 64x64) (4.4.1)
			GPUCyPerPix,						// Average cycles per pixel							(4.4.2)
			FragQueueWaitRdCy,					// Fragment queue job descriptor read wait cycles
			FragQueueWaitIssueCy,				// Fragment queue job issue wait cycles
			FragQueueWaitDepCy,					// Fragment queue job dependency wait cycles
			FragQueueWaitFinishCy,		// 80	// Fragment queue job finish wait cycles
			NonFragQueueWaitRdCy,				// Non-fragment queue job descriptor read wait cycles
			NonFragQueueWaitIssueCy,			// Non-fragment queue job issue wait cycles
			NonFragQueueWaitDepCy,				// Non-fragment queue job dependency wait cycles
			NonFragQueueWaitFinishCy,			// Non-fragment queue job finish wait cycles
			ResQueueWaitRdCy,					// Reserved queue job descriptor read wait cycles
			ResQueueWaitIssueCy,				// Reserved queue job issue wait cycles
			ResQueueWaitDepCy,					// Reserved queue job dependency wait cycles
			ResQueueWaitFinishCy,				// Reserved queue job finish wait cycles
			MMUL2Hit,							// MMU L2 lookup TLB hits
			MMUL2Rd,					// 90	// MMU L2 table read requests
			MMULookup,							// MMU lookup requests
			L2CacheLookup,
			L2CacheRdLookup,
			L2CacheWrLookup,
			FragFPKActiveCy,					// Forward pixel kill buffer active cycles
			LSRdCy,								// Load/store unit read issues
			LSWrCy,								// Load/store unit write issues
			LSAtomic,							// Load/store unit atomic issues					(9.1.6)
			TilerPosCacheHit,					// Position cache hit requests
			TilerPosCacheMiss,			// 100	// Position cache miss requests
			FragFPKBUtil,						// Fragment FPK buffer utilization					(5.3.4)
			FragQueueWaitFlushCy,				// Fragment queue cache flush wait cycles
			NonFragQueueWaitFlushCy,			// Non-fragment queue cache flush wait cycles
			ResQueueWaitFlushCy,				// Non-fragment queue cache flush wait cycles
			L2CacheFlush,						// L2 cache flush requests
			GeomSampleCullPrim,					// Sample test culled primitives
			TilerRdBt,							// Output internal read beats
			TilerWrBt,
			GeomPosShadTask,					// Tiler position shading requests
			TilerPosShadStallCy,		// 110	// Tiler position shading stall cycles
			TilerPosShadFIFOFullCy,				// Tiler position FIFO full cycles
			TilerVarCacheHit,					// Varying cache hits
			TilerVarCacheMiss,					// Varying cache misses
			GeomVarShadTask,					// Tiler varying shading requests
			TilerVarShadStallCy,				// Tiler varying shading stall cycles
			FragRastPrim,						// Rasterized primitives
			FragWarp,							// Fragment warps									(5.1.2)
			FragPartWarp,
			FragEZSUpdateQd,					// Early ZS updated quads
			FragLZSTestQd,				// 120	// Late ZS tested quads
			FragLZSKillQd,						// Late ZS killed quads
			FragOpaqueQd,						// Occluding quads
			NonFragTask,
			NonFragWarp,						// Non-fragment warps								(5.1.1)
			EngActiveCy,
			EngDivergedInstr,
			EngStarveCy,
			TexQuads,							// Texture quads
			TexQuadPass,
			TexQuadPassDescMiss,		// 130
			TexQuadPassMip,
			TexQuadPassTri,
			TexCacheFetch,
			TexCacheCompressFetch,
			TexCacheLookup,
			LSFullRd,							// Load/store unit full read issues					(9.1.2)
			LSPartRd,							// Load/store unit partial read issues				(9.1.3)
			LSFullWr,							// Load/store unit full write issues				(9.1.4)
			LSPartWr,							// Load/store unit partial write issues				(9.1.5)
			VarInstr,					// 140	// Varying unit instructions
			Var32IssueSlot,						// 32-bit interpolation slots
			Var16IssueSlot,						// 16-bit interpolation slots
			AttrInstr,							// Attribute instructions
			SCBusFFEL2RdBt,						// Fragment front-end read beats from L2 cache
			SCBusFFEExtRdBt,					// Fragment front-end read beats from external memory
			SCBusLSL2RdBt,						// Load/store unit read beats from L2 cache
			SCBusLSExtRdBt,						// Load/store unit read beats from external memory
			SCBusTexL2RdBt,						// Texture unit read beats from L2 cache
			SCBusTexExtRdBt,					// Texture unit read beats from external memory
			SCBusOtherL2RdBt,			// 150	// Miscellaneous read beats from L2 cache
			SCBusLSWBWrBt,						// Load/store unit write-back write beats
			SCBusTileWrBt,						// Tile unit write beats to L2 memory system
			SCBusLSOtherWrBt,					// Load/store unit other write beats
			MMUL3Rd,							// MMU L3 table read requests
			MMUL3Hit,							// MMU L3 lookup TLB hits
			MMUS2Lookup,						// MMU stage 2 lookup requests
			MMUS2L3Rd,							// MMU stage 2 L3 lookup requests
			MMUS2L2Rd,							// MMU stage 2 L2 lookup requests
			MMUS2L3Hit,							// MMU stage 2 L3 lookup TLB hits
			MMUS2L2Hit,					// 160	// MMU stage 2 L2 lookup TLB hits
			L2CacheRd,							// Input internal read requests
			L2CacheRdStallCy,					// Input internal read stall cycles
			L2CacheWr,							// Input internal write requests
			L2CacheWrStallCy,					// Input internal write stall cycles
			L2CacheSnp,							// Input internal snoop requests
			L2CacheSnpStallCy,					// Input internal snoop stall cycles
			L2CacheL1Rd,						// Output internal read requests
			L2CacheL1RdStallCy,					// Output internal read stall cycles
			L2CacheL1Wr,						// Output internal write requests
			L2CacheSnpLookup,			// 170	// Input external snoop lookup requests
			ExtBusRd,							// Output external read transactions
			ExtBusRdNoSnoop,					// Output external ReadNoSnoop transactions
			ExtBusRdUnique,						// Output external ReadUnique transactions
			ExtBusRdOTQ1,						// Output external outstanding reads 0-25%
			ExtBusRdOTQ2,						// Output external outstanding reads 25-50%
			ExtBusRdOTQ3,						// Output external outstanding reads 50-75%
			ExtBusRdLat0,						// Output external read latency 0-127 cycles				(3.5.1)
			ExtBusRdLat128,						// Output external read latency 128-191 cycles				(3.5.2)
			ExtBusRdLat192,						// Output external read latency 192-255 cycles				(3.5.3)
			ExtBusRdLat256,				// 180	// Output external read latency 256-319 cycles				(3.5.4)
			ExtBusRdLat320,						// Output external read latency 320-383 cycles				(3.5.5)
			ExtBusWr,							// Output external write transactions
			ExtBusWrNoSnoopFull,				// Output external WriteNoSnoopFull transactions
			ExtBusWrNoSnoopPart,				// Output external WriteNoSnoopPartial transactions
			ExtBusWrSnoopFull,					// Output external WriteSnoopFull transactions
			ExtBusWrSnoopPart,					// Output external WriteSnoopPartial transactions
			ExtBusWrOTQ1,						// Output external outstanding writes 0-25%
			ExtBusWrOTQ2,						// Output external outstanding writes 25-50%
			ExtBusWrOTQ3,						// Output external outstanding writes 50-75%
			L2CacheIncSnp,				// 190	// Input external snoop transactions
			L2CacheIncSnpStallCy,				// Input external snoop stall cycles
			L2CacheRdMissRate,					// L2 cache read miss rate
			L2CacheWrMissRate,					// L2 cache write miss rate
			ExtBusRdLat384,						// Output external read latency 384+ cycles					(3.5.6)
			ExtBusRdOTQ4,						// Output external outstanding reads 75-100%
			ExtBusWrOTQ4,						// Output external outstanding writes 75-100%
			GeomSampleCullRate,					// Sample test cull rate									(4.2.4)
			GeomPosShadThread,					// Position shader thread invocations						(4.3.1)
			GeomPosShadThreadPerPrim,			// Position threads per input primitive, should be < 1.5	(4.3.3)
			TilerPosCacheHitRate,		// 200	// Position cache hit rate
			GeomVarShadThread,					// Varying shader thread invocations						(4.3.2)
			GeomVarShadThreadPerPrim,			// Varying threads per input primitive						(4.3.4)
			TilerVarCacheHitRate,
			FragOpaqueQdRate,					// Occluding quad percentage
			FragTransparentQd,					// Non-occluding quads
			FragShadedQd,						// Shaded coarse quads
			FragPartWarpRate,
			FragEZSUpdateRate,					// Early ZS updated quad percentage							(4.5.2)
			FragFPKKillQd,						// Forward pixel kill killed quads
			FragFPKKillRate,			// 210	// Forward pixel kill killed quad percentage				(4.5.4)
			EngDivergedInstrRate,				// Warp divergence percentage								(6.2.2)
			TexCacheUtil,
			TexMipInstrRate,
			TexCacheCompressFetchRate,
			TexTriInstrRate,
			Var32IssueCy,						// 32-bit interpolation active cycles						(7.1.3)
			Var16IssueCy,						// 16-bit interpolation active cycles						(7.1.2)
			VarIssueCy,							// Varying unit issue cycles								(7.1.1)
			VarUtil,							// Varying unit utilization									(6.1.2)
			SCBusFFEL2RdBy,				// 220	// Front-end unit read bytes from L2 cache					(11.1.1)
			SCBusFFEExtRdBy,					// Front-end unit read bytes from external memory			(11.2.1)
			SCBusLSL2RdBy,						// Load/store unit read bytes from L2 cache					(11.1.2)
			SCBusLSL2RdByPerRd,					// Load/store unit bytes read from L2 per access cycle		(9.2.1)
			SCBusLSExtRdBy,						// Load/store unit read bytes from external memory			(11.2.2)
			SCBusLSExtRdByPerRd,				// Load/store unit bytes read from external memory per access cycle	(9.2.2)
			SCBusTexL2RdBy,						// Texture unit read bytes from L2 cache					(11.1.3)
			SCBusTexL2RdByPerRd,				// Texture unit bytes read from L2 per texture cycle		(8.2.1)
			SCBusTexExtRdBy,					// Texture unit read bytes from external memory				(11.2.3)
			SCBusTexExtRdByPerRd,				// Texture unit bytes read from external memory per texture cycle	(8.2.2)
			SCBusLSWrBt,				// 230	// Load/store unit write beats to L2 memory system
			SCBusLSWrBy,						// Load/store unit write bytes								(11.3.1)
			SCBusLSWrByPerWr,					// Load/store unit bytes written to L2 per access cycle		(9.2.3)
			SCBusTileWrBy,						// Tile unit write bytes									(11.3.2)
			CoreAllRegsWarp,					// Warps using more than 32 registers
			CoreFullQdWarp,						// Full quad warps
			CoreAllRegsWarpRate,				// All registers warp rate									(6.2.3)
			CoreFullQdWarpRate,					// Full quad warp rate										(6.3.3)
			TexMipInstr,
			TexCompressInstr,
			Tex3DInstr,					// 240
			TexTriInstr,
			TexCoordStallCy,
			TexDataStallCy,
			TexPartDataStallCy,
			SCBusOtherWrBt,
			TexCompressInstrRate,
			Tex3DInstrRate,
			SCBusOtherWrBy,
			FragRastPartQd,						// Partial rasterized fine quads
			EngFMAInstr,				// 250	// Arithmetic FMA instructions
			EngCVTInstr,						// Arithmetic CVT instructions
			EngSFUInstr,						// Arithmetic SFU instructions
			EngICacheMiss,						// Instruction cache misses
			EngSWBlendInstr,
			TexInBt,
			TexDescStallCy,						// Texture descriptor stall cycles
			TexDataFetchStallCy,
			TexFiltStallCy,
			TexFullBiFiltCy,					// Texture filtering cycles using full bilinear		(V: 8.1.2)
			TexFullTriFiltCy,			// 260	// Texture filtering cycles using full trilinear	(V: 8.1.3)
			TexOutMsg,
			TexOutBt,
			FragRastPartQdRate,					// Partial coverage rate							(6.3.1)
			EngFMAPipeUtil,						// FMA pipe utilization
			EngCVTPipeUtil,						// CVT pipe utilization
			EngSFUPipeUtil,						// SFU pipe utilization
			EngArithInstr,						// Arithmetic instruction issue cycles
			EngSWBlendRate,						// Shader blend percentage							(6.2.4)
			TexInBusUtil,						// Texture input bus utilization
			TexOutBusUtil,				// 270	// Texture output bus utilization
			TexFiltFullRate,					// Texture full speed filtering percentage
			AnyActiveCy,
			AnyUtil,
			CSFMCUActiveCy,						// MCU active cycles								(3.1.5)
			GPUIterActiveCy,
			GPUIRQ,
			L2CacheFlushCy,
			VertIterQueuedCy,
			VertIterJob,
			VertIterTask,				// 280
			VertIterTotalActiveCy,
			VertIterIRQActiveCy,
			VertIterAssignStallCy,
			TilerIterDrainStallCy,
			CompIterQueuedCy,					// Compute queue active cycles						(3.1.4)
			CompIterJob,
			CompIterTask,
			CompIterTotalActiveCy,
			CompIterIRQActiveCy,
			CompIterAssignStallCy,		// 290
			CompIterDrainStallCy,
			FragIterQueuedCy,
			FragIterJob,
			FragIterTask,
			FragIterTotalActiveCy,
			FragIterIRQActiveCy,
			FragIterAssignStallCy,
			CSFCEUActiveCy,
			CSFLSUActiveCy,
			CSFCS0ActiveCy,				// 300
			CS0WaitStallCy,
			CSFCS1ActiveCy,
			CS1WaitStallCy,
			CSFCS2ActiveCy,
			CS2WaitStallCy,
			CSFCS3ActiveCy,
			CS3WaitStallCy,
			L2CacheEvict,
			L2CacheCleanUnique,
			FragIterActiveCy,			// 310
			FragIterUtil,
			VertIterActiveCy,
			VertIterUtil,
			CompIterActiveCy,
			CompIterUtil,						// Compute queue utilization ???					(3.2.3)
			CSFMCUUtil,							// Microcontroller utilization						(3.2.4)
			CSFLSUUtil,
			CSFCEUUtil,
			EngNarrowInstr,						// Narrow arithmetic instructions (8/16 bit)
			FragRastCoarseQd,			// 320
			RTUTri,
			RTUBox,
			RTUTriBin1,							// Ray tracing triangle batches with 1-4 rays		(10.4.4)
			RTUTriBin5,							// Ray tracing triangle batches with 5-8 rays		(10.4.3)
			RTUTriBin9,							// Ray tracing triangle batches with 9-12 rays		(10.4.2)
			RTUTriBin13,						// Ray tracing triangle batches with 13-16 rays		(10.4.1)
			RTUBoxBin1,							// Ray tracing box nodes with 1-4 rays				(10.3.4)
			RTUBoxBin5,							// Ray tracing box nodes with 5-8 rays				(10.3.3)
			RTUBoxBin9,							// Ray tracing box nodes with 9-12 rays				(10.3.2)
			RTUBoxBin13,				// 330	// Ray tracing box nodes with 13-16 rays			(10.3.1)
			RTUOpaqueHit,						// Ray tracing opaque triangle hits					(10.2.2)
			RTUNonOpaqueHit,					// Ray tracing non-opaque triangle hits				(10.2.3)
			RTUFirstHitTerm,					// Ray tracing first hit terminations				(10.2.5)
			RTUMiss,							// Ray tracing ray misses							(10.2.4)
			RTURay,								// Ray tracing started rays							(10.2.1)
			RTUBoxIssueCy,						// Ray tracing box tester issue cycles				(10.1.1)
			RTUTriIssueCy,						// Ray tracing triangle tester issue cycles			(10.1.2)
			GeomFaceCullPrim,
			GeomPlaneCullPrim,
			GeomFaceCullRate,			// 340
			GeomPlaneCullRate,
			FragShadRate,						// Fragment shading rate							(4.6.1)	(6.3.4)
			CoreFragWarpOcc,					// Fragment warp occupancy %						(6.3.2)
			EngNarrowInstrRate,					// Narrow arithmetic percentage						(6.2.1)
			RTUUtil,							// Ray tracing unit utilization						(6.1.5)
			BinningIterQueuedCy,				// Binning phase queue active cycles				(3.1.2)
			BinningIterJob,
			BinningIterTask,
			BinningIterIRQActiveCy,
			BinningIterAssignStallCy,	// 350
			MainIterQueuedCy,					// Main phase queue active cycles					(3.1.3)
			MainIterJob,
			MainIterTask,
			MainIterIRQActiveCy,
			MainIterAssignStallCy,
			TexL1CacheLoadCy,
			TexCacheSimpleLoadCy,
			TexL1CacheOutputCy,
			TexL1CacheLookupCy,
			TexOutSingleMsg,			// 360
			TexCacheLookupCy,
			TexCacheComplexLoadCy,
			TexIndexCy,
			TexClkStarvedCy,
			TexClkActiveCy,
			GeomScissorCullPrim,
			GeomVisibleDVSPrim,
			MainIterActiveCy,
			MainIterUtil,
			BinningIterActiveCy,		// 370
			BinningIterUtil,
			GeomScissorCullRate,
			_Count
		};
		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, double >;

		struct HWInfo
		{
			ubyte		shaderCoreCount		= 0;
			ubyte		execEngineCount		= 0;
			ushort		busWidth			= 0;	// bits
			ushort		l2Slices			= 0;
			Bytes32u	l2SliceSize;
			ushort		tileSize			= 0;	// pixels?
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

		ND_ bool  Initialize (const ECounterSet &counterSet)		__NE___;
			void  Deinitialize ()									__NE___;
		ND_ bool  IsInitialized ()									C_NE___;

		ND_ ECounterSet	EnabledCounterSet ()						C_NE___;
		ND_ HWInfo		GetHWInfo ()								C_NE___;

			void  Sample (OUT Counters_t &)							C_NE___;


	  #ifndef AE_ENABLE_MALI_HWCPIPE
		ND_ bool  InitClient (RC<MaliProfilerClient>)				__NE___;
	  #endif
	};


} // AE::Profiler
