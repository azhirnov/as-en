// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Memory/MemoryProfiler.h"
#include "profiler/Impl/ProfilerUtils.h"

namespace AE::Profiler
{
	
	//
	// Memory Profiler
	//

	class MemoryProfiler final : public Threading::IMemoryProfiler, public ProfilerUtils
	{
	// methods
	public:
		explicit MemoryProfiler (TimePoint_t startTime);
		~MemoryProfiler () {}
		
		void  DrawImGUI () {}
		void  Draw (Canvas &) {}
		void  Update () {}
	};


} // AE::Profiler