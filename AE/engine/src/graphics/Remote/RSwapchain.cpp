// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/RSwapchain.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

# include "GraphicsLib.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;


/*
=================================================
	constructor / destructor
=================================================
*/
	RSwapchain::RSwapchain () __NE___ :
		_indices{ MutableIdxBits{} }
	{}

	RSwapchain::~RSwapchain () __NE___
	{
		ASSERT( _device == null );
		ASSERT( _formats.empty() );
		ASSERT( _presentModes.empty() );
	}

/*
=================================================
	GetSurfaceFormats
=================================================
*/
	bool  RSwapchain::GetSurfaceFormats (OUT FeatureSet::SurfaceFormatSet_t &result) C_NE___
	{
		result = Default;

		SHAREDLOCK( _guard );
		CHECK_ERR( not _formats.empty() );

		for (auto& fmt : _formats)
		{
			ESurfaceFormat	sf = ESurfaceFormat_Cast( fmt.colorFormat, fmt.colorSpace );
			if ( sf < ESurfaceFormat::_Count )
				result.insert( sf );
		}
		return true;
	}

	usize  RSwapchain::GetSurfaceFormats (OUT SurfaceFormat* dst, const usize maxCount) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( not _formats.empty() );

		const usize	count = Min( _formats.size(), maxCount );

		for (usize i = 0; i < count; ++i) {
			dst[i] = _formats[i];
		}
		return count;
	}

/*
=================================================
	GetPresentModes
=================================================
*/
	usize  RSwapchain::GetPresentModes (OUT EPresentMode* dst, const usize maxCount) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( not _presentModes.empty() );

		const usize	count = Min( _presentModes.size(), maxCount );

		for (usize i = 0; i < count; ++i) {
			dst[i] = _presentModes[i];
		}
		return count;
	}

/*
=================================================
	AcquireNextImage
=================================================
*/
	RSwapchain::EAcquireResult  RSwapchain::AcquireNextImage () __NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( not IsImageAcquired(), EAcquireResult::Error );

		Msg::Swapchain_AcquireNextImage					msg;
		RC<Msg::Swapchain_AcquireNextImage_Response>	res;

		CHECK_ERR( _device->SendAndWait( msg, OUT res ), EAcquireResult::Error );

		MutableIdxBits	idx;
		idx.semaphoreId	= res->semaphoreId;
		idx.imageIdx	= res->imageIdx;
		_indices.store( idx );

		if ( AnyEqual( res->result, EAcquireResult::OK, EAcquireResult::OK_RecreateLater ))
			CHECK_ERR( IsImageAcquired(), EAcquireResult::Error );

		return res->result;
	}

/*
=================================================
	Present
=================================================
*/
	RSwapchain::EPresentResult  RSwapchain::Present (RQueuePtr queue) __NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( IsImageAcquired(), EPresentResult::Error );

		Msg::Swapchain_Present				msg;
		RC<Msg::Swapchain_Present_Response>	res;

		auto	idx = _indices.load();

		msg.submitIdx		= GraphicsScheduler().GetExecutionIndex();
		msg.presentQueue	= queue->type;
		msg.semaphoreId		= idx.semaphoreId;
		msg.imageIdx		= idx.imageIdx;

		CHECK_ERR( _device->SendAndWait( msg, OUT res ), EPresentResult::Error );

		MutableIdxBits	cur_idx = _indices.load();
		cur_idx.imageIdx = _MaxImageIndex;
		_indices.store( cur_idx );

		return res->result;
	}

/*
=================================================
	GetCurrentImageAndViewID
=================================================
*/
	ImageAndViewID  RSwapchain::GetCurrentImageAndViewID () C_NE___
	{
		const uint	idx = GetCurrentImageIndex();

		if_likely( idx < _MaxSwapchainLength )
			return ImageAndViewID{ _imageIDs[idx], _imageViewIDs[idx] };

		return Default;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RSwapchainInitializer::RSwapchainInitializer () __NE___
	{}

/*
=================================================
	CreateSurface
=================================================
*/
	bool  RSwapchainInitializer::CreateSurface (const NativeWindow &wnd, StringView dbgName) __NE___
	{
		EXLOCK( _guard );

		_device = &GraphicsScheduler().GetDevice();
		CHECK_ERR( _device->IsInitialized() );

		{
			Msg::Surface_Create					msg;
			RC<Msg::Surface_Create_Response>	res;

			CHECK_ERR( _device->SendAndWait( msg, OUT res ));
			CHECK_ERR( res->created );

			_formats		= res->formats;
			_presentModes	= res->presentModes;
		}

		_nativeWnd = wnd;

		Unused( dbgName );
		return true;
	}

/*
=================================================
	DestroySurface
=================================================
*/
	void  RSwapchainInitializer::DestroySurface () __NE___
	{
		EXLOCK( _guard );

		CHECK_ERRV( _device != null );

		Msg::Surface_Destroy	msg;
		CHECK( _device->Send( msg ));

		_device		= null;
		_nativeWnd	= Default;

		_formats.clear();
		_presentModes.clear();
	}

/*
=================================================
	Create
=================================================
*/
	bool  RSwapchainInitializer::Create (const uint2& viewSize, const SwapchainDesc& desc, StringView dbgName) __NE___
	{
		EXLOCK( _guard );

		CHECK_ERR_MSG( _device != null, "surface is not created" );

		Msg::Swapchain_Create				msg;
		RC<Msg::Swapchain_Create_Response>	res;

		msg.viewSize	= viewSize;
		msg.desc		= desc;
		msg.dbgName		= dbgName;

		CHECK_ERR( _device->SendAndWait( msg, OUT res ));

		if ( res->minimized )
		{
			_surfaceSize.store( ushort2{res->viewSize} );
			return true;
		}

		CHECK_ERR( res->created );

		_desc = res->desc;
		_surfaceSize.store( ushort2{res->viewSize} );

		_imageAvailableSem	= res->imageAvailable;
		_renderFinishedSem	= res->renderFinished;

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		const usize	img_count	= res->imageIds.size();
		CHECK( img_count == res->viewIds.size() );

		RemoteImageDesc		img_desc;
		RemoteImageViewDesc	view_desc;

		img_desc.desc.dimension		= uint3{res->viewSize, 1u};
		img_desc.desc.imageDim		= EImageDim_2D;
		img_desc.desc.options		= _desc.options;
		img_desc.desc.usage			= _desc.usage;
		img_desc.desc.format		= _desc.colorFormat;
		img_desc.releaseRef			= false;

		view_desc.desc.viewType		= EImage_2D;
		view_desc.desc.format		= _desc.colorFormat;
		view_desc.desc.aspectMask	= EImageAspect::Color;
		view_desc.releaseRef		= false;

		for (usize i = 0; i < img_count; ++i)
		{
			img_desc.imageId	= res->imageIds[i];
			view_desc.viewId	= res->viewIds[i];

			auto	prev_img = _imageIDs[i].Attach( res_mngr.CreateImage( img_desc, "SwapchainImage_"s + ToString(i) ));
			res_mngr.DelayedRelease( prev_img );
			CHECK_ERR( _imageIDs[i] );

			auto	prev_view = _imageViewIDs[i].Attach( res_mngr.CreateImageView( view_desc, _imageIDs[i], "SwapchainImageView_"s + ToString(i) ));
			res_mngr.DelayedRelease( prev_view );
			CHECK_ERR( _imageViewIDs[i] );
		}

		if ( auto glib = _device->GetGraphicsLib() )
			CHECK_ERR( glib->CreateSwapchain( viewSize, desc, _nativeWnd ));

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RSwapchainInitializer::Destroy () __NE___
	{
		EXLOCK( _guard );

		if ( _device == null )
			RETURN_ERRV( "surface is not created" );

		Msg::Swapchain_Destroy	msg;
		CHECK( _device->Send( msg ));

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		for (auto& id : _imageViewIDs)
		{
			if ( auto tmp = id.Release() )
				DEV_CHECK( res_mngr.DelayedRelease( INOUT tmp ) == 0 );
		}
		for (auto& id : _imageIDs)
		{
			if ( auto tmp = id.Release() )
				res_mngr.DelayedRelease( INOUT tmp );
		}
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  RSwapchainInitializer::IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage) C_NE___
	{
		Msg::Swapchain_IsSupported	msg;
		RC<Msg::DefaultResponse>	res;

		msg.presentMode		= presentMode;
		msg.colorFormat		= colorFormat;
		msg.colorImageUsage	= colorImageUsage;

		CHECK_ERR( _device->SendAndWait( msg, OUT res ));
		return res->ok;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
