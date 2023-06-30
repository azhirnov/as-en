// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    ASBuildCtx -->  DirectASBuildCtx   --> BarrierMngr --> Metal device 
                \-> IndirectASBuildCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MBaseIndirectContext.h"
# include "graphics/Metal/Commands/MBaseDirectContext.h"
# include "graphics/Metal/Commands/MAccumBarriers.h"
# include "graphics/Metal/Resources/MRTGeometry.h"
# include "graphics/Metal/Resources/MRTScene.h"

namespace AE::Graphics::_hidden_
{

    //
    // Metal Direct AS Build Context implementation
    //

    class _MDirectASBuildCtx : public MBaseDirectContext
    {
    // methods
    public:
        void  Copy (MetalAccelStruct src, MetalAccelStruct dst)                             __Th___;
        void  CopyCompacted (MetalAccelStruct src, MetalAccelStruct dst)                    __Th___;

        ND_ MetalCommandBufferRC    EndCommandBuffer ()                                     __Th___;
        ND_ MCommandBuffer          ReleaseCommandBuffer ()                                 __Th___;

        MBARRIERMNGR_INHERIT_MBARRIERS

    protected:
        _MDirectASBuildCtx (const RenderTask &task, MCommandBuffer cmdbuf, DebugLabel dbg)  __Th___;

        ND_ auto  _Encoder ()                                                               __NE___;

        void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
        void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

        void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
        void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);

        void  _WriteCompactedSize (MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size);

        void  _DebugMarker (DebugLabel dbg)                                         { ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_DebugMarker( dbg ); }
        void  _PushDebugGroup (DebugLabel dbg)                                      { ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PushDebugGroup( dbg ); }
        void  _PopDebugGroup ()                                                     { ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PopDebugGroup(); }
    };



    //
    // Metal Indirect AS Build Context implementation
    //

    class _MIndirectASBuildCtx : public MBaseIndirectContext
    {
    // methods
    public:
        void  Copy (MetalAccelStruct src, MetalAccelStruct dst)                                 __Th___;
        void  CopyCompacted (MetalAccelStruct src, MetalAccelStruct dst)                        __Th___;

        ND_ MBakedCommands      EndCommandBuffer ()                                             __Th___;
        ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ()                                         __Th___;

        MBARRIERMNGR_INHERIT_MBARRIERS

    protected:
        _MIndirectASBuildCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)__Th___;

        void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
        void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

        void  _Build  (const RTSceneBuild &cmd, RTSceneID dst);
        void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);

        void  _WriteCompactedSize (MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size);
    };



    //
    // Metal AS Build Context implementation
    //

    template <typename CtxImpl>
    class _MASBuildContextImpl : public CtxImpl, public IASBuildContext
    {
    // types
    public:
        static constexpr bool   IsASBuildContext        = true;
        static constexpr bool   IsMetalASBuildContext   = true;

        using CmdBuf_t      = typename CtxImpl::CmdBuf_t;
    private:
        using RawCtx        = CtxImpl;
        using AccumBar      = MAccumBarriers< _MASBuildContextImpl< CtxImpl >>;
        using DeferredBar   = MAccumDeferredBarriersForCtx< _MASBuildContextImpl< CtxImpl >>;


    // methods
    public:
        explicit _MASBuildContextImpl (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)         __Th___;

        _MASBuildContextImpl ()                                                                                             = delete;
        _MASBuildContextImpl (const _MASBuildContextImpl &)                                                                 = delete;

        using RawCtx::Copy;
        using RawCtx::CopyCompacted;

        void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)                                                         __Th_OV { RawCtx::_Build( cmd, dst ); }
        void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)                                       __Th_OV { RawCtx::_Update( cmd, src, dst ); }
        void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)                        __Th_OV;

        void  Build  (const RTSceneBuild &cmd, RTSceneID dst)                                                               __Th_OV { RawCtx::_Build( cmd, dst ); }
        void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)                                                __Th_OV { RawCtx::_Update( cmd, src, dst ); }
        void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)                              __Th_OV;

        void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)         __Th_OV;
        void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)            __Th_OV;

        void  WriteProperty (ERTASProperty property, MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size)  __Th___;

        Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)                                              __Th_OV;
        Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)                                                 __Th_OV;

        ND_ Promise<Bytes>  ReadProperty (ERTASProperty property, MetalAccelStruct as)                                      __Th___;

        MBARRIERMNGR_INHERIT_BARRIERS
    };

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
    using MDirectASBuildContext     = Graphics::_hidden_::_MASBuildContextImpl< Graphics::_hidden_::_MDirectASBuildCtx >;
    using MIndirectASBuildContext   = Graphics::_hidden_::_MASBuildContextImpl< Graphics::_hidden_::_MIndirectASBuildCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{
/*
=================================================
    constructor
=================================================
*/
    template <typename C>
    _MASBuildContextImpl<C>::_MASBuildContextImpl (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) :
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
    void  _MASBuildContextImpl<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode)
    {
        auto  [src_geom, dst_geom]  = _GetResourcesOrThrow( src, dst );

        BEGIN_ENUM_CHECKS();
        switch ( mode )
        {
            case ERTASCopyMode::Clone :     return Copy( src_geom.Handle(), dst_geom.Handle() );
            case ERTASCopyMode::Compaction: return CopyCompacted( src_geom.Handle(), dst_geom.Handle() );
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown RT AS copy mode", void() );
    }

/*
=================================================
    Copy
=================================================
*/
    template <typename C>
    void  _MASBuildContextImpl<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode)
    {
        auto  [src_scene, dst_scene]  = _GetResourcesOrThrow( src, dst );

        BEGIN_ENUM_CHECKS();
        switch ( mode )
        {
            case ERTASCopyMode::Clone :     return Copy( src_scene.Handle(), dst_scene.Handle() );
            case ERTASCopyMode::Compaction: return CopyCompacted( src_scene.Handle(), dst_scene.Handle() );
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown RT AS copy mode", void() );
    }

/*
=================================================
    WriteProperty
=================================================
*/
    template <typename C>
    void  _MASBuildContextImpl<C>::WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)
    {
        auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );

        return WriteProperty( property, src_as.Handle(), dst_buf.Handle(), offset, size );
    }

    template <typename C>
    void  _MASBuildContextImpl<C>::WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)
    {
        auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );

        return WriteProperty( property, src_as.Handle(), dst_buf.Handle(), offset, size );
    }

    template <typename C>
    void  _MASBuildContextImpl<C>::WriteProperty (ERTASProperty property, MetalAccelStruct as, MetalBuffer dstBuffer, Bytes offset, Bytes size)
    {
        CHECK_ERRV( property == ERTASProperty::CompactedSize );

        return RawCtx::_WriteCompactedSize( as, dstBuffer, offset, size );
    }

/*
=================================================
    ReadProperty
=================================================
*/
    template <typename C>
    Promise<Bytes>  _MASBuildContextImpl<C>::ReadProperty (ERTASProperty property, RTGeometryID as)
    {
        auto&   src_as = _GetResourcesOrThrow( as );

        return ReadProperty( property, src_as.Handle() );
    }

    template <typename C>
    Promise<Bytes>  _MASBuildContextImpl<C>::ReadProperty (ERTASProperty property, RTSceneID as)
    {
        auto&   src_as = _GetResourcesOrThrow( as );

        return ReadProperty( property, src_as.Handle() );
    }

    template <typename C>
    Promise<Bytes>  _MASBuildContextImpl<C>::ReadProperty (ERTASProperty property, MetalAccelStruct as)
    {
        CHECK_ERR( property == ERTASProperty::CompactedSize );

        //  RawCtx::_WriteCompactedSize( as, staging_buffer, staging_buffer_offset, 4_b );

        Unused( as );
        // TODO
        return Default;
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
