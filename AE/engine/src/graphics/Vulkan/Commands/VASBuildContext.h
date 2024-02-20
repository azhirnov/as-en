// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    ASBuildCtx -->  DirectASBuildCtx   --> BarrierMngr --> Vulkan device
                \-> IndirectASBuildCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/Resources/VRTScene.h"

namespace AE::Graphics::_hidden_
{

    //
    // Vulkan Direct AS Build Context implementation
    //

    class _VDirectASBuildCtx : public VBaseDirectContext
    {
    // types
    private:
        using Validator_t   = ASBuildContextValidation;


    // methods
    public:
        void  Copy (const VkCopyAccelerationStructureInfoKHR &info)                                 __Th___;
        void  SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)            __Th___;
        void  DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)        __Th___;

        ND_ VkCommandBuffer EndCommandBuffer ()                                                     __Th___;
        ND_ VCommandBuffer  ReleaseCommandBuffer ()                                                 __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VDirectASBuildCtx (const RenderTask &task, VCommandBuffer cmdbuf, DebugLabel dbg)          __Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::ASBuild } {}

        void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst)                                __Th___;
        void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)              __Th___;

        void  _Build  (const RTSceneBuild &cmd, RTSceneID dst)                                      __Th___;
        void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)                       __Th___;

        void  _BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
                              VkDeviceAddress indirectMem, Bytes indirectStride)                    __Th___;
        void  _BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem)  __Th___;

        void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
                      VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);

        void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query);
        void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset,
                              const VQueryManager::Query &query);

        ND_ DeviceProperties::RayTracingProperties const&  _RTProps ()                              C_NE___ { return _GetBarrierMngr().GetDevice().GetDeviceProperties().rayTracing; }
    };



    //
    // Vulkan Indirect AS Build Context implementation
    //

    class _VIndirectASBuildCtx : public VBaseIndirectContext
    {
    // types
    private:
        using Validator_t   = ASBuildContextValidation;


    // methods
    public:
        void  Copy (const VkCopyAccelerationStructureInfoKHR &info)                                 __Th___;
        void  SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)            __Th___;
        void  DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)        __Th___;

        ND_ VBakedCommands      EndCommandBuffer ()                                                 __Th___;
        ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()                                             __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VIndirectASBuildCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)    __Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::ASBuild } {}

        void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst)                                __Th___;
        void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)              __Th___;

        void  _Build  (const RTSceneBuild &cmd, RTSceneID dst)                                      __Th___;
        void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)                       __Th___;

        void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
                      VkAccelerationStructureBuildRangeInfoKHR const* const& ranges)                __Th___;

        void  _BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
                              VkDeviceAddress indirectMem, Bytes indirectStride)                    __Th___;
        void  _BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem)  __Th___;

        void  _BuildIndirect (const VkAccelerationStructureBuildGeometryInfoKHR &info,
                              VkDeviceAddress indirectMem, uint const* maxPrimCount, uint indirectStride)__Th___;

        void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query)     __Th___;
        void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset,
                              const VQueryManager::Query &query)                                    __Th___;

        ND_ DeviceProperties::RayTracingProperties const&  _RTProps ()                              C_NE___ { return _GetBarrierMngr().GetDevice().GetDeviceProperties().rayTracing; }
    };



    //
    // Vulkan AS Build Context implementation
    //

    template <typename CtxImpl>
    class _VASBuildContextImpl : public CtxImpl, public IASBuildContext
    {
    // types
    public:
        using CmdBuf_t      = typename CtxImpl::CmdBuf_t;
    private:
        using RawCtx        = CtxImpl;
        using AccumBar      = VAccumBarriers< _VASBuildContextImpl< CtxImpl >>;
        using DeferredBar   = VAccumDeferredBarriersForCtx< _VASBuildContextImpl< CtxImpl >>;
        using Validator_t   = ASBuildContextValidation;


    // methods
    public:
        explicit _VASBuildContextImpl (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default) __Th___;

        _VASBuildContextImpl ()                                                                                     = delete;
        _VASBuildContextImpl (const _VASBuildContextImpl &)                                                         = delete;

        using RawCtx::Copy;

        void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)                                                 __Th_OV { RawCtx::_Build( cmd, dst ); }
        void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)                               __Th_OV { RawCtx::_Update( cmd, src, dst ); }
        void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)                __Th_OV;

        void  Build  (const RTSceneBuild &cmd, RTSceneID dst)                                                       __Th_OV { RawCtx::_Build( cmd, dst ); }
        void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)                                        __Th_OV { RawCtx::_Update( cmd, src, dst ); }
        void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)                      __Th_OV;

        void  SerializeToMemory (RTGeometryID src, DeviceAddress dst)                                               __Th___;
        void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)                                   __Th___;

        void  SerializeToMemory (RTSceneID src, DeviceAddress dst)                                                  __Th___;
        void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)                                      __Th___;

        void  DeserializeFromMemory (DeviceAddress src, RTGeometryID dst)                                           __Th___;
        void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)                               __Th___;

        void  DeserializeFromMemory (DeviceAddress src, RTSceneID dst)                                              __Th___;
        void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)                                  __Th___;

        void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size) __Th_OV { return _WriteProperty( property, as, dstBuffer, offset, size ); }
        void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)    __Th_OV { return _WriteProperty( property, as, dstBuffer, offset, size ); }

        Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)                                      __Th_OV { return _ReadProperty( property, as ); }
        Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)                                         __Th_OV { return _ReadProperty( property, as ); }

        void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer,
                             Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)                                 __Th___;
        void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
                             BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b,
                             Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)                                 __Th___;

        void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer)                  __Th___;
        void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst,
                             BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b)                             __Th___;

        VBARRIERMNGR_INHERIT_BARRIERS

    private:
        template <typename ASType>
        void  _WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size)      __Th___;

        template <typename ASType>
        ND_ Promise<Bytes>  _ReadProperty (ERTASProperty property, ASType as)                                       __Th___;

        ND_ VQueryManager::Query  _ASQueryOrThrow (ERTASProperty property)                                          C_Th___;
    };

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
    using VDirectASBuildContext     = Graphics::_hidden_::_VASBuildContextImpl< Graphics::_hidden_::_VDirectASBuildCtx >;
    using VIndirectASBuildContext   = Graphics::_hidden_::_VASBuildContextImpl< Graphics::_hidden_::_VIndirectASBuildCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{
/*
=================================================
    Copy***
=================================================
*/
    inline void  _VDirectASBuildCtx::Copy (const VkCopyAccelerationStructureInfoKHR &info) __Th___
    {
        vkCmdCopyAccelerationStructureKHR( _cmdbuf.Get(), &info );
    }

    inline void  _VDirectASBuildCtx::SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info) __Th___
    {
        GCTX_CHECK( info.dst.deviceAddress != Default );
        GCTX_CHECK( IsMultipleOf( info.dst.deviceAddress, 256 ));

        vkCmdCopyAccelerationStructureToMemoryKHR( _cmdbuf.Get(), &info );
    }

    inline void  _VDirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info) __Th___
    {
        GCTX_CHECK( info.src.deviceAddress != Default );
        GCTX_CHECK( IsMultipleOf( info.src.deviceAddress, 256 ));

        vkCmdCopyMemoryToAccelerationStructureKHR( _cmdbuf.Get(), &info );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    template <typename C>
    _VASBuildContextImpl<C>::_VASBuildContextImpl (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
        RawCtx{ task, RVRef(cmdbuf), dbg }
    {
        Validator_t::CtxInit( task.GetQueueMask() );
    }

/*
=================================================
    Copy
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode) __Th___
    {
        auto  [src_geom, dst_geom] = _GetResourcesOrThrow( src, dst );

        VALIDATE_GCTX( Copy( src_geom.Description(), dst_geom.Description(), mode ));

        VkCopyAccelerationStructureInfoKHR  info;
        info.sType  = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
        info.pNext  = null;
        info.src    = src_geom.Handle();
        info.dst    = dst_geom.Handle();
        info.mode   = VEnumCast( mode );

        RawCtx::Copy( info );
    }

/*
=================================================
    Copy
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode) __Th___
    {
        auto  [src_scene, dst_scene] = _GetResourcesOrThrow( src, dst );

        VALIDATE_GCTX( Copy( src_scene.Description(), dst_scene.Description(), mode ));

        VkCopyAccelerationStructureInfoKHR  info;
        info.sType  = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
        info.pNext  = null;
        info.src    = src_scene.Handle();
        info.dst    = dst_scene.Handle();
        info.mode   = VEnumCast( mode );

        RawCtx::Copy( info );
    }

/*
=================================================
    _ASQueryOrThrow
=================================================
*/
    template <typename C>
    VQueryManager::Query  _VASBuildContextImpl<C>::_ASQueryOrThrow (ERTASProperty property) C_Th___
    {
        EQueryType  q_type = Default;

        switch_enum( property )
        {
            case ERTASProperty::CompactedSize :     q_type = EQueryType::AccelStructCompactedSize;      break;
            case ERTASProperty::SerializationSize : q_type = EQueryType::AccelStructSerializationSize;  break;
            case ERTASProperty::Size :              q_type = EQueryType::AccelStructSize;               break;
            default_unlikely :                      CHECK_THROW( false ); // unknown ERTASProperty
        }
        switch_end

        auto&   qm      = this->_mngr.GetQueryManager();
        auto    query   = qm.AllocQuery( this->_mngr.GetQueueType(), q_type );
        CHECK_THROW( query );

        return query;
    }

/*
=================================================
    _WriteProperty
----
    TODO: optimize barriers:
        - write property to query
        - query wait result
        - copy from query to buffer
=================================================
*/
    template <typename C>
    template <typename ASType>
    void  _VASBuildContextImpl<C>::_WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___
    {
        auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );
        auto  query             = _ASQueryOrThrow( property );

        VALIDATE_GCTX( WriteProperty( property, dst_buf.Description(), offset, size ));
        Unused( size );

        RawCtx::_WriteProperty( src_as.Handle(), dst_buf.Handle(), offset, query );
    }

/*
=================================================
    _ReadProperty
=================================================
*/
    template <typename C>
    template <typename ASType>
    Promise<Bytes>  _VASBuildContextImpl<C>::_ReadProperty (ERTASProperty property, ASType as) __Th___
    {
        VALIDATE_GCTX( ReadProperty( property ));

        auto&   src_as  = _GetResourcesOrThrow( as );
        auto    query   = _ASQueryOrThrow( property );

        RawCtx::_WriteProperty( src_as.Handle(), query );

        return Threading::MakePromise(  [query] () -> Threading::PromiseResult<Bytes>
                                        {
                                            auto&   query_mngr  = GraphicsScheduler().GetQueryManager();
                                            Bytes   size;
                                            CHECK_PE( query_mngr.GetRTASProperty( query, OUT &size, Sizeof(size) ));
                                            return size;
                                        },
                                        Tuple{ this->_mngr.GetBatchRC() },
                                        "VASBuildContext::ReadProperty",
                                        ETaskQueue::PerFrame
                                     );
    }

/*
=================================================
    SerializeToMemory
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, DeviceAddress dst) __Th___
    {
        auto&   src_as = _GetResourcesOrThrow( src );
        VALIDATE_GCTX( SerializeToMemory( src_as.Description(), dst ));

        VkCopyAccelerationStructureToMemoryInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
        info.pNext              = null;
        info.src                = src_as.Handle();
        info.dst.deviceAddress  = VkDeviceAddress(dst);
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;

        return RawCtx::SerializeToMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset) __Th___
    {
        auto&   dst_buf = _GetResourcesOrThrow( dst );
        VALIDATE_GCTX( SerializeToMemory( dst_buf.Description(), dstOffset ));

        return SerializeToMemory( src, BitCast<VkDeviceAddress>( dst_buf.GetDeviceAddress() + dstOffset ));
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, DeviceAddress dst) __Th___
    {
        auto&   src_as = _GetResourcesOrThrow( src );
        VALIDATE_GCTX( SerializeToMemory( src_as.Description(), dst ));

        VkCopyAccelerationStructureToMemoryInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
        info.pNext              = null;
        info.src                = src_as.Handle();
        info.dst.deviceAddress  = VkDeviceAddress(dst);
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;

        return RawCtx::SerializeToMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset) __Th___
    {
        auto&   dst_buf = _GetResourcesOrThrow( dst );
        VALIDATE_GCTX( SerializeToMemory( dst_buf.Description(), dstOffset ));

        return SerializeToMemory( src, BitCast<VkDeviceAddress>( dst_buf.GetDeviceAddress() + dstOffset ));
    }

/*
=================================================
    DeserializeFromMemory
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (DeviceAddress src, RTGeometryID dst) __Th___
    {
        auto&   dst_as = _GetResourcesOrThrow( dst );
        VALIDATE_GCTX( DeserializeFromMemory( src, dst_as ));

        VkCopyMemoryToAccelerationStructureInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
        info.pNext              = null;
        info.src.deviceAddress  = VkDeviceAddress(src);
        info.dst                = dst_as->Handle();
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;

        return RawCtx::DeserializeFromMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst) __Th___
    {
        auto&   src_buf = _GetResourcesOrThrow( src );
        VALIDATE_GCTX( DeserializeFromMemory( src_buf.Description(), srcOffset ));

        return DeserializeFromMemory( BitCast<VkDeviceAddress>( src_buf.GetDeviceAddress() + srcOffset ), dst );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (DeviceAddress src, RTSceneID dst) __Th___
    {
        auto&   dst_as = _GetResourcesOrThrow( dst );
        VALIDATE_GCTX( DeserializeFromMemory( src, dst_as.Description() ));

        VkCopyMemoryToAccelerationStructureInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
        info.pNext              = null;
        info.src.deviceAddress  = VkDeviceAddress(src);
        info.dst                = dst_as.Handle();
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;

        return RawCtx::DeserializeFromMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst) __Th___
    {
        auto&   src_buf = _GetResourcesOrThrow( src );
        VALIDATE_GCTX( DeserializeFromMemory( src_buf.Description(), srcOffset ));

        return DeserializeFromMemory( BitCast<VkDeviceAddress>( src_buf.GetDeviceAddress() + srcOffset ), dst );
    }

/*
=================================================
    BuildIndirect
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer, Bytes indirectStride) __Th___
    {
        return RawCtx::_BuildIndirect( cmd, dst, indirectBuffer, indirectStride );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, BufferID indirectBuffer,
                                                  Bytes indirectBufferOffset, Bytes indirectStride) __Th___
    {
        auto&   ibuf = _GetResourcesOrThrow( indirectBuffer );
        VALIDATE_GCTX( BuildIndirect( cmd, ibuf.Description(), indirectBufferOffset, indirectStride ));
        return RawCtx::_BuildIndirect( cmd, dst, BitCast<VkDeviceAddress>( ibuf.GetDeviceAddress() + indirectBufferOffset ), indirectStride );
    }

/*
=================================================
    BuildIndirect
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer) __Th___
    {
        return RawCtx::_BuildIndirect( cmd, dst, indirectBuffer );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
    {
        auto&   ibuf = _GetResourcesOrThrow( indirectBuffer );
        VALIDATE_GCTX( BuildIndirect( cmd, ibuf.Description(), indirectBufferOffset ));
        return RawCtx::_BuildIndirect( cmd, dst, BitCast<VkDeviceAddress>( ibuf.GetDeviceAddress() + indirectBufferOffset ));
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
