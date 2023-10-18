// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VASBuildContext.h"

namespace AE::Graphics::_hidden_
{
/*
=================================================
    _Build
=================================================
*/
    void  _VDirectASBuildCtx::_Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges) __Th___
    {
        DEBUG_ONLY(
        switch ( info.mode )
        {
            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR :
                ASSERT( info.dstAccelerationStructure != Default );
                break;
            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :
                ASSERT( info.srcAccelerationStructure != Default );
                ASSERT( info.dstAccelerationStructure != Default );
                break;

            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR :
            default_unlikely :
                DBG_WARNING( "unknown build mode" );
                break;
        })

        vkCmdBuildAccelerationStructuresKHR( _cmdbuf.Get(), 1, &info, &ranges );
    }

/*
=================================================
    _Build
=================================================
*/
    void  _VDirectASBuildCtx::_Build (const RTGeometryBuild &cmd, RTGeometryID dst) __Th___
    {
        auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

        VALIDATE_GCTX( Build( geom.Description(), scratch_buf.Description(), cmd.scratch.offset ));

        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureBuildRangeInfoKHR*       ranges      = null;
        AllocatorImpl2< VTempLinearAllocator >          allocator;

        CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

        build_info.mode                         = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure     = geom.Handle();
        build_info.scratchData.deviceAddress    = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

        _Build( build_info, ranges );
    }

/*
=================================================
    _Update
=================================================
*/
    void  _VDirectASBuildCtx::_Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) __Th___
    {
        auto  [src_geom, dst_geom, scratch_buf] = _GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id );

        VALIDATE_GCTX( Update( src_geom.Description(), dst_geom.Description(), scratch_buf.Description(), cmd.scratch.offset ));

        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureBuildRangeInfoKHR*       ranges      = null;
        AllocatorImpl2< VTempLinearAllocator >          allocator;

        CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

        build_info.mode                         = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
        build_info.srcAccelerationStructure     = src_geom.Handle();
        build_info.dstAccelerationStructure     = dst_geom.Handle();
        build_info.scratchData.deviceAddress    = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

        _Build( build_info, ranges );
    }

/*
=================================================
    _BuildIndirect
=================================================
*/
    void  _VDirectASBuildCtx::_BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, VkDeviceAddress indirectMem, Bytes indirectStride) __Th___
    {
        auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

        ASSERT( indirectMem != Default );
        VALIDATE_GCTX( BuildIndirect( geom.Description(),
                                      scratch_buf.Description(), cmd.scratch.offset,
                                      indirectStride ));

        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        AllocatorImpl2< VTempLinearAllocator >          allocator;
        uint*                                           max_prim_count  = null;
        const uint                                      stride          = uint(indirectStride);

        CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT max_prim_count, OUT build_info ));

        build_info.mode                         = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure     = geom.Handle();
        build_info.scratchData.deviceAddress    = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

        vkCmdBuildAccelerationStructuresIndirectKHR( _cmdbuf.Get(), 1, &build_info, &indirectMem, &stride, &max_prim_count );
    }

/*
=================================================
    _Build
=================================================
*/
    void  _VDirectASBuildCtx::_Build (const RTSceneBuild &cmd, RTSceneID dst) __Th___
    {
        auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

        VALIDATE_GCTX( Build( scene.Description(),
                              scratch_buf.Description(), cmd.scratch.offset,
                              inst_buf.Description(), cmd.instanceData.offset ));

        VkAccelerationStructureBuildGeometryInfoKHR build_info;
        VkAccelerationStructureGeometryKHR          geom;
        VkAccelerationStructureBuildRangeInfoKHR    range;
        VkAccelerationStructureBuildRangeInfoKHR*   p_ranges    = &range;

        CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));

        build_info.mode                             = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure         = scene.Handle();
        build_info.scratchData.deviceAddress        = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        geom.geometry.instances.data.deviceAddress  = BitCast<VkDeviceAddress>(  inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
        ASSERT( IsMultipleOf( geom.geometry.instances.data.deviceAddress, _RTProps().instanceDataAlign ));

        _Build( build_info, p_ranges );
    }

/*
=================================================
    _Update
=================================================
*/
    void  _VDirectASBuildCtx::_Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) __Th___
    {
        auto  [src_scene, dst_scene, scratch_buf, inst_buf] =
                    _GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id, cmd.instanceData.id );

        VALIDATE_GCTX( Update( src_scene.Description(), dst_scene.Description(),
                               scratch_buf.Description(), cmd.scratch.offset,
                               inst_buf.Description(), cmd.instanceData.offset ));

        VkAccelerationStructureBuildGeometryInfoKHR build_info;
        VkAccelerationStructureGeometryKHR          geom;
        VkAccelerationStructureBuildRangeInfoKHR    range;
        VkAccelerationStructureBuildRangeInfoKHR*   p_ranges    = &range;

        CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));

        build_info.mode                             = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
        build_info.srcAccelerationStructure         = src_scene.Handle();
        build_info.dstAccelerationStructure         = dst_scene.Handle();
        build_info.scratchData.deviceAddress        = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        geom.geometry.instances.data.deviceAddress  = BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
        ASSERT( IsMultipleOf( geom.geometry.instances.data.deviceAddress, _RTProps().instanceDataAlign ));

        _Build( build_info, p_ranges );
    }

/*
=================================================
    _BuildIndirect
=================================================
*/
    void  _VDirectASBuildCtx::_BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem) __Th___
    {
        auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

        ASSERT( indirectMem != Default );
        VALIDATE_GCTX( BuildIndirect( scene.Description(),
                                      scratch_buf.Description(), cmd.scratch.offset,
                                      inst_buf.Description(), cmd.instanceData.offset ));

        VkAccelerationStructureBuildGeometryInfoKHR build_info;
        VkAccelerationStructureGeometryKHR          geom;
        VkAccelerationStructureBuildRangeInfoKHR    range;
        uint const*                                 max_inst_count  = &cmd.maxInstanceCount;
        const uint                                  stride          = sizeof(VkAccelerationStructureBuildRangeInfoKHR);

        CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));

        build_info.mode                             = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure         = scene.Handle();
        build_info.scratchData.deviceAddress        = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        geom.geometry.instances.data.deviceAddress  = BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
        ASSERT( IsMultipleOf( geom.geometry.instances.data.deviceAddress, _RTProps().instanceDataAlign ));
        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

        vkCmdBuildAccelerationStructuresIndirectKHR( _cmdbuf.Get(), 1, &build_info, &indirectMem, &stride, &max_inst_count );
    }

/*
=================================================
    _WriteProperty
=================================================
*/
    void  _VDirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, const VQueryManager::Query &query) __Th___
    {
        constexpr VkDeviceSize  stride = 8;

        _WriteProperty( as, query );

        // If VK_QUERY_RESULT_WAIT_BIT is set, this command defines an execution dependency with any earlier commands that writes one of the identified queries.
        vkCmdCopyQueryPoolResults( _cmdbuf.Get(), query.pool, query.first, 1, dstBuffer, VkDeviceSize(offset), stride, VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT );
    }

/*
=================================================
    _WriteProperty
=================================================
*/
    void  _VDirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query) __Th___
    {
        GCTX_CHECK( query );
        vkCmdWriteAccelerationStructuresPropertiesKHR( _cmdbuf.Get(), 1, &as, VEnumCast( query.type ), query.pool, query.first );
    }

/*
=================================================
    EndCommandBuffer
=================================================
*/
    VkCommandBuffer  _VDirectASBuildCtx::EndCommandBuffer () __Th___
    {
        ASSERT( _NoPendingBarriers() );
        DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::ASBuild ); )

        return VBaseDirectContext::_EndCommandBuffer();  // throw
    }

/*
=================================================
    ReleaseCommandBuffer
=================================================
*/
    VCommandBuffer  _VDirectASBuildCtx::ReleaseCommandBuffer () __Th___
    {
        ASSERT( _NoPendingBarriers() );
        DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::ASBuild ); )

        return VBaseDirectContext::_ReleaseCommandBuffer();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _Build
=================================================
*/
    void  _VIndirectASBuildCtx::_Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
                                        VkAccelerationStructureBuildRangeInfoKHR const* const& srcRanges) __Th___
    {
        ASSERT( info.sType == VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR );
        ASSERT( info.pNext == null );
        ASSERT( srcRanges != null );

        DEBUG_ONLY(
        switch ( info.mode )
        {
            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR :
                ASSERT( info.dstAccelerationStructure != Default );
                break;

            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :
                ASSERT( info.srcAccelerationStructure != Default );
                ASSERT( info.dstAccelerationStructure != Default );
                break;

            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR :
            default_unlikely :
                DBG_WARNING( "unknown build mode" );
                break;
        })

        auto&   cmd     = _cmdbuf->CreateCmd< BuildASCmd >();   // throw
        cmd.info        = info;
        cmd.pRangeInfos = srcRanges;
    }

/*
=================================================
    _Build
=================================================
*/
    void  _VIndirectASBuildCtx::_Build (const RTGeometryBuild &cmd, RTGeometryID dst) __Th___
    {
        auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

        VALIDATE_GCTX( Build( geom.Description(), scratch_buf.Description(), cmd.scratch.offset ));

        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureBuildRangeInfoKHR*       ranges      = null;
        AllocatorImpl< GraphicsFrameAllocatorRef >      allocator   {_GetFrameId()};

        CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

        build_info.mode                         = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure     = geom.Handle();
        build_info.scratchData.deviceAddress    = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

        _Build( build_info, ranges );
    }

/*
=================================================
    _Update
=================================================
*/
    void  _VIndirectASBuildCtx::_Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) __Th___
    {
        auto  [src_geom, dst_geom, scratch_buf] = _GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id );

        VALIDATE_GCTX( Update( src_geom.Description(), dst_geom.Description(),
                               scratch_buf.Description(), cmd.scratch.offset ));

        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureBuildRangeInfoKHR*       ranges      = null;
        AllocatorImpl< GraphicsFrameAllocatorRef >      allocator   {_GetFrameId()};

        CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

        build_info.mode                         = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
        build_info.srcAccelerationStructure     = src_geom.Handle();
        build_info.dstAccelerationStructure     = dst_geom.Handle();
        build_info.scratchData.deviceAddress    = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

        _Build( build_info, ranges );
    }

/*
=================================================
    _Build
=================================================
*/
    void  _VIndirectASBuildCtx::_Build (const RTSceneBuild &cmd, RTSceneID dst) __Th___
    {
        auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

        VALIDATE_GCTX( Build( scene.Description(),
                              scratch_buf.Description(), cmd.scratch.offset,
                              inst_buf.Description(), cmd.instanceData.offset ));

        AllocatorImpl< GraphicsFrameAllocatorRef >      allocator   {_GetFrameId()};
        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureGeometryKHR*             geom        = allocator.Allocate< VkAccelerationStructureGeometryKHR >();
        VkAccelerationStructureBuildRangeInfoKHR*       range       = allocator.Allocate< VkAccelerationStructureBuildRangeInfoKHR >();

        CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT *geom, OUT *range, OUT build_info ));

        build_info.mode                             = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure         = scene.Handle();
        build_info.scratchData.deviceAddress        = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        geom->geometry.instances.data.deviceAddress = BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
        ASSERT( IsMultipleOf( geom->geometry.instances.data.deviceAddress, _RTProps().instanceDataAlign ));

        _Build( build_info, range );
    }

/*
=================================================
    _Update
=================================================
*/
    void  _VIndirectASBuildCtx::_Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) __Th___
    {
        auto  [src_scene, dst_scene, scratch_buf, inst_buf] =
                    _GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id, cmd.instanceData.id );

        VALIDATE_GCTX( Update( src_scene.Description(), dst_scene.Description(),
                               scratch_buf.Description(), cmd.scratch.offset,
                               inst_buf.Description(), cmd.instanceData.offset ));

        AllocatorImpl< GraphicsFrameAllocatorRef >      allocator   {_GetFrameId()};
        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureGeometryKHR*             geom        = allocator.Allocate< VkAccelerationStructureGeometryKHR >();
        VkAccelerationStructureBuildRangeInfoKHR*       range       = allocator.Allocate< VkAccelerationStructureBuildRangeInfoKHR >();

        CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT *geom, OUT *range, OUT build_info ));

        build_info.mode                             = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
        build_info.srcAccelerationStructure         = src_scene.Handle();
        build_info.dstAccelerationStructure         = dst_scene.Handle();
        build_info.scratchData.deviceAddress        = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        geom->geometry.instances.data.deviceAddress = BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
        ASSERT( IsMultipleOf( geom->geometry.instances.data.deviceAddress, _RTProps().instanceDataAlign ));

        _Build( build_info, range );
    }

/*
=================================================
    _BuildIndirect
=================================================
*/
    void  _VIndirectASBuildCtx::_BuildIndirect (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkDeviceAddress indirectMem,
                                                const uint* maxPrimCount, uint indirectStride) __Th___
    {
        ASSERT( info.sType == VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR );
        ASSERT( info.pNext == null );
        ASSERT( maxPrimCount != null );
        ASSERT( indirectMem != Default );

        DEBUG_ONLY(
        switch ( info.mode )
        {
            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR :
                ASSERT( info.dstAccelerationStructure != Default );
                break;

            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :
                ASSERT( info.srcAccelerationStructure != Default );
                ASSERT( info.dstAccelerationStructure != Default );
                break;

            case VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR :
            default_unlikely :
                DBG_WARNING( "unknown build mode" );
                break;
        })

        auto&   cmd         = _cmdbuf->CreateCmd< BuildASIndirectCmd >();   // throw
        cmd.info            = info;
        cmd.indirectMem     = indirectMem;
        cmd.maxPrimCount    = maxPrimCount;
        cmd.indirectStride  = indirectStride;
    }

/*
=================================================
    _BuildIndirect
=================================================
*/
    void  _VIndirectASBuildCtx::_BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, VkDeviceAddress indirectMem, Bytes indirectStride) __Th___
    {
        auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

        VALIDATE_GCTX( BuildIndirect( geom.Description(),
                                      scratch_buf.Description(), cmd.scratch.offset,
                                      indirectStride ));

        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        AllocatorImpl< GraphicsFrameAllocatorRef >      allocator       {_GetFrameId()};
        uint*                                           max_prim_count  = null;

        CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT max_prim_count, OUT build_info ));

        build_info.mode                         = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure     = geom.Handle();
        build_info.scratchData.deviceAddress    = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

        _BuildIndirect( build_info, indirectMem, max_prim_count, uint(indirectStride) );
    }

/*
=================================================
    _BuildIndirect
=================================================
*/
    void  _VIndirectASBuildCtx::_BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem) __Th___
    {
        auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

        VALIDATE_GCTX( BuildIndirect( scene.Description(),
                                      scratch_buf.Description(), cmd.scratch.offset,
                                      inst_buf.Description(), cmd.instanceData.offset ));

        AllocatorImpl< GraphicsFrameAllocatorRef >      allocator       {_GetFrameId()};
        VkAccelerationStructureBuildGeometryInfoKHR     build_info;
        VkAccelerationStructureGeometryKHR*             geom            = allocator.Allocate< VkAccelerationStructureGeometryKHR >();
        VkAccelerationStructureBuildRangeInfoKHR        range;
        uint const*                                     max_inst_count  = &cmd.maxInstanceCount;

        CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT *geom, OUT range, OUT build_info ));

        build_info.mode                             = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        build_info.dstAccelerationStructure         = scene.Handle();
        build_info.scratchData.deviceAddress        = BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

        geom->geometry.instances.data.deviceAddress = BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

        ASSERT( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
        ASSERT( IsMultipleOf( geom->geometry.instances.data.deviceAddress, _RTProps().instanceDataAlign ));

        _BuildIndirect( build_info, indirectMem, max_inst_count, sizeof(VkAccelerationStructureBuildRangeInfoKHR) );
    }

/*
=================================================
    Copy
=================================================
*/
    void  _VIndirectASBuildCtx::Copy (const VkCopyAccelerationStructureInfoKHR &info) __Th___
    {
        ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR );
        ASSERT( info.pNext == null );

        auto&   cmd = _cmdbuf->CreateCmd< CopyASCmd >();    // throw
        cmd.src     = info.src;
        cmd.dst     = info.dst;
        cmd.mode    = info.mode;
    }

/*
=================================================
    _WriteProperty
=================================================
*/
    void  _VIndirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, const VQueryManager::Query &query) __Th___
    {
        constexpr VkDeviceSize  stride = 8;

        _WriteProperty( as, query );

        auto&   cmd     = _cmdbuf->CreateCmd< CopyQueryPoolResultsCmd >();  // throw
        cmd.srcPool     = query.pool;
        cmd.srcIndex    = query.first;
        cmd.srcCount    = 1;
        cmd.dstBuffer   = dstBuffer;
        cmd.dstOffset   = offset;
        cmd.stride      = Bytes{stride};
        cmd.flags       = VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT;
    }

/*
=================================================
    _WriteProperty
=================================================
*/
    void  _VIndirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query) __Th___
    {
        ASSERT( query );

        auto&   cmd = _cmdbuf->CreateCmd< WriteASPropertiesCmd >(); // throw
        cmd.as      = as;
        cmd.pool    = query.pool;
        cmd.index   = query.first;
        cmd.type    = VEnumCast( query.type );
    }

/*
=================================================
    SerializeToMemory / DeserializeFromMemory
=================================================
*/
    void  _VIndirectASBuildCtx::SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info) __Th___
    {
        ASSERT( info.pNext == null );
        ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR );
        ASSERT( info.dst.deviceAddress != Default );

        auto&   cmd = _cmdbuf->CreateCmd< CopyASToMemoryCmd >();    // throw

        cmd.src     = info.src;
        cmd.dst     = info.dst.deviceAddress;
        cmd.mode    = info.mode;
    }

    void  _VIndirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info) __Th___
    {
        ASSERT( info.pNext == null );
        ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR );
        ASSERT( info.src.deviceAddress != Default );

        auto&   cmd = _cmdbuf->CreateCmd< CopyMemoryToASCmd >();    // throw

        cmd.src     = info.src.deviceAddress;
        cmd.dst     = info.dst;
        cmd.mode    = info.mode;
    }

/*
=================================================
    EndCommandBuffer
=================================================
*/
    VBakedCommands  _VIndirectASBuildCtx::EndCommandBuffer () __Th___
    {
        ASSERT( _NoPendingBarriers() );
        DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::ASBuild ); )

        return VBaseIndirectContext::_EndCommandBuffer();  // throw
    }

/*
=================================================
    ReleaseCommandBuffer
=================================================
*/
    VSoftwareCmdBufPtr  _VIndirectASBuildCtx::ReleaseCommandBuffer () __Th___
    {
        ASSERT( _NoPendingBarriers() );
        DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::ASBuild ); )

        return VBaseIndirectContext::_ReleaseCommandBuffer();
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
