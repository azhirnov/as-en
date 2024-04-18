// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/ClientServerBase.h"

namespace AE::Networking
{

/*
=================================================
	_Initialize2
=================================================
*/
	bool  ClientServerBase::_Initialize2 (RC<MessageFactory>	mf,
										  RC<IAllocator>		alloc,
										  FrameUID				firstFrameId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( mf );
		CHECK_ERR( not _IsInitialized2() );  // already initialized

		_prevFrameId	= firstFrameId;
		_msgFactory		= RVRef(mf);
		_allocator		= RVRef(alloc);

		if ( not _allocator )
			_allocator = AE::GetDefaultAllocator();

		return true;
	}

/*
=================================================
	_Deinitialize2
=================================================
*/
	void  ClientServerBase::_Deinitialize2 () __NE___
	{
		DRC_EXLOCK( _drCheck );

		_prevFrameId	= Default;
		_msgFactory		= null;
		_allocator		= null;
		_channels.fill( null );
		_producers.fill( Default );
		_consumers.clear();
	}

/*
=================================================
	Update
=================================================
*/
	ClientServerBase::MsgQueueStatistic  ClientServerBase::Update (const FrameUID frameId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		ASSERT( _IsInitialized2() );

		// frame index must be the same or increase monotonously
		ASSERT( frameId == _prevFrameId		or
				frameId == _prevFrameId.Next()	);

		if ( frameId > _prevFrameId )
		{
			_msgFactory->NextFrame( frameId );
			_prevFrameId = frameId;
		}

		// produce outcoming messages
		for (uint i = 0; i < uint(EChannel::_Count); ++i)
		{
			auto&	channel = _channels[i];
			if ( not channel )
				continue;

			for (auto& producer : _producers[i])
			{
				auto	msg_list = producer->Produce( frameId );
				if ( not msg_list.empty() )
					channel->Send( msg_list );
			}
		}

		// send & receive
		MsgQueueStatistic	stat;

		for (auto& ch : _channels)
		{
			if ( ch )
				ch->ProcessMessages( frameId, INOUT stat );
		}

		// consume incoming messages
		for (auto& ch : _channels)
		{
			if ( not ch )
				continue;

			auto&	msg_map = ch->Receive();
			for (auto [id, list] : msg_map)
			{
				if_unlikely( list.first.empty() or list.first.FirstChunk()->IsEmpty() )
					continue;	// TODO: empty only if memory is out

				auto	it = _consumers.find( id );
				if_likely( it != _consumers.end() )
				{
					for (auto& consumer : it->second)
						consumer->Consume( list.first );
				}
				else
				{
					AE_LOG_DBG( "messages in group ("s << ToString(uint(id)) << ") is not consumed" );
				}
			}
			msg_map.clear();
		}

		return stat;
	}

/*
=================================================
	Send
=================================================
*/
	void  ClientServerBase::Send (EChannel type, MsgList_t msgList) __NE___
	{
		if_unlikely( auto* first = msgList.FirstChunk();  first == null or first->IsEmpty() )
			return;

		DRC_EXLOCK( _drCheck );

		ASSERT( _IsInitialized2() );
		CHECK_ERRV( uint(type) < _channels.size() );
		CHECK_ERRV( _channels[ uint(type) ] );

		_channels[ uint(type) ]->Send( msgList );
	}

/*
=================================================
	Add
=================================================
*/
	bool  ClientServerBase::Add (RC<ICSMessageProducer> producer) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( producer );

		for (EChannel type : producer->GetChannels())
		{
			CHECK_ERR( uint(type) < _producers.size() );
			CHECK_ERR( _channels[ uint(type) ] );

			_producers[ uint(type) ].push_back( producer );  // throw
		}
		return true;
	}

/*
=================================================
	Remove
=================================================
*/
	void  ClientServerBase::Remove (const ICSMessageProducer* producer) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERRV( producer != null );

		for (auto& per_channel : _producers)
		{
			EraseIfEqual( INOUT per_channel, producer );
		}
	}

/*
=================================================
	RemoveAllProducers
=================================================
*/
	void  ClientServerBase::RemoveAllProducers () __NE___
	{
		DRC_EXLOCK( _drCheck );

		for (auto& p : _producers)
			p.clear();
	}

/*
=================================================
	Add
=================================================
*/
	bool  ClientServerBase::Add (RC<ICSMessageConsumer> consumer) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( consumer );
		CHECK_ERR( _consumers.size() < _consumers.capacity() );

		const CSMessageGroupID	group_id = consumer->GetGroupID();

		auto&	arr = _consumers( group_id );
		arr.push_back( RVRef(consumer) );  // throw

		return true;
	}

/*
=================================================
	Remove
=================================================
*/
	void  ClientServerBase::Remove (const ICSMessageConsumer* consumer) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERRV( consumer != null );

		const CSMessageGroupID	group_id = consumer->GetGroupID();

		auto	it = _consumers.find( group_id );
		if ( it != _consumers.end() )
		{
			EraseIfEqual( INOUT it->second, consumer );
		}
	}

/*
=================================================
	RemoveAll
=================================================
*/
	void  ClientServerBase::RemoveAll (CSMessageGroupID groupId) __NE___
	{
		DRC_EXLOCK( _drCheck );
		_consumers.EraseByKey( groupId );
	}

/*
=================================================
	RemoveAllConsumers
=================================================
*/
	void  ClientServerBase::RemoveAllConsumers () __NE___
	{
		DRC_EXLOCK( _drCheck );
		_consumers.clear();
	}


} // AE::Networking
