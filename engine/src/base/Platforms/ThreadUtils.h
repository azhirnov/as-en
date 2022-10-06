// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/NtStringView.h"
#include <thread>

#ifdef AE_PLATFORM_APPLE
# include <pthread.h>
#endif

namespace AE::Base
{




	//
	// Thread Utils
	//

	struct ThreadUtils
	{
			static void		SetName (NtStringView name);
		ND_ static String	GetName ();
		
			template <typename R, typename P>
			static void		Sleep (const std::chrono::duration<R,P>& relativeTime)	{ std::this_thread::sleep_for( relativeTime ); }
			
			template <typename R, typename P>
			static bool		WaitIO (const std::chrono::duration<R,P>& relativeTime)	{ return _WaitIOms( std::chrono::duration_cast<milliseconds>( relativeTime ).count() ); }

			static bool		SetAffinity (const std::thread::native_handle_type &handle, uint coreIdx);
			static bool		SetPriority (const std::thread::native_handle_type &handle, float priority);

		ND_	static uint		GetCoreIndex ();	// current logical CPU core
			
		ND_ static auto		GetID ()			{ return std::this_thread::get_id(); }
		ND_ static usize	GetIntID ()			{ return usize(HashOf( std::this_thread::get_id() )); }

		ND_ static uint		MaxThreadCount ()	{ return std::thread::hardware_concurrency(); }

			static void		Yield ()
			{
				#if defined(AE_CPU_ARCH_X86) or defined(AE_CPU_ARCH_X64)
					// if hyperthreading is supported - switch to another thread
					_mm_pause(); // or __builtin_ia32_pause
				#elif defined(AE_PLATFORM_APPLE)
					// Notifies the scheduler that the current thread is willing to release its processor to other threads of the same or higher priority.
					::pthread_yield_np();
				#elif defined(AE_PLATFORM_ANDROID)
					__builtin_arm_yield();	// __yield()
				#else
					std::this_thread::yield();
				#endif
			}

	private:
			static bool		_WaitIOms (ulong ms);
	};

} // AE::Base
