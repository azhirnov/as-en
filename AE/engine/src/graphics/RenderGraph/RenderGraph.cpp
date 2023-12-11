// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/RenderGraph/RenderGraph.h"

namespace AE::RG::_hidden_
{

/*
=================================================
    constructor
=================================================
*/
    RenderGraph::RenderGraph () __NE___ :
        _rts{ GraphicsScheduler() }
    {
        NOTHROW( _semToBatch.reserve( GraphicsConfig::MaxPendingCmdBatches );)
    }

/*
=================================================
    destructor
=================================================
*/
    RenderGraph::~RenderGraph () __NE___
    {
    }

/*
=================================================
    WaitAll
=================================================
*/
    bool  RenderGraph::WaitAll (milliseconds timeout) __NE___
    {
        if ( _rts.WaitAll( timeout ))
        {
            _ClearCurrentFrame();
            return true;
        }
        return false;
    }

/*
=================================================
    _ClearCurrentFrame
=================================================
*/
    void  RenderGraph::_ClearCurrentFrame () __NE___
    {
        _rgDataPool.clear();
        _semToBatch.clear();
        _outSurfaces.clear();
    }

/*
=================================================
    _CmdBatch
=================================================
*/
    RGCommandBatchPtr  RenderGraph::_CmdBatch (const EQueueType queue, DebugLabel dbg) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _rgDataPool.size() < _rgDataPool.capacity() );   // overflow

        auto&   f           = _CurrentFrame();
        uint    idx         = f.queues[ uint(queue) ].submitIdx++;
        auto*   rg_batch    = &_rgDataPool.emplace_back( *this );
        auto    cmd_batch   = _rts.BeginCmdBatch( queue, idx, dbg, rg_batch );

        CHECK_ERR( cmd_batch );

        // Allow to merge batches into single submit.
        cmd_batch->SetSubmissionMode( ESubmitMode::Deferred );

        rg_batch->_initialBarriers.emplace( cmd_batch->DeferredBarriers() );
        rg_batch->_finalBarriers  .emplace( cmd_batch->DeferredBarriers() );

        #if defined(AE_ENABLE_VULKAN)
            _semToBatch.insert_or_assign( ulong(cmd_batch->GetSemaphore().semaphore), cmd_batch );

        #elif defined(AE_ENABLE_METAL)
            _semToBatch.insert_or_assign( ulong(cmd_batch->GetSemaphore().event.Ptr()), cmd_batch );

        #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            _semToBatch.insert_or_assign( cmd_batch->GetSemaphore().semaphoreId, cmd_batch );

        #else
        #   error not implemented
        #endif

        return RVRef(cmd_batch);
    }

/*
=================================================
    BeginOnSurface
----
    always use 'ctx.AddSurfaceTargets()' to start image state tracking
=================================================
*/
    AsyncTask  RenderGraph::BeginOnSurface (Ptr<App::IOutputSurface> surface, RGCommandBatchPtr batch) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( surface );
        CHECK_ERR( batch );

        auto    [it, inserted] = _outSurfaces.emplace( surface, OutSurfaceInfo{} );
        if ( inserted )
        {
            CHECK_ERR_MSG( batch.AsBatch()->CurrentCmdBufIndex() == 0,
                "cmd batch must not start rendering until it acquire surface image" );

            batch.AsBatch()->SetSubmissionMode( ESubmitMode::Immediately );

            it->second.acquireImageTask = surface->Begin( batch.AsBatchRC(), batch.AsBatchRC(), Default );
            it->second.forBatch         = RVRef(batch);
        }
        else
        {
            CHECK_ERR( it->second.forBatch == batch );
        }

        if_unlikely( not it->second.acquireImageTask )
        {
            _outSurfaces.EraseByIter( it );
            RETURN_ERR( "failed to acquire surface image" );
        }
        return it->second.acquireImageTask;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _UseResource
=================================================
*/
    void  RenderGraph::CmdBatchBuilder::_UseResource (const ResourceKey key, EResourceState initial, EResourceState final)  __NE___
    {
        CHECK_ERRV( _batch );

        auto&       cmd_batch   = *_batch.AsBatch();
        auto&       rg_batch    = *_batch.AsRG();
        const auto  dst_queue   = cmd_batch.GetQueueType();

        DRC_EXLOCK( rg_batch._drCheck );

        ResGlobalState  old_state;
        if_unlikely( not _rg.UpdateResource( key, final, cmd_batch, OUT old_state ))
        {
            DBG_WARNING( "resource is not tracked by RG, use 'rg.AddResource()' before" );

            // allow to use untracked resources
            ASSERT( initial != Default );
            ASSERT( final   != Default );
            CHECK( rg_batch._batchStates.insert_or_assign( key, RGBatchData::InBatchState{ initial, final }).second );

            return;
        }

        if ( initial == Default )   initial = (old_state.currentState == Default ? old_state.defaultState : old_state.currentState);
        if ( final   == Default )   final   = old_state.defaultState;

        ASSERT( initial != Default );
        ASSERT( final   != Default );

        EResourceState  prev_state = old_state.currentState;

        if_unlikely( old_state.lastQueue != dst_queue and old_state.lastBatch )
        {
            cmd_batch.AddInputSemaphore( old_state.lastBatch );


            #if defined(AE_ENABLE_VULKAN)
            auto    batch_it = _rg._semToBatch.find( ulong(old_state.lastBatch.semaphore) );

            #elif defined(AE_ENABLE_METAL)
            auto    batch_it = _rg._semToBatch.find( ulong(old_state.lastBatch.event.Ptr()) );

            #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            auto    batch_it = _rg._semToBatch.find( old_state.lastBatch.semaphoreId );

            #else
            #   error not implemented
            #endif

            // queue ownership transfer operation
            if_likely( batch_it != _rg._semToBatch.end() )
            {
                CHECK( not batch_it->second->IsSubmitted() );

                // Submit batch immediately if it has dependency to batch in different queue,
                // otherwise it is allowed to merge batches to the single submit.
                batch_it->second->SetSubmissionMode( ESubmitMode::Immediately );

                // resource with exclusive sharing requires queue ownership transfer operation
                if_unlikely( old_state.exclusiveSharing )
                {
                    auto&   prev_rg_batch = *Cast<RGCommandBatchPtr::RGBatchData>( batch_it->second->GetUserData() );
                    DRC_SHAREDLOCK( prev_rg_batch._drCheck );

                    auto    res_it = prev_rg_batch._batchStates.find( key );

                    if ( res_it != prev_rg_batch._batchStates.end() )
                        prev_state = res_it->second.final;

                    if_likely( key.IsImage() )
                    {
                        rg_batch._initialBarriers->AcquireImageOwnership( key.AsImage(), old_state.lastQueue, prev_state, initial );
                        prev_rg_batch._finalBarriers->ReleaseImageOwnership( key.AsImage(), prev_state, initial, dst_queue );
                    }
                    else
                    if ( key.IsBuffer() )
                    {
                        rg_batch._initialBarriers->AcquireBufferOwnership( key.AsBuffer(), old_state.lastQueue, prev_state, initial );
                        prev_rg_batch._finalBarriers->ReleaseBufferOwnership( key.AsBuffer(), prev_state, initial, dst_queue );
                    }
                    else
                        DBG_WARNING( "unsupported resource type" );

                    CHECK( rg_batch._batchStates.insert_or_assign( key, RGBatchData::InBatchState{ initial, final }).second );
                    return;
                }
            }
            else
            if_unlikely( old_state.exclusiveSharing )
            {
                // can not add queue ownership transfer, source batch is not known,
                // so content of the resource may be invalidated

                prev_state |= EResourceState::Invalidate;

                AE_LOG_DBG( "source batch is not known, resource content will be invalidated" );
            }
        }

        // additional state transition if expected state differs from default state
        if_likely( key.IsImage() ){
            if ( EResourceState_RequireImageBarrier( prev_state, initial, True{"relaxed"} ))
                rg_batch._initialBarriers->ImageBarrier( key.AsImage(), prev_state, initial );
        }else{
            if ( EResourceState_RequireMemoryBarrier( prev_state, initial, True{"relaxed"} ))
                rg_batch._initialBarriers->MemoryBarrier( prev_state, initial );
        }

        CHECK( rg_batch._batchStates.insert_or_assign( key, RGBatchData::InBatchState{ initial, final }).second );
    }

/*
=================================================
    UploadMemory
=================================================
*/
    RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UploadMemory () rvNE___
    {
        CHECK_ERR( _batch, RVRef(*this) );

        auto&   rg_batch = *_batch.AsRG();

        rg_batch._uploadMemory.store( UMax );

        rg_batch._initialBarriers->MemoryBarrier( EResourceState::Host_Write, EResourceState::VertexBuffer );
        rg_batch._initialBarriers->MemoryBarrier( EResourceState::Host_Write, EResourceState::IndexBuffer );
        rg_batch._initialBarriers->MemoryBarrier( EResourceState::Host_Write, EResourceState::CopySrc );

        //rg_batch._initialBarriers->MemoryBarrier( EResourceState::Host_Write, EResourceState::IndirectBuffer );
        //rg_batch._initialBarriers->MemoryBarrier( EResourceState::Host_Write, EResourceState::ShaderUniform );

        return RVRef(*this);
    }

/*
=================================================
    ReadbackMemory
=================================================
*/
    RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::ReadbackMemory () rvNE___
    {
        CHECK_ERR( _batch, RVRef(*this) );

        auto&   rg_batch = *_batch.AsRG();

        rg_batch._readbackMemory.store( UMax );

        rg_batch._finalBarriers->MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

        return RVRef(*this);
    }

/*
=================================================
    UseResource
=================================================
*/
    RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResource (ImageViewID id, EResourceState initial, EResourceState final) rvNE___
    {
        auto&   res_mngr = _rg._rts.GetResourceManager();
        auto*   view     = res_mngr.GetResource( id );
        CHECK( view != null );

        if_likely( view != null )
            return RVRef(*this).UseResource( view->ImageId(), initial, final );

        return RVRef(*this);
    }

    RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResource (BufferViewID id, EResourceState initial, EResourceState final) rvNE___
    {
        auto&   res_mngr = _rg._rts.GetResourceManager();
        auto*   view     = res_mngr.GetResource( id );
        CHECK( view != null );

        if_likely( view != null )
            return RVRef(*this).UseResource( view->BufferId(), initial, final );

        return RVRef(*this);
    }


} // AE::RG::_hidden_
