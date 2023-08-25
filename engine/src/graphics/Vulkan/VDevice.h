// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DeviceProperties.h"
# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/FeatureSet.h"
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Vulkan/VQueue.h"
# include "graphics/Vulkan/Utils/VNvPerf.h"
# include "VulkanExtEmulation.h"

namespace AE::Graphics
{

    //
    // Vulkan Device
    //

    class VDevice : public VulkanDeviceFn, public Noncopyable
    {
    // types
    public:
        using InstanceVersion   = TVersion2< "VkInstance"_StringToID >;
        using DeviceVersion     = TVersion2< "VkDevice"_StringToID >;
        using SpirvVersion      = TVersion2< "SPIRV"_StringToID >;

        #define VKFEATS_STRUCT
        #include "vulkan_loader/vk_features.h"
        #undef  VKFEATS_STRUCT

        using ResourceFlags     = DeviceResourceFlags;
        using DevMemoryInfoOpt  = Optional< DeviceMemoryInfo >;


    protected:
        using Queues_t          = FixedArray< VQueue, VConfig::MaxQueues >;
        using QueueTypes_t      = StaticArray< VQueuePtr, uint(EQueueType::_Count) >;

        using ExtensionName_t   = FixedString<VK_MAX_EXTENSION_NAME_SIZE>;
        using ExtensionSet_t    = FlatHashSet< ExtensionName_t >;

        static constexpr uint   MaxQueueFamilies = 32;
        using QueueFamilyProperties_t   = FixedArray< VkQueueFamilyProperties, MaxQueueFamilies >;
        using QueueCount_t              = FixedArray< uint, MaxQueueFamilies >;


    // variables
    protected:
        VkDevice                _vkLogicalDevice        = Default;

        EQueueMask              _queueMask              = Default;
        QueueTypes_t            _queueTypes             = {};
        Queues_t                _queues;

        VkPhysicalDevice        _vkPhysicalDevice       = Default;
        VkInstance              _vkInstance             = Default;
        InstanceVersion         _vkInstanceVersion;
        DeviceVersion           _vkDeviceVersion;
        SpirvVersion            _spirvVersion;

        DeviceProperties        _devProps;              // platform independent
        VExtensions             _extensions;            // large

        VulkanDeviceFnTable     _deviceFnTable;         // large
        ResourceFlags           _resFlags;

        ExtensionSet_t          _instanceExtensions;
        ExtensionSet_t          _deviceExtensions;

        VProperties             _properties;            // very large

        VNvPerf                 _nvPerf;

        NO_UNIQUE_ADDRESS
          VulkanExtEmulation    _extEmulation;

        DRC_ONLY(
            RWDataRaceCheck     _drCheck;
        )


    // methods
    public:
        VDevice ()                                                                  __NE___;
        ~VDevice ()                                                                 __NE___;

        ND_ VExtensions const&      GetVExtensions ()                               C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _extensions; }
        ND_ VProperties const&      GetVProperties ()                               C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _properties; }
        ND_ ResourceFlags const&    GetResourceFlags ()                             C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _resFlags; }
        ND_ DeviceProperties const& GetDeviceProperties ()                          C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _devProps; }

        ND_ VkDevice                GetVkDevice ()                                  C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _vkLogicalDevice; }
        ND_ VkPhysicalDevice        GetVkPhysicalDevice ()                          C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _vkPhysicalDevice; }
        ND_ VkInstance              GetVkInstance ()                                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _vkInstance; }
        ND_ InstanceVersion         GetInstanceVersion ()                           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _vkInstanceVersion; }
        ND_ DeviceVersion           GetDeviceVersion ()                             C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _vkDeviceVersion; }
        ND_ SpirvVersion            GetSpirvVersion ()                              C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _spirvVersion; }
        ND_ Version2                GetVkVersion ()                                 C_NE___ { return GetDeviceVersion().Cast<0>(); }
        ND_ ArrayView<VQueue>       GetQueues ()                                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _queues; }
        ND_ VQueuePtr               GetQueue (EQueueType type)                      C_NE___ { DRC_SHAREDLOCK( _drCheck );  return uint(type) < _queueTypes.size() ? _queueTypes[uint(type)] : null; }
        ND_ EQueueMask              GetAvailableQueues ()                           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _queueMask; }
        ND_ StringView              GetDeviceName ()                                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _properties.properties.deviceName; }

        ND_ bool                    IsInitialized ()                                C_NE___ { return GetVkDevice() != Default; }

        ND_ VNvPerf const&          GetNvPerf ()                                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _nvPerf; }
        ND_ bool                    HasNvPerf ()                                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _nvPerf.IsInitialized(); }

        ND_ DevMemoryInfoOpt        GetMemoryUsage ()                               C_NE___;

        ND_ bool                    IsUnderDebugger ()                              C_NE___;


        // check extensions
        ND_ bool  HasInstanceExtension (StringView name)                            C_NE___;
        ND_ bool  HasDeviceExtension (StringView name)                              C_NE___;

        template <typename T>
            bool  SetObjectName (T id, NtStringView name, VkObjectType type)        C_NE___;
            bool  SetObjectName (ulong id, NtStringView name, VkObjectType type)    C_NE___;

            void  GetQueueFamilies (EQueueMask mask, OUT VQueueFamilyIndices_t &)   C_NE___;

        ND_ bool  GetMemoryTypeIndex (uint memoryTypeBits,
                                      VkMemoryPropertyFlagBits includeFlags,
                                      VkMemoryPropertyFlagBits optIncludeFlags,
                                      VkMemoryPropertyFlagBits excludeFlags,
                                      VkMemoryPropertyFlagBits optExcludeFlags,
                                      OUT uint &memoryTypeIndex)                    C_NE___;
        ND_ bool  GetMemoryTypeIndex (uint memoryTypeBits, EMemoryType memType,
                                      OUT uint &memoryTypeIndex)                    C_NE___;

        ND_ bool  CheckConstantLimits ()                                            C_NE___;
        ND_ bool  CheckExtensions ()                                                C_NE___;

            void  InitFeatureSet (OUT FeatureSet &)                                 C_NE___;
    };



    //
    // Vulkan Device Initializer
    //

    class VDeviceInitializer final : public VDevice
    {
    // types
    public:
        struct InstanceCreateInfo
        {
            StringView                                  appName             = AE_ENGINE_NAME " App";
            StringView                                  engineName          = AE_ENGINE_NAME;
            ArrayView<const char*>                      instanceLayers;
            ArrayView<const char*>                      instanceExtensions;
            InstanceVersion                             version             {1,3};
            uint                                        appVer              = 0;
            uint                                        engineVer           = 0;
            ArrayView<VkValidationFeatureEnableEXT>     enableValidations;
            ArrayView<VkValidationFeatureDisableEXT>    disableValidations;
        };

        struct QueueCreateInfo
        {
        // variables
            VkQueueFlagBits             includeFlags    = Zero;
            VkQueueFlagBits             excludeFlags    = Zero;
            float                       priority        = 0.0f;
            VkQueueGlobalPriorityKHR    globalPriority  = VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_KHR;
            FixedString<64>             debugName;

        // methods
            QueueCreateInfo () __NE___ {}

            explicit QueueCreateInfo (VkQueueFlagBits           includeFlags,
                                      VkQueueFlagBits           excludeFlags    = Zero,
                                      float                     priority        = 0.0f,
                                      VkQueueGlobalPriorityKHR  globalPriority  = VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_KHR,
                                      StringView                name            = {}) __NE___ :
                includeFlags{includeFlags}, excludeFlags{excludeFlags},
                priority{priority}, globalPriority{globalPriority},
                debugName{name}
            {}
        };

        struct ObjectDbgInfo
        {
            StringView      type;
            StringView      name;
            ulong           handle;
        };

        struct DebugReport
        {
            ArrayView<ObjectDbgInfo>    objects;
            StringView                  message;
            bool                        isError     = false;
        };
        using DebugReport_t = Function< void (const DebugReport &) >;

    private:
        struct DbgReportData
        {
            VkDebugReportCallbackEXT    debugReportCallback     = Default;
            VkDebugUtilsMessengerEXT    debugUtilsMessenger     = Default;
            DebugReport_t               callback;

            bool                        breakOnValidationError  = true;
            Array<ObjectDbgInfo>        tempObjectDbgInfos;
            String                      tempString;
        };


    // variable
    private:
        Synchronized< Mutex,
            DbgReportData >         _dbgReport;

        bool                        _enableInfoLog          = false;
        bool                        _isCopy                 = false;


    // methods
    public:
        explicit VDeviceInitializer (Bool enableInfoLog = False{})                                  __NE___;
        ~VDeviceInitializer ()                                                                      __NE___;

        ND_ InstanceVersion  GetMaxInstanceVersion ()                                               C_NE___;

            bool  LoadNvPerf ()                                                                     __NE___;
            bool  LoadArmProfiler ()                                                                __NE___;

        ND_ bool  CreateInstance (const InstanceCreateInfo &ci)                                     __NE___;

        //ND_ bool  SetInstance (VkInstance value, const InstanceCreateInfo &ci)                    __NE___;

            bool  DestroyInstance ()                                                                __NE___;

        ND_ bool  ChooseDevice (StringView deviceName)                                              __NE___;
        ND_ bool  ChooseHighPerformanceDevice ()                                                    __NE___;
        ND_ bool  SetPhysicalDevice (VkPhysicalDevice value)                                        __NE___;

        ND_ bool  CreateDefaultQueue ()                                                             __NE___;
        ND_ bool  CreateDefaultQueues (EQueueMask required, EQueueMask optional = Default)          __NE___;
        ND_ bool  CreateQueues (ArrayView<QueueCreateInfo> queues)                                  __NE___;

        ND_ bool  CreateLogicalDevice (ArrayView<const char*>   extensions          = {},
                                       const FeatureSet*        fsToDeviceFeatures  = null)         __NE___;
            //bool  SetLogicalDevice (VkDevice value, ArrayView<const char*> extensions = {})       __NE___;
            bool  DestroyLogicalDevice ()                                                           __NE___;

            bool  CreateDebugCallback (VkDebugUtilsMessageSeverityFlagsEXT  severity,
                                       DebugReport_t &&                     callback = Default)     __NE___;
            void  DestroyDebugCallback ()                                                           __NE___;

        ND_ bool  Init (const VDeviceInitializer &otherDev)                                         __NE___;
        ND_ bool  Init (const GraphicsCreateInfo &ci, ArrayView<const char*> instanceExtensions)    __NE___;

        ND_ static ArrayView<const char*>   GetRecommendedInstanceLayers ()                         __NE___;

        ND_ VulkanDeviceFnTable &           EditDeviceFnTable ()                                    __NE___ { DRC_EXLOCK( _drCheck ); return _deviceFnTable; }


    private:
        #define VKFEATS_FN_DECL
        #include "vulkan_loader/vk_features.h"
        #undef  VKFEATS_FN_DECL

        ND_ bool  _CreateInstance (NtStringView appName, NtStringView engineName, ArrayView<const char*> instanceLayers,
                                   ArrayView<const char*> instanceExtensions, InstanceVersion version,
                                   uint appVer, uint engineVer, const VkValidationFeaturesEXT *pValidation)             __Th___;

        void  _ValidateInstanceVersion (VkInstance instance, ArrayView<const char*> layers, INOUT uint &version)        C_Th___;
        void  _ValidateInstanceLayers (INOUT Array<const char*> &layers)                                                C_Th___;
        void  _ValidateInstanceExtensions (Array<const char*> layers, INOUT Array<const char*> &ext)                    C_Th___;
        bool  _ChooseHighPerformanceDevice ()                                                                           __Th___;
        bool  _CreateLogicalDevice (ArrayView<const char*> extensions, const FeatureSet* fsToDeviceFeatures)            __Th___;
        void  _ValidateDeviceExtensions (VkPhysicalDevice physDev, INOUT Array<const char*> &ext)                       C_Th___;
        void  _ValidateSpirvVersion (OUT SpirvVersion &ver)                                                             C_NE___;
        void  _UpdateDeviceVersion (VkPhysicalDevice physicalDevice, OUT DeviceVersion &devVersion)                     C_NE___;
        void  _SetResourceFlags (OUT ResourceFlags &)                                                                   C_NE___;
        void  _OnCreateInstance (ArrayView<const char*> instanceExtensions, ArrayView<const char*> instanceLayers)      __Th___;

        void  _LogInstance (ArrayView<const char*> instanceLayers)                                                      C_Th___;
        void  _LogPhysicalDevices ()                                                                                    C_NE___;
        void  _LogLogicalDevice ()                                                                                      C_Th___;
        void  _LogResourceFlags ()                                                                                      C_Th___;
        void  _LogExternalTools ()                                                                                      C_Th___;

        void  _InitQueues (ArrayView<VkQueueFamilyProperties> props, INOUT Queues_t &queues, INOUT QueueTypes_t &qtypes)C_NE___;
        void  _ValidateQueueStages (INOUT Queues_t &queues)                                                             C_NE___;

        ND_ bool  _InitFeaturesAndPropertiesByFeatureSet (const FeatureSet &fs)                                         __NE___;

        // new debug api
        VKAPI_ATTR static VkBool32 VKAPI_CALL
            _DebugUtilsCallback (VkDebugUtilsMessageSeverityFlagBitsEXT         messageSeverity,
                                 VkDebugUtilsMessageTypeFlagsEXT                messageTypes,
                                 const VkDebugUtilsMessengerCallbackDataEXT*    pCallbackData,
                                 void*                                          pUserData) __NE___;

        // old debug api for mobile devices
        VKAPI_ATTR static VkBool32 VKAPI_CALL
            _DebugReportCallback (VkDebugReportFlagsEXT         flags,
                                  VkDebugReportObjectTypeEXT    objectType,
                                  ulong                         object,
                                  usize                         /*location*/,
                                  int                           /*messageCode*/,
                                  const char*                   /*pLayerPrefix*/,
                                  const char*                   pMessage,
                                  void*                         pUserData) __NE___;

        void  _DebugReport (INOUT String &, bool breakOnError, DebugReport_t &cb, const DebugReport &) __Th___;
    };

    static constexpr VkDebugUtilsMessageSeverityFlagsEXT    DefaultDebugMessageSeverity =   //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                                                            //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

/*
=================================================
    SetObjectName
=================================================
*/
    template <typename T>
    bool  VDevice::SetObjectName (T id, NtStringView name, VkObjectType type) C_NE___
    {
        STATIC_ASSERT( sizeof(T) <= sizeof(ulong) );
        return SetObjectName( ulong(id), name, type );
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
