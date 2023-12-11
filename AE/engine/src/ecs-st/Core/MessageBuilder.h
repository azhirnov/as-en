// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Common.h"

namespace AE::ECS
{
    struct MsgTag_AddedComponent {};
    struct MsgTag_RemovedComponent {};
    struct MsgTag_ComponentChanged {};


    //
    // Message Builder
    //

    class MessageBuilder
    {
    // types
    private:
        struct MessageKey
        {
            usize   _value = UMax;

            MessageKey ()                                   __NE___ {}
            MessageKey (ComponentID compId, MsgTagID tagId) __NE___;

            ND_ bool  operator == (const MessageKey &)      C_NE___;
        };

        struct MessageKeyHash {
            usize  operator () (const MessageKey &)         C_NE___;
        };

        struct MessageData
        {
            using Listener_t = Function< void (MessageData &) >;

            Array< EntityID >       entities;
            Array< ubyte >          components;     // TODO: align
            Array< Listener_t >     listeners;
        };

        using MessageMap_t  = HashMap< MessageKey, MessageData, MessageKeyHash >;
        using Pending_t     = Array< MessageData * >;


    // variables
    private:
        MessageMap_t    _msgTypes;
        Pending_t       _pending;


    // methods
    public:

        template <typename Tag>
        ND_ bool  Add (EntityID id, ComponentID compId)                                             __NE___;

        template <typename Tag>
        ND_ bool  Add (EntityID id, ComponentID compId, ArrayView<ubyte> data)                      __NE___;

        template <typename Tag>
        ND_ bool  Add (EntityID id, ComponentID compId, const Pair<void*, Bytes> &data)             __NE___;

        template <typename Tag, typename Comp>
        ND_ bool  Add (EntityID id, const Comp& comp)                                               __NE___;

        template <typename Tag>
        ND_ bool  AddMulti (ComponentID compId, ArrayView<EntityID> ids, ArrayView<ubyte> data)     __NE___;

        template <typename Tag>
        ND_ bool  AddMulti (ComponentID compId, ArrayView<EntityID> ids)                            __NE___;

        template <typename Comp, typename Tag, typename Fn>
        ND_ bool  AddListener (Fn &&fn)                                                             __NE___;

        template <typename Tag>
        ND_ bool  HasListener (ComponentID compId)                                                  C_NE___;

            void  Process ()                                                                        __NE___;
    };




    inline MessageBuilder::MessageKey::MessageKey (ComponentID compId, MsgTagID tagId) __NE___ :
        _value{ (usize(compId.value) << 16) | usize(tagId.value) }
    {}

    inline bool  MessageBuilder::MessageKey::operator == (const MessageKey &rhs) C_NE___
    {
        return _value == rhs._value;
    }
//-----------------------------------------------------------------------------


    inline usize  MessageBuilder::MessageKeyHash::operator () (const MessageKey &x) C_NE___
    {
        return x._value;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    Add
=================================================
*/
    template <typename Tag>
    bool  MessageBuilder::Add (EntityID id, ComponentID compId) __NE___
    {
        MessageKey const    key  { compId, MsgTagTypeInfo<Tag>::id };
        auto                iter = _msgTypes.find( key );

        // no listener to process this message
        if ( iter == _msgTypes.end() )
            return true;

        auto&   msg = iter->second;

        if ( msg.entities.empty() )
            NOTHROW_ERR( _pending.push_back( &msg ));

        ASSERT( msg.components.empty() );

        NOTHROW_ERR( msg.entities.push_back( id ));
        return true;
    }

/*
=================================================
    Add
=================================================
*/
    template <typename Tag>
    bool  MessageBuilder::Add (EntityID id, ComponentID compId, ArrayView<ubyte> comp) __NE___
    {
        MessageKey const    key  { compId, MsgTagTypeInfo<Tag>::id };
        auto                iter = _msgTypes.find( key );

        // no listener to process this message
        if ( iter == _msgTypes.end() )
            return true;

        auto&   msg = iter->second;

        if ( msg.entities.empty() )
            NOTHROW_ERR( _pending.push_back( &msg ));

        ASSERT( msg.entities.empty() or not msg.components.empty() );
        ASSERT( comp.size() );

        NOTHROW_ERR( msg.components.resize( (msg.entities.size() + 1) * comp.size() ));
        MemCopy( OUT msg.components.data() + Bytes{msg.entities.size() * comp.size()}, comp.data(), ArraySizeOf( comp ));

        NOTHROW_ERR( msg.entities.push_back( id ));
        return true;
    }

    template <typename Tag, typename Comp>
    bool  MessageBuilder::Add (EntityID id, const Comp& comp) __NE___
    {
        return Add<Tag>( id, ComponentTypeInfo<Comp>::id, ArrayView<ubyte>{ Cast<ubyte>(&comp), sizeof(comp) });
    }

    template <typename Tag>
    bool  MessageBuilder::Add (EntityID id, ComponentID compId, const Pair<void*, Bytes> &data) __NE___
    {
        ASSERT( data.first );
        return Add<Tag>( id, compId, ArrayView<ubyte>{ Cast<ubyte>(data.first), usize(data.second) });
    }

/*
=================================================
    AddListener
=================================================
*/
    template <typename Comp, typename Tag, typename Fn>
    bool  MessageBuilder::AddListener (Fn &&fn) __NE___
    {
        using FI = FunctionInfo< Fn >;

        MessageKey  key{ ComponentTypeInfo<Comp>::id, MsgTagTypeInfo<Tag>::id };
        auto&       msg = _msgTypes[ key ];

        if constexpr( FI::args::Count == 1 )
        {
            StaticAssert( IsSameTypes<typename FI::args::template Get<0>, ArrayView<EntityID>> );
            //StaticAssert( not IsSameTypes< Tag, MsgTag_RemovedComponent >);
            NOTHROW_ERR(
                msg.listeners.push_back(
                    [fn = FwdArg<Fn>(fn)] (const MessageData &data)
                    {
                        fn( ArrayView<EntityID>{ data.entities });
                    }));
            return true;
        }

        if constexpr( FI::args::Count == 2 )
        {
            StaticAssert( IsSameTypes<typename FI::args::template Get<0>, ArrayView<EntityID>> );
            StaticAssert( IsSameTypes<typename FI::args::template Get<1>, ArrayView<Comp>> );
            NOTHROW_ERR(
                msg.listeners.push_back(
                    [fn = FwdArg<Fn>(fn)] (const MessageData &data)
                    {
                        ASSERT( data.components.size() );
                        fn( ArrayView<EntityID>{ data.entities },
                            ArrayView<Comp>{ Cast<Comp>(data.components.data()), data.entities.size() });
                    }));
            return true;
        }
    }

/*
=================================================
    AddMulti
=================================================
*/
    template <typename Tag>
    bool  MessageBuilder::AddMulti (ComponentID compId, ArrayView<EntityID> ids, ArrayView<ubyte> compData) __NE___
    {
        MessageKey const    key  { compId, MsgTagTypeInfo<Tag>::id };
        auto                iter = _msgTypes.find( key );

        // no listener to process this message
        if ( iter == _msgTypes.end() )
            return true;

        auto&   msg = iter->second;

        if ( msg.entities.empty() )
            NOTHROW_ERR( _pending.push_back( &msg ));

        ASSERT( msg.entities.empty() or not msg.components.empty() );
        ASSERT( compData.size() );
        ASSERT( ids.size() );

        const usize comp_size = compData.size() / ids.size();
        NOTHROW_ERR( msg.components.resize( (msg.entities.size() + 1) * comp_size ));
        MemCopy( OUT msg.components.data() + Bytes{msg.entities.size() * comp_size}, compData.data(), ArraySizeOf( compData ));

        NOTHROW_ERR( msg.entities.insert( msg.entities.end(), ids.begin(), ids.end() ));
        return true;
    }

/*
=================================================
    AddMulti
=================================================
*/
    template <typename Tag>
    bool  MessageBuilder::AddMulti (ComponentID compId, ArrayView<EntityID> ids) __NE___
    {
        MessageKey const    key  { compId, MsgTagTypeInfo<Tag>::id };
        auto                iter = _msgTypes.find( key );

        // no listener to process this message
        if ( iter == _msgTypes.end() )
            return true;

        auto&   msg = iter->second;

        if ( msg.entities.empty() )
            NOTHROW_ERR( _pending.push_back( &msg ));

        ASSERT( msg.components.empty() );

        NOTHROW_ERR( msg.entities.insert( msg.entities.end(), ids.begin(), ids.end() ));
        return true;
    }

/*
=================================================
    HasListener
=================================================
*/
    template <typename Tag>
    bool  MessageBuilder::HasListener (ComponentID compId) C_NE___
    {
        MessageKey  key{ compId, MsgTagTypeInfo<Tag>::id };
        auto        iter = _msgTypes.find( key );

        return iter != _msgTypes.end();
    }

/*
=================================================
    Process
=================================================
*/
    inline void  MessageBuilder::Process () __NE___
    {
        for (auto* msg : _pending)
        {
            for (auto& ml : msg->listeners)
            {
                ml( *msg );
            }

            msg->components.clear();
            msg->entities.clear();
        }

        _pending.clear();
    }

} // AE::ECS
