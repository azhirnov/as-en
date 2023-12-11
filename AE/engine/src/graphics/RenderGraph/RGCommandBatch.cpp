// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/RenderGraph/RGCommandBatch.h"
#include "graphics/Private/EnumUtils.h"

namespace AE::RG::_hidden_
{

/*
=================================================
    _UseResource
=================================================
*/
    void  RGCommandBatchPtr::RenderTaskBuilder::_UseResource (ResourceKey key, EResourceState initial, EResourceState final) __NE___
    {
        auto&   rg_batch = _RGBatch();
        DRC_EXLOCK( rg_batch._drCheck );

        EResourceState  old_state   = Default;
        EResourceState  def_state   = Default;

        final &= ~EResourceState::Invalidate;

        if_unlikely( initial == Default )
        {
            ResStateTracker::ResGlobalState g_state;

            // set current state as default
            if ( rg_batch._globalStates.UpdateResource( key, Default, _cmdBatch, OUT g_state ))
            {
                ASSERT( _cmdBatch.GetQueueType() == g_state.lastQueue or g_state.lastQueue == Default );
                def_state   = g_state.defaultState;
                initial     = (g_state.currentState | (_cmdBatch.GetQueueType() == g_state.lastQueue ? EResourceState::Unknown : EResourceState::Invalidate));
            }
            else
            {
                def_state   = EResourceState::General;
                initial     = def_state;
            }
            final = (final == Default ? def_state : final);
        }

        // get state from batch scope
        {
            auto&   batch_states = rg_batch._batchStates;
            auto    it           = batch_states.find( key );

            // already tracked in batch scope
            if ( it != batch_states.end() )
            {
                old_state   = it->second.current;
                initial     = (initial == Default ? old_state : initial);
            }
            else
            // add resource for tracking in batch
            {
                old_state = initial;

                if_unlikely( def_state == Default )
                {
                    auto [def_state2, is_undef] = rg_batch._globalStates.GetDefaultState( key );
                    def_state   = def_state2;
                    old_state   |= (is_undef and key.IsImage() ? EResourceState::Invalidate : Default); // image may be in 'undefined' layout
                }

                it = batch_states.emplace( key, RGBatchData::InBatchState{ initial, def_state }).first;
            }

            it->second.current = final;
        }

        // set initial state in task scope
        {
            auto&   rs = _StateMap();
            DRC_EXLOCK( rs.drCheck );

            bool    inserted = rs.map.insert_or_assign( key, InTaskState{ initial, final }).second;
            CHECK_ERRV( inserted ); // resource already used in task
        }

        if_likely( key.IsImage() ){
            if ( EResourceState_RequireImageBarrier( old_state, initial, True{"relaxed"} ))
                _initialBarriers.ImageBarrier( key.AsImage(), old_state, initial );
        }else{
            if ( EResourceState_RequireMemoryBarrier( old_state, initial, True{"relaxed"} ))
                _initialBarriers.MemoryBarrier( old_state, initial );
        }
    }

/*
=================================================
    UseResource
=================================================
*/
    RGCommandBatchPtr::RenderTaskBuilder&&  RGCommandBatchPtr::RenderTaskBuilder::UseResource (ImageViewID id, EResourceState initial, EResourceState final) rvNE___
    {
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();
        auto*   view     = res_mngr.GetResource( id );
        CHECK( view != null );

        if_likely( view != null )
            return RVRef(*this).UseResource( view->ImageId(), initial, final );

        return RVRef(*this);
    }

    RGCommandBatchPtr::RenderTaskBuilder&&  RGCommandBatchPtr::RenderTaskBuilder::UseResource (BufferViewID id, EResourceState initial, EResourceState final) rvNE___
    {
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();
        auto*   view     = res_mngr.GetResource( id );
        CHECK( view != null );

        if_likely( view != null )
            return RVRef(*this).UseResource( view->BufferId(), initial, final );

        return RVRef(*this);
    }

/*
=================================================
    _BeforeRun
=================================================
*/
    void  RGCommandBatchPtr::RenderTaskBuilder::_BeforeRun () __NE___
    {
        const bool  is_first    = _rtask->IsFirstInBatch();
        auto&       rg_batch    = *Cast<RGBatchData>( _cmdBatch.GetUserData() );
        DRC_EXLOCK( rg_batch._drCheck );

        {
            auto&   rs = rg_batch._perTask[ _rtask->GetExecutionIndex() ];
            DRC_EXLOCK( rs.drCheck );

            for (const auto& [id, state] : rg_batch._batchStates)
            {
                rs.map.emplace( id, InTaskState{ state.current, _last ? state.final : state.current });  // throw
            }
        }

        if_unlikely( is_first )
        {
            _initialBarriers.Merge( INOUT *rg_batch._initialBarriers );
        }

        if_unlikely( _last )
        {
            _finalBarriers.Merge( INOUT *rg_batch._finalBarriers );

            for (const auto& [id, state] : rg_batch._batchStates)
            {
                if_likely( id.IsImage() ){
                    if ( EResourceState_RequireImageBarrier( state.current, state.final, True{"relaxed"} ))
                        _finalBarriers.ImageBarrier( id.AsImage(), state.current, state.final );
                }else{
                    if ( EResourceState_RequireMemoryBarrier( state.current, state.final, True{"relaxed"} ))
                        _finalBarriers.MemoryBarrier( state.current, state.final );
                }
            }
            rg_batch._batchStates.clear();
        }

        CHECK( rg_batch._renderTasks.try_push_back( _rtask ));
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    RGCommandBatchPtr::RGBatchData::RGBatchData (RGBatchData &&other) __NE___ :
        _batchStates{ RVRef(other._batchStates) },
        _perTask{ RVRef(other._perTask) },
        _globalStates{ other._globalStates },
        _initialBarriers{ RVRef(other._initialBarriers) },
        _finalBarriers{ RVRef(other._finalBarriers) }
    {
        DRC_EXLOCK( other._drCheck );
    }

/*
=================================================
    SetRenderPassInitialStates
=================================================
*/
    void  RGCommandBatchPtr::RGBatchData::SetRenderPassInitialStates (uint taskIdx, INOUT RenderPassDesc &desc) __Th___
    {
        constexpr auto  rs_mask     = EResourceState::Invalidate;   // keep only 'Invalidate' flag
        auto&           res_mngr    = GraphicsScheduler().GetResourceManager();
        constexpr auto  new_state   = EResourceState::Unknown;      // disable transitions in render pass

        DRC_EXLOCK( _perTask[taskIdx].drCheck );

        for (auto [name, att] : desc.attachments)
        {
            auto&   view = res_mngr.GetResourcesOrThrow( att.imageView );

            EResourceState  old_state;
            if_likely( _ResourceState( taskIdx, ResourceKey{view.ImageId()}, new_state, OUT old_state ))
            {
                att.initial                 = old_state | (att.initial & rs_mask);
                att.final                   = new_state;
                att.relaxedStateTransition  = false;
            }
        }
    }

/*
=================================================
    SetRenderPassFinalStates
=================================================
*/
    void  RGCommandBatchPtr::RGBatchData::SetRenderPassFinalStates (uint taskIdx, const PrimaryCmdBufState_t &primaryState) __Th___
    {
    #if defined(AE_ENABLE_VULKAN)
        const auto      fb_images   = primaryState.framebuffer->Images();
        const auto&     att_states  = primaryState.renderPass->AttachmentStates();
        auto&           rs          = _perTask[ taskIdx ];

        DRC_EXLOCK( rs.drCheck );

        for (usize i = 0; i < fb_images.size(); ++i)
        {
            auto    it = rs.map.find( ResourceKey{fb_images[i]} );
            ASSERT( it != rs.map.end() );

            if_likely( it != rs.map.end() )
                it->second.current = att_states[i].final;   // RP will transit state to 'final' so update current state.
        }

    #elif defined(AE_ENABLE_METAL)
        // TODO
        Unused( taskIdx, primaryState );

    #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        // TODO
        Unused( taskIdx, primaryState );

    #else
    #   error not implemented
    #endif
    }

/*
=================================================
    _ResourceState
=================================================
*/
    bool  RGCommandBatchPtr::RGBatchData::_ResourceState (uint taskIdx, ResourceKey key, EResourceState newState, OUT EResourceState &oldState) __NE___
    {
        auto&   rs = _perTask[ taskIdx ];
        DRC_EXLOCK( rs.drCheck );

        auto    it = rs.map.find( key );

        // resource state is not tracked in task
        if_unlikely( it == rs.map.end() )
        {
            if_unlikely( not key.IsImage() and EResourceState_IsReadOnly( newState ))
                return false;   // don't track read-only resources

            // start resource state tracking in task
            auto [def_state, is_undef]  = _globalStates.GetDefaultState( key );
            EResourceState  current     = def_state | (is_undef and key.IsImage() ? EResourceState::Invalidate : Default);

            it = rs.map.emplace( key, InTaskState{ current, def_state }).first;  // throw
        }

        oldState = it->second.current;

        if_unlikely( AllBits( newState, EResourceState::Invalidate )) {
            // invalidate (discard previous content)
            it->second.current = oldState | EResourceState::Invalidate;
        }else{
            it->second.current = newState;
        }
        return true;
    }

/*
=================================================
    _CheckResourceState
=================================================
*/
    bool  RGCommandBatchPtr::RGBatchData::_CheckResourceState (uint taskIdx, ResourceKey key, const EResourceState state) C_NE___
    {
        constexpr auto  mask1   = EResourceState(_EResState::AccessMask);
        constexpr auto  mask2   = EResourceState(~_EResState::AccessMask);

        auto&   rs = _perTask[ taskIdx ];
        DRC_SHAREDLOCK( rs.drCheck );

        auto    it  = rs.map.find( key );

        if ( it != rs.map.end() )
        {
            if ( AllBits( it->second.current, state, mask1 )        and
                 AllBits( it->second.current & mask2, state & mask2 ))
                return true;

            if ( EResourceState_IsReadOnly( state ))
                return not EResourceState_HasWriteAccess( it->second.current );
        }

        // resource state is not tracked
        return true;
    }

/*
=================================================
    AddSurfaceTargets
=================================================
*/
    void  RGCommandBatchPtr::RGBatchData::AddSurfaceTargets (uint taskIdx, const App::IOutputSurface::RenderTargets_t &targets) __NE___
    {
        auto&   rs = _perTask[ taskIdx ];
        DRC_EXLOCK( rs.drCheck );

        for (auto& rt : targets)
        {
            // 'finalState' used as default state for transition between render tasks.
            auto    initial  = (taskIdx == 0 ? rt.initialState | EResourceState::Invalidate : rt.finalState);
            bool    inserted = rs.map.insert_or_assign( ResourceKey{rt.imageId}, InTaskState{ initial, rt.finalState }).second;
            CHECK( inserted );
        }
    }


} // AE::RG::_hidden_
