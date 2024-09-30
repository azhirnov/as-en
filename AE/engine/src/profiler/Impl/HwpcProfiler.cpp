// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "base/Algorithms/StringUtils.h"
#include "profiler/Impl/HwpcProfiler.h"
#include "profiler/Remote/RemoteArmProfiler.h"
#include "profiler/Remote/RemoteMaliProfiler.h"
#include "profiler/Remote/RemoteNVidiaProfiler.h"
#include "profiler/Remote/RemoteAdrenoProfiler.h"
#include "profiler/Remote/RemotePowerVRProfiler.h"
#include "profiler/Remote/RemoteGeneralProfiler.h"
#include "graphics/GraphicsImpl.h"

namespace AE::Profiler
{

/*
=================================================
	constructor
=================================================
*/
	HwpcProfiler::HwpcProfiler (TimePoint_t startTime) :
		ProfilerUtils{ startTime }
	{}

/*
=================================================
	destructor
=================================================
*/
	HwpcProfiler::~HwpcProfiler ()
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  HwpcProfiler::Initialize (ClientServer_t client, MsgProducer_t msgProducer)
	{
		CHECK_ERR( (client != null) == (msgProducer != null) );

		_isRemote = bool{client};

		_initialized |= _InitNvProf( client, msgProducer );
		_initialized |= _InitArmProf( client, msgProducer );
		_initialized |= _InitMaliProf( client, msgProducer );
		_initialized |= _InitAdrenoProf( client, msgProducer );
		_initialized |= _InitPowerVRProf( client, msgProducer );
		_initialized |= _InitGeneralProf( client, msgProducer );

		#ifdef AE_ENABLE_IMGUI
		_InitImGui();
		#endif

		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	void  HwpcProfiler::Draw (Canvas &)
	{
		if ( not _initialized )
			return;

		// TODO
	}

/*
=================================================
	Update
=================================================
*/
	void  HwpcProfiler::Update (secondsf dt, uint frameCount)
	{
		if ( not _initialized )
			return;

		const float	inv_dt		= 1.f / dt.count();
		const float	inv_frames	= 1.f / float(frameCount);
		const bool	per_frame	= not _isRemote;

		SampleGraphicsCounters( inv_dt );
		SampleCPUCounters( inv_dt );

	  #ifdef AE_ENABLE_IMGUI
		_UpdateGeneralPerfImGui( per_frame, inv_frames );
		_UpdateArmCountersImGui( per_frame, inv_frames );
		_UpdateMaliCountersImGui( per_frame, inv_frames );
		_UpdateAdrenoCountersImGui( per_frame, inv_frames );
		_UpdatePowerVRCountersImGui( per_frame, inv_frames );
		_UpdateNVidiaCountersImGui( per_frame, inv_frames );
	  #else
		Unused( dt, frameCount );
	  #endif
	}

/*
=================================================
	Tick
=================================================
*/
	void  HwpcProfiler::Tick ()
	{
		_pvrProf.profiler.Tick();
	}

/*
=================================================
	_InitNvProf
=================================================
*/
	bool  HwpcProfiler::_InitNvProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
	  #if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_NVML)
		Unused( client, msgProducer );
	  #else
		// initialize remote profiling
		if ( client )
		{
			CHECK( msgProducer->GetChannels() == EnumSet<EChannel>{EChannel::Reliable} );

			auto	nv_prof_client = MakeRC<NVidiaProfilerClient>( RVRef(msgProducer) );

			CHECK( client->Add( nv_prof_client->GetMsgConsumer() ));

			CHECK( _nvProf.profiler.InitClient( nv_prof_client ));
		}
	  #endif

		using ECounter = NVidiaProfiler::ECounter;

		_nvProf.requiredCounters = NVidiaProfiler::ECounterSet{}.SetAll();

		return _nvProf.profiler.Initialize( _nvProf.requiredCounters );
	}

/*
=================================================
	_InitArmProf
=================================================
*/
	bool  HwpcProfiler::_InitArmProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
	  #if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_ARM_PMU)
		Unused( client, msgProducer );
	  #else
		// initialize remote profiling
		if ( client )
		{
			CHECK( msgProducer->GetChannels() == EnumSet<EChannel>{EChannel::Reliable} );

			auto	arm_prof_client = MakeRC<ArmProfilerClient>( RVRef(msgProducer) );

			CHECK( client->Add( arm_prof_client->GetMsgConsumer() ));

			CHECK( _armProf.profiler.InitClient( arm_prof_client ));
		}
	  #endif

		using ECounter = ArmProfiler::ECounter;

		_armProf.requiredCounters = ArmProfiler::ECounterSet{}.SetAll();

		return _armProf.profiler.Initialize( _armProf.requiredCounters );
	}

/*
=================================================
	_InitMaliProf
=================================================
*/
	bool  HwpcProfiler::_InitMaliProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
	  #if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_ARM_PMU)
		Unused( client, msgProducer );
	  #else
		// initialize remote profiling
		if ( client )
		{
			CHECK( msgProducer->GetChannels() == EnumSet<EChannel>{EChannel::Reliable} );

			auto	mali_prof_client = MakeRC<MaliProfilerClient>( RVRef(msgProducer) );

			CHECK( client->Add( mali_prof_client->GetMsgConsumer() ));

			CHECK( _maliProf.profiler.InitClient( mali_prof_client ));
		}
	  #endif

		using ECounter = MaliProfiler::ECounter;

		_maliProf.requiredCounters = MaliProfiler::ECounterSet{
				// Clock //
					#if 1
						ECounter::GPUActiveCy,			ECounter::PerCoreActiveCy,		ECounter::TilerActiveCy,
						ECounter::FragThroughputCy,		ECounter::NonFragThroughputCy,
						ECounter::ExtMemEnergy,			ECounter::CoreEnergy,			ECounter::TotalEnergy,
					#endif
				// External memory //
					#if 1
						ECounter::ExtBusRdBy,			ECounter::ExtBusWrBy,			ECounter::ExtBusTotalBy,
						ECounter::ExtBusRdStallRate,	ECounter::ExtBusWrStallRate,
						ECounter::ExtBusRdLat0,			ECounter::ExtBusRdLat128,		ECounter::ExtBusRdLat192,	ECounter::ExtBusRdLat256,	ECounter::ExtBusRdLat320,	ECounter::ExtBusRdLat384,
						ECounter::ExtBusRdOTQ1,			ECounter::ExtBusRdOTQ2,			ECounter::ExtBusRdOTQ3,		ECounter::ExtBusRdOTQ4,
						ECounter::ExtBusWrOTQ1,			ECounter::ExtBusWrOTQ2,			ECounter::ExtBusWrOTQ3,		ECounter::ExtBusWrOTQ4,
						ECounter::SCBusFFEExtRdBy,		ECounter::SCBusLSExtRdBy,		ECounter::SCBusTexExtRdBy,
					#endif
				// Cache //
					#if 1
						ECounter::L2CacheRdMissRate,	ECounter::L2CacheWrMissRate,
						ECounter::SCBusTileWrBy,		ECounter::SCBusLSWrBy,
						ECounter::SCBusFFEL2RdBy,		ECounter::SCBusLSL2RdBy,		ECounter::SCBusTexL2RdBy,
						ECounter::L2CacheFlushCy,
						ECounter::TilerPosCacheHitRate,	ECounter::TilerVarCacheHitRate,
					#endif
				// Binning phase //
					#if 1
						ECounter::GeomTotalCullPrim,	ECounter::GeomVisiblePrim,		ECounter::GeomTotalPrim,
					//	ECounter::GeomPosShadTask,		ECounter::GeomVarShadTask,
						ECounter::GeomPosShadThread,	ECounter::GeomVarShadThread,
						ECounter::GeomVarShadThreadPerPrim,	ECounter::GeomPosShadThreadPerPrim,
					#endif
				// Rasterization //
					#if 1
						ECounter::FragEZSTestQd,		ECounter::FragEZSKillQd,		ECounter::FragLZSTestQd,	ECounter::FragLZSKillQd,
						ECounter::FragEZSKillRate,		ECounter::FragFPKKillRate,		ECounter::FragLZSKillRate,
						ECounter::FragOpaqueQdRate,		ECounter::FragOverdraw,
						ECounter::GeomTrianglePrim,		ECounter::GeomPointPrim,		ECounter::GeomLinePrim,
						ECounter::FragTile,				ECounter::FragTileKill,
						ECounter::FragTileKillRate,
					//	ECounter::FragRastQd,			ECounter::FragOpaqueQd,			ECounter::FragTransparentQd,	ECounter::FragShadedQd,
					#endif
				// Functional unit utilization //
					#if 1
						ECounter::CoreUtil,
						ECounter::VarUtil,
						ECounter::TexUtil,
						ECounter::TexCacheUtil,
						ECounter::LSUtil,
						ECounter::RTUUtil,
						ECounter::NonFragUtil,			ECounter::FragUtil,
						ECounter::TilerUtil,
						ECounter::FragFPKBUtil,
						ECounter::GPUIRQUtil,			ECounter::NonFragQueueUtil,		ECounter::FragQueueUtil,
					#endif
				// Shader core //
					#if 1
						ECounter::ALUUtil,
						ECounter::EngNarrowInstrRate,
						ECounter::EngFMAPipeUtil,		ECounter::EngCVTPipeUtil,		ECounter::EngSFUPipeUtil,
						ECounter::EngDivergedInstrRate,
						ECounter::FragWarp,				ECounter::NonFragWarp,			ECounter::CoreFullWarp,		ECounter::CoreAllRegsWarp,
						ECounter::CoreAllRegsWarpRate,
						ECounter::CoreFullWarpRate,
						ECounter::FragRastPartQdRate,
					#endif
				// Usage //
					#if 0
						ECounter::ExtBusRdStallCy,		ECounter::ExtBusWrStallCy,
						ECounter::GPUIRQActiveCy,
						ECounter::FragQueueActiveCy,	ECounter::NonFragQueueActiveCy,
						ECounter::FragActiveCy,			ECounter::NonFragActiveCy,		ECounter::TilerActiveCy,
						ECounter::CoreActiveCy,			ECounter::LSIssueCy,
					#endif
				};

		return _maliProf.profiler.Initialize( _maliProf.requiredCounters );
	}

/*
=================================================
	_InitPowerVRProf
=================================================
*/
	bool  HwpcProfiler::_InitPowerVRProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
	  #if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_PVRCOUNTER)
		Unused( client, msgProducer );
	  #else
		// initialize remote profiling
		if ( client )
		{
			CHECK( msgProducer->GetChannels() == EnumSet<EChannel>{EChannel::Reliable} );

			auto	pvr_prof_client = MakeRC<PowerVRProfilerClient>( RVRef(msgProducer) );

			CHECK( client->Add( pvr_prof_client->GetMsgConsumer() ));

			CHECK( _pvrProf.profiler.InitClient( pvr_prof_client ));
		}
	  #endif

		using ECounter = PowerVRProfiler::ECounter;

		_pvrProf.requiredCounters = PowerVRProfiler::ECounterSet{
						ECounter::GPU_MemoryInterfaceLoad,		ECounter::GPU_ClockSpeed,					ECounter::Tiler_TriangleRatio,
						ECounter::Texture_ReadStall,			ECounter::Shader_ShaderProcessingLoad,		ECounter::GPU_MemoryRead,
						ECounter::GPU_MemoryWrite,				ECounter::VertexShader_RegisterOverload,	ECounter::PixelShader_RegisterOverload,
						ECounter::Renderer_HSR_Efficiency,		ECounter::TDM_Active,						ECounter::SPM_Active,
						ECounter::Renderer_ISP_PixelLoad,		ECounter::Shader_CyclesPerComputeKernel,	ECounter::Shader_CyclesPerVertex,
						ECounter::Shader_CyclesPerPixel,		ECounter::ComputeShader_ProcessingLoad,		ECounter::VertexShader_ProcessingLoad,
						ECounter::PixelShader_ProcessingLoad,	ECounter::RendererActive,					ECounter::GeometryActive,
						ECounter::Texture_FetchesPerPixel,		ECounter::Texture_FilterCyclesPerFetch,		ECounter::Texture_FilterInputLoad,
						ECounter::Texture_FilterLoad,			ECounter::Texture_ReadCyclesPerFetch,		ECounter::GPU_MemoryTotal
					};

		if ( _isRemote )
		{
			_pvrProf.requiredCounters.insert( ECounter::RendererTime );
			_pvrProf.requiredCounters.insert( ECounter::GeometryTime );
			_pvrProf.requiredCounters.insert( ECounter::TDM_Time );
			_pvrProf.requiredCounters.insert( ECounter::Tiler_TrianglesInputPerSecond );
			_pvrProf.requiredCounters.insert( ECounter::Tiler_TrianglesOutputPerSecond );
		}
		else
		{
			_pvrProf.requiredCounters.insert( ECounter::RendererTimePerFrame );
			_pvrProf.requiredCounters.insert( ECounter::GeometryTimePerFrame );
			_pvrProf.requiredCounters.insert( ECounter::TDM_TimePerFrame );
			_pvrProf.requiredCounters.insert( ECounter::Tiler_TrianglesInputPerFrame );
			_pvrProf.requiredCounters.insert( ECounter::Tiler_TrianglesOutputPerFrame );
		}

		return _pvrProf.profiler.Initialize( _pvrProf.requiredCounters );
	}

/*
=================================================
	_InitAdrenoProf
=================================================
*/
	bool  HwpcProfiler::_InitAdrenoProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
	  #if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_ADRENO_PERFCOUNTER)
		Unused( client, msgProducer );
	  #else
		// initialize remote profiling
		if ( client )
		{
			CHECK( msgProducer->GetChannels() == EnumSet<EChannel>{EChannel::Reliable} );

			auto	adreno_prof_client = MakeRC<AdrenoProfilerClient>( RVRef(msgProducer) );

			CHECK( client->Add( adreno_prof_client->GetMsgConsumer() ));

			CHECK( _adrenoProf.profiler.InitClient( adreno_prof_client ));
		}
	  #endif

		using ECounter = AdrenoProfiler::ECounter;

		_adrenoProf.requiredCounters = AdrenoProfiler::ECounterSet{
				// LRZ //
					#if 1
						ECounter::LRZ_PrimKilledByMaskGen,		ECounter::LRZ_PrimKilledByLRZ,	ECounter::LRZ_PrimPassed,
						ECounter::LRZ_TileKilled,				ECounter::LRZ_TotalPixel,
						ECounter::LRZ_Read,						ECounter::LRZ_Write,
					#endif
				// Render backend //
					#if 1
						ECounter::RB_Z_Pass,					ECounter::RB_Z_Fail,			ECounter::RB_S_Fail,	ECounter::RB_TotalPass,
						ECounter::RB_ZRead,						ECounter::RB_ZWrite,
						ECounter::RB_CRead,						ECounter::RB_CWrite,
					//	ECounter::RB_AliveCycles2D,
					#endif
				// CCU //
					#if 1
					//	ECounter::CCU_PartialBlockRead,			ECounter::CCU_2DPixels,
						ECounter::CCU_DepthBlocks,				ECounter::CCU_ColorBlocks,
						ECounter::CCU_GMemRead,					ECounter::CCU_GMemWrite,
						ECounter::CCU_2dReadReq,				ECounter::CCU_2dWriteReq,
					#endif
				// Rasterizer //
					#if 1
						ECounter::RAS_SuperTiles,				ECounter::RAS_8x4Tiles,
						ECounter::RAS_FullyCoveredSuperTiles,	ECounter::RAS_FullyCovered8x4Tiles,
					#endif
				// Shader/Streaming Processor //
					#if 1
						ECounter::SSP_ALUcy,					ECounter::SSP_EFUcy,
						ECounter::SSP_VS_EFUInst,				ECounter::SSP_VS_FullALUInst,	ECounter::SSP_VS_HalfALUInst,
						ECounter::SSP_FS_EFUInst,				ECounter::SSP_FS_FullALUInst,	ECounter::SSP_FS_HalfALUInst,
						ECounter::SSP_L2Read,					ECounter::SSP_L2Write,
					#endif
				// Compression and Decompression //
					#if 1
						ECounter::CMP_2dReadData,				ECounter::CMP_2dWriteData,
					#endif
				};

		return _adrenoProf.profiler.Initialize( _adrenoProf.requiredCounters );
	}

/*
=================================================
	_InitGeneralProf
=================================================
*/
	bool  HwpcProfiler::_InitGeneralProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		Unused( client, msgProducer );
	  #else
		// initialize remote profiling
		if ( client )
		{
			CHECK( msgProducer->GetChannels() == EnumSet<EChannel>{EChannel::Reliable} );

			auto	gen_prof_client = MakeRC<GeneralProfilerClient>( RVRef(msgProducer) );

			CHECK( client->Add( gen_prof_client->GetMsgConsumer() ));

			CHECK( _genProf.profiler.InitClient( gen_prof_client ));
		}
	  #endif

		_genProf.requiredCounters = GeneralProfiler::ECounterSet{}.SetAll();

		return _genProf.profiler.Initialize( _genProf.requiredCounters );
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  HwpcProfiler::Deinitialize ()
	{
		_nvProf.profiler.Deinitialize();
		_armProf.profiler.Deinitialize();
		_pvrProf.profiler.Deinitialize();
		_genProf.profiler.Deinitialize();
		_maliProf.profiler.Deinitialize();
		_adrenoProf.profiler.Deinitialize();

		// free memory
		Reconstruct( _nvProf.counters );
		Reconstruct( _armProf.counters );
		Reconstruct( _pvrProf.counters );
		Reconstruct( _maliProf.counters );
		Reconstruct( _adrenoProf.counters );

		_initialized = false;
	}

/*
=================================================
	SampleGraphicsCounters
=================================================
*/
	void  HwpcProfiler::SampleGraphicsCounters (float invdt)
	{
		const auto	Sample = [b = not _isRemote, invdt] (auto& prof)
		{{
			if ( b and not prof.profiler.IsInitialized() )
				return;

			prof.invTimeDelta = invdt;
			prof.profiler.Sample( OUT prof.counters, INOUT prof.invTimeDelta );
		}};

		Sample( _nvProf );
		Sample( _pvrProf );
		Sample( _maliProf );
		Sample( _adrenoProf );
	}

/*
=================================================
	SampleCPUCounters
=================================================
*/
	void  HwpcProfiler::SampleCPUCounters (float invdt)
	{
		const auto	Sample = [b = not _isRemote, invdt] (auto& prof)
		{{
			if ( b and not prof.profiler.IsInitialized() )
				return;

			prof.invTimeDelta = invdt;
			prof.profiler.Sample( OUT prof.counters, INOUT prof.invTimeDelta );
		}};

		Sample( _armProf );
		Sample( _genProf );
	}


} // AE::Profiler
