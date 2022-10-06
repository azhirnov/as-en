// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/NativeWindow.h"
# include "graphics/Public/SwapchainDesc.h"
# include "graphics/Vulkan/VDevice.h"

namespace AE::Graphics
{

	//
	// Vulkan Swapchain
	//

	class VSwapchain : protected VulkanDeviceFn, public Noncopyable
	{
	// types
	public:
		static constexpr VkImageUsageFlagBits	DefaultImageUsage	=
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

	protected:
		static constexpr uint	MaxSwapchainLength = 8;

		using Images_t			= FixedArray< VkImage, MaxSwapchainLength >;
		using ImageIDs_t		= FixedArray< Strong<ImageID>, MaxSwapchainLength >;
		using ImageViewIDs_t	= FixedArray< Strong<ImageViewID>, MaxSwapchainLength >;
		using Semaphores_t		= StaticArray< VkSemaphore, MaxSwapchainLength >;


	// variables
	protected:
		VDevice const&					_device;

		uint2							_surfaceSize;
		VkSwapchainKHR					_vkSwapchain		= Default;
		VkSurfaceKHR					_vkSurface			= Default;

		uint							_semaphoreId	: 3;
		uint							_currImageIndex	: 16;
		Images_t						_vkImages;
		ImageIDs_t						_imageIDs;
		ImageViewIDs_t					_imageViewIDs;

		Semaphores_t					_imageAvailableSem	= {};
		Semaphores_t					_renderFinishedSem	= {};

		VkFormat						_vkColorFormat		= VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR					_vkColorSpace		= VK_COLOR_SPACE_MAX_ENUM_KHR;
		uint							_minImageCount		= 2;
		VkSurfaceTransformFlagBitsKHR	_vkPreTransform		= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		VkPresentModeKHR				_vkPresentMode		= VK_PRESENT_MODE_FIFO_KHR;
		VkCompositeAlphaFlagBitsKHR		_vkCompositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		VkImageUsageFlagBits			_vkColorImageUsage	= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		EPixelFormat					_colorFormat		= Default;
		
		DRC_ONLY( RWDataRaceCheck		_drCheck; )


	// methods
	protected:
		explicit VSwapchain (const VDevice &dev);

	public:
		~VSwapchain ();

		ND_ bool  IsSupported (VQueuePtr queue) const;
		
			bool  GetColorFormats (OUT FeatureSet::PixelFormatSet_t &formats) const;
			
		ND_ VkResult  AcquireNextImage ();
		ND_ VkResult  AcquireNextImage (OUT VkSemaphore &imageAvailable, OUT VkSemaphore &renderFinished);
		ND_ VkResult  Present (VQueuePtr queue, ArrayView<VkSemaphore> renderFinished = Default);
		
		ND_ bool						IsInitialized ()			const	{ return GetVkSwapchain() != Default; }
		
		ND_ VkSurfaceKHR				GetVkSurface ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _vkSurface; }
		ND_ VkSwapchainKHR				GetVkSwapchain ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _vkSwapchain; }

		// same as output params in 'AcquireNextImage()'
		ND_ VkSemaphore					GetImageAvailableSemaphore ()const	{ DRC_SHAREDLOCK( _drCheck );  return _imageAvailableSem[_semaphoreId]; }
		ND_ VkSemaphore					GetRenderFinishedSemaphore ()const	{ DRC_SHAREDLOCK( _drCheck );  return _renderFinishedSem[_semaphoreId]; }

		ND_ uint2						GetSurfaceSize ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _surfaceSize; }

		ND_ EPixelFormat				GetColorFormat ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _colorFormat; }

		ND_ VkFormat					GetVkColorFormat ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _vkColorFormat; }
		ND_ VkColorSpaceKHR				GetVkColorSpace ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _vkColorSpace; }
		ND_ VkSurfaceTransformFlagBitsKHR GetVkPreTransformFlags ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _vkPreTransform; }
		ND_ VkPresentModeKHR			GetVkPresentMode ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _vkPresentMode; }
		ND_ VkCompositeAlphaFlagBitsKHR	GetVkCompositeAlphaMode ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _vkCompositeAlpha; }

		ND_ uint						GetSwapchainLength ()		const	{ DRC_SHAREDLOCK( _drCheck );  return uint(_vkImages.size()); }
		ND_ uint						GetCurretImageIndex ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _currImageIndex; }
		ND_ bool						IsImageAcquired ()			const	{ DRC_SHAREDLOCK( _drCheck );  return GetCurretImageIndex() < GetSwapchainLength(); }

		ND_ VkImageUsageFlagBits		GetVkImageUsage ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _vkColorImageUsage; }
		ND_ VkImage						GetVkCurrentImage ()		const;
		ND_ Tuple<ImageID, ImageViewID>	GetCurrentImageAndViewID ()	const;
	};

	

	//
	// Vulkan Swapchain Initializer
	//

	class VSwapchainInitializer final : public VSwapchain
	{
	// variables
	private:
		Ptr<VResourceManager>	_resMngr;


	// methods
	public:
		explicit VSwapchainInitializer (const VDevice &dev);

		ND_ bool  CreateSurface (const NativeWindow &, StringView dbgName = {});
			void  DestroySurface ();

		ND_ bool  IsSupported (VkPresentModeKHR presentMode, VkFormat colorFormat, VkImageUsageFlagBits colorImageUsage) const;
		ND_ bool  IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage) const;
		
		ND_ bool  ChooseColorFormat (INOUT VkFormat &colorFormat, INOUT VkColorSpaceKHR &colorSpace) const;

		struct CreateInfo
		{
			uint2							viewSize;
			VkFormat						colorFormat			= VK_FORMAT_R8G8B8A8_SRGB;
			VkColorSpaceKHR					colorSpace			= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			uint							minImageCount		= 2;
			VkPresentModeKHR				presentMode			= VK_PRESENT_MODE_FIFO_KHR;
			VkSurfaceTransformFlagBitsKHR	transform			= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			VkCompositeAlphaFlagBitsKHR		compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			VkImageUsageFlagBits			colorImageUsage		= DefaultImageUsage;
		};
		ND_ bool  Create (VResourceManager*	resMngr,
						  const CreateInfo&	info,
						  StringView		dbgName	= {});
		
		ND_ bool  Create (VResourceManager&		resMngr,
						  const uint2&			viewSize,
						  const SwapchainDesc&	desc,
						  StringView			dbgName	= {});

			void  Destroy ();

		ND_ bool  Recreate (const uint2 &size);
		
		ND_ static ArrayView<const char*>  GetInstanceExtensions ();

	private:
		ND_ bool  _Create (VResourceManager* resMngr, const CreateInfo& info, StringView dbgName);
		ND_ bool  _CreateColorAttachment (VResourceManager* resMngr);
			void  _PrintInfo () const;

		ND_ bool  _CreateSemaphores ();
			void  _DestroySemaphores ();

		ND_ bool  _GetImageUsage (OUT VkImageUsageFlags &imageUsage,	VkPresentModeKHR presentMode, VkFormat colorFormat, const VkSurfaceCapabilitiesKHR &surfaceCaps) const;
		ND_ bool  _GetCompositeAlpha (INOUT VkCompositeAlphaFlagBitsKHR &compositeAlpha, const VkSurfaceCapabilitiesKHR &surfaceCaps) const;
			void  _GetPresentMode (INOUT VkPresentModeKHR &presentMode) const;
			void  _GetSwapChainExtent (INOUT VkExtent2D &extent, const VkSurfaceCapabilitiesKHR &surfaceCaps) const;
			void  _GetSurfaceTransform (INOUT VkSurfaceTransformFlagBitsKHR &transform, const VkSurfaceCapabilitiesKHR &surfaceCaps) const;
			void  _GetSurfaceImageCount (INOUT uint &minImageCount, const VkSurfaceCapabilitiesKHR &surfaceCaps) const;
	};


}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
