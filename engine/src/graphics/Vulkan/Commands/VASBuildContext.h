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

        void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
        void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

        void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
        void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);

        void  _BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
                              VkDeviceAddress indirectMem, Bytes indirectStride);
        void  _BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem);

        void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
                      VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);

        void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query);
        void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset,
                              Bytes size, const VQueryManager::Query &query);

        ND_ DeviceProperties::RayTracingProperties const&  _RTProps ()                              C_NE___ { return _GetBarrierMngr().GetDevice().GetDeviceProperties().rayTracing; }

        ND_ auto&   _GetASFeats ()                                                                  C_NE___ { return _mngr.GetDevice().GetProperties().accelerationStructureFeats; }
    };



    //
    // Vulkan Indirect AS Build Context implementation
    //

    class _VIndirectASBuildCtx : public VBaseIndirectContext
    {
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

        void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
        void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

        void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
        void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);

        void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
                      VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);

        void  _BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
                              VkDeviceAddress indirectMem, Bytes indirectStride);
        void  _BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem);

        void  _BuildIndirect (const VkAccelerationStructureBuildGeometryInfoKHR &info,
                              VkDeviceAddress indirectMem, uint const* maxPrimCount, uint indirectStride);

        void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query);
        void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset,
                              Bytes size, const VQueryManager::Query &query);

        ND_ DeviceProperties::RayTracingProperties const&  _RTProps ()                              C_NE___ { return _GetBarrierMngr().GetDevice().GetDeviceProperties().rayTracing; }

        ND_ auto&   _GetASFeats ()                                                                  C_NE___ { return _mngr.GetDevice().GetProperties().accelerationStructureFeats; }
    };



    //
    // Vulkan AS Build Context implementation
    //

    template <typename CtxImpl>
    class _VASBuildContextImpl : public CtxImpl, public IASBuildContext
    {
    // types
    public:
        static constexpr bool   IsASBuildContext        = true;
        static constexpr bool   IsVulkanASBuildContext  = true;

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
        void  _WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size);

        template <typename ASType>
        ND_ Promise<Bytes>  _ReadProperty (ERTASProperty property, ASType as);

        ND_ VQueryManager::Query  _ASQueryOrThrow (ERTASProperty property) const;

        using RawCtx::_RTProps;
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
    inline void  _VDirectASBuildCtx::Copy (const VkCopyAccelerationStructureInfoKHR &info)
    {
        vkCmdCopyAccelerationStructureKHR( _cmdbuf.Get(), &info );
    }

    inline void  _VDirectASBuildCtx::SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)
    {
        ASSERT( info.dst.deviceAddress != Default );
        vkCmdCopyAccelerationStructureToMemoryKHR( _cmdbuf.Get(), &info );
    }

    inline void  _VDirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)
    {
        ASSERT( info.src.deviceAddress != Default );
        vkCmdCopyMemoryToAccelerationStructureKHR( _cmdbuf.Get(), &info );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    template <typename C>
    _VASBuildContextImpl<C>::_VASBuildContextImpl (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) :
        RawCtx{ task, RVRef(cmdbuf), dbg }
    {
        CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, task.GetQueueMask() ));
    }

/*
=================================================
    Copy
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode)
    {
        auto  [src_geom, dst_geom] = _GetResourcesOrThrow( src, dst );

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
    void  _VASBuildContextImpl<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode)
    {
        auto  [src_scene, dst_scene] = _GetResourcesOrThrow( src, dst );

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
    VQueryManager::Query  _VASBuildContextImpl<C>::_ASQueryOrThrow (ERTASProperty property) const
    {
        EQueryType  q_type = Default;

        BEGIN_ENUM_CHECKS();
        switch ( property )
        {
            case ERTASProperty::CompactedSize :     q_type = EQueryType::AccelStructCompactedSize;      break;
            case ERTASProperty::SerializationSize : q_type = EQueryType::AccelStructSerializationSize;  break;
            case ERTASProperty::Size :              CHECK_THROW( this->_GetExtensions().rayTracingMaintenance1 );  q_type = EQueryType::AccelStructSize;  break;
            default :                               CHECK_THROW( false ); // unknown ERTASProperty
        }
        END_ENUM_CHECKS();

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
    void  _VASBuildContextImpl<C>::_WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size)
    {
        auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );
        auto  query             = _ASQueryOrThrow( property );

        RawCtx::_WriteProperty( src_as.Handle(), dst_buf.Handle(), offset, size, query );
    }

/*
=================================================
    _ReadProperty
=================================================
*/
    template <typename C>
    template <typename ASType>
    Promise<Bytes>  _VASBuildContextImpl<C>::_ReadProperty (ERTASProperty property, ASType as)
    {
        auto&   src_as  = _GetResourcesOrThrow( as );
        auto    query   = _ASQueryOrThrow( property );

        RawCtx::_WriteProperty( src_as.Handle(), query );

        return Threading::MakePromise(  [query] () -> Threading::PromiseResult<Bytes>
                                        {
                                            auto&   query_mngr  = RenderTaskScheduler().GetQueryManager();
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
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, DeviceAddress dst)
    {
        auto&   src_as = _GetResourcesOrThrow( src );

        VkCopyAccelerationStructureToMemoryInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
        info.pNext              = null;
        info.src                = src_as.Handle();
        info.dst.deviceAddress  = VkDeviceAddress(dst);
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;

        return RawCtx::SerializeToMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)
    {
        auto&   dst_buf = _GetResourcesOrThrow( dst );
        Validator_t::SerializeToMemory( dst_buf, dstOffset );

        return SerializeToMemory( src, BitCast<VkDeviceAddress>( dst_buf.GetDeviceAddress() + dstOffset ));
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, DeviceAddress dst)
    {
        auto&   src_as = _GetResourcesOrThrow( src );

        VkCopyAccelerationStructureToMemoryInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
        info.pNext              = null;
        info.src                = src_as.Handle();
        info.dst.deviceAddress  = VkDeviceAddress(dst);
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;

        return RawCtx::SerializeToMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)
    {
        auto&   dst_buf = _GetResourcesOrThrow( dst );
        Validator_t::SerializeToMemory( dst_buf, dstOffset );

        return SerializeToMemory( src, BitCast<VkDeviceAddress>( dst_buf.GetDeviceAddress() + dstOffset ));
    }

/*
=================================================
    DeserializeFromMemory
=================================================
*/
    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (DeviceAddress src, RTGeometryID dst)
    {
        auto&   dst_as = _GetResourcesOrThrow( dst );

        VkCopyMemoryToAccelerationStructureInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
        info.pNext              = null;
        info.src.deviceAddress  = VkDeviceAddress(src);
        info.dst                = dst_as->Handle();
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;

        return RawCtx::DeserializeFromMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)
    {
        auto&   src_buf = _GetResourcesOrThrow( src );
        Validator_t::SerializeToMemory( src_buf, srcOffset );

        return SerializeToMemory( BitCast<VkDeviceAddress>( src_buf.GetDeviceAddress() + srcOffset ), dst );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (DeviceAddress src, RTSceneID dst)
    {
        auto&   dst_as = _GetResourcesOrThrow( dst );

        VkCopyMemoryToAccelerationStructureInfoKHR  info;
        info.sType              = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
        info.pNext              = null;
        info.src.deviceAddress  = VkDeviceAddress(src);
        info.dst                = dst_as.Handle();
        info.mode               = VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;

        return RawCtx::DeserializeFromMemory( info );
    }

    template <typename C>
    void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)
    {
        auto&   src_buf = _GetResourcesOrThrow( src );
        Validator_t::SerializeToMemory( src_buf, srcOffset );

        return SerializeToMemory( BitCast<VkDeviceAddress>( src_buf.GetDeviceAddress() + srcOffset ), dst );
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
        Validator_t::BuildIndirect( cmd, ibuf, indirectBufferOffset, indirectStride );
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
        Validator_t::BuildIndirect( cmd, ibuf, indirectBufferOffset );
        return RawCtx::_BuildIndirect( cmd, dst, BitCast<VkDeviceAddress>( ibuf.GetDeviceAddress() + indirectBufferOffset ));
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
