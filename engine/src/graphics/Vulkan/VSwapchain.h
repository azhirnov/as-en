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
		static constexpr uint	_MaxSwapchainLength = 8;
		static constexpr uint	_ImageIndexBits		= 16;
		static constexpr uint	_MaxImageIndex		= (1u << _ImageIndexBits) - 1;

		using Images_t			= FixedArray< VkImage,				_MaxSwapchainLength >;
		using ImageIDs_t		= FixedArray< Strong<ImageID>,		_MaxSwapchainLength >;
		using ImageViewIDs_t	= FixedArray< Strong<ImageViewID>,	_MaxSwapchainLength >;
		using Semaphores_t		= StaticArray< VkSemaphore,			_MaxSwapchainLength >;


	// variables
	protected:
		VDevice const&					_device;

		uint2							_surfaceSize;
		VkSwapchainKHR					_vkSwapchain		= Default;
		VkSurfaceKHR					_vkSurface			= Default;

		uint							_semaphoreId	: 3;
		uint							_currImageIndex	: _ImageIndexBits;

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
		
		DRC_ONLY(
			RWDataRaceCheck				_drCheck;
			RWDataRaceCheck				_drCheck2;		// for '_currImageIndex' and '_semaphoreId'
		)


	// methods
	protected:
		explicit VSwapchain (const VDevice &dev)						__NE___;

	public:
		~VSwapchain ()													__NE___;

		ND_ bool  IsSupported (VQueuePtr queue)							C_NE___;
		
			bool  GetColorFormats (OUT FeatureSet::PixelFormatSet_t &formats) C_NE___;
			
		ND_ VkResult  AcquireNextImage ()																	__NE___;
		ND_ VkResult  AcquireNextImage (OUT VkSemaphore &imageAvailable, OUT VkSemaphore &renderFinished)	__NE___;
		ND_ VkResult  Present (VQueuePtr queue, ArrayView<VkSemaphore> renderFinished = Default)			__NE___;
		
		ND_ bool						IsInitialized ()				C_NE___	{ return GetVkSwapchain() != Default; }
		
		ND_ VkSurfaceKHR				GetVkSurface ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkSurface; }
		ND_ VkSwapchainKHR				GetVkSwapchain ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkSwapchain; }

		// same as output params in 'AcquireNextImage()'
		ND_ VkSemaphore					GetImageAvailableSemaphore ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck2 ); return _imageAvailableSem[_semaphoreId]; }
		ND_ VkSemaphore					GetRenderFinishedSemaphore ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck2 ); return _renderFinishedSem[_semaphoreId]; }

		ND_ uint2						GetSurfaceSize ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _surfaceSize; }

		ND_ EPixelFormat				GetColorFormat ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _colorFormat; }

		ND_ VkFormat					GetVkColorFormat ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkColorFormat; }
		ND_ VkColorSpaceKHR				GetVkColorSpace ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkColorSpace; }
		ND_ VkSurfaceTransformFlagBitsKHR GetVkPreTransformFlags ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkPreTransform; }
		ND_ VkPresentModeKHR			GetVkPresentMode ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkPresentMode; }
		ND_ VkCompositeAlphaFlagBitsKHR	GetVkCompositeAlphaMode ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkCompositeAlpha; }

		ND_ uint						GetSwapchainLength ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint(_vkImages.size()); }
		ND_ uint						GetCurretImageIndex ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck2 ); return _currImageIndex; }
		ND_ bool						IsImageAcquired ()				C_NE___	{ return GetCurretImageIndex() < GetSwapchainLength(); }

		ND_ VkImageUsageFlagBits		GetVkImageUsage ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vkColorImageUsage; }
		ND_ VkImage						GetVkCurrentImage ()			C_NE___;
		ND_ ImageAndViewID				GetCurrentImageAndViewID ()		C_NE___;
		ND_ ImageAndViewID				GetImageAndViewID (uint i)		C_NE___;
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
		explicit VSwapchainInitializer (const VDevice &dev)																	__NE___;

		ND_ bool  CreateSurface (const NativeWindow &, StringView dbgName = Default)										__NE___;
			void  DestroySurface ()																							__NE___;

		ND_ bool  IsSupported (VkPresentModeKHR presentMode, VkFormat colorFormat, VkImageUsageFlagBits colorImageUsage)	C_NE___;
		ND_ bool  IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage)				C_NE___;
		
		ND_ bool  ChooseColorFormat (INOUT VkFormat &colorFormat, INOUT VkColorSpaceKHR &colorSpace)						C_NE___;

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
						  StringView		dbgName	= Default)																__NE___;
		
		ND_ bool  Create (VResourceManager&		resMngr,
						  const uint2&			viewSize,
						  const SwapchainDesc&	desc,
						  StringView			dbgName	= Default)															__NE___;

			void  Destroy ()																								__NE___;
		
		ND_ static ArrayView<const char*>  GetInstanceExtensions ()															__NE___;

	private:
		ND_ bool  _Create (VResourceManager* resMngr, const CreateInfo& info, StringView dbgName)							__NE___;
		ND_ bool  _CreateColorAttachment (VResourceManager* resMngr)														__NE___;
			void  _PrintInfo ()																								C_NE___;

		ND_ bool  _CreateSemaphores ()																						__NE___;
			void  _DestroySemaphores ()																						__NE___;

		ND_ bool  _GetImageUsage (OUT VkImageUsageFlags &imageUsage,	VkPresentModeKHR presentMode, VkFormat colorFormat, const VkSurfaceCapabilitiesKHR &surfaceCaps)C_NE___;
		ND_ bool  _GetCompositeAlpha (INOUT VkCompositeAlphaFlagBitsKHR &compositeAlpha, const VkSurfaceCapabilitiesKHR &surfaceCaps)									C_NE___;
			void  _GetPresentMode (INOUT VkPresentModeKHR &presentMode)																									C_NE___;
			void  _GetSwapChainExtent (INOUT VkExtent2D &extent, const VkSurfaceCapabilitiesKHR &surfaceCaps)															C_NE___;
			void  _GetSurfaceTransform (INOUT VkSurfaceTransformFlagBitsKHR &transform, const VkSurfaceCapabilitiesKHR &surfaceCaps)									C_NE___;
			void  _GetSurfaceImageCount (INOUT uint &minImageCount, const VkSurfaceCapabilitiesKHR &surfaceCaps)														C_NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
