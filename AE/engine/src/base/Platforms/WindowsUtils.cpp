// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include <signal.h>
# include "base/Platforms/WindowsHeader.cpp.h"
# include <shellapi.h>
# include "base/Platforms/WindowsUtils.h"
# include "base/Platforms/WindowsLibrary.h"
# include "base/Platforms/WindowsProcess.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Algorithms/ToString.h"
# include "base/FileSystem/FileSystem.h"

namespace AE
{
	void FastCloseApp ()
	{
		// close application without calling any destructors
		raise( SIGABRT );

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
		NonNull( ptr );
		::SecureZeroMemory( ptr, usize(size) );	// winxp
	}

/*
=================================================
	GetErrorCode
=================================================
*/
	uint  WindowsUtils::GetErrorCode () __NE___
	{
		return ::GetLastError();
	}

	uint  WindowsUtils::GetNetworkErrorCode () __NE___
	{
		return ::WSAGetLastError();	// winvista
	}

/*
=================================================
	CheckError
=================================================
*/
	bool  WindowsUtils::CheckError (StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		return CheckError( GetErrorCode(), msg, loc, level );
	}

	bool  WindowsUtils::CheckError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		if ( msg.empty() )
			msg = "WinAPI error: ";

		return _CheckError( err, msg, loc, level, ELogScope::System );
	}

/*
=================================================
	GetMemoryPageInfo
=================================================
*/
	WindowsUtils::MemoryPageInfo  WindowsUtils::GetMemoryPageInfo () __NE___
	{
		SYSTEM_INFO		sys_info = {};
		::GetSystemInfo( OUT &sys_info );	// win2000

		MemoryPageInfo	info;
		info.pageSize				= Bytes{ sys_info.dwPageSize };
		info.allocationGranularity	= Bytes{ sys_info.dwAllocationGranularity };
		return info;
	}

/*
=================================================
	CheckNetworkError
=================================================
*/
	bool  WindowsUtils::CheckNetworkError (StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		return CheckNetworkError( GetNetworkErrorCode(), msg, loc, level );
	}

	bool  WindowsUtils::CheckNetworkError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level) __NE___
	{
		if ( msg.empty() )
			msg = "WinAPI network error: ";

		return _CheckError( err, msg, loc, level, ELogScope::Network );
	}

/*
=================================================
	_CheckError
=================================================
*/
	bool  WindowsUtils::_CheckError (const uint err, StringView msg, const SourceLoc &loc, ELogLevel level, ELogScope scope) __NE___
	{
		if_likely( err == SO_ERROR )
			return true;

	  #ifdef AE_ENABLE_LOGS
		TRY{
			char	buf[128] = {};
			DWORD	dw_count = ::FormatMessageA(						// winxp
										FORMAT_MESSAGE_FROM_SYSTEM |
										FORMAT_MESSAGE_IGNORE_INSERTS,
										null,
										err,
										MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
										LPSTR{buf},
										uint(CountOf( buf )),
										null );
			dw_count = (dw_count > 2 ? dw_count-2 : 0);

			String	str {msg};

			if_likely( dw_count > 0 )
				str << StringView{ buf, dw_count };
			else
				str << "code:" << ToString<10>( err );

			AE_PRIVATE_LOGX( level, scope, str, loc );
		}
		CATCH_ALL()
	  #else
		Unused( msg, loc, level, scope );
	  #endif

		return false;
	}
//-----------------------------------------------------------------------------



namespace
{
/*
=================================================
	_WinDynamicLibs
=================================================
*/
	struct _WinDynamicLibs
	{
	// variables
	private:
		WindowsLibrary	_libKernel32;
		WindowsLibrary	_libShcore;

	public:
		decltype(&::GetThreadDescription)			getThreadDescription			= null;		// win10
		decltype(&::SetThreadDescription)			setThreadDescription			= null;		// win10
		decltype(&::GetSystemCpuSetInformation)		getSystemCpuSetInformation		= null;		// win10

		decltype(&::GetDpiForMonitor)				getDpiForMonitor				= null;		// win8.1
		decltype(&::SetProcessDpiAwareness)			setProcessDpiAwareness			= null;		// win8.1
		decltype(&::GetCurrentThreadStackLimits)	getCurrentThreadStackLimits		= null;		// win8


	// methods
	public:
		_WinDynamicLibs () __NE___
		{
			if ( _libKernel32.Load( "kernel32.dll" ))
			{
				Unused( _libKernel32.GetProcAddr( "GetThreadDescription",			OUT getThreadDescription ));
				Unused( _libKernel32.GetProcAddr( "SetThreadDescription",			OUT setThreadDescription ));
				Unused( _libKernel32.GetProcAddr( "GetSystemCpuSetInformation",		OUT getSystemCpuSetInformation ));
				Unused( _libKernel32.GetProcAddr( "GetCurrentThreadStackLimits",	OUT getCurrentThreadStackLimits ));
			}
			if ( _libShcore.Load( "Shcore.dll" ))
			{
				Unused( _libShcore.GetProcAddr( "GetDpiForMonitor",			OUT getDpiForMonitor ));
				Unused( _libShcore.GetProcAddr( "SetProcessDpiAwareness",	OUT setProcessDpiAwareness ));
			}
		}
	};

	ND_ static _WinDynamicLibs&  WinDynamicLibs () __NE___
	{
		static _WinDynamicLibs	lib;
		return lib;
	}


#ifndef AE_CFG_RELEASE
/*
=================================================
	SetCurrentThreadNameXP
=================================================
*/
	static void  SetCurrentThreadNameXP (const char* name) __NE___
	{
		#pragma pack(push,8)
		typedef struct tagTHREADNAME_INFO
		{
			DWORD	dwType;			// Must be 0x1000.
			LPCSTR	szName;			// Pointer to name (in user addr space).
			DWORD	dwThreadID;		// Thread ID (-1=caller thread).
			DWORD	dwFlags;		// Reserved for future use, must be zero.
		 } THREADNAME_INFO;
		#pragma pack(pop)

		constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

		THREADNAME_INFO info;
		info.dwType		= 0x1000;
		info.szName		= name;
		info.dwThreadID	= ::GetCurrentThreadId();
		info.dwFlags	= 0;

	#pragma warning(push)
	#pragma warning(disable: 6320 6322)
		__try{
			::RaiseException( MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), Cast<ULONG_PTR>(&info) );	// winxp
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}
	#pragma warning(pop)
	}

/*
=================================================
	SetCurrentThreadName10
=================================================
*/
	static bool  SetCurrentThreadName10 (NtStringView name) __NE___
	{
		auto&	kernel = WinDynamicLibs();

		if ( kernel.setThreadDescription == null )
			return false;

		WCHAR	str[256] = {};
		for (usize i = 0, cnt = Min( name.size(), CountOf(str)-1 ); i < cnt; ++i) {
			str[i] = WCHAR(name.c_str()[i]);
		}

		HRESULT	hr = kernel.setThreadDescription( ::GetCurrentThread(), str );
		ASSERT( SUCCEEDED(hr) );
		Unused( hr );

		return true;
	}

/*
=================================================
	GetCurrentThreadName10
=================================================
*/
	static bool  GetCurrentThreadName10 (OUT String &name)
	{
		auto&	kernel = WinDynamicLibs();

		if ( kernel.getThreadDescription == null )
			return false;

		PWSTR	w_name;
		HRESULT	hr = kernel.getThreadDescription( ::GetCurrentThread(), OUT &w_name );

		if ( not SUCCEEDED(hr) )
			return false;

		name.reserve( 128 );	// throw

		for (; *w_name; ++w_name) {
			if ( (*w_name >= 0) and (*w_name < 128) )
				name.push_back( char(*w_name) );
		}

		::LocalFree( w_name );  // winxp
		return true;
	}

#endif // AE_CFG_RELEASE

} // namespace
//-----------------------------------------------------------------------------



#ifndef AE_CFG_RELEASE
/*
=================================================
	SetCurrentThreadName
=================================================
*/
	void  WindowsUtils::SetCurrentThreadName (NtStringView name) __NE___
	{
		StaticLogger::SetCurrentThreadName( StringView{name} );

		if ( SetCurrentThreadName10( name ))
			return;

		SetCurrentThreadNameXP( name.c_str() );
	}

/*
=================================================
	GetCurrentThreadName
=================================================
*/
	String  WindowsUtils::GetCurrentThreadName ()
	{
		String	name;
		GetCurrentThreadName10( OUT name );
		return name;
	}

#endif // AE_CFG_RELEASE

/*
=================================================
	GetCurrentThreadHandle
=================================================
*/
	ThreadHandle  WindowsUtils::GetCurrentThreadHandle () __NE___
	{
		return ::GetCurrentThread();  // winxp
	}

/*
=================================================
	SetThreadAffinity
=================================================
*/
	bool  WindowsUtils::SetThreadAffinity (const ThreadHandle &handle, const uint logicalCoreIdx) __NE___
	{
		ASSERT_Lt( logicalCoreIdx, std::thread::hardware_concurrency() );

		DWORD_PTR	mask;

		#if AE_PLATFORM_BITS == 64
		{
			ASSERT( logicalCoreIdx < 64 );
			mask = 1ull << (logicalCoreIdx & 63);
		}
		#elif AE_PLATFORM_BITS == 32
		{
			ASSERT( logicalCoreIdx < 32 );
			mask = 1u << (logicalCoreIdx & 31);
		}
		#endif

		if_likely( ::SetThreadAffinityMask( handle, mask ) != FALSE )	// winxp
			return true;

		Unused( CheckError( "SetThreadAffinityMask ", {}, ELogLevel::Info ));
		return false;

		// TODO
		/*
		GROUP_AFFINITY	affinity = {};
		affinity.Mask	= 1ull << (logicalCoreIdx & 63);
		affinity.Group	= logicalCoreIdx >> 64;

		return ::SetThreadGroupAffinity( handle, &affinity, null ) != FALSE;
		*/
	}

	bool  WindowsUtils::SetCurrentThreadAffinity (uint logicalCoreIdx) __NE___
	{
		return SetThreadAffinity( GetCurrentThreadHandle(), logicalCoreIdx );
	}

/*
=================================================
	ResetThreadAffinity
=================================================
*/
	bool  WindowsUtils::ResetThreadAffinity (const ThreadHandle &handle) __NE___
	{
		DWORD_PTR	mask;

		#if AE_PLATFORM_BITS == 64
		{
			mask = (1ull << (std::thread::hardware_concurrency() & 63)) - 1;
		}
		#elif AE_PLATFORM_BITS == 32
		{
			mask = (1u << (std::thread::hardware_concurrency() & 31)) - 1;
		}
		#endif

		if_likely( ::SetThreadAffinityMask( handle, mask ) != FALSE )	// winxp
			return true;

		Unused( CheckError( "SetThreadAffinityMask ", {}, ELogLevel::Info ));
		return false;
	}

	bool  WindowsUtils::ResetCurrentThreadAffinity () __NE___
	{
		return ResetThreadAffinity( GetCurrentThreadHandle() );
	}

/*
=================================================
	SetThreadPriority
=================================================
*/
	bool  WindowsUtils::SetThreadPriority (const ThreadHandle &handle, EThreadPriority priority) __NE___
	{
		int		i_priority;
		switch_enum( priority )
		{
			case EThreadPriority::PerFrame :		i_priority = THREAD_PRIORITY_HIGHEST;		break;
			case EThreadPriority::PerFrameLow :		i_priority = THREAD_PRIORITY_ABOVE_NORMAL;	break;
			case EThreadPriority::Default :			i_priority = THREAD_PRIORITY_NORMAL;		break;
			case EThreadPriority::Background :		i_priority = THREAD_PRIORITY_BELOW_NORMAL;	break;
			case EThreadPriority::BackgroundLow :	i_priority = THREAD_PRIORITY_LOWEST;		break;
			case EThreadPriority::Highest :			i_priority = THREAD_PRIORITY_TIME_CRITICAL;	break;
			case EThreadPriority::_Count :
			default :								RETURN_ERR( "unknown thread priority" );
		}
		switch_end
		return ::SetThreadPriority( handle, i_priority ) != FALSE;	// winxp
	}

	bool  WindowsUtils::SetCurrentThreadPriority (EThreadPriority priority) __NE___
	{
		return SetThreadPriority( GetCurrentThreadHandle(), priority );
	}

/*
=================================================
	GetLogicalCoreIndex
=================================================
*/
	uint  WindowsUtils::GetLogicalCoreIndex () __NE___
	{
		return ::GetCurrentProcessorNumber();	// winvista
		// TODO: GetCurrentProcessorNumberEx
	}

/*
=================================================
	ThreadWaitIO
=================================================
*/
	bool  WindowsUtils::ThreadWaitIO (milliseconds relativeTime) __NE___
	{
		// If the bAlertable is TRUE and the thread that called this function is the same thread that called
		// the extended I/O function (ReadFileEx or WriteFileEx), the function returns when either the time-out
		// period has elapsed or when an I/O completion callback function occurs. If an I/O completion callback occurs,
		// the I/O completion function is called.
		return ::SleepEx( CheckCast{ relativeTime.count() }, TRUE ) == WAIT_IO_COMPLETION;	// winxp
	}

/*
=================================================
	GetTimerResolution
=================================================
*/
	bool  WindowsUtils::GetTimerResolution (OUT nanoseconds &period) __NE___
	{
		WindowsLibrary	lib;

		if ( lib.Load( "ntdll.dll" ))
		{
			using NtQueryTimerResolution_t = LONG (NTAPI *) (OUT PULONG MinimumResolution, OUT PULONG MaximumResolution, OUT PULONG CurrentResolution);

			NtQueryTimerResolution_t	fn;
			if ( lib.GetProcAddr( "NtQueryTimerResolution", OUT fn ))
			{
				ULONG min, max, cur;
				fn( OUT &min, OUT &max, OUT &cur );

				period = nanoseconds{ cur * 100 };
				return true;
			}
		}
		return false;
	}

/*
=================================================
	ThreadSleep_1us
----
	actual: 500ns
=================================================
*/
	void  WindowsUtils::ThreadSleep_1us () __NE___
	{
		for (uint i = 0; i < 5; ++i)
		{
			YieldProcessor();	// winvista
			YieldProcessor();
			YieldProcessor();
			YieldProcessor();
			YieldProcessor();	// ~100ns
		}
	}

/*
=================================================
	ThreadNanoSleep
=================================================
*/
	void  WindowsUtils::ThreadNanoSleep (nanoseconds relativeTime) __NE___
	{
		ASSERT( relativeTime >= nanoseconds{16} );
		ASSERT( relativeTime <= nanoseconds{512*128} );

		const usize		cnt2 = Min( 512u, usize(relativeTime.count()) / 128 );
		const usize		cnt1 = (usize(relativeTime.count()) % 128 + 16) / 32;

		for (usize i = 0; i < cnt1; ++i)
		{
			YieldProcessor();	// winvista		// ~30ns
		}

		for (usize i = 0; i < cnt2; ++i)
		{
			YieldProcessor();	// winvista
			YieldProcessor();
			YieldProcessor();
			YieldProcessor();
			YieldProcessor();
			YieldProcessor();
			YieldProcessor();
			YieldProcessor();	// ~130 ns
		}
	}

/*
=================================================
	_MicroSleepImpl
----
	step from docs: 100ns
	from tests: 100-1000ns = ~10us, 10-100us = ~0.5ms, >0.5ms = exact +-0.5ms
=================================================
*/
	void  WindowsUtils::ThreadSleep_500us () __NE___
	{
		if_unlikely( not _MicroSleepImpl( nanoseconds{400'000}, true ))
		{
			ThreadNanoSleep( nanoseconds{500'000} );
		}
	}

	bool  WindowsUtils::_MicroSleepImpl (nanoseconds relativeTime, bool isWin10v1803) __NE___
	{
		ASSERT( relativeTime >= nanoseconds{100} );

		// CREATE_WAITABLE_TIMER_HIGH_RESOLUTION requires Windows 10, version 1803
		auto	timer = ::CreateWaitableTimerExA( null, null, (isWin10v1803 ? CREATE_WAITABLE_TIMER_HIGH_RESOLUTION : 0), TIMER_ALL_ACCESS );	// winvista
		if_unlikely( timer == null )
			return false;

		LARGE_INTEGER	li;
		li.QuadPart = - Max( 1u, (relativeTime.count() + 50) / 100 );	// negative for relative time

		if_unlikely( ::SetWaitableTimerEx( timer, &li, 0, null, null, FALSE, 0 ) == FALSE )  // win7
		{
			::CloseHandle( timer );
			return false;
		}

		::WaitForSingleObject( timer, INFINITE );
		::CloseHandle( timer );

		return true;
	}

/*
=================================================
	ThreadSleep_15ms
----
	depends on 'timeBeginPeriod' so set 15ms, by default it will be ~15.6ms
=================================================
*/
	void  WindowsUtils::ThreadSleep_15ms () __NE___
	{
		::Sleep( 14 );
	}

/*
=================================================
	ThreadMilliSleep
=================================================
*/
	void  WindowsUtils::ThreadMilliSleep (milliseconds relativeTime) __NE___
	{
		ASSERT( relativeTime.count() > 0 );
		::Sleep( DWORD(relativeTime.count()) );
	}

/*
=================================================
	SwitchToPendingThread
----
	Causes the calling thread to yield execution to another thread
	that is ready to run on the current processor.
	The operating system selects the next thread to be executed.
	If not switched it takes ~150ns.
=================================================
*/
	bool  WindowsUtils::SwitchToPendingThread () __NE___
	{
		return ::SwitchToThread();	// winxp
	}

/*
=================================================
	ThreadPause
----
	Signals to the processor to give resources to threads that are waiting for them.
	This macro is only effective on processors that support technology allowing
	multiple threads running on a single processor, such as Intel's Hyperthreading technology.
=================================================
*/
	void  WindowsUtils::ThreadPause () __NE___
	{
		// same as '_mm_pause()' on x64, '__builtin_ia32_pause()' for x86
		YieldProcessor();	// winvista
	}

/*
=================================================
	IsUnderDebugger
=================================================
*/
	bool  WindowsUtils::IsUnderDebugger () __NE___
	{
		return ::IsDebuggerPresent() != 0;  // winxp
	}

/*
=================================================
	GetOSVersion
=================================================
*/
	Version3  WindowsUtils::GetOSVersion () __NE___
	{
		if ( ::IsWindows10OrGreater() )			return {10, 0, 0};
		if ( ::IsWindows8Point1OrGreater() )	return { 8, 1, 0};
		if ( ::IsWindows8OrGreater() )			return { 8, 0, 0};
		if ( ::IsWindows7SP1OrGreater() )		return { 7, 1, 0};
		if ( ::IsWindows7OrGreater() )			return { 7, 0, 0};
		//if ( ::IsWindowsVistaSP2OrGreater() )	return { 6, 2, 0};
		//if ( ::IsWindowsVistaSP1OrGreater() )	return { 6, 1, 0};
		//if ( ::IsWindowsVistaOrGreater() )	return { 6, 0, 0};
		//if ( ::IsWindowsXPSP3OrGreater() )	return { 5, 3, 0};
		//if ( ::IsWindowsXPSP2OrGreater() )	return { 5, 2, 0};
		//if ( ::IsWindowsXPSP1OrGreater() )	return { 5, 1, 0};
		//if ( ::IsWindowsXPOrGreater() )		return { 5, 0, 0};
		return Default;
	}

/*
=================================================
	GetOSName
=================================================
*/
#ifndef AE_RELEASE
	String  WindowsUtils::GetOSName () __NE___
	{
		String	name;
		Unused( ReadRegistry( R"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", "ProductName", OUT name ));
		return name;
	}
#endif

/*
=================================================
	ClipboardExtract
=================================================
*/
	template <typename DataType, uint Format>
	bool  WindowsUtils::_ClipboardExtract (OUT DataType &outResult, void* wnd) __NE___
	{
		if_unlikely( ::OpenClipboard( BitCast<HWND>(wnd) ) == FALSE )	// win2000
			return false;

		bool	result	= false;
		HANDLE	data	= ::GetClipboardData( Format );	// win2000

		if_likely( data != null )
		{
			auto*	text = Cast< typename DataType::value_type >( ::GlobalLock( data ));
			if_likely( text != null )
			{
				NOTHROW(
					outResult	= text;  // throw
					result		= true;
				)
				::GlobalUnlock( data );	// winxp
			}
		}
		::CloseClipboard();	// win2000
		return result;
	}

	bool  WindowsUtils::ClipboardExtract (OUT U8String &outResult, void* wnd) __NE___
	{
		WString	tmp;
		if ( not _ClipboardExtract< WString, CF_UNICODETEXT >( OUT tmp, wnd ))
			return false;

		CHECK_ERR( ConvertString<wchar_t>( OUT outResult, tmp ));
		return true;
	}

/*
=================================================
	ClipboardPut
=================================================
*/
	bool  WindowsUtils::_ClipboardPut (const void* data, Bytes dataSize, uint format, void* wnd) __NE___
	{
		CHECK_ERR( data != null );

		HGLOBAL		hmem = ::GlobalAlloc( GMEM_MOVEABLE, SIZE_T(dataSize) ); // winxp
		if_unlikely( hmem == null )
			return false;

		bool	result	= false;
		void*	dst		= ::GlobalLock( hmem );

		if_likely( dst != null )
		{
			std::memcpy( OUT dst, data, size_t(dataSize) );
			::GlobalUnlock( hmem );										// winxp

			if_likely( ::OpenClipboard( BitCast<HWND>(wnd) ) != FALSE )	// win2000
			{
				::EmptyClipboard();										// win2000
				result = (::SetClipboardData( format, hmem ) != null);	// win2000
				::CloseClipboard();										// win2000
			}
		}
		return result;
	}

	bool  WindowsUtils::ClipboardPut (U8StringView str, void* wnd) __NE___
	{
		if_unlikely( str.empty() )
			return ClipboardClear();

		WString		wstr;
		CHECK_ERR( ConvertString( OUT wstr, str ));

		return _ClipboardPut( wstr.c_str(), (wstr.size()+1) * SizeOf<wchar_t>, CF_UNICODETEXT, wnd );
	}

/*
=================================================
	ClipboardClear
=================================================
*/
	bool  WindowsUtils::ClipboardClear (void* wnd) __NE___
	{
		if_likely( ::OpenClipboard( BitCast<HWND>(wnd) ) != FALSE )	// win2000
		{
			::EmptyClipboard();										// win2000
			::CloseClipboard();										// win2000
			return true;
		}
		return false;
	}

/*
=================================================
	GetLocales
=================================================
*/
namespace
{
	static BOOL CALLBACK  EnumUILanguagesProc (LPSTR arg0, LONG_PTR arg1) __NE___
	{
		auto&	arr = *BitCast< Array<String> *>( arg1 );
		NOTHROW( arr.push_back( arg0 ));
		return TRUE;
	}
}
	bool  WindowsUtils::GetLocales (OUT Array<String> &outLocales) __NE___
	{
		return ::EnumUILanguagesA( &EnumUILanguagesProc, MUI_LANGUAGE_NAME, BitCast<LONG_PTR>( &outLocales )) == TRUE;	// win2000
	}

/*
=================================================
	_GetSystemCpuSetInformationFn
=================================================
*/
	void*  WindowsUtils::_GetSystemCpuSetInformationFn () __NE___
	{
		return BitCast<void*>( WinDynamicLibs().getSystemCpuSetInformation );
	}

/*
=================================================
	_GetDpiForMonitorFn
=================================================
*/
	void*  WindowsUtils::_GetDpiForMonitorFn () __NE___
	{
		return BitCast<void*>( WinDynamicLibs().getDpiForMonitor );
	}

/*
=================================================
	_SetProcessDpiAwarenessFn
=================================================
*/
	void*  WindowsUtils::_SetProcessDpiAwarenessFn () __NE___
	{
		return BitCast<void*>( WinDynamicLibs().setProcessDpiAwareness );
	}

/*
=================================================
	GetDefaultStackSize / GetCurrentThreadStackSize
=================================================
*/
	Bytes  WindowsUtils::GetDefaultStackSize () __NE___
	{
		// from https://learn.microsoft.com/en-us/windows/win32/procthread/thread-stack-size
		return 1_MiB;
	}

	Bytes  WindowsUtils::GetCurrentThreadStackSize () __NE___
	{
		auto*	fn = WinDynamicLibs().getCurrentThreadStackLimits;
		if ( fn != null )
		{
			ULONG_PTR	low, high;
			fn( OUT &low, OUT &high );
			return Bytes{low};
		}
		return GetDefaultStackSize();
	}

/*
=================================================
	GetProcessID
=================================================
*/
	ulong  WindowsUtils::GetProcessID () __NE___
	{
		return ::GetCurrentProcessId();
	}

/*
=================================================
	GetEnvironmentVariable
=================================================
*/
	bool  WindowsUtils::GetEnvironmentVariable (NtStringView name, OUT String &value) __NE___
	{
		const DWORD	len = ::GetEnvironmentVariableA( name.c_str(), null, 0 );

		if_unlikely( len == 0 )
		{
			//WIN_CHECK_DEV( "GetEnvironmentVariable" );
			return false;
		}

		NOTHROW_ERR( value.resize( len+2 ));

		const DWORD	err = ::GetEnvironmentVariableA( name.c_str(), OUT value.data(), DWORD(value.size()) );

		value.resize( Min( err, value.size() ));  // no throw

		return err > 0 and err < len;
	}

/*
=================================================
	HasEnvironmentVariable
=================================================
*/
	bool  WindowsUtils::HasEnvironmentVariable (NtStringView name) __NE___
	{
		DWORD	err = ::GetEnvironmentVariableA( name.c_str(), null, 0 );
		return err != 0;
	}

/*
=================================================
	SetEnvironmentVariable
----
	for current process
=================================================
*/
	bool  WindowsUtils::SetEnvironmentVariable (NtStringView name, NtStringView value) __NE___
	{
		CHECK_ERR( not name.empty() );
		CHECK_ERR( not value.empty() );
		CHECK_ERR( value.size() < 32'767 );

		return ::SetEnvironmentVariableA( name.c_str(), value.c_str() ) != 0;
	}

/*
=================================================
	DeleteEnvironmentVariable
=================================================
*/
	bool  WindowsUtils::DeleteEnvironmentVariable (NtStringView name) __NE___
	{
		CHECK_ERR( not name.empty() );
		return ::SetEnvironmentVariableA( name.c_str(), null ) != 0;
	}

/*
=================================================
	GetExeLocation
=================================================
*/
	Path  WindowsUtils::GetExeLocation () __NE___
	{
		wchar_t	buf[MAX_PATH] = {};
		CHECK_ERR( ::GetModuleFileNameW( null, buf, DWORD(CountOf(buf)) ) != FALSE );

		NOTHROW_ERR( return Path{ buf };)
	}

/*
=================================================
	QueryPerformanceCounterToTimePoint
=================================================
*/
	HighResClock::time_point  WindowsUtils::QueryPerformanceCounterToTimePoint (slong qpc) __NE___
	{
		// from '__msvc_chrono.hpp'

		using time_point	= HighResClock::time_point;
		using duration		= HighResClock::duration;
		using period		= HighResClock::period;

		const slong		freq			= _Query_perf_frequency(); // doesn't change after system boot
		constexpr slong TenMHz			= 10'000'000;
		constexpr slong TwentyFourMHz	= 24'000'000;

		// 10 MHz is a very common QPC frequency on modern x86/x64 PCs.
		if ( freq == TenMHz )
		{
			constexpr slong mul = period::den / TenMHz;
			return time_point{ duration{ qpc * mul }};
		}

		// 24 MHz is a common frequency on ARM/ARM64, including cases where it emulates x86/x64.
		if ( freq == TwentyFourMHz )
		{
			const slong	whole = (qpc / TwentyFourMHz) * period::den;
			const slong	part  = (qpc % TwentyFourMHz) * period::den / TwentyFourMHz;
			return time_point{ duration{ whole + part }};
		}

		const slong	whole = (qpc / freq) * period::den;
		const slong	part  = (qpc % freq) * period::den / freq;
		return time_point{ duration{ whole + part }};
	}

/*
=================================================
	HasRegistryKey
=================================================
*/
	bool  WindowsUtils::HasRegistryKey (NtStringView key) __NE___
	{
		HKEY	hkey;
		DWORD	access	= KEY_QUERY_VALUE;
		LSTATUS status	= ::RegOpenKeyExA( HKEY_LOCAL_MACHINE, key.c_str(), 0, access, OUT &hkey );
		bool	ok		= (status == ERROR_SUCCESS);

		::RegCloseKey( hkey );
		return ok;
	}

/*
=================================================
	ReadRegistry
=================================================
*/
	bool  WindowsUtils::ReadRegistry (NtStringView key, NtStringView valueName, OUT String &outValue) __NE___
	{
		outValue.clear();
		CHECK_ERR( not key.empty() and not valueName.empty() );

		HKEY	hkey;
		DWORD	access	= KEY_QUERY_VALUE;
		LSTATUS status	= ::RegOpenKeyExA( HKEY_LOCAL_MACHINE, key.c_str(), 0, access, OUT &hkey );

		if ( status == ERROR_SUCCESS )
		{
			BYTE	buf [1024];
			DWORD	size = sizeof(buf);

			//DWORD	flags = RRF_RT_REG_SZ | REG_EXPAND_SZ | RRF_ZEROONFAILURE;
			//status = ::RegGetValueA( hkey, null, valueName.c_str(), flags, null, OUT buf, INOUT &buf_size );

			status = ::RegQueryValueExA( hkey, valueName.c_str(), null, null, buf, INOUT &size );

			if ( status == ERROR_SUCCESS )
			{
				::RegCloseKey( hkey );
				NOTHROW_ERR( outValue.assign( Cast<char>(buf), size ));
				return true;
			}
		}
		::RegCloseKey( hkey );
		return false;
	}

/*
=================================================
	SetThreadThrottling
=================================================
*/
	bool  WindowsUtils::SetThreadThrottling (const ThreadHandle &handle, EThreadPowerThrottling state) __NE___
	{
		THREAD_POWER_THROTTLING_STATE	throttling_state = {};
		throttling_state.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;

		switch_enum( state )
		{
			case EThreadPowerThrottling::Enable :
				throttling_state.ControlMask	= THREAD_POWER_THROTTLING_EXECUTION_SPEED;
				throttling_state.StateMask		= THREAD_POWER_THROTTLING_EXECUTION_SPEED;
				break;

			case EThreadPowerThrottling::Disable :
				throttling_state.ControlMask	= THREAD_POWER_THROTTLING_EXECUTION_SPEED;
				throttling_state.StateMask		= 0;
				break;

			case EThreadPowerThrottling::Auto :
				throttling_state.ControlMask	= 0;
				throttling_state.StateMask		= 0;
				break;
		}
		switch_end

		return ::SetThreadInformation( handle, ::ThreadPowerThrottling, &throttling_state, sizeof(throttling_state) ) != FALSE;
	}

/*
=================================================
	SetSystemSleepState
=================================================
*/
	bool  WindowsUtils::SetSystemSleepState (ESystemSleepState state) __NE___
	{
		EXECUTION_STATE		flags = ES_CONTINUOUS;

		switch_enum( state )
		{
			case ESystemSleepState::AllowSleep :					break;
			case ESystemSleepState::AllowLowPowerMode :				flags |= ES_AWAYMODE_REQUIRED;	break;
			case ESystemSleepState::DontSleep_AllowTurnDisplayOff :	flags |= ES_SYSTEM_REQUIRED;	break;
			case ESystemSleepState::DisplayAlwaysOn :				flags |= ES_DISPLAY_REQUIRED;	break;
		}
		switch_end

		return ::SetThreadExecutionState( flags ) != 0;	// winxp
	}

/*
=================================================
	GetComputerName
=================================================
*/
	String  WindowsUtils::GetComputerName () __NE___
	{
		char	buf [MAX_COMPUTERNAME_LENGTH+1] = {};
		DWORD	size = sizeof(buf);

		CHECK_ERR( ::GetComputerNameA( OUT buf, INOUT &size ) != 0 );  // win2000
		return String{buf};
	}

/*
=================================================
	GetUserName
=================================================
*/
	String  WindowsUtils::GetUserName () __NE___
	{
		char	buf [128] = {};
		DWORD	size = sizeof(buf);

		CHECK_ERR( ::GetUserNameA( OUT buf, INOUT &size ) != 0 );  // win2000
		return String{buf};
	}

/*
=================================================
	AddExceptionToFirewall
=================================================
*/
	bool  WindowsUtils::AddExceptionToFirewall (const Path &exePath, StringView ruleName, Bool inbound, Bool isTCP, ArrayView<ushort> ports) __NE___
	{
		CHECK_ERR( FileSystem::IsFile( exePath ));
		CHECK_ERR( not ruleName.empty() );

		// TODO: check if rule already exists

		String	cmd;
		cmd << "Start-Process powershell -Verb RunAs -ArgumentList '-NoProfile', '-Command', "
			"'New-NetFirewallRule -DisplayName \""s << ruleName << "\" "
			"-Direction " << (inbound ? "Inbound " : "Outbound ") <<
			"-Program \"" << ToString( exePath ) << "\" "
			"-Protocol " << (isTCP ? "TCP " : "UDP ") <<
			"-Action Allow ";

		if ( not ports.empty() )
		{
			cmd << "-LocalPort ";
			for (ushort port : ports) {
				cmd << ToString( port ) << ',';
			}
			cmd.pop_back();
		}
		cmd << '\'';  // end of 'New-NetFirewallRule'

		return WindowsProcess::Execute( cmd, WindowsProcess::EFlags::UsePowerShell | WindowsProcess::EFlags::NoWindow );
	}

/*
=================================================
	IsRunningAsAdmin
=================================================
*/
	bool  WindowsUtils::IsRunningAsAdmin () __NE___
	{
		BOOL						is_admin		= FALSE;
		PSID						admin_group		= null;
		SID_IDENTIFIER_AUTHORITY	nt_authority	= SECURITY_NT_AUTHORITY;

		if ( ::AllocateAndInitializeSid(
				&nt_authority, 2,
				SECURITY_BUILTIN_DOMAIN_RID,
				DOMAIN_ALIAS_RID_ADMINS,
				0, 0, 0, 0, 0, 0,
				OUT &admin_group ))
		{
			::CheckTokenMembership( null, admin_group, OUT &is_admin );
			::FreeSid( admin_group );
		}
		return is_admin == TRUE;
	}

/*
=================================================
	RunAsAdmin
=================================================
*/
	bool  WindowsUtils::RunAsAdmin (const Path &path) __NE___
	{
		HINSTANCE res = ::ShellExecuteW( null, L"runas", path.native().c_str(), L"", null, SW_SHOWNORMAL );
		// If the function succeeds, it returns a value greater than 32.
		return usize(res) > 32;
	}

/*
=================================================
	Dialog_OkCancel
=================================================
*/
	int  WindowsUtils::Dialog_OkCancel (NtStringView caption, NtStringView msg) __NE___
	{
		UINT	flags = MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST;
				flags |= MB_OKCANCEL | MB_DEFBUTTON1;

		int	result = ::MessageBoxExA( null, msg.c_str(), caption.c_str(), flags, MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ));
		switch ( result )
		{
			case IDCANCEL :	return 1;
			case IDOK :		return 0;
		}
		return -1;
	}

/*
=================================================
	SetProcessAffinity
=================================================
*/
	bool  WindowsUtils::SetProcessAffinity (CpuArchInfo::CoreBits_t coreMask) __NE___
	{
		DWORD_PTR	mask;

		#if AE_PLATFORM_BITS == 64
			mask = coreMask.to_ullong();

		#elif AE_PLATFORM_BITS == 32
			mask = coreMask.to_ulong();
		#endif

		if_likely( ::SetProcessAffinityMask( ::GetCurrentProcess(), mask ) != 0 )	// winxp
			return true;

		Unused( CheckError( "SetProcessAffinityMask ", {}, ELogLevel::Info ));
		return false;
	}

/*
=================================================
	OpenURL
=================================================
*/
	bool  WindowsUtils::_OpenURL (const wchar_t* url) __NE___
	{
		SHELLEXECUTEINFOW	info = {};
		info.cbSize		= sizeof(info);
		info.lpVerb		= L"open";
		info.lpFile		= url;
		info.nShow		= SW_SHOWNORMAL;

		return ::ShellExecuteExW( &info ) != FALSE;
	}

	bool  WindowsUtils::OpenURL (U8StringView url) __NE___
	{
		WString		path;
		CHECK_ERR( ConvertString( OUT path, url ));

		return _OpenURL( path.c_str() );
	}

	bool  WindowsUtils::OpenURL (StringView url) __NE___
	{
		WString		path;
		CHECK_ERR( ConvertString( OUT path, url ));

		return _OpenURL( path.c_str() );
	}

	bool  WindowsUtils::OpenURL (const Path &url) __NE___
	{
		return _OpenURL( url.native().c_str() );
	}

/*
=================================================
	ProtectCurrentProcess
=================================================
*
	void  WindowsUtils::ProtectCurrentProcess () __NE___
	{
		// TODO
		//	https://www.ired.team/offensive-security/defense-evasion/preventing-3rd-party-dlls-from-injecting-into-your-processes
		//	https://www.ired.team/offensive-security/defense-evasion/acg-arbitrary-code-guard-processdynamiccodepolicy
	}
*/

} // AE::Base

#endif // AE_PLATFORM_WINDOWS
