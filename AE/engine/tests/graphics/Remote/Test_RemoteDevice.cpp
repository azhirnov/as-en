// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/RDevice.h"
# include "graphics/Remote/RSwapchain.h"
# include "platform/Public/IWindow.h"
# include "platform/Public/IApplication.h"
# include "../UnitTest_Common.h"

using namespace AE::Networking;
using namespace AE::Threading;
using namespace AE::App;


extern void Test_RemoteDevice (IApplication &, IWindow &wnd)
{
    #if 0
    {
        // TODO: start remote server

        RDeviceInitializer  dev;

        RDeviceInitializer::InstanceCreateInfo  inst_ci;
        inst_ci.appName         = "TestApp";
        inst_ci.serverProvider  = MakeRC<DefaultServerProviderV1>( IpAddress::FromLocalPortTCP( 3000 ));

        CHECK_FATAL( dev.CreateInstance( inst_ci ));
        CHECK_FATAL( dev.CreateDefaultQueue() );
        CHECK_FATAL( dev.CreateLogicalDevice() );

        CHECK_FATAL( dev.IsInitialized() );
        CHECK_FATAL( dev.CheckConstantLimits() );
        CHECK_FATAL( dev.CheckExtensions() );

        CHECK_FATAL( dev.DestroyLogicalDevice() );
        CHECK_FATAL( dev.DestroyInstance() );
        CHECK_FATAL( not dev.IsInitialized() );
    }
    #else
    {
        // TODO: start remote server

        RDeviceInitializer      dev {True{}};
        GraphicsCreateInfo      info;
        RSwapchainInitializer   swapchain;

        info.staging.readStaticSize .fill( 2_Mb );
        info.staging.writeStaticSize.fill( 2_Mb );

        info.device.appName         = "TestApp";
        info.device.requiredQueues  = EQueueMask::Graphics;
        info.device.optionalQueues  = Default; //EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer;
        info.device.validation      = EDeviceValidation::Enabled;
    //  info.device.devFlags        = EDeviceFlags::SetStableClock;

        info.swapchain.colorFormat  = EPixelFormat::RGBA8_UNorm;
        info.swapchain.usage        = EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
        info.swapchain.options      = EImageOpt::BlitDst;
        info.swapchain.presentMode  = EPresentMode::FIFO;
        info.swapchain.minImageCount= 2;

        CHECK_FATAL( dev.Init( info,
                               MakeRC<DefaultServerProviderV1>( IpAddress::FromLocalPortTCP( 3000 )),
                               EThreadArray{ EThread::Main }
                              ));

        CHECK_FATAL( dev.IsInitialized() );
        CHECK_FATAL( dev.CheckConstantLimits() );
        CHECK_FATAL( dev.CheckExtensions() );

        RenderTaskScheduler::InstanceCtor::Create( dev );
        CHECK_FATAL( GraphicsScheduler().Initialize( info ));

        CHECK_FATAL( swapchain.CreateSurface( wnd.GetNative() ));
        CHECK_FATAL( swapchain.Create( uint2{1024, 768}, info.swapchain ));

        swapchain.Destroy();
        swapchain.DestroySurface();

        RenderTaskScheduler::InstanceCtor::Destroy();

        CHECK_FATAL( dev.DestroyLogicalDevice() );
        CHECK_FATAL( dev.DestroyInstance() );
        CHECK_FATAL( not dev.IsInitialized() );
    }
    #endif

    TEST_PASSED();
}

#endif // AE_ENABLE_REMOTE_GRAPHICS
