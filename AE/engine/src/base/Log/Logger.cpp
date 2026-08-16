// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
#	include <android/log.h>
#else
#	include <iostream>
#endif
#ifdef AE_PLATFORM_EMSCRIPTEN
#	include <emscripten.h>
#endif
#ifdef AE_COMPILER_MSVC
#	include "base/Platforms/WindowsHeader.cpp.h"
#endif

#include "base/Log/Logger.h"
#include "base/Algorithms/ToString.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/Parser.h"
#include "base/DataSource/File.h"
#include "base/Platforms/ThreadUtils.h"
#include "base/FileSystem/FileSystem.h"
#include "base/Platforms/Platform.h"
#include "base/Debug/StackTrace.h"


#ifdef AE_PLATFORM_EMSCRIPTEN
namespace AE
{
	void _ems_debugbreak ()
	{
		emscripten_run_script( "alert('breakpoint')" );
	}
}
#endif


namespace AE::Base
{
namespace
{
/*
=================================================
	ScopeToString
=================================================
*/
	ND_ static String  ScopeToString (ELogScope scope) __Th___
	{
		String	str{ToString( scope )};
		if ( not str.empty() )
			str << ' ';
		return str;
	}

/*
=================================================
	LevelToChar
=================================================
*/
	ND_ static char  LevelToChar (ELogLevel level) __NE___
	{
		switch_enum( level )
		{
			case ELogLevel::Debug :			return 'D';		break;
			case ELogLevel::Info :			return 'I';		break;
			case ELogLevel::Warning :		return 'W';		break;
			case ELogLevel::Error :			return 'E';		break;
			case ELogLevel::Fatal :			return 'F';		break;

			case ELogLevel::_Count :
			default :						return ' ';
		}
		switch_end
	}

/*
=================================================
	MinimizeThreadID
=================================================
*/
	Nd__In usize  MinimizeThreadID (usize id) __NE___
	{
		if constexpr( sizeof(id) > 4 ) {
			id ^= (id >> 32);
		}
		id = ((id >> 16) ^ id) & 0xFFFF;
		return id;
	}

/*
=================================================
	GetCallStack
=================================================
*/
	ND_ String  GetCallStack () __Th___
	{
		#ifdef AE_PLATFORM_WINDOWS
			constexpr StringView	fname = "base\\Log\\Log.cpp";
		#else
			constexpr StringView	fname = "base/Log/Log.cpp";
		#endif

		return StackTrace::ToString( fname );
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	VisualStudioLogOutput
=================================================
*/
#ifdef AE_COMPILER_MSVC
	ILogger::EResult  VisualStudioLogOutput::Process (const MessageInfo &info)
	{
		const String	str = String{info.loc.FileName()} << '(' << ToString( info.loc.Line() ) << "): "
							<< ScopeToString( info.scope ) << ToString( info.level )
							<< ": " << info.message << '\n';

		::OutputDebugStringA( str.c_str() );	// thread safe

		return EResult::Unknown;
	}
#endif
/*
=================================================
	VSCodeLogOutput
=================================================
*/
	ILogger::EResult  VSCodeLogOutput::Process (const MessageInfo &info)
	{
		const StringView	c_Reset		= "\033[0m";
		const StringView	c_Red		= "\033[91m";
		const StringView	c_Orange	= "\033[33m";

		String	str;
		switch ( info.level )
		{
			case ELevel::Warning :	str << c_Orange;	break;
			case ELevel::Error :
			case ELevel::Fatal :	str << c_Red; 		break;
		}

		str << info.loc.FileName() << ':' << ToString( info.loc.Line() ) << ": "
			<< ScopeToString( info.scope ) << ToString( info.level )
			<< ": in " << info.loc.FunctionName() << ": " << info.message;

		if ( info.level >= ELevel::Warning )
			str << c_Reset;

		{
			EXLOCK( _guard );
			std::cout << str << std::endl;
		}
		return EResult::Unknown;
	}

/*
=================================================
	CreateIDEOutput
=================================================
*/
	ILogger::LoggerPtr  ILogger::CreateIDEOutput () __NE___
	{
	#ifdef AE_CI_BUILD_TEST
		return {};
	#elif defined(AE_COMPILER_MSVC)
		if ( ::IsDebuggerPresent() )
			return MakeUnique<VisualStudioLogOutput>();
		else
			return {};
	#elif defined(AE_PLATFORM_LINUX)
		return MakeUnique<VSCodeLogOutput>();
	#else
		return {};
	#endif
	}
//-----------------------------------------------------------------------------


/*
=================================================
	DialogLogOutput
=================================================
*/
#if defined(AE_PLATFORM_WINDOWS)	or \
	defined(AE_PLATFORM_APPLE)		or \
	defined(AE_PLATFORM_LINUX)

	ILogger::EResult  DialogLogOutput::Process (const MessageInfo &info)
	{
		if_likely( not (_levelBits[ usize(info.level) ] and _scopeBits[ usize(info.scope) ] ))
			return EResult::Unknown;

		if ( not _enabled )
			return EResult::Unknown;

		// MessageBox is thread safe, but multiple messages from different threads should be disallowed
		EXLOCK( _guard );

		const String	caption	= "Error message";

		String	str	= "File:      "s << FileSystem::ToShortPath( info.loc.FileName() ) <<
					  "\nLine:     " << ToString( info.loc.Line() ) <<
					  "\nFunction: " << info.loc.FunctionName() <<
					  "\nScope:    " << ToString( info.scope ) <<
					//"\nLevel     " << ToString( info.level ) <<
					  "\n\nMessage:\n";
		{
			usize	pos = 0;
			if ( Parser::MoveToLine( info.message, INOUT pos, 30 ))
				str << SubString( info.message, 0, pos ) << "...";
			else
				str << info.message;
		}

		return _ProcessImpl( caption, str, info.level );
	}
#endif
/*
=================================================
	_ProcessImpl (Windows)
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
	ILogger::EResult  DialogLogOutput::_ProcessImpl (const String &caption, String &msg, ELevel)
	{
		UINT	flags = MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST;

		if ( ::IsDebuggerPresent() )
			flags |= MB_ABORTRETRYIGNORE | MB_DEFBUTTON3;
		else
			flags |= MB_OKCANCEL | MB_DEFBUTTON1;

		int	result = ::MessageBoxExA( null, msg.c_str(), caption.c_str(), flags, MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ));
		switch ( result )
		{
			case IDCANCEL :
			case IDABORT  :	return EResult::Abort;

			case IDRETRY  :	return EResult::Break;

			case IDOK :
			case IDIGNORE :	return EResult::Continue;
		}
		return EResult::Unknown;
	}
#endif
/*
=================================================
	_ProcessImpl (Linux)
=================================================
*/
#ifdef AE_PLATFORM_LINUX
	ILogger::EResult  DialogLogOutput::_ProcessImpl (const String &caption, String &msg, ELevel)
	{
		constexpr StringView	btn_abort	{"Abort"};
		constexpr StringView	btn_retry	{"Retry"};
		constexpr StringView	btn_ignore	{"Ignore"};

		String	cmd;
		cmd.reserve( 256 );

		FindAndReplace( INOUT msg, '"', '\'' );

		cmd << "zenity --question --switch --no-wrap --no-markup"
			<< " --icon dialog-error"
			<< " --title \"" << caption << '"'  // must not contains quotes ""
			<< " --text \"" << msg << '"'
			<< " --extra-button " << btn_abort
			<< " --extra-button " << btn_retry
			<< " --extra-button " << btn_ignore;

		UnixProcess		process;
		if ( not process.ExecuteAsync( cmd, UnixProcess::EFlags::NoWindow | UnixProcess::EFlags::ReadOutput ))
		{
			AE_PRIVATE_BREAK_POINT();
			return EResult::Unknown;
		}

		String	res;
		if ( not process.WaitAndClose( OUT res, seconds{60 * 60 * 24} ))
			return EResult::Unknown;

		if ( StartsWith( res, btn_abort ))
			return EResult::Abort;

		if ( StartsWith( res, btn_retry ))
			return EResult::Break;

		if ( StartsWith( res, btn_ignore ))
			return EResult::Continue;

		// unknown
		return EResult::Unknown;
	}
#endif
/*
=================================================
	DialogLogOutputEms
=================================================
*/
#ifdef AE_PLATFORM_EMSCRIPTEN
	ILogger::EResult  DialogLogOutputEms::Process (const MessageInfo &info)
	{
		if_likely( not (_levelBits[ usize(info.level) ] and _scopeBits[ usize(info.scope) ] ))
			return EResult::Unknown;

		if ( not _enabled )
			return EResult::Unknown;

		String	msg = "confirm( \""s << info.message << "\" )";

		TRY{
			int	ok = emscripten_run_script_int( msg.c_str() );
			return ok == 0 ? EResult::Break : EResult::Continue;
		}
		CATCH_ALL(
			return EResult::Unknown;
		)
	}
#endif
/*
=================================================
	CreateDialogOutput
=================================================
*/
	ILogger::LoggerPtr  ILogger::CreateDialogOutput (LevelBits levelBits, ScopeBits scopeBits) __NE___
	{
		#if defined(AE_CI_BUILD_TEST) or defined(AE_CI_BUILD_PERF) or defined(AE_CFG_RELEASE)
			Unused( levelBits, scopeBits );
			return {};

		#elif defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_APPLE) or defined(AE_PLATFORM_LINUX)
			return MakeUnique<DialogLogOutput>( levelBits, scopeBits );

		#elif defined(AE_PLATFORM_EMSCRIPTEN)
			return MakeUnique<DialogLogOutputEms>( levelBits, scopeBits );

		#elif defined(AE_PLATFORM_EMSCRIPTEN)
			return MakeUnique<AndroidToastLogOutput>( levelBits, scopeBits );

		#elif defined(AE_PLATFORM_ANDROID) and defined(AE_ANDROID_CONSOLE_MODE)
			return MakeUnique<ConsolePauseOnError>();

		#else
			Unused( levelBits, scopeBits );
			return ILogger::CreateBreakOnError();
		#endif
	}
//-----------------------------------------------------------------------------



#ifdef AE_PLATFORM_ANDROID
/*
=================================================
	AndroidLogOutput
=================================================
*/
	ILogger::EResult  AndroidLogOutput::Process (const MessageInfo &info)
	{
		int	log_level = ANDROID_LOG_VERBOSE;

		switch_enum( info.level )
		{
			case ELevel::Debug :	log_level = ANDROID_LOG_DEBUG;	break;
			case ELevel::Info :		log_level = ANDROID_LOG_INFO;	break;
			case ELevel::Warning :	log_level = ANDROID_LOG_WARN;	break;
			case ELevel::Error :	log_level = ANDROID_LOG_ERROR;	break;
			case ELevel::Fatal :	log_level = ANDROID_LOG_FATAL;	break;

			case ELevel::_Count :
			default :				break;
		}
		switch_end

		char	buf [800];
		usize	offset		= 0;
		String	short_path	{ FileSystem::ToShortPath( info.loc.FileName() )};
		String	tid			= ToString<16>( MinimizeThreadID( ThreadUtils::GetIntID() ));

	  #if 1
		for (; offset < info.message.size();)
		{
			const usize	max		= Min( offset + CountOf(buf)-5, info.message.size() );
			usize		end		= Clamp( info.message.rfind( '\n', max ), offset, max );
						end		= (end - offset == 0) ? info.message.size() : end;
			const usize	size	= end - offset;

			MemCopy( OUT buf, Sizeof(buf), info.message.data() + offset, Bytes{size} );
			buf[size] = 0;

			// thread safe
			if ( offset == 0 ){
				Unused( __android_log_print( log_level, _tag.c_str(), "[%s] %s (%i): %s", tid.c_str(), short_path.c_str(), info.loc.Line(), buf ));
			}else{
				Unused( __android_log_write( log_level, _tag.c_str(), buf ));
			}
			offset = end;
		}
	  #else
			Unused( __android_log_print( log_level, _tag.c_str(), "[%s] %s (%i): %s", tid.c_str(), short_path.c_str(), info.loc.Line(), info.message.data() ));
	  #endif

		if ( info.level >= ELevel::Error )
		{
			String	cs = GetCallStack();
			if ( not cs.empty() )
			{
				"callstack:\n" >> cs;
				Unused( __android_log_write( log_level, _tag.c_str(), cs.c_str() ));
			}
		}

		return EResult::Unknown;
	}

/*
=================================================
	AndroidToastLogOutput
=================================================
*/
	namespace {
		static Mutex					s_ToastLogGuard;
		static FixedArray<String, 16>	s_ToastLog;
	}

	ILogger::EResult  AndroidToastLogOutput::Process (const MessageInfo &info)
	{
		if_likely( not (_levelBits[ usize(info.level) ] and _scopeBits[ usize(info.scope) ] ))
			return EResult::Unknown;

		const usize		max_len	= 128;
		usize			len		= Min( info.message.size(), max_len );
		String			msg;

		if ( len < info.message.size() )
			msg = String{info.message.substr( 0, len )} << "...";
		else
			msg = String{info.message};

		{
			EXLOCK( s_ToastLogGuard );

			if ( not s_ToastLog.IsFull() )
				s_ToastLog.push_back( RVRef(msg) );
		}
		return EResult::Unknown;
	}

/*
=================================================
	ExtractToast
=================================================
*/
	String  AndroidToastLogOutput::ExtractToast () __NE___
	{
		EXLOCK( s_ToastLogGuard );

		if ( s_ToastLog.empty() )
			return {};

		String	res = RVRef(s_ToastLog.front());
		s_ToastLog.erase( 0 );
		return res;
	}

#endif // AE_PLATFORM_ANDROID


/*
=================================================
	ConsoleLogOutput
=================================================
*/
	ILogger::EResult  ConsoleLogOutput::Process (const MessageInfo &info)
	{
		String	str = String{ FileSystem::ToShortPath( info.loc.FileName() )} << '(' << ToString( info.loc.Line() ) << "): " << info.message;

		if ( info.level >= ELevel::Error )
		{
			String	cs = GetCallStack();
			if ( not cs.empty() )
				str << "\ncallstack:\n" << cs;
		}

	  #if defined(AE_PLATFORM_WINDOWS) and not (defined(AE_CI_BUILD_TEST) or defined(AE_CI_BUILD_PERF))
		switch_enum( info.level )
		{
			case ELevel::Warning :		"\x1B[33m" >> str;	str << "\x1B[0m ";	break;

			case ELevel::Error :
			case ELevel::Fatal :		"\x1B[31m" >> str;	str << "\x1B[0m ";	break;

			case ELevel::Debug :
			case ELevel::Info :
			case ELevel::_Count :		break;
		}
		switch_end
	  #endif

	  #if defined(AE_PLATFORM_UNIX_BASED) and not (defined(AE_CI_BUILD_TEST) or defined(AE_CI_BUILD_PERF))
		const StringView	c_Reset		= "\033[0m";
		const StringView	c_Red		= "\033[91m";
		const StringView	c_Orange	= "\033[33m";

		switch ( info.level )
		{
			case ELevel::Warning :	c_Orange >> str;	str << c_Reset;		break;
			case ELevel::Error :
			case ELevel::Fatal :	c_Red >> str; 		str << c_Reset;		break;
		}
	  #endif

		{
			EXLOCK( _guard );
			std::cout << str << std::endl;
		}
		return EResult::Unknown;
	}

/*
=================================================
	CreateConsoleOutput
=================================================
*/
	ILogger::LoggerPtr	ILogger::CreateConsoleOutput (StringView tag) __NE___
	{
		// enable console colors
		#ifdef AE_PLATFORM_WINDOWS
		{
			HANDLE	hnd		= ::GetStdHandle( STD_OUTPUT_HANDLE );
			DWORD	mode	= 0;

			::GetConsoleMode( hnd, OUT &mode );
			::SetConsoleMode( hnd, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING );

			::SetConsoleOutputCP( CP_UTF8 );
			//::setvbuf( stdout, null, _IOFBF, 1000 );
		}
		#endif

		Unused( tag );

		#if defined(AE_PLATFORM_ANDROID) and not defined(AE_ANDROID_CONSOLE_MODE)
			if ( tag.empty() )
				tag = "<<<< AE >>>>";

			return MakeUnique<AndroidLogOutput>( tag );

		#elif defined(AE_PLATFORM_LINUX) and not defined(AE_CI_BUILD_TEST)
			// use 'CreateIDEOutput()' instead
			return {};
		#else
			return MakeUnique<ConsoleLogOutput>();
		#endif
	}
//-----------------------------------------------------------------------------


/*
=================================================
	ConsolePauseOnError
=================================================
*/
	ILogger::EResult  ConsolePauseOnError::Process (const MessageInfo &info) __Th___
	{
		if_unlikely( info.level >= ELogLevel::Error )
		{
			EXLOCK( _guard );
			std::cout << "pause on error, press any key to continue " << std::endl;
			char key;
			std::cin >> key;
		}
		return EResult::Continue;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	FileLogOutput::ctor
=================================================
*/
	FileLogOutput::FileLogOutput (RC<WStream> file) __NE___ :
		_file{ RVRef(file) }
	{
		CHECK( _file and _file->IsOpen() );
	}

/*
=================================================
	FileLogOutput::Process
=================================================
*/
	ILogger::EResult  FileLogOutput::Process (const MessageInfo &info)
	{
		EXLOCK( _guard );

		if_likely( _file )
		{
			String str;
			str << "[l:" << ToString( info.level ) << ", s:" << ToString( info.scope ) << ", t:";

			// thread name
			{
				usize	tid	= ThreadUtils::GetIntID();
				auto	it	= _threadNames.find( tid );

				if_likely( it != _threadNames.end() )
					str << it->second;
				else
					str << ToString<16>( MinimizeThreadID( tid ));
			}
			str << "]\n";

			str << info.message;

			str << "\n\t{" << FileSystem::ToShortPath( info.loc.FileName() ) << '(' << ToString( info.loc.Line() ) << ")}";

			Unused( _file->Write( str ));
			_file->Flush();
		}
		return EResult::Unknown;
	}

/*
=================================================
	FileLogOutput::SetCurrentThreadName
=================================================
*/
	void  FileLogOutput::SetCurrentThreadName (StringView name) __NE___
	{
		TRY{
			EXLOCK( _guard );
			const usize		tid	= ThreadUtils::GetIntID();

			_threadNames.insert_or_assign( tid, String{name} );
		}
		CATCH_ALL()
	}

/*
=================================================
	CreateFileOutput
=================================================
*/
	ILogger::LoggerPtr	ILogger::CreateFileOutput (StringView fileName) __NE___
	{
		const auto		mode	= FileWStream::EMode::OpenRewrite | FileWStream::EMode::SharedRead;
		Path			path	= Path{fileName}.replace_extension(".txt");
		RC<FileWStream>	file	= FileSystem::OpenUnusedFile<FileWStream>( INOUT path, mode, 10 );

		if ( file )
		{
			AE_LOG_DBG( "Created text logger to file '"s << ToString( FileSystem::ToAbsolute( path )) << "'" );
			return MakeUnique<FileLogOutput>( RVRef(file) );
		}
		return LoggerPtr{};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	HtmlLogOutput::ctor
=================================================
*/
	HtmlLogOutput::HtmlLogOutput (RC<WStream> file, bool tnames) __NE___ :
		_file{ RVRef(file) },
		_enableThreadNames{ tnames }
	{
		EXLOCK( _guard );

		if ( _file and _file->IsOpen() )
		{
			_Flush( R"(
<html> <head> <title> log </title> </head> <body BGCOLOR="#ffffff">
<p><PRE><font face="Courier New, Verdana" size="2" color="#000000"/>
)" );
		}
		else
		{
			CHECK_MSG( false, "failed to open html log" );
			_file = null;
		}
	}

/*
=================================================
	HtmlLogOutput::dtor
=================================================
*/
	HtmlLogOutput::~HtmlLogOutput () __NE___
	{
		EXLOCK( _guard );

		if ( _file )
		{
			String	str;
			_SetColor( EColor::Black, EColor::White, INOUT str );
			str << "Log closed.</PRE> </p> </body> </html>\n";

			_Flush( str );
			_file = null;
		}
	}

/*
=================================================
	HtmlLogOutput::_SetColor
=================================================
*/
	void  HtmlLogOutput::_SetColor (EColor col, EColor, INOUT String &str)
	{
		str << "<font color=\"#" << FormatAlignedI<16>( uint(col), 6, '0' )
			//<< "\"; style=\"background-color: #" << FormatAlignedI<16>( uint(bg), 6, '0' )
			<< "\">";
	}

/*
=================================================
	HtmlLogOutput::_Flush
=================================================
*/
	void  HtmlLogOutput::_Flush (StringView str) C_NE___
	{
		Unused( _file->Write( str ));
		_file->Flush();
	}

/*
=================================================
	HtmlLogOutput::Process
=================================================
*/
	ILogger::EResult  HtmlLogOutput::Process (const MessageInfo &info)
	{
		bool	add_time	= false;
		bool	add_file	= false;
		EColor	col			= EColor::Black;
		EColor	bg_col		= EColor::White;

		/*switch_enum( info.scope )
		{
			case EScope::Unknown :			bg_col = EColor::Black;	break;
			case EScope::GraphicsDriver :	bg_col = EColor::Green;	break;
			case EScope::Network :			bg_col = EColor::Gray;	break;
			case EScope::Engine :			bg_col = EColor::Gold;	break;
			case EScope::System :			bg_col = EColor::Gray;	break;
			case EScope::Client :			bg_col = EColor::Blue;	break;
			case EScope::_Count :
			default :						DBG_WARNING( "unknown log level" );
		}
		switch_end*/

		switch_enum( info.level )
		{
			case ELevel::Debug :		add_time = true;	add_file = true;	col = EColor::Navy;			break;
			case ELevel::Info :			add_time = true;	add_file = true;	col = EColor::DarkGreen;	break;
			case ELevel::Warning :		add_time = true;	add_file = true;	col = EColor::Orange;		break;
			case ELevel::Error :		add_time = true;	add_file = true;	col = EColor::Red;			break;
			case ELevel::Fatal :		add_time = true;	add_file = true;	col = EColor::DarkRed;		break;
			case ELevel::_Count :
			default :					DBG_WARNING( "unknown log level" );
		}
		switch_end


		String	str;
		str.reserve( 256 );

		_SetColor( col, bg_col, INOUT str );

		str << LevelToChar( info.level ) << ' ';

		// thread name
		if ( _enableThreadNames )
		{
			EXLOCK( _guard );

			str << '[';

			usize	tid	= ThreadUtils::GetIntID();
			auto	it	= _threadInfos.find( tid );

			if ( it != _threadInfos.end() )
			{
				str << it->second.name;
				//bg_col = it->second.bgColor;
			}
			else
				str << ToString<16>( MinimizeThreadID( tid ));

			str << "] ";
		}

		if ( add_time )
		{} // TODO

		str << info.message << "</font>";

		if ( add_file )
		{
			_SetColor( EColor::Silver, bg_col, INOUT str );
			str << "  (file: '" << FileSystem::ToShortPath( info.loc.FileName() ) << "', line: " << ToString( info.loc.Line() ) << ")</font>";
		}

		if_unlikely( info.level >= ELevel::Warning )
		{
			String	cs = GetCallStack();
			if ( not cs.empty() )
			{
				"<details><summary>" >> str;
				str << "</summary>  callstack:\n";

				_SetColor( EColor::DarkGrey, bg_col, INOUT str );

				str << cs << "</font></details>";
			}
			else
				str << "\n";
		}
		else
			str << "\n";


		EXLOCK( _guard );

		if ( _file )
			_Flush( str );

		return EResult::Unknown;
	}

/*
=================================================
	HtmlLogOutput::SetCurrentThreadName
=================================================
*/
	void  HtmlLogOutput::SetCurrentThreadName (StringView name) __NE___
	{
		if ( not _enableThreadNames )
			return;

		TRY{
			EXLOCK( _guard );
			const usize	tid	= ThreadUtils::GetIntID();

			ThreadInfo	info;
			info.name	= String{name};

			if_likely( not _threadInfos.empty() )
			{
				const float		h	= _random.Uniform( 0.f, 0.7f );
				const RGBA8u	col	{RGBA32f{ HSVColor{ h, 0.05f }}};

				info.bgColor	= EColor( BitCast<uint>( col.ABGR() ) >> 8 );
			}
			else
			{
				info.bgColor	= EColor( BitCast<uint>( HtmlColor::White.ABGR() ) >> 8 );
			}

			_threadInfos.insert_or_assign( tid, RVRef(info) );
		}
		CATCH_ALL()
	}

/*
=================================================
	CreateHtmlOutput
=================================================
*/
	ILogger::LoggerPtr	ILogger::CreateHtmlOutput (StringView fileName) __NE___
	{
		const auto		mode	= FileWStream::EMode::OpenRewrite | FileWStream::EMode::SharedRead;
		Path			path	= Path{fileName}.replace_extension(".html");
		RC<FileWStream>	file	= FileSystem::OpenUnusedFile<FileWStream>( INOUT path, mode, 10 );

		if ( file )
		{
			AE_LOG_DBG( "Created html logger to file '"s << ToString( FileSystem::ToAbsolute( path )) << "'" );
			return MakeUnique<HtmlLogOutput>( RVRef(file), true );
		}
		return LoggerPtr{};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	HtmlLogOutputPerThread ctor / dtor
=================================================
*/
	HtmlLogOutputPerThread::HtmlLogOutputPerThread (StringView prefix) __NE___ :
		_filenamePrefix{ FileSystem::ToAbsolute( prefix )}
	{}

	HtmlLogOutputPerThread::~HtmlLogOutputPerThread () __NE___
	{}

/*
=================================================
	HtmlLogOutputPerThread::Process
=================================================
*/
	ILogger::EResult  HtmlLogOutputPerThread::Process (const MessageInfo &info) __Th___
	{
		{
			SHAREDLOCK( _guard );
			auto	it = _perThread.find( info.threadId );
			if_likely( it != _perThread.end() )
				return it->second->Process( info );
		}
		{
			EXLOCK( _guard );
			auto&	output = _perThread[ info.threadId ];

			if ( not output )
			{
				const auto		mode	= FileWStream::EMode::OpenRewrite | FileWStream::EMode::SharedRead;
				Path			path	= Path{_filenamePrefix}.replace_extension(".html");
				RC<FileWStream>	file	= FileSystem::OpenUnusedFile<FileWStream>( INOUT path, mode, 10 );

				if_unlikely( not file )
					return EResult::Unknown;

				output.reset( new HtmlLogOutput{ RVRef(file), false });
			}
			return output->Process( info );
		}
	}

/*
=================================================
	CreateHtmlOutputPerThread
=================================================
*/
	ILogger::LoggerPtr  ILogger::CreateHtmlOutputPerThread (StringView prefix) __NE___
	{
		return MakeUnique<HtmlLogOutputPerThread>( prefix );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BreakOnErrorLogger::Process
=================================================
*/
	ILogger::EResult  BreakOnErrorLogger::Process (const MessageInfo &info) __Th___
	{
		return	info.level >= ELevel::Error  ?
					EResult::Break :
					EResult::Continue;
	}

/*
=================================================
	CreateBreakOnError
=================================================
*/
	ILogger::LoggerPtr  ILogger::CreateBreakOnError () __NE___
	{
	#ifndef AE_CFG_RELEASE
		if ( PlatformUtils::IsUnderDebugger() )
			return MakeUnique<BreakOnErrorLogger>();
		else
	#endif
			return {};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GetDialogLevelBits
=================================================
*/
	ILogger::LevelBits  ILogger::GetDialogLevelBits () __NE___
	{
		LevelBits	res{ ~0u };
		res[ uint(ELevel::Debug) ]		= false;
		res[ uint(ELevel::Info)  ]		= false;
		res[ uint(ELevel::Warning) ]	= false;
		return res;
	}

/*
=================================================
	GetDialogScopeBits
=================================================
*/
	ILogger::ScopeBits  ILogger::GetDialogScopeBits () __NE___
	{
		return ScopeBits{ ~0u };
	}


} // AE::Base
