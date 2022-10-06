// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/NativeWindow.h"
# include "graphics/Public/SwapchainDesc.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{

	//
	// Metal Capture Scope (for Xcode debugger)
	//

	class MCaptureScope
	{
	// variables
	private:
		MetalCaptureScopeRC		_scope;


	// methods
	public:
		MCaptureScope () {}
		~MCaptureScope () {}

		ND_ bool  Create ();

		void  Begin ();
		void  End ();
	};



	//
	// Metal Swapchain
	//

	class MSwapchain : public Noncopyable
	{
	// types
	protected:
		static constexpr uint	ImageCacheSize	= 8;

		using MtlImageCache_t		= StaticArray< MetalImage,			ImageCacheSize >;
		using ImageIdCache_t		= StaticArray< Strong<ImageID>,		ImageCacheSize >;
		using ImageViewIdCache_t	= StaticArray< Strong<ImageViewID>,	ImageCacheSize >;

		using ImagePresentQueue_t	= StaticArray< MetalImage, GraphicsConfig::MaxFrames >;


	// variables
	protected:
		uint2						_surfaceSize;
		uint						_imageIdx			= 0;

		MtlImageCache_t				_mtlImageCache;
		ImageIdCache_t				_imageIdCache;
		ImageViewIdCache_t			_imageViewIdCache;

		ImagePresentQueue_t			_imageQueue;
		MetalSharedEventRC			_semaphore;

		MetalCADrawableRC			_mtlDrawable;
		MetalCALayerRC				_mtlLayer;
		
		EPixelFormat				_colorFormat		= Default;
		EColorSpace					_colorSpace			= Default;
		EPresentMode				_presentMode		= Default;
		ubyte						_imageCount			= 0;
		
		MDevice const&				_device;
		Ptr<MResourceManager>		_resMngr;

		DRC_ONLY( RWDataRaceCheck	_drCheck; )


	// methods
	protected:
		explicit MSwapchain (const MDevice &);

	public:
		~MSwapchain ();

			bool  GetColorFormats (OUT FeatureSet::PixelFormatSet_t &formats) const;
			
		ND_ bool  AcquireNextImage ();
		ND_ bool  Present (MQueuePtr queue);

		ND_ bool			IsInitialized ()		const	{ DRC_SHAREDLOCK( _drCheck );  return bool(_mtlLayer) and _colorFormat != Default; }

		ND_ MetalCALayer	GetMtlSurface ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _mtlLayer; }

		ND_ uint2			GetSurfaceSize ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _surfaceSize; }

		ND_ EPixelFormat	GetColorFormat ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _colorFormat; }
		ND_ EColorSpace		GetColorSpace ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _colorSpace; }
		ND_ EPresentMode	GetPresentMode ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _presentMode; }
		ND_ uint			GetSwapchainLength ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _imageCount; }

		ND_ bool			IsImageAcquired ()		const;	//{ DRC_SHAREDLOCK( _drCheck );  return bool(_mtlImage); }
		
		// same as output params in 'AcquireNextImage()'
		ND_ MetalCmdBatchDependency		GetImageAvailableSemaphore ()const;
		ND_ MetalCmdBatchDependency		GetRenderFinishedSemaphore ()const;

		ND_ MetalImage					GetMtlCurrentImage ()		const;
		ND_ Tuple<ImageID, ImageViewID>	GetCurrentImageAndViewID ()	const;
	};
	


	//
	// Metal Swapchain Initializer
	//

	class MSwapchainInitializer final : public MSwapchain
	{
	private:
		static constexpr EImageUsage	_ImageUsage =
			EImageUsage::Transfer | EImageUsage::Sampled | EImageUsage::Storage |
			EImageUsage::ColorAttachment | EImageUsage::InputAttachment;


	// methods
	public:
		explicit MSwapchainInitializer (const MDevice &);
		
		ND_ bool  CreateSurface (const NativeWindow &, StringView dbgName = {});
			void  DestroySurface ();

		ND_ bool  IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage) const;

		ND_ bool  ChooseFormat (INOUT EPixelFormat &colorFormat, INOUT EColorSpace &colorSpace) const;

		struct CreateInfo
		{
			uint2			viewSize;
			EPixelFormat	colorFormat			= EPixelFormat::sRGB8_A8;
			EColorSpace		colorSpace			= EColorSpace::sRGB_nonlinear;
			uint			minImageCount		= 2;
			EImageUsage		colorImageUsage		= EImageUsage::ColorAttachment;
			EPresentMode	presentMode			= EPresentMode::FIFO;
		};
		ND_ bool  Create (MResourceManager*	resMngr,
						  const CreateInfo&	info,
						  StringView		dbgName	= {});

		ND_ bool  Create (MResourceManager&		resMngr,
						  const uint2&			viewSize,
						  const SwapchainDesc&	desc,
						  StringView			dbgName	= {});

			void  Destroy ();

		ND_ bool  Recreate (const uint2 &size);

	private:
		ND_ bool  _Create (MResourceManager* resMngr, const CreateInfo& info, StringView dbgName);
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
