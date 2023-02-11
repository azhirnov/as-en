// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Prefixes:
		Lf	- lock-free / wait-free
		Ts	- thread-safe (with locks)
*/

#pragma once

#include "base/Defines/StdInclude.h"
#include "base/Utils/Threading.h"
#include "base/Utils/RefCounter.h"

#if not defined(AE_ENABLE_DATA_RACE_CHECK) and (defined(AE_DBG_OR_DEV) or defined(AE_CI_BUILD))
#	define AE_ENABLE_DATA_RACE_CHECK	1
#else
#	define AE_ENABLE_DATA_RACE_CHECK	0
#endif


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

#endif // AE_CPP_DETECT_MISMATCH
