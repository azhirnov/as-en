// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ecs-st/Core/Registry.h"

namespace AE::ECS
{

/*
=================================================
    constructor
=================================================
*/
    Registry::Registry () __Th___ :
        _componentInfo{ new ComponentMap_t::element_type{} }    // throw
    {
        DRC_EXLOCK( _drCheck );
    }

/*
=================================================
    destructor
=================================================
*/
    Registry::~Registry () __NE___
    {
        DRC_EXLOCK( _drCheck );
        ASSERT( _singleComponents.empty() );
    }

/*
=================================================
    GetArchetype
=================================================
*/
    Ptr<Archetype const>  Registry::GetArchetype (EntityID id) __NE___
    {
        ArchetypeStorage*   storage = null;
        Index_t             index;
        _entities.GetArchetype( id, OUT storage, OUT index );

        return storage ? &storage->GetArchetype() : null;
    }

/*
=================================================
    CreateEntity
=================================================
*/
    EntityID  Registry::CreateEntity () __NE___
    {
        DRC_EXLOCK( _drCheck );

        EntityID    id;
        CHECK_ERR( _entities.Assign( OUT id ));
        return id;
    }

/*
=================================================
    DestroyEntity
=================================================
*/
    bool  Registry::DestroyEntity (EntityID id) __NE___
    {
        DRC_EXLOCK( _drCheck );

        CHECK( _RemoveEntity( id ));
        return _entities.Unassign( id );
    }

/*
=================================================
    DestroyAllEntities
=================================================
*/
    void  Registry::DestroyAllEntities () __NE___
    {
        DRC_EXLOCK( _drCheck );

        for (auto& [arch, storage] : _archetypes)
        {
            // TODO: add message with MsgTag_RemovedComponent ???

            storage->Clear();
        }

        _entities.Clear();
        _archetypes.clear();
    }

/*
=================================================
    DestroyAllSingleComponents
=================================================
*/
    void  Registry::DestroyAllSingleComponents () __NE___
    {
        DRC_EXLOCK( _drCheck );

        _singleComponents.clear();
    }

/*
=================================================
    _RemoveEntity
=================================================
*/
    bool  Registry::_RemoveEntity (EntityID entId) __NE___
    {
        ArchetypeStorage*   storage = null;
        Index_t             index;

        if ( _entities.GetArchetype( entId, OUT storage, OUT index ) and storage != null )
        {
            ASSERT( storage->IsValid( entId, index ));
            ASSERT( not storage->IsLocked() );

            // add messages
            #if AE_ECS_ENABLE_DEFAULT_MESSAGES
            {
                auto    comp_ids    = storage->GetComponentIDs();
                auto    comp_sizes  = storage->GetComponentSizes();
                auto    comp_data   = storage->GetComponentData();

                for (usize i = 0; i < comp_ids.size(); ++i)
                {
                    usize   comp_size = usize(comp_sizes[i]);

                    if ( comp_size > 0 )
                    {
                        ubyte*  comp_ptr = Cast<ubyte>( comp_data[i] + Bytes{comp_size} * usize(index) );
                        CHECK( _messages.Add<MsgTag_RemovedComponent>( entId, comp_ids[i], ArrayView<ubyte>{ comp_ptr, comp_size }));
                    }
                    else
                        CHECK( _messages.Add<MsgTag_RemovedComponent>( entId, comp_ids[i] ));
                }
            }
            #endif

            EntityID    moved;
            if_unlikely( not storage->Erase( index, OUT moved ))
                return false;

            // update reference to entity which was moved to new index
            if ( moved )
                _entities.SetArchetype( moved, storage, index );

            _entities.SetArchetype( entId, null, Default );

            _DecreaseStorageSize( storage );
        }
        return true;
    }

/*
=================================================
    _AddEntity
=================================================
*/
    void  Registry::_AddEntity (const Archetype &arch, EntityID entId, OUT ArchetypeStorage* &outStorage, OUT Index_t &index) __NE___
    {
        auto                    [iter, inserted] = _archetypes.insert({ arch, ArchetypeStoragePtr{} });
        Archetype const&        key              = iter->first;
        ArchetypeStoragePtr&    storage          = iter->second;

        if ( inserted )
        {
            storage.reset( new ArchetypeStorage{ *this, key, ECS_Config::InitialStorageSize }); // throw

            _OnNewArchetype( &*iter );
        }

        ASSERT( not storage->IsLocked() );

        if ( storage->Add( entId, OUT index ))
        {
            outStorage = storage.get();
            _entities.SetArchetype( entId, storage.get(), index );
            return;
        }

        _IncreaseStorageSize( storage.get(), 1 );

        CHECK( storage->Add( entId, OUT index ));
        _entities.SetArchetype( entId, storage.get(), index );

        outStorage = storage.get();
    }

    void  Registry::_AddEntity (const Archetype &arch, EntityID entId) __NE___
    {
        ArchetypeStorage*   storage = null;
        Index_t             index;
        return _AddEntity( arch, entId, OUT storage, OUT index );
    }

/*
=================================================
    _MoveEntity
=================================================
*/
    void  Registry::_MoveEntity (const Archetype &arch, EntityID entId, ArchetypeStorage* srcStorage, Index_t srcIndex,
                                 OUT ArchetypeStorage* &dstStorage, OUT Index_t &dstIndex) __NE___
    {
        _AddEntity( arch, entId, OUT dstStorage, OUT dstIndex );

        if_likely( srcStorage != null )
        {
            ASSERT( not srcStorage->IsLocked() );

            auto    comp_ids = dstStorage->GetComponentIDs();

            // copy components
            for (auto& comp_id : comp_ids)
            {
                auto    src = srcStorage->GetComponent( srcIndex, comp_id );
                auto    dst = dstStorage->GetComponent( dstIndex, comp_id );

                if ( (src.first != null) & (dst.first != null) )
                {
                    ASSERT( src.second == dst.second );
                    MemCopy( OUT dst.first, src.first, src.second );
                }
            }

            EntityID    moved;
            srcStorage->Erase( srcIndex, OUT moved );

            // update reference to entity that was moved to new index
            if ( moved )
                _entities.SetArchetype( moved, srcStorage, srcIndex );

            _DecreaseStorageSize( srcStorage );
        }
    }

/*
=================================================
    RemoveComponent
=================================================
*/
    bool  Registry::RemoveComponent (EntityID entId, ComponentID compId) __NE___
    {
        DRC_EXLOCK( _drCheck );

        ArchetypeStorage*   src_storage     = null;
        Index_t             src_index;
        ArchetypeDesc       desc;

        _entities.GetArchetype( entId, OUT src_storage, OUT src_index );

        if_unlikely( src_storage == null )
            return false;

        // get new archetype
        {
            desc = src_storage->GetArchetype().Desc();

            if_unlikely( not desc.Exists( compId ))
                return false;

            desc.Remove( compId );
        }

        ASSERT( not src_storage->IsLocked() );

        auto    comp_data = src_storage->GetComponent( src_index, compId );

        #if AE_ECS_ENABLE_DEFAULT_MESSAGES
            if ( comp_data.first != null ) {
                CHECK( _messages.Add<MsgTag_RemovedComponent>( entId, compId, comp_data ));
            }else{
                CHECK( _messages.Add<MsgTag_RemovedComponent>( entId, compId ));
            }
        #endif

        // add entity to new archetype
        ArchetypeStorage*   dst_storage     = null;
        Index_t             dst_index;

        _MoveEntity( Archetype{desc}, entId, src_storage, src_index, OUT dst_storage, OUT dst_index );
        return true;
    }

/*
=================================================
    RemoveComponents
=================================================
*/
    void  Registry::RemoveComponents (QueryID query, const ArchetypeDesc &removeComps) __NE___
    {
        DRC_EXLOCK( _drCheck );

        const auto& q = _queries[ query.Index() ];

        CHECK( not q.locked );
        q.locked = true;

        const auto  remove_comp_ids = removeComps.GetIDs();

        for (auto* arch : q.archetypes)
        {
            ArchetypeDesc           desc            = arch->first.Desc();
            ArchetypeStorage*       src_storage     = arch->second.get();

            desc.Remove( removeComps );

            auto                    [iter, inserted] = _archetypes.insert({ Archetype{desc}, ArchetypeStoragePtr{} });
            Archetype const&        key              = iter->first;
            ArchetypeStoragePtr&    dst_storage      = iter->second;

            if ( inserted )
            {
                // TODO: if 'removeComps' is just a tags then you can change archetype in 'src_storage' instead of copying all data

                dst_storage.reset( new ArchetypeStorage{ *this, key, ECS_Config::InitialStorageSize }); // throw

                _OnNewArchetype( &*iter );
            }

            ASSERT( not src_storage->IsLocked() );
            ASSERT( not dst_storage->IsLocked() );

            // copy components
            {
                _IncreaseStorageSize( dst_storage.get(), src_storage->Count() );

                auto        comp_ids    = dst_storage->GetComponentIDs();
                auto        comp_sizes  = dst_storage->GetComponentSizes();
                auto        comp_data   = dst_storage->GetComponentData();
                usize       count       = src_storage->Count();

                Index_t     start;
                CHECK( dst_storage->AddEntities( ArrayView<EntityID>{ src_storage->GetEntities(), count }, OUT start ));

                for (usize i = 0; i < comp_ids.size(); ++i)
                {
                    ComponentID comp_id     = comp_ids[i];
                    Bytes       comp_size   = Bytes{comp_sizes[i]};
                    const auto* src         = src_storage->GetComponents( comp_id );
                    auto*       dst         = comp_data[i];

                    if ( (src != null) & (dst != null) )
                    {
                        dst = dst + (comp_size * usize(start));

                        MemCopy( OUT dst, src, comp_size * count );
                    }
                }

                for (usize i = 0; i < count; ++i)
                {
                    _entities.SetArchetype( src_storage->GetEntities()[i], dst_storage.get(), Index_t(usize(start) + i) );
                }
            }

            // add messages
            #if AE_ECS_ENABLE_DEFAULT_MESSAGES
            {
                auto    comp_ids    = src_storage->GetComponentIDs();
                auto    comp_sizes  = src_storage->GetComponentSizes();
                auto    comp_data   = src_storage->GetComponentData();
                auto*   ent         = src_storage->GetEntities();
                usize   count       = src_storage->Count();

                for (usize i = 0; i < comp_ids.size(); ++i)
                {
                    ComponentID comp_id     = comp_ids[i];
                    usize       comp_size   = count * usize(comp_sizes[i]);

                    if ( removeComps.Exists( comp_id ) and
                         _messages.HasListener<MsgTag_RemovedComponent>( comp_id ))
                    {
                        if ( comp_size > 0 ) {
                            CHECK( _messages.AddMulti<MsgTag_RemovedComponent>( comp_id, ArrayView{ ent, count }, ArrayView{ Cast<ubyte>(comp_data[i]), comp_size }));
                        }else{
                            CHECK( _messages.AddMulti<MsgTag_RemovedComponent>( comp_id, ArrayView{ ent, count }));
                        }
                    }
                }
            }
            #endif

            src_storage->Clear();
            _DecreaseStorageSize( src_storage );
        }

        q.locked = false;
    }
/*
=================================================
    Process
=================================================
*/
    void  Registry::Process () __NE___
    {
        DRC_EXLOCK( _drCheck );

        const auto  FlushEvents = [this] ()
        {{
            for (auto iter = _pendingEvents.rbegin(); iter != _pendingEvents.rend(); ++iter) {
                _eventQueue.push_back( RVRef(*iter) );  // throw
            }
            _pendingEvents.clear();
        }};

        //CHECK( _pendingEvents.empty() );
        CHECK( _eventQueue.empty() );

        _messages.Process();
        FlushEvents();

        for (; _eventQueue.size();)
        {
            auto    fn = RVRef( _eventQueue.back() );
            _eventQueue.pop_back();

            fn();
            _messages.Process();

            FlushEvents();
        }
    }

/*
=================================================
    CreateQuery
=================================================
*/
    QueryID  Registry::CreateQuery (const ArchetypeQueryDesc &desc) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK( desc.IsValid() );

        for (usize i = 0; i < _queries.size(); ++i)
        {
            if ( desc == _queries[i].desc )
                return QueryID{ CheckCast<ushort>(i), 0 };
        }

        auto&   q = _queries.emplace_back();    // throw
        q.desc  = desc;

        for (auto& arch : _archetypes)
        {
            if ( desc.Compatible( arch.first.Desc() ))
                q.archetypes.push_back( &arch );        // throw
        }

        q.locked = false;
        return QueryID{ CheckCast<ushort>(_queries.size()-1), 0 };
    }

/*
=================================================
    _OnNewArchetype
=================================================
*/
    void  Registry::_OnNewArchetype (ArchetypePair_t* arch) __NE___
    {
        for (usize i = 0; i < _queries.size(); ++i)
        {
            auto&   q = _queries[i];

            if ( q.desc.Compatible( arch->first.Desc() ))
            {
                CHECK( not q.locked );

                q.archetypes.push_back( arch );
            }
        }
    }


} // AE::ECS
