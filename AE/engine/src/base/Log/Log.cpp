// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_COMPILER_MSVC
#   include "base/Platforms/WindowsHeader.cpp.h"
#endif

#include "base/Common.h"
#include "base/Containers/InPlace.h"
#include "base/Utils/Atomic.h"

#ifdef AE_ENABLE_LOGS

namespace AE::Base
{
namespace
{
    static StaticLogger::LevelBits              s_levelBits { ~0u };
    static StaticLogger::ScopeBits              s_scopeBits { ~0u };
    static std::shared_mutex                    s_loggersGuard;
    static Optional< Array< Unique<ILogger> >>  s_loggers;
    static int                                  s_refCounter    = 0;
    static thread_local Atomic<int>             s_recursion     {0};

/*
=================================================
    ProcessMessage
=================================================
*/
    ND_ static StaticLogger::EResult  ProcessMessage2 (const ILogger::MessageInfo &info)
    {
        SHAREDLOCK( s_loggersGuard );

        StaticLogger::EResult   result  = StaticLogger::EResult::Unknown;

        if_unlikely( s_levelBits[ usize(info.level) ] and s_scopeBits[ usize(info.scope) ] )
        {
            if_unlikely( not s_loggers.has_value() )
            {
                AE_PRIVATE_BREAK_POINT();
                return StaticLogger::EResult::Continue;
            }
            if_unlikely( s_loggers->empty() )
            {
                return StaticLogger::EResult::Continue;
            }

            for (auto& log : *s_loggers)
            {
                result = Max( result, log->Process( info ));    // throw
            }
        }

        return result;
    }

    ND_ static StaticLogger::EResult  ProcessMessage (const ILogger::MessageInfo &info)
    {
        if_unlikely( s_recursion.fetch_add( 1 ) > 0 )
        {
            #ifdef AE_COMPILER_MSVC
              ::OutputDebugStringA( "Recursion call for logger." );
            #endif
            return StaticLogger::EResult::Break;    // avoid recursion
        }

        auto    res = ProcessMessage2( info );

        s_recursion.fetch_sub( 1 );
        return res;
    }

} // namespace


/*
=================================================
    Initialize
=================================================
*/
    void  StaticLogger::Initialize () __NE___
    {
        EXLOCK( s_loggersGuard );

        ++s_refCounter;
        s_loggers.emplace();
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  StaticLogger::Deinitialize (bool checkMemLeaks) __NE___
    {
        EXLOCK( s_loggersGuard );

        if ( --s_refCounter != 0 )
            return;

        s_loggers.reset();

        Unused( checkMemLeaks );
        #ifdef AE_ENABLE_MEMLEAK_CHECKS
        if ( checkMemLeaks )
        {
            if ( not AE_DUMP_MEMLEAKS() )
            {
                ILogger::MessageInfo    info;
                info.message    = "AE_DUMP_MEMLEAKS failed";
                info.func       = "";
                info.file       = __FILE__;
                info.line       = __LINE__;
                info.threadId   = 0;
                info.level      = ILogger::ELevel::Fatal;
                info.scope      = ILogger::EScope::Engine;

            #if defined(AE_PLATFORM_WINDOWS) and not defined(AE_CI_BUILD)
                Unused( ILogger::CreateDialogOutput()->Process( info ));
            #else
                Unused( ILogger::CreateConsoleOutput()->Process( info ));
            #endif
            }
        }
        #endif
    }

/*
=================================================
    SetFilter
=================================================
*/
    void  StaticLogger::SetFilter (LevelBits levelBits, ScopeBits scopeBits) __NE___
    {
        EXLOCK( s_loggersGuard );

        s_levelBits = levelBits;
        s_scopeBits = scopeBits;

    }

/*
=================================================
    ClearLoggers
=================================================
*/
    void  StaticLogger::ClearLoggers () __NE___
    {
        EXLOCK( s_loggersGuard );

        s_loggers->clear();
    }

/*
=================================================
    AddLogger
=================================================
*/
    void  StaticLogger::AddLogger (Unique<ILogger> ptr) __NE___
    {
        if ( ptr == null )
            return;

        EXLOCK( s_loggersGuard );

        s_loggers->emplace_back( RVRef( ptr ));     // should not throw
    }

/*
=================================================
    InitDefault
=================================================
*/
    void  StaticLogger::InitDefault () __NE___
    {
        Initialize();

        TRY{
            AddLogger( ILogger::CreateIDEOutput() );
            AddLogger( ILogger::CreateConsoleOutput() );
            //AddLogger( ILogger::CreateFileOutput( "log.txt" ));
            //AddLogger( ILogger::CreateHtmlOutput( "log.html" ));
            AddLogger( ILogger::CreateDialogOutput() );
        }
        CATCH_ALL();

        SetCurrentThreadName( "main" );
    }

/*
=================================================
    Process
=================================================
*/
    StaticLogger::EResult  StaticLogger::Process (StringView msg, StringView func, StringView file, unsigned int line, ILogger::ELevel level, ILogger::EScope scope) __Th___
    {
    #ifdef AE_ENABLE_LOGS
        ILogger::MessageInfo    info;
        info.message    = msg;
        info.func       = func;
        info.file       = file;
        info.line       = line;
        info.threadId   = ThreadUtils::GetIntID();
        info.level      = level;
        info.scope      = scope;

        return ProcessMessage( info );

    #else
        Unused( msg, func, file, line, level, scope );

    #endif
    }

    StaticLogger::EResult  StaticLogger::Process (const char* msg, const char* func, const char* file, unsigned int line, ILogger::ELevel level, ILogger::EScope scope) __Th___
    {
        return Process( StringView{msg}, StringView{func}, StringView{file}, line, level, scope );
    }

/*
=================================================
    SetCurrentThreadName
=================================================
*/
    void  StaticLogger::SetCurrentThreadName (std::string_view name) __NE___
    {
        EXLOCK( s_loggersGuard );

        if_unlikely( not s_loggers.has_value() )
        {
            AE_PRIVATE_BREAK_POINT();
            return;
        }

        for (auto& log : *s_loggers)
        {
            log->SetCurrentThreadName( name );
        }
    }

} // AE::Base
//-----------------------------------------------------------------------------

#else

namespace AE::Base
{
    void  StaticLogger::Initialize ()                           __NE___ {}
    void  StaticLogger::Deinitialize (bool)                     __NE___ {}
    void  StaticLogger::SetFilter (LevelBits, ScopeBits)        __NE___ {}
    void  StaticLogger::ClearLoggers ()                         __NE___ {}
    void  StaticLogger::AddLogger (Unique<ILogger>)             __NE___ {}
    void  StaticLogger::InitDefault ()                          __NE___ {}
    void  StaticLogger::SetCurrentThreadName (std::string_view) __NE___ {}

    StaticLogger::EResult  StaticLogger::Process (StringView, StringView, StringView, unsigned int, ILogger::ELevel, ILogger::EScope) __Th___ { return StaticLogger::EResult::Continue; }
    StaticLogger::EResult  StaticLogger::Process (const char*, const char*, const char*, unsigned int, ILogger::ELevel, ILogger::EScope) __Th___ { return StaticLogger::EResult::Continue; }

} // AE::Base

#endif // AE_ENABLE_LOGS
