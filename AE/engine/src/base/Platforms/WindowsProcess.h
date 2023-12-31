// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "base/Utils/FileSystem.h"
# include "base/Utils/Threading.h"

namespace AE::Base
{

    //
    // WinAPI Process
    //

    class WindowsProcess final
    {
    // types
    public:
        enum class EFlags
        {
            None            = 0,
            NoWindow        = 1 << 0,
            ReadOutput      = 1 << 1,   // optional for sync execution, not compatible with 'NoWindow' flag
            UseCommandPromt = 1 << 2,
            Unknown         = None,
        };

    private:
        using Mutex = std::mutex;


    // variables
    private:
        void *      _thread         = null;
        void *      _process        = null;
        void *      _streamRead     = null;
        void *      _streamWrite    = null;
        EFlags      _flags          = Default;

        static constexpr auto   _DefTimeout = milliseconds{60'000};


    // methods
    public:
        WindowsProcess ()   {}
        ~WindowsProcess ();

        bool  ExecuteAsync (String &commandLine, EFlags flags = EFlags::NoWindow);
        bool  ExecuteAsync (WString &commandLine, EFlags flags = EFlags::NoWindow);
        bool  ExecuteAsync (String &commandLine, const Path &currentDir, EFlags flags = EFlags::NoWindow);
        bool  ExecuteAsync (WString &commandLine, const Path &currentDir, EFlags flags = EFlags::NoWindow);

        ND_ bool  IsActive () C_NE___;

        bool  Terminate (milliseconds timeout = _DefTimeout);

        bool  WaitAndClose (milliseconds timeout = _DefTimeout);
        bool  WaitAndClose (INOUT String &output, Mutex* outputGuard = null, milliseconds timeout = _DefTimeout);

        static bool  Execute (String &commandLine, EFlags flags = EFlags::NoWindow, milliseconds timeout = _DefTimeout);
        static bool  Execute (WString &commandLine, EFlags flags = EFlags::NoWindow, milliseconds timeout = _DefTimeout);
        static bool  Execute (String &commandLine, INOUT String &output, Mutex* outputGuard = null, milliseconds timeout = _DefTimeout);
        static bool  Execute (WString &commandLine, INOUT String &output, Mutex* outputGuard = null, milliseconds timeout = _DefTimeout);

    private:
        template <typename T>
        bool  _ExecuteAsync (BasicString<T> &commandLine, const Path* currentDir, EFlags flags);
    };

    AE_BIT_OPERATORS( WindowsProcess::EFlags );


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
