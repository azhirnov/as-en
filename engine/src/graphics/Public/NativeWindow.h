// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "graphics/Public/MetalTypes.h"

namespace AE::Graphics
{

    //
    // Native Window
    //

    struct NativeWindow
    {
        #if defined(AE_PLATFORM_WINDOWS)
            void*           hinstance       = null;     // HINSTANCE
            void*           hwnd            = null;     // HWND

        #elif defined(AE_PLATFORM_ANDROID)
            void*           nativeWindow    = null;     // ANativeWindow

        #elif defined(AE_PLATFORM_LINUX)
            void*           window          = null;     // X11 Window
            void*           display         = null;     // X11 Display*

        #elif defined(AE_PLATFORM_APPLE)
            MetalCALayerRC  layer;                      // CAMetalLayer*

        #else
        #   error Unsupported platform!
        #endif
    };


} // AE::Graphics
