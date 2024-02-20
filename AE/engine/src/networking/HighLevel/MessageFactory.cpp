// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/MessageFactory.h"

namespace AE::Networking
{

/*
=================================================
    constructor
=================================================
*/
    MessageFactory::MessageFactory () __NE___
    {
        for (auto& alloc : _dbAlloc) {
            alloc = MakeRC< Allocator_t >();
        }
    }

/*
=================================================
    Register
=================================================
*/
    bool  MessageFactory::Register (const CSMessageUID uid, CreateAndDecode_t ctor) __NE___
    {
        auto [group_id, msg_id] = CSMessage::_UnpackUID( uid );

        auto&   group = _msgTypes[group_id];

        EXLOCK( group.guard );
        CHECK_ERR( not group.immutable );

        return group.map.emplace( msg_id, ctor ).second;
    }

    bool  MessageFactory::Register (const CSMessageGroupID groupId, ArrayView<MsgAndCtor_t> arr, Bool lockGroup) __NE___
    {
        auto&   group = _msgTypes[ uint(groupId) ];

        EXLOCK( group.guard );
        CHECK_ERR( not group.immutable );

        for (auto& [uid, ctor] : arr)
        {
            auto [group_id, msg_id] = CSMessage::_UnpackUID( uid );
            CHECK_ERR( uint(groupId) == group_id );
            CHECK_ERR( group.map.emplace( msg_id, ctor ).second );
        }

        if ( lockGroup )
            group.immutable = true;

        return true;
    }

/*
=================================================
    DeserializeMsg
=================================================
*/
    bool  MessageFactory::DeserializeMsg (const FrameUID frameId, const CSMessageUID uid, EClientLocalID cid, OUT MsgPtr_t &msg, DataDecoder &des) __NE___
    {
        auto [group_id, msg_id] = CSMessage::_UnpackUID( uid );

        CreateAndDecode_t   fn = null;
        {
            auto&   group = _msgTypes[group_id];

            SHAREDLOCK( group.guard );
            ASSERT( group.immutable );

            auto    it = group.map.find( msg_id );
            CHECK_ERR( it != group.map.end() );

            fn = it->second;
        }

        return fn( OUT msg, GetAllocator( frameId ), cid, des );
    }

/*
=================================================
    NextFrame
=================================================
*/
    void  MessageFactory::NextFrame (const FrameUID frameId) __NE___
    {
        const uint  id = frameId.Remap2();

        _dbAlloc[id]->Discard();

        DEBUG_ONLY(
            _dbFrameId[id].store( frameId );
        )
    }


} // AE::Networking
