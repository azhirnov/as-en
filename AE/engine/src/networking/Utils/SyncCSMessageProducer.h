// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

#pragma once

#include "networking/Utils/MsgAndSync.h"

namespace AE::Networking
{

	//
	// Client/Server Message Producer
	//

	template <typename AllocatorType>
	class SyncCSMessageProducer : public ICSMessageProducer
	{
	// types
	public:
		template <typename T>
		using Msg				= Networking::_hidden_::MsgWithExtra<T>;
	private:
		using _SpinLock_t		= Networking::_hidden_::MsgAndSync_SpinLock_t;
		using Allocator_t		= AllocatorType;
		using DoubleBufAlloc_t	= StaticArray< Allocator_t, 2 >;
		using LfMessageList_t	= Threading::LfChunkList< CSMessagePtr, NetConfig::MsgPerChunk >;


	// variables
	private:
		Atomic<uint>		_index		{0};
		LfMessageList_t		_outputMsg;
		DoubleBufAlloc_t	_dbAlloc;


	// methods
	public:
		SyncCSMessageProducer ()										__NE___;
		~SyncCSMessageProducer ()										__NE___;

		template <typename T>
		ND_ Msg<T>	CreateMsg (Bytes extraSize = 0_b)					__NE___	{ return CreateMsg<T>( Default, extraSize ); }

		template <typename T>
		ND_ Msg<T>	CreateMsg (EClientLocalID, Bytes extraSize = 0_b)	__NE___;

		template <typename T>
		ND_ bool	AddMessage (Msg<T> &)								__NE___;

	private:
		// ICSMessageProducer //
		ChunkList<CSMessagePtr>  Produce (FrameUID frameId)				__NE_OF;

		ND_ Allocator_t&	_GetAllocator ()							__NE___	{ return _dbAlloc[ _index.load() & 1 ]; }
	};



/*
=================================================
	constructor
=================================================
*/
	template <typename A>
	SyncCSMessageProducer<A>::SyncCSMessageProducer () __NE___
	{
		CHECK( _outputMsg.Init( _GetAllocator() ));
	}

/*
=================================================
	destructor
=================================================
*/
	template <typename A>
	SyncCSMessageProducer<A>::~SyncCSMessageProducer () __NE___
	{
		Unused( _outputMsg.Release() );
		for (auto& alloc : _dbAlloc)
			alloc.Discard();
	}

/*
=================================================
	CreateMsg
=================================================
*/
	template <typename A>
	template <typename T>
	typename SyncCSMessageProducer<A>::template Msg<T>  SyncCSMessageProducer<A>::CreateMsg (EClientLocalID clientId, Bytes extraSize) __NE___
	{
		return Msg<T>{ CSMessageCtor<T>::CreateForEncode( _GetAllocator(), clientId, extraSize ), extraSize };
	}

/*
=================================================
	AddMessage
=================================================
*/
	template <typename A>
	template <typename T>
	bool  SyncCSMessageProducer<A>::AddMessage (Msg<T> &msg) __NE___
	{
		ASSERT( msg );
		return _outputMsg.Emplace( _GetAllocator(), CSMessagePtr{msg} );
	}

/*
=================================================
	Produce
=================================================
*/
	template <typename A>
	ChunkList<CSMessagePtr>  SyncCSMessageProducer<A>::Produce (const FrameUID frameId) __NE___
	{
		const uint					new_fid = frameId.Remap2();
		ChunkList<CSMessagePtr>		result;
		{
			const uint	bits		= _index.load();
			const uint	prev_id		= bits & 1;
			const uint	prev_fid	= (bits >> 2) & 1;

			if ( new_fid != prev_fid )
			{
				// New 'frameId' indicates that all previous messages are sent and memory can be reused.
				const uint	id = (new_fid == prev_fid ? prev_id : ((prev_id+1) & 1));

				_index.store( id | (new_fid << 2) );
				_dbAlloc[id].Discard();
			}

			result = _outputMsg.Release();
			CHECK( _outputMsg.Init( _GetAllocator() ));
		}
		return result;
	}


} // AE::Networking
