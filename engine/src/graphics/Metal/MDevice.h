// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "base/Utils/Version.h"
# include "base/CompileTime/StringToID.h"
# include "graphics/Public/DeviceProperties.h"
# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/FeatureSet.h"
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/GraphicsCreateInfo.h"
# include "graphics/Metal/MQueue.h"
# include "graphics/Metal/MFeatureSet.h"

namespace AE::Graphics
{

    //
    // Metal Device
    //

    class MDevice : public Noncopyable
    {
    // types
    public:
        using MetalVersion      = TVersion2< "Metal"_StringToID >;

        using ResourceFlags     = DeviceResourceFlags;
        using DevMemoryInfoOpt  = Optional< DeviceMemoryInfo >;

        using MFeatures         = MFeatureSet::Features;
        using MProperties       = MFeatureSet::Properties;


    protected:
        using Queues_t          = FixedArray< MQueue, MConfig::MaxQueues >;
        using QueueTypes_t      = StaticArray< MQueuePtr, uint(EQueueType::_Count) >;


    // variables
    protected:
        MetalDeviceRC           _device;
        MetalVersion            _version;

        EQueueMask              _queueMask      = Default;
        QueueTypes_t            _queueTypes     = {};
        Queues_t                _queues;

        DeviceProperties        _devProps;      // platform independent
        MFeatureSet             _mtlFS;
        ResourceFlags           _resFlags;

        FixedString<64>         _devName;

        DRC_ONLY(
            RWDataRaceCheck     _drCheck;
        )


    // methods
    public:
        MDevice ()                                                  __NE___;
        ~MDevice ()                                                 __NE___;

        ND_ DeviceProperties const& GetDeviceProperties ()          C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _devProps; }
        ND_ MFeatures const&        GetFeatures ()                  C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _mtlFS.features; }
        ND_ MProperties const&      GetProperties ()                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _mtlFS.properties; }
        ND_ ResourceFlags const&    GetResourceFlags ()             C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _resFlags; }

        ND_ MetalVersion            GetVersion ()                   C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _version; }
        ND_ MetalDevice             GetMtlDevice ()                 C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _device; }
        ND_ ArrayView<MQueue>       GetQueues ()                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _queues; }
        ND_ MQueuePtr               GetQueue (EQueueType type)      C_NE___ { DRC_SHAREDLOCK( _drCheck );  return uint(type) < _queueTypes.size() ? _queueTypes[uint(type)] : null; }
        ND_ EQueueMask              GetAvailableQueues ()           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _queueMask; }
        ND_ StringView              GetDeviceName ()                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _devName; }

        ND_ bool                    IsInitialized ()                C_NE___ { DRC_SHAREDLOCK( _drCheck );  return bool(_device); }

        ND_ DevMemoryInfoOpt        GetMemoryUsage ()               C_NE___;

        ND_ bool  CheckConstantLimits ()                            C_NE___;
        ND_ bool  CheckExtensions ()                                C_NE___;

            void  InitFeatureSet (OUT FeatureSet &outFeatureSet)    C_NE___;

    protected:
        ND_ MGPUFamilies  _GetGPUFamilies ()                        C_NE___;
    };



    //
    // Metal Device Initializer
    //

    class MDeviceInitializer final : public MDevice
    {
    // variables
    private:
        const bool      _enableInfoLog  = false;


    // methods
    public:
        explicit MDeviceInitializer (bool enableInfoLog = false)                            __NE___;
        ~MDeviceInitializer ()                                                              __NE___;

        ND_ bool  ChooseHighPerformanceDevice ()                                            __NE___;
        ND_ bool  CreateDefaultQueue ()                                                     __NE___;
        ND_ bool  CreateDefaultQueues (EQueueMask required, EQueueMask optional = Default)  __NE___;

        ND_ bool  CreateLogicalDevice ()                                                    __NE___;
            bool  DestroyLogicalDevice ()                                                   __NE___;

        ND_ bool  Init (const GraphicsCreateInfo &ci)                                       __NE___;

    private:
        void  _SetResourceFlags (OUT ResourceFlags &)                                       C_NE___;
        void  _InitFeaturesAndProperties (OUT MFeatures &, OUT MProperties &)               C_NE___;
        bool  _CreateLogicalDevice ()                                                       __Th___;

        void  _LogLogicalDevice ()                                                          C_Th___;
    };


} // AE::Graphics

#endif // AE_ENABLE_METAL
