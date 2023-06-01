// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/EnumBitSet.h"
#include "base/Containers/FixedString.h"
#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Thread Manager
	//

	class ThreadMngr final : public Noninstancable
	{
	// types
	public:

		//
		// Worker Thread Config
		//
		struct WorkerConfig
		{
		// variables
			EThreadArray		threads;
			nanoseconds			sleepStep		{0};
			nanoseconds			maxSleepOnIdle	{0};
			FixedString<64>		name;

		// methods
			WorkerConfig ()						__NE___	{}
			WorkerConfig (WorkerConfig &&)		__NE___	= default;
			WorkerConfig (const WorkerConfig &)	__NE___	= default;

			WorkerConfig (const EThreadArray &threads, nanoseconds sleepStep, nanoseconds maxSleepOnIdle, StringView name) __NE___ :
				threads{threads}, sleepStep{sleepStep}, maxSleepOnIdle{maxSleepOnIdle}, name{name} {}
				
			ND_ static WorkerConfig  CreateNonSleep (StringView name = "worker")								__NE___	{ return CreateNonSleep( EThreadArray{ ETaskQueue::PerFrame }, name ); }
			ND_ static WorkerConfig  CreateNonSleep (const EThreadArray &threads, StringView name = "worker")	__NE___;
			
			ND_ static WorkerConfig  CreateDefault (StringView name = "worker")									__NE___	{ return CreateDefault( EThreadArray{ ETaskQueue::PerFrame }, name ); }
			ND_ static WorkerConfig  CreateDefault (const EThreadArray &threads, StringView name = "worker")	__NE___;
		};


	// methods
	public:
		ND_ static RC<IThread>  CreateThread (const WorkerConfig &cfg)							__NE___;

		ND_ static bool  SetupThreads (const TaskScheduler::Config	&cfg,
									   EnumBitSet<EThread>			 mask,
									   uint							 maxThreads,
									   OUT EThreadArray				&allowProcessInMain)		__NE___;

	private:
		friend class TaskScheduler;
		ND_ static RC<IThread>  _CreateMainThread ()											__NE___;
		
		ND_ static bool  _SetupThreads_v1 (TaskScheduler				&scheduler,
										   const EnumBitSet<EThread>	 mask,
										   OUT EThreadArray				&allowProcessInMain)	__NE___;

		ND_ static bool  _SetupThreads_v2 (TaskScheduler				&scheduler,
										   const CpuArchInfo			&cpuInfo,
										   const EnumBitSet<EThread>	 mask,
										   const uint					 maxThreads,
										   OUT EThreadArray				&allowProcessInMain)	__NE___;
	};


} // AE::Threading
