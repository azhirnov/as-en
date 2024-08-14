// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"

# include "base/Platforms/WindowsProcess.h"
# include "base/Platforms/WindowsUtils.h"

# include "base/Algorithms/StringUtils.h"
# include "base/Utils/Helpers.h"

namespace AE::Base
{

/*
=================================================
	destructor
=================================================
*/
	WindowsProcess::~WindowsProcess ()
	{
		ASSERT( _thread == null );
		ASSERT( _process == null );
	}

/*
=================================================
	_ExecuteAsync
=================================================
*/
	template <typename T>
	bool  WindowsProcess::_ExecuteAsync (BasicString<T> &commandLine, const Path* currentDir, EFlags flags)
	{
		CHECK_ERR(	_thread == null and
					_process == null );

		using STARTUPINFO_t = Conditional< IsSameTypes< T, char >, STARTUPINFOA, STARTUPINFOW >;

		HANDLE	stdout_read	 = null;
		HANDLE	stdout_write = null;
		HANDLE	stdin_read	 = null;
		HANDLE	stdin_write	 = null;

		ON_DESTROY( [&]() {
				if ( stdin_read )	{ ::CloseHandle( stdin_read );	 stdin_read = null; }
				if ( stdin_write )	{ ::CloseHandle( stdin_write );	 stdin_write = null; }
				if ( stdout_read )	{ ::CloseHandle( stdout_read );	 stdout_read = null; }
				if ( stdout_write )	{ ::CloseHandle( stdout_write ); stdout_write = null; }
			});

		STARTUPINFO_t			startup_info = {};
		PROCESS_INFORMATION		proc_info	 = {};
		const bool				has_dir		 = not (currentDir == null or currentDir->empty());

		startup_info.cb = sizeof(startup_info);

		if ( AllBits( flags, EFlags::ReadOutput ))
		{
			SECURITY_ATTRIBUTES		s_attr = {};
			s_attr.nLength			= sizeof(s_attr);
			s_attr.bInheritHandle	= TRUE;

			CHECK_ERR( ::CreatePipe( OUT &stdout_read, OUT &stdout_write, &s_attr, 0 ) != 0 );	// win2000
			CHECK_ERR( ::SetHandleInformation( stdout_read, HANDLE_FLAG_INHERIT, 0 ) != 0 );	// win2000

			CHECK_ERR( ::CreatePipe( OUT &stdin_read, OUT &stdin_write, &s_attr, 0 ) != 0 );	// win2000
			CHECK_ERR( ::SetHandleInformation( stdin_write, HANDLE_FLAG_INHERIT, 0 ) != 0 );	// win2000

			startup_info.hStdError	= stdout_write;
			startup_info.hStdOutput	= stdout_write;
			startup_info.hStdInput	= stdin_read;
			startup_info.dwFlags	|= STARTF_USESTDHANDLES;
		}

		bool	result = false;

		if constexpr( IsSameTypes< T, char >)
		{
			if ( AnyBits( flags, EFlags::UseCommandPrompt | EFlags::UsePowerShell ))
			{
				char	buf [MAX_PATH] = {};
				::GetSystemDirectoryA( buf, UINT(CountOf(buf)) );	// win2000

				if ( AllBits( flags, EFlags::UseCommandPrompt ))
					commandLine = "\""s << buf << "\\cmd.exe\" /C " << commandLine;
				else
				if ( AllBits( flags, EFlags::UsePowerShell ))
					commandLine = "\""s << buf << "\\WindowsPowerShell\\v1.0\\powershell.exe\" /C " << commandLine;
			}

			result = ::CreateProcessA(		// winxp
				null,
				commandLine.data(),											// command line
				null,														// process security attributes
				null,														// primary thread security attributes
				AllBits( flags, EFlags::ReadOutput ) ? TRUE : FALSE,		// handles are inherited
				AllBits( flags, EFlags::NoWindow ) ? CREATE_NO_WINDOW : 0,	// creation flags
				null,														// use parent's environment
				(has_dir ? ToString(*currentDir).c_str() : null),			// use parent's current directory
				INOUT &startup_info,
				OUT &proc_info
			);
		}
		else
		{
			if ( AnyBits( flags, EFlags::UseCommandPrompt | EFlags::UsePowerShell ))
			{
				wchar_t	buf [MAX_PATH] = {};
				::GetSystemDirectoryW( buf, UINT(CountOf(buf)) );	// win2000

				if ( AllBits( flags, EFlags::UseCommandPrompt ))
					commandLine = L"\""s << buf << L"\\cmd.exe\" /C " << commandLine;
				else
				if ( AllBits( flags, EFlags::UsePowerShell ))
					commandLine = L"\""s << buf << L"\\WindowsPowerShell\\v1.0\\powershell.exe\" /C " << commandLine;
			}

			result = ::CreateProcessW(		// winxp
				null,
				commandLine.data(),											// command line
				null,														// process security attributes
				null,														// primary thread security attributes
				AllBits( flags, EFlags::ReadOutput ) ? TRUE : FALSE,		// handles are inherited
				AllBits( flags, EFlags::NoWindow ) ? CREATE_NO_WINDOW : 0,	// creation flags
				null,														// use parent's environment
				(has_dir ? currentDir->c_str() : null),						// use parent's current directory
				INOUT &startup_info,
				OUT &proc_info
			);
		}

		if ( not result )
		{
			WIN_CHECK_DEV( "CreateProcess failed: " );
			return false;
		}

		_thread			= proc_info.hThread;
		_process		= proc_info.hProcess;
		_flags			= flags;

		_streamOutRead	= stdout_read;
		_streamInWrite	= stdin_write;

		stdout_read		= null;
		stdin_write		= null;

		return true;
	}

/*
=================================================
	Execute
=================================================
*/
	bool  WindowsProcess::Execute (String &commandLine, EFlags flags, milliseconds timeout)
	{
		return Execute( commandLine, Path{}, flags, timeout );
	}

	bool  WindowsProcess::Execute (String &commandLine, INOUT String &output, EFlags flags, milliseconds timeout)
	{
		return Execute( commandLine, Path{}, INOUT output, flags, timeout );
	}

	bool  WindowsProcess::Execute (String &commandLine, const Path &currentDir, EFlags flags, milliseconds timeout)
	{
		WindowsProcess	proc;
		bool			result;

		result = proc._ExecuteAsync( commandLine, &currentDir, flags );
		result = result and proc.WaitAndClose( timeout );

		return result;
	}

	bool  WindowsProcess::Execute (String &commandLine, const Path &currentDir, INOUT String &output, EFlags flags, milliseconds timeout)
	{
		ASSERT( AllBits( flags, EFlags::ReadOutput ));

		WindowsProcess	proc;
		bool			result;

		result = proc._ExecuteAsync( commandLine, &currentDir, flags );
		result = result and proc.WaitAndClose( INOUT output, timeout );

		return result;
	}

/*
=================================================
	ExecuteAsync
=================================================
*/
	bool  WindowsProcess::ExecuteAsync (String &commandLine, EFlags flags)
	{
		return _ExecuteAsync( commandLine, null, flags );
	}

	bool  WindowsProcess::ExecuteAsync (String &commandLine, const Path &currentDir, EFlags flags)
	{
		return _ExecuteAsync( commandLine, &currentDir, flags );
	}

	bool  WindowsProcess::ExecuteAsync (WString &commandLine, EFlags flags)
	{
		return _ExecuteAsync( commandLine, null, flags );
	}

	bool  WindowsProcess::ExecuteAsync (WString &commandLine, const Path &currentDir, EFlags flags)
	{
		return _ExecuteAsync( commandLine, &currentDir, flags );
	}

/*
=================================================
	Terminate
=================================================
*/
	bool  WindowsProcess::Terminate (milliseconds timeout)
	{
		if ( _thread == null or _process == null )
			return true;

		bool	result;

		result = (::TerminateProcess( _process, 1 ) == TRUE);	// winxp
		result = result and WaitAndClose( timeout );

		return result;
	}

/*
=================================================
	WaitAndClose
=================================================
*/
	bool  WindowsProcess::WaitAndClose (const milliseconds inTimeout)
	{
		if ( _thread == null or _process == null )
			return true;

		bool		result	= false;
		const DWORD	timeout	= (inTimeout.count() >= INFINITE ? INFINITE : DWORD(inTimeout.count()));

		if ( ::WaitForSingleObject( _thread, timeout ) == WAIT_OBJECT_0 )	// winxp
		{
			DWORD process_exit;
			::GetExitCodeProcess( _process, OUT &process_exit );	// winxp

			ASSERT( process_exit != STILL_ACTIVE );
			result = true;
		}

		::CloseHandle( _thread );
		::CloseHandle( _process );

		if ( _streamOutRead )
			::CloseHandle( _streamOutRead );

		if ( _streamInWrite != null )
			::CloseHandle( _streamInWrite );

		_thread			= null;
		_process		= null;
		_streamOutRead	= null;
		_streamInWrite	= null;
		_flags			= Default;

		return result;
	}

/*
=================================================
	WaitAndClose
=================================================
*/
	bool  WindowsProcess::WaitAndClose (INOUT String &output, const milliseconds inTimeout)
	{
		if ( _thread == null or _process == null )
			return true;

		bool		result	= false;
		const DWORD	timeout	= (inTimeout.count() >= INFINITE ? INFINITE : DWORD(inTimeout.count()));

		if ( ::WaitForSingleObject( _thread, timeout ) == WAIT_OBJECT_0 )	// winxp
		{
			DWORD process_exit;
			::GetExitCodeProcess( _process, OUT &process_exit );	// winxp

			ASSERT( process_exit != STILL_ACTIVE );
			result = true;
		}

		::CloseHandle( _thread );
		::CloseHandle( _process );

		ASSERT( AllBits( _flags, EFlags::ReadOutput ));

		if ( _streamOutRead != null )
		{
			CHAR	buf [_BufSize];
			for (;;)
			{
				DWORD	readn;
				BOOL	success = ::ReadFile( _streamOutRead, buf, DWORD(CountOf(buf)), OUT &readn, null );

				if ( not success or readn == 0 )
					break;

				output << StringView{ buf, readn };
			}
			::CloseHandle( _streamOutRead );
		}

		if ( _streamInWrite != null )
			::CloseHandle( _streamInWrite );

		_thread			= null;
		_process		= null;
		_streamOutRead	= null;
		_streamInWrite	= null;
		_flags			= Default;

		return result;
	}

/*
=================================================
	ReadOutput
=================================================
*/
	bool  WindowsProcess::ReadOutput (INOUT String &output)
	{
		if ( _streamOutRead == null )
			return false;

		CHAR	buf [_BufSize];

		DWORD	readn;
		BOOL	success = ::ReadFile( _streamOutRead, buf, DWORD(CountOf(buf)), OUT &readn, null );

		if ( not success or readn == 0 )
			return false;

		output << StringView{ buf, readn };
		return true;
	}

/*
=================================================
	IsActive
=================================================
*/
	bool  WindowsProcess::IsActive () C_NE___
	{
		if ( _thread == null or _process == null )
			return false;

		if ( ::WaitForSingleObject( _thread, 0 ) == WAIT_OBJECT_0 )	// winxp
			return false;

		return true;
	}


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
