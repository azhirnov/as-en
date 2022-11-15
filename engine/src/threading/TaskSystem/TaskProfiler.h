// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Common.h"

namespace AE::Threading
{

	//
	// Task Profiler interface
	//

	class ITaskProfiler : public EnableRC<ITaskProfiler>
	{
	// interface
	public:
		// Before task execution in current thread.
		virtual void  Begin (const IAsyncTask &) = 0;

		// After task execution in current thread.
		virtual void  End (const IAsyncTask &) = 0;

		// Task added to the queue at first time or when rerunning, when used 'Continue()'.
		virtual void  Enqueue (const IAsyncTask &) = 0;

		// When new thread are added to task scheduler.
		virtual void  AddThread (const IThread &) = 0;
	};


} // AE::Threading