// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Core/ArchetypeStorage.h"

namespace AE::ECS
{

    //
    // Entity Pool
    //

    struct EntityPool
    {
    // types
    public:
        using Self      = EntityPool;
        using Index_t   = EntityID::Index_t;

    private:
        using Generation_t  = EntityID::Generation_t;
        using LocalIndex_t  = ArchetypeStorage::Index_t;

        static constexpr LocalIndex_t   InvalidIndex = Default;

        struct EntityRef
        {
            ArchetypeStorage*   storage     = null;
            LocalIndex_t        index       = InvalidIndex;
            Generation_t        generation  = 0;
        };


    // variables
    private:
        Array< EntityRef >      _entities;
        Array< Index_t >        _available;


    // methods
    public:
        EntityPool ()                                                                                   __NE___ {}
        ~EntityPool ()                                                                                  __NE___;

        ND_ bool  Assign (OUT EntityID &id)                                                             __NE___;
            bool  Unassign (EntityID id)                                                                __NE___;

            bool  SetArchetype (EntityID id, ArchetypeStorage* storage, LocalIndex_t index)             __NE___;
            bool  GetArchetype (EntityID id, OUT ArchetypeStorage* &storage, OUT LocalIndex_t &index)   C_NE___;

        ND_ bool  IsValid (EntityID id)                                                                 C_NE___;

            void  Clear ()                                                                              __NE___;
    };



/*
=================================================
    destructor
=================================================
*/
    inline EntityPool::~EntityPool () __NE___
    {
        ASSERT( _entities.size() == _available.size() );
    }

/*
=================================================
    Assign
=================================================
*/
    inline bool  EntityPool::Assign (OUT EntityID &id) __NE___
    {
        Index_t idx;

        if ( _available.size() )
        {
            idx = _available.back();
            _available.pop_back();
        }
        else
        {
            idx = Index_t(_entities.size());
            NOTHROW_ERR( _entities.push_back( EntityRef{} ));
        }

        id = EntityID{ idx, _entities[idx].generation };
        return true;
    }

/*
=================================================
    Unassign
=================================================
*/
    inline bool  EntityPool::Unassign (EntityID id) __NE___
    {
        CHECK_ERR( IsValid( id ));

        auto& item = _entities[ id.Index() ];

        item.storage = null;
        item.index   = InvalidIndex;

        ++item.generation;
        NOTHROW_ERR( _available.push_back( id.Index() ));
        return true;
    }

/*
=================================================
    SetArchetype
=================================================
*/
    inline bool  EntityPool::SetArchetype (EntityID id, ArchetypeStorage* storage, LocalIndex_t index) __NE___
    {
        if ( IsValid( id ))
        {
            ASSERT( (storage != null) == (index != InvalidIndex) );

            auto& item   = _entities[ id.Index() ];
            item.storage = storage;
            item.index   = index;
            return true;
        }
        return false;
    }

/*
=================================================
    GetArchetype
=================================================
*/
    inline bool  EntityPool::GetArchetype (EntityID id, OUT ArchetypeStorage* &storage, OUT LocalIndex_t &index) C_NE___
    {
        if ( IsValid( id ))
        {
            auto& item   = _entities[ id.Index() ];
            storage = item.storage;
            index   = item.index;
            return true;
        }
        return false;
    }

/*
=================================================
    IsValid
=================================================
*/
    inline bool  EntityPool::IsValid (EntityID id) C_NE___
    {
        return  id.Index() < _entities.size() and
                _entities[ id.Index() ].generation == id.Generation();
    }

/*
=================================================
    Clear
=================================================
*/
    inline void  EntityPool::Clear () __NE___
    {
        _entities.clear();
        _available.clear();
    }


} // AE::ECS
