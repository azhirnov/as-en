// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/StringParser.h"
#include "base/Platforms/Platform.h"
#include "base/DataSource/FileStream.h"

#include "DrawTestCore.h"

#include "res_loaders/DDS/DDSImageSaver.h"

#include "threading/TaskSystem/ThreadManager.h"

#include "../UnitTest_Common.h"

using namespace AE::App;
using namespace AE::Threading;


/*
=================================================
    Test_DrawTests
=================================================
*/
extern void Test_DrawTests (IApplication &app, IWindow &wnd)
{
    DrawTestCore    test;
    CHECK_FATAL( test.Run( app, wnd ));

    TEST_PASSED();
}

/*
=================================================
    constructor
=================================================
*/
DrawTestCore::DrawTestCore () :
    #if defined(AE_ENABLE_VULKAN)
        _refDumpPath{ AE_CURRENT_DIR "/Vulkan/ref" },
        _vulkan{ True{"enable info log"} }
    #elif defined(AE_ENABLE_METAL)
        _refDumpPath{ AE_CURRENT_DIR "/Metal/ref" },
        _metal{ True{"enable info log"} }
    #else
    #   error not implemented
    #endif
{
    _tests.emplace_back( &DrawTestCore::Test_Canvas_Rect );
}

/*
=================================================
    Run
=================================================
*/
bool  DrawTestCore::Run (IApplication &app, IWindow &wnd)
{
    CHECK_ERR( _Create( app, wnd ));

    bool    result = _RunTests();

    _Destroy();

    return result;
}

/*
=================================================
    _LoadReference
=================================================
*/
Unique<ImageComparator>  DrawTestCore::_LoadReference (StringView name) const
{
    Unique<ImageComparator> img_cmp{ new ImageComparator{} };

    Path    path{ AE_REF_IMG_PATH };
    FileSystem::CreateDirectories( path );

    path.append( name );
    path.replace_extension( "dds" );

    img_cmp->LoadReference( RVRef(path) );

    return img_cmp;
}

/*
=================================================
    SaveImage
=================================================
*/
bool  DrawTestCore::SaveImage (StringView name, const ImageMemView &view)
{
    using namespace AE::ResLoader;

    Path    path{ AE_CURRENT_DIR };
    path.append( "vulkan/img" );

    FileSystem::CreateDirectories( path );

    path.append( name );
    path.replace_extension( "dds" );

    DDSImageSaver   saver;
    IntermImage     img;    CHECK( img.SetData( view, null ));

    CHECK_ERR( saver.SaveImage( path, img ));
    return true;
}

/*
=================================================
    _CompilePipelines
=================================================
*/
bool  DrawTestCore::_CompilePipelines ()
{
    auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

    {
        auto    file = MakeRC<FileRStream>( AE_RES_PACK );
        CHECK_ERR( file->IsOpen() );

        PipelinePackDesc    desc;
        desc.stream         = file;
        desc.surfaceFormat  = _swapchain.GetDescription().format;

        CHECK_ERR( desc.surfaceFormat != Default );
        CHECK_ERR( res_mngr.InitializeResources( desc ));
    }

    _canvasPpln = res_mngr.LoadRenderTech( Default, RenderTechName{"CanvasDrawTest"}, Default );
    CHECK_ERR( _canvasPpln );

    return true;
}

/*
=================================================
    _GetGraphicsCreateInfo
=================================================
*/
GraphicsCreateInfo  DrawTestCore::_GetGraphicsCreateInfo ()
{
    GraphicsCreateInfo  info;

    info.maxFrames      = 2;
    info.staging.readStaticSize .fill( 2_Mb );
    info.staging.writeStaticSize.fill( 2_Mb );
    info.staging.maxReadDynamicSize     = 16_Mb;
    info.staging.maxWriteDynamicSize    = 16_Mb;
    info.staging.dynamicBlockSize       = 4_Mb;
    info.staging.vstreamSize            = 4_Mb;

    info.useRenderGraph = true;

    return info;
}
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_VULKAN
/*
=================================================
    _Create
=================================================
*/
bool  DrawTestCore::_Create (IApplication &app, IWindow &wnd)
{
    VDeviceInitializer::InstanceCreateInfo  inst_ci;
    inst_ci.appName             = "TestApp";
    inst_ci.instanceLayers      = _vulkan.GetRecommendedInstanceLayers();
    inst_ci.instanceExtensions  = app.GetVulkanInstanceExtensions();
    inst_ci.version             = {1,2};

    CHECK_ERR( _vulkan.CreateInstance( inst_ci ));

    // this is a test and the test should fail for any validation error
    _vulkan.CreateDebugCallback( DefaultDebugMessageSeverity,
                                 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOG_SE(rep.message);  CHECK_FATAL(not rep.isError); });

    CHECK_ERR( _vulkan.ChooseHighPerformanceDevice() );
    CHECK_ERR( _vulkan.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
    CHECK_ERR( _vulkan.CreateLogicalDevice() );
    CHECK_ERR( _vulkan.CheckConstantLimits() );
    CHECK_ERR( _vulkan.CheckExtensions() );

    _refDumpPath /= _vulkan.GetDeviceName();
    FileSystem::CreateDirectories( _refDumpPath );

    {
        FlatHashMap<VkQueue, String>    qnames;
        for (auto& q : _vulkan.GetQueues()) {
            qnames.emplace( q.handle, q.debugName );
        }
        _syncLog.Initialize( INOUT _vulkan.EditDeviceFnTable(), RVRef(qnames) );
    }

    VRenderTaskScheduler::CreateInstance( _vulkan );

    const GraphicsCreateInfo    info = _GetGraphicsCreateInfo();

    auto&   rts = RenderTaskScheduler();
    CHECK_ERR( rts.Initialize( info ));

    CHECK_ERR( _swapchain.CreateSurface( wnd.GetNative() ));
    CHECK_ERR( rts.GetResourceManager().OnSurfaceCreated( _swapchain ));

    VSwapchainInitializer::VSwapchainDesc   swapchain_ci;
    swapchain_ci.viewSize = wnd.GetSurfaceSize();
    CHECK_ERR( _swapchain.Create( swapchain_ci ));

    _canvas.reset( new Canvas{} );

    Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
            EThreadArray{ EThread::PerFrame, EThread::Renderer },
            "render thread"
        }));

    CHECK_ERR( _CompilePipelines() );

    return true;
}

/*
=================================================
    _RunTests
=================================================
*/
bool  DrawTestCore::_RunTests ()
{
    for (;;)
    {
        if ( not _tests.empty() )
        {
            _syncLog.Enable();

            TestFunc_t& func    = _tests.front();
            bool        passed  = (this->*func)();

            _syncLog.Disable();

            _testsPassed += uint(passed);
            _testsFailed += uint(not passed);
            _tests.pop_front();

            Scheduler().ProcessTask( ETaskQueue::Main, 0 );
        }
        else
        {
            AE_LOGI( "Tests passed: " + ToString( _testsPassed ) + ", failed: " + ToString( _testsFailed ));
            break;
        }
    }
    return not _testsFailed;
}

/*
=================================================
    _Destroy
=================================================
*/
void  DrawTestCore::_Destroy ()
{
    _syncLog.Deinitialize( INOUT _vulkan.EditDeviceFnTable() );

    _canvasPpln = null;
    _canvas.reset();

    _swapchain.Destroy();
    _swapchain.DestroySurface();

    VRenderTaskScheduler::DestroyInstance();

    CHECK( _vulkan.DestroyLogicalDevice() );
    CHECK( _vulkan.DestroyInstance() );
}

/*
=================================================
    _CompareDumps
=================================================
*/
bool  DrawTestCore::_CompareDumps (StringView filename) const
{
    Path    fname = _refDumpPath;   fname.append( String{filename} << ".txt" );

    String  right;
    _syncLog.GetLog( OUT right );

    // override dump
    if ( UpdateAllReferenceDumps )
    {
        FileWStream     wfile{ fname };
        CHECK_ERR( wfile.IsOpen() );
        CHECK_ERR( wfile.Write( StringView{right} ));
        return true;
    }

    // read from file
    String  left;
    {
        FileRStream     rfile{ fname };
        CHECK_ERR( rfile.IsOpen() );
        CHECK_ERR( rfile.Read( usize(rfile.Size()), OUT left ));
    }

    return StringParser::CompareLineByLine( left, right,
                [filename] (uint lline, StringView lstr, uint rline, StringView rstr)
                {
                    AE_LOGE( "in: "s << filename << "\n\n"
                                << "line mismatch:" << "\n(" << ToString( lline ) << "): " << lstr
                                << "\n(" << ToString( rline ) << "): " << rstr );
                },
                [filename] () { AE_LOGE( "in: "s << filename << "\n\n" << "sizes of dumps are not equal!" ); }
            );
}

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_METAL
/*
=================================================
    _Create
=================================================
*/
bool  DrawTestCore::_Create (IApplication &, IWindow &wnd)
{
    CHECK_ERR( _metal.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
    CHECK_ERR( _metal.CreateLogicalDevice() );
    CHECK_ERR( _metal.CheckConstantLimits() );
    CHECK_ERR( _metal.CheckExtensions() );

    MRenderTaskScheduler::CreateInstance( _metal );

    const GraphicsCreateInfo    info = _GetGraphicsCreateInfo();

    auto&   rts = RenderTaskScheduler();
    CHECK_ERR( rts.Initialize( info ));

    CHECK_ERR( _swapchain.CreateSurface( wnd.GetNative() ));
    CHECK_ERR( rts.GetResourceManager().OnSurfaceCreated( _swapchain ));

    SwapchainDesc   swapchain_ci;
    CHECK_ERR( _swapchain.Create( wnd.GetSurfaceSize(), swapchain_ci ));

    Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
            EThreadArray{ EThread::PerFrame, EThread::Renderer },
            "render thread"
        }));

    CHECK_ERR( _CompilePipelines() );

    return true;
}

/*
=================================================
    _RunTests
=================================================
*/
bool  DrawTestCore::_RunTests ()
{
    for (;;)
    {
        if ( not _tests.empty() )
        {
            TestFunc_t& func    = _tests.front();
            bool        passed  = (this->*func)();

            _testsPassed += uint(passed);
            _testsFailed += uint(not passed);
            _tests.pop_front();

            Scheduler().ProcessTask( ETaskQueue::Main, 0 );
        }
        else
        {
            AE_LOGI( "Tests passed: " + ToString( _testsPassed ) + ", failed: " + ToString( _testsFailed ));
            break;
        }
    }
    return not _testsFailed;
}

/*
=================================================
    _Destroy
=================================================
*/
void  DrawTestCore::_Destroy ()
{
    _canvasPpln = null;
    _canvas.reset();

    _swapchain.Destroy();
    _swapchain.DestroySurface();

    MRenderTaskScheduler::DestroyInstance();

    CHECK( _metal.DestroyLogicalDevice() );
}

/*
=================================================
    _CompareDumps
=================================================
*/
bool  DrawTestCore::_CompareDumps (StringView) const
{
    return true;    // not supported for Metal
}

#endif // AE_ENABLE_METAL

