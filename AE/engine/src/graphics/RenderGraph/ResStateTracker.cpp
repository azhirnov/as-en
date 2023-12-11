// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/RenderGraph/ResStateTracker.h"

namespace AE::RG::_hidden_
{
/*
=================================================
    destructor
=================================================
*/
    ResStateTracker::ResourceMap::~ResourceMap () __NE___
    {
    #ifdef AE_DEBUG
        for (auto& chunk_ptr : _idToStateIdx)
        {
            auto*   chunk = chunk_ptr.exchange( null );
            if ( chunk != null )
            {
                for (auto& a_idx : *chunk)
                {
                    Index_t idx = a_idx.exchange( UMax );
                    if ( idx != UMax )
                        CHECK( _statePool.Unassign( idx ));
                }
            }
        }
    #endif

        _statePool.Release( False{"don't check for assigned"} );
    }

/*
=================================================
    Contains
=================================================
*/
    bool  ResStateTracker::ResourceMap::Contains (ResourceKey key) C_NE___
    {
        if ( key.IsInvalid() )
            return false;

        const usize     chunk_idx   = key.Index() % MaxChunks;
        const usize     idx         = key.Index() / MaxChunks;
        const auto*     chunk       = _idToStateIdx[chunk_idx].load();
        ASSERT( idx < ChunkSize );

        if ( chunk != null )
        {
            const Index_t   elem_idx = (*chunk)[idx].load();

            if ( elem_idx != UMax )
            {
                auto&   elem = _statePool[ elem_idx ];

                SHAREDLOCK( elem.guard );
                return elem.id == key.id;
            }
        }
        return false;
    }

/*
=================================================
    Assign
=================================================
*/
    bool  ResStateTracker::ResourceMap::Assign (ResourceKey key, const ResGlobalState &info) __NE___
    {
        if ( key.IsInvalid() )
            return false;

        const usize     chunk_idx   = key.Index() % MaxChunks;
        const usize     idx         = key.Index() / MaxChunks;
        auto&           chunk_ptr   = _idToStateIdx[chunk_idx];
        IDtoIdxChunk_t* chunk       = chunk_ptr.load();
        ASSERT( idx < ChunkSize );

        if_unlikely( chunk == null )
        {
            // only one thread will allocate new chunk
            EXLOCK( _allocGuard );

            chunk = chunk_ptr.load();

            // another thread may allocate this chunk
            if ( chunk == null )
            {
                GlobalLinearAllocatorRef    alloc;
                chunk = Cast<IDtoIdxChunk_t>( alloc.Allocate( SizeAndAlignOf<IDtoIdxChunk_t> ));
                CHECK_ERR( chunk != null );

                std::memset( chunk->data(), 0xFF, usize(ArraySizeOf(*chunk)) );

                CHECK( chunk_ptr.exchange( chunk ) == null );
            }
        }

        auto&       elem_idx_ref    = (*chunk)[idx];
        Index_t     elem_idx        = elem_idx_ref.load();

        if ( elem_idx == UMax )
            CHECK_ERR( _statePool.Assign( OUT elem_idx ));

        // override previous state
        {
            auto&   elem    = _statePool[ elem_idx ];
            EXLOCK( elem.guard );

            elem.state  = info;
            elem.id     = key.id;
        }

        Index_t     old_idx = elem_idx_ref.exchange( elem_idx );

        if_unlikely( elem_idx != old_idx and _statePool.IsAssigned( old_idx ))
        {
            DEBUG_ONLY(
                auto&   elem = _statePool[ old_idx ];
                EXLOCK( elem.guard );
                CHECK( elem.id == key.id );
            )
            _statePool.Unassign( old_idx );
        }

        return true;
    }

/*
=================================================
    Remove
=================================================
*/
    bool  ResStateTracker::ResourceMap::Remove (ResourceKey key) __NE___
    {
        if ( key.IsInvalid() )
            return false;

        const usize     chunk_idx   = key.Index() % MaxChunks;
        const usize     idx         = key.Index() / MaxChunks;
        auto*           chunk       = _idToStateIdx[chunk_idx].load();
        ASSERT( idx < ChunkSize );

        if_likely( chunk != null )
        {
            Index_t     old_idx = (*chunk)[idx].exchange( UMax );

            return _statePool.Unassign( old_idx );
        }
        return false;
    }

/*
=================================================
    Find
=================================================
*/
    ResStateTracker::ResourceMap::SearchResult  ResStateTracker::ResourceMap::Find (ResourceKey key) __NE___
    {
        if ( key.IsInvalid() )
            return SearchResult{ null };

        const usize     chunk_idx   = key.Index() % MaxChunks;
        const usize     idx         = key.Index() / MaxChunks;
        const auto*     chunk       = _idToStateIdx[chunk_idx].load();
        ASSERT( idx < ChunkSize );

        if_likely( chunk != null )
        {
            const Index_t   elem_idx = (*chunk)[idx].load();

            if_likely( elem_idx != UMax )
            {
                auto&   ref = _statePool[ elem_idx ];
                ref.guard.lock();

                if_likely( ref.id == key.id )
                    return SearchResult{ &ref };

                ref.guard.unlock();
            }
        }
        return SearchResult{ null };
    }

/*
=================================================
    Find
=================================================
*/
    ResStateTracker::ResourceMap::CSearchResult  ResStateTracker::ResourceMap::Find (ResourceKey key) C_NE___
    {
        if ( key.IsInvalid() )
            return CSearchResult{ null };

        const usize     chunk_idx   = key.Index() % MaxChunks;
        const usize     idx         = key.Index() / MaxChunks;
        const auto*     chunk       = _idToStateIdx[chunk_idx].load();
        ASSERT( idx < ChunkSize );

        if_likely( chunk != null )
        {
            const Index_t   elem_idx = (*chunk)[idx].load();

            if_likely( elem_idx != UMax )
            {
                auto&   ref = _statePool[ elem_idx ];
                ref.guard.lock_shared();

                if_likely( ref.id == key.id )
                    return CSearchResult{ &ref };

                ref.guard.unlock_shared();
            }
        }
        return CSearchResult{ null };
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _ResMngr
=================================================
*/
    IResourceManager&  ResStateTracker::_ResMngr () C_NE___
    {
        return GraphicsScheduler().GetResourceManager();
    }

/*
=================================================
    _AddResource (ImageID)
=================================================
*/
    bool  ResStateTracker::_AddResource (ImageID id, EResourceState currentState, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue) __NE___
    {
        if ( defaultState == Default )
            defaultState = EResourceState::General;

        const ImageDesc     desc = _ResMngr().GetDescription( id );
        ResGlobalState      info;

        info.defaultState       = defaultState;
        info.currentState       = currentState;
        info.exclusiveSharing   = desc.IsExclusiveSharing();

        if ( batch )
        {
            info.lastBatch  = batch->GetSemaphore();
            info.lastQueue  = batch->GetQueueType();
            ASSERT( queue == Default or info.lastQueue == queue );
        }
        else
        {
            info.lastQueue = queue;
        }
        return _AddResource2( ResourceKey{id}, info );
    }

/*
=================================================
    _AddResource (BufferID)
=================================================
*/
    bool  ResStateTracker::_AddResource (BufferID id, EResourceState currentState, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue) __NE___
    {
        if ( defaultState == Default )
            defaultState = EResourceState::General;

        const BufferDesc    desc = _ResMngr().GetDescription( id );
        ResGlobalState      info;

        info.defaultState       = defaultState;
        info.currentState       = currentState;
        info.exclusiveSharing   = desc.IsExclusiveSharing();

        if ( batch )
        {
            info.lastBatch  = batch->GetSemaphore();
            info.lastQueue  = batch->GetQueueType();
            ASSERT( queue == Default or info.lastQueue == queue );
        }
        else
        {
            info.lastQueue = queue;
        }
        return _AddResource2( ResourceKey{id}, info );
    }

/*
=================================================
    _AddResource (RTGeometryID)
=================================================
*/
    bool  ResStateTracker::_AddResource (RTGeometryID id, EResourceState currentState, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue) __NE___
    {
        if ( defaultState == Default )
            defaultState = EResourceState::General;

        ResGlobalState          info;
        info.defaultState       = defaultState;
        info.currentState       = currentState;
        info.exclusiveSharing   = false;

        if ( batch )
        {
            info.lastBatch  = batch->GetSemaphore();
            info.lastQueue  = batch->GetQueueType();
            ASSERT( queue == Default or info.lastQueue == queue );
        }
        else
        {
            info.lastQueue = queue;
        }
        return _AddResource2( ResourceKey{id}, info );
    }

/*
=================================================
    _AddResource (RTSceneID)
=================================================
*/
    bool  ResStateTracker::_AddResource (RTSceneID id, EResourceState currentState, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue) __NE___
    {
        if ( defaultState == Default )
            defaultState = EResourceState::General;

        ResGlobalState          info;
        info.defaultState       = defaultState;
        info.currentState       = currentState;
        info.exclusiveSharing   = false;

        if ( batch )
        {
            info.lastBatch  = batch->GetSemaphore();
            info.lastQueue  = batch->GetQueueType();
            ASSERT( queue == Default or info.lastQueue == queue );
        }
        else
        {
            info.lastQueue = queue;
        }
        return _AddResource2( ResourceKey{id}, info );
    }

/*
=================================================
    _AddResource2
=================================================
*/
    inline bool  ResStateTracker::_AddResource2 (ResourceKey key, const ResGlobalState &info) __NE___
    {
        return _globalStates[ key.type ].Assign( key, info );
    }

/*
=================================================
    Create***
=================================================
*/
    Strong<ImageID>         ResStateTracker::CreateImage (const ImageDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)          __NE___ { auto id = _ResMngr().CreateImage( desc, dbgName, RVRef(allocator) );      AddResource( id.Get() );    return RVRef(id); }
    Strong<BufferID>        ResStateTracker::CreateBuffer (const BufferDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)        __NE___ { auto id = _ResMngr().CreateBuffer( desc, dbgName, RVRef(allocator) );     AddResource( id.Get() );    return RVRef(id); }
    Strong<ImageID>         ResStateTracker::CreateImage (const NativeImageDesc_t &desc, StringView dbgName)                                __NE___ { auto id = _ResMngr().CreateImage( desc, dbgName );                        AddResource( id.Get() );    return RVRef(id); }
    Strong<BufferID>        ResStateTracker::CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName)                              __NE___ { auto id = _ResMngr().CreateBuffer( desc, dbgName );                       AddResource( id.Get() );    return RVRef(id); }
    Strong<RTGeometryID>    ResStateTracker::CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)__NE___ { auto id = _ResMngr().CreateRTGeometry( desc, dbgName, RVRef(allocator) ); AddResource( id.Get() );    return RVRef(id); }
    Strong<RTSceneID>       ResStateTracker::CreateRTScene (const RTSceneDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)      __NE___ { auto id = _ResMngr().CreateRTScene( desc, dbgName, RVRef(allocator) );    AddResource( id.Get() );    return RVRef(id); }

    Strong<ImageViewID>     ResStateTracker::CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName)                 __NE___ { auto id = _ResMngr().CreateImageView( desc, image, dbgName );     AddResourceIfNotTracked( image );   return RVRef(id); }
    Strong<BufferViewID>    ResStateTracker::CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName)             __NE___ { auto id = _ResMngr().CreateBufferView( desc, buffer, dbgName );   AddResourceIfNotTracked( buffer );  return RVRef(id); }

/*
=================================================
    UpdateResource
=================================================
*/
    bool  ResStateTracker::UpdateResource (ResourceKey key, EResourceState newState, const CommandBatch &newBatch, OUT ResGlobalState &oldState) __NE___
    {
        auto&   map     = _globalStates[ key.type ];
        auto    it      = map.Find( key );
        if_likely( it )
        {
            oldState = *it;

            it->currentState    = newState == Default ? it->defaultState : newState;
            it->lastBatch       = newBatch.GetSemaphore();
            it->lastQueue       = newBatch.GetQueueType();
            return true;
        }
        return false;
    }

/*
=================================================
    RemoveResource
=================================================
*/
    void  ResStateTracker::RemoveResource (ResourceKey key) __NE___
    {
        if ( key.type < _globalStates.size() )
            _globalStates[ key.type ].Remove( key );
    }

/*
=================================================
    ReleaseResource
=================================================
*/
    template <typename ID>
    bool  ResStateTracker::_ReleaseResource (INOUT ID &id) __NE___
    {
        auto    tmp = id.Get();
        bool    res = _ResMngr().ReleaseResource( INOUT id );

        if ( res )
            RemoveResource( tmp );

        return res;
    }

    bool  ResStateTracker::ReleaseResource (INOUT Strong<ImageID>       &id) __NE___ { return _ReleaseResource( id ); }
    bool  ResStateTracker::ReleaseResource (INOUT Strong<BufferID>      &id) __NE___ { return _ReleaseResource( id ); }
    bool  ResStateTracker::ReleaseResource (INOUT Strong<RTGeometryID>  &id) __NE___ { return _ReleaseResource( id ); }
    bool  ResStateTracker::ReleaseResource (INOUT Strong<RTSceneID>     &id) __NE___ { return _ReleaseResource( id ); }
    bool  ResStateTracker::ReleaseResource (INOUT Strong<ImageViewID>   &id) __NE___ { return _ResMngr().ReleaseResource( INOUT id ); }
    bool  ResStateTracker::ReleaseResource (INOUT Strong<BufferViewID>  &id) __NE___ { return _ResMngr().ReleaseResource( INOUT id ); }

/*
=================================================
    GetDefaultState
=================================================
*/
    Tuple<EResourceState, bool>  ResStateTracker::GetDefaultState (ResourceKey key) C_NE___
    {
        auto&   map = _globalStates[ key.type ];
        auto    it  = map.Find( key );

        if_likely( it )
            return Tuple{ it->defaultState, it->lastQueue == Default };

        return Tuple{ EResourceState::General, True{"undefined"} };
    }

/*
=================================================
    SetDefaultState
=================================================
*/
    bool  ResStateTracker::SetDefaultState (ResourceKey key, EResourceState state) __NE___
    {
        auto&   map     = _globalStates[ key.type ];
        auto    it      = map.Find( key );
        if_likely( it )
        {
            it->defaultState = state;
            return true;
        }
        return false;
    }

/*
=================================================
    IsTracked
=================================================
*/
    bool  ResStateTracker::IsTracked (ResourceKey key) C_NE___
    {
        auto&   map = _globalStates[ key.type ];
        return map.Contains( key );
    }


} // AE::RG::_hidden_
