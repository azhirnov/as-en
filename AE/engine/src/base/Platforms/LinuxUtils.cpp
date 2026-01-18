// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_LINUX
# include "base/Defines/StdInclude.h"

# undef  _GNU_SOURCE
# define _GNU_SOURCE 1
# include <sys/prctl.h>
# include <pthread.h>
# include <sched.h>
# include <signal.h>

#include <sys/utsname.h>

# include "base/Platforms/LinuxUtils.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Algorithms/ToString.h"

namespace AE
{
	void FastCloseApp ()
	{
        raise( SIGABRT );
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
		NonNull( ptr );
		::explicit_bzero( ptr, usize(size) );
	}

#ifndef AE_CFG_RELEASE
/*
=================================================
	SetCurrentThreadName
=================================================
*/
	void  LinuxUtils::SetCurrentThreadName (NtStringView name) __NE___
	{
		StaticLogger::SetCurrentThreadName( StringView{name} );

		ASSERT( name.length() <= 16 );
		int	res = ::prctl( PR_SET_NAME, (unsigned long) name.c_str(), 0, 0, 0 );
		Unused( res );
		ASSERT( res == 0 );
	}

/*
=================================================
	GetCurrentThreadName
=================================================
*/
	String  LinuxUtils::GetCurrentThreadName ()
	{
		char	buf [16];
		int		res = ::prctl( PR_GET_NAME, buf, 0, 0, 0 );
		Unused( res );
		ASSERT( res == 0 );
		return String{buf};
	}
#endif // AE_CFG_RELEASE

/*
=================================================
	GetCurrentThreadHandle
=================================================
*/
	ThreadHandle  LinuxUtils::GetCurrentThreadHandle () __NE___
	{
		return ::pthread_self();
	}

/*
=================================================
	SetThreadAffinity
=================================================
*/
	bool  LinuxUtils::SetThreadAffinity (const ThreadHandle &handle, const uint logicalCoreIdx) __NE___
	{
		ASSERT_Lt( logicalCoreIdx, std::thread::hardware_concurrency() );

		cpu_set_t cpuset;
		CPU_ZERO( &cpuset );
		CPU_SET( logicalCoreIdx, &cpuset );
		return ::pthread_setaffinity_np( handle, sizeof(cpu_set_t), &cpuset ) == 0;
	}

	bool  LinuxUtils::SetCurrentThreadAffinity (const uint logicalCoreIdx) __NE___
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
	bool  LinuxUtils::ResetThreadAffinity (const ThreadHandle &handle) __NE___
	{
		cpu_set_t cpuset;
		CPU_ZERO( &cpuset );

		for (uint i = 0, cnt = std::thread::hardware_concurrency(); i < cnt; ++i)
			CPU_SET( i, &cpuset );

		return ::pthread_setaffinity_np( handle, sizeof(cpu_set_t), &cpuset ) == 0;
	}

	bool  LinuxUtils::ResetCurrentThreadAffinity () __NE___
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
	bool  LinuxUtils::SetThreadPriority (const ThreadHandle &handle, EThreadPriority priority) __NE___
	{
		// TODO:
		//	pthread_setschedprio
		//	https://pubs.opengroup.org/onlinepubs/007904875/functions/xsh_chap02_08.html#tag_02_08_04_01
		Unused( handle, priority );
		return false;
	}

	bool  LinuxUtils::SetCurrentThreadPriority (EThreadPriority priority) __NE___
	{
		// TODO
		Unused( priority );
		return false;
	}

/*
=================================================
	SetProcessAffinity
=================================================
*/
	bool  LinuxUtils::SetProcessAffinity (CpuArchInfo::CoreBits_t coreMask) __NE___
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
	GetLogicalCoreIndex
=================================================
*/
	uint  LinuxUtils::GetLogicalCoreIndex () __NE___
	{
		return ::sched_getcpu();
	}

/*
=================================================
	GetOSName
=================================================
*/
# ifndef AE_RELEASE
	String  LinuxUtils::GetOSName () __NE___
	{
		utsname		os_info = {};
		CHECK_ERR( ::uname( OUT &os_info ) == 0 );

		NOTHROW_ERR(
			String	result;
			result	<< os_info.sysname << ' '
					<< os_info.nodename << ' '
					<< os_info.release << ' '
					<< os_info.version;
			return result;
		)
	}
# endif

/*
=================================================
	GetOSVersion
=================================================
*/
	Version3  LinuxUtils::GetOSVersion () __NE___
	{
		//utsname		os_info = {};
		//CHECK_ERR( ::uname( OUT &os_info ) == 0 );

		// TODO
		return Version3{};
	}

/*
=================================================
	ClipboardExtract
=================================================
*/
	bool  LinuxUtils::ClipboardExtract (OUT U8String &result, void* disp, void* wnd) __NE___
	{
		// TODO: https://stackoverflow.com/questions/27378318/c-get-string-from-clipboard-on-linux
		return false;
	}

/*
=================================================
	ClipboardPut
=================================================
*/
	bool  LinuxUtils::ClipboardPut (NtU8StringView str, void* disp, void* wnd) __NE___
	{
		// TODO
		return false;
	}

/*
=================================================
	GetEnvironmentVariable
=================================================
*/
	bool  LinuxUtils::GetEnvironmentVariable (NtStringView name, OUT String &value) __NE___
	{
		if ( char* ptr = ::getenv( name.c_str() ))
		{
			value = ptr;
			return true;
		}
		return false;
	}

/*
=================================================
	HasEnvironmentVariable
=================================================
*/
	bool  LinuxUtils::HasEnvironmentVariable (NtStringView name) __NE___
	{
		return ::getenv( name.c_str() ) != null;
	}

/*
=================================================
	SetEnvironmentVariable
----
	for current process
=================================================
*/
	bool  LinuxUtils::SetEnvironmentVariable (NtStringView name, NtStringView value) __NE___
	{
		return ::setenv( name.c_str(), value.c_str(), 1 ) == 0;
	}

/*
=================================================
	DeleteEnvironmentVariable
=================================================
*/
	bool  LinuxUtils::DeleteEnvironmentVariable (NtStringView name) __NE___
	{
		return ::unsetenv( name.c_str() ) == 0;
	}

/*
=================================================
	GetExeLocation
=================================================
*/
	Path  LinuxUtils::GetExeLocation () __NE___
	{
		char 	buf [512];
		ssize  	size = ::readlink( "/proc/self/exe", OUT buf, CountOf(buf) );

		if ( size <= 0 )
			return {};

		NOTHROW_ERR( return Path{ StringView( buf, size )};)
	}

/*
=================================================
	SetSystemSleepState
=================================================
*/
	bool  LinuxUtils::SetSystemSleepState (ESystemSleepState) __NE___
	{
		// TODO
		return false;
	}

/*
=================================================
	GetUserName
=================================================
*/
	String  LinuxUtils::GetUserName () __NE___
	{
		char	username [LOGIN_NAME_MAX] = {};
		int		result = ::getlogin_r( OUT username, LOGIN_NAME_MAX );

		CHECK_ERR( result == 0 );
		return String{username};
	}

} // AE::Base

#endif // AE_PLATFORM_LINUX
