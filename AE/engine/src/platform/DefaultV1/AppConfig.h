// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/GraphicsCreateInfo.h"
#include "platform/Public/IWindow.h"
#include "platform/Public/VRDevice.h"

namespace AE::AppV1
{
    using namespace AE::App;


    //
    // Application Config
    //

    struct AppConfig
    {
    // types
    private:
        using GraphicsCreateInfo    = Graphics::GraphicsCreateInfo;
        using VRImageDesc           = IVRDevice::VRImageDesc;
        using EThread               = Threading::EThread;
        using VRDeviceTypes         = FixedArray< IVRDevice::EDeviceType, 4 >;

        struct ThreadConfig : Threading::TaskScheduler::Config
        {
            EnumBitSet<EThread>     mask        {EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::FileIO};
            uint                    maxThreads  = 2;
        };


    // variables
    public:
        GraphicsCreateInfo  graphics;
        ThreadConfig        threading;
        WindowDesc          window;

        bool                enableVR    = false;
        VRDeviceTypes       vrDevices;
        VRImageDesc         vr;
    };


} // AE::AppV1
