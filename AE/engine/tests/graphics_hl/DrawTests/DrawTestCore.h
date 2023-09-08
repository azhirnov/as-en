// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "platform/Public/IWindow.h"
#include "platform/Public/IApplication.h"

#include "graphics_hl/Canvas/Canvas.h"

#include "graphics_test/GraphicsTest.h"

using namespace AE;
using namespace AE::Graphics;

#ifdef AE_ENABLE_VULKAN
# include "VulkanSyncLog.h"
#endif

using EStatus = AE::Threading::IAsyncTask::EStatus;

using ImageComparator = GraphicsTest::ImageComparator;


class DrawTestCore
{
// types
protected:
    using TestFunc_t    = bool (DrawTestCore::*) ();
    using TestQueue_t   = Deque< TestFunc_t >;

    static constexpr bool   UpdateAllReferenceDumps = true;


// variables
protected:
    Unique<Canvas>              _canvas;
    RenderTechPipelinesPtr      _canvasPpln;

    TestQueue_t                 _tests;
    uint                        _testsPassed        = 0;
    uint                        _testsFailed        = 0;

    Path                        _refDumpPath;

  #if defined(AE_ENABLE_VULKAN)
    VDeviceInitializer          _vulkan;
    VSwapchainInitializer       _swapchain;
    VulkanSyncLog               _syncLog;

  #elif defined(AE_ENABLE_METAL)
    MDeviceInitializer          _metal;
    MSwapchainInitializer       _swapchain;

  #else
  # error not implemented
  #endif


// methods
public:
    DrawTestCore ();
    ~DrawTestCore () {}

    bool  Run (AE::App::IApplication &app, AE::App::IWindow &wnd);

    static bool  SaveImage (StringView name, const ImageMemView &view);

protected:
    ND_ Unique<ImageComparator>  _LoadReference (StringView filename) const;

    ND_ bool  _Create (AE::App::IApplication &app, AE::App::IWindow &wnd);
    ND_ bool  _RunTests ();
        void  _Destroy ();

    ND_ bool  _CompilePipelines ();
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
