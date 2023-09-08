// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: only main thread
*/

#pragma once

#include "platform/DefaultV1/AppConfig.h"

namespace AE::AppV1
{

    //
    // Base Application interface
    //

    class IBaseApp : public EnableRC< IBaseApp >
    {
    // types
    protected:
        using EWndState = IWindow::EState;


    // interface
    public:
        // Called when surface is created, you can load related resources here.
        ND_ virtual bool  OnSurfaceCreated (IOutputSurface &)                                       __NE___ = 0;


        // Called when window is in foreground (visible) and focused.
        //   'state' - may be 'InForeground' or 'Focused'.
        //
            virtual void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>, EWndState state) __NE___ = 0;


        // Called when window is moved to background or when closed.
        //   'output' - surface of the window which is moved to background.
        //
            virtual void  StopRendering (Ptr<IOutputSurface> output)                                __NE___ = 0;


            virtual void  WaitFrame (const Threading::EThreadArray &)                               __NE___ = 0;
            virtual void  RenderFrame ()                                                            __NE___ = 0;
    };


} // AE::AppV1
