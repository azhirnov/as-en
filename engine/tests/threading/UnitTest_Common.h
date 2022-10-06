// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"
#include "base/Platforms/Platform.h"
#include "base/Algorithms/StringUtils.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Threading;


struct LocalTaskScheduler
{
	LocalTaskScheduler (usize maxWorkerThreads)
	{
		TaskScheduler::CreateInstance();
		Scheduler().Setup( maxWorkerThreads );
	}

	~LocalTaskScheduler ()
	{
		Scheduler().Release();
		TaskScheduler::DestroyInstance();
	}

	TaskScheduler* operator -> ()
	{
		return &Scheduler();
	}
};
