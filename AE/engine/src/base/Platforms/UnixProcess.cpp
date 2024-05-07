// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/UnixProcess.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Algorithms/StringUtils.h"

#ifdef AE_PLATFORM_UNIX_BASED

namespace AE::Base
{
/*
=================================================
	destructor
=================================================
*/
	UnixProcess::~UnixProcess ()
	{
		ASSERT( _file == null );
	}

/*
=================================================
	ExecuteAsync
=================================================
*/
	bool  UnixProcess::ExecuteAsync (StringView commandLine, EFlags flags)
	{
		return _ExecuteAsync( commandLine, null, flags );
	}

	bool  UnixProcess::ExecuteAsync (StringView commandLine, const Path &currentDir, EFlags flags)
	{
		return _ExecuteAsync( commandLine, &currentDir, flags );
	}

/*
=================================================
	_ExecuteAsync
=================================================
*/
	bool  UnixProcess::_ExecuteAsync (StringView commandLine, const Path* currentDir, EFlags flags)
	{
		CHECK_ERR( _file == null );

		String	cmd;
		if ( currentDir != null and not currentDir->empty() )
			cmd << "cd \"" << currentDir->string() << "\" && ";

		cmd << commandLine;
		cmd << " 2>&1";

		_file = ::popen( cmd.c_str(), "r" );
		CHECK_ERR( _file != null );

		return true;
	}

/*
=================================================
	WaitAndClose
=================================================
*/
	bool  UnixProcess::WaitAndClose (milliseconds timeout)
	{
		Unused( timeout );

		if ( _file == null )
			return true;

		FILE*	f = _file;
		_file = null;

		CHECK_ERR( ::pclose( f ) != -1 );
		return true;
	}

	bool  UnixProcess::WaitAndClose (INOUT String &output, milliseconds timeout)
	{
		Unused( timeout );

		if ( _file == null )
			return true;

		char	buf [_BufSize];
		while ( ::fgets( OUT buf, int(CountOf(buf)), _file ) != null )
		{
			output << buf;
		}

		FILE*	f = _file;
		_file = null;

		CHECK_ERR( ::pclose( f ) != -1 );
		return true;
	}

/*
=================================================
	IsActive
=================================================
*/
	bool  UnixProcess::IsActive () C_NE___
	{
		if ( _file == null )
			return false;

		return ::feof( _file ) == 0;
	}

/*
=================================================
	Terminate
=================================================
*/
	bool  UnixProcess::Terminate (milliseconds timeout)
	{
		// TODO
		return WaitAndClose( timeout );
	}

/*
=================================================
	ReadOutput
=================================================
*/
	bool  UnixProcess::ReadOutput (INOUT String &output)
	{
		if ( _file == null )
			return false;

		char	buf [_BufSize];
		bool	success	= ::fgets( OUT buf, int(CountOf(buf)), _file ) != null;

		if ( not success )
			return false;

		output << buf;
		return true;
	}

/*
=================================================
	Execute
=================================================
*/
	bool  UnixProcess::Execute (StringView commandLine, EFlags flags, milliseconds timeout)
	{
		return Execute( commandLine, Path{}, flags, timeout );
	}

	bool  UnixProcess::Execute (StringView commandLine, INOUT String &output, EFlags flags, milliseconds timeout)
	{
		return Execute( commandLine, Path{}, INOUT output, flags, timeout );
	}

	bool  UnixProcess::Execute (StringView commandLine, const Path &currentDir, EFlags flags, milliseconds timeout)
	{
		UnixProcess		proc;
		bool			result;

		result = proc._ExecuteAsync( commandLine, &currentDir, flags );
		result = result and proc.WaitAndClose( timeout );

		return result;
	}

	bool  UnixProcess::Execute (StringView commandLine, const Path &currentDir, INOUT String &output, EFlags flags, milliseconds timeout)
	{
		ASSERT( AllBits( flags, EFlags::ReadOutput ));

		UnixProcess		proc;
		bool			result;

		result = proc._ExecuteAsync( commandLine, &currentDir, flags );
		result = result and proc.WaitAndClose( INOUT output, timeout );

		return result;
	}


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
