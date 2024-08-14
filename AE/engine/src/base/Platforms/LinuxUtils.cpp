// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_LINUX
# include "base/Defines/StdInclude.h"

# undef  _GNU_SOURCE
# define _GNU_SOURCE 1
# include <sys/prctl.h>
# include <pthread.h>
# include <sched.h>

#include <sys/utsname.h>

# include "base/Platforms/LinuxUtils.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

/*
=================================================
	SecureZeroMem
=================================================
*/
	void  SecureZeroMem (OUT void* ptr, Bytes size) __NE___
	{
		::explicit_bzero( ptr, usize(size) );
	}

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
	bool  LinuxUtils::SetThreadAffinity (const ThreadHandle &handle, uint coreIdx) __NE___
	{
		ASSERT_Lt( coreIdx, std::thread::hardware_concurrency() );

		cpu_set_t cpuset;
		CPU_ZERO( &cpuset );
		CPU_SET( coreIdx, &cpuset );
		return ::pthread_setaffinity_np( handle, sizeof(cpu_set_t), &cpuset ) == 0;
	}

	bool  LinuxUtils::SetCurrentThreadAffinity (uint coreIdx) __NE___
	{
		ASSERT_Lt( coreIdx, std::thread::hardware_concurrency() );

		::cpu_set_t  mask;
		CPU_ZERO( OUT &mask );
		CPU_SET( coreIdx, INOUT &mask );

		return ::sched_setaffinity( 0, sizeof(mask), &mask ) == 0;
	}

/*
=================================================
	SetThreadPriority
=================================================
*/
	bool  LinuxUtils::SetThreadPriority (const ThreadHandle &handle, float priority) __NE___
	{
		// TODO:
		//	pthread_setschedprio
		//	https://pubs.opengroup.org/onlinepubs/007904875/functions/xsh_chap02_08.html#tag_02_08_04_01
		Unused( handle, priority );
		return false;
	}

	bool  LinuxUtils::SetCurrentThreadPriority (float priority) __NE___
	{
		// TODO
		Unused( priority );
		return false;
	}

/*
=================================================
	GetProcessorCoreIndex
=================================================
*/
	uint  LinuxUtils::GetProcessorCoreIndex () __NE___
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
	bool  LinuxUtils::ClipboardExtract (OUT String &result, void* disp, void* wnd) __NE___
	{
		// TODO: https://stackoverflow.com/questions/27378318/c-get-string-from-clipboard-on-linux
		return false;
	}

/*
=================================================
	ClipboardPut
=================================================
*/
	bool  LinuxUtils::ClipboardPut (StringView str, void* disp, void* wnd) __NE___
	{
		// TODO
		return false;
	}


} // AE::Base

#endif // AE_PLATFORM_LINUX
