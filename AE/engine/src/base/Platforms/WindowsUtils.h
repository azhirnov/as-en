// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "base/Containers/NtStringView.h"
# include "base/Utils/Version.h"
# include "base/Utils/Threading.h"
# include "base/Platforms/CPUInfo.h"
# include "base/Platforms/PlatformEnums.h"

namespace AE::Base
{

	//
	// WinAPI Utils
	//

	struct WindowsUtils final
	{
	// types
		struct MemoryPageInfo
		{
			Bytes	pageSize;
			Bytes	allocationGranularity;
		};


	// functions

		// Errors //
		ND_ static uint  GetErrorCode ()													__NE___;
		ND_ static uint  GetNetworkErrorCode ()												__NE___;

			static bool  CheckError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)					__NE___;
			static bool  CheckError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)		__NE___;

			static bool  CheckNetworkError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)			__NE___;
			static bool  CheckNetworkError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error) __NE___;


		// Memory //
		ND_ static MemoryPageInfo	GetMemoryPageInfo ()									__NE___;


		// Thread //
			static void		ThreadPause ()													__NE___;
			static void		ThreadSleep_1us ()												__NE___;
			static void		ThreadSleep_500us ()											__NE___;
			static void		ThreadSleep_15ms ()												__NE___;

			static void		ThreadNanoSleep (nanoseconds relativeTime)						__NE___;
			static bool		ThreadMicroSleep (nanoseconds relativeTime)						__NE___	{ return _MicroSleepImpl( relativeTime, true ); }
			static void		ThreadMilliSleep (milliseconds relativeTime)					__NE___;

			static bool		ThreadWaitIO (milliseconds relativeTime)						__NE___;
		ND_	static bool		SwitchToPendingThread ()										__NE___;

			static bool		GetTimerResolution (OUT nanoseconds &period)					__NE___;

		NdCx__ static auto  NanoSleepTimeStep ()											__NE___	{ return nanoseconds{30}; }
		NdCx__ static auto  MicroSleepTimeStep ()											__NE___	{ return nanoseconds{500'000}; }	// step from docs: 100ns, real step: 500us
		NdCx__ static auto  MilliSleepTimeStep ()											__NE___	{ return nanoseconds{1'000'000'000 / 64}; }	// by default it is 64 context switch per second

		ND_ static ThreadHandle  GetCurrentThreadHandle ()									__NE___;

	  #ifndef AE_CFG_RELEASE
			static void		SetCurrentThreadName (NtStringView name)						__NE___;
		ND_ static String	GetCurrentThreadName ()											__Th___;
	  #endif

			static bool		SetThreadAffinity (const ThreadHandle &, uint logicalCoreIdx)	__NE___;
			static bool		ResetThreadAffinity (const ThreadHandle &)						__NE___;
			static bool		SetThreadPriority (const ThreadHandle &, EThreadPriority)		__NE___;

			static bool		SetCurrentThreadAffinity (uint logicalCoreIdx)					__NE___;
			static bool		ResetCurrentThreadAffinity ()									__NE___;
			static bool		SetCurrentThreadPriority (EThreadPriority priority)				__NE___;

			static bool		SetThreadThrottling (const ThreadHandle &, EThreadPowerThrottling)	__NE___;

		ND_	static uint		GetLogicalCoreIndex ()											__NE___;

		ND_ static Bytes	GetDefaultStackSize ()											__NE___;
		ND_ static Bytes	GetCurrentThreadStackSize ()									__NE___;

		ND_ static bool		SetSystemSleepState (ESystemSleepState)							__NE___;


		// Process //
		ND_ static ulong	GetProcessID ()													__NE___;

		ND_ static Path		GetExeLocation ()												__NE___;

		ND_ static bool		SetProcessAffinity (CpuArchInfo::CoreBits_t)					__NE___;

		ND_ static bool		AddExceptionToFirewall (const Path			&exePath,
													StringView			ruleName,
													Bool				inbound,
													Bool				isTCP,
													ArrayView<ushort>	ports)				__NE___;

		ND_ static bool		IsRunningAsAdmin ()												__NE___;
		ND_ static bool		RunAsAdmin (const Path &path)									__NE___;

		ND_ static bool		OpenURL (U8StringView url)										__NE___;
		ND_ static bool		OpenURL (StringView url)										__NE___;
		ND_ static bool		OpenURL (const Path &url)										__NE___;


		// OS //
		ND_ static Version3		GetOSVersion ()												__NE___;
		ND_ static bool			IsUnderDebugger ()											__NE___;
		ND_ static auto			GetOSType ()												__NE___	{ return EOperationSystem::Windows; }

		#ifdef AE_RELEASE
		ND_ static StringView	GetOSName ()												__NE___	{ return "Windows"; }
		#else
		ND_ static String		GetOSName ()												__NE___;
		#endif

		ND_ static String		GetComputerName ()											__NE___;
		ND_ static String		GetUserName ()												__NE___;

		// env variable
		ND_ static bool		GetEnvironmentVariable (NtStringView name, OUT String &value)	__NE___;
		ND_ static bool		HasEnvironmentVariable (NtStringView name)						__NE___;
		ND_ static bool		SetEnvironmentVariable (NtStringView name, NtStringView value)	__NE___;
		ND_ static bool		DeleteEnvironmentVariable (NtStringView name)					__NE___;

		// registry
		ND_ static bool		HasRegistryKey (NtStringView key)								__NE___;
		ND_ static bool		ReadRegistry (NtStringView key, NtStringView valueName, OUT String &value)	__NE___;


		// Locale //
			static bool		GetLocales (OUT Array<String> &)								__NE___;


		// Clipboard //
		ND_ static bool		ClipboardExtract (OUT U8String &result, void* wnd = null)		__NE___;
		ND_ static bool		ClipboardPut (U8StringView str, void* wnd = null)				__NE___;
		ND_ static bool		ClipboardClear (void* wnd = null)								__NE___;


		// Dialog //
		ND_ static int		Dialog_OkCancel (NtStringView caption, NtStringView msg)		__NE___;


		// Time //
		ND_ static HighResClock::time_point  QueryPerformanceCounterToTimePoint (slong qpc)	__NE___;


	private:
		ND_ static bool  _CheckError (uint err, StringView msg, const SourceLoc &loc, ELogLevel level, ELogScope scope) __NE___;

		template <typename DataType, uint Format>
		ND_ static bool  _ClipboardExtract (OUT DataType &result, void* wnd)						__NE___;
		ND_ static bool  _ClipboardPut (const void* data, Bytes dataSize, uint format, void* wnd)	__NE___;

			static bool  _MicroSleepImpl (nanoseconds relativeTime, bool isWin10v1803)		__NE___;

		ND_ static bool  _OpenURL (const wchar_t* url)										__NE___;

	public:
		ND_ static void*  _GetSystemCpuSetInformationFn ()									__NE___;
		ND_ static void*  _GetDpiForMonitorFn ()											__NE___;
		ND_ static void*  _SetProcessDpiAwarenessFn ()										__NE___;
	};


} // AE::Base


#ifdef AE_DEBUG
#	define WIN_CHECK_DEV( _msg_ ) \
		AE::Base::WindowsUtils::CheckError( (_msg_), AE::Base::SourceLoc::current(), AE::ELogLevel::Debug )

#	define WIN_CHECK_DEV2( _err_, _msg_ ) \
		AE::Base::WindowsUtils::CheckError( (_err_), (_msg_), AE::Base::SourceLoc::current(), AE::ELogLevel::Debug )
#else
#	define WIN_CHECK_DEV( _msg_ )			{}
#	define WIN_CHECK_DEV2( _err_, _msg_ )	{}
#endif

#define WIN_CHECK( _msg_ ) \
	AE::Base::WindowsUtils::CheckError( (_msg_), AE::Base::SourceLoc::current(), AE::ELogLevel::Error )

#define WIN_CHECK2( _err_, _msg_ ) \
	AE::Base::WindowsUtils::CheckError( (_err_), (_msg_), AE::Base::SourceLoc::current(), AE::ELogLevel::Error )


#endif // AE_PLATFORM_WINDOWS
