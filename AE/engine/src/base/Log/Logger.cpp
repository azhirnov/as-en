// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
#   include <android/log.h>
#else
#   include <iostream>
#endif
#ifdef AE_PLATFORM_EMSCRIPTEN
#   include <emscripten.h>
#endif
#ifdef AE_COMPILER_MSVC
#   include "base/Platforms/WindowsHeader.cpp.h"
#endif

#include "base/Log/Logger.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/Parser.h"
#include "base/DataSource/FileStream.h"
#include "base/Platforms/ThreadUtils.h"


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
    ToShortPath
=================================================
*/
    ND_ static StringView  ToShortPath (StringView file) __NE___
    {
        const uint  max_parts = 3;

        usize   i = Max( file.length(), 1u ) - 1;
        uint    j = 0;

        for (; i < file.length() and j < max_parts; --i)
        {
            const char  c = file[i];

            if_unlikely( (c == '\\') | (c == '/') )
                ++j;
        }

        if ( i < file.length() )
            return file.substr( i + (j == max_parts ? 2 : 0) );
        else
            return file;
    }

/*
=================================================
    ScopeToString
=================================================
*/
    ND_ static StringView  ScopeToString (ELogScope scope) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( scope )
        {
            case ELogScope::GraphicsDriver :    return "GraphicsDriver ";   break;
            case ELogScope::Engine :            return "Engine ";           break;
            case ELogScope::Client :            return "Client ";           break;
            case ELogScope::System :            return "System ";           break;
            case ELogScope::Network :           return "Network ";          break;

            case ELogScope::Unknown :
            case ELogScope::_Count :
            default :                           return {};
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    LevelToString
=================================================
*/
    ND_ static StringView  LevelToString (ELogLevel level) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( level )
        {
            case ELogLevel::Debug :         return "Debug";         break;
            case ELogLevel::Info :          return "Info";          break;
            case ELogLevel::Warning :       return "Warning";       break;
            case ELogLevel::SilentError:
            case ELogLevel::Error :         return "Error";         break;
            case ELogLevel::Fatal :         return "Fatal error";   break;

            case ELogLevel::_Count :
            default :                       return {};
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    LevelToChar
=================================================
*/
    ND_ static char  LevelToChar (ELogLevel level) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( level )
        {
            case ELogLevel::Debug :         return 'D';     break;
            case ELogLevel::Info :          return 'I';     break;
            case ELogLevel::Warning :       return 'W';     break;
            case ELogLevel::SilentError:
            case ELogLevel::Error :         return 'E';     break;
            case ELogLevel::Fatal :         return 'F';     break;

            case ELogLevel::_Count :
            default :                       return ' ';
        }
        END_ENUM_CHECKS();
    }

} // namespace
//-----------------------------------------------------------------------------


#ifdef AE_COMPILER_MSVC

/*
=================================================
    VisualStudioLogOutput
=================================================
*/
    ILogger::EResult  VisualStudioLogOutput::Process (const MessageInfo &info)
    {
        const String    str = String{info.file} << '(' << ToString( info.line ) << "): "
                            << ScopeToString( info.scope ) << LevelToString( info.level )
                            << ": " << info.message << '\n';

        ::OutputDebugStringA( str.c_str() );    // thread safe

        return EResult::Unknown;
    }
#endif // AE_COMPILER_MSVC

/*
=================================================
    CreateIDEOutput
=================================================
*/
    ILogger::LoggerPtr  ILogger::CreateIDEOutput () __NE___
    {
    #ifdef AE_COMPILER_MSVC
        if ( ::IsDebuggerPresent() )
            return MakeUnique<VisualStudioLogOutput>();
        else
            return {};
    #else
        return {};
    #endif
    }
//-----------------------------------------------------------------------------


#if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_APPLE)
/*
=================================================
    DialogLogOutput
=================================================
*/
    ILogger::EResult  DialogLogOutput::Process (const MessageInfo &info)
    {
        if_likely( not (_levelBits[ usize(info.level) ] and _scopeBits[ usize(info.scope) ] ))
            return EResult::Unknown;

        // MessageBox is thread safe, but multiple messages from different thread should be disallowed
        EXLOCK( _guard );

        const String    caption = "Error message";

        String  str = "File:      "s << ToShortPath( info.file ) <<
                      "\nLine:     " << ToString( info.line ) <<
                      "\nFunction: " << info.func <<
                      "\nScope:    " << ScopeToString( info.scope ) <<
                    //"\nLevel     " << LevelToString( info.level ) <<
                      "\n\nMessage:\n";
        {
            usize   pos = 0;
            if ( Parser::MoveToLine( info.message, INOUT pos, 30 ))
                str << info.message.substr( 0, pos ) << "...";
            else
                str << info.message;
        }

        return _ProcessImpl( caption, str, info.level );
    }
#endif

#ifdef AE_PLATFORM_WINDOWS
/*
=================================================
    DialogLogOutput
=================================================
*/
    ILogger::EResult  DialogLogOutput::_ProcessImpl (const String &caption, const String &msg, ELevel)
    {
        UINT    flags = MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST;

        if ( ::IsDebuggerPresent() )
            flags |= MB_ABORTRETRYIGNORE | MB_DEFBUTTON3;
        else
            flags |= MB_OKCANCEL | MB_DEFBUTTON1;

        int result = ::MessageBoxExA( null, msg.c_str(), caption.c_str(), flags, MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ));
        switch ( result )
        {
            case IDCANCEL :
            case IDABORT  : return EResult::Abort;

            case IDRETRY  : return EResult::Break;

            case IDOK :
            case IDIGNORE : return EResult::Continue;
        };

        return EResult::Unknown;
    }
#endif

#ifdef AE_PLATFORM_EMSCRIPTEN
/*
=================================================
    DialogLogOutputEms
=================================================
*/
    ILogger::EResult  DialogLogOutputEms::Process (const MessageInfo &info)
    {
        if_likely( not (_levelBits[ usize(info.level) ] and _scopeBits[ usize(info.scope) ] ))
            return EResult::Unknown;

        String  msg = "confirm( \""s << info.message << "\" )";

        TRY{
            int ok = emscripten_run_script_int( msg.c_str() );
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
        #if not defined(AE_CI_BUILD)
        # if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_APPLE)
            return MakeUnique<DialogLogOutput>( levelBits, scopeBits );

        # elif defined(AE_PLATFORM_EMSCRIPTEN) and defined(AE_DISABLE_THREADS)
            return MakeUnique<DialogLogOutputEms>( levelBits, scopeBits );

        # else
            Unused( levelBits, scopeBits );
            return {};
        # endif
        #else
            Unused( levelBits, scopeBits );
            return {};
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
        int log_level   = ANDROID_LOG_VERBOSE;
        BEGIN_ENUM_CHECKS();
        switch ( info.level )
        {
            case ELevel::Debug :    log_level = ANDROID_LOG_DEBUG;  break;
            case ELevel::Info :     log_level = ANDROID_LOG_INFO;   break;
            case ELevel::Warning :  log_level = ANDROID_LOG_WARN;   break;
            case ELevel::Error :    log_level = ANDROID_LOG_ERROR;  break;
            case ELevel::Fatal :    log_level = ANDROID_LOG_FATAL;  break;

            case ELevel::_Count :
            default :               break;
        }
        END_ENUM_CHECKS();

        char        buf [800];
        usize       offset      = 0;
        StringView  short_path  = ToShortPath( info.file );

        for (; offset < info.message.size();)
        {
            const usize max = Min( offset + CountOf(buf)-1, info.message.size() );
            usize       end = Clamp( info.message.rfind( '\n', max ), offset, max );
                        end = (max - end < 5 ? max : ((float(end - offset) / float(max - offset)) < 0.7f ? max : end));
            const usize pos = end - offset;

            MemCopy( OUT buf, Sizeof(buf), info.message.data() + offset, Bytes{pos} );
            buf[pos] = 0;

            // thread safe
            if ( offset == 0 ){
                Unused( __android_log_print( log_level, _tag.c_str(), "%s (%i): %s", short_path.data(), info.line, buf ));
            }else{
                Unused( __android_log_write( log_level, _tag.c_str(), buf ));
            }
            offset = end;
        }

        if ( info.level >= ELevel::Error )
            return EResult::Break;

        return EResult::Unknown;
    }

/*
=================================================
    CreateConsoleOutput
=================================================
*/
    ILogger::LoggerPtr  ILogger::CreateConsoleOutput (StringView tag) __NE___
    {
        if ( tag.empty() )
            tag = "<<<< AE >>>>";

        return MakeUnique<AndroidLogOutput>( tag );
    }

#else

/*
=================================================
    ConsoleLogOutput
=================================================
*/
    ILogger::EResult  ConsoleLogOutput::Process (const MessageInfo &info)
    {
        String str = String{ ToShortPath( info.file )} << '(' << ToString( info.line ) << "): " << info.message;

        BEGIN_ENUM_CHECKS();
        switch ( info.level )
        {
            case ELevel::Debug :
            case ELevel::Info :         str << '\n';                            break;

            case ELevel::SilentError :
            case ELevel::Warning :      "\x1B[33m" >> str;  str << "\x1B[0m\n"; break;

            case ELevel::Error :
            case ELevel::Fatal :        "\x1B[31m" >> str;  str << "\x1B[0m\n"; break;

            case ELevel::_Count :       break;
        }
        END_ENUM_CHECKS();

        EXLOCK( _guard );
        std::cout << str;

        return EResult::Unknown;
    }

/*
=================================================
    CreateConsoleOutput
=================================================
*/
    ILogger::LoggerPtr  ILogger::CreateConsoleOutput (StringView) __NE___
    {
        // enable console colors
        #ifdef AE_PLATFORM_WINDOWS
        {
            HANDLE  hnd     = ::GetStdHandle( STD_OUTPUT_HANDLE );
            DWORD   mode    = 0;

            ::GetConsoleMode( hnd, OUT &mode );
            ::SetConsoleMode( hnd, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING );
        }
        #endif

        return MakeUnique<ConsoleLogOutput>();
    }

#endif
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
            str << "[l:" << LevelToString( info.level ) << ", s:" << ScopeToString( info.scope ) << ", t:";

            // thread name
            {
                usize   tid = ThreadUtils::GetIntID();
                auto    it  = _threadNames.find( tid );

                if_likely( it != _threadNames.end() )
                    str << it->second;
                else
                    str << ToString<16>( tid );
            }
            str << "]\n";

            str << info.message;

            str << "\n\t{" << ToShortPath( info.file ) << '(' << ToString( info.line ) << ")}";

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
            const usize     tid = ThreadUtils::GetIntID();

            _threadNames.insert_or_assign( tid, String{name} );
        }
        CATCH_ALL()
    }

/*
=================================================
    CreateFileOutput
=================================================
*/
    ILogger::LoggerPtr  ILogger::CreateFileOutput (StringView fileName) __NE___
    {
        auto    file = MakeRC<FileWStream>( NtStringView{fileName} );

        if ( file->IsOpen() )
            return MakeUnique<FileLogOutput>( RVRef(file) );
        else
            return {};
    }
//-----------------------------------------------------------------------------



/*
=================================================
    HtmlLogOutput::ctor
=================================================
*/
    HtmlLogOutput::HtmlLogOutput (RC<WStream> file) __NE___ :
        _file{ RVRef(file) },
        _txtColor{ uint(EColor::Black) },
        _bgColor{ uint(EColor::White) }
    {
        CHECK( _file and _file->IsOpen() );

        EXLOCK( _guard );

        _Flush( R"(
<html> <head> <title> log </title> </head> <body BGCOLOR="#ffffff">
<p><PRE><font face="Courier New, Verdana" size="2" color="#000000">
)" );
    }

/*
=================================================
    HtmlLogOutput::dtor
=================================================
*/
    HtmlLogOutput::~HtmlLogOutput () __NE___
    {
        EXLOCK( _guard );

        String  str;
        _SetColor( EColor::Black, EColor::White, INOUT str );
        str << "Log closed.</font></PRE> </p> </body> </html>\n";

        _Flush( str );
    }

/*
=================================================
    HtmlLogOutput::_SetColor
=================================================
*/
    void  HtmlLogOutput::_SetColor (EColor col, EColor bg, INOUT String &str)
    {
        if ( _txtColor != uint(col) or _bgColor != uint(bg) )
        {
            _txtColor   = uint(col);
            _bgColor    = uint(bg);

            str << "</font><font color=\"#" << FormatAlignedI<16>( _txtColor, 6, '0' )
                << "\"; style=\"background-color: #" << FormatAlignedI<16>( _bgColor, 6, '0' ) << "\">";
        }
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
        bool    add_time    = false;
        bool    add_file    = false;
        EColor  col         = EColor::Black;
        EColor  bg_col      = EColor::White;

        BEGIN_ENUM_CHECKS();
        /*switch ( info.scope )
        {
            case EScope::Unknown :          col = EColor::Black;    break;
            case EScope::GraphicsDriver :   col = EColor::Green;    break;
            case EScope::Network :          col = EColor::Gray;     break;
            case EScope::Engine :           col = EColor::Gold;     break;
            case EScope::System :           col = EColor::Gray;     break;
            case EScope::Client :           col = EColor::Blue;     break;
            case EScope::_Count :
            default :                       DBG_WARNING( "unknown log level" );
        }*/
        switch ( info.level )
        {
            case ELevel::Debug :        add_time = true;    add_file = true;    col = EColor::Navy;         break;
            case ELevel::Info :         add_time = true;    add_file = true;    col = EColor::DarkGreen;    break;
            case ELevel::Warning :      add_time = true;    add_file = true;    col = EColor::Orange;       break;
            case ELevel::SilentError:
            case ELevel::Error :        add_time = true;    add_file = true;    col = EColor::Red;          break;
            case ELevel::Fatal :        add_time = true;    add_file = true;    col = EColor::DarkRed;      break;
            case ELevel::_Count :
            default :                   DBG_WARNING( "unknown log level" );
        }
        END_ENUM_CHECKS();


        EXLOCK( _guard );

        if_likely( _file )
        {
            String  str;
            str.reserve( 256 );

            str << LevelToChar( info.level ) << ' ';

            // thread name
            {
                str << '[';

                usize   tid = ThreadUtils::GetIntID();
                auto    it  = _threadInfos.find( tid );

                if_likely( it != _threadInfos.end() )
                {
                    str << it->second.name;
                    bg_col = it->second.bgColor;
                }
                else
                    str << ToString<16>( tid );

                str << "] ";
            }

            // insert color
            {
                String  tmp;
                _SetColor( col, bg_col, OUT tmp );

                tmp >> str;
            }

            if ( add_time )
            {} // TODO

            str << info.message;

            if ( add_file )
            {
                _SetColor( EColor::Silver, EColor(_bgColor), INOUT str );
                str << "  (file: '" << ToShortPath( info.file ) << "', line: " << ToString( info.line ) << ")";
            }

            str << "\n";

            _Flush( str );
        }
        return EResult::Unknown;
    }

/*
=================================================
    HtmlLogOutput::SetCurrentThreadName
=================================================
*/
    void  HtmlLogOutput::SetCurrentThreadName (StringView name) __NE___
    {
        TRY{
            EXLOCK( _guard );
            const usize tid = ThreadUtils::GetIntID();

            ThreadInfo  info;
            info.name   = String{name};

            if_likely( not _threadInfos.empty() )
            {
                const float     h   = _random.Uniform( 0.f, 0.7f );
                const RGBA8u    col {RGBA32f{ HSVColor{ h, 0.05f }}};

                info.bgColor    = EColor( BitCast<uint>( col.ABGR() ) >> 8 );
            }
            else
            {
                info.bgColor    = EColor( BitCast<uint>( HtmlColor::White.ABGR() ) >> 8 );
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
    ILogger::LoggerPtr  ILogger::CreateHtmlOutput (StringView fileName) __NE___
    {
        auto    file = MakeRC<FileWStream>( NtStringView{fileName} );

        if ( file->IsOpen() )
            return MakeUnique<HtmlLogOutput>( RVRef(file) );
        else
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
        LevelBits   res{ ~0u };
        res[ uint(ELevel::Debug) ]          = false;
        res[ uint(ELevel::Info)  ]          = false;
        res[ uint(ELevel::SilentError)  ]   = false;
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
