// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS)

# include "base/Defines/StdInclude.h"

# undef  _GNU_SOURCE
# define _GNU_SOURCE 1
# include <pthread.h>
# include <sched.h>

# include <mach/thread_policy.h>
# include <mach/thread_act.h>

# include "base/Platforms/AppleUtils.h"
# include "base/Algorithms/ToString.h"

namespace AE
{
	void FastCloseApp ()
	{
		std::abort();
	}
}

namespace AE::Base
{
/*
=================================================
	SecureZeroMem
=================================================
*/
	void  SecureZeroMem (OUT void* ptr, Bytes size) __NE___
	{
		return Base::_hidden_::SecureZeroMemFallback( OUT ptr, size );
	}

#ifndef AE_CFG_RELEASE
/*
=================================================
	SetCurrentThreadName
=================================================
*/
	void  AppleUtils::SetCurrentThreadName (NtStringView name) __NE___
	{
		StaticLogger::SetCurrentThreadName( StringView{name} );

		ASSERT( name.length() <= 16 );

	  #if 0
		int	res = ::prctl( PR_SET_NAME, (unsigned long) name.c_str(), 0, 0, 0 );
	  #elif 1
		int	res = ::pthread_setname_np( name.c_str() );
	  #else
		::thread_set_thread_name( ::current_thread(), name.c_str() );
		int res = 0;
	  #endif

		ASSERT( res == 0 );  Unused( res );
	}

/*
=================================================
	GetCurrentThreadName
=================================================
*/
	String  AppleUtils::GetCurrentThreadName ()
	{
		char	buf [16] = {};
	  #if 0
		int res = ::prctl( PR_GET_NAME, OUT buf, 0, 0, 0 );
	  #else
		int res = ::pthread_getname_np( ::pthread_self(), OUT buf, 16 );
	  #endif
		ASSERT( res == 0 );  Unused( res );
		return String{buf};
	}
#endif // AE_CFG_RELEASE

/*
=================================================
	GetCurrentThreadHandle
=================================================
*/
	ThreadHandle  AppleUtils::GetCurrentThreadHandle () __NE___
	{
		return ::pthread_self();
	}

/*
=================================================
	SetThreadAffinity
----
	doesn't work on ARM!
	https://developer.apple.com/library/archive/releasenotes/Performance/RN-AffinityAPI/index.html#//apple_ref/doc/uid/TP40006635
=================================================
*/
	bool  AppleUtils::SetThreadAffinity (const ThreadHandle &handle, const uint logicalCoreIdx) __NE___
	{
	#ifdef AE_CPU_ARCH_ARM_BASED
		RETURN_ERR( "use SetCurrentThreadAffinity instead" );
	
	#else
		ASSERT_Lt( logicalCoreIdx, std::thread::hardware_concurrency() );

		thread_affinity_policy	ap = {};
		ap.affinity_tag = 1 << logicalCoreIdx;

		auto td = ::pthread_mach_thread_np( handle );
		int res = ::thread_policy_set( td, THREAD_AFFINITY_POLICY, BitCast<thread_policy_t>(&ap), THREAD_AFFINITY_POLICY_COUNT );
		ASSERT( res == 0 );  Unused( res );

		return true;
	#endif
	}

	bool  AppleUtils::SetCurrentThreadAffinity (const uint logicalCoreIdx) __NE___
	{
	#ifdef AE_CPU_ARCH_ARM_BASED
		const auto&		cpu_info = Base::CpuArchInfo::Get();
		
		if ( auto* core = cpu_info.GetCore( logicalCoreIdx ))
		{
			EThreadPriority		priority = EThreadPriority::Default;
			switch_enum( core->type )
			{
				case ECoreType::HighPerformance :	priority = EThreadPriority::PerFrame;
				case ECoreType::Performance :		priority = EThreadPriority::PerFrameLow;
				case ECoreType::EnergyEfficient :	priority = EThreadPriority::Background;
				case ECoreType::LowPower :			priority = EThreadPriority::BackgroundLow;
				case ECoreType::_Count :
				case ECoreType::Unknown	: 			break;
			}
			switch_end
			return SetCurrentThreadPriority( priority );
		}
		return false;
	#else
		return SetThreadAffinity( GetCurrentThreadHandle(), logicalCoreIdx );
	#endif
	}

/*
=================================================
	ResetThreadAffinity
=================================================
*/
	bool  AppleUtils::ResetThreadAffinity (const ThreadHandle &) __NE___
	{
		return false;
	}

	bool  AppleUtils::ResetCurrentThreadAffinity () __NE___
	{
		return false;
	}

/*
=================================================
	SetThreadPriority
----
	https://developer.apple.com/documentation/apple-silicon/tuning-your-code-s-performance-for-apple-silicon
	https://developer.apple.com/videos/play/tech-talks/110147/	time: 25:11
=================================================
*/
	bool  AppleUtils::SetThreadPriority (const ThreadHandle &, EThreadPriority) __NE___
	{
		RETURN_ERR( "use SetCurrentThreadPriority instead" );
	}

	bool  AppleUtils::SetCurrentThreadPriority (EThreadPriority priority) __NE___
	{
		qos_class_t	qos;
		switch_enum( priority )
		{
			case EThreadPriority::PerFrame :		qos = QOS_CLASS_USER_INTERACTIVE;	break;	// per-frame work
			case EThreadPriority::PerFrameLow :		qos = QOS_CLASS_USER_INITIATED;		break;	// cross-frame work
			case EThreadPriority::Default :			qos = QOS_CLASS_DEFAULT;			break;	// streaming / multiple frames deadline
			case EThreadPriority::Background :		qos = QOS_CLASS_UTILITY;			break;	// background asset download
			case EThreadPriority::BackgroundLow :	qos = QOS_CLASS_BACKGROUND;			break;	// may not run for a very long time, only E-core

			case EThreadPriority::Highest :
			{
				auto	td = ::pthread_self();
				struct sched_param	params;
				params.sched_priority = ::sched_get_priority_max( SCHED_RR );
				return ::pthread_setschedparam( td, SCHED_RR, &params ) == 0;
			}

			case EThreadPriority::_Count :
			default :								RETURN_ERR( "unknown thread priority" );
		}
		switch_end

		::pthread_set_qos_class_self_np( qos, 0 );
		return true;
	}

/*
=================================================
	GetLogicalCoreIndex
=================================================
*/
	uint  AppleUtils::GetLogicalCoreIndex () __NE___
	{
		return 0;
		//return ::sched_getcpu();
	}
	
/*
=================================================
	SetSystemSleepState
=================================================
*/
	bool  AppleUtils::SetSystemSleepState (ESystemSleepState) __NE___
	{
		// TODO
		return false;
	}


} // AE::Base

#endif // AE_PLATFORM_MACOS or AE_PLATFORM_IOS
