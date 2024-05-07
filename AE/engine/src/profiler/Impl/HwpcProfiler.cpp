// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "base/Algorithms/StringUtils.h"
#include "profiler/Impl/HwpcProfiler.h"
#include "profiler/Remote/RemoteArmProfiler.h"
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

		_initialized |= _InitNvProf();
		_initialized |= _InitArmProf( client, msgProducer );
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


	  #ifdef AE_ENABLE_IMGUI
		const float	inv_fc = 1.f / float(frameCount);

		_UpdateCpuUsageImGui();
		_UpdateArmCountersImGui( double(inv_fc) );
		_UpdateAdrenoCountersImGui( inv_fc );
		_UpdatePowerVRCountersImGui( inv_fc );
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
	bool  HwpcProfiler::_InitNvProf ()
	{
		#ifdef AE_ENABLE_VULKAN
		{
			auto&	dev = GraphicsScheduler().GetDevice();
			if ( dev.HasNvPerf() )
			{
				_nvPerf = &dev.GetNvPerf();
				return true;
			}
		}
		#endif
		return false;
	}

/*
=================================================
	_InitArmProf
=================================================
*/
	bool  HwpcProfiler::_InitArmProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
		using ECounter = ArmProfiler::ECounter;

	  #if defined(AE_ENABLE_REMOTE_GRAPHICS) or defined(AE_ENABLE_ARM_HWCPIPE)
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

		return _armProf.profiler.Initialize( ArmProfiler::ECounterSet{
					// processor
						ECounter::CPU_Cycles,
						ECounter::CPU_CacheMisses,					ECounter::CPU_CacheReferences,
						ECounter::CPU_BranchMisses,					ECounter::CPU_BranchInstructions,
					// graphics
						ECounter::GPU_ExternalMemoryReadStalls,		ECounter::GPU_ExternalMemoryWriteStalls,
						ECounter::GPU_ExternalMemoryReadBytes,		ECounter::GPU_ExternalMemoryWriteBytes,
						ECounter::GPU_ExternalMemoryReadAccesses,	ECounter::GPU_ExternalMemoryWriteAccesses,
						ECounter::GPU_Tiles,
						ECounter::GPU_CulledPrimitives,				ECounter::GPU_VisiblePrimitives,	ECounter::GPU_InputPrimitives,
						ECounter::GPU_EarlyZTests,					ECounter::GPU_EarlyZKilled,
						ECounter::GPU_LateZTests,					ECounter::GPU_LateZKilled,
						ECounter::GPU_Cycles,
						ECounter::GPU_CacheReadLookups,				ECounter::GPU_CacheWriteLookups,
						ECounter::GPU_VertexCycles,					ECounter::GPU_FragmentCycles,		ECounter::GPU_ComputeCycles,	ECounter::GPU_VertexComputeCycles,
						ECounter::GPU_VertexJobs,					ECounter::GPU_FragmentJobs,			ECounter::GPU_ComputeJobs,		ECounter::GPU_VertexComputeJobs,
						ECounter::GPU_ShaderTextureCycles,			ECounter::GPU_Tiles,				ECounter::GPU_ShaderLoadStoreCycles,
						ECounter::GPU_TransactionEliminations,		ECounter::GPU_ShaderArithmeticCycles,	ECounter::GPU_ShaderInterpolatorCycles
					});
	}

/*
=================================================
	_InitPowerVRProf
=================================================
*/
	bool  HwpcProfiler::_InitPowerVRProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
		using ECounter = PowerVRProfiler::ECounter;

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

		return _pvrProf.profiler.Initialize( PowerVRProfiler::ECounterSet{
						ECounter::GPU_MemoryInterfaceLoad,		ECounter::GPU_ClockSpeed,					ECounter::Tiler_TriangleRatio,
						ECounter::Texture_ReadStall,			ECounter::Shader_ShaderProcessingLoad,		ECounter::GPU_MemoryRead,
						ECounter::GPU_MemoryWrite,				ECounter::VertexShader_RegisterOverload,	ECounter::PixelShader_RegisterOverload,
						ECounter::Tiler_TrianglesInputPerFrame,	ECounter::Tiler_TrianglesOutputPerFrame,	ECounter::Renderer_HSR_Efficiency,
						ECounter::Renderer_ISP_PixelLoad,		ECounter::RendererTimePerFrame,				ECounter::GeometryTimePerFrame,
						ECounter::TDM_TimePerFrame,				ECounter::Shader_CyclesPerComputeKernel,	ECounter::Shader_CyclesPerVertex,
						ECounter::Shader_CyclesPerPixel,		ECounter::ComputeShader_ProcessingLoad,		ECounter::VertexShader_ProcessingLoad,
						ECounter::PixelShader_ProcessingLoad,	ECounter::RendererActive,					ECounter::GeometryActive,
						ECounter::TDM_Active,					ECounter::SPM_Active
					});
	}

/*
=================================================
	_InitAdrenoProf
=================================================
*/
	bool  HwpcProfiler::_InitAdrenoProf (ClientServer_t client, MsgProducer_t msgProducer)
	{
		using ECounter = AdrenoProfiler::ECounter;

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

		return _adrenoProf.profiler.Initialize( AdrenoProfiler::ECounterSet{}.SetAll() );
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  HwpcProfiler::Deinitialize ()
	{
		#ifdef AE_ENABLE_VULKAN
		_nvPerf = null;
		#endif

		_armProf.profiler.Deinitialize();
		_pvrProf.profiler.Deinitialize();
		_adrenoProf.profiler.Deinitialize();

		Reconstruct( _armProf.counters );
		Reconstruct( _pvrProf.counters );
		Reconstruct( _adrenoProf.counters );

		_initialized = false;
	}

} // AE::Profiler
