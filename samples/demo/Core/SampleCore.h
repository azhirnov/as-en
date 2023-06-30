// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "demo/Common.h"
#include "base/DataSource/MemStream.h"
#include "demo/Core/ISample.h"
#include "demo/Core/ImGuiRenderer.h"

namespace AE::Samples::Demo
{

    //
    // Sample Application
    //

    class SampleApplication final : public AppV1::DefaultAppListener
    {
    // methods
    public:
        SampleApplication ();

        void  OnStart (IApplication &)      __NE_OV;

    private:
        ND_ bool  _InitVFS (IApplication &);
    };



    //
    // Sample Core
    //

    class SampleCore final : public AppV1::IBaseApp
    {
        friend class SampleVRListener;

    // types
    private:
        class UIDrawTask;

        struct MainLoopData
        {
            Ptr<IInputActions>      input;      // lifetime is same as Window/VRDevice lifetime
            Ptr<IOutputSurface>     output;     // lifetime is same as Window/VRDevice lifetime
            AsyncTask               endFrame;
        };
        using MainLoopSync_t    = Synchronized< RWSpinLock, MainLoopData >;


    // variables
    private:
        Strong<PipelinePackID>      _pplnPack;
        MainLoopSync_t              _mainLoop;
        RC<ISample>                 _sample;

        RC<MemRStream>              _inputActionsData;

        Atomic<bool>                _initialized {false};


    // methods
    public:
        SampleCore ();
        ~SampleCore ();

        ND_ bool  LoadInputActions ();

    private:
        ND_ bool  _CompileBaseResources (IOutputSurface &);
        ND_ bool  _CompileResources (IOutputSurface &);

            bool  _InitUI ();

        ND_ AsyncTask  _DrawFrame ();


    // IBaseApp //
    private:
        bool  OnSurfaceCreated (IOutputSurface &)                       __NE_OV;
        void  InitInputActions (IInputActions &)                        __NE_OV;
        void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>)  __NE_OV;
        void  StopRendering ()                                          __NE_OV;
        void  SurfaceDestroyed ()                                       __NE_OV;
        void  WaitFrame (const Threading::EThreadArray &)               __NE_OV;
        void  RenderFrame ()                                            __NE_OV;
    };


} // AE::Samples::Demo
