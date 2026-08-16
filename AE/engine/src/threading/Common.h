// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
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

#include "pch/Base.h"

#ifdef AE_CI_BUILD_PERF
#	define AE_ENABLE_DATA_RACE_CHECK	0
#elif defined(AE_DEBUG) or defined(AE_CI_BUILD_TEST)
#	define AE_ENABLE_DATA_RACE_CHECK	1
#else
#	define AE_ENABLE_DATA_RACE_CHECK	0
#endif

namespace AE::_Coro_
{
	class AsyncTaskImpl;
}

namespace AE::Threading
{
	using namespace AE::Base;

	ImportBitOperators;

	using AsyncTask		= RC< AE::_Coro_::AsyncTaskImpl >;
	using AtomicTask	= AtomicRC< AE::_Coro_::AsyncTaskImpl >;

	class IThread;
	class LfTaskQueue;
	class TaskScheduler;

} // AE::Threading


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  if AE_ENABLE_DATA_RACE_CHECK
#	pragma detect_mismatch( "AE_ENABLE_DATA_RACE_CHECK", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_DATA_RACE_CHECK", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
