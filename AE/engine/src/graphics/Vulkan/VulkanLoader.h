// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# if not defined(VK_NO_PROTOTYPES) and defined(VULKAN_CORE_H_)
#   error invalid configuration, include vulkan.h after this file.
# endif

# ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
# endif

# include "base/Defines/StdInclude.h"
# define VK_ENABLE_BETA_EXTENSIONS
# ifdef AE_COMPILER_MSVC
#   pragma warning (push, 0)
#   include <vulkan/vulkan.h>
#   pragma warning (pop)
# else
#   include <vulkan/vulkan.h>
# endif

# include "graphics/Graphics.pch.h"

namespace AE::Graphics
{
    using namespace AE::Base;

#   define VKLOADER_STAGE_DECLFNPOINTER
#    include "vulkan_loader/fn_vulkan_lib.h"
#    include "vulkan_loader/fn_vulkan_inst.h"
#   undef  VKLOADER_STAGE_DECLFNPOINTER

#   define VKLOADER_STAGE_INLINEFN
#    include "vulkan_loader/fn_vulkan_lib.h"
#    include "vulkan_loader/fn_vulkan_inst.h"
#   undef  VKLOADER_STAGE_INLINEFN

    extern PFN_vkGetInstanceProcAddr  _var_vkGetInstanceProcAddr;
    ND_ VKAPI_ATTR forceinline PFN_vkVoidFunction vkGetInstanceProcAddr (VkInstance instance, const char * pName) { return _var_vkGetInstanceProcAddr( instance, pName ); }



    //
    // Vulkan Device Functions Table
    //
    struct VulkanDeviceFnTable final : Noncopyable
    {
        friend struct VulkanLoader;
        friend class VulkanDeviceFn;

    // variables
    public:
#       define VKLOADER_STAGE_FNPOINTER
#        include "vulkan_loader/fn_vulkan_dev.h"
#       undef  VKLOADER_STAGE_FNPOINTER


    // methods
    public:
        VulkanDeviceFnTable () __NE___ {}
    };



    //
    // Vulkan Device Functions
    //
    class VulkanDeviceFn
    {
    // variables
    private:
        VulkanDeviceFnTable const *     _table = null;

    // methods
    protected:
        void  VulkanDeviceFn_Init (const VulkanDeviceFn &other)         __NE___;
        void  VulkanDeviceFn_Init (const VulkanDeviceFnTable *table)    __NE___;

    public:
        VulkanDeviceFn ()                                               __NE___ : _table{null} {}
        VulkanDeviceFn (const VulkanDeviceFn &)                         __NE___ = default;
        explicit VulkanDeviceFn (VulkanDeviceFnTable *table)            __NE___ : _table{table} {}

        ND_ VulkanDeviceFnTable const* _GetVkTable ()                   C_NE___ { return _table; }

#       define VKLOADER_STAGE_INLINEFN
#        include "vulkan_loader/fn_vulkan_dev.h"
#       undef  VKLOADER_STAGE_INLINEFN
    };



    //
    // Vulkan Loader
    //
    struct VulkanLoader final : Noninstanceable
    {
        ND_ static bool  Initialize (NtStringView libName = {})                                                         __NE___;
        ND_ static bool  LoadInstance (VkInstance instance)                                                             __NE___;
            static void  Unload ()                                                                                      __NE___;

        ND_ static bool  LoadDevice (VkDevice device, OUT VulkanDeviceFnTable &table)                                   __NE___;
            static void  ResetDevice (OUT VulkanDeviceFnTable &table)                                                   __NE___;

            static void  SetupInstanceBackwardCompatibility (Version2 instanceVersion)                                  __NE___;
            static void  SetupDeviceBackwardCompatibility (Version2 deviceVersion, INOUT VulkanDeviceFnTable &table)    __NE___;
    };

} // AE::Graphics

#undef VULKAN_ENUM_BIT_OPERATORS


// check for 'VulkanDeviceFnTable' structure size mismatch
# ifdef AE_CPP_DETECT_MISMATCH

#  if defined(VK_USE_PLATFORM_ANDROID_KHR) and VK_USE_PLATFORM_ANDROID_KHR
#   pragma detect_mismatch( "VK_USE_PLATFORM_ANDROID_KHR", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_ANDROID_KHR", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_IOS_MVK) and VK_USE_PLATFORM_IOS_MVK
#   pragma detect_mismatch( "VK_USE_PLATFORM_IOS_MVK", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_IOS_MVK", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_MACOS_MVK) and VK_USE_PLATFORM_MACOS_MVK
#   pragma detect_mismatch( "VK_USE_PLATFORM_MACOS_MVK", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_MACOS_MVK", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_MIR_KHR) and VK_USE_PLATFORM_MIR_KHR
#   pragma detect_mismatch( "VK_USE_PLATFORM_MIR_KHR", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_MIR_KHR", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_VI_NN) and VK_USE_PLATFORM_VI_NN
#   pragma detect_mismatch( "VK_USE_PLATFORM_VI_NN", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_VI_NN", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_WAYLAND_KHR) and VK_USE_PLATFORM_WAYLAND_KHR
#   pragma detect_mismatch( "VK_USE_PLATFORM_WAYLAND_KHR", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_WAYLAND_KHR", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_WIN32_KHR) and VK_USE_PLATFORM_WIN32_KHR
#   pragma detect_mismatch( "VK_USE_PLATFORM_WIN32_KHR", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_WIN32_KHR", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_XCB_KHR) and VK_USE_PLATFORM_XCB_KHR
#   pragma detect_mismatch( "VK_USE_PLATFORM_XCB_KHR", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_XCB_KHR", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_XLIB_KHR) and VK_USE_PLATFORM_XLIB_KHR
#   pragma detect_mismatch( "VK_USE_PLATFORM_XLIB_KHR", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_XLIB_KHR", "0" )
#  endif

#  if defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT) and VK_USE_PLATFORM_XLIB_XRANDR_EXT
#   pragma detect_mismatch( "VK_USE_PLATFORM_XLIB_XRANDR_EXT", "1" )
#  else
#   pragma detect_mismatch( "VK_USE_PLATFORM_XLIB_XRANDR_EXT", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
#endif // AE_ENABLE_VULKAN
