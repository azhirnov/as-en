// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Private/EnumToString.h"
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
namespace
{
    StaticAssert( sizeof(DispatchIndirectCommand)       == sizeof(VkDispatchIndirectCommand) );
    StaticAssert( sizeof(DrawIndirectCommand)           == sizeof(VkDrawIndirectCommand) );
    StaticAssert( sizeof(DrawIndexedIndirectCommand)    == sizeof(VkDrawIndexedIndirectCommand) );
    StaticAssert( sizeof(DrawMeshTasksIndirectCommand)  == sizeof(VkDrawMeshTasksIndirectCommandEXT) );
    StaticAssert( sizeof(TraceRayIndirectCommand)       == sizeof(VkTraceRaysIndirectCommandKHR) );
    StaticAssert( sizeof(TraceRayIndirectCommand2)      == sizeof(VkTraceRaysIndirectCommand2KHR) );
    StaticAssert( sizeof(ASBuildIndirectCommand)        == sizeof(VkAccelerationStructureBuildRangeInfoKHR) );
    StaticAssert( sizeof(DeviceAddress)             == sizeof(VkDeviceAddress) );

    StaticAssert( offsetof(DrawIndirectCommand, vertexCount)    == offsetof(VkDrawIndirectCommand, vertexCount) );
    StaticAssert( offsetof(DrawIndirectCommand, instanceCount)  == offsetof(VkDrawIndirectCommand, instanceCount) );
    StaticAssert( offsetof(DrawIndirectCommand, firstVertex)    == offsetof(VkDrawIndirectCommand, firstVertex) );
    StaticAssert( offsetof(DrawIndirectCommand, firstInstance)  == offsetof(VkDrawIndirectCommand, firstInstance) );

    StaticAssert( offsetof(DrawIndexedIndirectCommand, indexCount)      == offsetof(VkDrawIndexedIndirectCommand, indexCount) );
    StaticAssert( offsetof(DrawIndexedIndirectCommand, instanceCount)   == offsetof(VkDrawIndexedIndirectCommand, instanceCount) );
    StaticAssert( offsetof(DrawIndexedIndirectCommand, firstIndex)      == offsetof(VkDrawIndexedIndirectCommand, firstIndex) );
    StaticAssert( offsetof(DrawIndexedIndirectCommand, vertexOffset)    == offsetof(VkDrawIndexedIndirectCommand, vertexOffset) );
    StaticAssert( offsetof(DrawIndexedIndirectCommand, firstInstance)   == offsetof(VkDrawIndexedIndirectCommand, firstInstance) );

    StaticAssert( FrameUID::MaxFramesLimit() == GraphicsConfig::MaxFrames );

    StaticAssert( VK_HEADER_VERSION == 275 );

    static constexpr usize  c_MaxMemTypes = std::initializer_list<EMemoryType>{
                                                EMemoryType::DeviceLocal,   EMemoryType::Transient,     EMemoryType::HostCoherent,
                                                EMemoryType::HostCached,    EMemoryType::Dedicated,     EMemoryType::HostCachedCoherent,
                                                EMemoryType::Unified,       EMemoryType::UnifiedCached }.size();
    StaticAssert( decltype(DeviceResourceFlags::memTypes)::capacity() >= c_MaxMemTypes );


#   include "vulkan_loader/vkenum_to_str.h"

/*
=================================================
    VK_DBGUTILS_DBGREPORT_OBJECT_TYPES
=================================================
*/
#define VK_DBGUTILS_DBGREPORT_OBJECT_TYPES( _visitor_ )\
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT,                     VK_OBJECT_TYPE_UNKNOWN                      ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT,                    VK_OBJECT_TYPE_INSTANCE                     ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT,             VK_OBJECT_TYPE_PHYSICAL_DEVICE              ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT,                      VK_OBJECT_TYPE_DEVICE                       ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT,                       VK_OBJECT_TYPE_QUEUE                        ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT,                   VK_OBJECT_TYPE_SEMAPHORE                    ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT,              VK_OBJECT_TYPE_COMMAND_BUFFER               ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT,                       VK_OBJECT_TYPE_FENCE                        ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT,               VK_OBJECT_TYPE_DEVICE_MEMORY                ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT,                      VK_OBJECT_TYPE_BUFFER                       ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT,                       VK_OBJECT_TYPE_IMAGE                        ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT,                       VK_OBJECT_TYPE_EVENT                        ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT,                  VK_OBJECT_TYPE_QUERY_POOL                   ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT,                 VK_OBJECT_TYPE_BUFFER_VIEW                  ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT,                  VK_OBJECT_TYPE_IMAGE_VIEW                   ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT,               VK_OBJECT_TYPE_SHADER_MODULE                ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT,              VK_OBJECT_TYPE_PIPELINE_CACHE               ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT,             VK_OBJECT_TYPE_PIPELINE_LAYOUT              ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT,                 VK_OBJECT_TYPE_RENDER_PASS                  ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT,                    VK_OBJECT_TYPE_PIPELINE                     ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT,       VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT        ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT,                     VK_OBJECT_TYPE_SAMPLER                      ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT,             VK_OBJECT_TYPE_DESCRIPTOR_POOL              ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT,              VK_OBJECT_TYPE_DESCRIPTOR_SET               ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT,                 VK_OBJECT_TYPE_FRAMEBUFFER                  ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT,                VK_OBJECT_TYPE_COMMAND_POOL                 ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT,                 VK_OBJECT_TYPE_SURFACE_KHR                  ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT,               VK_OBJECT_TYPE_SWAPCHAIN_KHR                ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT_EXT,   VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT    ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT,                 VK_OBJECT_TYPE_DISPLAY_KHR                  ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT,            VK_OBJECT_TYPE_DISPLAY_MODE_KHR             ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT_EXT,        VK_OBJECT_TYPE_VALIDATION_CACHE_EXT         ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_EXT,    VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION     ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_EXT,  VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE   ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR_EXT,  VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR   ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV_EXT,   VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV    ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_CU_MODULE_NVX_EXT,               VK_OBJECT_TYPE_CU_MODULE_NVX                ) \
    _visitor_( VK_DEBUG_REPORT_OBJECT_TYPE_CU_FUNCTION_NVX_EXT,             VK_OBJECT_TYPE_CU_FUNCTION_NVX              )

/*
=================================================
    DebugReportObjectTypeToString
=================================================
*/
    ND_ static VkObjectType  DebugReportObjectTypeToObjectType (VkDebugReportObjectTypeEXT objType)
    {
        switch_enum( objType )
        {
            #define REPORT_TO_UTILS( _dbgReportType_, _dbgUtilsType_ )  case _dbgReportType_ :  return _dbgUtilsType_;
            VK_DBGUTILS_DBGREPORT_OBJECT_TYPES( REPORT_TO_UTILS );
            #undef REPORT_TO_UTILS

            case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA_EXT :
            case VK_DEBUG_REPORT_OBJECT_TYPE_CUDA_MODULE_NV_EXT :
            case VK_DEBUG_REPORT_OBJECT_TYPE_CUDA_FUNCTION_NV_EXT :
            case VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT : break;
        }
        switch_end
        return VK_OBJECT_TYPE_MAX_ENUM;
    }

/*
=================================================
    DebugReportObjectTypeToString
=================================================
*/
    ND_ static VkDebugReportObjectTypeEXT  DebugReportObjectTypeToObjectType (VkObjectType objType)
    {
        switch_enum( objType )
        {
            #define UTILS_TO_REPORT( _dbgReportType_, _dbgUtilsType_ )  case _dbgUtilsType_ :  return _dbgReportType_;
            VK_DBGUTILS_DBGREPORT_OBJECT_TYPES( UTILS_TO_REPORT );
            #undef UTILS_TO_REPORT

            case VK_OBJECT_TYPE_VIDEO_SESSION_KHR :
            case VK_OBJECT_TYPE_VIDEO_SESSION_PARAMETERS_KHR :
            case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT :
            case VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR :
            case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV :
            case VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT :
            case VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL :
            case VK_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA :
            case VK_OBJECT_TYPE_MICROMAP_EXT :
            case VK_OBJECT_TYPE_OPTICAL_FLOW_SESSION_NV :
            case VK_OBJECT_TYPE_SHADER_EXT :
            case VK_OBJECT_TYPE_CUDA_MODULE_NV :
            case VK_OBJECT_TYPE_CUDA_FUNCTION_NV :
            case VK_OBJECT_TYPE_MAX_ENUM :  break;
        }
        switch_end
        return VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT;
    }

/*
=================================================
    MsgSeverityToReportFlags
=================================================
*/
    ND_ static VkDebugReportFlagsEXT MsgSeverityToReportFlags (VkDebugUtilsMessageSeverityFlagsEXT inFlags)
    {
        VkDebugReportFlagsEXT   result = 0;

        for (auto t : BitfieldIterate( inFlags ))
        {
            switch_enum( t )
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT :          result |= VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;  break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT :             result |= VK_DEBUG_REPORT_INFORMATION_BIT_EXT;  break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT :          result |= VK_DEBUG_REPORT_WARNING_BIT_EXT;      break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT :            result |= VK_DEBUG_REPORT_ERROR_BIT_EXT;        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT :
                default_unlikely :                                              DBG_WARNING( "unknown message severity type" ); break;
            }
            switch_end
        }
        return result;
    }

/*
=================================================
    VkObjectTypeToString
=================================================
*/
    ND_ static StringView  VkObjectTypeToString (VkObjectType objType)
    {
        switch_enum( objType )
        {
            case VK_OBJECT_TYPE_INSTANCE :                      return "Instance";
            case VK_OBJECT_TYPE_PHYSICAL_DEVICE :               return "PhysicalDevice";
            case VK_OBJECT_TYPE_DEVICE :                        return "Device";
            case VK_OBJECT_TYPE_QUEUE :                         return "Queue";
            case VK_OBJECT_TYPE_SEMAPHORE :                     return "Semaphore";
            case VK_OBJECT_TYPE_COMMAND_BUFFER :                return "CommandBuffer";
            case VK_OBJECT_TYPE_FENCE :                         return "Fence";
            case VK_OBJECT_TYPE_DEVICE_MEMORY :                 return "DeviceMemory";
            case VK_OBJECT_TYPE_BUFFER :                        return "Buffer";
            case VK_OBJECT_TYPE_IMAGE :                         return "Image";
            case VK_OBJECT_TYPE_EVENT :                         return "Event";
            case VK_OBJECT_TYPE_QUERY_POOL :                    return "QueryPool";
            case VK_OBJECT_TYPE_BUFFER_VIEW :                   return "BufferView";
            case VK_OBJECT_TYPE_IMAGE_VIEW :                    return "ImageView";
            case VK_OBJECT_TYPE_SHADER_MODULE :                 return "ShaderModule";
            case VK_OBJECT_TYPE_PIPELINE_CACHE :                return "PipelineCache";
            case VK_OBJECT_TYPE_PIPELINE_LAYOUT :               return "PipelineLayout";
            case VK_OBJECT_TYPE_RENDER_PASS :                   return "RenderPass";
            case VK_OBJECT_TYPE_PIPELINE :                      return "Pipeline";
            case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT :         return "DescriptorSetLayout";
            case VK_OBJECT_TYPE_SAMPLER :                       return "Sampler";
            case VK_OBJECT_TYPE_DESCRIPTOR_POOL :               return "DescriptorPool";
            case VK_OBJECT_TYPE_DESCRIPTOR_SET :                return "DescriptorSet";
            case VK_OBJECT_TYPE_FRAMEBUFFER :                   return "Framebuffer";
            case VK_OBJECT_TYPE_COMMAND_POOL :                  return "CommandPool";
            case VK_OBJECT_TYPE_SURFACE_KHR :                   return "Surface";
            case VK_OBJECT_TYPE_SWAPCHAIN_KHR :                 return "Swapchain";
            case VK_OBJECT_TYPE_DISPLAY_KHR :                   return "Display";
            case VK_OBJECT_TYPE_DISPLAY_MODE_KHR :              return "DisplayMode";
            case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV :   return "IndirectCommandsLayoutNv";
            case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR :    return "AccelerationStructure";
            case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT :     return "DebugReportCallback";
            case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT :     return "DebugUtilsMessenger";
            case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION :      return "SamplerYcBr";
            case VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL:return "PerformanceConfigIntel";
            case VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT :         return "PrivateDataSlot";
            case VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR :        return "DeferredOperation";
            case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_KHR: return "DescriptorUpdateTemplate";
            case VK_OBJECT_TYPE_VIDEO_SESSION_KHR :             return "VideoSession";
            case VK_OBJECT_TYPE_VIDEO_SESSION_PARAMETERS_KHR :  return "VideoSessionParams";
            case VK_OBJECT_TYPE_CU_MODULE_NVX :                 return "CUmoduleNVX";
            case VK_OBJECT_TYPE_CU_FUNCTION_NVX :               return "CUfunctionNVX";
            case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV :     return "AccelerationStructureNV";
            case VK_OBJECT_TYPE_MICROMAP_EXT :                  return "Mcromap";
            case VK_OBJECT_TYPE_OPTICAL_FLOW_SESSION_NV :       return "OpticalFlowSessionNV";
            case VK_OBJECT_TYPE_SHADER_EXT :                    return "Shader";
            case VK_OBJECT_TYPE_CUDA_MODULE_NV :                return "CUDA module";
            case VK_OBJECT_TYPE_CUDA_FUNCTION_NV :              return "CUDA fn";

            case VK_OBJECT_TYPE_UNKNOWN :
            case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT :
            case VK_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA :
            case VK_OBJECT_TYPE_MAX_ENUM :
                break;
        }
        switch_end
        return "unknown";
    }
}
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    VDevice::VDevice () __NE___ :
        _extEmulation{ &_deviceFnTable }
    {
        VulkanDeviceFn_Init( &_deviceFnTable );
    }

/*
=================================================
    destructor
=================================================
*/
    VDevice::~VDevice () __NE___
    {
        CHECK( _vkInstance == Default );
        CHECK( _vkLogicalDevice == Default );
    }

/*
=================================================
    HasInstanceExtension
=================================================
*/
    bool  VDevice::HasInstanceExtension (StringView name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        return _instanceExtensions.contains( name );
    }

/*
=================================================
    HasDeviceExtension
=================================================
*/
    bool  VDevice::HasDeviceExtension (StringView name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        return _deviceExtensions.contains( name );
    }

/*
=================================================
    SetObjectName
=================================================
*/
    bool  VDevice::SetObjectName (ulong id, NtStringView name, VkObjectType type) C_NE___
    {
        if ( name.empty() or id == 0 )
            return false;

        DRC_SHAREDLOCK( _drCheck );

        if ( _extensions.debugUtils )
        {
            VkDebugUtilsObjectNameInfoEXT   info = {};
            info.sType          = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            info.objectType     = type;
            info.objectHandle   = id;
            info.pObjectName    = name.c_str();

            VK_CHECK( vkSetDebugUtilsObjectNameEXT( _vkLogicalDevice, &info ));
            return true;
        }

        if ( _extensions.debugMarker )
        {
            VkDebugMarkerObjectNameInfoEXT  info = {};
            info.sType          = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
            info.objectType     = DebugReportObjectTypeToObjectType(type);
            info.object         = id;
            info.pObjectName    = name.c_str();

            VK_CHECK( vkDebugMarkerSetObjectNameEXT( _vkLogicalDevice, &info ));
            return true;
        }

        return false;
    }

/*
=================================================
    GetQueueFamilies
=================================================
*/
    void  VDevice::GetQueueFamilies (EQueueMask mask, OUT VQueueFamilyIndices_t &result) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        result.clear();

        for (uint i = 0; (1u << i) <= uint(mask); ++i)
        {
            if ( not AllBits( mask, 1u << i ))
                continue;

            auto    q = GetQueue( EQueueType(i) );
            if ( q == null )
                continue;

            result.push_back( uint(q->familyIndex) );
        }
    }

/*
=================================================
    GetMemoryTypeIndex
=================================================
*/
    bool  VDevice::GetMemoryTypeIndex (const uint memoryTypeBits,
                                      VkMemoryPropertyFlagBits includeFlags, VkMemoryPropertyFlagBits optIncludeFlags,
                                      VkMemoryPropertyFlagBits excludeFlags, VkMemoryPropertyFlagBits optExcludeFlags,
                                      OUT uint &memoryTypeIndex) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        memoryTypeIndex = UMax;

        auto&   mem_props       = GetVProperties().memoryProperties;
        uint    with_inc_opt    = UMax;
        uint    with_exc_opt    = UMax;
        uint    without_opt     = UMax;

        for (uint i = 0; i < mem_props.memoryTypeCount; ++i)
        {
            const auto&     mem_type = mem_props.memoryTypes[i];
            const auto      flags    = mem_type.propertyFlags;

            if ( AllBits( memoryTypeBits, 1u << i ) and AllBits( flags, includeFlags ))
            {
                if ( not AnyBits( flags, excludeFlags ))
                {
                    const bool  has_inc_opt = AllBits( flags, optIncludeFlags );
                    const bool  not_exc_opt = not AnyBits( flags, optExcludeFlags );

                    if ( has_inc_opt and not_exc_opt )
                    {
                        memoryTypeIndex = i;
                        return true;
                    }

                    if ( has_inc_opt )
                        with_inc_opt = i;

                    if ( not_exc_opt )
                        with_exc_opt = i;

                    if ( not has_inc_opt and not not_exc_opt )
                        without_opt = i;
                }
            }
        }

        if ( memoryTypeIndex != UMax )
            return true;

        if ( with_inc_opt != UMax )
        {
            memoryTypeIndex = with_inc_opt;
            return true;
        }

        if ( with_exc_opt != UMax )
        {
            memoryTypeIndex = with_exc_opt;
            return true;
        }

        if ( without_opt != UMax )
        {
            memoryTypeIndex = without_opt;
            return true;
        }

        return false;
    }

/*
=================================================
    GetMemoryTypeIndex
=================================================
*/
    bool  VDevice::GetMemoryTypeIndex (uint memoryTypeBits, EMemoryType memType, OUT uint &memoryTypeIndex) C_NE___
    {
        VkMemoryPropertyFlagBits    include_flags       = Zero;
        VkMemoryPropertyFlagBits    exclude_flags       = Zero;
        VkMemoryPropertyFlagBits    opt_include_flags   = Zero;
        VkMemoryPropertyFlagBits    opt_exclude_flags   = Zero;

        constexpr EMemoryType       mask = EMemoryType::DeviceLocal | EMemoryType::Transient | EMemoryType::HostCachedCoherent;

        switch_enum( memType & mask )
        {
            case EMemoryType::DeviceLocal :
                include_flags       = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                exclude_flags       = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                opt_exclude_flags   = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                break;

            case EMemoryType::Transient :
                include_flags       = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
                exclude_flags       = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                opt_exclude_flags   = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
                break;

            case EMemoryType::HostCoherent :
                include_flags       = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                exclude_flags       = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
                opt_exclude_flags   = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                break;

            case EMemoryType::HostCached :
                include_flags       = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                exclude_flags       = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
                opt_exclude_flags   = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                break;

            case EMemoryType::HostCachedCoherent :
                include_flags       = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                exclude_flags       = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
                break;

            case EMemoryType::Unified :
                include_flags       = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                exclude_flags       = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                break;

            case EMemoryType::UnifiedCached :
                include_flags       = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
                exclude_flags       = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
                break;

            case EMemoryType::_External :
            case EMemoryType::_Last :
            case EMemoryType::All :
            case EMemoryType::Unknown :
            case EMemoryType::Dedicated :
            default_unlikely :
                RETURN_ERR( "unsupported memory type" );
        }
        switch_end

        return GetMemoryTypeIndex( memoryTypeBits, include_flags, opt_include_flags, exclude_flags, opt_exclude_flags, OUT memoryTypeIndex );
    }

/*
=================================================
    AllocateMemory
=================================================
*/
    VkResult  VDevice::AllocateMemory (const VkMemoryAllocateInfo &allocateInfo, OUT VkDeviceMemory &memory) C_NE___
    {
        auto    err = vkAllocateMemory( GetVkDevice(), &allocateInfo, null, OUT &memory );

        #ifdef AE_DEBUG
        if_unlikely( err == VK_ERROR_OUT_OF_HOST_MEMORY or err == VK_ERROR_OUT_OF_DEVICE_MEMORY )
        {
            DRC_SHAREDLOCK( _drCheck );
            if ( IsInitialized() and GetVExtensions().memoryBudget )
            {
                VkPhysicalDeviceMemoryBudgetPropertiesEXT   budget = {};
                VkPhysicalDeviceMemoryProperties2           props  = {};
                budget.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
                props.sType     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
                props.pNext     = &budget;

                vkGetPhysicalDeviceMemoryProperties2KHR( _vkPhysicalDevice, OUT &props );

                const auto&     type    = props.memoryProperties.memoryTypes[ allocateInfo.memoryTypeIndex ];
                const auto&     heap    = props.memoryProperties.memoryHeaps[ type.heapIndex ];

                AE_LOGI( "Out of memory error, can't allocate memType("s << ToString(allocateInfo.memoryTypeIndex) << ") size(" <<
                         ToString(Bytes{allocateInfo.allocationSize}) << ") heap(" << ToString(type.heapIndex) << ") heapSize(" <<
                         ToString(Bytes{heap.size}) << ") heapUsage(" << ToString(Bytes{budget.heapUsage[ type.heapIndex ]}) <<
                         ") heapAvailable(" << ToString(Bytes{budget.heapBudget[ type.heapIndex ]}) << ")" );
            }
        }
        #endif

        return err;
    }

/*
=================================================
    GetMemoryTypeBits
=================================================
*/
    uint  VDevice::GetMemoryTypeBits (const EMemoryType memType) C_NE___
    {
        constexpr EMemoryType   mask    = EMemoryType::DeviceLocal | EMemoryType::Transient | EMemoryType::HostCachedCoherent;
        auto                    it      = _memTypeToBits.find( memType & mask );

        return (it != _memTypeToBits.end() ? it->second : 0u);
    }

/*
=================================================
    CheckConstantLimits
=================================================
*/
    bool  VDevice::CheckConstantLimits () C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        return _devProps.CompareWithConstant( AnyTypeCRef{_extensions} );
    }

/*
=================================================
    CheckExtensions
----
    Engine assumes that device supports required extensions
=================================================
*/
    bool  VDevice::CheckExtensions () C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        const auto  CheckExt = [] (bool supported, const char* name)
        {{
            Unused( name );
            if ( not supported )
                AE_LOG_DBG( "Required extension '"s << name << "' is not supported" );
            return supported;
        }};

        bool    result  = true;
        result &= CheckExt( _extensions.renderPass2,        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME );    // native or emulated
        result &= CheckExt( _extensions.loadOpNone,         VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME );     // native or emulated
        result &= CheckExt( _extensions.synchronization2,   VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME );      // native or emulated
        result &= CheckExt( _extensions.swapchain,          VK_KHR_SWAPCHAIN_EXTENSION_NAME );

      #if AE_VK_TIMELINE_SEMAPHORE
        result &= CheckExt( _extensions.timelineSemaphore,  VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME );
        result &= CheckExt( _extensions.hostQueryReset,     VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME );
      #endif

        if ( _extensions.rayTracingPipeline )
            result &= CheckExt( _extensions.rayTracingMaintenance1, VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME );

        return result;
    }

/*
=================================================
    GetMemoryUsage
=================================================
*/
    VDevice::DevMemoryInfoOpt  VDevice::GetMemoryUsage () C_NE___
    {
        DevMemoryInfoOpt    result;

        DRC_SHAREDLOCK( _drCheck );
        if ( not IsInitialized() or not GetVExtensions().memoryBudget )
            return result;

        result = DeviceMemoryInfo{};

        VkPhysicalDeviceMemoryBudgetPropertiesEXT   budget = {};
        VkPhysicalDeviceMemoryProperties2           props  = {};
        budget.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
        props.sType     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        props.pNext     = &budget;

        vkGetPhysicalDeviceMemoryProperties2KHR( _vkPhysicalDevice, OUT &props );

        Bytes*  usage [] = { &result->deviceUsage, &result->hostUsage, &result->unifiedUsage };
        Bytes*  avail [] = { &result->deviceAvailable, &result->hostAvailable, &result->unifiedAvailable };
        Bytes*  total [] = { &result->deviceTotal, &result->hostTotal, &result->unifiedTotal };

        for (uint i = 0; i < props.memoryProperties.memoryHeapCount; ++i)
        {
            *usage[ _memHeapToType[i] ] += budget.heapUsage[i];
            *avail[ _memHeapToType[i] ] += budget.heapBudget[i];
            *total[ _memHeapToType[i] ] += props.memoryProperties.memoryHeaps[i].size;
        }

        // driver bug: memory_budget extension enabled but not active
        ASSERT( (result->deviceAvailable + result->hostAvailable + result->unifiedAvailable) > 0 );

        return result;
    }

/*
=================================================
    IsUnderDebugger
=================================================
*/
    bool  VDevice::IsUnderDebugger () C_NE___
    {
        if ( _extensions.toolingInfo )
        {
            uint    count = 0;
            VK_CHECK_ERR( vkGetPhysicalDeviceToolPropertiesEXT( _vkPhysicalDevice, OUT &count, null ));

            return count > 0;
        }
        return false;
    }

/*
=================================================
    PrintPipelineExecutableStatistics
=================================================
*/
    bool  VDevice::PrintPipelineExecutableStatistics (StringView pplnName, const VkPipeline pipeline) C_NE___
    {
    #ifdef AE_ENABLE_LOGS
        if ( not GetVExtensions().pplnExecProps )
            return false;

        TRY{
            VkPipelineInfoKHR   ppln_info;
            ppln_info.sType     = VK_STRUCTURE_TYPE_PIPELINE_INFO_KHR;
            ppln_info.pNext     = null;
            ppln_info.pipeline  = pipeline;

            uint    count = 0;
            VK_CHECK_ERR( vkGetPipelineExecutablePropertiesKHR( GetVkDevice(), &ppln_info, OUT &count, null ));

            Array<VkPipelineExecutablePropertiesKHR>    exec_props;
            exec_props.resize( count );
            for (auto& prop : exec_props) prop.sType = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_PROPERTIES_KHR;

            VK_CHECK_ERR( vkGetPipelineExecutablePropertiesKHR( GetVkDevice(), &ppln_info, INOUT &count, OUT exec_props.data() ));

            VkPipelineExecutableInfoKHR exec_info;
            exec_info.sType     = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_INFO_KHR;
            exec_info.pNext     = null;
            exec_info.pipeline  = pipeline;

            Array<VkPipelineExecutableStatisticKHR> statistics;

            const StringView    s_stages        {"stages"};
            const StringView    s_name          {"name"};
            const StringView    s_subgroupSize  {"subgroupSize"};

            String  str;
            for (const auto [prop, idx] : WithIndex(exec_props))
            {
                exec_info.executableIndex = uint(idx);
                VK_CHECK_ERR( vkGetPipelineExecutableStatisticsKHR( GetVkDevice(), &exec_info, OUT &count, null ));

                if ( count == 0 ) continue;

                statistics.resize( count );
                for (auto& stat : statistics) stat.sType = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR;
                VK_CHECK_ERR( vkGetPipelineExecutableStatisticsKHR( GetVkDevice(), &exec_info, OUT &count, OUT statistics.data() ));

                if ( exec_props.size() > 1 )
                    str << "\n[" << ToString(idx) << "]";

                uint    j       = 0;
                usize   max_len = Max( s_stages.size(), s_name.size(), s_subgroupSize.size() );

                for (auto& stat : statistics)
                    AssignMax( INOUT max_len, StringView{stat.name}.size() );

                str << "\n  " << s_stages << ": ";
                AppendToString( INOUT str, s_stages.size(), max_len, !!(j++ & 1), '.', ' ' );
                str << ' ' << ToString( AEEnumCast( VkShaderStageFlagBits( prop.stages )));

                str << "\n  " << s_name << ": ";
                AppendToString( INOUT str, s_name.size(), max_len, !!(j++ & 1), '.', ' ' );
                str << ' ' << prop.name << " (" << prop.description << ')';

                str << "\n  " << s_subgroupSize << ": ";
                AppendToString( INOUT str, s_subgroupSize.size(), max_len, !!(j++ & 1), '.', ' ' );
                str << ' ' << ToString( prop.subgroupSize );

                for (const auto [stat, i] : WithIndex(statistics))
                {
                    StringView  name {stat.name};

                    str << "\n  " << name << ": ";
                    AppendToString( INOUT str, name.size(), max_len, !!(j++ & 1), '.', ' ' );
                    str << ' ';

                    switch_enum( stat.format )
                    {
                        case VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_BOOL32_KHR :   str << (stat.value.b32 ? "true" : "false");     break;
                        case VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_INT64_KHR :    str << ToString( stat.value.i64 );              break;
                        case VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_UINT64_KHR :   str << ToString( stat.value.u64 );              break;
                        case VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_FLOAT64_KHR :  str << ToString( stat.value.f64 );              break;
                        case VK_PIPELINE_EXECUTABLE_STATISTIC_FORMAT_MAX_ENUM_KHR : break;
                    }
                    switch_end
                    str << "  (" << stat.description << ')';
                }
            }

            if ( str.empty() )
                return true;

            ("\nPipeline '"s << pplnName << "' ExecutableStatistics {") >> str;
            str << "\n}\n";

            AE_LOGI( str );
            return true;
        }
        CATCH_ALL(
            return false;
        )
    #endif
    }

/*
=================================================
    PrintPipelineExecutableInternalRepresentations
=================================================
*/
    bool  VDevice::PrintPipelineExecutableInternalRepresentations (StringView pplnName, const VkPipeline pipeline) C_NE___
    {
    #ifdef AE_ENABLE_LOGS
        if ( not GetVExtensions().pplnExecProps )
            return false;

        TRY{
            VkPipelineInfoKHR   ppln_info;
            ppln_info.sType     = VK_STRUCTURE_TYPE_PIPELINE_INFO_KHR;
            ppln_info.pNext     = null;
            ppln_info.pipeline  = pipeline;

            uint    count = 0;
            VK_CHECK_ERR( vkGetPipelineExecutablePropertiesKHR( GetVkDevice(), &ppln_info, OUT &count, null ));

            Array<VkPipelineExecutablePropertiesKHR>    exec_props;
            exec_props.resize( count );
            for (auto& prop : exec_props) prop.sType = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_PROPERTIES_KHR;

            VK_CHECK_ERR( vkGetPipelineExecutablePropertiesKHR( GetVkDevice(), &ppln_info, INOUT &count, OUT exec_props.data() ));

            VkPipelineExecutableInfoKHR exec_info;
            exec_info.sType     = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_INFO_KHR;
            exec_info.pNext     = null;
            exec_info.pipeline  = pipeline;

            Array<VkPipelineExecutableInternalRepresentationKHR>    exec_irep;

            String  str;
            for (const auto [prop, idx] : WithIndex(exec_props))
            {
                exec_info.executableIndex = uint(idx);
                VK_CHECK_ERR( vkGetPipelineExecutableInternalRepresentationsKHR( GetVkDevice(), &exec_info, OUT &count, null ));

                if ( count == 0 ) continue;

                exec_irep.resize( count );
                for (auto& stat : exec_irep) stat.sType = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_INTERNAL_REPRESENTATION_KHR;
                VK_CHECK_ERR( vkGetPipelineExecutableInternalRepresentationsKHR( GetVkDevice(), &exec_info, OUT &count, OUT exec_irep.data() ));

                str << "\n[" << ToString(idx) << "]"
                    << "\n  stages:       " << ToString( AEEnumCast( VkShaderStageFlagBits( prop.stages )))
                    << "\n  name:         " << prop.name << " (" << prop.description << ')'
                    << "\n  subgroupSize: " << ToString( prop.subgroupSize );

                for (const auto [rep, i] : WithIndex(exec_irep))
                {
                    str << "\n  [" << ToString(i) << "]"
                        << "\n    name:        " << rep.name
                        << "\n    description: " << rep.description;

                    if ( rep.isText )
                        str << "\n    text:        " << StringView{ Cast<char>(rep.pData), rep.dataSize };
                }
            }

            if ( str.empty() )
                return true;

            ("\nPipeline '"s << pplnName << "' ExecutableInternalRepresentations {") >> str;
            str << "\n}\n";

            AE_LOGI( str );
            return true;
        }
        CATCH_ALL(
            return false;
        )
    #endif
    }

/*
=================================================
    PrintPipelineExecutableInfo
=================================================
*/
    bool  VDevice::PrintPipelineExecutableInfo (StringView name, VkPipeline pipeline, EPipelineOpt options) C_NE___
    {
        bool    res = GetVExtensions().pplnExecProps;

        if ( AllBits( options, EPipelineOpt::CaptureStatistics ))
            res &= PrintPipelineExecutableStatistics( name, pipeline );

        if ( AllBits( options, EPipelineOpt::CaptureInternalRepresentation ))
            res &= PrintPipelineExecutableInternalRepresentations( name, pipeline );

        return res;
    }

/*
=================================================
    GetMemoryRequirements (buffer)
=================================================
*/
    bool  VDevice::GetMemoryRequirements (const VkBufferCreateInfo &ci, OUT VkMemoryRequirements2* memReq) C_NE___
    {
        CHECK_ERR( memReq != null );

        if ( _extensions.maintenance4 )
        {
            VkDeviceBufferMemoryRequirements    info = {};
            info.sType          = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS;
            info.pCreateInfo    = &ci;

            vkGetDeviceBufferMemoryRequirementsKHR( _vkLogicalDevice, &info, OUT memReq );
            return true;
        }

        VkBuffer    buf = Default;
        VK_CHECK_ERR( vkCreateBuffer( _vkLogicalDevice, &ci, null, OUT &buf ));

        if ( _extensions.bindMemory2 )
        {
            VkBufferMemoryRequirementsInfo2 info = {};
            info.sType  = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
            info.buffer = buf;
            vkGetBufferMemoryRequirements2KHR( _vkLogicalDevice, &info, OUT memReq );
        }else
            vkGetBufferMemoryRequirements( _vkLogicalDevice, buf, OUT &memReq->memoryRequirements );

        vkDestroyBuffer( _vkLogicalDevice, buf, null );
        return true;
    }

/*
=================================================
    GetMemoryRequirements (image)
=================================================
*/
    bool  VDevice::GetMemoryRequirements (const VkImageCreateInfo &ci, OUT VkMemoryRequirements2* memReq) C_NE___
    {
        CHECK_ERR( memReq != null );

        if ( _extensions.maintenance4 )
        {
            VkDeviceImageMemoryRequirements info = {};
            info.sType          = VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS;
            info.pCreateInfo    = &ci;

            // This parameter is ignored unless pCreateInfo::tiling is VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
            // or pCreateInfo::flags has VK_IMAGE_CREATE_DISJOINT_BIT set.
            //info.planeAspect

            vkGetDeviceImageMemoryRequirementsKHR( _vkLogicalDevice, &info, OUT memReq );
            return true;
        }

        VkImage img = Default;
        VK_CHECK_ERR( vkCreateImage( _vkLogicalDevice, &ci, null, OUT &img ));

        if ( _extensions.bindMemory2 )
        {
            VkImageMemoryRequirementsInfo2  info = {};
            info.sType  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
            info.image  = img;
            vkGetImageMemoryRequirements2KHR( _vkLogicalDevice, &info, OUT memReq );
        }else
            vkGetImageMemoryRequirements( _vkLogicalDevice, img, OUT &memReq->memoryRequirements );

        vkDestroyImage( _vkLogicalDevice, img, null );
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    VDeviceInitializer::VDeviceInitializer (Bool enableInfoLog) __NE___ :
        _enableInfoLog{ enableInfoLog }
    {
        auto    dbg_report = _dbgReport.WriteNoLock();
        EXLOCK( dbg_report );

        NOTHROW( dbg_report->tempObjectDbgInfos.reserve( 16 ));
        dbg_report->tempString.reserve( 1024 );
    }

/*
=================================================
    destructor
=================================================
*/
    VDeviceInitializer::~VDeviceInitializer () __NE___
    {
        DRC_EXLOCK( _drCheck );

        DestroyDebugCallback();
    }

/*
=================================================
    GetMaxInstanceVersion
=================================================
*/
    VDeviceInitializer::InstanceVersion  VDeviceInitializer::GetMaxInstanceVersion () C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        uint    ver = VK_MAKE_VERSION( 1, 0, 0 );
        Unused( vkEnumerateInstanceVersion( OUT &ver ));

        return InstanceVersion{ VK_VERSION_MAJOR(ver), VK_VERSION_MINOR(ver) };
    }

/*
=================================================
    LoadNvPerf
=================================================
*/
    bool  VDeviceInitializer::LoadNvPerf () __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( not _nvPerf.IsLoaded() );
        CHECK_ERR( GetVkInstance() == Default );

        return _nvPerf.Load();
    }

/*
=================================================
    LoadArmProfiler
=================================================
*/
    bool  VDeviceInitializer::LoadArmProfiler () __NE___
    {
        DRC_EXLOCK( _drCheck );

        // TODO
        return false;
    }

/*
=================================================
    LoadArmProfiler
=================================================
*/
    bool  VDeviceInitializer::LoadRenderDoc () __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( not _rdc.IsInitialized() );
        CHECK_ERR( GetVkInstance() != Default );

        return _rdc.Initialize( GetVkInstance(), NativeWindow{} );
    }

/*
=================================================
    CreateInstance
=================================================
*/
    bool  VDeviceInitializer::CreateInstance (const InstanceCreateInfo &ci) __NE___
    {
        DRC_EXLOCK( _drCheck );

        VkValidationFeaturesEXT     validation      = {};
        validation.sType                            = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        validation.enabledValidationFeatureCount    = uint(ci.enableValidations.size());
        validation.pEnabledValidationFeatures       = ci.enableValidations.data();
        validation.disabledValidationFeatureCount   = uint(ci.disableValidations.size());
        validation.pDisabledValidationFeatures      = ci.disableValidations.data();

        NOTHROW_ERR(
            return _CreateInstance( ci.appName, ci.engineName, ci.instanceLayers, ci.instanceExtensions,
                                    ci.version, ci.appVer, ci.engineVer, &validation );
        )
    }

    bool  VDeviceInitializer::_CreateInstance (NtStringView appName, NtStringView engineName, ArrayView<const char*> layers,
                                               ArrayView<const char*> extensions, InstanceVersion version,
                                               uint appVer, uint engineVer, const VkValidationFeaturesEXT* pValidation) __Th___
    {
        CHECK_ERR( _vkInstance == Default );
        CHECK_ERR( VulkanLoader::Initialize() );

        uint    vk_ver = VK_MAKE_VERSION( version.major, version.minor, 0 );
        _ValidateInstanceVersion( Default, layers, extensions, INOUT vk_ver );
        _vkInstanceVersion = {VK_VERSION_MAJOR(vk_ver), VK_VERSION_MINOR(vk_ver)};

        _extEmulation.OnInitialize();

        Array< const char* >    instance_layers;
        instance_layers.assign( layers.begin(), layers.end() );     // throw

        Array< const char* >    instance_extensions = _GetInstanceExtensions( _vkInstanceVersion );         // throw
        instance_extensions.insert( instance_extensions.end(), extensions.begin(), extensions.end() );      // throw

        if ( _nvPerf.IsLoaded() )
            _nvPerf.GetInstanceExtensions( *this, INOUT instance_extensions );

        _ValidateInstanceLayers( INOUT instance_layers, Bool{not _enableInfoLog} );                             // throw
        _ValidateInstanceExtensions( instance_layers, INOUT instance_extensions, Bool{not _enableInfoLog} );    // throw


        VkApplicationInfo   app_info = {};
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.apiVersion         = vk_ver;
        app_info.pApplicationName   = appName.c_str();
        app_info.applicationVersion = appVer;
        app_info.pEngineName        = engineName.c_str();
        app_info.engineVersion      = engineVer;

        VkInstanceCreateInfo    instance_ci = {};
        instance_ci.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_ci.pApplicationInfo        = &app_info;
        instance_ci.enabledExtensionCount   = uint(instance_extensions.size());
        instance_ci.ppEnabledExtensionNames = instance_extensions.size() ? instance_extensions.data() : null;
        instance_ci.enabledLayerCount       = uint(instance_layers.size());
        instance_ci.ppEnabledLayerNames     = instance_layers.size() ? instance_layers.data() : null;

        if ( ArrayContains( instance_extensions.begin(), instance_extensions.end(), StringView{VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME} ))
            instance_ci.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        if ( pValidation != null                                                                            and
             (pValidation->enabledValidationFeatureCount | pValidation->disabledValidationFeatureCount) != 0 )
        {
            if ( ArrayContains( instance_extensions.begin(), instance_extensions.end(), StringView{VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME} ))
                instance_ci.pNext = pValidation;
        }

        VK_CHECK_ERR( vkCreateInstance( &instance_ci, null, OUT &_vkInstance ));

        CHECK_ERR( VulkanLoader::LoadInstance( _vkInstance ));

        _OnCreateInstance( instance_extensions, instance_layers );  // throw
        return true;
    }

/*
=================================================
    SetInstance
=================================================
*
    bool  VDeviceInitializer::SetInstance (VkInstance newInstance, InstanceVersion version, ArrayView<const char*> instanceExtensions, ArrayView<const char*> instanceLayers)
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _vkInstance == Default );
        CHECK_ERR( newInstance );
        CHECK_ERR( VulkanLoader::Initialize() );
        _extEmulation.OnInitialize();

        _vkInstance = newInstance;
        CHECK_ERR( VulkanLoader::LoadInstance( _vkInstance ));

        NOTHROW_ERR(
        for (auto inst : instanceExtensions) {
            _instanceExtensions.insert( inst ); // throw
        })

        uint    vk_ver = VK_MAKE_VERSION( version.major, version.minor, 0 );
        _ValidateInstanceVersion( _vkInstance, instanceLayers, instanceExtensions, INOUT vk_ver );
        _vkInstanceVersion = {VK_VERSION_MAJOR(vk_ver), VK_VERSION_MINOR(vk_ver)};

        _OnCreateInstance( instanceExtensions, instanceLayers );    // throw
        return true;
    }

/*
=================================================
    _OnCreateInstance
=================================================
*/
    void  VDeviceInitializer::_OnCreateInstance (ArrayView<const char*> instanceExtensions, ArrayView<const char*> instanceLayers) __Th___
    {
        VulkanLoader::SetupInstanceBackwardCompatibility( _vkInstanceVersion.Cast<0>() );
        _extEmulation.OnLoadInstance( _vkInstance, _vkInstanceVersion.Cast<0>() );

        for (auto inst : instanceExtensions) {
            _instanceExtensions.insert( inst ); // throw
        }

        if ( _enableInfoLog )
        {
            _LogInstance( instanceLayers ); // throw
            _LogPhysicalDevices();
        }
        _CheckInstanceExtensions();
    }

/*
=================================================
    DestroyInstance
=================================================
*/
    bool  VDeviceInitializer::DestroyInstance () __NE___
    {
        DRC_EXLOCK( _drCheck );

        if ( _vkInstance == Default )
            return false;

        CHECK_ERR( _vkLogicalDevice == Default );

        DestroyDebugCallback();

        _extEmulation.OnUnload();

        if ( not _isCopy )
            vkDestroyInstance( _vkInstance, null );

        VulkanLoader::Unload();

        if ( _nvPerf.IsLoaded() )
            _nvPerf.Deinitialize();

        _vkInstance         = Default;
        _vkPhysicalDevice   = Default;
        _vkInstanceVersion  = Default;
        _vkDeviceVersion    = Default;
        _spirvVersion       = Default;
        _extensions         = Default;
        _properties         = Default;
        _resFlags           = Default;

        _instanceExtensions.clear();

        if ( _enableInfoLog )
            AE_LOG_DBG( "Destroyed Vulkan instance" );

        return true;
    }

/*
=================================================
    GetVendorNameByID
=================================================
*/
namespace {
    ND_ static StringView  GetVendorNameByID (uint id)
    {
        return ToString( GetVendorTypeByID( id ));
    }
}

/*
=================================================
    ChooseDevice
=================================================
*/
    bool  VDeviceInitializer::ChooseDevice (StringView deviceName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _vkInstance );
        CHECK_ERR( _vkLogicalDevice == Default );

        if ( deviceName.empty() )
            return false;

        FixedArray< VkPhysicalDevice, 16 >  devices;
        {
            uint    count = 0;
            VK_CHECK( vkEnumeratePhysicalDevices( GetVkInstance(), OUT &count, null ));
            CHECK_ERR( count > 0 );

            devices.resize( count );
            count = uint(devices.size());
            VK_CHECK( vkEnumeratePhysicalDevices( GetVkInstance(), OUT &count, OUT devices.data() ));
            devices.resize( Min( count, devices.size() ));
        }

        VkPhysicalDevice    pdev = Default;

        for (auto& dev : devices)
        {
            VkPhysicalDeviceProperties  prop = {};
            vkGetPhysicalDeviceProperties( dev, OUT &prop );

            const bool  match = not deviceName.empty()                          and
                                (HasSubStringIC( prop.deviceName, deviceName )  or
                                 HasSubStringIC( GetVendorNameByID( prop.vendorID ), deviceName ));

            if ( match and pdev == Default )
                pdev = dev;
        }

        if ( pdev == Default )
            return false;

        return SetPhysicalDevice( pdev );
    }

/*
=================================================
    ChooseHighPerformanceDevice
=================================================
*/
    bool  VDeviceInitializer::ChooseHighPerformanceDevice () __NE___
    {
        NOTHROW_ERR(
            return _ChooseHighPerformanceDevice();
        )
    }

    bool  VDeviceInitializer::_ChooseHighPerformanceDevice () __Th___
    {
        const auto  CalcTotalMemory = [] (VkPhysicalDeviceMemoryProperties &memProps) -> Bytes
        {{
            Bytes   total;
            for (uint j = 0; j < memProps.memoryTypeCount; ++j)
            {
                if ( AllBits( memProps.memoryTypes[j].propertyFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ))
                {
                    const uint idx = memProps.memoryTypes[j].heapIndex;

                    if ( AllBits( memProps.memoryHeaps[idx].flags, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ))
                    {
                        total += Bytes( memProps.memoryHeaps[idx].size );
                        memProps.memoryHeaps[idx].size = 0;
                    }
                }
            }
            return total;
        }};


        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _vkInstance );
        CHECK_ERR( _vkLogicalDevice == Default );

        FixedArray< VkPhysicalDevice, 16 >  devices;
        {
            uint    count = 0;
            VK_CHECK( vkEnumeratePhysicalDevices( GetVkInstance(), OUT &count, null ));
            CHECK_ERR( count > 0 );

            devices.resize( count );
            count = uint(devices.size());

            VK_CHECK( vkEnumeratePhysicalDevices( GetVkInstance(), OUT &count, OUT devices.data() ));
            devices.resize( Min( count, devices.size() ));

            if ( count == 1 )
                return SetPhysicalDevice( devices[0] );
        }

        VkPhysicalDevice    any_device          = Default;
        VkPhysicalDevice    high_perf_device    = Default;
        float               max_performance     = 0.0f;

        for (auto& dev : devices)
        {
            VkPhysicalDeviceFeatures            feat     = {};
            VkPhysicalDeviceProperties          props    = {};
            VkPhysicalDeviceMemoryProperties    mem_prop = {};

            vkGetPhysicalDeviceFeatures( dev, OUT &feat );
            vkGetPhysicalDeviceProperties( dev, OUT &props );
            vkGetPhysicalDeviceMemoryProperties( dev, OUT &mem_prop );

            DeviceVersion   dev_ver;
            {
                dev_ver = {VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion)};
                if ( dev_ver.Cast<0>() > _vkInstanceVersion.Cast<0>() )
                    dev_ver = _vkInstanceVersion.Cast< DeviceVersion::GetUID() >();
            }

            Array<const char *>     device_extensions = {   // throw
                VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME,
                VK_EXT_MESH_SHADER_EXTENSION_NAME,
                VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME,
                VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
            };
            _ValidateDeviceExtensions( dev, INOUT device_extensions );


            const auto  HasExtension            = [&device_extensions] (StringView ext) { return ArrayContains( ArrayView<const char *>{device_extensions}, ext ); };

            const bool  is_discrete             = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            const bool  is_integrated           = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;

            const bool  shader_sm_builtins_nv   = (dev_ver >= DeviceVersion{1,1} and HasExtension( VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME ));
            const bool  mesh_shader             = (HasExtension( VK_EXT_MESH_SHADER_EXTENSION_NAME ));
            const bool  shader_core_props_amd   = (HasExtension( VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME ));
            const bool  accel_struct            = (dev_ver >= DeviceVersion{1,1} and HasExtension( VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME ));
            const bool  ray_tracing_ppln        = (dev_ver >= DeviceVersion{1,1} and HasExtension( VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME ));
            const bool  shader_core_arm         = HasExtension( VK_ARM_SHADER_CORE_BUILTINS_EXTENSION_NAME );

            const auto  dev_local_mem           = CalcTotalMemory( mem_prop );


            VkPhysicalDeviceShaderCorePropertiesAMD         shaderCorePropsAMDProps     = {};
            VkPhysicalDeviceShaderSMBuiltinsPropertiesNV    shaderSMBuiltinsNVProps     = {};
            VkPhysicalDeviceShaderCoreBuiltinsPropertiesARM shaderCoreBuiltinsARMProps  = {};

            if ( GetInstanceVersion() >= InstanceVersion{1,1} or HasInstanceExtension( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ))
            {
                VkPhysicalDeviceProperties2     props2      = {};
                void **                         next_props  = &props2.pNext;

                props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

                if ( shader_sm_builtins_nv )
                {
                    *next_props = &shaderSMBuiltinsNVProps;
                    next_props  = &shaderSMBuiltinsNVProps.pNext;
                    shaderSMBuiltinsNVProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV;
                }
                if ( shader_core_props_amd )
                {
                    *next_props = &shaderCorePropsAMDProps;
                    next_props  = &shaderCorePropsAMDProps.pNext;
                    shaderCorePropsAMDProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD;
                }
                if ( shader_core_arm )
                {
                    *next_props = &shaderCoreBuiltinsARMProps;
                    next_props  = &shaderCoreBuiltinsARMProps.pNext;
                    shaderCoreBuiltinsARMProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_PROPERTIES_ARM;
                }

                *next_props = null;
                vkGetPhysicalDeviceProperties2KHR( dev, OUT &props2 );
            }


            float   perf = 0.f;                                             // magic function:
            perf += float(ulong(dev_local_mem)) / float(1u << 30);                  // memory in Gb
            perf += float(is_discrete) * 4.f + float(is_integrated) * 1.f;          // discrete > integrated > any other
            perf += float(feat.tessellationShader + feat.geometryShader +
                          accel_struct + ray_tracing_ppln + mesh_shader) * 0.2f;    // new features in newer GPU

            // AMD
            if ( shader_core_props_amd )
                perf += float(shaderCorePropsAMDProps.shaderEngineCount          * shaderCorePropsAMDProps.shaderArraysPerEngineCount *
                              shaderCorePropsAMDProps.computeUnitsPerShaderArray * shaderCorePropsAMDProps.simdPerComputeUnit         *
                              shaderCorePropsAMDProps.wavefrontsPerSimd) / 200.f;   // num warps

            // NV
            if ( shader_sm_builtins_nv )
                perf += float(shaderSMBuiltinsNVProps.shaderSMCount * shaderSMBuiltinsNVProps.shaderWarpsPerSM) / 100.f;    // num warps

            // ARM
            if ( shader_core_arm )
                perf += float(shaderCoreBuiltinsARMProps.shaderCoreCount * shaderCoreBuiltinsARMProps.shaderWarpsPerCore);  // num warps

            if ( perf > max_performance ) {
                max_performance     = perf;
                high_perf_device    = dev;
            }

            if ( any_device == Default )
                any_device = dev;
        }

        if ( high_perf_device == Default )
            high_perf_device = any_device;

        if ( any_device == Default )
            return false;

        return SetPhysicalDevice( high_perf_device );
    }

/*
=================================================
    SetPhysicalDevice
=================================================
*/
    bool  VDeviceInitializer::SetPhysicalDevice (VkPhysicalDevice newPhysicalDevice) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _vkInstance != Default );
        CHECK_ERR( _vkLogicalDevice == Default );
        CHECK_ERR( newPhysicalDevice != Default );

        _vkPhysicalDevice = newPhysicalDevice;
        _UpdateDeviceVersion( _vkPhysicalDevice, OUT _vkDeviceVersion );

        return true;
    }

/*
=================================================
    _UpdateDeviceVersion
=================================================
*/
    void  VDeviceInitializer::_UpdateDeviceVersion (VkPhysicalDevice physicalDevice, OUT DeviceVersion &devVersion) C_NE___
    {
        ASSERT( physicalDevice != Default );

        VkPhysicalDeviceProperties  props;
        vkGetPhysicalDeviceProperties( physicalDevice, OUT &props );

        devVersion = {VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion)};

        if ( devVersion.Cast<0>() > _vkInstanceVersion.Cast<0>() )
            devVersion = _vkInstanceVersion.Cast< DeviceVersion::GetUID() >();
    }

/*
=================================================
    _SetResourceFlags
=================================================
*/
    void  VDeviceInitializer::_SetResourceFlags (OUT ResourceFlags &outResFlags) C_NE___
    {
        outResFlags = {};

        const auto&     props = GetVProperties();

        // memory type
        {
            ASSERT( not _memTypeToBits.empty() );
            for (auto t : _memTypeToBits.GetKeyArray())
                outResFlags.memTypes.insert( t );
        }

        // descriptor types
        {
            outResFlags.descrTypes.insert( EDescriptorType::UniformBuffer );
            outResFlags.descrTypes.insert( EDescriptorType::StorageBuffer );
            outResFlags.descrTypes.insert( EDescriptorType::UniformTexelBuffer );
            outResFlags.descrTypes.insert( EDescriptorType::StorageTexelBuffer );
            outResFlags.descrTypes.insert( EDescriptorType::StorageImage );
            outResFlags.descrTypes.insert( EDescriptorType::SampledImage );
            outResFlags.descrTypes.insert( EDescriptorType::CombinedImage );
            outResFlags.descrTypes.insert( EDescriptorType::CombinedImage_ImmutableSampler );
            outResFlags.descrTypes.insert( EDescriptorType::SubpassInput );
            outResFlags.descrTypes.insert( EDescriptorType::Sampler );
            outResFlags.descrTypes.insert( EDescriptorType::ImmutableSampler );

            if ( props.accelerationStructureFeats.accelerationStructure )
                outResFlags.descrTypes.insert( EDescriptorType::RayTracingScene );

            StaticAssert( uint(EDescriptorType::_Count) == 12 );
        }

        // image & buffer flags
        {
            outResFlags.bufferUsage =   EBufferUsage::TransferSrc | EBufferUsage::TransferDst | EBufferUsage::Uniform |
                                        EBufferUsage::UniformTexel | EBufferUsage::StorageTexel | EBufferUsage::Storage |
                                        EBufferUsage::Index | EBufferUsage::Vertex | EBufferUsage::Indirect;

            outResFlags.imageUsage =    EImageUsage::TransferSrc | EImageUsage::TransferDst | EImageUsage::Sampled | EImageUsage::Storage |
                                        EImageUsage::ColorAttachment | EImageUsage::DepthStencilAttachment | EImageUsage::InputAttachment;
            outResFlags.imageOptions =  EImageOpt::ColorAttachmentBlend | EImageOpt::SampledLinear | EImageOpt::CubeCompatible |
                                        EImageOpt::MutableFormat | EImageOpt::BlitSrc | EImageOpt::BlitDst;

            if ( props.accelerationStructureFeats.accelerationStructure )
                outResFlags.bufferUsage |= EBufferUsage::ASBuild_Scratch | EBufferUsage::ASBuild_ReadOnly;

            if ( props.rayTracingPipelineFeats.rayTracingPipeline )
                outResFlags.bufferUsage |= EBufferUsage::ShaderBindingTable;

            if ( props.bufferDeviceAddressFeats.bufferDeviceAddress )
                outResFlags.bufferUsage |= EBufferUsage::ShaderAddress;

            if ( props.features.vertexPipelineStoresAndAtomics )
            {
                outResFlags.bufferOptions |= EBufferOpt::VertexPplnStore;
                outResFlags.imageOptions  |= EImageOpt::VertexPplnStore;
            }
            if ( props.features.fragmentStoresAndAtomics )
            {
                outResFlags.bufferOptions |= EBufferOpt::FragmentPplnStore;
                outResFlags.imageOptions  |= EImageOpt::FragmentPplnStore;
            }
            if ( props.features.sparseBinding )
            {
                outResFlags.bufferOptions |= EBufferOpt::SparseAliased;
                outResFlags.imageOptions  |= EImageOpt::SparseAliased;
            }
            if ( props.features.sparseResidencyBuffer )
                outResFlags.bufferOptions |= EBufferOpt::SparseResidency;   // combination of aliased|residency may be unsupported

            if ( props.features.sparseResidencyAliased and props.features.sparseResidencyBuffer )
                outResFlags.bufferOptions |= EBufferOpt::SparseResidencyAliased;

            if ( props.features.sparseResidencyAliased and props.features.sparseResidencyImage2D )
                outResFlags.imageOptions  |= EImageOpt::SparseResidencyAliased;

            if ( props.fragShadingRateFeats.attachmentFragmentShadingRate )
                outResFlags.imageUsage |= EImageUsage::ShadingRate;

            // If filterMinmaxImageComponentMapping is VK_FALSE only the R component of the sampled image value is defined and the other component values are undefined.
            if ( _extensions.samplerFilterMinmax and not props.samplerFilterMinmaxProps.filterMinmaxImageComponentMapping )
                outResFlags.imageOptions |= EImageOpt::SampledMinMax;
        }
    }

/*
=================================================
    _InitMemoryTypeToTypeBits
=================================================
*/
    void  VDeviceInitializer::_InitMemoryTypeToTypeBits (OUT MemTypeToTypeBits_t &result) C_Th___
    {
        const auto& mem_props = GetVProperties().memoryProperties;

        // Is all memory types are device local or has separated device and host memory.
        bool    all_device_local = true;
        for (uint i = 0; i < mem_props.memoryTypeCount; ++i) {
            all_device_local &= AnyBits( mem_props.memoryTypes[i].propertyFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
        }

        for (uint i = 0; i < mem_props.memoryTypeCount; ++i)
        {
            const auto&     mt      = mem_props.memoryTypes[i];
            EMemoryType     dst     = AEEnumCast( VkMemoryPropertyFlagBits(mt.propertyFlags), false );
            const uint      bit     = 1u << i;

            if ( dst == Default )
                continue;

            if ( AllBits( dst, EMemoryType::Transient ))  { result( EMemoryType::Transient ) |= bit; continue; }

            if ( all_device_local )
            {
                // integrated GPU
                if ( AllBits( dst, EMemoryType::Unified ))              result( EMemoryType::Unified )              |= bit;
                if ( AllBits( dst, EMemoryType::UnifiedCached ))        result( EMemoryType::UnifiedCached )        |= bit;
                if ( AllBits( dst, EMemoryType::HostCoherent ))         result( EMemoryType::HostCoherent )         |= bit;
                if ( AllBits( dst, EMemoryType::HostCached ))           result( EMemoryType::HostCached )           |= bit;
                if ( AllBits( dst, EMemoryType::HostCachedCoherent ))   result( EMemoryType::HostCachedCoherent )   |= bit;
                if ( not AllBits( dst, EMemoryType::HostCached ))       result( EMemoryType::DeviceLocal )          |= bit;
            }
            else
            {
                // discrete GPU
                if ( AllBits( dst, EMemoryType::Unified ))
                {
                    result( EMemoryType::Unified ) |= bit;
                    if ( AllBits( dst, EMemoryType::UnifiedCached ))  result( EMemoryType::UnifiedCached ) |= bit;
                    continue;
                }

                if ( AllBits( dst, EMemoryType::DeviceLocal ))          { result( EMemoryType::DeviceLocal ) |= bit; continue; }

                if ( AllBits( dst, EMemoryType::HostCoherent ))         result( EMemoryType::HostCoherent )         |= bit;
                if ( AllBits( dst, EMemoryType::HostCached ))           result( EMemoryType::HostCached )           |= bit;
                if ( AllBits( dst, EMemoryType::HostCachedCoherent ))   result( EMemoryType::HostCachedCoherent )   |= bit;
            }
        }

        // validate
        ASSERT( result.contains( EMemoryType::DeviceLocal ));
        ASSERT( result.contains( EMemoryType::HostCoherent ));
        ASSERT( result.contains( EMemoryType::HostCached ));
    }

/*
=================================================
    _InitMemHeapToMemType
=================================================
*/
    void  VDeviceInitializer::_InitMemHeapToMemType (OUT MemHeapToMemType_t &result) C_Th___
    {
        ASSERT( not _memTypeToBits.empty() );

        const auto& mem_props = GetVProperties().memoryProperties;

        result.fill( VK_MAX_MEMORY_HEAPS-1 );

        auto    dev_mem     = _memTypeToBits.find( EMemoryType::DeviceLocal );
        auto    uni_mem     = _memTypeToBits.find( EMemoryType::Unified );
        uint    heap_bits   = 0;

        const auto  SetHeapBits = [&heap_bits, &mem_props, &result] (uint typeBits, ubyte memType)
        {{
            uint    new_heap_bits = 0;
            for (uint i : BitIndexIterate( typeBits ))
            {
                uint    heap_idx = mem_props.memoryTypes[i].heapIndex;

                new_heap_bits       |= ToBit<uint>( heap_idx );
                result[ heap_idx ]  =  memType;
            }

            //ASSERT( not AnyBits( heap_bits, new_heap_bits )); // must not intersects
            heap_bits |= new_heap_bits;
        }};

        if ( dev_mem != _memTypeToBits.end() and uni_mem != _memTypeToBits.end() )
        {
            const uint  uni_mem_bits = uni_mem->second & ~dev_mem->second;

            if ( uni_mem_bits != 0 )
            {
                // discrete GPU with unified memory
                SetHeapBits( uni_mem_bits, 2 );
                SetHeapBits( dev_mem->second & ~uni_mem->second, 0 );
            }
            else
            {
                // integrated GPU or discrete GPU without unified memory
                SetHeapBits( dev_mem->second, 0 );
            }
        }

        // add host memory
        for (uint i = 0; i < mem_props.memoryHeapCount; ++i)
        {
            if ( HasBit( heap_bits, i )) continue;
            if ( AllBits( mem_props.memoryHeaps[i].flags, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT )) continue;

            heap_bits |= ToBit<uint>( i );
            result[i]  = 1;
        }

        ASSERT( heap_bits == ToBitMask<uint>( mem_props.memoryHeapCount ));
    }

/*
=================================================
    _LogExternalTools
=================================================
*/
    void  VDeviceInitializer::_LogExternalTools () C_Th___
    {
    #ifdef AE_ENABLE_LOGS
        if ( not _extensions.toolingInfo )
            return;

        uint    count = 0;
        VK_CHECK( vkGetPhysicalDeviceToolPropertiesEXT( _vkPhysicalDevice, OUT &count, null ));

        if ( count == 0 )
            return;

        Array<VkPhysicalDeviceToolProperties>   props;
        props.resize( count );

        VK_CHECK( vkGetPhysicalDeviceToolPropertiesEXT( _vkPhysicalDevice, INOUT &count, OUT props.data() ));

        String  str;
        str << "Tools:\n";

        for (const auto& prop : props)
        {
            str << "  name:        " << prop.name << "\n"
                << "  version:     " << prop.version << "\n"
                << "  purposes:    ";

            for (uint bits = prop.purposes; bits != 0;)
            {
                switch_enum( ExtractBit( INOUT bits ))
                {
                    case VK_TOOL_PURPOSE_VALIDATION_BIT :           str << "Validation";            break;
                    case VK_TOOL_PURPOSE_PROFILING_BIT :            str << "Profiling";             break;
                    case VK_TOOL_PURPOSE_TRACING_BIT :              str << "Tracing";               break;
                    case VK_TOOL_PURPOSE_ADDITIONAL_FEATURES_BIT :  str << "AdditionalFeatures";    break;
                    case VK_TOOL_PURPOSE_MODIFYING_FEATURES_BIT :   str << "ModifyingFeatures";     break;
                    case VK_TOOL_PURPOSE_DEBUG_REPORTING_BIT_EXT :  str << "DebugReporting";        break;
                    case VK_TOOL_PURPOSE_DEBUG_MARKERS_BIT_EXT :    str << "DebugMarkers";          break;
                    default :                                       str << "unknown";               break;
                }
                switch_end

                if ( bits != 0 )
                    str << " | ";
            }

            str << "\n"
                << "  description: " << prop.description << "\n"
                << "  layer:       " << prop.layer << "\n"
                << "-----\n";
        }

        AE_LOGI( str );
    #endif
    }

/*
=================================================
    _ValidateSpirvVersion
=================================================
*/
    void  VDeviceInitializer::_ValidateSpirvVersion (OUT SpirvVersion &ver) C_NE___
    {
        switch ( _vkDeviceVersion.To10() )
        {
            case 13 :           ver = SpirvVersion{1,6};                    break;
            case 12 :           ver = SpirvVersion{1,5};                    break;
            case 11 :           ver = _extensions.spirv14 ? SpirvVersion{1,4} : SpirvVersion{1,3};  break;
            case 10 :           ver = SpirvVersion{1,0};                    break;
            default_unlikely :  DBG_WARNING( "unsupported vk version" );    break;
        }
    }

/*
=================================================
    CreateDefaultQueue
=================================================
*/
    bool  VDeviceInitializer::CreateDefaultQueue () __NE___
    {
        QueueCreateInfo queue_ci;
        queue_ci.includeFlags   = VK_QUEUE_GRAPHICS_BIT;

        return CreateQueues( {&queue_ci, 1} );
    }

/*
=================================================
    CreateDefaultQueues
=================================================
*/
    bool  VDeviceInitializer::CreateDefaultQueues (EQueueMask required, EQueueMask optional) __NE___
    {
        CHECK_ERR( _queueCount == 0 );

        uint    count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( _vkPhysicalDevice, OUT &count, null );
        CHECK_ERR( count > 0 );

        QueueFamilyProperties_t  queue_family_props;
        queue_family_props.resize( Min( count, queue_family_props.capacity() ));

        vkGetPhysicalDeviceQueueFamilyProperties( _vkPhysicalDevice, OUT &count, OUT queue_family_props.data() );
        queue_family_props.resize( Min( count, queue_family_props.size() ));

        QueueCount_t    qcount; qcount.resize( queue_family_props.size() );

        const auto  FindHWQueue = [this, &queue_family_props, &qcount] (EQueueType type) -> bool
        {{
            VkQueueFlagBits     include_flags   = Zero;
            VkQueueFlagBits     exclude_flags   = Zero;

            switch_enum( type )
            {
                case EQueueType::Graphics :
                    include_flags   = VK_QUEUE_GRAPHICS_BIT;
                    exclude_flags   = VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
                    break;

                case EQueueType::AsyncCompute :
                    include_flags   = VK_QUEUE_COMPUTE_BIT;
                    exclude_flags   = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
                    break;

                case EQueueType::AsyncTransfer :
                    include_flags   = VK_QUEUE_TRANSFER_BIT;
                    exclude_flags   = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
                    break;

                case EQueueType::VideoEncode :
                    include_flags   = VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
                    break;

                case EQueueType::VideoDecode :
                    include_flags   = VK_QUEUE_VIDEO_DECODE_BIT_KHR;
                    break;

                case EQueueType::_Count :
                case EQueueType::Unknown :
                default_unlikely :
                    RETURN_ERR( "unknown queue type" );
            }
            switch_end

            for (usize i = 0; i < queue_family_props.size(); ++i)
            {
                auto&   q = queue_family_props[i];

                if ( q.queueFlags & exclude_flags )
                    continue;

                if ( qcount[i] < q.queueCount and AllBits( q.queueFlags, include_flags ))
                {
                    auto&   vq = _queues[ _queueCount++ ];
                    vq.familyIndex      = VQueueFamily(i);
                    vq.priority         = 0.0f;
                    vq.globalPriority   = VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT;
                    ++qcount[i];
                    return true;
                }
            }
            return false;
        }};

        for (auto type : BitIndexIterate<EQueueType>( required ))
        {
            CHECK_ERR( FindHWQueue( type ));
        }

        for (auto type : BitIndexIterate<EQueueType>( optional & ~required ))
        {
            FindHWQueue( type );
        }

        _InitQueues( queue_family_props, INOUT _queues, INOUT _queueTypes );

        CHECK_ERR(  _queueCount != 0            and
                    _queueCount <= _queues.size() );
        return true;
    }

/*
=================================================
    CreateQueues
=================================================
*/
    bool  VDeviceInitializer::CreateQueues (ArrayView<QueueCreateInfo> queues) __NE___
    {
        CHECK_ERR( _queueCount == 0 );

        uint    count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( _vkPhysicalDevice, OUT &count, null );
        CHECK_ERR( count > 0 );

        QueueFamilyProperties_t  queue_family_props;
        queue_family_props.resize( Min( count, queue_family_props.capacity() ));

        vkGetPhysicalDeviceQueueFamilyProperties( _vkPhysicalDevice, OUT &count, OUT queue_family_props.data() );
        queue_family_props.resize( Min( count, queue_family_props.size() ));

        QueueCount_t    qcount; qcount.resize( queue_family_props.size() );

        for (auto& q : queues)
        {
            for (usize i = 0; i < queue_family_props.size(); ++i)
            {
                auto&   prop = queue_family_props[i];

                if ( prop.queueFlags & q.excludeFlags )
                    continue;

                if ( qcount[i] < prop.queueCount and AllBits( prop.queueFlags, q.includeFlags ))
                {
                    auto&   vq = _queues[ _queueCount++ ];
                    vq.familyIndex      = VQueueFamily(i);
                    vq.priority         = q.priority;
                    vq.debugName        = q.debugName;
                    vq.globalPriority   = q.globalPriority;
                    ++qcount[i];
                    break;
                }
            }
        }

        _InitQueues( queue_family_props, INOUT _queues, INOUT _queueTypes );

        CHECK_ERR(  _queueCount != 0            and
                    _queueCount <= _queues.size() );
        return true;
    }

/*
=================================================
    CreateLogicalDevice
=================================================
*/
    bool  VDeviceInitializer::CreateLogicalDevice (ArrayView<const char*> extensions, const FeatureSet* fsToDeviceFeatures) __NE___
    {
        NOTHROW_ERR(
            return _CreateLogicalDevice( extensions, fsToDeviceFeatures );
        )
    }

    bool  VDeviceInitializer::_CreateLogicalDevice (ArrayView<const char*> extensions, const FeatureSet* fsToDeviceFeatures) __Th___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _vkPhysicalDevice != Default );
        CHECK_ERR( _vkLogicalDevice == Default );
        CHECK_ERR( _queueCount != 0 );

        // setup device create info
        VkDeviceCreateInfo  device_info = {};
        device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;


        // setup extensions
        Array<const char *>     device_extensions = _GetDeviceExtensions( _vkDeviceVersion );       // throw
        device_extensions.insert( device_extensions.end(), extensions.begin(), extensions.end() );  // throw

        if ( _nvPerf.IsLoaded() )
            _nvPerf.GetDeviceExtensions( *this, INOUT device_extensions );

        _ValidateDeviceExtensions( _vkPhysicalDevice, INOUT device_extensions );    // throw

        NOTHROW_ERR(
        for (auto* ext : device_extensions) {
            _deviceExtensions.insert( ext );
        })
        _CheckDeviceExtensions();   // use '_extensions'

        if ( not device_extensions.empty() )
        {
            device_info.enabledExtensionCount   = uint(device_extensions.size());
            device_info.ppEnabledExtensionNames = device_extensions.data();
        }


        // setup queues
        constexpr uint                                                      max_queues      = uint(EQueueType::_Count);
        StaticArray< VkDeviceQueueCreateInfo, max_queues >                  queue_infos;
        StaticArray< StaticArray<float, max_queues>, max_queues >           priorities      = {};
        StaticArray< VkDeviceQueueGlobalPriorityCreateInfoKHR, max_queues > global_priority = {};
        {
            device_info.queueCreateInfoCount    = 0;
            device_info.pQueueCreateInfos       = queue_infos.data();

            for (usize i = 0; i < queue_infos.size(); ++i)
            {
                auto&   ci = queue_infos[i];

                ci.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                ci.pNext            = null;
                ci.flags            = 0;
                ci.queueFamilyIndex = uint(i);
                ci.queueCount       = 0;
                ci.pQueuePriorities = priorities[i].data();

                if ( _extensions.queueGlobalPriority )
                {
                    auto&   gprior = global_priority[i];
                    gprior.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_GLOBAL_PRIORITY_CREATE_INFO_KHR;
                    gprior.pNext            = null;
                    gprior.globalPriority   = VK_QUEUE_GLOBAL_PRIORITY_MAX_ENUM_KHR;

                    ci.pNext = &gprior;
                }
            }

            for (usize i = 0, cnt = _queueCount; i < cnt; ++i)
            {
                auto&   q = _queues[i];
                CHECK_ERR( uint(q.familyIndex) < queue_infos.size() );

                auto&   qinfo   = queue_infos[ uint(q.familyIndex) ];
                auto&   prior   = priorities[ uint(q.familyIndex) ];
                auto&   gprior  = global_priority[ uint(q.familyIndex) ];

                CHECK_ERR( qinfo.queueCount < priorities[0].size() );

                q.queueIndex = qinfo.queueCount++;
                prior[q.queueIndex] = q.priority;

                if ( _extensions.queueGlobalPriority )
                {
                    if ( gprior.globalPriority == VK_QUEUE_GLOBAL_PRIORITY_MAX_ENUM_KHR )
                        gprior.globalPriority = q.globalPriority;
                    else
                        CHECK_ERR( gprior.globalPriority == q.globalPriority );
                }
            }

            // remove unused queues
            for (usize i = 0; i < queue_infos.size(); ++i)
            {
                auto&   src = queue_infos[i];

                if ( src.queueCount == 0 )
                {
                    if ( i+1 < queue_infos.size() )
                        src = queue_infos[i + 1];
                    continue;
                }

                ASSERT( global_priority[i].globalPriority != VK_QUEUE_GLOBAL_PRIORITY_MAX_ENUM_KHR );

                ++device_info.queueCreateInfoCount;
            }
        }

        // setup features
        {
            void*   dev_info_pnext = null;
            _InitFeaturesAndProperties( INOUT &dev_info_pnext );

            // disable some features
            {
                _properties.features.robustBufferAccess = VK_FALSE; // this feature affects performance

                _properties.bufferDeviceAddressFeats.bufferDeviceAddressCaptureReplay   = VK_FALSE;
                _properties.bufferDeviceAddressFeats.bufferDeviceAddressMultiDevice     = VK_FALSE;

                _properties.accelerationStructureFeats.accelerationStructureCaptureReplay   = VK_FALSE;
                _properties.accelerationStructureFeats.accelerationStructureHostCommands    = VK_FALSE;

                _properties.rayTracingPipelineFeats.rayTracingPipelineShaderGroupHandleCaptureReplay        = VK_FALSE;
                _properties.rayTracingPipelineFeats.rayTracingPipelineShaderGroupHandleCaptureReplayMixed   = VK_FALSE;

                _properties.cooperativeMatrixFeats.cooperativeMatrixRobustBufferAccess = VK_FALSE;
            }

            if ( fsToDeviceFeatures != null )
                CHECK_ERR( _InitFeaturesAndPropertiesByFeatureSet( *fsToDeviceFeatures ));

            device_info.pEnabledFeatures    = &_properties.features;
            device_info.pNext               = dev_info_pnext;
        }

        VK_CHECK_ERR( vkCreateDevice( _vkPhysicalDevice, &device_info, null, OUT &_vkLogicalDevice ));

        CHECK_ERR( VulkanLoader::LoadDevice( _vkLogicalDevice, OUT _deviceFnTable ));
        VulkanLoader::SetupDeviceBackwardCompatibility( _vkDeviceVersion.Cast<0>(), INOUT _deviceFnTable );

        _queueMask = Default;
        for (usize i = 0, cnt = _queueCount; i < cnt; ++i)
        {
            auto&   q = _queues[i];
            _queueMask |= q.type;
            vkGetDeviceQueue( _vkLogicalDevice, uint(q.familyIndex), q.queueIndex, OUT &q.handle );
        }
        ASSERT( _queueCount == BitCount( _queueMask ));

        _ValidateQueueStages( _queues );
        _ValidateSpirvVersion( OUT _spirvVersion );
        _InitMemoryTypeToTypeBits( OUT _memTypeToBits );
        _InitMemHeapToMemType( OUT _memHeapToType );
        _SetResourceFlags( OUT _resFlags );
        _devProps.InitVulkan( _extensions, _properties );

        if ( _enableInfoLog )
        {
            _resFlags.Print();
            _LogLogicalDevice();    // throw
            _devProps.Print();
            _LogExternalTools();    // throw
        }

        if ( not (_nvPerf.IsLoaded() and _nvPerf.Initialize( *this )) )
            _nvPerf.Deinitialize();

        return true;
    }

/*
=================================================
    SetLogicalDevice
=================================================
*
    bool  VDeviceInitializer::SetLogicalDevice (VkDevice newDevice, ArrayView<const char*> extensions) __NE___
    {
        CHECK_ERR( _vkPhysicalDevice );
        CHECK_ERR( _vkLogicalDevice == Default );
        CHECK_ERR( _queueCount != 0 );
        CHECK_ERR( newDevice != Default );

        _vkLogicalDevice = newDevice;

        CHECK_ERR( VulkanLoader::LoadDevice( _vkLogicalDevice, OUT _deviceFnTable ));

        for (auto ext : extensions) {
            _deviceExtensions.insert( ext );    // throw
        }

        // TODO: get queues

        CHECK_ERR( _InitDeviceFeatures() );
        CHECK_ERR( _SetupQueueTypes() );
        return true;
    }

/*
=================================================
    DestroyLogicalDevice
=================================================
*/
    bool  VDeviceInitializer::DestroyLogicalDevice () __NE___
    {
        DRC_EXLOCK( _drCheck );

        if ( _vkLogicalDevice == Default )
            return false;

        if ( not _isCopy )
            vkDestroyDevice( _vkLogicalDevice, null );

        VulkanLoader::ResetDevice( OUT _deviceFnTable );

        _vkLogicalDevice    = Default;
        _queueMask          = Default;
        _queueCount         = 0;

        _queueTypes.fill( Default );
        _deviceExtensions.clear();

        if ( _enableInfoLog )
            AE_LOGI( "Destroyed Vulkan logical device" );

        return true;
    }

/*
=================================================
    _LogLogicalDevice
=================================================
*/
    void  VDeviceInitializer::_LogLogicalDevice () C_Th___
    {
    #ifdef AE_ENABLE_LOGS
        String  str = "\nCreated Vulkan device:";

        str << "\n  requiredApiVersion: . . . . " << ToString( _vkDeviceVersion.major ) << '.' << ToString( _vkDeviceVersion.minor )
            << "\n  SPIRV version:              " << ToString( _spirvVersion.major ) << '.' << ToString( _spirvVersion.minor )
            << "\n  vendorID: . . . . . . . . . " << GetVendorNameByID( _properties.properties.vendorID ) << " (" << ToString<16>( _properties.properties.vendorID ) << ")"
            << "\n  deviceName:                 " << GetDeviceName()
            << "\n  apiVersion: . . . . . . . . " << ToString(VK_VERSION_MAJOR( _properties.properties.apiVersion )) << '.'
                                                  << ToString(VK_VERSION_MINOR( _properties.properties.apiVersion )) << '.'
                                                  << ToString(VK_VERSION_PATCH( _properties.properties.apiVersion ))
            << "\n  driverVersion:              " << ToString(VK_VERSION_MAJOR( _properties.properties.driverVersion )) << '.'
                                                  << ToString(VK_VERSION_MINOR( _properties.properties.driverVersion )) << '.'
                                                  << ToString(VK_VERSION_PATCH( _properties.properties.driverVersion ));

        if ( _extensions.driverProperties )
        {
            const auto& ver = _properties.driverPropertiesProps.conformanceVersion;
            str << "\n  conformanceVersion: . . . . " << ToString( ver.major ) << '.' << ToString( ver.minor ) << '.' << ToString( ver.subminor ) << '.' << ToString( ver.patch );
            str << "\n  driverID:                   ";

            switch_enum( _properties.driverPropertiesProps.driverID )
            {
                case VK_DRIVER_ID_AMD_PROPRIETARY :             str << "AMD proprietary";               break;
                case VK_DRIVER_ID_AMD_OPEN_SOURCE :             str << "AMD open source";               break;
                case VK_DRIVER_ID_MESA_RADV :                   str << "Mesa RADV";                     break;
                case VK_DRIVER_ID_NVIDIA_PROPRIETARY :          str << "NVidia proprietary";            break;
                case VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS :   str << "Intel proprietary windows";     break;
                case VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA :      str << "Intel open source";             break;
                case VK_DRIVER_ID_IMAGINATION_PROPRIETARY :     str << "Imagination proprietary";       break;
                case VK_DRIVER_ID_QUALCOMM_PROPRIETARY :        str << "Qualcomm proprietary";          break;
                case VK_DRIVER_ID_ARM_PROPRIETARY :             str << "ARM proprietary";               break;
                case VK_DRIVER_ID_GOOGLE_SWIFTSHADER :          str << "Google swiftshader";            break;
                case VK_DRIVER_ID_GGP_PROPRIETARY :             str << "GGP proprietary";               break;
                case VK_DRIVER_ID_BROADCOM_PROPRIETARY :        str << "Broadcom proprietary";          break;
                case VK_DRIVER_ID_MESA_LLVMPIPE :               str << "Mesa LLVMPIPE";                 break;
                case VK_DRIVER_ID_MOLTENVK :                    str << "MoltenVk";                      break;
                case VK_DRIVER_ID_COREAVI_PROPRIETARY :         str << "Coreavi proprietary";           break;
                case VK_DRIVER_ID_JUICE_PROPRIETARY :           str << "Juice proprietary";             break;
                case VK_DRIVER_ID_VERISILICON_PROPRIETARY :     str << "Verisilicon proprietary";       break;
                case VK_DRIVER_ID_SAMSUNG_PROPRIETARY :         str << "Samsung proprietary";           break;
                case VK_DRIVER_ID_MESA_TURNIP :                 str << "Mesa turnip";                   break;
                case VK_DRIVER_ID_MESA_V3DV :                   str << "Mesa v3dv";                     break;
                case VK_DRIVER_ID_MESA_PANVK :                  str << "Mesa PanVk";                    break;
                case VK_DRIVER_ID_MESA_VENUS :                  str << "Mesa Venus";                    break;
                case VK_DRIVER_ID_MESA_DOZEN :                  str << "Mesa Dozen";                    break;
                case VK_DRIVER_ID_MESA_NVK :                    str << "Mesa NVK";                      break;
                case VK_DRIVER_ID_IMAGINATION_OPEN_SOURCE_MESA: str << "Mesa Img open source";          break;
                case VK_DRIVER_ID_MESA_AGXV :                   str << "Mesa AGXV";                     break;
                case VK_DRIVER_ID_MAX_ENUM :
                default :                                       str << "unknown";                       break;
            }
            switch_end
        }
        str << _GetVulkanExtensionsString();
        str << "\n----";
        AE_LOGI( str );

        str = "Vulkan device queues:";
        for (usize i = 0, cnt = _queueCount; i < cnt; ++i)
        {
            auto&   q = _queues[i];
            str << "\n  [" << ToString(i) << "] "
                << q.debugName.c_str()
                << ", family: " << ToString( uint(q.familyIndex) )
                << ", familyFlags: " << VkQueueFlagsToString( q.familyFlags );
        }
        str << "\n----";
        AE_LOGI( str );
    #endif
    }

/*
=================================================
    _InitQueues
=================================================
*/
    void  VDeviceInitializer::_InitQueues (ArrayView<VkQueueFamilyProperties> props, INOUT Queues_t &queues, INOUT QueueTypes_t &qtypes) C_NE___
    {
        qtypes = {};

        for (usize i = 0, cnt = _queueCount; i < cnt; ++i)
        {
            auto&       vq          = queues[i];
            auto&       q           = props[ uint(vq.familyIndex) ];
            const bool  is_video    = AnyBits( q.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR );

            vq.familyFlags  = BitCast<VkQueueFlagBits>(q.queueFlags);

            vq.timestampValidBits           = q.timestampValidBits;
            vq.minImageTransferGranularity  = { q.minImageTransferGranularity.width,
                                                q.minImageTransferGranularity.height,
                                                q.minImageTransferGranularity.depth };

            StaticAssert( uint(EQueueMask::All) == 0x1F );

            if ( AllBits( q.queueFlags, VK_QUEUE_GRAPHICS_BIT ) and
                 not is_video                                   and
                 qtypes[ uint(EQueueType::Graphics) ] == null   )
            {
                if ( vq.debugName.empty() )
                    vq.debugName = "Graphics";

                auto [stages, access] = VPipelineScope::GetStagesAndAccess({ EPipelineScope::Graphics, EPipelineScope::Compute, EPipelineScope::Transfer_Graphics,
                                                                             EPipelineScope::RayTracing, EPipelineScope::RTAS_Build, EPipelineScope::Host });

                vq.type             = EQueueType::Graphics;
                vq.supportedStages  = stages;
                vq.supportedAccess  = access;

                qtypes[ uint(vq.type) ] = &vq;
            }
            else
            // second graphics queue can be used for async compute
            if ( AllBits( q.queueFlags, VK_QUEUE_COMPUTE_BIT )      and
                 not is_video                                       and
                 qtypes[ uint(EQueueType::AsyncCompute) ] == null   )
            {
                if ( vq.debugName.empty() )
                    vq.debugName = "AsyncCompute";

                auto [stages, access] = VPipelineScope::GetStagesAndAccess({ EPipelineScope::Compute, EPipelineScope::Transfer_Copy,
                                                                             EPipelineScope::RayTracing, EPipelineScope::RTAS_Build, EPipelineScope::Host });

                vq.type             = EQueueType::AsyncCompute;
                vq.supportedStages  = stages;
                vq.supportedAccess  = access;

                qtypes[ uint(vq.type) ] = &vq;
            }
            else
            if ( AllBits( q.queueFlags, VK_QUEUE_TRANSFER_BIT )     and
                 not is_video                                       and
                 qtypes[ uint(EQueueType::AsyncTransfer) ] == null  )
            {
                if ( vq.debugName.empty() )
                    vq.debugName = "AsyncTransfer";

                auto [stages, access] = VPipelineScope::GetStagesAndAccess({ EPipelineScope::Transfer_Copy, EPipelineScope::Host });

                vq.type             = EQueueType::AsyncTransfer;
                vq.supportedStages  = stages;
                vq.supportedAccess  = access;

                qtypes[ uint(vq.type) ] = &vq;
            }
            else
            if ( AllBits( q.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR )     and
                 not AnyBits( q.queueFlags, VK_QUEUE_VIDEO_ENCODE_BIT_KHR ) and
                 qtypes[ uint(EQueueType::VideoDecode) ] == null            )
            {
                if ( vq.debugName.empty() )
                    vq.debugName = "VideoDecode";

                auto [stages, access] = VPipelineScope::GetStagesAndAccess({ EPipelineScope::Transfer_Copy, EPipelineScope::Video, EPipelineScope::Host });

                vq.type             = EQueueType::VideoDecode;
                vq.supportedStages  = stages;
                vq.supportedAccess  = access;

                qtypes[ uint(vq.type) ] = &vq;
            }
            else
            if ( AllBits( q.queueFlags, VK_QUEUE_VIDEO_ENCODE_BIT_KHR )     and
                 not AnyBits( q.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR ) and
                 qtypes[ uint(EQueueType::VideoEncode) ] == null            )
            {
                if ( vq.debugName.empty() )
                    vq.debugName = "VideoEncode";

                auto [stages, access] = VPipelineScope::GetStagesAndAccess({ EPipelineScope::Transfer_Copy, EPipelineScope::Video, EPipelineScope::Host });

                vq.type             = EQueueType::VideoEncode;
                vq.supportedStages  = stages;
                vq.supportedAccess  = access;

                qtypes[ uint(vq.type) ] = &vq;
            }
            else
            {
                CHECK_MSG( false, "too much queues" );
            }

            vq.supportedStages  |= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            vq.supportedAccess  |= VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
        }
    }

/*
=================================================
    _ValidateQueueStages
=================================================
*/
    void  VDeviceInitializer::_ValidateQueueStages (Queues_t &queues) C_NE___
    {
        VkPipelineStageFlagBits2    remove_stages   = 0;
        VkAccessFlagBits2           remove_access   = 0;

        if ( not _properties.features.tessellationShader )
            remove_stages |= VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT_KHR | VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT_KHR;

        if ( not _properties.features.geometryShader )
            remove_stages |= VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT_KHR;

        if ( not _properties.meshShaderFeats.meshShader )
            remove_stages |= VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT;

        if ( not _properties.meshShaderFeats.taskShader )
            remove_stages |= VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT;

        if ( not _properties.rayTracingPipelineFeats.rayTracingPipeline )
            remove_stages |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;

        if ( not _properties.accelerationStructureFeats.accelerationStructure )
        {
            remove_stages |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
            remove_access |= VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
        }
        if ( not _properties.fragShadingRateFeats.attachmentFragmentShadingRate )
        {
            remove_stages |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
            remove_access |= VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
        }
        if ( not _properties.subpassShadingHWFeats.subpassShading )
            remove_stages |= VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI;

        if ( not _properties.rayTracingMaintenance1Feats.rayTracingMaintenance1 )
        {
            remove_stages |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR;
            remove_access |= VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR;
        }

        for (usize i = 0, cnt = _queueCount; i < cnt; ++i)
        {
            auto&   q = queues[i];

            q.supportedStages   &= ~remove_stages;
            q.supportedAccess   &= ~remove_access;

            ASSERT( q.supportedStages != Zero );
            ASSERT( q.supportedAccess != Zero );
        }
    }

/*
=================================================
    _ValidateInstanceVersion
=================================================
*/
    void  VDeviceInitializer::_ValidateInstanceVersion (VkInstance instance, ArrayView<const char*> layers, ArrayView<const char*> extensions, INOUT uint &version) C_Th___
    {
        const uint  min_ver     = VK_API_VERSION_1_0;
        const uint  old_ver     = version;
        uint        current_ver = 0;

        VK_CHECK( vkEnumerateInstanceVersion( OUT &current_ver ));

        version = Min( Max( version, min_ver ), Max( current_ver, min_ver ));

        // Instance version can be greater than physical device version,
        // but actual version is min of instance and physical device versions.
        {
            VkInstance  tmp_inst = Default;

            // create temp instance
            if ( instance == Default )
            {
                Array< const char* >    instance_layers;
                Array< const char* >    instance_extensions = _GetInstanceExtensions( InstanceVersion{VK_VERSION_MAJOR(min_ver), VK_VERSION_MINOR(min_ver)} );

                instance_layers.assign( layers.begin(), layers.end() );                                         // throw
                instance_extensions.insert( instance_extensions.end(), extensions.begin(), extensions.end() );  // throw

                _ValidateInstanceLayers( INOUT instance_layers, True{"silent"} );                               // throw
                _ValidateInstanceExtensions( instance_layers, INOUT instance_extensions, True{"silent"} );      // throw

                VkApplicationInfo       app_info = {};
                app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                app_info.apiVersion         = min_ver;
                app_info.pApplicationName   = "temp";
                app_info.pEngineName        = "temp";

                VkInstanceCreateInfo    instance_ci = {};
                instance_ci.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instance_ci.pApplicationInfo        = &app_info;
                instance_ci.enabledLayerCount       = uint(instance_layers.size());
                instance_ci.ppEnabledLayerNames     = instance_layers.size() ? instance_layers.data() : null;
                instance_ci.enabledExtensionCount   = uint(instance_extensions.size());
                instance_ci.ppEnabledExtensionNames = instance_extensions.size() ? instance_extensions.data() : null;

                if ( ArrayContains( instance_extensions.begin(), instance_extensions.end(), StringView{VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME} ))
                    instance_ci.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

                VK_CHECK_ERRV( vkCreateInstance( &instance_ci, null, OUT &tmp_inst ));

                instance = tmp_inst;
                CHECK( VulkanLoader::LoadInstance( instance ));
            }

            // find min version for all physical devices
            if ( instance != Default )
            {
                FixedArray< VkPhysicalDevice, 16 >  devices;

                uint    count = 0;
                VK_CHECK( vkEnumeratePhysicalDevices( instance, OUT &count, null ));
                ASSERT( count > 0 );

                if ( count > 0 )
                {
                    devices.resize( count );
                    count = uint(devices.size());
                    VK_CHECK( vkEnumeratePhysicalDevices( instance, OUT &count, OUT devices.data() ));
                    devices.resize( Min( count, devices.size() ));

                    for (auto& dev : devices)
                    {
                        VkPhysicalDeviceProperties  prop = {};
                        vkGetPhysicalDeviceProperties( dev, OUT &prop );

                        version = Min( version, VK_MAKE_VERSION( VK_VERSION_MAJOR(prop.apiVersion), VK_VERSION_MINOR(prop.apiVersion), 0 ));
                    }
                }
            }

            // destroy temp instance
            if ( tmp_inst != Default )
            {
                vkDestroyInstance( tmp_inst, null );
                VulkanLoader::ResetInstance();
            }
        }

        if ( old_ver != version and _enableInfoLog )
            AE_LOG_DBG( "Vulkan instance version changed to: "s << ToString(VK_VERSION_MAJOR(version)) << '.' << ToString(VK_VERSION_MINOR(version)) );
    }

/*
=================================================
    _ValidateInstanceLayers
=================================================
*/
    void  VDeviceInitializer::_ValidateInstanceLayers (INOUT Array<const char*> &layers, Bool silent) C_Th___
    {
        // load supported layers
        uint    count = 0;
        VK_CHECK( vkEnumerateInstanceLayerProperties( OUT &count, null ));

        if ( count == 0 )
        {
            layers.clear();
            return;
        }

        Array<VkLayerProperties> inst_layers;
        inst_layers.resize( count );        // throw

        VK_CHECK( vkEnumerateInstanceLayerProperties( OUT &count, OUT inst_layers.data() ));
        inst_layers.resize( Min( count, inst_layers.size() ));

        // validate
        for (auto iter = layers.begin(); iter != layers.end();)
        {
            bool    found       = false;
            uint    spec_ver    = 0;

            for (auto& prop : inst_layers)
            {
                if_unlikely( StringView(*iter) == prop.layerName )
                {
                    found       = true;
                    spec_ver    = prop.specVersion;
                    break;
                }
            }

            if ( not found )
            {
                if_unlikely( not silent )
                    AE_LOG_DBG( "Removed layer '"s << (*iter) << "'" );

                iter = layers.erase( iter );
            }
            else
            {
                #ifdef AE_DEBUG
                if ( spec_ver < VK_HEADER_VERSION_COMPLETE and not silent )
                {
                    // this may cause a crash or false-positive in validation layer, you should update vulkan SDK.
                    AE_LOG_SE( "Instance layer '"s << *iter << "' version (" << ToString(VK_API_VERSION_MAJOR(spec_ver))
                                << '.' << ToString(VK_API_VERSION_MINOR(spec_ver)) << '.' << ToString(VK_API_VERSION_PATCH(spec_ver))
                                << ") is less than header version (" << ToString(VK_API_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE))
                                << '.' << ToString(VK_API_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE)) << '.' << ToString(VK_API_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE))
                                << ")" );
                }
                #endif

                ++iter;
            }
        }
    }

/*
=================================================
    _ValidateInstanceExtensions
=================================================
*/
    void  VDeviceInitializer::_ValidateInstanceExtensions (Array<const char*> layers, INOUT Array<const char*> &extensions, Bool silent) C_Th___
    {
        FlatHashSet<StringView> instance_extensions;

        // load supported extensions
        uint    count = 0;
        VK_CHECK( vkEnumerateInstanceExtensionProperties( null, OUT &count, null ));

        if ( count == 0 )
        {
            extensions.clear();
            return;
        }

        Array< VkExtensionProperties >      inst_ext;
        inst_ext.resize( count );   // throw

        VK_CHECK( vkEnumerateInstanceExtensionProperties( null, OUT &count, OUT inst_ext.data() ));
        inst_ext.resize( Min( count, inst_ext.size() ));

        for (auto& ext : inst_ext) {
            instance_extensions.insert( StringView(ext.extensionName) );
        }

        #if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_LINUX)
            // extension 'VK_EXT_validation_features' is supported if enabled validation layer
            if ( ArrayContains( layers.begin(), layers.end(), StringView{"VK_LAYER_KHRONOS_validation"} ))
                instance_extensions.insert( StringView{VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME} );
        #endif

        // validate
        for (auto iter = extensions.begin(); iter != extensions.end();)
        {
            if ( instance_extensions.find( StringView{*iter} ) == instance_extensions.end() )
            {
                if_unlikely( not silent )
                    AE_LOG_DBG( "Removed instance extension '"s << (*iter) << "'" );

                iter = extensions.erase( iter );
            }
            else
                 ++iter;
        }
    }

/*
=================================================
    _ValidateDeviceExtensions
=================================================
*/
    void  VDeviceInitializer::_ValidateDeviceExtensions (VkPhysicalDevice physDev, INOUT Array<const char*> &extensions) C_Th___
    {
        // load supported device extensions
        uint    count = 0;
        VK_CHECK( vkEnumerateDeviceExtensionProperties( physDev, null, OUT &count, null ));

        if ( count == 0 )
        {
            extensions.clear();
            return;
        }

        Array< VkExtensionProperties >  dev_ext;
        dev_ext.resize( count );    // throw

        VK_CHECK( vkEnumerateDeviceExtensionProperties( physDev, null, OUT &count, OUT dev_ext.data() ));
        dev_ext.resize( Min( count, dev_ext.size() ));


        // validate
        for (auto iter = extensions.begin(); iter != extensions.end();)
        {
            bool    found = false;

            for (auto& ext : dev_ext)
            {
                if ( StringView(*iter) == ext.extensionName )
                {
                    found = true;
                    break;
                }
            }

            if ( not found )
            {
                if_unlikely( _enableInfoLog )
                    AE_LOG_DBG( "Removed device extension '"s << (*iter) << "'" );

                iter = extensions.erase( iter );
            }
            else
                ++iter;
        }
    }

/*
=================================================
    _LogInstance
=================================================
*/
    void  VDeviceInitializer::_LogInstance (ArrayView<const char*> instanceLayers) C_Th___
    {
    #ifdef AE_ENABLE_LOGS
        String  str;
        str << "Created Vulkan instance " << ToString(_vkInstanceVersion.major) << '.' << ToString(_vkInstanceVersion.minor) << '\n';

        str << "Layers:\n";
        for (auto& layer : instanceLayers)
            str << "  " << layer << '\n';

        str << "Extensions:\n";
        for (auto& ext :  _instanceExtensions)
            str << "  " << ext.c_str() << '\n';

        AE_LOGI( str );
    #endif
    }

/*
=================================================
    _LogPhysicalDevices
=================================================
*/
    void  VDeviceInitializer::_LogPhysicalDevices () C_NE___
    {
        uint                                count   = 0;
        FixedArray< VkPhysicalDevice, 16 >  devices;

        VK_CHECK( vkEnumeratePhysicalDevices( GetVkInstance(), OUT &count, null ));
        CHECK_ERRV( count > 0 );

        devices.resize( count );
        count = uint(devices.size());

        VK_CHECK( vkEnumeratePhysicalDevices( GetVkInstance(), OUT &count, OUT devices.data() ));
        devices.resize( Min( count, devices.size() ));

        for (auto& dev : devices)
        {
            VkPhysicalDeviceProperties  prop = {};
            vkGetPhysicalDeviceProperties( dev, OUT &prop );

            AE_LOGI( "Found Vulkan device: "s << prop.deviceName );
        }
    }

/*
=================================================
    CreateDebugCallback
=================================================
*/
    bool  VDeviceInitializer::CreateDebugCallback (VkDebugUtilsMessageSeverityFlagsEXT severity, DebugReport_t &&callback) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( GetVkInstance() );

        #ifdef AE_CFG_RELEASE
        AE_LOG_SE( "Vulkan debug utils should not be used in release build" );
        #endif

        auto    dbg_report = _dbgReport.WriteNoLock();
        EXLOCK( dbg_report );

        if ( _extensions.debugUtils )
        {
            CHECK_ERR( dbg_report->debugUtilsMessenger == Default );

            VkDebugUtilsMessengerCreateInfoEXT  info = {};
            info.sType              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            info.messageSeverity    = severity;
            info.messageType        = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            info.pfnUserCallback    = _DebugUtilsCallback;
            info.pUserData          = this;

            VK_CHECK_ERR( vkCreateDebugUtilsMessengerEXT( GetVkInstance(), &info, null, OUT &dbg_report->debugUtilsMessenger ));

            dbg_report->callback = RVRef(callback);
            return true;
        }

        if ( _extensions.debugReport )
        {
            CHECK_ERR( dbg_report->debugReportCallback == Default );

            VkDebugReportCallbackCreateInfoEXT  info = {};
            info.sType          = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            info.flags          = MsgSeverityToReportFlags( severity );
            info.pfnCallback    = _DebugReportCallback;
            info.pUserData      = this;

            VK_CHECK_ERR( vkCreateDebugReportCallbackEXT( GetVkInstance(), &info, null, OUT &dbg_report->debugReportCallback ));

            dbg_report->callback = RVRef(callback);
            return true;
        }

        return false;
    }

/*
=================================================
    DestroyDebugCallback
=================================================
*/
    void  VDeviceInitializer::DestroyDebugCallback () __NE___
    {
        DRC_EXLOCK( _drCheck );

        auto    dbg_report = _dbgReport.WriteNoLock();
        EXLOCK( dbg_report );

        if ( GetVkInstance() and dbg_report->debugUtilsMessenger != Default ) {
            vkDestroyDebugUtilsMessengerEXT( GetVkInstance(), dbg_report->debugUtilsMessenger, null );
        }

        if ( GetVkInstance() and dbg_report->debugReportCallback != Default ) {
            vkDestroyDebugReportCallbackEXT( GetVkInstance(), dbg_report->debugReportCallback, null );
        }

        dbg_report->debugUtilsMessenger = Default;
        dbg_report->debugReportCallback = Default;
    }

/*
=================================================
    _DebugUtilsCallback
=================================================
*/
    VKAPI_ATTR VkBool32 VKAPI_CALL
        VDeviceInitializer::_DebugUtilsCallback (VkDebugUtilsMessageSeverityFlagBitsEXT         messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT                /*messageTypes*/,
                                                 const VkDebugUtilsMessengerCallbackDataEXT*    pCallbackData,
                                                 void*                                          pUserData) __NE___
    {
        using VErrorName = NamedID< 128, 0x1834'1292, false >;

        static constexpr VErrorName stage_mask1     {"VUID-vkCmdPipelineBarrier2-srcStageMask-03849"};  // __ false-positive on queue ownership transfer
        static constexpr VErrorName stage_mask2     {"VUID-vkCmdPipelineBarrier2-dstStageMask-03850"};  // /
        static constexpr VErrorName img_fmt_list    {"VUID-VkImageViewCreateInfo-image-01762"};         // - false-positive: image format list allows to create image view with different format without VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT
        static constexpr VErrorName iface_mismatch  {"UNASSIGNED-CoreValidation-Shader-InterfaceTypeMismatch"};
        static constexpr VErrorName access_mask1    {"VUID-VkMemoryBarrier2-srcAccessMask-07454"};      // \ https://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/6628
        static constexpr VErrorName access_mask2    {"VUID-VkMemoryBarrier2-dstAccessMask-07454"};      // / (for sdk 1.3.261.1)

        auto*               self    = static_cast<VDeviceInitializer *>(pUserData);
        const VErrorName    msg_id  {pCallbackData->pMessageIdName};

        if ( (msg_id == stage_mask1)    or (msg_id == stage_mask2)  or (msg_id == img_fmt_list) or
             (msg_id == iface_mismatch) or (msg_id == access_mask1) or (msg_id == access_mask2) )
            return VK_FALSE;

        auto    dbg_report = self->_dbgReport.WriteNoLock();
        EXLOCK( dbg_report );

        TRY{
            dbg_report->tempObjectDbgInfos.resize( pCallbackData->objectCount );    // throw

            for (usize i = 0; i < dbg_report->tempObjectDbgInfos.size(); ++i)
            {
                auto&   obj = pCallbackData->pObjects[i];

                dbg_report->tempObjectDbgInfos[i] = { VkObjectTypeToString( obj.objectType ),
                                                      obj.pObjectName ? StringView{obj.pObjectName} : StringView{},
                                                      obj.objectHandle };
            }

            self->_DebugReport( dbg_report->tempString,
                                dbg_report->breakOnValidationError,
                                dbg_report->callback,
                                { dbg_report->tempObjectDbgInfos, pCallbackData->pMessage,
                                  AllBits( messageSeverity, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT )
                                });                                                                         // throw
        }CATCH_ALL();

        // The application should always return VK_FALSE.
        // The VK_TRUE value is reserved for use in layer development.
        return VK_FALSE;
    }

/*
=================================================
    _DebugReportCallback
=================================================
*/
    VKAPI_ATTR VkBool32 VKAPI_CALL
        VDeviceInitializer::_DebugReportCallback (VkDebugReportFlagsEXT      flags,
                                                  VkDebugReportObjectTypeEXT objectType,
                                                  ulong                      object,
                                                  usize                      /*location*/,
                                                  int                        /*messageCode*/,
                                                  const char*                /*pLayerPrefix*/,
                                                  const char*                pMessage,
                                                  void*                      pUserData) __NE___
    {
        auto*   self        = static_cast<VDeviceInitializer *>(pUserData);
        auto    dbg_report  = self->_dbgReport.WriteNoLock();
        EXLOCK( dbg_report );

        TRY{
            dbg_report->tempObjectDbgInfos.resize( 1 ); // throw

            dbg_report->tempObjectDbgInfos[0] = { VkObjectTypeToString(DebugReportObjectTypeToObjectType( objectType )), "", object };

            self->_DebugReport( dbg_report->tempString,
                                dbg_report->breakOnValidationError,
                                dbg_report->callback,
                                { dbg_report->tempObjectDbgInfos, pMessage, AllBits( flags, VK_DEBUG_REPORT_ERROR_BIT_EXT ) }); // throw
        }
        CATCH_ALL()

        return VK_FALSE;
    }

/*
=================================================
    _DebugReport
=================================================
*/
    void  VDeviceInitializer::_DebugReport (INOUT String &str, bool breakOnError, DebugReport_t &callback, const DebugReport &msg) __Th___
    {
        if ( callback )
            return callback( msg );

      #ifdef AE_ENABLE_LOGS
        str << msg.message << '\n';

        for (auto& obj : msg.objects)
        {
            str << "object{ " << obj.type << ", \"" << obj.name << "\", " << ToString(obj.handle) << " }\n";
        }
        str << "----------------------------\n";

        if ( breakOnError and msg.isError ){
            AE_LOGE( str );
        }else{
            AE_LOG_SE( str );
        }
      #endif
    }

/*
=================================================
    GetRecommendedInstanceLayers
=================================================
*/
    ArrayView<const char*>  VDeviceInitializer::GetRecommendedInstanceLayers () __NE___
    {
        static const char*  instance_layers[] =
        {
            "VK_LAYER_KHRONOS_validation"

        #if defined(AE_PLATFORM_WINDOWS) and defined(AE_DEBUG)
        //  , "VK_LAYER_LUNARG_device_simulation"
        #endif
        };
        return instance_layers;
    }

/*
=================================================
    Init
=================================================
*/
    bool  VDeviceInitializer::Init (const VDeviceInitializer &otherDev) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _vkLogicalDevice == Default );
        CHECK_ERR( _vkPhysicalDevice == Default );
        CHECK_ERR( _vkInstance == Default );

        CHECK_ERR( otherDev.IsInitialized() );
        DRC_SHAREDLOCK( otherDev._drCheck );

        _isCopy         = true;
        _enableInfoLog  = false;    // disable duplicated messages

        _vkInstance         = otherDev._vkInstance;
        _vkPhysicalDevice   = otherDev._vkPhysicalDevice;
        _vkLogicalDevice    = otherDev._vkLogicalDevice;
        _vkInstanceVersion  = otherDev._vkInstanceVersion;
        _vkDeviceVersion    = otherDev._vkDeviceVersion;

        // copy queues
        _queueMask  = otherDev._queueMask;
        _queueCount = otherDev._queueCount;

        ASSERT( _queueCount == BitCount( _queueMask ));

        for (usize i = 0, cnt = _queueCount; i < cnt; ++i)
        {
            auto&   q = _queues[i];

            // copy fields except mutex
            constexpr Bytes offset {offsetof( VQueue, handle )};
            MemCopy( &q + offset, &otherDev._queues[i] + offset, Sizeof(q) - offset );

            _queueTypes[ uint(q.type) ] = &q;
        }

        _extensions = otherDev._extensions;

        NOTHROW(
            _instanceExtensions = otherDev._instanceExtensions;
            _deviceExtensions   = otherDev._deviceExtensions;
        )

        // initialize
        CHECK_ERR( VulkanLoader::Initialize() );
        _extEmulation.OnInitialize();

        CHECK_ERR( VulkanLoader::LoadInstance( _vkInstance ));
        VulkanLoader::SetupInstanceBackwardCompatibility( _vkInstanceVersion.Cast<0>() );
        _extEmulation.OnLoadInstance( _vkInstance, _vkInstanceVersion.Cast<0>() );

        _CheckInstanceExtensions();
        _CheckDeviceExtensions();

        CHECK_ERR( VulkanLoader::LoadDevice( _vkLogicalDevice, OUT _deviceFnTable ));
        VulkanLoader::SetupDeviceBackwardCompatibility( _vkDeviceVersion.Cast<0>(), INOUT _deviceFnTable );

        _ValidateQueueStages( _queues );
        _SetResourceFlags( OUT _resFlags );
        _ValidateSpirvVersion( OUT _spirvVersion );

        return true;
    }

/*
=================================================
    Init
=================================================
*/
    bool  VDeviceInitializer::Init (const GraphicsCreateInfo &ci, ArrayView<const char*> instanceExtensions) __NE___
    {
        DRC_EXLOCK( _drCheck );

        EDeviceFlags    dev_flags = ci.device.devFlags;

        #ifdef AE_CFG_RELEASE
        {
            constexpr auto  dbg_flags = EDeviceFlags::_NvApiMask | EDeviceFlags::_ArmProfMask | EDeviceFlags::EnableRenderDoc;
            dev_flags &= dbg_flags;
        }
        #endif

        if ( AnyBits( dev_flags, EDeviceFlags::_NvApiMask ))
            LoadNvPerf();

        if ( AnyBits( dev_flags, EDeviceFlags::_ArmProfMask ))
            LoadArmProfiler();

        // instance
        {
            const bool  enable_validation = (ci.device.validation != EDeviceValidation::Disabled);

            // presets
            const VkValidationFeatureDisableEXT minimal_disable_feats [] = {
                // keep VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT
                VK_VALIDATION_FEATURE_DISABLE_SHADERS_EXT, VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT,
                VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT, VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT,
                VK_VALIDATION_FEATURE_DISABLE_UNIQUE_HANDLES_EXT, VK_VALIDATION_FEATURE_DISABLE_SHADER_VALIDATION_CACHE_EXT };

            const VkValidationFeatureEnableEXT  synchronization_enable_feats [] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
            const auto&                         synchronization_disable_feats   = minimal_disable_feats;

            const VkValidationFeatureEnableEXT  bestpractices_enable_feats  []  = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
            const auto&                         bestpractices_disable_feats     = minimal_disable_feats;

            const VkValidationFeatureEnableEXT  shaderbased_enable_feats  []    = { VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT, VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT };

            const VkValidationFeatureEnableEXT  printf_enable_feats  []         = { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };

            Array<const char*>  layers;
            if ( enable_validation )
            {
                auto    validation_layers = GetRecommendedInstanceLayers();
                layers.insert( layers.end(), validation_layers.begin(), validation_layers.end() );
            }

            if ( AnyBits( dev_flags, EDeviceFlags::EnableRenderDoc ))
                layers.push_back( RenderDocApi::GetVkLayer() );

            InstanceCreateInfo  instance_ci;
            instance_ci.appName             = ci.device.appName;
            instance_ci.engineName          = AE_ENGINE_NAME;
            instance_ci.instanceLayers      = layers;
            instance_ci.instanceExtensions  = instanceExtensions;

            switch_enum( ci.device.validation )
            {
                case EDeviceValidation::Disabled :
                case EDeviceValidation::Enabled :
                    break;

                case EDeviceValidation::MinimalPreset :
                    instance_ci.enableValidations   = {};
                    instance_ci.disableValidations  = minimal_disable_feats;
                    break;

                case EDeviceValidation::SynchronizationPreset :
                    instance_ci.enableValidations   = synchronization_enable_feats;
                    instance_ci.disableValidations  = synchronization_disable_feats;
                    break;

                case EDeviceValidation::BestPracticesPreset :
                    instance_ci.enableValidations   = bestpractices_enable_feats;
                    instance_ci.disableValidations  = bestpractices_disable_feats;
                    break;

                case EDeviceValidation::ShaderBasedPreset :
                    instance_ci.enableValidations   = shaderbased_enable_feats;
                    instance_ci.disableValidations  = {};
                    break;

                case EDeviceValidation::ShaderPrintfPreset :
                    instance_ci.enableValidations   = printf_enable_feats;
                    instance_ci.disableValidations  = {};
                    break;

                default_unlikely :
                    DBG_WARNING( "unknown validation type" );
                    break;
            }
            switch_end

            CHECK_ERR( CreateInstance( instance_ci ));

            if ( enable_validation )
            {
                CreateDebugCallback( DefaultDebugMessageSeverity,
                                     [] (const VDeviceInitializer::DebugReport &rep) { AE_LOG_SE(rep.message);  CHECK(not rep.isError); });
            }

            if ( AnyBits( dev_flags, EDeviceFlags::EnableRenderDoc ))
                LoadRenderDoc();
        }

        // device
        {
            if ( not ci.device.deviceName.empty() )
                Unused( ChooseDevice( ci.device.deviceName ));

            if ( GetVkPhysicalDevice() == Default )
                CHECK_ERR( ChooseHighPerformanceDevice() );

            CHECK_ERR( CreateDefaultQueues( ci.device.requiredQueues, ci.device.optionalQueues ));
            CHECK_ERR( CreateLogicalDevice() );
        }

        if ( AllBits( dev_flags, EDeviceFlags::SetStableClock ) and _nvPerf.IsInitialized() )
            _nvPerf.SetStableClockState( true );

        return true;
    }


#   define VKFEATS_FN_IMPL
#   include "vulkan_loader/vk_features.h"
#   undef  VKFEATS_FN_IMPL


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
