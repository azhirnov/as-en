// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VulkanLoader.h"
# include "graphics/Vulkan/VulkanCheckError.h"

namespace AE::Graphics
{

#  if 1
#   define VK_LOG( _msg_ )  static struct VkLogCallOnce { VkLogCallOnce() { AE_LOG_DBG( _msg_ ); } } log
#  else
#   define VK_LOG( _msg_ )  AE_LOG_DBG( _msg_ )
#  endif

#   define VKLOADER_STAGE_FNPOINTER
#    include "vulkan_loader/fn_vulkan_lib.h"
#    include "vulkan_loader/fn_vulkan_inst.h"
#   undef  VKLOADER_STAGE_FNPOINTER

#   define VKLOADER_STAGE_DUMMYFN
#    include "vulkan_loader/fn_vulkan_lib.h"
#    include "vulkan_loader/fn_vulkan_inst.h"
#    include "vulkan_loader/fn_vulkan_dev.h"
#   undef  VKLOADER_STAGE_DUMMYFN

    PFN_vkGetInstanceProcAddr  _var_vkGetInstanceProcAddr = null;

    VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL Dummy_vkGetInstanceProcAddr (VkInstance , const char * ) {  VK_LOG( "used dummy function 'vkGetInstanceProcAddr'" );  return null;  }

/*
=================================================
    VulkanLib
=================================================
*/
namespace {
    struct VulkanLib
    {
        Library                     module;
        VkInstance                  instance            = Default;
        PFN_vkGetInstanceProcAddr   getInstanceProcAddr = null;
        int                         refCounter          = 0;


        ND_ static VulkanLib&  Instance () __NE___
        {
            static VulkanLib    lib;
            return lib;
        }
    };
}
/*
=================================================
    Initialize
----
    must be externally synchronized!
=================================================
*/
    bool VulkanLoader::Initialize (NtStringView libName) __NE___
    {
        VulkanLib&  lib = VulkanLib::Instance();

        if ( lib.module and lib.refCounter > 0 )
        {
            ++lib.refCounter;
            return true;
        }

        if ( not libName.empty() )
            Unused( lib.module.Load( libName ));

    #ifdef AE_PLATFORM_WINDOWS
        if ( not lib.module )
            Unused( lib.module.Load( NtStringView{ "vulkan-1.dll" }));

        if ( not lib.module )
            Unused( lib.module.Load( FileSystem::GetWindowsPath() / "System32" / "vulkan-1.dll" ));

        // software emulation
        if ( not lib.module )
            Unused( lib.module.Load( NtStringView{ "vkswiftshader.dll" }));
    #else
        if ( not lib.module )
            Unused( lib.module.Load( NtStringView{ "libvulkan.so" }));

        if ( not lib.module )
            Unused( lib.module.Load( NtStringView{ "libvulkan.so.1" }));

        // software emulation
        if ( not lib.module )
            Unused( lib.module.Load( NtStringView{ "libvkswiftshader.so" }));
    #endif

        if ( not lib.module  )
            return false;

        // write library path to log
        AE_LOG_DBG( "Vulkan library path: \""s << ToString(lib.module.GetPath()) << '"' );

        _var_vkGetInstanceProcAddr = &Dummy_vkGetInstanceProcAddr;

        // all global functions can be loaded using 'vkGetInstanceProcAddr', so we need to import only this function address.
        if ( not lib.module.GetProcAddr( "vkGetInstanceProcAddr", OUT lib.getInstanceProcAddr ))
            return false;

        _var_vkGetInstanceProcAddr = lib.getInstanceProcAddr;

        ++lib.refCounter;

        // it is allowed to use null instance handle in 'vkGetInstanceProcAddr'.
        const auto  Load =  [&lib] (OUT auto& outResult, const char *procName, auto dummy)
                            {{
                                using FN = decltype(dummy);
                                FN  result = BitCast<FN>( lib.getInstanceProcAddr( null, procName ));
                                outResult = result ? result : dummy;
                            }};

        #define VKLOADER_STAGE_GETADDRESS
        #include "vulkan_loader/fn_vulkan_lib.h"
        #undef  VKLOADER_STAGE_GETADDRESS

        CHECK_ERR( _var_vkCreateInstance != &Dummy_vkCreateInstance );
        return true;
    }

/*
=================================================
    LoadInstance
----
    must be externally synchronized!
    warning: multiple instances are not supported!
=================================================
*/
    bool VulkanLoader::LoadInstance (VkInstance instance) __NE___
    {
        VulkanLib&  lib = VulkanLib::Instance();

        ASSERT( instance != Default );
        ASSERT( lib.instance == Default or lib.instance == instance );

        if ( lib.getInstanceProcAddr == null )
            return false;

        if ( lib.instance == instance )
            return true;    // functions already loaded for this instance

        lib.instance = instance;

        const auto  Load =  [&lib] (OUT auto& outResult, const char *procName, auto dummy)
                            {{
                                using FN = decltype(dummy);
                                FN  result = BitCast<FN>( vkGetInstanceProcAddr( lib.instance, procName ));
                                outResult = result ? result : dummy;
                            }};

        #define VKLOADER_STAGE_GETADDRESS
        #include "vulkan_loader/fn_vulkan_inst.h"
        #undef  VKLOADER_STAGE_GETADDRESS

        return true;
    }

/*
=================================================
    LoadDevice
----
    access to the 'vkGetDeviceProcAddr' must be externally synchronized!
=================================================
*/
    bool VulkanLoader::LoadDevice (VkDevice device, OUT VulkanDeviceFnTable &table) __NE___
    {
        CHECK_ERR( _var_vkGetDeviceProcAddr != &Dummy_vkGetDeviceProcAddr );

        const auto  Load =  [device] (OUT auto& outResult, const char *procName, auto dummy)
                            {{
                                using FN = decltype(dummy);
                                FN  result = BitCast<FN>( vkGetDeviceProcAddr( device, procName ));
                                outResult = result ? result : dummy;
                            }};

        #define VKLOADER_STAGE_GETADDRESS
        #include "vulkan_loader/fn_vulkan_dev.h"
        #undef  VKLOADER_STAGE_GETADDRESS

        return true;
    }

/*
=================================================
    ResetDevice
=================================================
*/
    void VulkanLoader::ResetDevice (OUT VulkanDeviceFnTable &table) __NE___
    {
        const auto  Load =  [] (OUT auto& outResult, const char *, auto dummy) {
                                outResult = dummy;
                            };

        #define VKLOADER_STAGE_GETADDRESS
        #include "vulkan_loader/fn_vulkan_dev.h"
        #undef  VKLOADER_STAGE_GETADDRESS
    }

/*
=================================================
    Unload
----
    must be externally synchronized!
=================================================
*/
    void VulkanLoader::Unload () __NE___
    {
        VulkanLib&  lib = VulkanLib::Instance();

        ASSERT( lib.refCounter > 0 );

        if ( (--lib.refCounter) != 0 )
            return;

        lib.module.Unload();
        lib.instance            = null;
        lib.getInstanceProcAddr = null;

        const auto  Load =  [] (OUT auto& outResult, const char *, auto dummy)
                            {{
                                outResult = dummy;
                            }};

        #define VKLOADER_STAGE_GETADDRESS
        #include "vulkan_loader/fn_vulkan_lib.h"
        #include "vulkan_loader/fn_vulkan_inst.h"
        #undef  VKLOADER_STAGE_GETADDRESS
    }

/*
=================================================
    VulkanDeviceFn_Init
=================================================
*/
    void VulkanDeviceFn::VulkanDeviceFn_Init (const VulkanDeviceFn &other) __NE___
    {
        _table = other._table;
    }

    void VulkanDeviceFn::VulkanDeviceFn_Init (const VulkanDeviceFnTable *table) __NE___
    {
        _table = table;
    }

/*
=================================================
    SetupInstanceBackwardCompatibility
=================================================
*/
    void VulkanLoader::SetupInstanceBackwardCompatibility (Version2 version) __NE___
    {
    #define VK_COMPAT( _dst_, _src_ )   \
        ASSERT( _var_##_src_ != null ); \
        _var_##_dst_ = _var_##_src_

        if ( version >= Version2{1,1} )
        {
          // VK_KHR_get_physical_device_properties2
            VK_COMPAT( vkGetPhysicalDeviceFeatures2KHR,                     vkGetPhysicalDeviceFeatures2 );
            VK_COMPAT( vkGetPhysicalDeviceProperties2KHR,                   vkGetPhysicalDeviceProperties2 );
            VK_COMPAT( vkGetPhysicalDeviceFormatProperties2KHR,             vkGetPhysicalDeviceFormatProperties2 );
            VK_COMPAT( vkGetPhysicalDeviceImageFormatProperties2KHR,        vkGetPhysicalDeviceImageFormatProperties2 );
            VK_COMPAT( vkGetPhysicalDeviceQueueFamilyProperties2KHR,        vkGetPhysicalDeviceQueueFamilyProperties2 );
            VK_COMPAT( vkGetPhysicalDeviceMemoryProperties2KHR,             vkGetPhysicalDeviceMemoryProperties2 );
            VK_COMPAT( vkGetPhysicalDeviceSparseImageFormatProperties2KHR,  vkGetPhysicalDeviceSparseImageFormatProperties2 );

          // VK_KHR_device_group_creation
            //VK_COMPAT( vkEnumeratePhysicalDeviceGroupsKHR,                vkEnumeratePhysicalDeviceGroups );
        }

        if ( version >= Version2{1,2} )
        {
        }

        if ( version >= Version2{1,3} )
        {
          // VK_EXT_tooling_info
            //VK_COMPAT( vkGetPhysicalDeviceToolPropertiesEXT,              vkGetPhysicalDeviceToolProperties   );
        }

    #undef VK_COMPAT
    }

/*
=================================================
    SetupDeviceBackwardCompatibility
=================================================
*/
    void VulkanLoader::SetupDeviceBackwardCompatibility (Version2 version, INOUT VulkanDeviceFnTable &table) __NE___
    {
    #define VK_COMPAT( _dst_, _src_ )           \
        ASSERT( table._var_##_src_ != null );   \
        table._var_##_dst_ = table._var_##_src_

        if ( version >= Version2{1,1} )
        {
          // VK_KHR_maintenance1
            VK_COMPAT( vkTrimCommandPoolKHR,                    vkTrimCommandPool                   );

          // VK_KHR_maintenance3
            VK_COMPAT( vkGetDescriptorSetLayoutSupportKHR,      vkGetDescriptorSetLayoutSupport     );

          // VK_KHR_bind_memory2
            VK_COMPAT( vkBindBufferMemory2KHR,                  vkBindBufferMemory2                 );
            VK_COMPAT( vkBindImageMemory2KHR,                   vkBindImageMemory2                  );

          // VK_KHR_get_memory_requirements2
            VK_COMPAT( vkGetImageMemoryRequirements2KHR,        vkGetImageMemoryRequirements2       );
            VK_COMPAT( vkGetBufferMemoryRequirements2KHR,       vkGetBufferMemoryRequirements2      );
            VK_COMPAT( vkGetImageSparseMemoryRequirements2KHR,  vkGetImageSparseMemoryRequirements2 );
        /*
          // VK_KHR_sampler_ycbcr_conversion
            VK_COMPAT( vkCreateSamplerYcbcrConversionKHR,       vkCreateSamplerYcbcrConversion      );
            VK_COMPAT( vkDestroySamplerYcbcrConversionKHR,      vkDestroySamplerYcbcrConversion     );

          // VK_KHR_descriptor_update_template
            VK_COMPAT( vkCreateDescriptorUpdateTemplateKHR,     vkCreateDescriptorUpdateTemplate    );
            VK_COMPAT( vkDestroyDescriptorUpdateTemplateKHR,    vkDestroyDescriptorUpdateTemplate   );
            VK_COMPAT( vkUpdateDescriptorSetWithTemplateKHR,    vkUpdateDescriptorSetWithTemplate   );

          // VK_KHR_device_group
            VK_COMPAT( vkGetDeviceGroupPeerMemoryFeaturesKHR,   vkGetDeviceGroupPeerMemoryFeatures  );
            VK_COMPAT( vkCmdSetDeviceMaskKHR,                   vkCmdSetDeviceMask                  );
            VK_COMPAT( vkCmdDispatchBaseKHR,                    vkCmdDispatchBase                   );
            */
        }
        
        if ( version >= Version2{1,2} )
        {
          // VK_KHR_draw_indirect_count
            VK_COMPAT( vkCmdDrawIndirectCountKHR,               vkCmdDrawIndirectCount              );
            VK_COMPAT( vkCmdDrawIndexedIndirectCountKHR,        vkCmdDrawIndexedIndirectCount       );

          // VK_KHR_create_renderpass2
            VK_COMPAT( vkCreateRenderPass2KHR,                  vkCreateRenderPass2                 );
            VK_COMPAT( vkCmdBeginRenderPass2KHR,                vkCmdBeginRenderPass2               );
            VK_COMPAT( vkCmdNextSubpass2KHR,                    vkCmdNextSubpass2                   );
            VK_COMPAT( vkCmdEndRenderPass2KHR,                  vkCmdEndRenderPass2                 );

          // VK_KHR_timeline_semaphore
            VK_COMPAT( vkGetSemaphoreCounterValueKHR,           vkGetSemaphoreCounterValue          );
            VK_COMPAT( vkWaitSemaphoresKHR,                     vkWaitSemaphores                    );
            VK_COMPAT( vkSignalSemaphoreKHR,                    vkSignalSemaphore                   );

          // VK_EXT_host_query_reset
            VK_COMPAT( vkResetQueryPoolEXT,                     vkResetQueryPool                    );
        }
        /*
        if ( version >= Version2{1,3} )
        {
          // VK_KHR_buffer_device_address
            VK_COMPAT( vkGetBufferDeviceAddressKHR,             vkGetBufferDeviceAddress            );
            VK_COMPAT( vkGetBufferOpaqueCaptureAddressKHR,      vkGetBufferOpaqueCaptureAddress     );
            VK_COMPAT( vkGetDeviceMemoryOpaqueCaptureAddressKHR,vkGetDeviceMemoryOpaqueCaptureAddress);

          // VK_KHR_copy_commands2
            VK_COMPAT( vkCmdBlitImage2KHR,                      vkCmdBlitImage2                     );
            VK_COMPAT( vkCmdCopyBuffer2KHR,                     vkCmdCopyBuffer2                    );
            VK_COMPAT( vkCmdCopyBufferToImage2KHR,              vkCmdCopyBufferToImage2             );
            VK_COMPAT( vkCmdCopyImage2KHR,                      vkCmdCopyImage2                     );
            VK_COMPAT( vkCmdCopyImageToBuffer2KHR,              vkCmdCopyImageToBuffer2             );
            VK_COMPAT( vkCmdResolveImage2KHR,                   vkCmdResolveImage2                  );

          // VK_KHR_synchronization2
            VK_COMPAT( vkCmdPipelineBarrier2KHR,                vkCmdPipelineBarrier2               );
            VK_COMPAT( vkCmdResetEvent2KHR,                     vkCmdResetEvent2                    );
            VK_COMPAT( vkCmdSetEvent2KHR,                       vkCmdSetEvent2                      );
            VK_COMPAT( vkCmdWaitEvents2KHR,                     vkCmdWaitEvents2                    );
            VK_COMPAT( vkCmdWriteTimestamp2KHR,                 vkCmdWriteTimestamp2                );
            VK_COMPAT( vkQueueSubmit2KHR,                       vkQueueSubmit2                      );

          // VK_KHR_maintenance4
            VK_COMPAT( vkGetDeviceBufferMemoryRequirementsKHR,      vkGetDeviceBufferMemoryRequirements      );
            VK_COMPAT( vkGetDeviceImageMemoryRequirementsKHR,       vkGetDeviceImageMemoryRequirements       );
            VK_COMPAT( vkGetDeviceImageSparseMemoryRequirementsKHR, vkGetDeviceImageSparseMemoryRequirements );

          // VK_EXT_extended_dynamic_state, VK_EXT_extended_dynamic_state2
            // not supported

          // VK_EXT_private_data
            // not supported
        }
        */
    #undef VK_COMPAT
    }

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
