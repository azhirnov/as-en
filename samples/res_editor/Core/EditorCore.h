// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Core/EditorUI.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{

    struct ResEditorAppConfig
    {
        // VFS
        Array<Path>     vfsPathes;
        Array<String>   vfsPathPrefixes;

        // UI
        Path            uiDataFolder;

        // pipelines
        Array<Path>     pipelineSearchDirs;

        // shaders
        Array<Path>     shaderSearchDirs;
        Array<Path>     shaderIncludeDirs;

        // script config
        Path            scriptFolder;
        Path            scriptSecondaryFolder;
        Path            cppTypesFolder;
        Path            scriptHeaderOutFolder;

        // output
        Path            shaderTraceFolder;
        Path            screenshotFolder;
        Path            videoFolder;

        // graphics settings
        bool            setStableGPUClock   = false;


        ResEditorAppConfig ()                                   = default;
        ResEditorAppConfig (ResEditorAppConfig &&)              = default;
        ResEditorAppConfig (const ResEditorAppConfig &)         = default;
        ResEditorAppConfig&  operator = (ResEditorAppConfig &&) = default;

        ND_ static ResEditorAppConfig const&  Get ();
    };



    //
    // Resource Editor Application
    //

    class ResEditorApplication final : public AppV1::DefaultAppListener
    {
    // variables
    private:
        Ptr<IApplication>   _app;


    // methods
    public:
        ResEditorApplication ()                                 __NE___;
        ~ResEditorApplication ()                                __NE_OV;

        void  OnStart (IApplication &)                          __NE_OV;
        void  OnStop  (IApplication &)                          __NE_OV;

        ND_ auto  GetApp ()                                     C_NE___ { return _app; }

    private:
        bool  _InitVFS ();

        ND_ ResEditorCore&  _Core ()                            __NE___ { return *Cast<ResEditorCore>( _impl.get() ); }
    };



    //
    // Resource Editor Core
    //

    class ResEditorCore final : public AppV1::IBaseApp
    {
        friend class ResEditorApplication;

    // types
    private:
        class ProcessInputTask;

        struct MainLoopData
        {
            Ptr<IInputActions>      input;      // lifetime is same as Window/VRDevice lifetime
            Ptr<IOutputSurface>     output;     // lifetime is same as Window/VRDevice lifetime
            RC<Renderer>            renderer;
            AsyncTask               endFrame;
        };
        using MainLoopDataSync = Synchronized< SharedMutex, MainLoopData >;


    // variables
    private:
        MainLoopDataSync            _mainLoop;

        EditorUI                    _ui;
        Unique<ScriptExe>           _script;

        RC<MemRStream>              _inputActionsData;

        Unique<RenderGraphImpl>     _rg;

        //App::RenderDocApi         _rdc;
        //bool                      _enableCapture  = false;


    // methods
    public:
        ResEditorCore ();
        ~ResEditorCore ();


    // API for EditorUI
    public:
        ND_ bool  RunRenderScriptAsync (const Path &);


    // API for ResEditorWindow
    public:
        ND_ bool  OnStart ();
    private:
        ND_ bool  _LoadInputActions ();


    // main loop
    private:
        ND_ static CoroTask     _ProcessInput (TsInputActions input, RC<Renderer> renderer, Ptr<EditorUI> ui, ActionQueueReader reader);
        ND_ static CoroTask     _SetInputMode (Ptr<IInputActions> input, InputModeName mode);
        ND_ AsyncTask           _DrawFrame ();


    // IBaseApp //
    private:
        bool  OnSurfaceCreated (IOutputSurface &)                                   __NE_OV;
        void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>, EWndState)   __NE_OV;
        void  StopRendering (Ptr<IOutputSurface>)                                   __NE_OV;
        void  WaitFrame (const Threading::EThreadArray &)                           __NE_OV;
        void  RenderFrame ()                                                        __NE_OV;

        void  _InitInputActions (IInputActions &)                                   __NE___;
    };


} // AE::ResEditor
