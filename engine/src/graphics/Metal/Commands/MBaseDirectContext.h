// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Private/ContextValidation.h"
# include "graphics/Metal/Commands/MBarrierManager.h"
# include "graphics/Metal/Commands/MDrawBarrierManager.h"
# include "graphics/Metal/Commands/MAccumDeferredBarriers.h"
# include "graphics/Metal/MRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{

    //
    // Metal Direct Context base class
    //

    class _MBaseDirectContext
    {
    // types
    public:
        static constexpr bool   IsIndirectContext = false;

        using CmdBuf_t = MCommandBuffer;


    // variables
    protected:
        MCommandBuffer      _cmdbuf;


    // methods
    public:
        virtual ~_MBaseDirectContext ()                                 __NE___ { DBG_CHECK_MSG( not (_cmdbuf.HasCmdBuf() or _cmdbuf.HasEncoder()), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" ); }

        void  PipelineBarrier (const MDependencyInfo &info);

    protected:
        explicit _MBaseDirectContext (MCommandBuffer cmdbuf)            __Th___ : _cmdbuf{ RVRef( cmdbuf )} { CHECK_THROW( _IsValid() ); }

        void  _DebugMarker (DebugLabel dbg)                                     { return _cmdbuf.DebugMarker( dbg ); }
        void  _PushDebugGroup (DebugLabel dbg)                                  { return _cmdbuf.PushDebugGroup( dbg ); }
        void  _PopDebugGroup ()                                                 { return _cmdbuf.PopDebugGroup(); }

        //void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);

        ND_ bool                    _IsValid ()                         C_NE___ { return _cmdbuf.IsRecording(); }

        ND_ bool                    _EndEncoding ();
        ND_ MetalCommandBufferRC    _EndCommandBuffer ();
        ND_ MCommandBuffer          _ReleaseCommandBuffer ();

        ND_ static MCommandBuffer  _ReuseOrCreateCommandBuffer (const MCommandBatch &batch, MCommandBuffer cmdbuf, DebugLabel dbg)      __NE___;
        ND_ static MCommandBuffer  _ReuseOrCreateCommandBuffer (const MDrawCommandBatch &batch, MCommandBuffer cmdbuf, DebugLabel dbg)  __NE___;
    };



    //
    // Metal Direct Context base class with barrier manager
    //

    class MBaseDirectContext : public _MBaseDirectContext
    {
    // variables
    protected:
        MBarrierManager     _mngr;


    // methods
    public:
        MBaseDirectContext (const RenderTask &task, MCommandBuffer, DebugLabel) __Th___;
        ~MBaseDirectContext ()                                                  __NE_OV { ASSERT( _NoPendingBarriers() ); }

    protected:
        ND_ bool    _NoPendingBarriers ()                                       C_NE___ { return _mngr.NoPendingBarriers(); }
        ND_ auto&   _GetFeatures ()                                             C_NE___ { return _mngr.GetDevice().GetFeatures(); }

        ND_ MetalCommandBufferRC    _EndCommandBuffer ();
    };
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf, DebugLabel dbg) __Th___ :
        _MBaseDirectContext{    // throw
            _ReuseOrCreateCommandBuffer( *task.GetBatchPtr(), RVRef(cmdbuf),
                                         dbg ? dbg : DebugLabel{ task.DbgFullName(), task.DbgColor() })
        },
        _mngr{ task }
    {
        ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );

        if ( auto* bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
            PipelineBarrier( *bar );
    }

/*
=================================================
    _EndCommandBuffer
=================================================
*/
    inline MetalCommandBufferRC  MBaseDirectContext::_EndCommandBuffer ()
    {
        if ( auto* bar = _mngr.GetBatch().ExtractFinalBarriers( _mngr.GetRenderTask().GetExecutionIndex() ))
            PipelineBarrier( *bar );

        return _MBaseDirectContext::_EndCommandBuffer();
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
