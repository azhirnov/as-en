// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/OutputSurface.h"
#include "platform/Public/IWindow.h"

#if defined(AE_ENABLE_VULKAN)
#   include "graphics/Vulkan/VSwapchain.h"
#elif defined(AE_ENABLE_METAL)
#   include "graphics/Metal/MSwapchain.h"
#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#   include "graphics/Remote/RSwapchain.h"
#else
#   error not implemented
#endif

namespace AE::App
{

    //
    // Window Output Surface
    //

    class WindowSurface final : public IOutputSurface
    {
    // types
    private:
        class AcquireNextImageTask;
        class PresentImageTask;
        class RecreateSwapchainTask;

      #if defined(AE_ENABLE_VULKAN)
        using Swapchain_t = Graphics::VSwapchainInitializer;
      #elif defined(AE_ENABLE_METAL)
        using Swapchain_t = Graphics::MSwapchainInitializer;
      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        using Swapchain_t = Graphics::RSwapchainInitializer;
      #else
      # error not implemented
      #endif

        struct SurfaceData
        {
            CommandBatchPtr             endCmdBatch;
            AsyncTask                   prevTask;

            Graphics::SwapchainDesc     desc;       // pending
            Ptr< const IWindow >        window;
        };
        using SurfaceDataSync_t = Threading::Synchronized< SharedMutex, SurfaceData >;


    // variables
    private:
        Atomic<bool>        _initialized    {false};
        Atomic<bool>        _recreate       {false};
        FAtomic<float>      _pixToMm        {1.f};

        Swapchain_t         _swapchain;     // thread safe

        SurfaceDataSync_t   _surfData;


    // methods
    public:
        WindowSurface ()                                                                                            __NE___;
        ~WindowSurface ()                                                                                           __NE___;

        ND_ bool  Init (IWindow &wnd, const Graphics::SwapchainDesc &desc)                                          __NE___;


    // IOutputSurface //
        bool                IsInitialized ()                                                                        C_NE_OV { return _initialized.load(); }
        RenderPassInfo      GetRenderPassInfo ()                                                                    C_NE_OV;
        bool                SetSurfaceMode (const SurfaceInfo &)                                                    __NE_OV;

        AsyncTask           Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask>)__NE_OV;
        bool                GetTargets (OUT RenderTargets_t &targets)                                               C_NE_OV;
        AsyncTask           End (ArrayView<AsyncTask> deps)                                                         __NE_OV;

        TargetInfos_t       GetTargetInfo ()                                                                        C_NE_OV;
        SurfaceFormats_t    GetSurfaceFormats ()                                                                    C_NE_OV;
        PresentModes_t      GetPresentModes ()                                                                      C_NE_OV;
        SurfaceInfo         GetSurfaceInfo ()                                                                       C_NE_OV;


    // IWindow private api
    public:
        void  CreateSwapchain ()                                                                                    __NE___;
        void  ResizeSwapchain ()                                                                                    __NE___;
        void  DestroySwapchain ()                                                                                   __NE___;


    private:
        void  _UpdateDesc (SurfaceDataSync_t::WriteNoLock_t &)                                                      __NE___;

        bool  _CreateSwapchain (SurfaceDataSync_t::WriteNoLock_t &)                                                 __NE___;
    };


} // AE::App

