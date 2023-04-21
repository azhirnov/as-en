// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	API for Adreno / Mali hardware performance counters.
*/

#pragma once

#include "base/Utils/EnumBitSet.h"

namespace AE::Profiler
{
	using namespace AE::Base;


	//
	// Android GPU Profiler (Adreno / Mali)
	//

	class AndroidGpuProfiler
	{
	// types
	public:
		enum class ECounter : ubyte
		{
			_Count,
			Unknown			= 0xFF,
		};
		using ECounterSet	= EnumBitSet< ECounter >;

	private:
		struct Impl;


	// variables
	private:
		Unique<Impl>	_impl;


	// methods
	public:
		AndroidGpuProfiler ()											__NE___;
		~AndroidGpuProfiler ()											__NE___;

		ND_ bool  Initialize (const ECounterSet &counterSet)			__NE___;
			void  Deinitialize ()										__NE___;
		ND_ bool  IsInitialized ()										C_NE___;
	};


} // AE::Profiler
