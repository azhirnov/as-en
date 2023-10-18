// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: only main thread
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
        //
        ND_ virtual bool  OnSurfaceCreated (IWindow &)                          __NE___ = 0;


        // Called when window is in foreground (visible) and focused.
        //   'input'  - lifetime is same as Window/VRDevice lifetime.
        //   'output' - lifetime is same as Window/VRDevice lifetime.
        //   'state'  - may be 'InForeground' or 'Focused'.
        //
            virtual void  StartRendering (Ptr<IInputActions>    input,
                                          Ptr<IOutputSurface>   output,
                                          EWndState             state)          __NE___ = 0;


        // Called when window is moved to background or when closed.
        //   'output' - surface of the window which is moved to background.
        //
            virtual void  StopRendering (Ptr<IOutputSurface> output)            __NE___ = 0;


        // Called each frame. Implementation may start rendering here.
        //
            virtual void  RenderFrame ()                                        __NE___ = 0;


        // Called each frame. Implementation must wait until all render tasks,
        // which started in 'RenderFrame()', will complete.
        //   'threadMask'   - thread types which can be passed to 'Scheduler().ProcessTasks()'
        //   'window'       - active window or null.
        //   'vrDevice'     - active VR device or null.
        //
            virtual void  WaitFrame (const Threading::EThreadArray  &threadMask,
                                     Ptr<IWindow>                   window,
                                     Ptr<IVRDevice>                 vrDevice)   __NE___ = 0;
    };


} // AE::AppV1
