// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_PLATFORM_ANDROID
# include "base/Defines/StdInclude.h"

# undef  _GNU_SOURCE
# define _GNU_SOURCE 1
# include <sys/prctl.h>
# include <pthread.h>
# include <sched.h>
# include <sys/system_properties.h>
# include <arm_acle.h>

# include "base/Platforms/AndroidUtils.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Algorithms/ToString.h"

namespace AE
{
	void FastCloseApp ()
	{
		std::terminate();
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
	#if __ANDROID_API__ >= 29
		if_likely( ptr != null )
			::explicit_bzero( ptr, usize{size} );

	#elif defined(__STDC_LIB_EXT1__)
		if_likely( ptr != null )
			memset_s( ptr, usize{size}, 0, usize{size} );

	#else
		return Base::_hidden_::SecureZeroMemFallback( OUT ptr, size );
	#endif
	}

#ifndef AE_CFG_RELEASE
/*
=================================================
	SetCurrentThreadName
=================================================
*/
	void  AndroidUtils::SetCurrentThreadName (NtStringView name) __NE___
	{
		StaticLogger::SetCurrentThreadName( StringView{name} );

		ASSERT( name.length() <= 16 );

	  #if 1
		int	res = ::prctl( PR_SET_NAME, (unsigned long) name.c_str(), 0, 0, 0 );
	  #else
		int	res = ::pthread_setname_np( ::pthread_self(), name.c_str() );
	  #endif

		ASSERT( res == 0 );  Unused( res );
	}

/*
=================================================
	GetCurrentThreadName
=================================================
*/
	String  AndroidUtils::GetCurrentThreadName ()
	{
		char	buf [16];
	  #if 1
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
	ThreadHandle  AndroidUtils::GetCurrentThreadHandle () __NE___
	{
		return ::pthread_self();
	}

/*
=================================================
	SetThreadAffinity
=================================================
*/
	bool  AndroidUtils::SetThreadAffinity (const ThreadHandle &handle, uint logicalCoreIdx) __NE___
	{
		CHECK_ERR( handle == GetCurrentThreadHandle() );
		return SetCurrentThreadAffinity( logicalCoreIdx );
	}

	bool  AndroidUtils::SetCurrentThreadAffinity (const uint logicalCoreIdx) __NE___
	{
		ASSERT_Lt( logicalCoreIdx, std::thread::hardware_concurrency() );

		::cpu_set_t  mask;
		CPU_ZERO( OUT &mask );
		CPU_SET( logicalCoreIdx, INOUT &mask );

		return ::sched_setaffinity( 0, sizeof(mask), &mask ) == 0;
	}

/*
=================================================
	ResetThreadAffinity
=================================================
*/
	bool  AndroidUtils::ResetThreadAffinity (const ThreadHandle &handle) __NE___
	{
		CHECK_ERR( handle == GetCurrentThreadHandle() );
		return ResetCurrentThreadAffinity();
	}

	bool  AndroidUtils::ResetCurrentThreadAffinity () __NE___
	{
		::cpu_set_t  mask;
		CPU_ZERO( OUT &mask );

		for (uint i = 0, cnt = std::thread::hardware_concurrency(); i < cnt; ++i)
			CPU_SET( i, INOUT &mask );

		return ::sched_setaffinity( 0, sizeof(mask), &mask ) == 0;
	}

/*
=================================================
	SetThreadPriority
=================================================
*/
	bool  AndroidUtils::SetThreadPriority (const ThreadHandle &handle, EThreadPriority priority) __NE___
	{
		// TODO
		Unused( handle, priority );
		return false;
	}

	bool  AndroidUtils::SetCurrentThreadPriority (EThreadPriority priority) __NE___
	{
		// TODO
		Unused( priority );
		return false;
	}

/*
=================================================
	GetLogicalCoreIndex
=================================================
*/
	uint  AndroidUtils::GetLogicalCoreIndex () __NE___
	{
		return ::sched_getcpu();
	}

/*
=================================================
	GetSDKVersion
=================================================
*/
	uint  AndroidUtils::GetSDKVersion () __NE___
	{
	#if 0
		char	sdk_ver_str [92];
		if ( __system_property_get( "ro.build.version.sdk", OUT sdk_ver_str )) {
			return std::atoi( sdk_ver_str );
		}
		return 0;
	#else
		return android_get_device_api_level();
	#endif
	}

/*
=================================================
	GetTargetSDKVersion
=================================================
*/
	uint  AndroidUtils::GetTargetSDKVersion () __NE___
	{
		ASSERT( GetSDKVersion() >= 24 );
		return android_get_application_target_sdk_version();
	}

/*
=================================================
	GetOSVersion
=================================================
*/
	Version3  AndroidUtils::GetOSVersion () __NE___
	{
	#if 0
		char	sdk_ver_str [92];
		if ( __system_property_get( "ro.build.version.release", OUT sdk_ver_str ))
		{
			StringView	str {sdk_ver_str};
			usize		pos = str.find( '.' );
			return Version3{ uint(std::atoi(str.data())), uint(std::atoi(str.data() + pos)), 0 };
		}
		return Default;
	#else
		switch ( android_get_device_api_level() )
		{
			case 35 :	return Version3{ 15, 0, 0 };
			case 34 :	return Version3{ 14, 0, 0 };
			case 33 :	return Version3{ 13, 0, 0 };
			case 32 :
			case 31 :	return Version3{ 12, 0, 0 };
			case 30 :	return Version3{ 11, 0, 0 };
			case 29 :	return Version3{ 10, 0, 0 };
			case 28 :	return Version3{  9, 0, 0 };
			case 27 :	return Version3{  8, 1, 0 };
			case 26 :	return Version3{  8, 0, 0 };
			case 25 :	return Version3{  7, 1, 0 };
			case 24 :	return Version3{  7, 0, 0 };
		}
		return Default;
	#endif
	}

/*
=================================================
	IsUnderDebugger
----
	https://developer.android.com/reference/android/os/Debug.html#isDebuggerConnected()
=================================================
*/
	static bool  Android_IsUnderDebugger = false;

	extern "C" void AE_DLL_EXPORT Android_SetIsUnderDebugger (bool value)
	{
		Android_IsUnderDebugger = value;
	}

	bool  AndroidUtils::IsUnderDebugger () __NE___
	{
	  #if 0 //AE_CXX_VER >= 26
		return std::is_debugger_present();

	  #elif defined(AE_CFG_RELEASE)
		return false;
	  #else
		return Android_IsUnderDebugger;
	  #endif
	}

/*
=================================================
	SetProcessAffinity
=================================================
*/
	bool  AndroidUtils::SetProcessAffinity (CpuArchInfo::CoreBits_t coreMask) __NE___
	{
		pid_t	pid = ::getpid();

		::cpu_set_t  mask;
		CPU_ZERO( OUT &mask );

		StaticAssert( sizeof(mask.__bits[0]) == 8 );
		mask.__bits[0] = coreMask.to_ullong();

		return ::sched_setaffinity( pid, sizeof(mask), &mask ) == 0;
	}

/*
=================================================
	_SetActivityCallbacks
=================================================
*/
	namespace {
		static SharedMutex							s_ActivityCallbacksGuard;
		static AndroidUtils::ActivityCallbacks		s_ActivityCallbacks;
	}

	void  AndroidUtils::_SetActivityCallbacks (const ActivityCallbacks &cb) __NE___
	{
		EXLOCK( s_ActivityCallbacksGuard );
		s_ActivityCallbacks = cb;
	}

/*
=================================================
	ClipboardExtract
=================================================
*/
	bool  AndroidUtils::ClipboardExtract (OUT U8String &result, void* wnd) __NE___
	{
		ASSERT( wnd == null );
		Unused( wnd );

		EXLOCK( s_ActivityCallbacksGuard );

		CHECK_ERR( s_ActivityCallbacks.clipboardExtract != null );
		return s_ActivityCallbacks.clipboardExtract( s_ActivityCallbacks.userData, OUT result );
	}

/*
=================================================
	ClipboardPut
=================================================
*/
	bool  AndroidUtils::ClipboardPut (U8StringView str, void* wnd) __NE___
	{
		ASSERT( wnd == null );
		Unused( wnd );

		EXLOCK( s_ActivityCallbacksGuard );

		CHECK_ERR( s_ActivityCallbacks.clipboardPut != null );
		return s_ActivityCallbacks.clipboardPut( s_ActivityCallbacks.userData, str );
	}

/*
=================================================
	ClipboardClear
=================================================
*/
	bool  AndroidUtils::ClipboardClear (void* wnd) __NE___
	{
		ASSERT( wnd == null );
		Unused( wnd );

		EXLOCK( s_ActivityCallbacksGuard );

		CHECK_ERR( s_ActivityCallbacks.clipboardClear != null );
		return s_ActivityCallbacks.clipboardClear( s_ActivityCallbacks.userData );
	}

/*
=================================================
	OpenURL
=================================================
*/
	bool  AndroidUtils::OpenURL (U8StringView url) __NE___
	{
		EXLOCK( s_ActivityCallbacksGuard );
		CHECK_ERR( s_ActivityCallbacks.openURL != null );
		return s_ActivityCallbacks.openURL( s_ActivityCallbacks.userData, url );
	}

	bool  AndroidUtils::OpenURL (StringView url) __NE___
	{
		return OpenURL( U8StringView{ Cast<CharUtf8>(url.data()), url.size() });
	}

	bool  AndroidUtils::OpenURL (const Path &url) __NE___
	{
		return OpenURL( StringView{ url.native() });
	}

/*
=================================================
	SetSystemSleepState
=================================================
*/
	bool  AndroidUtils::SetSystemSleepState (ESystemSleepState state) __NE___
	{
	//	EXLOCK( s_ActivityCallbacksGuard );
	//	CHECK_ERR( s_ActivityCallbacks.setSystemSleepState != null );
	//	return s_ActivityCallbacks.setSystemSleepState( s_ActivityCallbacks.userData, state );
		return true;
	}

} // AE::Base

#endif // AE_PLATFORM_ANDROID
