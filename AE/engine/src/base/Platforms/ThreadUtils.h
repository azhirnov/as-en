// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/Threading.h"
#include "base/Platforms/Platform.h"

namespace AE::Base
{

	//
	// Thread Utils
	//

	struct ThreadUtils : Noninstanceable
	{
		// Name //
	  #ifndef AE_CFG_RELEASE
			static void		SetName (NtStringView name)										__NE___	{ return PlatformUtils::SetCurrentThreadName( name ); }
		ND_ static String	GetName ()														__Th___	{ return PlatformUtils::GetCurrentThreadName(); }
	  #else
			static void		SetName (NtStringView)											__NE___	{}
		//ND_ static String	GetName ()														__NE___	{ return Default; }
	  #endif


		// Sleep //
			static void		MilliSleep (milliseconds relativeTime)							__NE___	{ return PlatformUtils::ThreadMilliSleep( relativeTime ); }
			static bool		MicroSleep (nanoseconds relativeTime)							__NE___	{ return PlatformUtils::ThreadMicroSleep( relativeTime ); }
			static void		NanoSleep (nanoseconds relativeTime)							__NE___	{ return PlatformUtils::ThreadNanoSleep( relativeTime ); }

			static void		ProgressiveSleep (uint)											__NE___;
			static void		ProgressiveSleepInf (uint)										__NE___;
			static void		ProgressiveSleep (uint, milliseconds maxWait)					__NE___;

			static void		Pause ()														__NE___	{ return PlatformUtils::ThreadPause(); }		// keep high CPU frequency
			static void		Sleep_1us ()													__NE___	{ return PlatformUtils::ThreadSleep_1us(); }	// ARM64: low power mode
			static void		Sleep_500us ()													__NE___	{ return PlatformUtils::ThreadSleep_500us(); }	// All: low power mode
			static void		Sleep_15ms ()													__NE___	{ return PlatformUtils::ThreadSleep_15ms(); }	// All: low power mode

		NdCx__ static auto  NanoSleepTimeStep ()											__NE___	{ return PlatformUtils::NanoSleepTimeStep(); }
		NdCx__ static auto  MicroSleepTimeStep ()											__NE___	{ return PlatformUtils::MicroSleepTimeStep(); }
		NdCx__ static auto  MilliSleepTimeStep ()											__NE___	{ return PlatformUtils::MilliSleepTimeStep(); }

		NdCx__ static uint  SpinBeforeLock ()												__NE___	{ return 1'000; }


		// Handle / Affinity / Priority //
		ND_ static auto		GetHandle ()													__NE___	{ return PlatformUtils::GetCurrentThreadHandle(); }
		ND_ static bool		IsCurrent (const ThreadHandle &handle)							__NE___	{ return handle == PlatformUtils::GetCurrentThreadHandle(); }

			static bool		SetAffinity (const ThreadHandle &handle, uint logicalCoreIdx)	__NE___	{ return PlatformUtils::SetThreadAffinity( handle, logicalCoreIdx ); }
			static bool		SetPriority (const ThreadHandle &h, EThreadPriority priority)	__NE___	{ return PlatformUtils::SetThreadPriority( h, priority ); }

			static bool		SetAffinity (uint logicalCoreIdx)								__NE___	{ return PlatformUtils::SetCurrentThreadAffinity( logicalCoreIdx ); }
			static bool		SetPriority (EThreadPriority priority)							__NE___	{ return PlatformUtils::SetCurrentThreadPriority( priority ); }

		ND_ static Bytes	GetDefaultStackSize ()											__NE___	{ return PlatformUtils::GetDefaultStackSize(); }


		// ID //
		ND_	static uint		LogicalCoreIndex ()												__NE___	{ return PlatformUtils::GetLogicalCoreIndex(); }

		ND_ static auto		GetID ()														__NE___	{ return std::this_thread::get_id(); }
		ND_ static usize	GetIntID ()														__NE___;

		ND_ static auto		GetID (const StdThread &t)										__NE___	{ return t.get_id(); }
		ND_ static usize	GetIntID (const StdThread &t)									__NE___;

		ND_ static uint		MaxThreadCount ()												__NE___	{ return std::thread::hardware_concurrency(); }
	};


/*
=================================================
	ProgressiveSleep
=================================================
*/
	inline void  ThreadUtils::ProgressiveSleep (const uint iteration) __NE___
	{
		ProgressiveSleep( iteration, seconds{1} );
	}

	inline void  ThreadUtils::ProgressiveSleepInf (const uint iteration) __NE___
	{
		ProgressiveSleep( iteration, std::chrono::hours{30*24} );
	}

	inline void  ThreadUtils::ProgressiveSleep (const uint iteration, const milliseconds maxWait) __NE___
	{
		constexpr uint	nano_sleep_count	= 8;
		constexpr uint	micro_sleep_count	= nano_sleep_count + 8;

		if_likely( iteration < nano_sleep_count )
		{
			Sleep_1us();
			return;
		}

		// power safe mode
		if_likely( iteration < micro_sleep_count )
		{
			Sleep_500us();
			return;
		}

		// multiple threads at the same core is rare case
		if ( not PlatformUtils::SwitchToPendingThread() )
		{
			Unused( maxWait );
			ASSERT_MSG( (iteration - micro_sleep_count) * milliseconds{15} < maxWait, "deadlock" );

			Sleep_15ms();
		}
	}
	
/*
=================================================
	GetIntID
=================================================
*/
	inline usize  ThreadUtils::GetIntID () __NE___
	{
		auto	id = std::this_thread::get_id();
		return usize{BitCast< ToUnsignedInteger<decltype(id)> >( id )};
	}
	
	inline usize  ThreadUtils::GetIntID (const StdThread &t) __NE___
	{
		auto	id = t.get_id();
		return usize{BitCast< ToUnsignedInteger<decltype(id)> >( id )};
	}

} // AE::Base
