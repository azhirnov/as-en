// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "platform/Public/IWindow.h"
#include "platform/Public/IApplication.h"

#include "graphics_hl/Canvas/Canvas.h"

#include "graphics_test/GraphicsTest.h"

using namespace AE;
using namespace AE::Threading;
using namespace AE::Graphics;

#ifdef AE_ENABLE_VULKAN
# include "VulkanSyncLog.h"
#endif

using EStatus = AE::Threading::IAsyncTask::EStatus;

using ImageComparator = GraphicsTest::ImageComparator;

static constexpr seconds    c_MaxTimeout    {100};
static const EThreadArray   c_ThreadArr     {EThread::PerFrame, EThread::Renderer};


class DrawTestCore
{
// types
protected:
    using TestFunc_t    = bool (DrawTestCore::*) ();
    using TestQueue_t   = Deque< TestFunc_t >;
    using FStorage_t    = RC<AE::VFS::IVirtualFileStorage>;

    static constexpr bool   UpdateAllReferenceDumps = true;


// variables
protected:
    Unique<Canvas>              _canvas;
    RenderTechPipelinesPtr      _canvasPpln;

    TestQueue_t                 _tests;
    uint                        _testsPassed        = 0;
    uint                        _testsFailed        = 0;

    FStorage_t                  _refImageStorage;
    Path                        _refImagePath;

  #if defined(AE_ENABLE_VULKAN)
    FStorage_t                  _refDumpStorage;
    Path                        _refDumpPath;
    VDeviceInitializer          _device;
    VSwapchainInitializer       _swapchain;
    VulkanSyncLog               _syncLog;

  #elif defined(AE_ENABLE_METAL)
    MDeviceInitializer          _device;
    MSwapchainInitializer       _swapchain;

  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
    RDeviceInitializer          _device;
    RSwapchainInitializer       _swapchain;
    const ushort                _serverPort     = 3000;

  #else
  # error not implemented
  #endif


// methods
public:
    DrawTestCore ();
    ~DrawTestCore () {}

    bool  Run (AE::App::IApplication &app, AE::App::IWindow &wnd);

    bool  SaveImage (StringView name, const ImageMemView &view) const;

protected:
    ND_ Unique<ImageComparator>  _LoadReference (StringView filename) const;

    ND_ bool  _Create (AE::App::IApplication &app, AE::App::IWindow &wnd);
    ND_ bool  _RunTests ();
        void  _Destroy ();

    ND_ bool  _CompilePipelines (AE::App::IApplication &app);
    ND_ bool  _CompareDumps (StringView filename) const;

    ND_ static GraphicsCreateInfo  _GetGraphicsCreateInfo ();

private:
    bool  Test_Canvas_Rect ();
};



ND_ inline String  _GetFuncName (StringView src)
{
    usize   pos = src.find_last_of( "::" );

    if ( pos != StringView::npos )
        return String{ src.substr( pos+1 )};
    else
        return String{ src };
}


# define TEST_NAME          _GetFuncName( AE_FUNCTION_NAME )

# define RG_CHECK( ... )    { bool res = (__VA_ARGS__);  CHECK_MSG( res, AE_TOSTRING( __VA_ARGS__ ));  result &= res; }
