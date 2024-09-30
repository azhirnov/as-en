// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class NVidiaProfilerClient;


	//
	// NVidia GPU Profiler
	//

	class NVidiaProfiler
	{
	// types
	public:
		enum class ECounter : ubyte
		{
			MemoryUtil,			// %
			GpuUtil,			// %

			DevMemUsed,			// %
			UnifiedMemUsed,		// %

			DevMemUsedMb,		// Mb
			UnifiedMemUsedMb,	// Mb

			GraphicsClock,		// MHz
			SMClock,			// MHz
			MemoryClock,		// MHz
			VideoClock,			// MHz

			GpuTemperature,		// C
			PowerUsage,			// Watts
			FanSpeed,			// intended fan speed in %, may be > 100%
			PerfState,			// 0..1

			_Count
		};

		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, float >;
		using MHz_t			= uint;

		enum class EArch : ubyte
		{
			Unknown	= 0,
			Kepler,
			Maxwell,
			Pascal,
			Volta,
			Turing,
			Ampere,
			Ada,
		};

		struct Clock
		{
			MHz_t		def		= 0;
			MHz_t		max		= 0;
			MHz_t		boost	= 0;
		};

		struct HWInfo
		{
			String		name;
			Version2	cudaCompCap;
			EArch		arch		= Default;

			struct {
				ubyte		gen			= 0;
				ubyte		maxGen		= 0;
				uint		width		= 0;
				uint		maxWidth	= 0;
			}			pcie;

			Clock		graphicsClock;
			Clock		smClock;
			Clock		memClock;
			Clock		videoClock;

			Bytes		devMemory;
			Bytes		unifiedMemory;
		};

	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		NVidiaProfiler ()											__NE___;
		~NVidiaProfiler ()											__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)		__NE___;
			void  Deinitialize ()									__NE___;
		ND_ bool  IsInitialized ()									C_NE___;

		//	bool  SetStableClockState (bool stable)					C_NE___;	// TODO

		ND_ ECounterSet	EnabledCounterSet ()						C_NE___;
		ND_ HWInfo		GetHWInfo ()								C_NE___;

			void  Sample (OUT Counters_t &, INOUT float &invdt)		C_NE___;


	  #ifndef AE_ENABLE_NVML
		ND_ bool  InitClient (RC<NVidiaProfilerClient>)				__NE___;
	  #endif
	};

} // AE::Profiler
