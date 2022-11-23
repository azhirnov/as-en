// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"
#include "base/Platforms/Platform.h"
#include "base/Algorithms/StringUtils.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Threading;

enum class WorkerQueueCount	: uint {};
enum class IOThreadCount	: uint {};


struct LocalTaskScheduler
{
	explicit LocalTaskScheduler (WorkerQueueCount count)
	{
		TaskScheduler::Config	cfg;
		cfg.maxWorkerQueues = uint(count);
		
		TaskScheduler::CreateInstance();
		TEST( Scheduler().Setup( cfg ));
	}
	
	explicit LocalTaskScheduler (IOThreadCount count)
	{
		TaskScheduler::Config	cfg;
		cfg.maxWorkerQueues	= 1;
		cfg.maxIOThreads	= uint(count);

		TaskScheduler::CreateInstance();
		TEST( Scheduler().Setup( cfg ));
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
