// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
#include "base/Platforms/Platform.h"
#include "base/DataSource/FileStream.h"
#include "base/Algorithms/Parser.h"

#include "threading/TaskSystem/ThreadManager.h"

#include "Test_RenderGraph.h"

#include "res_loaders/DDS/DDSImageSaver.h"

#include "../UnitTest_Common.h"

using namespace AE::App;
using namespace AE::Threading;

extern void Test_Image (IResourceManager &resMngr);
extern void Test_Buffer (IResourceManager &resMngr);

static constexpr uint  c_MaxRenderThreads = 3;

/*
=================================================
    constructor
=================================================
*/
RGTest::RGTest () :
    #if defined(AE_ENABLE_VULKAN)
        _refDumpPath{ AE_CURRENT_DIR "/Vulkan/ref" },
        _vulkan{ True{"enable info log"} }

    #elif defined(AE_ENABLE_METAL)
        _refDumpPath{ AE_CURRENT_DIR "/Metal/ref" },
        _metal{ True{"enable info log"} }

    #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        _refDumpPath{ AE_CURRENT_DIR "/Remote/ref" },
        _remote{ True{"enable info log"} }

    #else
    #   error not implemented
    #endif
{
    // too slow
    //_tests.emplace_back( &RGTest::Test_Buffer );
    //_tests.emplace_back( &RGTest::Test_Image );

    _tests.emplace_back( &RGTest::Test_Allocator );
    _tests.emplace_back( &RGTest::Test_FeatureSets );
    _tests.emplace_back( &RGTest::Test_FrameCounter );
    _tests.emplace_back( &RGTest::Test_ImageFormat );

    _tests.emplace_back( &RGTest::Test_UploadStream1 );
    _tests.emplace_back( &RGTest::Test_UploadStream2 );

    _tests.emplace_back( &RGTest::Test_CopyBuffer1 );
    _tests.emplace_back( &RGTest::Test_CopyBuffer2 );
    _tests.emplace_back( &RGTest::Test_CopyImage1 );
    _tests.emplace_back( &RGTest::Test_CopyImage2 );
    _tests.emplace_back( &RGTest::Test_Compute1 );
    _tests.emplace_back( &RGTest::Test_Compute2 );
    _tests.emplace_back( &RGTest::Test_AsyncCompute1 );
    _tests.emplace_back( &RGTest::Test_AsyncCompute2 );
    _tests.emplace_back( &RGTest::Test_AsyncCompute3 );
    _tests.emplace_back( &RGTest::Test_Draw1 );
    _tests.emplace_back( &RGTest::Test_Draw2 );
    _tests.emplace_back( &RGTest::Test_Draw3 );
    _tests.emplace_back( &RGTest::Test_Draw4 );
    _tests.emplace_back( &RGTest::Test_Draw5 );
    _tests.emplace_back( &RGTest::Test_DrawAsync1 );

  #ifndef AE_ENABLE_METAL
    _tests.emplace_back( &RGTest::Test_DrawMesh1 );
    _tests.emplace_back( &RGTest::Test_DrawMesh2 );
    _tests.emplace_back( &RGTest::Test_RayQuery1 );
    _tests.emplace_back( &RGTest::Test_RayTracing1 );
    _tests.emplace_back( &RGTest::Test_RayTracing2 );
    _tests.emplace_back( &RGTest::Test_RayTracing3 );
    _tests.emplace_back( &RGTest::Test_ShadingRate1 );
    _tests.emplace_back( &RGTest::Test_Debugger1 );
    _tests.emplace_back( &RGTest::Test_Debugger2 );
    _tests.emplace_back( &RGTest::Test_Debugger3 );
    _tests.emplace_back( &RGTest::Test_Debugger4 );
    _tests.emplace_back( &RGTest::Test_Debugger5 );
  #endif
}

/*
=================================================
    Test_Image/Buffer
=================================================
*/
bool  RGTest::Test_Image ()
{
    ::Test_Image( RenderTaskScheduler().GetResourceManager() );
    return true;
}

bool  RGTest::Test_Buffer ()
{
    ::Test_Buffer( RenderTaskScheduler().GetResourceManager() );
    return true;
}

/*
=================================================
    _LoadReference
=================================================
*/
Unique<ImageComparator>  RGTest::_LoadReference (StringView name) const
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
bool  RGTest::SaveImage (StringView name, const ImageMemView &view)
{
    using namespace AE::ResLoader;

    Path    path{ AE_CURRENT_DIR };
    path.append( "img" );

    FileSystem::CreateDirectories( path );

    path.append( name );
    path.replace_extension( "dds" );

    DDSImageSaver   saver;
    IntermImage     img;    CHECK( img.SetData( view, null ));

    CHECK_ERR( Cast<IImageSaver>(&saver)->SaveImage( path, img ));
    return true;
}

/*
=================================================
    Run
=================================================
*/
bool  RGTest::Run (IApplication &app, IWindow &wnd)
{
    CHECK_ERR( _Create( app, wnd ));

    bool    result = _RunTests();

    _Destroy();

    return result;
}

/*
=================================================
    _CompilePipelines
=================================================
*/
bool  RGTest::_CompilePipelines ()
{
    auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

    {
        auto    file = MakeRC<FileRStream>( AE_RES_PACK );
        CHECK_ERR( file->IsOpen() );

        PipelinePackDesc    desc;
        desc.stream         = file;
        desc.surfaceFormat  = _swapchain.GetDescription().colorFormat;

        CHECK_ERR( desc.surfaceFormat != Default );
        CHECK_ERR( res_mngr.InitializeResources( desc ));
    }

    _pipelines = res_mngr.LoadRenderTech( Default, RenderTechs::DrawTestRT, Default );
    CHECK_ERR( _pipelines );

    _acPipelines    = res_mngr.LoadRenderTech( Default, RenderTechs::AsyncCompTestRT,   Default );
    _msPipelines    = res_mngr.LoadRenderTech( Default, RenderTechs::DrawMeshesTestRT,  Default );
    _rtPipelines    = res_mngr.LoadRenderTech( Default, RenderTechs::RayTracingTestRT,  Default );
    _rqPipelines    = res_mngr.LoadRenderTech( Default, RenderTechs::RayQueryTestRT,    Default );
    _vrsPipelines   = res_mngr.LoadRenderTech( Default, RenderTechs::VRSTestRT,         Default );

    return true;
}

/*
=================================================
    _GetGraphicsCreateInfo
=================================================
*/
GraphicsCreateInfo  RGTest::_GetGraphicsCreateInfo ()
{
    GraphicsCreateInfo  info;
    info.maxFrames      = 2;
    info.staging.readStaticSize .fill( 2_Mb );
    info.staging.writeStaticSize.fill( 2_Mb );
    info.staging.maxReadDynamicSize     = 16_Mb;
    info.staging.maxWriteDynamicSize    = 16_Mb;
    info.staging.dynamicBlockSize       = 4_Mb;

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
bool  RGTest::_Create (IApplication &app, IWindow &wnd)
{
    {
        VDeviceInitializer::InstanceCreateInfo  inst_ci;
        inst_ci.appName             = "TestApp";
        inst_ci.instanceLayers      = _vulkan.GetRecommendedInstanceLayers();
        inst_ci.instanceExtensions  = app.GetVulkanInstanceExtensions();
        inst_ci.version             = {1,3};

        #if 0
        const VkValidationFeatureEnableEXT  sync_enable_feats  [] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
        const VkValidationFeatureDisableEXT sync_disable_feats [] = { VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT };

        inst_ci.enableValidations   = sync_enable_feats;
        inst_ci.disableValidations  = sync_disable_feats;
        #endif

        CHECK_ERR( _vulkan.CreateInstance( inst_ci ));
    }

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

    for (uint i = 0; i < c_MaxRenderThreads; ++i) {
        Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
                EThreadArray{ EThread::PerFrame, EThread::Renderer },
                "render thread"s << ToString(i)
            }));
    }

    CHECK_ERR( _CompilePipelines() );

    return true;
}

/*
=================================================
    _RunTests
=================================================
*/
bool  RGTest::_RunTests ()
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
void  RGTest::_Destroy ()
{
    _syncLog.Deinitialize( INOUT _vulkan.EditDeviceFnTable() );

    _pipelines      = null;
    _acPipelines    = null;
    _msPipelines    = null;
    _rtPipelines    = null;
    _rqPipelines    = null;
    _vrsPipelines   = null;

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
bool  RGTest::_CompareDumps (StringView filename) const
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

    return Parser::CompareLineByLine( left, right,
                [filename] (uint lline, StringView lstr, uint rline, StringView rstr)
                {
                    AE_LOGE( "in: "s << filename << "\n\n"
                                << "line mismatch:" << "\n(" << ToString( lline ) << "): " << lstr
                                << "\n(" << ToString( rline ) << "): " << rstr );
                },
                [filename] () { AE_LOGE( "in: "s << filename << "\n\n" << "sizes of dumps are not equal!" ); }
            );
}

/*
=================================================
    Test_VulkanRenderGraph
=================================================
*/
extern void Test_VulkanRenderGraph (IApplication &app, IWindow &wnd)
{
    RGTest      test;
    CHECK_FATAL( test.Run( app, wnd ));

    TEST_PASSED();
}

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_METAL
/*
=================================================
    _Create
=================================================
*/
bool  RGTest::_Create (IApplication &, IWindow &wnd)
{
    CHECK_ERR( _metal.ChooseHighPerformanceDevice() );
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

    for (uint i = 0; i < c_MaxRenderThreads; ++i) {
        Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
                EThreadArray{ EThread::PerFrame, EThread::Renderer },
                "render thread"s << ToString(i)
            }));
    }

    CHECK_ERR( _CompilePipelines() );

    return true;
}

/*
=================================================
    _RunTests
=================================================
*/
bool  RGTest::_RunTests ()
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
void  RGTest::_Destroy ()
{
    _pipelines      = null;
    _acPipelines    = null;
    _msPipelines    = null;
    _rtPipelines    = null;
    _rqPipelines    = null;
    _vrsPipelines   = null;

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
bool  RGTest::_CompareDumps (StringView) const
{
    return true;    // not supported for Metal
}

/*
=================================================
    Test_MetalRenderGraph
=================================================
*/
extern void Test_MetalRenderGraph (IApplication &app, IWindow &wnd)
{
    RGTest      test;
    CHECK_FATAL( test.Run( app, wnd ));

    TEST_PASSED();
}

#endif // AE_ENABLE_METAL
