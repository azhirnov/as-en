// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Disk/DiskStaticStorage.h"
#include "vfs/Disk/DiskDynamicStorage.h"

#include "res_editor/Core/EditorCore.h"
#include "res_editor/Scripting/ScriptExe.h"

AE_DECL_SCRIPT_OBJ( AE::ResEditor::ResEditorAppConfig,  "Config" );

#if AE_PUBLIC_VERSION and AE_GRAPHICS_STRONG_VALIDATION == 0
# error AE_GRAPHICS_STRONG_VALIDATION must be enabled for public version
#endif

namespace AE::ResEditor
{
    using namespace AE::Threading;

namespace
{
    static ResEditorAppConfig       s_REConfig;
    static constexpr auto           c_WindowMode = EWindowMode::Resizable;

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
            cfg.threading.maxThreads    = 2;
            cfg.threading.maxIOThreads  = 1;
            cfg.threading.mask          = { EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::FileIO };
        }

        // graphics
        {
            cfg.graphics.maxFrames = 2;

            cfg.graphics.staging.readStaticSize .fill( 1_Mb );
            cfg.graphics.staging.writeStaticSize.fill( 2_Mb );
            cfg.graphics.staging.maxReadDynamicSize     = 64_Mb;
            cfg.graphics.staging.maxWriteDynamicSize    = 64_Mb;
            cfg.graphics.staging.dynamicBlockSize       = 32_Mb;
            cfg.graphics.staging.vstreamSize            = 4_Mb;

            cfg.graphics.device.appName         = "ResourceEditor";
            cfg.graphics.device.requiredQueues  = EQueueMask::Graphics;
            cfg.graphics.device.optionalQueues  = Default;
            cfg.graphics.device.devFlags        = (s_REConfig.setStableGPUClock ? EDeviceFlags::SetStableClock : Default) |
                                                  (s_REConfig.enableRenderDoc ? EDeviceFlags::EnableRenderDoc : Default) ;

          #if AE_PUBLIC_VERSION and defined(AE_RELEASE)
            cfg.graphics.device.validation      = EDeviceValidation::Disabled;
          #else
            cfg.graphics.device.validation      = EDeviceValidation::Enabled;
          #endif

            cfg.graphics.swapchain.colorFormat  = EPixelFormat::RGBA8_UNorm;

            cfg.graphics.swapchain.usage        = EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
            cfg.graphics.swapchain.options      = EImageOpt::BlitDst;
            cfg.graphics.swapchain.presentMode  = EPresentMode::FIFO;       // vsync
            cfg.graphics.swapchain.minImageCount= 2;

            cfg.graphics.useRenderGraph         = true;
        }

        // window
        {
            cfg.window.title    = "ResourceEditor";
            cfg.window.size     = {1600, 900};
            cfg.window.mode     = c_WindowMode;
        }

        // VR
        {
            cfg.enableVR        = false;
            cfg.vr.dimension    = {1024, 1024};
            cfg.vr.format       = EPixelFormat::BGRA8_UNorm;
            cfg.vr.usage        = EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer;  // default
            cfg.vr.options      = EImageOpt::BlitDst;
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
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "VFSPath '"s << ToString(path) << "' must be existed folder" );

        self.vfsPaths.push_back( FileSystem::ToAbsolute( Path{path} ));
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
        {
            CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
                "Failed to create folder '"s << ToString(path) << "'" );
        }

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
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "PipelineSearchDir '"s << ToString(path) << "' must be existed folder" );

        self.pipelineSearchDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
    }

/*
=================================================
    ResEditorAppConfig_PipelineIncludeDir
=================================================
*/
    static void  ResEditorAppConfig_PipelineIncludeDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "PipelineIncludeDir '"s << ToString(path) << "' must be existed folder" );

        self.pipelineIncludeDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
    }

/*
=================================================
    ResEditorAppConfig_ShaderSearchDir
=================================================
*/
    static void  ResEditorAppConfig_ShaderSearchDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "ShaderSearchDir '"s << ToString(path) << "' must be existed folder" );

        self.shaderSearchDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
    }

/*
=================================================
    ResEditorAppConfig_ShaderIncludeDir
=================================================
*/
    static void  ResEditorAppConfig_ShaderIncludeDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "ShaderIncludeDir '"s << ToString(path) << "' must be existed folder" );

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
        {
            CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
                "Failed to create folder '"s << ToString(path) << "'" );
        }

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
        {
            CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
                "Failed to create folder '"s << ToString(path) << "'" );
        }

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
        {
            CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
                "Failed to create folder '"s << ToString(path) << "'" );
        }

        CHECK_THROW( self.videoFolder.empty() );
        self.videoFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_ExportDir
=================================================
*/
    static void  ResEditorAppConfig_ExportDir (ResEditorAppConfig &self, const String &path)
    {
        if ( not FileSystem::IsDirectory( path ))
        {
            CHECK_THROW_MSG( FileSystem::CreateDirectories( path ),
                "Failed to create folder '"s << ToString(path) << "'" );
        }

        CHECK_THROW( self.exportFolder.empty() );
        self.exportFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_ScriptDir
=================================================
*/
    static void  ResEditorAppConfig_ScriptDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "ScriptDir '"s << ToString(path) << "' must be existed folder" );

        CHECK_THROW( self.scriptFolder.empty() );
        self.scriptFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_CallableScriptDir
=================================================
*/
    static void  ResEditorAppConfig_CallableScriptDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "CallableScriptDir '"s << ToString(path) << "' must be existed folder" );

        CHECK_THROW( self.scriptCallableFolder.empty() );
        self.scriptCallableFolder = FileSystem::ToAbsolute( Path{path} );
    }

/*
=================================================
    ResEditorAppConfig_AddScriptIncludeDir
=================================================
*/
    static void  ResEditorAppConfig_AddScriptIncludeDir (ResEditorAppConfig &self, const String &path)
    {
        CHECK_THROW_MSG( FileSystem::IsDirectory( path ),
            "ScriptIncludeDir '"s << ToString(path) << "' must be existed folder" );

        self.scriptIncludeDirs.push_back( FileSystem::ToAbsolute( Path{path} ));
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
            binder.AddMethodFromGlobal( &ResEditorAppConfig_VFSPath,                "VFSPath",              {"path", "prefixInVFS"} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_UIDataDir,              "UIDataDir",            {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_PipelineSearchDir,      "PipelineSearchDir",    {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_PipelineIncludeDir,     "PipelineIncludeDir",   {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderSearchDir,        "ShaderSearchDir",      {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderIncludeDir,       "ShaderIncludeDir",     {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ScriptDir,              "ScriptDir",            {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_CallableScriptDir,      "CallableScriptDir",    {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_AddScriptIncludeDir,    "ScriptIncludeDir",     {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ShaderTraceDir,         "ShaderTraceDir",       {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ScreenshotDir,          "ScreenshotDir",        {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_VideoDir,               "VideoDir",             {} );
            binder.AddMethodFromGlobal( &ResEditorAppConfig_ExportDir,              "ExportDir",            {} );
            binder.AddProperty( &ResEditorAppConfig::setStableGPUClock,             "setStableGPUClock"     );
            binder.AddProperty( &ResEditorAppConfig::enableRenderDoc,               "enableRenderDoc"       );
        }

        ScriptEngine::ModuleSource  src;
        {
            FileRStream     file {filename};
            CHECK_ERR( file.IsOpen() );

            src.name            = ToString( filename.filename().replace_extension("") );
            src.dbgLocation     = {};
            src.usePreprocessor = false;
            CHECK_ERR( file.Read( file.RemainingSize(), OUT src.script ));
        }

        ScriptModulePtr     module = se->CreateModule( {src} );
        CHECK_ERR_MSG( module,
            "Failed to parse script '"s << ToString(filename) << "', fix errors or delete the file to allow the application to create a default script" );

        auto    fn = se->CreateScript< void (ResEditorAppConfig &) >( "main", module );
        CHECK_ERR_MSG( fn,
            "Script '"s << ToString(filename) << "' entry point 'main' is not exist" );

        ResEditorAppConfig  tmp;
        CHECK_ERR_MSG( fn->Run( OUT tmp ),
            "Failed to run script '"s << ToString(filename) << "', fix errors or delete the file to allow the application to create a default script" );

        s_REConfig = RVRef(tmp);
        return true;
    }

/*
=================================================
    _CreateDefaultResEditorAppConfig
=================================================
*/
    ND_ static bool  _CreateDefaultResEditorAppConfig (const Path &filename)
    {
        String  str;
        if ( AE_PUBLIC_VERSION )
        {
            str << R"(
void main (Config &out cfg)
{
    const string    vfs_path            = "";
    const string    local_path          = "data/";
    const string    shader_data_path    = "shared_data/";
    const string    ui_path             = "ui";
)";
        }
        else
        {
            Path    engine_path;
            if ( FileSystem::SearchBackward( FileSystem::CurrentPath(), "AE/samples/res_editor", 3, OUT engine_path ))
            {
                ASSERT( engine_path.is_absolute() );
                engine_path = engine_path.parent_path().parent_path().parent_path();
            }

            String  path = engine_path.string();
            FindAndReplace( INOUT path, '\\', '/' );

            if ( not path.empty() and path.back() != '/' )
                path << '/';

            str <<
"void main (Config &out cfg)\n"
"{\n"
"   const string    base_path           = \"" << path  << "\";\n"
"   const string    vfs_path            = base_path + \"AE-Data/\";\n"
"   const string    local_path          = base_path + \"AE/samples/res_editor/_data/\";\n"
"   const string    shader_data_path    = base_path + \"AE/engine/shared_data/\";\n"
"   const string    ui_path             = base_path + \"AE-Temp/samples/res_editor\";\n";
        }

        str << R"(
    // VFS //
    //  attach path on disk to VFS
    cfg.VFSPath( vfs_path + "shadertoy_data",   "shadertoy/" );
    cfg.VFSPath( vfs_path + "res_editor_data",  "res/" );
    cfg.VFSPath( local_path + "../_export",     "export/" );

    // pipeline dirs //
    //  where to search pipelines for 'UnifiedGeometry' and 'Model'.
    cfg.PipelineSearchDir( local_path + "pipelines" );
    cfg.PipelineIncludeDir( local_path + "pipelines/include" );

    // shaders //
    //  where to search shaders for pipelines and passes.
    cfg.ShaderSearchDir( local_path + "shaders" );
    cfg.ShaderIncludeDir( shader_data_path + "shaders" );
    cfg.ShaderIncludeDir( local_path + "shaders" );

    // scripts //
    //  all files with '.as' extension will be added to script list in editor.
    cfg.ScriptDir( local_path + "scripts" );
    //  scripts which can be used directly and for 'RunScript()' call.
    cfg.CallableScriptDir( local_path + "scripts/callable" );
    //  scripts which can be included in other scripts.
    cfg.ScriptIncludeDir( local_path + "script_inc" );

    // output //
    //  path for imgui
    cfg.UIDataDir( ui_path );
    //  where to put shader traces
    cfg.ShaderTraceDir( local_path + "../_shader_trace" );
    //  where to save screenshots
    cfg.ScreenshotDir( local_path + "../_screenshots" );
    //  where to save video
    cfg.VideoDir( local_path + "../_video" );
    //  where to save export (images, models, scenes, etc)
    cfg.ExportDir( local_path + "../_export" );

    // graphics settings //
    //  set stable GPU clock for profiling, otherwise driver can move GPU to low power mode.
    cfg.setStableGPUClock = false;

    //  on start attach RenderDoc to the app, this will disable mesh shader and ray tracing extensions.
    cfg.enableRenderDoc = false;
}
)";

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
        const Path  path = FileSystem::CurrentPath() / "res_editor_cfg.as";

        if ( not FileSystem::IsFile( path ))
            CHECK_FATAL( _CreateDefaultResEditorAppConfig( path ));

        try{
            if ( not _LoadResEditorAppConfigFromScript( path ))
            {
                // recreate
                CHECK_FATAL( _CreateDefaultResEditorAppConfig( path ));
                CHECK_FATAL( _LoadResEditorAppConfigFromScript( path ));
            }

            if ( not AE_PUBLIC_VERSION )
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
        AppCoreV1{ GetAppConfig(), MakeRCTh<ResEditorCore>() }
    {
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
    ResEditorApplication::~ResEditorApplication () __NE___
    {
    }

/*
=================================================
    _InitVFS
=================================================
*/
    bool  ResEditorApplication::_InitVFS ()
    {
        for (usize i = 0; i < s_REConfig.vfsPaths.size(); ++i)
        {
            String&     prefix = s_REConfig.vfsPathPrefixes[i];
            ASSERT( not prefix.empty() );

            if ( prefix.empty() )           continue;
            if ( prefix.back() != '/' )     prefix << '/';

            CHECK_ERR( GetVFS().AddStorage( VFS::VirtualFileStorageFactory::CreateStaticFolder( s_REConfig.vfsPaths[i], prefix )));
        }

        if ( not s_REConfig.exportFolder.empty() )
        {
            CHECK_ERR( GetVFS().AddStorage( VFS::StorageName{"export"},
                                            VFS::VirtualFileStorageFactory::CreateDynamicFolder( s_REConfig.exportFolder, "export/" )));
        }

        GetVFS().MakeImmutable();
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
        AppCoreV1::OnStart( app );

        CHECK_FATAL( Cast<ResEditorCore>(&GetBaseApp())->OnStart() );

        CHECK_FATAL( _OnStartImpl( app ));
    }

/*
=================================================
    OnStop
=================================================
*/
    void  ResEditorApplication::OnStop (IApplication &app) __NE___
    {
        AppCoreV1::OnStop( app );
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
    {}

/*
=================================================
    destructor
=================================================
*/
    ResEditorCore::~ResEditorCore ()
    {
      #ifdef AE_ENABLE_VULKAN
        GraphicsScheduler().GetDevice().GetRenderDocApi().PrintCaptures();
      #endif

        _mainLoop.Write( Default );
    }

/*
=================================================
    OnStart
=================================================
*/
    bool  ResEditorCore::OnStart ()
    {
        NOTHROW_ERR(
            _rg.reset( new RenderGraphImpl{} );

            ScriptExe::Config   cfg;
            cfg.cppTypesFolder          = s_REConfig.cppTypesFolder;
            cfg.scriptHeaderOutFolder   = s_REConfig.scriptHeaderOutFolder;
            cfg.vfsPaths                = s_REConfig.vfsPaths;
            cfg.vfsPathPrefixes         = s_REConfig.vfsPathPrefixes;
            cfg.scriptIncludeDirs       = s_REConfig.scriptIncludeDirs;

            _script.reset( new ScriptExe{ RVRef(cfg) });    // throw
        )
        return  _LoadInputActions();
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
    bool  ResEditorCore::OnSurfaceCreated (IWindow &wnd) __NE___
    {
        _window = &wnd;
        return _ui.Init( wnd.GetSurface(), c_WindowMode );
    }

/*
=================================================
    _InitInputActions
=================================================
*/
    void  ResEditorCore::_InitInputActions (IInputActions &ia) __NE___
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
    void  ResEditorCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output, EWndState state) __NE___
    {
        ASSERT( bool{input} == bool{output} );

        const bool  focused = (state == EWndState::Focused);
        bool        ia_changed;

        {
            auto    main_loop = _mainLoop.WriteNoLock();
            EXLOCK( main_loop );

            if ( not focused and main_loop->output != null )
                return;

            ia_changed = (main_loop->input != input) and (input != null);

            main_loop->input  = input;
            main_loop->output = output;
        }

        if ( ia_changed )
            _InitInputActions( *input );
    }

/*
=================================================
    StopRendering
=================================================
*/
    void  ResEditorCore::StopRendering (Ptr<IOutputSurface> output) __NE___
    {
        auto    main_loop   = _mainLoop.WriteNoLock();
        EXLOCK( main_loop );

        if ( output == null or main_loop->output == output )
            main_loop->output = null;
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
        cfg.scriptDir       = s_REConfig.scriptCallableFolder;
        cfg.monitor         = _monitor.Read();

        auto    output  = _mainLoop.ConstPtr()->output;
        if ( output )
        {
            auto    infos = output->GetTargetInfo();
            CHECK_ERR( infos.size() == 1 );
            cfg.dynSize->Resize( infos[0].dimension );
        }

        auto    renderer = _script->Run( scriptPath, cfg );
        if ( not renderer )
            return false;

        Scheduler().Run< AsyncTaskFn >(
            Tuple{  [this, renderer] ()
                    {
                        _ui.SetHelpText( renderer->GetHelpText() );
                        _ui.SetSurfaceFormat( renderer->GetSurfaceFormat() );
                        _mainLoop->renderer = RVRef(renderer);
                    },
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
    RenderFrame
=================================================
*/
    void  ResEditorCore::RenderFrame () __NE___
    {
        #ifdef AE_ENABLE_VULKAN
        if ( _ui.IsCaptureRequested() )
        {
            auto&   dev = GraphicsScheduler().GetDevice();
            if ( dev.HasRenderDocApi() )
            {
                CHECK( dev.GetRenderDocApi().TriggerFrameCapture() );
            }
        }
        #endif

        Ptr<IInputActions>      input;
        Ptr<IOutputSurface>     output;
        RC<Renderer>            renderer;
        {
            auto    main_loop = _mainLoop.ReadNoLock();
            SHAREDLOCK( main_loop );

            input       = main_loop->input;
            output      = main_loop->output;
            renderer    = main_loop->renderer;

            if ( output == null or not output->IsInitialized() )
                renderer = null;
        }

        auto&           rg          = RenderGraph();
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
            if ( not rg.BeginFrame( output ))
                return;

            AsyncTask   draw_task   = renderer  ? renderer->Execute({ proc_input }) : null;
            AsyncTask   ui_task     = ui        ? ui->Draw({ proc_input })          : null;

            AsyncTask   end_frame   = rg.EndFrame( Tuple{ draw_task, ui_task });

            if ( input )
                input->NextFrame( rg.GetNextFrameId() );
        }
        else
        {
            ThreadUtils::Sleep_15ms();
        }
    }

/*
=================================================
    WaitFrame
=================================================
*/
    void  ResEditorCore::WaitFrame (const Threading::EThreadArray   &threadMask,
                                    Ptr<IWindow>                    window,
                                    Ptr<IVRDevice>                  ) __NE___
    {
        CHECK( GraphicsScheduler().WaitNextFrame( threadMask, AE::DefaultTimeout ));

        if ( window )
        {
            if ( _window != window )
            {
                _window = window;
                _monitor.Write( window->GetMonitor() );
            }

            if ( auto new_mode = _ui.GetNewWindowMode();  new_mode.has_value() )
                Unused( window->SetMode( *new_mode ));
        }
    }

} // AE::ResEditor
//-----------------------------------------------------------------------------


using namespace AE;
using namespace AE::Base;
using namespace AE::App;
using namespace AE::ResEditor;

#define REQUIRE_APACHE_2
#include "base/Defines/DetectLicense.inl.h"


/*
=================================================
    AE_OnAppCreated
=================================================
*/
Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
#if AE_PUBLIC_VERSION
    StaticLogger::Initialize();
    StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );

#  ifdef AE_DEBUG
    StaticLogger::AddLogger( ILogger::CreateIDEOutput() );
    StaticLogger::AddLogger( ILogger::CreateHtmlOutput( "log.html" ));
#  endif
#else
    StaticLogger::InitDefault();
#endif

    AE_LOGI( String{AE_ENGINE_NAME} << ' ' << ToString(AE_VERSION.Get<0>()) << '.' << ToString(AE_VERSION.Get<1>()) << '.' << ToString(AE_VERSION.Get<2>()) );
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
    // Don't check for memleaks because of
    // false positive in glslang when used dynamic allocation in static variable.
    StaticLogger::Deinitialize( false );
}
