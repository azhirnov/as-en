// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/WindowSurface.h"

namespace AE::App
{
	using namespace AE::Graphics;
	using AE::Threading::WeakDep;

namespace {

// Windows, Vulkan docs:
//   "In a multithreaded environment, calling SendMessage from a thread that is not the thread associated with pCreateInfo::hwnd
//   will block until the application has processed the window message."
//   https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#platformCreateSurface_win32
//
// Android:
//   Using Acquire/Present in different threads may cause a long synchronization, so use single thread.

# if 1 //def AE_PLATFORM_ANDROID
	static constexpr auto	c_AcquireAndPresentQueue	= ETaskQueue::Main;
# else
	static constexpr auto	c_AcquireAndPresentQueue	= ETaskQueue::PerFrame;
# endif
	static constexpr auto	c_RecreateSwapchainQueue	= ETaskQueue::Main;
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
	#ifndef AE_ENABLE_REMOTE_GRAPHICS
		_recreate.store( true );
	#endif
	}

/*
=================================================
	CreateSwapchain
=================================================
*/
	void  WindowSurface::CreateSwapchain () __NE___
	{
		auto	data = _surfData.WriteLock();

		if ( data->desc.colorFormat == Default )
			return;	// wait for 'Init()'

		if_unlikely( _initialized.load() )
			return;	// already initialized

		_CreateSwapchain( data );
	}

/*
=================================================
	_CreateSwapchain
=================================================
*/
	bool  WindowSurface::_CreateSwapchain (SurfaceDataSync_t::WriteNoLock_t &data) __NE___
	{
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&	dev			= res_mngr.GetDevice();

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
		auto	data = _surfData.WriteLock();

		if_unlikely( not _initialized.exchange( false ))
			return;

		CHECK( GraphicsScheduler().WaitAll( AE::DefaultTimeout ));

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
		auto	data = _surfData.WriteLock();

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

			CHECK( GraphicsScheduler().WaitAll( AE::DefaultTimeout ));

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
		auto	data = _surfData.WriteLock();

		CHECK_ERR( _initialized.load() );

		if ( info.colorFormat != Default )	data->desc.colorFormat	= info.colorFormat;
		if ( info.colorSpace  != Default )	data->desc.colorSpace	= info.colorSpace;
		if ( info.presentMode != Default )	data->desc.presentMode	= info.presentMode;

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

		RenderPassInfo				result;
		RenderPassInfo::Attachment&	att = result.attachments.emplace_back();

		att.format	= _swapchain.GetDescription().colorFormat;

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
		CHECK_ERR( _swapchain.IsImageAcquired() );	// must be between 'Begin()' / 'End()'

		auto	image_and_view	= _swapchain.GetCurrentImageAndViewID();
		auto&	dst				= targets.emplace_back();
		auto	sw_desc			= _swapchain.GetDescription();

		dst.imageId			= image_and_view.image;
		dst.viewId			= image_and_view.view;
		dst.region			= RectI{ int2{0}, int2{_swapchain.GetSurfaceSize()} };

		dst.pixToMm			= _pixToMm.load();
		dst.format			= sw_desc.colorFormat;
		dst.colorSpace		= sw_desc.colorSpace;
		dst.transform		= _swapchain.GetSurfaceTransform();

		dst.initialState	= EResourceState::PresentImage;
		dst.finalState		= EResourceState::PresentImage;

		return true;
	}

/*
=================================================
	GetSurfaceFormats
=================================================
*/
	IOutputSurface::SurfaceFormats_t  WindowSurface::GetSurfaceFormats () C_NE___
	{
		SurfaceFormats_t	result;
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
		PresentModes_t	result;
		result.resize( result.capacity() );

		result.resize( _swapchain.GetPresentModes( OUT result.data(), result.capacity() ));
		return result;
	}

/*
=================================================
	GetTargetInfo
=================================================
*/
	IOutputSurface::TargetInfos_t  WindowSurface::GetTargetInfo () C_NE___
	{
		TargetInfos_t	result;
		result.emplace_back( _swapchain.GetSurfaceSize(), _pixToMm.load() );
		return result;
	}

/*
=================================================
	GetSurfaceInfo
=================================================
*/
	IOutputSurface::SurfaceInfo  WindowSurface::GetSurfaceInfo () C_NE___
	{
		SwapchainDesc	desc	= _swapchain.GetDescription();
		SurfaceInfo		result;

		result.type			= ESurfaceType::Screen;		// TODO: CurvedScreen
		result.colorFormat	= desc.colorFormat;
		result.colorSpace	= desc.colorSpace;
		result.presentMode	= desc.presentMode;

		return result;
	}

/*
=================================================
	_UpdateDesc
=================================================
*/
	void  WindowSurface::_UpdateDesc (SurfaceDataSync_t::WriteNoLock_t &data) __NE___
	{
		const auto	new_desc	= _swapchain.GetDescription();
		const auto	m			= data->window->GetMonitor();	// must be in main thread
		float		px_to_mm	= m.PixelsPerMillimeter();		// pix / mm

		_pixToMm.store( px_to_mm );

		if_unlikely( new_desc.colorSpace != data->prevColorSpace )
		{
			Unused( data->window->SetColorSpace( new_desc.colorSpace ));
		}

		data->desc				= new_desc;
		data->prevColorSpace	= new_desc.colorSpace;
	}

/*
=================================================
	_RecreateSwapchainTask
=================================================
*/
	AsyncTask  WindowSurface::_RecreateSwapchainTask (WindowSurface &surface) __NE___
	{
		return Scheduler().Run(
			c_RecreateSwapchainQueue,
			[] (WindowSurface &surface) -> AsyncCoro
			{
				auto	data = surface._surfData.WriteLock();

				auto&			swapchain	= surface._swapchain;
				SwapchainDesc	new_desc	= data->desc;
				uint2			new_size	= data->window->GetSurfaceSize();

				if_unlikely( not surface._recreate.exchange( false ))
					co_return;  // already recreated

				if_unlikely( Any( IsZero( new_size )))
				{
					// recreate later
					co_return;
				}

				if_likely( swapchain.Create( new_size, new_desc ))
				{
					surface._UpdateDesc( data );
				}
				else
				{
					DBG_WARNING( "failed to create swapchain" );
					surface._initialized.store( false );
					Coro_Error();
				}
			}( surface ),
			Tuple{},
			"WindowSurface::RecreateSwapchain"
		);
	}
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
/*
=================================================
	_AcquireNextImageTask
=================================================
*/
	AsyncCoro  WindowSurface::_AcquireNextImageTask (WindowSurface &	surface,
													 CommandBatchPtr	beginCmdBatch,
													 CommandBatchPtr	endCmdBatch) __NE___
	{
		for (;;)
		{
			for (; surface._recreate.load(); )
			{
				auto	task = _RecreateSwapchainTask( surface );
				Coro_Continue( task );
			}

			auto&		swapchain	= surface._swapchain;
			VkResult	err			= swapchain.AcquireNextImage();

			switch ( err )
			{
				case_likely VK_SUCCESS :
					break;

				case VK_SUBOPTIMAL_KHR :		// acquired, but should be recreated later
					surface._recreate.store( true );
					break;

				case VK_ERROR_OUT_OF_DATE_KHR :	// recreate immediately
				{
					surface._recreate.store( true );

					auto	task = _RecreateSwapchainTask( surface );
					Coro_Continue( task );
					break;
				}

				case VK_TIMEOUT :				// should never happens
				case VK_NOT_READY :				// no available images
				case VK_ERROR_SURFACE_LOST_KHR :
				default :
					Unused( __vk_CheckErrors( err, "Failed to acquire next swapchain image", AE_FUNCTION_NAME, SourceLoc::current() ));
					CHECK_CE( false, "Failed to acquire next swapchain image" );
			}

			if ( not AnyEqual( err, VK_SUCCESS, VK_SUBOPTIMAL_KHR ))
				continue;
			
			CHECK_CE( beginCmdBatch->AddInputSemaphore(  swapchain.GetImageAvailableSemaphore(), 0 ));
			CHECK_CE( endCmdBatch  ->AddOutputSemaphore( swapchain.GetRenderFinishedSemaphore(), 0 ));
			co_return;
		}
	}

/*
=================================================
	_PresentImageTask
=================================================
*/
	AsyncCoro  WindowSurface::_PresentImageTask (WindowSurface &	surface,
												 const FrameUID		frameId,
												 const EQueueType	presentQueue) __NE___
	{
		auto&		rts	= GraphicsScheduler();
		auto		q	= rts.GetDevice().GetQueue( presentQueue );
		VkResult	err	= surface._swapchain.Present( q, Default, frameId );

		switch ( err )
		{
			case_likely VK_SUCCESS :
				break;

			case VK_ERROR_SURFACE_LOST_KHR :
			case VK_ERROR_OUT_OF_DATE_KHR :
				surface._recreate.store( true );	// recreate later
				break;

			// Android: always returned if used custom rotation
			// Other: returned when swapchain size != surface size
			case VK_SUBOPTIMAL_KHR :
				surface._recreate.store( true );	// recreate later
				break;

			default :
				Unused( __vk_CheckErrors( err, "Presentation failed", AE_FUNCTION_NAME, SourceLoc::current() ));
				CHECK_CE( false, "Presentation failed" );
		}
		co_return;
	}
//-----------------------------------------------------------------------------


#elif defined(AE_ENABLE_METAL)
/*
=================================================
	_AcquireNextImageTask
=================================================
*
	AsyncCoro  WindowSurface::_AcquireNextImageTask (WindowSurface &	surface,
													 CommandBatchPtr	beginCmdBatch,
													 CommandBatchPtr	endCmdBatch) __NE___
	{
		auto&	swapchain = surface._swapchain;

		if_unlikely( surface._recreate.load() )
		{
			// TODO
		}

		CHECK_CE( swapchain.AcquireNextImage() );

		CHECK_CE( beginCmdBatch->AddInputSemaphore(  swapchain.GetImageAvailableSemaphore() ));
		CHECK_CE( endCmdBatch  ->AddOutputSemaphore( swapchain.GetRenderFinishedSemaphore() ));
	}

/*
=================================================
	_PresentImageTask
=================================================
*/
	AsyncCoro  WindowSurface::_PresentImageTask (WindowSurface &	surface,
												 const FrameUID		frameId,
												 const EQueueType	presentQueue) __NE___
	{
		auto&	rts	= GraphicsScheduler();
		auto	q	= rts.GetDevice().GetQueue( presentQueue );

		CHECK_CE( surface._swapchain.Present( q, Default, frameId ));
	}
//-----------------------------------------------------------------------------


#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
/*
=================================================
	_AcquireNextImageTask
=================================================
*/
	AsyncCoro  WindowSurface::_AcquireNextImageTask (WindowSurface &	surface,
													 CommandBatchPtr	beginCmdBatch,
													 CommandBatchPtr	endCmdBatch) __NE___
	{
		for (;;)
		{
			for (; surface._recreate.load(); )
			{
				auto	task = _RecreateSwapchainTask( surface );
				Coro_Continue( task );
			}

			auto&	swapchain	= surface._swapchain;
			auto	err			= swapchain.AcquireNextImage();

			switch_enum( err )
			{
				case_likely RSwapchain::EAcquireResult::OK :
					break;

				case RSwapchain::EAcquireResult::OK_RecreateLater :
					surface._recreate.store( true );
					break;

				case RSwapchain::EAcquireResult::Error_RecreateImmediately :
				{
					surface._recreate.store( true );

					auto	task = _RecreateSwapchainTask( surface );
					Coro_Continue( task );
					break;
				}

				case RSwapchain::EAcquireResult::Error :
				default :
					CHECK_CE( false, "Failed to acquire next swapchain image" );
			}
			switch_end

			CHECK_CE( beginCmdBatch->AddInputSemaphore(  swapchain.GetImageAvailableSemaphore(), 0 ));
			CHECK_CE( endCmdBatch  ->AddOutputSemaphore( swapchain.GetRenderFinishedSemaphore(), 0 ));
			co_return;
		}
	}

/*
=================================================
	_PresentImageTask
=================================================
*/
	AsyncCoro  WindowSurface::_PresentImageTask (WindowSurface &	surface,
												 const FrameUID		frameId,
												 const EQueueType	presentQueue) __NE___
	{
		auto&	rts	= GraphicsScheduler();
		auto	q	= rts.GetDevice().GetQueue( presentQueue );
		auto	err	= surface._swapchain.Present( q, frameId );

		switch_enum( err )
		{
			case_likely RSwapchain::EPresentResult::OK :
				break;

			case RSwapchain::EPresentResult::OK_RecreateLater :
			case RSwapchain::EPresentResult::Error_RecreateImmediately :
				surface._recreate.store( true );
				break;

			case RSwapchain::EPresentResult::Error :
			default :
				CHECK_CE( false, "Presentation failed" );
		}
		switch_end

		co_return;
	}
//-----------------------------------------------------------------------------

#else
#	error not implemented
#endif



/*
=================================================
	Begin
=================================================
*/
	AsyncTask  WindowSurface::Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps) __NE___
	{
		CHECK_ERR( beginCmdBatch and endCmdBatch );
		CHECK_ERR( beginCmdBatch->GetQueueType() == endCmdBatch->GetQueueType() );	// TODO: temporary
		CHECK_ERR( beginCmdBatch->GetSubmitIndex() <= endCmdBatch->GetSubmitIndex() );

		// Batches which are use surface image must be in recording state,
		// returned task must be added as input dependency to draw tasks.
		ASSERT( beginCmdBatch->IsRecording() );
		ASSERT( endCmdBatch->IsRecording() );

		auto	data = _surfData.WriteLock();

		CHECK_ERR( _initialized.load() );

		AsyncTask	present = RVRef(data->prevTask);	// can be null
		AsyncTask	task	= Scheduler().Run(
								c_AcquireAndPresentQueue,
								_AcquireNextImageTask( *this, RVRef(beginCmdBatch), endCmdBatch ),
								Tuple{ WeakDep{RVRef(present)}, deps },	// don't use strong dependency from 'present' !
								"WindowSurface::AcquireNextImage"
							);
		data->prevTask		= task;
		data->endCmdBatch	= RVRef(endCmdBatch);

		return task;
	}

/*
=================================================
	End
=================================================
*/
	AsyncTask  WindowSurface::End (ArrayView<AsyncTask> deps) __NE___
	{
		auto	data = _surfData.WriteLock();

		CHECK_ERR( _initialized.load() );
		CHECK_ERR( data->endCmdBatch );

		AsyncTask	acquire = RVRef(data->prevTask);	// can be null
		auto		queue	= data->endCmdBatch->GetQueueType();
		auto		fid		= data->endCmdBatch->GetFrameId();
		AsyncTask	task	= Scheduler().Run(
								c_AcquireAndPresentQueue,
								_PresentImageTask( *this, fid, queue ),
								Tuple{ RVRef(acquire), CmdBatchOnSubmit{RVRef(data->endCmdBatch)}, deps },
								"WindowSurface::PresentImage"
							);
		data->prevTask		= task;
		data->endCmdBatch	= null;

		return task;
	}


} // AE::App
