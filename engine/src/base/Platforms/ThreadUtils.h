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
			static void		SetName (NtStringView name);
		ND_ static String	GetName ();
		
		// interval ~10ms
			template <typename R, typename P>
			static void		Sleep (const std::chrono::duration<R,P>& relativeTime)		{ std::this_thread::sleep_for( relativeTime ); }
			
			template <typename R, typename P>
			static void		NanoSleep (const std::chrono::duration<R,P>& relativeTime)	{ _NanoSleep( relativeTime ); }

			template <typename R, typename P>
			static bool		WaitIO (const std::chrono::duration<R,P>& relativeTime)		{ return _WaitIOms( milliseconds{relativeTime} ); }

			static bool		SetAffinity (const std::thread::native_handle_type &handle, uint coreIdx);
			static bool		SetPriority (const std::thread::native_handle_type &handle, float priority);

		ND_	static uint		GetCoreIndex ();	// current logical CPU core
			
		ND_ static auto		GetID ()							{ return std::this_thread::get_id(); }
		ND_ static usize	GetIntID ()							{ return usize(HashOf( std::this_thread::get_id() )); }
		
		ND_ static auto		GetID (const std::thread &t)		{ return t.get_id(); }
		ND_ static usize	GetIntID (const std::thread &t)		{ return usize(HashOf( t.get_id() )); }

		ND_ static uint		MaxThreadCount ()					{ return std::thread::hardware_concurrency(); }
		
		// returns 'true' if has been switched to another thread
			static bool		Yield ();
			
			template <typename R, typename P>
			static void		YieldOrSleep (const std::chrono::duration<R,P>& relativeTime)	{ if_unlikely( not Yield() ) Sleep( relativeTime ); }
			static void		YieldOrSleep ()													{ if_unlikely( not Yield() ) Sleep( milliseconds{1} ); }

			static void		Pause ();

		ND_ static constexpr uint  SpinBeforeLock ()		{ return 1'000; }
		ND_ static constexpr uint  LargeSpinBeforeLock ()	{ return 10'000; }

	private:
			static bool  _WaitIOms (milliseconds relativeTime);
			static bool  _NanoSleep (nanoseconds relativeTime);
	};


} // AE::Base
