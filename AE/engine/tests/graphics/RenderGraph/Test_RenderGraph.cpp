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
    _device{ True{"enable info log"} }
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
    ::Test_Image( GraphicsScheduler().GetResourceManager() );
    return true;
}

bool  RGTest::Test_Buffer ()
{
    ::Test_Buffer( GraphicsScheduler().GetResourceManager() );
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

    const Path  path = (_refImagePath / name).replace_extension( "dds" );

    RC<RStream> rfile;
    if ( _refImageStorage->Open( OUT rfile, VFS::FileName{path.string()} ))
    {
        img_cmp->LoadReference( RVRef(rfile), path );
    }
    else
    {
        VFS::FileName   fname;
        CHECK_ERR( _refImageStorage->CreateFile( fname, path ));

        RC<WStream>     wfile;
        if ( _refImageStorage->Open( OUT wfile, fname ))
            img_cmp->Reset( RVRef(wfile), path );
    }
    return img_cmp;
}

/*
=================================================
    SaveImage
=================================================
*/
bool  RGTest::SaveImage (StringView name, const ImageMemView &view) const
{
    using namespace AE::ResLoader;

    const Path  path = (_refImagePath / name).replace_extension( "dds" );

    VFS::FileName   fname;
    CHECK_ERR( _refImageStorage->CreateFile( fname, path ));

    RC<WStream>     wfile;
    CHECK_ERR( _refImageStorage->Open( OUT wfile, fname ));

    DDSImageSaver   saver;
    IntermImage     img;    CHECK( img.SetData( view, null ));

    CHECK_ERR( Cast<IImageSaver>(&saver)->SaveImage( *wfile, img, EImageFormat::DDS ));
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

    for (uint i = 0; i < c_MaxRenderThreads; ++i) {
        Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
                EThreadArray{ EThread::PerFrame, EThread::Renderer },
                "render thread"s << ToString(i)
            }));
    }
    CHECK_ERR( _CompilePipelines( app ));

    bool    result = _RunTests();

    _Destroy();

    return result;
}

/*
=================================================
    _Destroy
=================================================
*/
void  RGTest::_Destroy ()
{
  #ifdef AE_ENABLE_VULKAN
    _syncLog.Deinitialize( INOUT _device.EditDeviceFnTable() );
  #endif

    _pipelines      = null;
    _dbgPipelines   = null;
    _acPipelines    = null;
    _msPipelines    = null;
    _rtPipelines    = null;
    _rqPipelines    = null;
    _vrsPipelines   = null;

    _swapchain.Destroy();
    _swapchain.DestroySurface();

    RenderTaskScheduler::InstanceCtor::Destroy();

    CHECK( _device.DestroyLogicalDevice() );
    CHECK( _device.DestroyInstance() );
}

/*
=================================================
    _CompilePipelines
=================================================
*/
bool  RGTest::_CompilePipelines (IApplication &app)
{
    auto&   res_mngr = GraphicsScheduler().GetResourceManager();

    {
    #ifdef AE_PLATFORM_ANDROID
        auto    storage = app.OpenStorage( EAppStorage::Builtin );
        CHECK_ERR( storage );

        RC<RStream> file;
        CHECK_ERR( storage->Open( OUT file, VFS::FileName{AE_RES_PACK} ));
    #else
        RC<RStream> file = MakeRC<FileRStream>( AE_RES_PACK );
        CHECK_ERR( file->IsOpen() );
        Unused( app );
    #endif

        PipelinePackDesc    desc;
        desc.stream         = file;
        desc.surfaceFormat  = _swapchain.GetDescription().colorFormat;

        CHECK_ERR( desc.surfaceFormat != Default );
        CHECK_ERR( res_mngr.InitializeResources( desc ));
    }

    _pipelines = res_mngr.LoadRenderTech( Default, RenderTechs::DrawTestRT, Default );
    CHECK_ERR( _pipelines );

    _dbgPipelines   = res_mngr.LoadRenderTech( Default, RenderTechs::DebugDrawTestRT,   Default );
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

    info.swapchain.colorFormat  = EPixelFormat::RGBA8_UNorm;

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
        inst_ci.instanceLayers      = _device.GetRecommendedInstanceLayers();
        inst_ci.instanceExtensions  = app.GetVulkanInstanceExtensions();
        inst_ci.version             = {1,3};

        #if 0
        const VkValidationFeatureEnableEXT  sync_enable_feats  [] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
        const VkValidationFeatureDisableEXT sync_disable_feats [] = { VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT };

        inst_ci.enableValidations   = sync_enable_feats;
        inst_ci.disableValidations  = sync_disable_feats;
        #endif

        CHECK_ERR( _device.CreateInstance( inst_ci ));
    }

    // this is a test and the test should fail for any validation error
    _device.CreateDebugCallback( DefaultDebugMessageSeverity,
                                 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOG_SE(rep.message);  CHECK_FATAL(not rep.isError); });

    CHECK_ERR( _device.ChooseHighPerformanceDevice() );
    CHECK_ERR( _device.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
    CHECK_ERR( _device.CreateLogicalDevice() );
    CHECK_ERR( _device.CheckConstantLimits() );
    CHECK_ERR( _device.CheckExtensions() );

    {
        FlatHashMap<VkQueue, String>    qnames;
        for (auto& q : _device.GetQueues()) {
            qnames.emplace( q.handle, q.debugName );
        }
        _syncLog.Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
    }

    RenderTaskScheduler::InstanceCtor::Create( _device );

    const GraphicsCreateInfo    info = _GetGraphicsCreateInfo();

    auto&   rts = GraphicsScheduler();
    CHECK_ERR( rts.Initialize( info ));

    CHECK_ERR( _swapchain.CreateSurface( wnd.GetNative() ));
    CHECK_ERR( rts.GetResourceManager().OnSurfaceCreated( _swapchain ));

  #ifdef AE_PLATFORM_ANDROID
    // android tests executed in separate thread and can not access to window size
    CHECK_ERR( _swapchain.Create( uint2{800,600}, info.swapchain ));
  #else
    CHECK_ERR( _swapchain.Create( wnd.GetSurfaceSize(), info.swapchain ));
  #endif

  #ifdef AE_PLATFORM_ANDROID
    _refDumpStorage = app.OpenStorage( EAppStorage::Cache );
    _refDumpPath    = Path{AE_REFDUMP_PATH} / _device.GetDeviceName();
  #else
    _refDumpStorage = VFS::VirtualFileStorageFactory::CreateDynamicFolder( AE_REFDUMP_PATH, Default, True{"createFolder"} );
    _refDumpPath    = _device.GetDeviceName();
  #endif
    CHECK_ERR( _refDumpStorage );

  #ifdef AE_PLATFORM_ANDROID
    _refImageStorage    = app.OpenStorage( EAppStorage::Cache );
    _refImagePath       = Path{AE_REF_IMG_PATH} / _device.GetDeviceName();
  #else
    _refImageStorage    = VFS::VirtualFileStorageFactory::CreateDynamicFolder( AE_REF_IMG_PATH, Default, True{"createFolder"} );
    _refImagePath       = _device.GetDeviceName();
  #endif
    CHECK_ERR( _refImageStorage );

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

            for (; Scheduler().ProcessTask( ETaskQueue::Main, EThreadSeed(0) );) {}
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
    _CompareDumps
=================================================
*/
bool  RGTest::_CompareDumps (StringView filename) const
{
    Path    fname = _refDumpPath;
    fname.append( String{filename} << ".txt" );

    String  right;
    _syncLog.GetLog( OUT right );

    // override dump
    if ( UpdateAllReferenceDumps )
    {
        VFS::FileName   name;
        CHECK_ERR( _refDumpStorage->CreateFile( name, fname ));

        RC<WStream>     wfile;
        CHECK_ERR( _refDumpStorage->Open( OUT wfile, name ));

        CHECK_ERR( wfile->Write( StringView{right} ));
        return true;
    }

    // read from file
    String  left;
    {
        RC<RStream>     rfile;
        CHECK_ERR( _refDumpStorage->Open( OUT rfile, VFS::FileName{fname.string()} ));

        CHECK_ERR( rfile->Read( usize(rfile->Size()), OUT left ));
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
    CHECK_ERR( _device.ChooseHighPerformanceDevice() );
    CHECK_ERR( _device.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
    CHECK_ERR( _device.CreateLogicalDevice() );
    CHECK_ERR( _device.CheckConstantLimits() );
    CHECK_ERR( _device.CheckExtensions() );

    RenderTaskScheduler::InstanceCtor::Create( _device );

    const GraphicsCreateInfo    info = _GetGraphicsCreateInfo();

    auto&   rts = GraphicsScheduler();
    CHECK_ERR( rts.Initialize( info ));

    CHECK_ERR( _swapchain.CreateSurface( wnd.GetNative() ));
    CHECK_ERR( rts.GetResourceManager().OnSurfaceCreated( _swapchain ));
    CHECK_ERR( _swapchain.Create( wnd.GetSurfaceSize(), info.swapchain ));

    _refImageStorage    = VFS::VirtualFileStorageFactory::CreateDynamicFolder( AE_REF_IMG_PATH, Default, True{"createFolder"} );
    _refImagePath       = _device.GetDeviceName();
    CHECK_ERR( _refImageStorage );

    return true;
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
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_REMOTE_GRAPHICS
/*
=================================================
    _Create
=================================================
*/
bool  RGTest::_Create (IApplication &, IWindow &wnd)
{
    using namespace AE::Networking;

    class ServerProvider final : public IServerProvider
    {
        IpAddress   _addr4;

    public:
        ServerProvider (const IpAddress &addr4) __NE___ : _addr4{addr4} {}

        void  GetAddress (EChannel, uint, Bool, OUT IpAddress &addr)    __NE_OV { addr = _addr4; }
        void  GetAddress (EChannel, uint, Bool, OUT IpAddress6 &)       __NE_OV {}
    };

    GraphicsCreateInfo  info = _GetGraphicsCreateInfo();

    info.device.appName         = "TestApp";
    info.device.requiredQueues  = EQueueMask::Graphics;
    info.device.optionalQueues  = EQueueMask::All;
    info.device.validation      = EDeviceValidation::Enabled;

    info.swapchain.colorFormat  = EPixelFormat::RGBA8_UNorm;
    info.swapchain.usage        = EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
    info.swapchain.options      = EImageOpt::BlitDst;
    info.swapchain.presentMode  = EPresentMode::FIFO;
    info.swapchain.minImageCount= 2;

    CHECK_ERR( _device.Init( info,
                             MakeRC<ServerProvider>( IpAddress::FromLocalPortTCP( _serverPort )),
                             EThreadArray{ EThread::Main, EThread::PerFrame, EThread::Renderer }
                            ));

    CHECK_ERR( _device.CheckConstantLimits() );
    CHECK_ERR( _device.CheckExtensions() );

    RenderTaskScheduler::InstanceCtor::Create( _device );

    auto&   rts = GraphicsScheduler();
    CHECK_ERR( rts.Initialize( info ));

    CHECK_ERR( _swapchain.CreateSurface( wnd.GetNative() ));
    CHECK_ERR( rts.GetResourceManager().OnSurfaceCreated( _swapchain ));
    CHECK_ERR( _swapchain.Create( wnd.GetSurfaceSize(), info.swapchain ));

    _refImageStorage    = VFS::VirtualFileStorageFactory::CreateDynamicFolder( AE_REF_IMG_PATH, Default, True{"createFolder"} );
    _refImagePath       = _device.GetDeviceName();
    CHECK_ERR( _refImageStorage );

    return true;
}

/*
=================================================
    Test_RemoteRenderGraph
=================================================
*/
extern void Test_RemoteRenderGraph (IApplication &app, IWindow &wnd)
{
    RGTest      test;
    CHECK_FATAL( test.Run( app, wnd ));

    TEST_PASSED();
}

#endif // AE_ENABLE_REMOTE_GRAPHICS
//-----------------------------------------------------------------------------



#if defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
/*
=================================================
    _CompareDumps
=================================================
*/
bool  RGTest::_CompareDumps (StringView) const
{
    return true;    // not supported for Metal / Remote
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

#endif
