// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/Helpers.h"

namespace AE::Base
{

	//
	// Thread Utils
	//

	struct ThreadUtils : Noninstancable
	{
			static void		SetName (NtStringView name)									__NE___;
		ND_ static String	GetName ();
		
		// interval ~10ms
			template <typename R, typename P>
			static void		Sleep (const std::chrono::duration<R,P>& relativeTime)		__NE___	{ std::this_thread::sleep_for( relativeTime ); }
			
			template <typename R, typename P>
			static void		NanoSleep (const std::chrono::duration<R,P>& relativeTime)	__NE___	{ _NanoSleep( relativeTime ); }

			template <typename R, typename P>
			static bool		WaitIO (const std::chrono::duration<R,P>& relativeTime)		__NE___	{ return _WaitIOms( milliseconds{relativeTime} ); }

			static bool		SetAffinity (const std::thread::native_handle_type &handle, uint coreIdx)	__NE___;
			static bool		SetPriority (const std::thread::native_handle_type &handle, float priority) __NE___;

		ND_	static uint		GetCoreIndex ()												__NE___;	// current logical CPU core
			
		ND_ static auto		GetID ()													__NE___	{ return std::this_thread::get_id(); }
		ND_ static usize	GetIntID ()													__NE___	{ return usize(HashOf( std::this_thread::get_id() )); }
		
		ND_ static auto		GetID (const std::thread &t)								__NE___	{ return t.get_id(); }
		ND_ static usize	GetIntID (const std::thread &t)								__NE___	{ return usize(HashOf( t.get_id() )); }

		ND_ static uint		MaxThreadCount ()											__NE___	{ return std::thread::hardware_concurrency(); }
		
		// returns 'true' if has been switched to another thread
			static bool		Yield ()													__NE___;
			
			template <typename R, typename P>
			static void		YieldOrSleep (const std::chrono::duration<R,P>& relativeTime)__NE___{ if_unlikely( not Yield() ) Sleep( relativeTime ); }
			static void		YieldOrSleep ()												__NE___	{ if_unlikely( not Yield() ) Sleep( milliseconds{1} ); }

			static void		Pause ()													__NE___;

		ND_ static constexpr uint  SpinBeforeLock ()									__NE___	{ return 1'000; }
		ND_ static constexpr uint  LargeSpinBeforeLock ()								__NE___	{ return 10'000; }

	private:
			static bool		_WaitIOms (milliseconds relativeTime)						__NE___;
			static bool		_NanoSleep (nanoseconds relativeTime)						__NE___;
	};


} // AE::Base
