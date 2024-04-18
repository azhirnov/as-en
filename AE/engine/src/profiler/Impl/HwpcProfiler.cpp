// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Random.h"
#include "base/Algorithms/StringUtils.h"
#include "profiler/Impl/HwpcProfiler.h"
#include "profiler/Remote/RemoteArmProfiler.h"
#include "graphics/Public/GraphicsImpl.h"

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
	Draw
=================================================
*/
	void  HwpcProfiler::Draw (Canvas &)
	{
		if ( not _initialized )
			return;
	}

/*
=================================================
	Update
=================================================
*/
	void  HwpcProfiler::Update (secondsf dt)
	{
		Unused( dt );

		if ( not _initialized )
			return;

		_UpdateCpuUsage();

	  #ifdef AE_ENABLE_IMGUI
		_UpdateArmCountersImGui( 1.0 / double(dt.count()) );
		_UpdateAndroidGpuCountersImGui( dt );
	  #endif

		// TODO: remove
	  #if 0
		static Random	rnd;

		_armProf.globalMemStalls .AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });
		_armProf.globalMemTraffic.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });
		_armProf.globalMemAccess .AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });
		_armProf.cacheLookups    .AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });

		_armProf.gpuCycles		.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});
		_armProf.shaderCycles	.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });
		_armProf.shaderJobs		.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });

		_armProf.primitives		.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });
		_armProf.zTest			.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f ), rnd.Uniform( 0.f, 100.f ) });

		_armProf.textureCycles	.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});
		_armProf.tiles			.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});
		_armProf.loadStoreCycles.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});

		_armProf.cpuCycles		.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});
		_armProf.cacheMisses	.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});
		_armProf.branchMisses	.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});
		_armProf.cacheRefs		.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});
		_armProf.branchInst		.AddAndUpdateRange({ rnd.Uniform( 0.f, 100.f )});

	  #endif
	}

/*
=================================================
	_UpdateCpuUsage
=================================================
*/
	void  HwpcProfiler::_UpdateCpuUsage ()
	{
		StaticArray< float, 64 >	user, kernel;

		CpuPerformance::GetUsage( OUT user.data(), OUT kernel.data(), uint(user.size()) );

		const auto&		cpu_info = CpuArchInfo::Get();
		for (auto& core : cpu_info.cpu.coreTypes)
		{
			for (uint core_id : BitIndexIterate( core.logicalBits.to_ullong() ))
			{
				auto&	graph = _cpuUsage.coreUsage[core_id];

				graph.Add({ user[core_id] + kernel[core_id], kernel[core_id] });
			}
		}
	}

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

		// CPU usage
		#ifdef AE_PLATFORM_WINDOWS
		_initialized |= true;
		#endif

		#ifdef AE_ENABLE_IMGUI
		_InitImGui();
		#endif

		return true;
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

		#ifndef AE_PLATFORM_ANDROID
		if ( client )
		{
			CHECK( msgProducer->GetChannels() == EnumSet<EChannel>{EChannel::Reliable} );

			auto	arm_prof_client = MakeRC<ArmProfilerClient>( RVRef(msgProducer) );

			CHECK( client->Add( arm_prof_client->GetMsgConsumer() ));

			CHECK( _armProf.profiler.InitClient( arm_prof_client ));
		}
		#else
			Unused( client, msgProducer );
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
						ECounter::GPU_ShaderTextureCycles,			ECounter::GPU_Tiles,				ECounter::GPU_ShaderLoadStoreCycles
					});
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

		_initialized = false;
	}

} // AE::Profiler
