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
			nanoseconds			sleepStep;
			nanoseconds			maxSleepOnIdle;
			FixedString<64>		name;

		// methods
			WorkerConfig ()						__NE___	{}
			WorkerConfig (WorkerConfig &&)		__NE___	= default;
			WorkerConfig (const WorkerConfig &)	__NE___	= default;

			WorkerConfig (const EThreadArray &threads, nanoseconds sleepStep, nanoseconds maxSleepOnIdle, StringView name) __NE___ :
				threads{threads}, sleepStep{sleepStep}, maxSleepOnIdle{maxSleepOnIdle}, name{name} {}
				
			ND_ static WorkerConfig  CreateNonSleep (StringView name = "worker")								__NE___	{ return CreateNonSleep( EThreadArray{ EThread::Worker }, name ); }
			ND_ static WorkerConfig  CreateNonSleep (const EThreadArray &threads, StringView name = "worker")	__NE___;
			
			ND_ static WorkerConfig  CreateDefault (StringView name = "worker")									__NE___	{ return CreateDefault( EThreadArray{ EThread::Worker }, name ); }
			ND_ static WorkerConfig  CreateDefault (const EThreadArray &threads, StringView name = "worker")	__NE___;
		};


	// methods
	public:
		ND_ static RC<IThread>  CreateThread (const WorkerConfig &cfg)	__NE___;
	};


} // AE::Threading