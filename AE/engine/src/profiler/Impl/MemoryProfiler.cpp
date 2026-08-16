// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
#endif
#include "profiler/Impl/MemoryProfiler.h"

namespace AE::Profiler
{

/*
=================================================
	constructor
=================================================
*/
	MemoryProfiler::MemoryProfiler (TimePoint_t startTime) __NE___ :
		ProfilerUtils{ startTime }
	{}

} // AE::Profiler
