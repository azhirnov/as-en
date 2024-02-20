// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: yes
*/

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

    class VSwapchain : public Noncopyable
    {
    // types
    public:
        static constexpr VkImageUsageFlagBits   DefaultImageUsage   =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

        struct VSwapchainDesc
        {
            uint2                           viewSize;
            VkFormat                        colorFormat         = VK_FORMAT_R8G8B8A8_SRGB;
            VkColorSpaceKHR                 colorSpace          = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            uint                            minImageCount       = 2;
            VkPresentModeKHR                presentMode         = VK_PRESENT_MODE_FIFO_KHR;
            VkSurfaceTransformFlagBitsKHR   transform           = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            VkCompositeAlphaFlagBitsKHR     compositeAlpha      = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            VkImageUsageFlagBits            colorImageUsage     = DefaultImageUsage;
            EImageOpt                       colorImageOptions   = Default;
        };

    protected:
        static constexpr uint   _MaxSwapchainLength = 8;
        static constexpr uint   _ImageIndexBits     = 16;
        static constexpr uint   _MaxImageIndex      = (1u << _ImageIndexBits) - 1;

        using Images_t          = StaticArray< VkImage,                 _MaxSwapchainLength >;
        using ImageIDs_t        = StaticArray< StrongAtom<ImageID>,     _MaxSwapchainLength >;
        using ImageViewIDs_t    = StaticArray< StrongAtom<ImageViewID>, _MaxSwapchainLength >;
        using Semaphores_t      = StaticArray< VkSemaphore,             _MaxSwapchainLength >;

        struct MutableIdxBits
        {
            uint    semaphoreId     : 3;
            uint    imageIdx        : _ImageIndexBits;

            MutableIdxBits () __NE___ : semaphoreId{0}, imageIdx{_MaxImageIndex} {}
        };


    // variables
    protected:
        VDevice const*                  _device             = null;
        mutable SharedMutex             _guard;

        VkSwapchainKHR                  _vkSwapchain        = Default;  // protected by '_guard'
        VkSurfaceKHR                    _vkSurface          = Default;  // protected by '_guard'

        Atomic< uint >                  _surfaceSize;
        StructAtomic< MutableIdxBits >  _indices;

        Images_t                        _vkImages           {};         // protected by '_guard'
        ImageIDs_t                      _imageIDs           {};         // protected by '_guard'
        ImageViewIDs_t                  _imageViewIDs       {};         // protected by '_guard'

        Semaphores_t                    _imageAvailableSem  {};
        Semaphores_t                    _renderFinishedSem  {};

        SwapchainDesc                   _desc;                          // protected by '_guard'


    // methods
    protected:
        VSwapchain ()                                                           __NE___;

    public:
        ~VSwapchain ()                                                          __NE___;

        ND_ bool   IsSupported (VQueuePtr queue)                                C_NE___;

            bool   GetSurfaceFormats (OUT FeatureSet::SurfaceFormatSet_t &)     C_NE___;
        ND_ usize  GetSurfaceFormats (OUT SurfaceFormat*, usize count)          C_NE___;
        ND_ usize  GetPresentModes (OUT EPresentMode*, usize count)             C_NE___;

        ND_ VkResult  AcquireNextImage ()                                       __NE___;
        ND_ VkResult  Present (VQueuePtr queue,
                               ArrayView<VkSemaphore> renderFinished = Default) __NE___;

        ND_ bool                IsInitialized ()                                C_NE___ { return GetVkSwapchain() != Default; }

        ND_ VkSurfaceKHR        GetVkSurface ()                                 C_NE___ { SHAREDLOCK( _guard );  return _vkSurface; }
        ND_ VkSwapchainKHR      GetVkSwapchain ()                               C_NE___ { SHAREDLOCK( _guard );  return _vkSwapchain; }

        ND_ VSwapchainDesc      GetVDescription ()                              C_NE___;
        ND_ SwapchainDesc       GetDescription ()                               C_NE___ { SHAREDLOCK( _guard );  return _desc; }

        // same as output params in 'AcquireNextImage()'
        ND_ VkSemaphore         GetImageAvailableSemaphore ()                   C_NE___ { return _imageAvailableSem[ _indices.load().semaphoreId ]; }
        ND_ VkSemaphore         GetRenderFinishedSemaphore ()                   C_NE___ { return _renderFinishedSem[ _indices.load().semaphoreId ]; }

        ND_ uint2               GetSurfaceSize ()                               C_NE___;

        ND_ uint                GetCurrentImageIndex ()                         C_NE___ { return _indices.load().imageIdx; }
        ND_ bool                IsImageAcquired ()                              C_NE___ { return GetCurrentImageIndex() < _MaxImageIndex; }

        ND_ ImageAndViewID      GetCurrentImageAndViewID ()                     C_NE___ { return GetImageAndViewID( GetCurrentImageIndex() ); }
        ND_ ImageAndViewID      GetImageAndViewID (uint i)                      C_NE___;

        ND_ static ArrayView<const char*>  GetInstanceExtensions ()             __NE___;
    };



    //
    // Vulkan Swapchain Initializer
    //

    class VSwapchainInitializer final : public VSwapchain
    {
    // methods
    public:
        VSwapchainInitializer ()                                                                                            __NE___;

        ND_ bool  CreateSurface (const NativeWindow &, StringView dbgName = Default)                                        __NE___;
            void  DestroySurface ()                                                                                         __NE___;

        ND_ bool  IsSupported (VkPresentModeKHR presentMode, VkFormat colorFormat, VkImageUsageFlagBits colorImageUsage)    C_NE___;
        ND_ bool  IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage)             C_NE___;

        ND_ bool  ChooseColorFormat (INOUT VkFormat &colorFormat, INOUT VkColorSpaceKHR &colorSpace)                        C_NE___;

        ND_ bool  Create (const VSwapchainDesc& desc,
                          StringView            dbgName = Default)                                                          __NE___;

        ND_ bool  Create (const uint2&          viewSize,
                          const SwapchainDesc&  desc,
                          StringView            dbgName = Default)                                                          __NE___;

            void  Destroy ()                                                                                                __NE___;

    private:
        ND_ bool  _Create (const VSwapchainDesc& desc, StringView dbgName)                                                  __NE___;
        ND_ bool  _CreateColorAttachment (VResourceManager &resMngr, const VkSwapchainCreateInfoKHR &, EImageOpt options)   __NE___;
            void  _PrintInfo (const VkSwapchainCreateInfoKHR &)                                                             C_NE___;

        ND_ bool  _ChooseColorFormat (INOUT VkFormat &colorFormat, INOUT VkColorSpaceKHR &colorSpace)                       C_NE___;

        ND_ bool  _CreateSemaphores ()                                                                                      __NE___;
            void  _DestroySemaphores ()                                                                                     __NE___;

        ND_ bool  _GetImageUsage (OUT VkImageUsageFlags &imageUsage, VkPresentModeKHR presentMode,
                                  VkFormat colorFormat, const VkSurfaceCapabilitiesKHR &surfaceCaps)                        C_NE___;
        ND_ bool  _GetCompositeAlpha (INOUT VkCompositeAlphaFlagBitsKHR &compositeAlpha,
                                      const VkSurfaceCapabilitiesKHR &surfaceCaps)                                          C_NE___;
            void  _GetPresentMode (INOUT VkPresentModeKHR &presentMode)                                                     C_NE___;
            void  _GetSwapChainExtent (INOUT VkExtent2D &extent, const VkSurfaceCapabilitiesKHR &surfaceCaps)               C_NE___;
            void  _GetSurfaceTransform (INOUT VkSurfaceTransformFlagBitsKHR &transform,
                                        const VkSurfaceCapabilitiesKHR &surfaceCaps)                                        C_NE___;
            void  _GetSurfaceImageCount (INOUT uint &minImageCount, const VkSurfaceCapabilitiesKHR &surfaceCaps)            C_NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
