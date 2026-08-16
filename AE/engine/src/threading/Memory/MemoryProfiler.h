// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe: yes
*/

#pragma once

#include "threading/Common.h"

namespace AE::Threading
{

	//
	// Memory Profiler interface
	//

	class IMemoryProfiler : public EnableRC<IMemoryProfiler>
	{
	};


} // AE::Threading
