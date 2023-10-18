// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VulkanLoader.h"

# ifdef AE_PLATFORM_WINDOWS
#   include "base/Platforms/WindowsHeader.h"
#   include <vulkan/vulkan_win32.h>
# endif

# ifdef AE_PLATFORM_LINUX
#   include <X11/Xlib.h>
#   include <vulkan/vulkan_xlib.h>
# endif

# ifdef AE_PLATFORM_ANDROID
#   include <vulkan/vulkan_android.h>
# endif

# include "base/Algorithms/StringUtils.h"

# include "graphics/Vulkan/VSwapchain.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/VEnumToString.h"

namespace AE::Graphics
{

/*
=================================================
    constructor
=================================================
*/
    VSwapchain::VSwapchain () __NE___ :
        _indices{ MutableIdxBits{} }
    {}

/*
=================================================
    destructor
=================================================
*/
    VSwapchain::~VSwapchain () __NE___
    {
        SHAREDLOCK( _guard );
        ASSERT( _vkSwapchain == Default );
        ASSERT( _vkSurface == Default );
    }

/*
=================================================
    IsSupported
=================================================
*/
    bool  VSwapchain::IsSupported (VQueuePtr queue) C_NE___
    {
        SHAREDLOCK( _guard );
        CHECK_ERR( queue );
        CHECK_ERR( _device != null );
        CHECK_ERR( _device->GetVkPhysicalDevice() and _vkSurface != Default );

        VkBool32    supported = 0;
        VK_CHECK( vkGetPhysicalDeviceSurfaceSupportKHR( _device->GetVkPhysicalDevice(), uint(queue->familyIndex), _vkSurface, OUT &supported ));

        return supported;
    }

/*
=================================================
    GetSurfaceFormats
=================================================
*/
    bool  VSwapchain::GetSurfaceFormats (OUT FeatureSet::SurfaceFormatSet_t &result) C_NE___
    {
        result = Default;

        SHAREDLOCK( _guard );
        CHECK_ERR( _device != null );
        CHECK_ERR( _vkSurface != Default );
        CHECK_ERR( _device->GetVkPhysicalDevice() != Default );

        uint                        count   = 0;
        Array< VkSurfaceFormatKHR > surf_formats;

        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceFormatsKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, null ));
        CHECK_ERR( count > 0 );

        CATCH_ERR( surf_formats.resize( count ));
        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceFormatsKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, OUT surf_formats.data() ));

        for (auto& fmt : surf_formats)
        {
            ESurfaceFormat  sf = ESurfaceFormat_Cast( AEEnumCast( fmt.format ), AEEnumCast( fmt.colorSpace ));
            if ( sf < ESurfaceFormat::_Count )
                result.insert( sf );
        }
        return true;
    }

/*
=================================================
    GetSurfaceFormats
=================================================
*/
    usize  VSwapchain::GetSurfaceFormats (OUT SurfaceFormat* dst, const usize maxCount) C_NE___
    {
        SHAREDLOCK( _guard );
        CHECK_ERR( _device != null );
        CHECK_ERR( _vkSurface != Default );
        CHECK_ERR( _device->GetVkPhysicalDevice() != Default );
        CHECK_ERR( (dst != null) == (maxCount != 0) );

        uint                        count   = 0;
        Array< VkSurfaceFormatKHR > surf_formats;

        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceFormatsKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, null ));
        CHECK_ERR( count > 0 );

        CATCH_ERR( surf_formats.resize( count ));
        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceFormatsKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, OUT surf_formats.data() ));

        for (usize i = 0, cnt = Min( maxCount, surf_formats.size() ); i < cnt; ++i)
        {
            dst[i] = SurfaceFormat{ AEEnumCast(surf_formats[i].format), AEEnumCast(surf_formats[i].colorSpace) };
        }
        return count;
    }

/*
=================================================
    GetPresentModes
=================================================
*/
    usize  VSwapchain::GetPresentModes (OUT EPresentMode* dst, const usize maxCount) C_NE___
    {
        SHAREDLOCK( _guard );
        CHECK_ERR( _device != null );
        CHECK_ERR( _vkSurface != Default );
        CHECK_ERR( _device->GetVkPhysicalDevice() != Default );
        CHECK_ERR( (dst != null) == (maxCount != 0) );

        uint                        count       = 0;
        Array< VkPresentModeKHR >   present_modes;

        VK_CHECK_ERR( vkGetPhysicalDeviceSurfacePresentModesKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, null ));
        CHECK_ERR( count > 0 );

        CATCH_ERR( present_modes.resize( count ));
        VK_CHECK_ERR( vkGetPhysicalDeviceSurfacePresentModesKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, OUT present_modes.data() ));

        for (usize i = 0, cnt = Min( maxCount, present_modes.size() ); i < cnt; ++i)
        {
            dst[i] = AEEnumCast( present_modes[i] );
        }
        return count;
    }

/*
=================================================
    GetSurfaceSize
=================================================
*/
    uint2  VSwapchain::GetSurfaceSize () C_NE___
    {
        uint    tmp = _surfaceSize.load();
        return uint2{ tmp & 0xFFFF, tmp >> 16 };
    }

/*
=================================================
    GetImageAndViewID
=================================================
*/
    ImageAndViewID  VSwapchain::GetImageAndViewID (uint imageIdx) C_NE___
    {
        return  imageIdx < _imageIDs.size() ?
                    ImageAndViewID{ _imageIDs[imageIdx].Get(), _imageViewIDs[imageIdx].Get() } :
                    Default;
    }

/*
=================================================
    GetVDescription
=================================================
*/
    VSwapchain::VSwapchainDesc  VSwapchain::GetVDescription () C_NE___
    {
        const auto      desc    = GetDescription();
        VSwapchainDesc  result;
        result.viewSize         = GetSurfaceSize();
        result.colorFormat      = VEnumCast( desc.colorFormat );
        result.colorSpace       = VEnumCast( desc.colorSpace );
        result.minImageCount    = desc.minImageCount;
        result.presentMode      = VEnumCast( desc.presentMode );
        result.colorImageUsage  = VEnumCast( desc.usage, EMemoryType::DeviceLocal );
        return result;
    }

/*
=================================================
    AcquireNextImage
----
    Fence is not used because on all tested platforms it doesn't change anything.
----
    Intel: when used vsync vkAcquireNextImage() stalls on 1/refresh_rate.
=================================================
*/
    VkResult  VSwapchain::AcquireNextImage () __NE___
    {
        SHAREDLOCK( _guard );

        MutableIdxBits  cur_idx = _indices.load();
        CHECK_ERR( cur_idx.imageIdx == _MaxImageIndex, VK_RESULT_MAX_ENUM );
        CHECK_ERR( _vkSwapchain != Default, VK_RESULT_MAX_ENUM );

        cur_idx.imageIdx = _MaxImageIndex;

        uint        index = UMax;
        VkResult    res   = _device->vkAcquireNextImageKHR( _device->GetVkDevice(), _vkSwapchain, UMax, _imageAvailableSem[cur_idx.semaphoreId], Default, OUT &index );

        if_likely( res == VK_SUCCESS )
        {
            ASSERT( index < _vkImages.size() );
            cur_idx.imageIdx = index & _MaxImageIndex;
        }

        _indices.store( cur_idx );
        return res;
    }

/*
=================================================
    Present
----
    NV: when used vsync vkAcquireNextImage() stalls on 1/refresh_rate.
=================================================
*/
    VkResult  VSwapchain::Present (VQueuePtr queue, ArrayView<VkSemaphore> renderFinished) __NE___
    {
        SHAREDLOCK( _guard );
        CHECK_ERR( _vkSwapchain != Default, VK_RESULT_MAX_ENUM );
        CHECK_ERR( queue, VK_RESULT_MAX_ENUM );

        MutableIdxBits  cur_idx = _indices.load();
        CHECK_ERR( cur_idx.imageIdx < _MaxImageIndex, VK_RESULT_MAX_ENUM );

        const VkSwapchainKHR    swap_chains[]   = { _vkSwapchain };
        const uint              image_indices[] = { cur_idx.imageIdx };
        STATIC_ASSERT( CountOf(swap_chains) == CountOf(image_indices) );

        if ( renderFinished.empty() )
            renderFinished = ArrayView<VkSemaphore>{ &_renderFinishedSem[cur_idx.semaphoreId], 1 };

        // 'renderFinished' should contains semaphore which is returned by AcquireNextImage()
        DEBUG_ONLY({
            auto    req_sem = _renderFinishedSem[cur_idx.semaphoreId];
            bool    found   = false;

            for (auto sem : renderFinished) {
                found = (req_sem == sem);
            }
            ASSERT( found );
        })

        VkPresentInfoKHR    present_info = {};
        present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.swapchainCount     = uint(CountOf( swap_chains ));
        present_info.pSwapchains        = swap_chains;
        present_info.pImageIndices      = image_indices;
        present_info.waitSemaphoreCount = uint(renderFinished.size());
        present_info.pWaitSemaphores    = renderFinished.data();

        cur_idx.imageIdx    = _MaxImageIndex;
        cur_idx.semaphoreId ++;
        _indices.store( cur_idx );

        EXLOCK( queue->guard );
        return _device->vkQueuePresentKHR( queue->handle, &present_info );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    VSwapchainInitializer::VSwapchainInitializer () __NE___
    {}

/*
=================================================
    CreateSurface
=================================================
*/
    bool  VSwapchainInitializer::CreateSurface (const NativeWindow &window, StringView dbgName) __NE___
    {
        EXLOCK( _guard );

        _device = &RenderTaskScheduler().GetDevice();
        CHECK_ERR( _device->GetVkInstance() != Default );
        CHECK_ERR( _device->GetVExtensions().surface );
        CHECK_ERR( _vkSurface == Default );

        #if defined(AE_PLATFORM_WINDOWS)
        {
            CHECK_ERR( window.hInstance != null and window.hWnd != null );

            VkWin32SurfaceCreateInfoKHR     surface_info = {};

            surface_info.sType      = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surface_info.hinstance  = BitCast< HINSTANCE >(window.hInstance);
            surface_info.hwnd       = BitCast< HWND >(window.hWnd);

            auto  fpCreateWin32SurfaceKHR = BitCast<PFN_vkCreateWin32SurfaceKHR>( vkGetInstanceProcAddr( _device->GetVkInstance(), "vkCreateWin32SurfaceKHR" ));
            CHECK_ERR( fpCreateWin32SurfaceKHR );

            VK_CHECK_ERR( fpCreateWin32SurfaceKHR( _device->GetVkInstance(), &surface_info, null, OUT &_vkSurface ));

            _device->SetObjectName( _vkSurface, dbgName, VK_OBJECT_TYPE_SURFACE_KHR );

            AE_LOG_DBG( "Created WinAPI Vulkan surface" );
        }
        #elif defined(AE_PLATFORM_ANDROID)
        {
            CHECK_ERR( window.nativeWindow != null );

            VkAndroidSurfaceCreateInfoKHR   surface_info = {};

            surface_info.sType  = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
            surface_info.flags  = 0;
            surface_info.window = BitCast< ANativeWindow *>( window.nativeWindow );

            auto  fpCreateAndroidSurfaceKHR = BitCast<PFN_vkCreateAndroidSurfaceKHR>( vkGetInstanceProcAddr( _device->GetVkInstance(), "vkCreateAndroidSurfaceKHR" ));
            CHECK_ERR( fpCreateAndroidSurfaceKHR );

            VK_CHECK_ERR( fpCreateAndroidSurfaceKHR( _device->GetVkInstance(), &surface_info, null, OUT &_vkSurface ));

            _device->SetObjectName( _vkSurface, dbgName, VK_OBJECT_TYPE_SURFACE_KHR );

            AE_LOG_DBG( "Created Android Vulkan surface" );
        }
        #elif defined(AE_PLATFORM_LINUX)
        {
            CHECK_ERR( window.x11Window != null and window.x11Display != null );

            VkXlibSurfaceCreateInfoKHR  surface_info = {};
            surface_info.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
            surface_info.dpy    = BitCast< ::Display *>( window.x11Display );
            surface_info.window = BitCast< ::Window >( window.x11Window );

            auto  fpCreateXlibSurfaceKHR = BitCast<PFN_vkCreateXlibSurfaceKHR>( vkGetInstanceProcAddr( _device->GetVkInstance(), "vkCreateXlibSurfaceKHR" ));
            CHECK_ERR( fpCreateXlibSurfaceKHR );

            VK_CHECK_ERR( fpCreateXlibSurfaceKHR( _device->GetVkInstance(), &surface_info, null, OUT &_vkSurface ));

            _device->SetObjectName( _vkSurface, dbgName, VK_OBJECT_TYPE_SURFACE_KHR );

            AE_LOG_DBG( "Created X11 Vulkan surface" );
        }
        #else
            #error unsupported platform!
            return false;
        #endif

        // check that surface supported with current device
        bool    present_supported = false;

        for (auto& q : _device->GetQueues())
        {
            VkBool32    supported = 0;
            VK_CHECK( vkGetPhysicalDeviceSurfaceSupportKHR( _device->GetVkPhysicalDevice(), uint(q.familyIndex), _vkSurface, OUT &supported ));

            if ( supported ) {
                present_supported = true;
                break;
            }
        }

        CHECK_ERR( present_supported );

        CHECK_ERR( _CreateSemaphores() );
        return true;
    }

/*
=================================================
    GetInstanceExtensions
=================================================
*/
    ArrayView<const char*>  VSwapchainInitializer::GetInstanceExtensions () __NE___
    {
        static const char*  extensions[] = {
            #ifdef AE_PLATFORM_WINDOWS
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            #endif
            #ifdef AE_PLATFORM_ANDROID
                VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
            #endif
            #ifdef AE_PLATFORM_LINUX
                VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
            #endif
            #ifdef AE_PLATFORM_MACOS
                VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
            #endif
            #ifdef AE_PLATFORM_IOS
                VK_MVK_IOS_SURFACE_EXTENSION_NAME,
            #endif
        };
        return extensions;
    }

/*
=================================================
    DestroySurface
=================================================
*/
    void  VSwapchainInitializer::DestroySurface () __NE___
    {
        EXLOCK( _guard );
        CHECK( _vkSwapchain == Default );

        if ( _vkSurface != Default )
        {
            vkDestroySurfaceKHR( _device->GetVkInstance(), _vkSurface, null );
            _vkSurface = Default;

            AE_LOG_DBG( "Destroyed Vulkan surface" );
        }

        if ( _device->GetVkDevice() != Default )
        {
            _DestroySemaphores();
        }
    }

/*
=================================================
    ChooseColorFormat
=================================================
*/
    bool  VSwapchainInitializer::ChooseColorFormat (INOUT VkFormat &colorFormat, INOUT VkColorSpaceKHR &colorSpace) C_NE___
    {
        SHAREDLOCK( _guard );
        return _ChooseColorFormat( INOUT colorFormat, INOUT colorSpace );
    }

    bool  VSwapchainInitializer::_ChooseColorFormat (INOUT VkFormat &colorFormat, INOUT VkColorSpaceKHR &colorSpace) C_NE___
    {
        CHECK_ERR( _device->GetVkPhysicalDevice() and _vkSurface != Default );

        uint                        count               = 0;
        Array< VkSurfaceFormatKHR > surf_formats;
        const VkFormat              def_format          = VK_FORMAT_B8G8R8A8_UNORM;
        const VkColorSpaceKHR       def_space           = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        const VkFormat              required_format     = colorFormat;
        const VkColorSpaceKHR       required_colorspace = colorSpace;

        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceFormatsKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, null ));
        CHECK_ERR( count > 0 );

        CATCH_ERR( surf_formats.resize( count ));
        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceFormatsKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, OUT surf_formats.data() ));

        if ( count == 1                                     and
             surf_formats[0].format == VK_FORMAT_UNDEFINED )
        {
            colorFormat = required_format;
            colorSpace  = surf_formats[0].colorSpace;
        }
        else
        {
            usize   both_match_idx      = UMax;
            usize   format_match_idx    = UMax;
            usize   space_match_idx     = UMax;
            usize   def_format_idx      = 0;
            usize   def_space_idx       = 0;

            for (usize i = 0; i < surf_formats.size(); ++i)
            {
                const auto& surf_fmt = surf_formats[i];

                if ( surf_fmt.format     == required_format     and
                     surf_fmt.colorSpace == required_colorspace )
                {
                    both_match_idx = i;
                }
                else
                // separate check
                if ( surf_fmt.format     == required_format )
                    format_match_idx = i;
                else
                if ( surf_fmt.colorSpace == required_colorspace )
                    space_match_idx = i;

                // check with default
                if ( surf_fmt.format     == def_format )
                    def_format_idx = i;

                if ( surf_fmt.colorSpace == def_space )
                    def_space_idx = i;
            }

            usize   idx = UMax;

            if ( both_match_idx != UMax )
                idx = both_match_idx;
            else
            if ( format_match_idx != UMax )
                idx = format_match_idx;
            else
            if ( def_format_idx != UMax )
                idx = def_format_idx;

            // TODO: space_match_idx and def_space_idx are unused yet

            if ( idx >= surf_formats.size() )
                idx = 0;

            colorFormat = surf_formats[ idx ].format;
            colorSpace  = surf_formats[ idx ].colorSpace;
        }

        return true;
    }

/*
=================================================
    IsSupported
=================================================
*/
    bool  VSwapchainInitializer::IsSupported (const VkPresentModeKHR presentMode, const VkFormat colorFormat, const VkImageUsageFlagBits colorImageUsage) C_NE___
    {
        SHAREDLOCK( _guard );
        CHECK_ERR( _device->GetVkPhysicalDevice() and _vkSurface != Default );

        VkSurfaceCapabilitiesKHR    surf_caps;
        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &surf_caps ));

        VkImageUsageFlags   image_usage;
        CHECK_ERR( _GetImageUsage( OUT image_usage, presentMode, colorFormat, surf_caps ));

        if ( not AllBits( image_usage, colorImageUsage ))
            return false;

        VkImageFormatProperties image_props = {};
        VK_CHECK( vkGetPhysicalDeviceImageFormatProperties( _device->GetVkPhysicalDevice(), colorFormat, VK_IMAGE_TYPE_2D,
                                                            VK_IMAGE_TILING_OPTIMAL, colorImageUsage, 0, OUT &image_props ));

        if ( not AllBits( image_props.sampleCounts, VK_SAMPLE_COUNT_1_BIT ))
            return false;

        return true;
    }

    bool  VSwapchainInitializer::IsSupported (EPresentMode presentMode, EPixelFormat colorFormat, EImageUsage colorImageUsage) C_NE___
    {
        return IsSupported( VEnumCast(presentMode), VEnumCast(colorFormat), VEnumCast( colorImageUsage, Default ));
    }

/*
=================================================
    Create
=================================================
*/
    bool  VSwapchainInitializer::Create (const VSwapchainDesc&  desc,
                                         StringView             dbgName) __NE___
    {
        EXLOCK( _guard );

        return _Create( desc, dbgName );
    }

    bool  VSwapchainInitializer::_Create (const VSwapchainDesc&     info,
                                          StringView                dbgName) __NE___
    {
        CHECK_ERR( _device != null );
        CHECK_ERR( _device->GetVkPhysicalDevice() != Default and
                   _device->GetVkDevice()         != Default and
                   _vkSurface                     != Default );
        CHECK_ERR( _device->GetVExtensions().swapchain );
        CHECK_ERR( not IsImageAcquired() );     // TODO: it's allowed

        VkSurfaceCapabilitiesKHR    surf_caps;
        VK_CHECK_ERR( vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &surf_caps ));

        VkSwapchainKHR              old_swapchain   = _vkSwapchain;
        VkSwapchainCreateInfoKHR    swapchain_info  = {};

        swapchain_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.pNext            = null;
        swapchain_info.surface          = _vkSurface;
        swapchain_info.imageFormat      = info.colorFormat;
        swapchain_info.imageColorSpace  = info.colorSpace;
        swapchain_info.imageExtent      = { info.viewSize.x, info.viewSize.y };
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.minImageCount    = info.minImageCount;
        swapchain_info.oldSwapchain     = old_swapchain;
        swapchain_info.clipped          = VK_TRUE;
        swapchain_info.preTransform     = info.transform;
        swapchain_info.presentMode      = info.presentMode;
        swapchain_info.compositeAlpha   = info.compositeAlpha;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        _GetSwapChainExtent( INOUT swapchain_info.imageExtent, surf_caps );
        if ( swapchain_info.imageExtent.width == 0 or swapchain_info.imageExtent.height == 0 )
            return false;

        CHECK_ERR( _ChooseColorFormat( INOUT swapchain_info.imageFormat, INOUT swapchain_info.imageColorSpace ));
        _GetSurfaceImageCount( INOUT swapchain_info.minImageCount, surf_caps );
        _GetSurfaceTransform( INOUT swapchain_info.preTransform, surf_caps );
        _GetPresentMode( INOUT swapchain_info.presentMode );
        CHECK_ERR( _GetImageUsage( OUT swapchain_info.imageUsage, swapchain_info.presentMode, info.colorFormat, surf_caps ));
        CHECK_ERR( _GetCompositeAlpha( INOUT swapchain_info.compositeAlpha, surf_caps ));

        swapchain_info.imageUsage &= info.colorImageUsage;

        VK_CHECK_ERR( _device->vkCreateSwapchainKHR( _device->GetVkDevice(), &swapchain_info, null, OUT &_vkSwapchain ));

        // convert to AE-enums
        {
            EMemoryType         mem_type;
            _desc.colorFormat   = AEEnumCast( swapchain_info.imageFormat );
            _desc.colorSpace    = AEEnumCast( swapchain_info.imageColorSpace );
            _desc.presentMode   = AEEnumCast( swapchain_info.presentMode );
            _desc.minImageCount = CheckCast<ubyte>( swapchain_info.minImageCount );
            AEEnumCast( VkImageUsageFlagBits(swapchain_info.imageUsage), OUT _desc.usage, OUT mem_type );
        }

        // destroy obsolete resources
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();
        {
            for (auto& id : _imageViewIDs)
            {
                if ( auto tmp = id.Release() )
                    CHECK( res_mngr.DelayedRelease( INOUT tmp ) == 0 );
            }
            for (auto& id : _imageIDs)
            {
                if ( auto tmp = id.Release() )
                    CHECK( res_mngr.DelayedRelease( INOUT tmp ) == 1 );
            }
            if ( old_swapchain != Default )
                res_mngr.DelayedRelease( old_swapchain );
        }

        _vkImages.fill( Default );

        _surfaceSize.store( (swapchain_info.imageExtent.width & 0xFFFF) | ((swapchain_info.imageExtent.height & 0xFFFF) << 16) );

        _device->SetObjectName( _vkSwapchain, dbgName, VK_OBJECT_TYPE_SWAPCHAIN_KHR );

        CHECK_ERR( _CreateColorAttachment( res_mngr, swapchain_info, info.colorImageOptions ));

        // if created new swapchain
        if ( not old_swapchain )
        {
            _PrintInfo( swapchain_info );
        }
        return true;
    }

/*
=================================================
    Create
=================================================
*/
    bool  VSwapchainInitializer::Create (const uint2&           viewSize,
                                         const SwapchainDesc&   desc,
                                         StringView             dbgName) __NE___
    {
        VSwapchainDesc      swapchain_ci;
        swapchain_ci.viewSize           = viewSize;
        swapchain_ci.colorFormat        = VEnumCast( desc.colorFormat );
        swapchain_ci.colorSpace         = VEnumCast( desc.colorSpace );
        swapchain_ci.colorImageUsage    = VEnumCast( desc.usage, EMemoryType::DeviceLocal );
        swapchain_ci.colorImageOptions  = desc.options;
        swapchain_ci.minImageCount      = desc.minImageCount;
        swapchain_ci.presentMode        = VEnumCast( desc.presentMode );

        return Create( swapchain_ci, dbgName );
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VSwapchainInitializer::Destroy () __NE___
    {
        EXLOCK( _guard );

        CHECK( not IsImageAcquired() );

        if ( _device->GetVkDevice() != Default )
        {
            if ( _vkSwapchain != Default )
            {
                _device->vkDestroySwapchainKHR( _device->GetVkDevice(), _vkSwapchain, null );

                AE_LOG_DBG( "Destroyed Vulkan swapchain" );
            }
        }

        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        for (auto& id : _imageViewIDs)
        {
            if ( auto tmp = id.Release() )
                CHECK( res_mngr.DelayedRelease( INOUT tmp ) == 0 );
        }
        for (auto& id : _imageIDs)
        {
            if ( auto tmp = id.Release() )
                CHECK( res_mngr.DelayedRelease( INOUT tmp ) == 1 );
        }

        _vkImages.fill( Default );

        _vkSwapchain    = Default;
        _desc           = Default;
    }

/*
=================================================
    _CreateColorAttachment
=================================================
*/
    bool  VSwapchainInitializer::_CreateColorAttachment (VResourceManager &resMngr, const VkSwapchainCreateInfoKHR &info, EImageOpt options) __NE___
    {
        uint    count = uint(_vkImages.size());
        VK_CHECK_ERR( _device->vkGetSwapchainImagesKHR( _device->GetVkDevice(), _vkSwapchain, OUT &count, OUT _vkImages.data() ));
        CHECK_ERR( count > 0 );
        CHECK_ERR( count <= _vkImages.size() );

        VulkanImageDesc     desc;
        desc.dimension      = uint3(info.imageExtent.width, info.imageExtent.height, 1);
        desc.arrayLayers    = 1;
        desc.flags          = VkImageCreateFlagBits(0);
        desc.options        = options;
        desc.format         = info.imageFormat;
        desc.imageType      = VK_IMAGE_TYPE_2D;
        desc.maxLevels      = 1;
        desc.samples        = VK_SAMPLE_COUNT_1_BIT;
        desc.usage          = VkImageUsageFlagBits(info.imageUsage);
        desc.memFlags       = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        desc.canBeDestroyed = false;    // images created by swapchain, so don't destroy they

        ImageViewDesc   view_desc;

        for (usize i = 0; i < count; ++i)
        {
            desc.image = _vkImages[i];

            Unused( _imageIDs[i].Attach( resMngr.CreateImage( desc, "SwapchainImage_"s + ToString(i) )));
            CHECK_ERR( _imageIDs[i] );

            Unused( _imageViewIDs[i].Attach( resMngr.CreateImageView( view_desc, _imageIDs[i], "SwapchainImageView_"s + ToString(i) )));
            CHECK_ERR( _imageViewIDs[i] );
        }
        return true;
    }

/*
=================================================
    _GetCompositeAlpha
=================================================
*/
    bool  VSwapchainInitializer::_GetCompositeAlpha (INOUT VkCompositeAlphaFlagBitsKHR &compositeAlpha,
                                                     const VkSurfaceCapabilitiesKHR &surfaceCaps) C_NE___
    {
        const VkCompositeAlphaFlagBitsKHR       composite_alpha_flags[] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };

        if ( AllBits( surfaceCaps.supportedCompositeAlpha, compositeAlpha ))
            return true;    // keep current

        compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;

        for (auto& flag : composite_alpha_flags)
        {
            if ( AllBits( surfaceCaps.supportedCompositeAlpha, flag ))
            {
                compositeAlpha = flag;
                return true;
            }
        }

        RETURN_ERR( "no suitable composite alpha flags found!" );
    }

/*
=================================================
    _GetSwapChainExtent
=================================================
*/
    void  VSwapchainInitializer::_GetSwapChainExtent (INOUT VkExtent2D &extent,
                                                      const VkSurfaceCapabilitiesKHR &surfaceCaps) C_NE___
    {
        if ( surfaceCaps.currentExtent.width  == UMax and
             surfaceCaps.currentExtent.height == UMax )
        {
            // keep window size
        }
        else
        {
            extent.width  = surfaceCaps.currentExtent.width;
            extent.height = surfaceCaps.currentExtent.height;
        }

        extent.width  = Clamp( extent.width,  surfaceCaps.minImageExtent.width,  surfaceCaps.maxImageExtent.width );
        extent.height = Clamp( extent.height, surfaceCaps.minImageExtent.height, surfaceCaps.maxImageExtent.height );
    }

/*
=================================================
    _GetPresentMode
=================================================
*/
    void  VSwapchainInitializer::_GetPresentMode (INOUT VkPresentModeKHR &presentMode) C_NE___
    {
        uint                        count       = 0;
        Array< VkPresentModeKHR >   present_modes;

        VK_CHECK( vkGetPhysicalDeviceSurfacePresentModesKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, null ));
        CHECK_ERRV( count > 0 );

        CATCH_ERRV( present_modes.resize( count ));
        VK_CHECK( vkGetPhysicalDeviceSurfacePresentModesKHR( _device->GetVkPhysicalDevice(), _vkSurface, OUT &count, OUT present_modes.data() ));

        bool    required_mode_supported     = false;
        bool    fifo_mode_supported         = false;
        bool    mailbox_mode_supported      = false;
        bool    immediate_mode_supported    = false;

        for (usize i = 0; i < present_modes.size(); ++i)
        {
            required_mode_supported     |= (present_modes[i] == presentMode);
            fifo_mode_supported         |= (present_modes[i] == VK_PRESENT_MODE_FIFO_KHR);
            mailbox_mode_supported      |= (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR);
            immediate_mode_supported    |= (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR);
        }

        if ( required_mode_supported )
            return; // keep current

        if ( mailbox_mode_supported )
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        else
        if ( fifo_mode_supported )
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
        else
        if ( immediate_mode_supported )
            presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

/*
=================================================
    _GetSurfaceImageCount
=================================================
*/
    void  VSwapchainInitializer::_GetSurfaceImageCount (INOUT uint &minImageCount, const VkSurfaceCapabilitiesKHR &surfaceCaps) C_NE___
    {
        if ( minImageCount < surfaceCaps.minImageCount )
        {
            minImageCount = surfaceCaps.minImageCount;
        }

        if ( surfaceCaps.maxImageCount > 0 and minImageCount > surfaceCaps.maxImageCount )
        {
            minImageCount = surfaceCaps.maxImageCount;
        }
    }

/*
=================================================
    _GetSurfaceTransform
----
    https://github.com/KhronosGroup/Vulkan-Samples/tree/main/samples/performance/surface_rotation
=================================================
*/
    void  VSwapchainInitializer::_GetSurfaceTransform (INOUT VkSurfaceTransformFlagBitsKHR &transform,
                                                       const VkSurfaceCapabilitiesKHR &surfaceCaps) C_NE___
    {
        //if ( AllBits( surfaceCaps.supportedTransforms, transform ))
        //  return; // keep current

        if ( AllBits( surfaceCaps.supportedTransforms, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ))
        {
            // TODO: bad for performance
            transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        else
        {
            transform = surfaceCaps.currentTransform;
        }
    }

/*
=================================================
    _GetImageUsage
=================================================
*/
    bool  VSwapchainInitializer::_GetImageUsage (OUT VkImageUsageFlags &imageUsage, const VkPresentModeKHR presentMode,
                                                 const VkFormat colorFormat, const VkSurfaceCapabilitiesKHR &surfaceCaps) C_NE___
    {
        if ( presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR   or
             presentMode == VK_PRESENT_MODE_MAILBOX_KHR     or
             presentMode == VK_PRESENT_MODE_FIFO_KHR        or
             presentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR )
        {
            imageUsage = surfaceCaps.supportedUsageFlags;
        }
        else
        if ( presentMode == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR   or
             presentMode == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR )
        {
            if ( not _device->GetVExtensions().surfaceCaps2 )
                return false;

            VkPhysicalDeviceSurfaceInfo2KHR surf_info = {};
            surf_info.sType     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
            surf_info.surface   = _vkSurface;

            VkSurfaceCapabilities2KHR   surf_caps2;
            VK_CHECK( vkGetPhysicalDeviceSurfaceCapabilities2KHR( _device->GetVkPhysicalDevice(), &surf_info, OUT &surf_caps2 ));

            for (VkBaseInStructure const *iter = reinterpret_cast<VkBaseInStructure const *>(&surf_caps2);
                    iter != null;
                    iter = iter->pNext)
            {
                if ( iter->sType == VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR )
                {
                    imageUsage = reinterpret_cast<VkSharedPresentSurfaceCapabilitiesKHR const*>(iter)->sharedPresentSupportedUsageFlags;
                    break;
                }
            }
        }
        else
        {
            return false;
        }

        ASSERT( AllBits( imageUsage, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ));
        imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


        // validation:
        VkFormatProperties  format_props;
        vkGetPhysicalDeviceFormatProperties( _device->GetVkPhysicalDevice(), colorFormat, OUT &format_props );

        CHECK_ERR( AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT ));
        ASSERT( AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT ));

        if ( AllBits( imageUsage, VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) and
             (not AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_SRC_BIT ) or
              not AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_BLIT_DST_BIT )) )
        {
            imageUsage &= ~VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }

        if ( AllBits( imageUsage, VK_IMAGE_USAGE_TRANSFER_DST_BIT ) and
             not AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_DST_BIT ))
        {
            imageUsage &= ~VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        if ( AllBits( imageUsage, VK_IMAGE_USAGE_STORAGE_BIT ) and
             not AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT ))
        {
            imageUsage &= ~VK_IMAGE_USAGE_STORAGE_BIT;
        }

        if ( AllBits( imageUsage, VK_IMAGE_USAGE_SAMPLED_BIT ) and
             not AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT ))
        {
            imageUsage &= ~VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        if ( AllBits( imageUsage, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ) and
             not AllBits( format_props.optimalTilingFeatures, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ))
        {
            imageUsage &= ~VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }

        return true;
    }

/*
=================================================
    _PrintInfo
=================================================
*/
    void  VSwapchainInitializer::_PrintInfo (const VkSwapchainCreateInfoKHR &info) C_NE___
    {
        AE_LOGI( "Created Vulkan Swapchain:"s
            << "\n  format:          " << VkFormat_ToString( info.imageFormat )
            << "\n  color space:     " << VkColorSpaceKHR_ToString( info.imageColorSpace )
            << "\n  present mode:    " << VkPresentModeKHR_ToString( info.presentMode )
            << "\n  transform:       " << VkSurfaceTransformFlagBitsKHR_ToString( info.preTransform )
            << "\n  composite alpha: " << VkCompositeAlphaFlagBitsKHR_ToString( info.compositeAlpha )
            << "\n  image usage:     " << VkImageUsageFlags_ToString( info.imageUsage )
            << "\n  min image count: " << ToString( info.minImageCount )
            << "\n----"
        );
    }

/*
=================================================
    _CreateSemaphores
=================================================
*/
    bool  VSwapchainInitializer::_CreateSemaphores () __NE___
    {
        const auto  CreateSems = [this] (Semaphores_t &semaphores, StringView name) -> bool
        {{
            VkSemaphoreCreateInfo   info = {};
            info.sType  = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            for (auto& id : semaphores)
            {
                if ( id != Default )
                    continue;

                VK_CHECK_ERR( _device->vkCreateSemaphore( _device->GetVkDevice(), &info, null, OUT &id ));
                _device->SetObjectName( id, name, VK_OBJECT_TYPE_SEMAPHORE );
            }
            return true;
        }};

        return  CreateSems( _imageAvailableSem, "Swapchain-ImageAvailable" ) and
                CreateSems( _renderFinishedSem, "Swapchain-RenderFinished" );
    }

/*
=================================================
    _DestroySemaphores
=================================================
*/
    void  VSwapchainInitializer::_DestroySemaphores () __NE___
    {
        const auto  DestroySems = [this] (Semaphores_t &semaphores)
        {{
            for (auto& id : semaphores)
            {
                if ( id != Default )
                {
                    _device->vkDestroySemaphore( _device->GetVkDevice(), id, null );
                    id = Default;
                }
            }
        }};

        DestroySems( _imageAvailableSem );
        DestroySems( _renderFinishedSem );
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
