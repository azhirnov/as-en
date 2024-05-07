// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe: yes
*/

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/NativeWindow.h"
# include "graphics/Public/SwapchainDesc.h"
# include "graphics/Remote/RDevice.h"

namespace AE::Graphics
{

	//
	// Remote Swapchain
	//

	class RSwapchain : public Noncopyable
	{
	// types
	public:
		using EAcquireResult = RemoteGraphics::Msg::Swapchain_AcquireNextImage_Response::EAcquireResult;
		using EPresentResult = RemoteGraphics::Msg::Swapchain_Present_Response::EPresentResult;


	protected:
		static constexpr uint	_MaxSwapchainLength = GraphicsConfig::MaxSwapchainLength;
		static constexpr uint	_ImageIndexBits		= 16;
		static constexpr uint	_MaxImageIndex		= (1u << _ImageIndexBits) - 1;

		using Semaphores_t		= StaticArray< RmSemaphoreID,			_MaxSwapchainLength >;
		using ImageIDs_t		= StaticArray< StrongAtom<ImageID>,		_MaxSwapchainLength >;
		using ImageViewIDs_t	= StaticArray< StrongAtom<ImageViewID>,	_MaxSwapchainLength >;

		struct MutableIdxBits
		{
			uint	semaphoreId		: 3;
			uint	imageIdx		: _ImageIndexBits;

			MutableIdxBits () __NE___ : semaphoreId{0}, imageIdx{_MaxImageIndex} {}
		};


	// variables
	protected:
		RDevice const*					_device			= null;
		mutable SharedMutex				_guard;

		StructAtomic< ushort2 >			_surfaceSize;
		StructAtomic< MutableIdxBits >	_indices;

		ImageIDs_t						_imageIDs			{};			// protected by '_guard'
		ImageViewIDs_t					_imageViewIDs		{};			// protected by '_guard'

		Array<SurfaceFormat>			_formats;
		Array<EPresentMode>				_presentModes;

		Semaphores_t					_imageAvailableSem	{};
		Semaphores_t					_renderFinishedSem	{};

		SwapchainDesc					_desc;							// protected by '_guard'
		NativeWindow					_nativeWnd;						// protected by '_guard'


	// methods
	protected:
		RSwapchain ()															__NE___;

	public:
		~RSwapchain ()															__NE___;

			bool	GetSurfaceFormats (OUT FeatureSet::SurfaceFormatSet_t &)	C_NE___;
		ND_ usize	GetSurfaceFormats (OUT SurfaceFormat*, usize count)			C_NE___;
		ND_ usize	GetPresentModes (OUT EPresentMode*, usize count)			C_NE___;

		ND_ EAcquireResult		AcquireNextImage ()								__NE___;
		ND_ EPresentResult		Present (RQueuePtr)								__NE___;

		ND_ bool				IsInitialized ()								C_NE___;

		ND_ RmSemaphoreID		GetImageAvailableSemaphore ()					C_NE___	{ return _imageAvailableSem[ GetCurrentSemaphoreIndex() ]; }
		ND_ RmSemaphoreID		GetRenderFinishedSemaphore ()					C_NE___	{ return _renderFinishedSem[ GetCurrentSemaphoreIndex() ]; }

		ND_ SwapchainDesc		GetDescription ()								C_NE___	{ SHAREDLOCK( _guard );  return _desc; }

		ND_ uint2				GetSurfaceSize ()								C_NE___	{ return uint2{_surfaceSize.load()}; }

		ND_ uint				GetCurrentImageIndex ()							C_NE___	{ return _indices.load().imageIdx; }
		ND_ uint				GetCurrentSemaphoreIndex ()						C_NE___	{ return _indices.load().semaphoreId; }
		ND_ bool				IsImageAcquired ()								C_NE___	{ return GetCurrentImageIndex() < _MaxSwapchainLength; }

		ND_ ImageAndViewID		GetCurrentImageAndViewID ()						C_NE___;
	};



	//
	// Remote Swapchain Initializer
	//

	class RSwapchainInitializer final : public RSwapchain
	{
	// methods
	public:
		explicit RSwapchainInitializer ()																			__NE___;

		ND_ bool  CreateSurface (const NativeWindow &, StringView dbgName = Default)								__NE___;
			void  DestroySurface ()																					__NE___;

		ND_ bool  IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage)		C_NE___;

		ND_ bool  Create (const uint2&			viewSize,
						  const SwapchainDesc&	desc,
						  StringView			dbgName	= Default)													__NE___;

			void  Destroy ()																						__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
