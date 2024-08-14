// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "base/Algorithms/StringUtils.h"
#include "profiler/Impl/HwpcProfiler.h"
#include "profiler/Remote/RemoteArmProfiler.h"
#include "profiler/Remote/RemoteMaliProfiler.h"
#include "profiler/Remote/RemoteNVidiaProfiler.h"
#include "profiler/Remote/RemoteAdrenoProfiler.h"
#include "profiler/Remote/RemotePowerVRProfiler.h"
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

		_initialized |= _InitNvProf( client, msgProducer );
		_initialized |= _InitArmProf( client, msgProducer );
		_initialized |= _InitMaliProf( client, msgProducer );
		_initialized |= _InitAdrenoProf( client, msgProducer );
		_initialized |= _InitPowerVRProf( client, msgProducer );

		// CPU usage
		{
			#if defined(AE_PLATFORM_WINDOWS) and not defined(AE_ENABLE_REMOTE_GRAPHICS)
			_cpuUsage.enabled = true;
			#endif
			_initialized |= _cpuUsage.enabled;
		}

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
	void  HwpcProfiler::Update (secondsf, uint frameCount)
	{
		if ( not _initialized )
			return;

		SampleGraphicsCounters();
		SampleCPUCounters();

	  #ifdef AE_ENABLE_IMGUI
		const float	inv_fc = 1.f / float(frameCount);

		_UpdateCpuUsageImGui();
		_UpdateArmCountersImGui( double(inv_fc) );
		_UpdateMaliCountersImGui( double(inv_fc) );
		_UpdateAdrenoCountersImGui( inv_fc );
		_UpdatePowerVRCountersImGui( inv_fc );
		_UpdateNVidiaCountersImGui( inv_fc );
	  #else
		Unused( frameCount );
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
				// External memory //
					#if 1
						ECounter::ExtBusRdBy,			ECounter::ExtBusWrBy,
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
						ECounter::FragTileKillRate,
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
						ECounter::FragWarp,				ECounter::NonFragWarp,			ECounter::CoreFullQdWarp,		ECounter::CoreAllRegsWarp,
						ECounter::CoreAllRegsWarpRate,
						ECounter::CoreFullQdWarpRate,
						ECounter::FragRastPartQdRate,
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
						ECounter::Tiler_TrianglesInputPerFrame,	ECounter::Tiler_TrianglesOutputPerFrame,	ECounter::Renderer_HSR_Efficiency,
						ECounter::Renderer_ISP_PixelLoad,		ECounter::RendererTimePerFrame,				ECounter::GeometryTimePerFrame,
						ECounter::TDM_TimePerFrame,				ECounter::Shader_CyclesPerComputeKernel,	ECounter::Shader_CyclesPerVertex,
						ECounter::Shader_CyclesPerPixel,		ECounter::ComputeShader_ProcessingLoad,		ECounter::VertexShader_ProcessingLoad,
						ECounter::PixelShader_ProcessingLoad,	ECounter::RendererActive,					ECounter::GeometryActive,
						ECounter::TDM_Active,					ECounter::SPM_Active
					};

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

		_adrenoProf.requiredCounters = AdrenoProfiler::ECounterSet{}.SetAll();

		return _adrenoProf.profiler.Initialize( _adrenoProf.requiredCounters );
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
	void  HwpcProfiler::SampleGraphicsCounters ()
	{
		const auto	Sample = [] (auto& prof)
		{{
			if ( not prof.profiler.IsInitialized() )
				return;

			prof.profiler.Sample( OUT prof.counters );
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
	void  HwpcProfiler::SampleCPUCounters ()
	{
		const auto	Sample = [] (auto& prof)
		{{
			if ( not prof.profiler.IsInitialized() )
				return;

			prof.profiler.Sample( OUT prof.counters );
		}};

		Sample( _armProf );
	}


} // AE::Profiler
