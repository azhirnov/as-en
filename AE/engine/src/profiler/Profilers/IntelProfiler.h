// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Utils/EnumSet.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	class IntelProfilerClient;


	//
	// Intel GPU Profiler
	//

	class IntelProfiler
	{
	// types
	public:
		enum class ECounter : ubyte
		{
			ActFreq,			// MHz, actual achieved frequency
			RC6Residency,		// ms, RC6 residency
			Throttle,			// 0/1

			_Count
		};

		using ECounterSet	= EnumSet< ECounter >;
		using Counters_t	= FlatHashMap< ECounter, float >;
		using MHz_t			= uint;

		struct HWInfo
		{
			MHz_t		rp0Freq		= 0;
			MHz_t		rp1Freq		= 0;
			MHz_t		rpnFreq		= 0;
		};

	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		IntelProfiler ()											__NE___;
		~IntelProfiler ()											__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)		__NE___;
			void  Deinitialize ()									__NE___;
		ND_ bool  IsInitialized ()									C_NE___;

		ND_ ECounterSet	EnabledCounterSet ()						C_NE___;
		ND_ HWInfo		GetHWInfo ()								C_NE___;

			void  Sample (OUT Counters_t &, INOUT float &invdt)		C_NE___;


	  #ifndef AE_PLATFORM_LINUX
		ND_ bool  InitClient (RC<IntelProfilerClient>)				__NE___;
	  #endif
	};

} // AE::Profiler
