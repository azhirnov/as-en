// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

    //
    // Vulkan Ray Tracing Geometry
    //

    class VRTGeometry final
    {
    // variables
    private:
        DeviceAddress               _address        = Default;
        VkAccelerationStructureKHR  _accelStruct    = Default;
        VkBuffer                    _buffer         = Default;
        RTGeometryDesc              _desc;

        Strong<MemoryID>            _memoryId;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        VRTGeometry ()                                                                                                      __NE___ {}
        ~VRTGeometry ()                                                                                                     __NE___;

        ND_ bool  Create (VResourceManager &, const RTGeometryDesc &, GfxMemAllocatorPtr, StringView dbgName)               __NE___;
            void  Destroy (VResourceManager &)                                                                              __NE___;

        ND_ VkAccelerationStructureKHR  Handle ()                                                                           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _accelStruct; }
        ND_ DeviceAddress               GetDeviceAddress ()                                                                 C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _address; }
        ND_ RTGeometryDesc const&       Description ()                                                                      C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _desc; }
        ND_ MemoryID                    MemoryId ()                                                                         C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
        ND_ bool                        IsExclusiveSharing ()                                                               C_NE___ { return false; }

        DEBUG_ONLY(  ND_ StringView     GetDebugName ()                                                                     C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })

        ND_ static bool             IsSerializedMemoryCompatible (const VDevice &dev, const void* ptr, Bytes size)          __NE___;

        ND_ static RTASBuildSizes   GetBuildSizes (const VResourceManager &, const RTGeometryBuild &desc)                   __NE___;

        ND_ static bool             ConvertBuildInfo (const VResourceManager &, IAllocatorRef allocator, const RTGeometryBuild &,
                                                      OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges,
                                                      OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo)           __NE___;

        ND_ static bool             ConvertBuildInfo (const VResourceManager &, IAllocatorRef allocator,
                                                      const RTGeometryBuild &, OUT uint* &maxPrimitiveCounts,
                                                      OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo)           __NE___;

        ND_ static bool             IsSupported (const VResourceManager &, const RTGeometryDesc &desc)                      __NE___;
        ND_ static bool             IsSupported (const VResourceManager &, const RTGeometryBuild &build)                    __NE___;

    private:
        template <bool IsForBuilding>
        ND_ static bool  _Convert (const VResourceManager &, const RTGeometryBuild &desc, IAllocatorRef allocator,
                                   OUT uint* &primitiveCount, OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges,
                                   OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo)                              __NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
