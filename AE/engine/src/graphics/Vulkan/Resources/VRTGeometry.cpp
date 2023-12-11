// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Private/EnumUtils.h"

namespace AE::Graphics
{

/*
=================================================
    destructor
=================================================
*/
    VRTGeometry::~VRTGeometry () __NE___
    {
        DRC_EXLOCK( _drCheck );
        ASSERT( _buffer == Default );
        ASSERT( _accelStruct == Default );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VRTGeometry::Create (VResourceManager &resMngr, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _buffer == Default );
        CHECK_ERR( _accelStruct == Default );
        CHECK_ERR( desc.size > 0 );

        _desc = desc;
        GRES_CHECK( IsSupported( resMngr, desc ));

        // create buffer
        VkBufferCreateInfo  buf_ci = {};
        buf_ci.sType        = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buf_ci.flags        = 0;
        buf_ci.usage        = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
        buf_ci.size         = VkDeviceSize( _desc.size );
        buf_ci.sharingMode  = VK_SHARING_MODE_EXCLUSIVE;

        auto&   dev = resMngr.GetDevice();
        VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buf_ci, null, OUT &_buffer ));

        _memoryId = resMngr.CreateMemoryObj( _buffer, BufferDesc{}.SetMemory( EMemoryType::DeviceLocal ), RVRef(allocator), dbgName );
        CHECK_ERR( _memoryId );

        VkAccelerationStructureCreateInfoKHR    blas_ci = {};
        blas_ci.sType       = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        blas_ci.createFlags = 0;    // VK_ACCELERATION_STRUCTURE_CREATE_MOTION_BIT_NV
        blas_ci.buffer      = _buffer;
        blas_ci.offset      = 0;
        blas_ci.size        = VkDeviceSize( _desc.size );
        blas_ci.type        = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

        VK_CHECK_ERR( dev.vkCreateAccelerationStructureKHR( dev.GetVkDevice(), &blas_ci, null, OUT &_accelStruct ));

        dev.SetObjectName( _accelStruct, dbgName, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR );

        VkAccelerationStructureDeviceAddressInfoKHR addr_info = {};
        addr_info.sType                 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addr_info.accelerationStructure = _accelStruct;

        _address = BitCast<DeviceAddress>( dev.vkGetAccelerationStructureDeviceAddressKHR( dev.GetVkDevice(), &addr_info ));
        CHECK_ERR( _address != Default );

        DEBUG_ONLY( _debugName = dbgName; )
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VRTGeometry::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        auto&   dev = resMngr.GetDevice();

        if ( _buffer != Default )
            dev.vkDestroyBuffer( dev.GetVkDevice(), _buffer, null );

        if ( _accelStruct != Default )
            dev.vkDestroyAccelerationStructureKHR( dev.GetVkDevice(), _accelStruct, null );

        resMngr.ImmediatelyRelease( INOUT _memoryId );

        _address        = Default;
        _memoryId       = Default;
        _accelStruct    = Default;
        _buffer         = Default;
        _desc           = Default;

        DEBUG_ONLY( _debugName.clear(); )
    }

/*
=================================================
    _Convert
=================================================
*/
    template <bool IsForBuilding>
    bool  VRTGeometry::_Convert (const VResourceManager &resMngr, const RTGeometryBuild &desc, IAllocatorRef allocator,
                                 OUT uint* &outPrimitiveCount, OUT VkAccelerationStructureBuildRangeInfoKHR* &outRanges,
                                 OUT VkAccelerationStructureBuildGeometryInfoKHR &outBuildInfo) __NE___
    {
        auto&       build_info  = outBuildInfo;

        const usize geom_count  = desc.triangles.size() + desc.aabbs.size();
        auto*       pp_geom     = allocator.Allocate< VkAccelerationStructureGeometryKHR *>( geom_count );
        auto*       prim_count  = IsForBuilding ? null : allocator.Allocate< uint >( geom_count );
        auto*       ranges      = IsForBuilding ? allocator.Allocate< VkAccelerationStructureBuildRangeInfoKHR >( geom_count ) : null;
        CHECK_ERR( pp_geom != null and (prim_count != null or ranges != null) );

        build_info              = {};
        build_info.sType        = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        build_info.flags        = VEnumCast( desc.options );
        build_info.type         = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        build_info.ppGeometries = pp_geom;
        build_info.geometryCount= CheckCast<uint>( geom_count );
        outPrimitiveCount       = prim_count;
        outRanges               = ranges;

        CHECK_ERR( desc.triangles.empty()   or desc.triangles.data< RTGeometryBuild::TrianglesInfo >() != null );
        CHECK_ERR( desc.aabbs.empty()       or desc.aabbs.data< RTGeometryBuild::AABBsInfo >() != null );

        if constexpr( IsForBuilding )
        {
            CHECK_ERR( desc.triangles.empty()   or desc.triangles.data< RTGeometryBuild::TrianglesData >() != null );
            CHECK_ERR( desc.aabbs.empty()       or desc.aabbs.data< RTGeometryBuild::AABBsData >() != null );
        }

        for (usize i = 0; i < desc.triangles.size(); ++i)
        {
            *pp_geom = allocator.Allocate< VkAccelerationStructureGeometryKHR >(1);

            auto&   info    = desc.triangles.at< RTGeometryBuild::TrianglesInfo >(i);
            auto&   dst     = **(pp_geom++);
            auto&   tri     = dst.geometry.triangles;

            dst.sType           = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
            dst.pNext           = null;
            dst.geometryType    = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
            dst.flags           = VEnumCast( info.options );

            tri.sType           = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
            tri.pNext           = null;
            tri.vertexFormat    = VEnumCast( info.vertexFormat );
            tri.maxVertex       = info.maxVertex;
            tri.indexType       = info.indexType == Default ? VK_INDEX_TYPE_NONE_KHR : VEnumCast( info.indexType );

            if constexpr( IsForBuilding )
            {
                auto&   data    = desc.triangles.at< RTGeometryBuild::TrianglesData >(i);

                auto*   vb  = resMngr.GetResource( data.vertexData,     False{"don't inc ref"}, True{"quiet"} );
                auto*   ib  = resMngr.GetResource( data.indexData,      False{"don't inc ref"}, True{"quiet"} );
                auto*   tb  = resMngr.GetResource( data.transformData,  False{"don't inc ref"}, True{"quiet"} );
                CHECK_ERR( vb != null );
                CHECK_ERR( (ib != null) == data.indexData.IsValid() );
                CHECK_ERR( (tb != null) == data.transformData.IsValid() );
                CHECK_ERR( (info.indexType != Default) == data.indexData.IsValid() );
                CHECK_ERR( info.allowTransforms == data.transformData.IsValid() );

                tri.vertexData.deviceAddress    = BitCast<VkDeviceAddress>( vb->GetDeviceAddress() + data.vertexDataOffset );
                tri.vertexStride                = VkDeviceSize(data.vertexStride);
                tri.indexData.deviceAddress     = ib != null ? BitCast<VkDeviceAddress>( ib->GetDeviceAddress() + data.indexDataOffset )     : 0;
                tri.transformData.deviceAddress = tb != null ? BitCast<VkDeviceAddress>( tb->GetDeviceAddress() + data.transformDataOffset ) : 0;

                // must be aligned to the size in bytes of the smallest component of the format in vertexFormat
                ASSERT( IsMultipleOf( tri.vertexData.deviceAddress, EVertexType_SizeOf( info.vertexFormat & ~EVertexType::_VecMask )));

                ASSERT( Bytes{data.vertexStride} >= EVertexType_SizeOf( info.vertexFormat ));
                ASSERT( (info.indexType == Default) or IsMultipleOf( tri.indexData.deviceAddress, EIndex_SizeOf( info.indexType )) );
                ASSERT( (not info.allowTransforms) or IsMultipleOf( tri.transformData.deviceAddress, 16 ) );
                ASSERT( vb->Size() >= (data.vertexDataOffset + Bytes{data.vertexStride} * info.maxVertex) );
                ASSERT( (info.indexType == Default) or (ib->Size() >= (data.indexDataOffset + info.maxPrimitives * EIndex_SizeOf( info.indexType ))) );
                ASSERT( (not info.allowTransforms) or (tb->Size() >= (data.transformDataOffset + SizeOf<VkTransformMatrixKHR>)) );

                auto&   range = *(ranges++);
                range.primitiveCount    = info.maxPrimitives;
                range.primitiveOffset   = 0;
                range.firstVertex       = 0;
                range.transformOffset   = 0;
            }
            else
            {
                // address is not used, just test for != 0
                tri.transformData.deviceAddress = info.allowTransforms ? 1 : 0;

                tri.vertexData.deviceAddress    = 0;    // unused
                tri.vertexStride                = 0;    // unused
                tri.indexData.deviceAddress     = 0;    // unused

                *(prim_count++) = info.maxPrimitives;
            }
        }

        for (usize i = 0; i < desc.aabbs.size(); ++i)
        {
            *pp_geom = allocator.Allocate< VkAccelerationStructureGeometryKHR >(1);

            auto&   info    = desc.aabbs.at< RTGeometryBuild::AABBsInfo >(i);
            auto&   dst     = **(pp_geom++);
            auto&   aabb    = dst.geometry.aabbs;

            dst.sType           = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
            dst.pNext           = null;
            dst.geometryType    = VK_GEOMETRY_TYPE_AABBS_KHR;
            dst.flags           = VEnumCast( info.options );

            aabb.sType          = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
            aabb.pNext          = null;

            if constexpr( IsForBuilding )
            {
                auto&   data = desc.aabbs.at< RTGeometryBuild::AABBsData >(i);

                auto*   buf = resMngr.GetResource( data.data, False{"don't inc ref"}, True{"quiet"} );
                CHECK_ERR( buf != null );

                aabb.data.deviceAddress = BitCast<VkDeviceAddress>( buf->GetDeviceAddress() + data.dataOffset );
                aabb.stride             = VkDeviceSize(data.stride);

                ASSERT( IsMultipleOf( aabb.data.deviceAddress, 8 ));
                ASSERT( buf->Size() >= (data.dataOffset + SizeOf<VkAabbPositionsKHR> * info.maxAABBs) );

                auto&   range = *(ranges++);
                range.primitiveCount    = info.maxAABBs;
                range.primitiveOffset   = 0;
                range.firstVertex       = 0;
                range.transformOffset   = 0;
            }
            else
            {
                aabb.data.deviceAddress = 0;    // unused
                aabb.stride             = 0;    // unused

                *(prim_count++) = info.maxAABBs;
            }
        }

        return true;
    }

/*
=================================================
    GetBuildSizes
=================================================
*/
    RTASBuildSizes  VRTGeometry::GetBuildSizes (const VResourceManager &resMngr, const RTGeometryBuild &desc) __NE___
    {
        GRES_CHECK( IsSupported( resMngr, desc ));

        IAllocatorAdaptor< VTempLinearAllocator >       allocator;
        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureBuildSizesInfoKHR        size_info   = {};
        VkAccelerationStructureBuildRangeInfoKHR *      ranges      = null;
        uint *                                          prim_count  = null;

        size_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

        CHECK_ERR( _Convert<false>( resMngr, desc, allocator, OUT prim_count, OUT ranges, OUT build_info ));
        ASSERT( prim_count != null );

        // from specs:
        //  The 'srcAccelerationStructure', 'dstAccelerationStructure', 'mode', 'scratchData' members of pBuildInfo are ignored.

        auto&   dev = resMngr.GetDevice();
        dev.vkGetAccelerationStructureBuildSizesKHR( dev.GetVkDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, prim_count, OUT &size_info );

        RTASBuildSizes  res;
        res.rtasSize            = Bytes{size_info.accelerationStructureSize};
        res.buildScratchSize    = Bytes{size_info.buildScratchSize};
        res.updateScratchSize   = Bytes{size_info.updateScratchSize};
        return res;
    }

/*
=================================================
    ConvertBuildInfo
=================================================
*/
    bool  VRTGeometry::ConvertBuildInfo (const VResourceManager &resMngr, IAllocatorRef allocator, const RTGeometryBuild &desc,
                                         OUT VkAccelerationStructureBuildRangeInfoKHR* &ranges, OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo) __NE___
    {
        GRES_CHECK( IsSupported( resMngr, desc ));

        uint*   prim_count  = null;
        CHECK_ERR( _Convert<true>( resMngr, desc, allocator, OUT prim_count, OUT ranges, OUT buildInfo ));
        ASSERT( ranges != null );
        return true;
    }

    bool  VRTGeometry::ConvertBuildInfo (const VResourceManager &resMngr, IAllocatorRef allocator, const RTGeometryBuild &desc,
                                         OUT uint* &maxPrimitiveCounts, OUT VkAccelerationStructureBuildGeometryInfoKHR &buildInfo) __NE___
    {
        GRES_CHECK( IsSupported( resMngr, desc ));

        VkAccelerationStructureBuildRangeInfoKHR*   ranges = null;
        CHECK_ERR( _Convert<true>( resMngr, desc, allocator, OUT maxPrimitiveCounts, OUT ranges, OUT buildInfo ));
        return true;
    }

/*
=================================================
    IsSerializedMemoryCompatible
=================================================
*/
    bool  VRTGeometry::IsSerializedMemoryCompatible (const VDevice &dev, const void* ptr, Bytes size) __NE___
    {
        CHECK_ERR( size >= sizeof(VK_UUID_SIZE)*2 );

        VkAccelerationStructureVersionInfoKHR   info;
        info.sType          = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_VERSION_INFO_KHR;
        info.pNext          = null;
        info.pVersionData   = Cast<uint8_t>(ptr);

        VkAccelerationStructureCompatibilityKHR compat = VK_ACCELERATION_STRUCTURE_COMPATIBILITY_INCOMPATIBLE_KHR;

        dev.vkGetDeviceAccelerationStructureCompatibilityKHR( dev.GetVkDevice(), &info, OUT &compat );

        return compat == VK_ACCELERATION_STRUCTURE_COMPATIBILITY_COMPATIBLE_KHR;
    }

/*
=================================================
    IsSupported (RTGeometryDesc)
=================================================
*/
    bool  VRTGeometry::IsSupported (const VResourceManager &resMngr, const RTGeometryDesc &desc) __NE___
    {
        if_unlikely( resMngr.GetFeatureSet().accelerationStructure() != EFeature::RequireTrue )
            return false;

        if_unlikely( desc.size == 0 )
            return false;

        // TODO: desc.options

        return true;
    }

/*
=================================================
    IsSupported (RTGeometryBuild)
=================================================
*/
    bool  VRTGeometry::IsSupported (const VResourceManager &resMngr, const RTGeometryBuild &build) __NE___
    {
        auto&       props       = resMngr.GetDevice().GetDeviceProperties().rayTracing;
        const usize geom_count  = build.triangles.size() + build.aabbs.size();
        ulong       max_prims   = 0;

        if_unlikely( geom_count == 0 or geom_count > props.maxGeometries )
            return false;

        FlatHashSet<EVertexType>    vertex_types;

        for (usize i = 0; i < build.triangles.size(); ++i)
        {
            auto&   info = build.triangles.at< RTGeometryBuild::TrianglesInfo >(i);

            max_prims += info.maxPrimitives;
            vertex_types.insert( info.vertexFormat );
        }

        for (usize i = 0; i < build.aabbs.size(); ++i)
        {
            auto&   info = build.aabbs.at< RTGeometryBuild::AABBsInfo >(i);

            max_prims += info.maxAABBs;
        }

        for (auto vt : vertex_types) {
            if_unlikely( not VBuffer::IsSupportedForASVertex( resMngr, vt ))
                return false;
        }

        if_unlikely( max_prims  > props.maxPrimitives   or
                     geom_count > props.maxGeometries )
            return false;

        return true;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
