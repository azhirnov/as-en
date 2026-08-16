// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	'DelayedRun' dependency used instead of thread sleep functions.
*/

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Delayed Run
	//

	struct DelayedRun
	{
		friend class DelayedTaskRunner;

	private:
		HighResClock::time_point	_endTime;
		DEBUG_ONLY( SourceLoc		_loc;)

	public:
		DelayedRun (nanoseconds delay, const SourceLoc &loc = SourceLoc::current())					__NE___ : _endTime{ HighResClock::now() + delay }	DEBUG_ONLY(, _loc{loc}) { Unused(loc); }
		DelayedRun (HighResClock::time_point endTime, const SourceLoc &loc = SourceLoc::current())	__NE___ : _endTime{ endTime }						DEBUG_ONLY(, _loc{loc}) { Unused(loc); }
	};

	#define Coro_Delay( _tp_ )		co_await Tuple{ Threading::DelayedRun{_tp_} };



	//
	// Delayed Task Runner
	//

	class DelayedTaskRunner final : public ITaskDependencyManager
	{
	// types
	private:
		struct Dependency
		{
			HighResClock::time_point	endTime;
			AsyncTask					task;
			DEBUG_ONLY( SourceLoc		loc;)
		};

		using DependencyArr_t	= Array< Dependency >;


		class DelayedTaskRunnerService final : public IOService
		{
		private:
			DelayedTaskRunner &		_mngr;
			DependencyArr_t			_delayedTasks;
			Threading::SpinLock		_guard;

		public:
			DelayedTaskRunnerService (DelayedTaskRunner &mngr)	__NE___;

			// IOService //
			usize			ProcessEvents ()					__NE_OV;
			EIOServiceType	GetIOServiceType ()					C_NE_OV	{ return EIOServiceType::DelayedRun; }
		};


	// variables
	private:
		Threading::Synchronized<
			Threading::RWSpinLock,
			DependencyArr_t >			_pendingTasks;


	// methods
	public:
		ND_ static bool  Register ()									__NE___;

		// ITaskDependencyManager //
		bool  Resolve (AnyTypeCRef dep, Task &, Bool defaultIsStrongDep)__NE_OV;

	private:
		DelayedTaskRunner ()											__NE___;
	};


} // AE::Threading
