// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/EmscriptenUtils.h"

#ifdef AE_PLATFORM_EMSCRIPTEN

namespace AE::Base
{

/*
=================================================
    SetCurrentThreadName
=================================================
*/
    void  EmscriptenUtils::SetCurrentThreadName (NtStringView name) __NE___
    {
        StaticLogger::SetCurrentThreadName( StringView{name} );

        // TODO
    }

/*
=================================================
    GetCurrentThreadName
=================================================
*/
    String  EmscriptenUtils::GetCurrentThreadName ()
    {
        // TODO
        return Default;
    }

/*
=================================================
    GetCurrentThreadHandle
=================================================
*/
    ThreadHandle  EmscriptenUtils::GetCurrentThreadHandle () __NE___
    {
        // TODO
        return Default;
    }

/*
=================================================
    SetThreadAffinity
=================================================
*/
    bool  EmscriptenUtils::SetThreadAffinity (const ThreadHandle &handle, uint coreIdx) __NE___
    {
        // TODO
        Unused( handle, coreIdx );
        return false;
    }

    bool  EmscriptenUtils::SetCurrentThreadAffinity (uint coreIdx) __NE___
    {
        // TODO
        Unused( coreIdx );
        return false;
    }

/*
=================================================
    SetThreadPriority
=================================================
*/
    bool  EmscriptenUtils::SetThreadPriority (const ThreadHandle &handle, float priority) __NE___
    {
        // TODO
        Unused( handle, priority );
        return false;
    }

    bool  EmscriptenUtils::SetCurrentThreadPriority (float priority) __NE___
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
    uint  EmscriptenUtils::GetProcessorCoreIndex () __NE___
    {
        // TODO
        return 0;
    }

/*
=================================================
    ThreadPause
=================================================
*/
    void  EmscriptenUtils::ThreadPause () __NE___
    {
        // not supported
    }


} // AE::Base

#endif // AE_PLATFORM_EMSCRIPTEN
