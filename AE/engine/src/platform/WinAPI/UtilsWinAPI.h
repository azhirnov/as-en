// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

    //
    // Utils
    //

    struct UtilsWinAPI
    {
        static void  SetMonitorNames (INOUT ApplicationBase::Monitors_t &monitors)  __NE___;
    };


} // AE::App

#endif // AE_PLATFORM_WINDOWS
