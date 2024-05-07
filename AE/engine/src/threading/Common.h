// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	All classes in 'Threading' module must be thread-safe.

	Prefixes:
		Lf		- lock-free / wait-free
		Ts		- thread-safe (with locks)
		Async	- thread-safe (with task system)

	'Must be externally synchronized' - means that mutex must be used
		with at least 'Acquire' memory barrier when locked and 'Release' memory barrier when unlocked.
*/

#pragma once

#include "threading/Threading.pch.h"

#ifdef AE_CI_BUILD_PERF
#	define AE_ENABLE_DATA_RACE_CHECK	0
#elif defined(AE_DEBUG) or defined(AE_CI_BUILD_TEST)
#	define AE_ENABLE_DATA_RACE_CHECK	1
#else
#	define AE_ENABLE_DATA_RACE_CHECK	0
#endif

// TODO: ThreadWakeup is slow
#define AE_USE_THREAD_WAKEUP	0


namespace AE::Threading
{
	using namespace AE::Base;


	class IAsyncTask;
	using AsyncTask = RC< IAsyncTask >;

	class IThread;
	class TaskScheduler;

} // AE::Threading


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  if AE_ENABLE_DATA_RACE_CHECK
#	pragma detect_mismatch( "AE_ENABLE_DATA_RACE_CHECK", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_DATA_RACE_CHECK", "0" )
#  endif

#  if AE_USE_THREAD_WAKEUP
#	pragma detect_mismatch( "AE_USE_THREAD_WAKEUP", "1" )
#  else
#	pragma detect_mismatch( "AE_USE_THREAD_WAKEUP", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
