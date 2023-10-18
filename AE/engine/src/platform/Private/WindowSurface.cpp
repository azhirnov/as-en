// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/WindowSurface.h"

namespace AE::App
{
    using namespace AE::Graphics;

namespace {
# if 1 //def AE_PLATFORM_ANDROID
    static constexpr auto   AcquireAndPresentQueue  = ETaskQueue::Main;
# else
    static constexpr auto   AcquireAndPresentQueue  = ETaskQueue::PerFrame;
# endif
    static constexpr auto   RecreateSwapchainQueue  = ETaskQueue::Main;
}

/*
=================================================
    constructor
=================================================
*/
    WindowSurface::WindowSurface () __NE___
    {}

/*
=================================================
    destructor
=================================================
*/
    WindowSurface::~WindowSurface () __NE___
    {
        DestroySwapchain();
    }

/*
=================================================
    ResizeSwapchain
=================================================
*/
    void  WindowSurface::ResizeSwapchain () __NE___
    {
        _recreate.store( true );
    }

/*
=================================================
    CreateSwapchain
=================================================
*/
    void  WindowSurface::CreateSwapchain () __NE___
    {
        auto    data = _surfData.WriteNoLock();
        EXLOCK( data );

        if ( data->desc.colorFormat == Default )
            return; // wait for 'Init()'

        if_unlikely( _initialized.load() )
            return; // already initialized

        _CreateSwapchain( data );
    }

/*
=================================================
    _CreateSwapchain
=================================================
*/
    bool  WindowSurface::_CreateSwapchain (SurfaceDataSync_t::WriteNoLock_t &data) __NE___
    {
        auto&   res_mngr    = RenderTaskScheduler().GetResourceManager();
        auto&   dev         = res_mngr.GetDevice();

        CHECK_ERR( dev.IsInitialized() );

        CHECK_ERR( _swapchain.CreateSurface( data->window->GetNative() ));
        CHECK_ERR( res_mngr.OnSurfaceCreated( _swapchain ));

        CHECK_ERR( _swapchain.Create( data->window->GetSurfaceSize(), data->desc ));
        _UpdateDesc( data );

        _initialized.store( true );
        _recreate.store( false );

        return true;
    }

/*
=================================================
    DestroySwapchain
=================================================
*/
    void  WindowSurface::DestroySwapchain () __NE___
    {
        // Begin() / End() must not run in the another thread, so lock the '_surfData'
        auto    data = _surfData.WriteNoLock();
        EXLOCK( data );

        if_unlikely( not _initialized.exchange( false ))
            return;

        CHECK( RenderTaskScheduler().WaitAll() );

        _swapchain.Destroy();
        _swapchain.DestroySurface();
    }

/*
=================================================
    Init
=================================================
*/
    bool  WindowSurface::Init (IWindow &wnd, const SwapchainDesc &desc) __NE___
    {
        auto    data = _surfData.WriteNoLock();
        EXLOCK( data );

        data->desc = desc;

        // recreate swapchain
        if_likely( _initialized.load() and data->window == &wnd )
        {
            _recreate.store( true );
            return true;
        }

        // recreate with new surface
        if_unlikely( _initialized.load() and data->window != &wnd )
        {
            _initialized.store( false );

            CHECK( RenderTaskScheduler().WaitAll() );

            data->window = &wnd;

            _swapchain.Destroy();
            _swapchain.DestroySurface();

            return _CreateSwapchain( data );
        }

        data->window = &wnd;
        return true;
    }

/*
=================================================
    SetSurfaceMode
=================================================
*/
    bool  WindowSurface::SetSurfaceMode (const SurfaceInfo &info) __NE___
    {
        auto    data = _surfData.WriteNoLock();
        EXLOCK( data );

        CHECK_ERR( _initialized.load() );

        if ( info.colorFormat != Default )  data->desc.colorFormat  = info.colorFormat;
        if ( info.colorSpace != Default )   data->desc.colorSpace   = info.colorSpace;
        if ( info.presentMode != Default )  data->desc.presentMode  = info.presentMode;

        _recreate.store( true );
        return true;
    }

/*
=================================================
    GetRenderPassInfo
=================================================
*/
    IOutputSurface::RenderPassInfo  WindowSurface::GetRenderPassInfo () C_NE___
    {
        CHECK_ERR( _initialized.load() );

        RenderPassInfo              result;
        RenderPassInfo::Attachment& att = result.attachments.emplace_back();

        att.format  = _swapchain.GetDescription().colorFormat;

        return result;
    }

/*
=================================================
    GetTargets
=================================================
*/
    bool  WindowSurface::GetTargets (OUT RenderTargets_t &targets) C_NE___
    {
        targets.clear();

        CHECK_ERR( _initialized.load() );
        CHECK_ERR( _swapchain.IsImageAcquired() );  // must be between 'Begin()' / 'End()'

        auto    image_and_view  = _swapchain.GetCurrentImageAndViewID();
        auto&   dst             = targets.emplace_back();
        auto    sw_desc         = _swapchain.GetDescription();

        dst.imageId         = image_and_view.image;
        dst.viewId          = image_and_view.view;
        dst.region          = RectI{ int2{0}, int2{_swapchain.GetSurfaceSize()} };

        dst.pixToMm         = _pixToMm.load();
        dst.format          = sw_desc.colorFormat;
        dst.colorSpace      = sw_desc.colorSpace;

        dst.initialState    = EResourceState::PresentImage;
        dst.finalState      = EResourceState::PresentImage;

        // TODO: get preTransform from swapchain and apply to projection

        return true;
    }

/*
=================================================
    GetSurfaceFormats
=================================================
*/
    IOutputSurface::SurfaceFormats_t  WindowSurface::GetSurfaceFormats () C_NE___
    {
        SurfaceFormats_t    result;
        result.resize( result.capacity() );

        result.resize( _swapchain.GetSurfaceFormats( OUT result.data(), result.capacity() ));
        return result;
    }

/*
=================================================
    GetPresentModes
=================================================
*/
    IOutputSurface::PresentModes_t  WindowSurface::GetPresentModes () C_NE___
    {
        PresentModes_t  result;
        result.resize( result.capacity() );

        result.resize( _swapchain.GetPresentModes( OUT result.data(), result.capacity() ));
        return result;
    }

/*
=================================================
    GetTargetSizes
=================================================
*/
    IOutputSurface::TargetSizes_t  WindowSurface::GetTargetSizes () C_NE___
    {
        TargetSizes_t   result;
        result.push_back( _swapchain.GetSurfaceSize() );
        return result;
    }

/*
=================================================
    GetSurfaceInfo
=================================================
*/
    IOutputSurface::SurfaceInfo  WindowSurface::GetSurfaceInfo () C_NE___
    {
        // result may be changed immediately if method used outside of 'Begin() / End()' scope.
        //ASSERT( _swapchain.IsImageAcquired() );

        SwapchainDesc   desc    = _swapchain.GetDescription();
        SurfaceInfo     result;

        result.type         = ESurfaceType::Screen;     // TODO: CurvedScreen
        result.colorFormat  = desc.colorFormat;
        result.colorSpace   = desc.colorSpace;
        result.presentMode  = desc.presentMode;

        return result;
    }

/*
=================================================
    _UpdateDesc
=================================================
*/
    void  WindowSurface::_UpdateDesc (SurfaceDataSync_t::WriteNoLock_t &data) __NE___
    {
                    data->desc  = _swapchain.GetDescription();

        const auto  m           = data->window->GetMonitor();   // must be in main thread
        float2      px_to_mm    = m.MillimetersPerPixel();

        _pixToMm.store( Average( px_to_mm.x, px_to_mm.y ));
    }
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
/*
=================================================
    RecreateSwapchainTask
=================================================
*/
    class WindowSurface::RecreateSwapchainTask final : public Threading::IAsyncTask
    {
    private:
        WindowSurface &     _surface;

    public:
        RecreateSwapchainTask (WindowSurface* surf) :
            IAsyncTask{ RecreateSwapchainQueue },
            _surface{ *surf }
        {}

        void  Run () __Th_OV
        {
            auto    data = _surface._surfData.WriteNoLock();
            EXLOCK( data );

            auto&           swapchain   = _surface._swapchain;
            SwapchainDesc   new_desc    = data->desc;

            if_unlikely( not _surface._recreate.exchange( false ))
                return;

            if_likely( swapchain.Create( data->window->GetSurfaceSize(), new_desc ))
            {
                _surface._UpdateDesc( data );
            }
            else
            {
                _surface._initialized.store( false );
                return OnFailure();
            }
        }

        StringView  DbgName ()  C_NE_OV { return "WindowSurface::RecreateSwapchain"; }
    };

/*
=================================================
    AcquireNextImageTask
=================================================
*/
    class WindowSurface::AcquireNextImageTask final : public Threading::IAsyncTask
    {
    private:
        WindowSurface &     _surface;
        CommandBatchPtr     _beginCmdBatch;
        CommandBatchPtr     _endCmdBatch;

    public:
        AcquireNextImageTask (WindowSurface* surf, CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch) :
            IAsyncTask{ AcquireAndPresentQueue },
            _surface{ *surf },
            _beginCmdBatch{ RVRef(beginCmdBatch) },
            _endCmdBatch{ RVRef(endCmdBatch) }
        {}

        void  Run () __Th_OV
        {
            if_unlikely( _surface._recreate.load() )
            {
                auto    task = Scheduler().Run<RecreateSwapchainTask>( Tuple{&_surface} );

                return Continue( Tuple{task} );
            }

            auto&       swapchain   = _surface._swapchain;
            VkResult    err         = swapchain.AcquireNextImage();

            switch ( err )
            {
                case_likely VK_SUCCESS :
                    break;

                case VK_SUBOPTIMAL_KHR :        // acquired, but should be recreated later
                    _surface._recreate.store( true );
                    break;

                case VK_ERROR_OUT_OF_DATE_KHR : // recreate immediately
                {
                    _surface._recreate.store( true );

                    auto    task = Scheduler().Run<RecreateSwapchainTask>( Tuple{&_surface} );
                    return Continue( Tuple{task} );
                }

                case VK_TIMEOUT :               // should never happens
                case VK_NOT_READY :             // no available images
                case VK_ERROR_SURFACE_LOST_KHR :
                default :
                    Unused( __vk_CheckErrors( err, "Failed to acquire next swapchain image", AE_FUNCTION_NAME, SourceLoc_Current() ));
                    CHECK_TE( false, "Failed to acquire next swapchain image" );
            }

            CHECK_TE( _beginCmdBatch->AddInputSemaphore(  swapchain.GetImageAvailableSemaphore(), 0 ));
            CHECK_TE( _endCmdBatch  ->AddOutputSemaphore( swapchain.GetRenderFinishedSemaphore(), 0 ));
        }

        StringView  DbgName ()  C_NE_OV { return "WindowSurface::AcquireNextImage"; }
    };

/*
=================================================
    PresentImageTask
=================================================
*/
    class WindowSurface::PresentImageTask final : public Threading::IAsyncTask
    {
    private:
        WindowSurface &     _surface;
        const EQueueType    _presentQueue;

    public:
        PresentImageTask (WindowSurface* surf, EQueueType presentQueue) :
            IAsyncTask{ AcquireAndPresentQueue },
            _surface{ *surf },
            _presentQueue{ presentQueue }
        {}

        void  Run () __Th_OV
        {
            auto&       rts = RenderTaskScheduler();
            auto        q   = rts.GetDevice().GetQueue( _presentQueue );
            VkResult    err = _surface._swapchain.Present( q );

            switch ( err )
            {
                case_likely VK_SUCCESS :
                    break;

                case VK_ERROR_SURFACE_LOST_KHR :
                case VK_ERROR_OUT_OF_DATE_KHR :
                    _surface._recreate.store( true );   // recreate later
                    break;

                // Android: always returned if used custom rotation
                // Other: returned when swapchain size != surface size
                case VK_SUBOPTIMAL_KHR :
                    #ifndef AE_PLATFORM_ANDROID
                    _surface._recreate.store( true );   // recreate later
                    #endif
                    break;

                default :
                    Unused( __vk_CheckErrors( err, "Presentation failed", AE_FUNCTION_NAME, SourceLoc_Current() ));
                    CHECK_TE( false, "Presentation failed" );
            }
        }

        StringView  DbgName ()  C_NE_OV { return "WindowSurface::PresentImage"; }
    };
//-----------------------------------------------------------------------------


#elif defined(AE_ENABLE_METAL)
/*
=================================================
    AcquireNextImageTask
=================================================
*/
    class WindowSurface::AcquireNextImageTask final : public Threading::IAsyncTask
    {
    private:
        WindowSurface &     _surface;
        CommandBatchPtr     _beginCmdBatch;
        CommandBatchPtr     _endCmdBatch;

    public:
        AcquireNextImageTask (WindowSurface* surf, CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch) :
            IAsyncTask{ AcquireAndPresentQueue },
            _surface{ *surf },
            _beginCmdBatch{ RVRef(beginCmdBatch) },
            _endCmdBatch{ RVRef(endCmdBatch) }
        {}

        void  Run () __Th_OV
        {
            auto&   swapchain = _surface._swapchain;

            if_unlikely( _surface._recreate.load() )
            {
                // TODO
            }

            CHECK_TE( swapchain.AcquireNextImage() );

            CHECK_TE( _beginCmdBatch->AddInputSemaphore(  swapchain.GetImageAvailableSemaphore() ));
            CHECK_TE( _endCmdBatch  ->AddOutputSemaphore( swapchain.GetRenderFinishedSemaphore() ));
        }

        StringView  DbgName ()  C_NE_OV { return "WindowSurface::AcquireNextImage"; }
    };

/*
=================================================
    PresentImageTask
=================================================
*/
    class WindowSurface::PresentImageTask final : public Threading::IAsyncTask
    {
    private:
        WindowSurface &     _surface;
        const EQueueType    _presentQueue;

    public:
        PresentImageTask (WindowSurface* surf, EQueueType presentQueue) :
            IAsyncTask{ AcquireAndPresentQueue },
            _surface{ *surf },
            _presentQueue{ presentQueue }
        {}

        void  Run () __Th_OV
        {
            auto&   rts = RenderTaskScheduler();
            auto    q   = rts.GetDevice().GetQueue( _presentQueue );

            CHECK_TE( _surface._swapchain.Present( q ));
        }

        StringView  DbgName ()  C_NE_OV { return "WindowSurface::PresentImage"; }
    };
//-----------------------------------------------------------------------------


#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
/*
=================================================
    AcquireNextImageTask
=================================================
*/
    class WindowSurface::AcquireNextImageTask final : public Threading::IAsyncTask
    {
    private:
        WindowSurface &     _surface;
        CommandBatchPtr     _beginCmdBatch;
        CommandBatchPtr     _endCmdBatch;

    public:
        AcquireNextImageTask (WindowSurface* surf, CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch) :
            IAsyncTask{ AcquireAndPresentQueue },
            _surface{ *surf },
            _beginCmdBatch{ RVRef(beginCmdBatch) },
            _endCmdBatch{ RVRef(endCmdBatch) }
        {}

        void  Run () __Th_OV
        {
            auto&   swapchain = _surface._swapchain;

            if_unlikely( _surface._recreate.load() )
            {
                // TODO
            }

            //CHECK_TE( swapchain.AcquireNextImage() );

            //CHECK_TE( _beginCmdBatch->AddInputSemaphore(  swapchain.GetImageAvailableSemaphore() ));
            //CHECK_TE( _endCmdBatch  ->AddOutputSemaphore( swapchain.GetRenderFinishedSemaphore() ));
        }

        StringView  DbgName ()  C_NE_OV { return "WindowSurface::AcquireNextImage"; }
    };

/*
=================================================
    PresentImageTask
=================================================
*/
    class WindowSurface::PresentImageTask final : public Threading::IAsyncTask
    {
    private:
        WindowSurface &     _surface;
        const EQueueType    _presentQueue;

    public:
        PresentImageTask (WindowSurface* surf, EQueueType presentQueue) :
            IAsyncTask{ AcquireAndPresentQueue },
            _surface{ *surf },
            _presentQueue{ presentQueue }
        {}

        void  Run () __Th_OV
        {
            auto&   rts = RenderTaskScheduler();
            auto    q   = rts.GetDevice().GetQueue( _presentQueue );

            //CHECK_TE( _surface._swapchain.Present( q ));
        }

        StringView  DbgName ()  C_NE_OV { return "WindowSurface::PresentImage"; }
    };
//-----------------------------------------------------------------------------

#else
#   error not implemented
#endif



/*
=================================================
    Begin
=================================================
*/
    AsyncTask  WindowSurface::Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps) __NE___
    {
        CHECK_ERR( beginCmdBatch and endCmdBatch );
        CHECK_ERR( beginCmdBatch->GetQueueType() == endCmdBatch->GetQueueType() );  // TODO: temporary
        CHECK_ERR( beginCmdBatch->GetSubmitIndex() <= endCmdBatch->GetSubmitIndex() );

        // Batches which are use surface image must be in recording state,
        // returned task must be added as input dependency to draw tasks.
        ASSERT( beginCmdBatch->IsRecording() );
        ASSERT( endCmdBatch->IsRecording() );

        auto    data = _surfData.WriteNoLock();
        EXLOCK( data );

        CHECK_ERR( _initialized.load() );

        AsyncTask   present = RVRef(data->prevTask);    // can be null
        ASSERT( present == null or DynCast<PresentImageTask>(present) != null );

        AsyncTask   task = Scheduler().Run<AcquireNextImageTask>( Tuple{ this, beginCmdBatch, endCmdBatch }, Tuple{ present, deps });

        data->prevTask      = task;
        data->endCmdBatch   = endCmdBatch;

        return task;
    }

/*
=================================================
    End
=================================================
*/
    AsyncTask  WindowSurface::End (ArrayView<AsyncTask> deps) __NE___
    {
        auto    data = _surfData.WriteNoLock();
        EXLOCK( data );

        CHECK_ERR( _initialized.load() );
        CHECK_ERR( data->endCmdBatch );

        AsyncTask   acquire = RVRef(data->prevTask);    // can be null
        ASSERT( acquire == null or DynCast<AcquireNextImageTask>(acquire) != null );

        AsyncTask   task = Scheduler().Run<PresentImageTask>( Tuple{ this, data->endCmdBatch->GetQueueType() },
                                                              Tuple{ acquire, CmdBatchOnSubmit{data->endCmdBatch}, deps });

        data->prevTask      = task;
        data->endCmdBatch   = null;

        return task;
    }


} // AE::App
