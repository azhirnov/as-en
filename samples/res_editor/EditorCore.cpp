// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "base/DataSource/MemStream.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/CStyleCast.h"

#include "vfs/Disk/DiskStaticStorage.h"
#include "vfs/Disk/DiskDynamicStorage.h"

#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptFn.h"

#include "res_editor/EditorCore.h"
#include "res_editor/Scripting/ScriptExe.h"

#define AE_PUBLIC_VERSION   true

AE_DECL_SCRIPT_OBJ( AE::ResEditor::ResEditorAppConfig,  "Config" );


namespace AE::ResEditor
{
    using namespace AE::Threading;


namespace
{
    static ResEditorAppConfig           s_REConfig;
    static Ptr<ResEditorApplication>    s_ResEditApp;

/*
=================================================
    GetAppConfig
=================================================
*/
    static AppV1::AppConfig  GetAppConfig ()
    {
        AppV1::AppConfig    cfg;

        // threading
        {
            cfg.threading.maxThreads    = 1;
            cfg.threading.maxIOThreads  = 1;
            cfg.threading.mask          = { EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::FileIO };
        }

        // graphics
        {
            cfg.graphics.maxFrames = 2;

            cfg.graphics.staging.readStaticSize .fill( 2_Mb );
            cfg.graphics.staging.writeStaticSize.fill( 2_Mb );
            cfg.graphics.staging.maxReadDynamicSize     = 64_Mb;
            cfg.graphics.staging.maxWriteDynamicSize    = 64_Mb;
            cfg.graphics.staging.dynamicBlockSize       = 32_Mb;
            cfg.graphics.staging.vstreamSize            = 16_Mb;

            cfg.graphics.device.appName         = "ResourceEditor";
            cfg.graphics.device.requiredQueues  = EQueueMask::Graphics;
            cfg.graphics.device.optionalQueues  = Default;
            cfg.graphics.device.validation      = EDeviceValidation::Enabled;

            cfg.graphics.swapchain.format       = EPixelFormat::RGBA8_UNorm;

            cfg.graphics.swapchain.usage        = EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
            cfg.graphics.swapchain.options      = EImageOpt::BlitDst;
            cfg.graphics.swapchain.presentMode  = EPresentMode::FIFO;       // vsync
            cfg.graphics.swapchain.minImageCount= 2;
        }

        // window
        {
            cfg.window.title        = "ResourceEditor";
            cfg.window.size         = {1600, 900};
            cfg.window.resizable    = true;
            cfg.window.fullscreen   = false;
        }

        return cfg;
    }

/*
=================================================
    ResEditorAppConfig_VFSPath
=================================================
*/
    static void  ResEditorAppConfig_VFSPath (ResEditorAppConfig &self, const String &path, const String &prefix)
    {
        CHECK_THROW( FileSystem::IsDirectory( path ));

        self.vfsPathes.push_back( FileSystem::ToAbsolute( Path{path} ));
        self.vfsPathPrefixes.push_back( prefix );
    }

/*
=================================================
    ResEditorAppConfig_UIDataDir
=================================================
*/
    static void  ResEditorAppConfig_UIDataDir (ResEditorAppConfig &self, const String &path)
    {
        if ( not FileSystem::IsDirectory( path ))
            CHECK_THROW( FileSystem::CreateDirectories( path ));

        CHECK_THROW( self.uiDataFolder.empty() );
        self.uiDataFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_PipelineSearchDir
=================================================
*/
    static void  ResEditorAppConfig_PipelineSearchDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW( FileSystem::IsDirectory( path ));

        self.pipelineSearchDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
    }

/*
=================================================
    ResEditorAppConfig_ShaderSearchDir
=================================================
*/
    static void  ResEditorAppConfig_ShaderSearchDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW( FileSystem::IsDirectory( path ));

        self.shaderSearchDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
    }

/*
=================================================
    ResEditorAppConfig_ShaderIncludeDir
=================================================
*/
    static void  ResEditorAppConfig_ShaderIncludeDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW( FileSystem::IsDirectory( path ));

        self.shaderIncludeDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
    }

/*
=================================================
    ResEditorAppConfig_ShaderTraceDir
=================================================
*/
    static void  ResEditorAppConfig_ShaderTraceDir (ResEditorAppConfig &self, const String &path)
    {
        if ( not FileSystem::IsDirectory( path ))
            CHECK_THROW( FileSystem::CreateDirectories( path ));

        CHECK_THROW( self.shaderTraceFolder.empty() );
        self.shaderTraceFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_ScreenshotDir
=================================================
*/
    static void  ResEditorAppConfig_ScreenshotDir (ResEditorAppConfig &self, const String &path)
    {
        if ( not FileSystem::IsDirectory( path ))
            CHECK_THROW( FileSystem::CreateDirectories( path ));

        CHECK_THROW( self.screenshotFolder.empty() );
        self.screenshotFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_VideoDir
=================================================
*/
    static void  ResEditorAppConfig_VideoDir (ResEditorAppConfig &self, const String &path)
    {
        if ( not FileSystem::IsDirectory( path ))
            CHECK_THROW( FileSystem::CreateDirectories( path ));

        CHECK_THROW( self.videoFolder.empty() );
        self.videoFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_ScriptDir
=================================================
*/
    static void  ResEditorAppConfig_ScriptDir (ResEditorAppConfig &self, const String &path)
    {
        if ( not FileSystem::IsDirectory( path ))
            CHECK_THROW( FileSystem::CreateDirectories( path ));

        CHECK_THROW( self.scriptFolder.empty() );
        self.scriptFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_SecondaryScriptDir
=================================================
*/
    static void  ResEditorAppConfig_SecondaryScriptDir (ResEditorAppConfig &self, const String &path)
    {
        if ( not FileSystem::IsDirectory( path ))
            CHECK_THROW( FileSystem::CreateDirectories( path ));

        CHECK_THROW( self.scriptSecondaryFolder.empty() );
        self.scriptSecondaryFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    _LoadResEditorAppConfigFromScript
=================================================
*/
    ND_ static bool  _LoadResEditorAppConfigFromScript (const Path &filename) __Th___
    {
        using namespace AE::Scripting;

        ScriptEnginePtr     se = MakeRC<ScriptEngine>();
        CHECK_THROW( se->Create() );

        CoreBindings::BindString( se );
        CoreBindings::BindArray( se );
        {
            ClassBinder<ResEditorAppConfig>     binder{ se };
            binder.CreateClassValue();
            binder.AddMethodFromGlobal( &ResEditorAppConfig_VFSPath,            "VFSPath" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_UIDataDir,          "UIDataDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_PipelineSearchDir,  "PipelineSearchDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderSearchDir,    "ShaderSearchDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderIncludeDir,   "ShaderIncludeDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ScriptDir,          "ScriptDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_SecondaryScriptDir, "SecondaryScriptDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderTraceDir,     "ShaderTraceDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ScreenshotDir,      "ScreenshotDir" );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_VideoDir,           "VideoDir" );
        }

        FileRStream     file {filename};
        CHECK_ERR( file.IsOpen() );

        ScriptEngine::ModuleSource  src;
        src.name            = ToString( filename.filename().replace_extension("") );
        src.dbgLocation     = {};
        src.usePreprocessor = false;
        CHECK_ERR( file.Read( file.RemainingSize(), OUT src.script ));

        ScriptModulePtr     module = se->CreateModule( {src} );
        CHECK_ERR( module );

        auto    fn = se->CreateScript< void (ResEditorAppConfig &) >( "main", module );
        CHECK_ERR( fn );

        ResEditorAppConfig  tmp;
        CHECK_ERR( fn->Run( OUT tmp ));

        s_REConfig = RVRef(tmp);
        return true;
    }

/*
=================================================
    _CreateDefaultResEditorAppConfig
=================================================
*/
    ND_ static bool  _CreateDefaultResEditorAppConfig (const Path &filename, const bool isRelease)
    {
        String  str;
        if ( isRelease )
        {
            str = R"(
void main (Config &out cfg)
{
    const string    vfs_path            = "";
    const string    local_path          = "data/";
    const string    shader_data_path    = "shared_data/";

    // VFS
    cfg.VFSPath( vfs_path + "shadertoy_data",   "shadertoy/" );

    // pipeline dirs
    cfg.PipelineSearchDir( local_path + "pipelines" );

    // shaders
    cfg.ShaderSearchDir( local_path + "shaders" );
    cfg.ShaderIncludeDir( shader_data_path + "shaders" );
    cfg.ShaderIncludeDir( local_path + "shaders" );

    // config
    cfg.ScriptDir( local_path + "scripts" );
    cfg.SecondaryScriptDir( local_path + "scripts2" );  // used in 'RunScript()'

    // output
    cfg.UIDataDir( "ui" );      // imgui
    cfg.ShaderTraceDir( local_path + "../_shader_trace" );
    cfg.ScreenshotDir( local_path + "../_screenshorts" );
    cfg.VideoDir( local_path + "../_video" );
}
)";
        }
        else
        {
            str = R"()";
        }
        FileWStream     file {filename};
        return  file.IsOpen()               and
                file.Write( StringView{str} );
    }

/*
=================================================
    InitResEditorAppConfig
=================================================
*/
    static void  InitResEditorAppConfig () __NE___
    {
        const Path  path        = FileSystem::CurrentPath() / "res_editor.as";
        const bool  is_release  = AE_PUBLIC_VERSION;

        if ( not FileSystem::IsFile( path ))
            CHECK_FATAL( _CreateDefaultResEditorAppConfig( path, is_release ));

        try{
            CHECK_FATAL( _LoadResEditorAppConfigFromScript( path ));

            if ( not is_release )
            {
                s_REConfig.scriptHeaderOutFolder    = AE_SHARED_DATA "/scripts";
                s_REConfig.cppTypesFolder           = AE_LOCAL_DATA_FOLDER "/cpp";
            }
        }
        catch(...) {
            CHECK_FATAL( !"failed to run initial script" );
        }
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    ResEditorAppConfig::Get
=================================================
*/
    ResEditorAppConfig const&  ResEditorAppConfig::Get ()
    {
        return s_REConfig;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ResEditorApplication::ResEditorApplication () __NE___ :
        DefaultAppListener{ GetAppConfig(), Default }
    {
        s_ResEditApp = this;

        // for imgui
        {
            const auto&     ui_path = s_REConfig.uiDataFolder;

            CHECK_FATAL( FileSystem::IsDirectory( ui_path ));
            CHECK_FATAL( FileSystem::SetCurrentPath( ui_path ));    // TODO: use VFS ?
        }
        CHECK_FATAL( FileSystem::IsDirectory( s_REConfig.scriptFolder ));
        CHECK_FATAL( _InitVFS() );
    }

/*
=================================================
    destructor
=================================================
*/
    ResEditorApplication::~ResEditorApplication ()
    {
        s_ResEditApp = null;
    }

/*
=================================================
    _InitVFS
=================================================
*/
    bool  ResEditorApplication::_InitVFS ()
    {
        for (usize i = 0; i < s_REConfig.vfsPathes.size(); ++i)
        {
            const StringView    prefix {s_REConfig.vfsPathPrefixes[i]};
            ASSERT( not prefix.empty() );
            ASSERT( prefix.back() == '/' );

            auto    storage = MakeRC<VFS::DiskStaticStorage>();
            CHECK_ERR( storage->Create( Path{s_REConfig.vfsPathes[i]}, prefix ));
            CHECK_ERR( GetVFS().AddStorage( storage ));
        }

        return true;
    }

/*
=================================================
    OnStart
=================================================
*/
    void  ResEditorApplication::OnStart (IApplication &app) __NE___
    {
        _app = &app;
        DefaultAppListener::OnStart( app );

        _impl = MakeRC<ResEditorCore>();

        // create window
        {
            auto    wnd = app.CreateWindow( MakeUnique<AppV1::DefaultIWndListener>( _Core().GetRC(), *this ), _config.window );
            CHECK_FATAL( wnd );
            _windows.emplace_back( RVRef(wnd) );
        }
    }

/*
=================================================
    OnStop
=================================================
*/
    void  ResEditorApplication::OnStop (IApplication &app) __NE___
    {
        DefaultAppListener::OnStop( app );
        _app = null;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ResEditorCore::ResEditorCore () :
        _ui{ *this, s_REConfig.scriptFolder }
    {
        ScriptExe::Config   cfg;
        cfg.cppTypesFolder          = s_REConfig.cppTypesFolder;
        cfg.scriptHeaderOutFolder   = s_REConfig.scriptHeaderOutFolder;

        _script.reset( new ScriptExe{ RVRef(cfg) });    // throw

        CHECK_FATAL( _LoadInputActions() );

        //Unused( _rdc.Initialize() );
    }

/*
=================================================
    destructor
=================================================
*/
    ResEditorCore::~ResEditorCore ()
    {
        _mainLoop.Write( Default );
    }

/*
=================================================
    _LoadInputActions
=================================================
*/
    bool  ResEditorCore::_LoadInputActions ()
    {
        // load input actions
        _inputActionsData = MakeRC<MemRStream>();

        FileRStream     file {Path{"controls.bin"}};        // TODO: use VFS
        CHECK_ERR( file.IsOpen() );
        CHECK_ERR( _inputActionsData->LoadRemaining( file ));

        return true;
    }

/*
=================================================
    OnSurfaceCreated
=================================================
*/
    bool  ResEditorCore::OnSurfaceCreated (IOutputSurface &output) __NE___
    {
        return _ui.Init( output );
    }

/*
=================================================
    InitInputActions
=================================================
*/
    void  ResEditorCore::InitInputActions (IInputActions &ia) __NE___
    {
        MemRefRStream   stream{ _inputActionsData->GetData() };

        CHECK( ia.LoadSerialized( stream ));

        CHECK( ia.SetMode( InputModeName{"Main.UI"} ));
    }

/*
=================================================
    StartRendering
=================================================
*/
    void  ResEditorCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output) __NE___
    {
        ASSERT( bool{input} == bool{output} );

        if ( output != null and not output->IsInitialized() )
            return;

        auto    main_loop   = _mainLoop.WriteNoLock();
        EXLOCK( main_loop );

        main_loop->input    = input;
        main_loop->output   = output;
    }

/*
=================================================
    StopRendering
=================================================
*/
    void  ResEditorCore::StopRendering () __NE___
    {
        auto    main_loop   = _mainLoop.WriteNoLock();
        EXLOCK( main_loop );

        main_loop->input    = null;
        main_loop->output   = null;
    }

/*
=================================================
    SurfaceDestroyed
=================================================
*/
    void  ResEditorCore::SurfaceDestroyed () __NE___
    {
        _mainLoop.Write( Default );
    }

/*
=================================================
    RunRenderScriptAsync
----
    should be used in background thread
=================================================
*/
    bool  ResEditorCore::RunRenderScriptAsync (const Path &scriptPath)
    {
        ScriptExe::ScriptConfig cfg;
        cfg.dynSize         = UIInteraction::Instance().GetDynamicSize();
        cfg.shaderDirs      = s_REConfig.shaderSearchDirs;
        cfg.includeDirs     = s_REConfig.shaderIncludeDirs;
        cfg.pipelineDirs    = s_REConfig.pipelineSearchDirs;
        cfg.scriptDir       = s_REConfig.scriptSecondaryFolder;

        auto    input   = _mainLoop.ConstPtr()->input;
        auto    output  = _mainLoop.ConstPtr()->output;
        if ( output )
        {
            auto    sizes = output->GetTargetSizes();
            CHECK_ERR( sizes.size() == 1 );
            cfg.dynSize->Resize( sizes[0] );
        }

        auto    renderer = _script->Run( scriptPath, cfg );
        CHECK_ERR( renderer );

        Scheduler().Run< AsyncTaskFn >(
            Tuple{  [this, renderer] () { _mainLoop->renderer = renderer; },
                    "StartRendering",
                    ETaskQueue::Main });
        return true;
    }

/*
=================================================
    _ProcessInput
=================================================
*/
    CoroTask  ResEditorCore::_ProcessInput (TsInputActions input, RC<Renderer> renderer, Ptr<EditorUI> ui, ActionQueueReader reader)
    {
        if ( ui )
        {
            bool    switch_mode = false;
            ui->ProcessInput( reader, OUT switch_mode );

            if_unlikely( switch_mode and renderer and renderer->GetInputMode().IsDefined() )
            {
                Scheduler().Run( ETaskQueue::Main,
                                 _SetInputMode( input.Unsafe(), renderer->GetInputMode() ),
                                 Tuple{}, "Core::SetInputMode" );
            }
        }

        if ( renderer )
        {
            bool    switch_mode = false;
            renderer->ProcessInput( reader, OUT switch_mode );

            if_unlikely( switch_mode )
            {
                Scheduler().Run( ETaskQueue::Main,
                                 _SetInputMode( input.Unsafe(), InputModeName{"Main.UI"} ),
                                 Tuple{}, "Core::SetInputMode" );
            }
        }

        co_return;
    }

/*
=================================================
    _SetInputMode
=================================================
*/
    CoroTask  ResEditorCore::_SetInputMode (Ptr<IInputActions> input, InputModeName mode)
    {
        CHECK( input->SetMode( mode ));
        co_return;
    }

/*
=================================================
    _DrawFrame
=================================================
*/
    AsyncTask  ResEditorCore::_DrawFrame ()
    {
        Ptr<IInputActions>      input;
        Ptr<IOutputSurface>     output;
        RC<Renderer>            renderer;
        {
            auto    main_loop = _mainLoop.ReadNoLock();
            SHAREDLOCK( main_loop );

            input       = main_loop->input;
            output      = main_loop->output;
            renderer    = output != null ? main_loop->renderer : null;
        }

        auto&           rg          = FrameGraph();
        Ptr<EditorUI>   ui          = output and _ui.IsInitialized() ? &_ui : null;
        AsyncTask       proc_input;

        if ( input )
        {
            ActionQueueReader reader = input->ReadInput( rg.GetPrevFrameId() );

            proc_input = Scheduler().Run( ETaskQueue::PerFrame,
                                          _ProcessInput( input, renderer, ui, reader ),
                                          Tuple{}, "Core::ProcessInput" );
        }

        // rendering depends on input processing
        if ( renderer or ui )
        {
            AsyncTask   begin_frame = rg.BeginFrame( output );

            AsyncTask   draw_task   = renderer  ? renderer->Execute({ proc_input, begin_frame })    : null;
            AsyncTask   ui_task     = ui        ? ui->Draw({ proc_input, begin_frame })             : null;

            AsyncTask   end_frame   = rg.EndFrame( Tuple{ draw_task, ui_task });

            if ( input )
                input->NextFrame( rg.GetNextFrameId() );

            return end_frame;
        }
        else
            return proc_input;
    }

/*
=================================================
    RenderFrame
=================================================
*/
    void  ResEditorCore::RenderFrame () __NE___
    {
        /*if ( _ui.IsCaptureRequested() )
        {
            _enableCapture = true;
            CHECK( _rdc.BeginFrame() );
        }*/

        _mainLoop->endFrame = _DrawFrame();
    }

/*
=================================================
    WaitFrame
=================================================
*/
    void  ResEditorCore::WaitFrame (const Threading::EThreadArray &threads) __NE___
    {
        AsyncTask   task;
        std::swap( task, _mainLoop->endFrame );

        for (;;)
        {
            if ( task == null or task->IsFinished() )
                break;

            Scheduler().ProcessTasks( threads, 0 );

            Scheduler().DbgDetectDeadlock();
        }

        /*if ( _enableCapture )
        {
            CHECK( _rdc.EndFrame() );
            _enableCapture = false;
        }*/
    }

} // AE::ResEditor
//-----------------------------------------------------------------------------


using namespace AE;
using namespace AE::Base;
using namespace AE::App;
using namespace AE::ResEditor;

//#define REQUIRE_APACHE_2
#define REQUIRE_GPLv3
#include "base/Defines/DetectLicense.inl.h"
#include "base/Algorithms/StringUtils.h"


/*
=================================================
    AE_OnAppCreated
=================================================
*/
Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
#if AE_PUBLIC_VERSION
    StaticLogger::Initialize();
    StaticLogger::AddLogger( ILogger::CreateIDEOutput() );
    StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );
    {
        ILogger::LevelBits  level_bits  {~0u};
        level_bits[ uint(ELogLevel::Debug) ]        = false;
        level_bits[ uint(ELogLevel::Info)  ]        = false;
        level_bits[ uint(ELogLevel::SilentError)  ] = false;

        ILogger::ScopeBits  scope_bits  {0};
        scope_bits[ uint(ELogScope::Client)  ]      = true;

        StaticLogger::AddLogger( ILogger::CreateDialogOutput( level_bits, scope_bits ));
    }
#else
    StaticLogger::InitDefault();
#endif

    AE_LOG_DBG( "License: "s << AE_LICENSE );

    InitResEditorAppConfig();

    return MakeUnique<ResEditorApplication>();
}

/*
=================================================
    AE_OnAppDestroyed
=================================================
*/
void  AE_OnAppDestroyed ()
{
    StaticLogger::Deinitialize( false );    // TODO: fix memleak in glslang
}
