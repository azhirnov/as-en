// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe:  yes
		- used split lock: CreateMsg() - lock_shared, AddMessage() - unlock_shared.
		- Produce() use exclusive lock.
*/

#pragma once

#include "networking/Utils/MsgAndSync.h"

namespace AE::Networking
{

	//
	// Async Client/Server Message Producer
	//

	template <typename AllocatorType>
	class alignas(AE_CACHE_LINE) AsyncCSMessageProducer : public ICSMessageProducer
	{
		StaticAssert( IsThreadSafeAllocator< AllocatorType >);

	// types
	public:
		template <typename T>
		using Msg				= Networking::_hidden_::MsgAndSync<T>;
	private:
		using _SpinLock_t		= Networking::_hidden_::MsgAndSync_SpinLock_t;
		using Allocator_t		= AllocatorType;
		using DoubleBufAlloc_t	= StaticArray< RC<Allocator_t>, 2 >;
		using LfMessageList_t	= Threading::LfChunkList< CSMessagePtr, NetConfig::MsgPerChunk >;


	// variables
	private:
		_SpinLock_t			_guard;
		Atomic<uint>		_index		{0};
		LfMessageList_t		_outputMsg;
		DoubleBufAlloc_t	_dbAlloc;


	// methods
	public:
		template <typename A>
		AsyncCSMessageProducer (Tag<A>)										__NE___	: AsyncCSMessageProducer{ MakeRC<A>(), MakeRC<A>() } {}
		AsyncCSMessageProducer ()											__NE___	: AsyncCSMessageProducer{ MakeRC<Allocator_t>(), MakeRC<Allocator_t>() } {}
		AsyncCSMessageProducer (RC<Allocator_t>, RC<Allocator_t>)			__NE___;
		~AsyncCSMessageProducer ()											__NE___	{ Unused( _outputMsg.Release() ); }


		template <typename T>
		ND_ Msg<T>	CreateMsg (Bytes extraSize = 0_b)						__NE___	{ return CreateMsg<T>( Default, extraSize ); }

		template <typename T>
		ND_ Msg<T>	CreateMsg (EClientLocalID, Bytes extraSize = 0_b)		__NE___;

		// non blocking
		template <typename T>
		ND_ Msg<T>	CreateMsgOpt (Bytes extraSize = 0_b)					__NE___	{ return CreateMsgOpt<T>( Default, extraSize ); }

		template <typename T>
		ND_ Msg<T>	CreateMsgOpt (EClientLocalID, Bytes extraSize = 0_b)	__NE___;


		template <typename T>
		ND_ bool	AddMessage (Msg<T> &)									__NE___;

		// safe to use between 'CreateMsg()' and 'AddMessage()'
		ND_ Allocator_t&	GetAllocator ()									__NE___	{ ASSERT( _guard.is_shared_locked() );  return _GetAllocator(); }


	private:
		// ICSMessageProducer //
		ChunkList<CSMessagePtr>  Produce (FrameUID frameId)					__NE_OF;

		ND_ Allocator_t&	_GetAllocator ()								__NE___	{ return *_dbAlloc[ _index.load() & 1 ]; }
	};


	using IAsyncCSMessageProducer = AsyncCSMessageProducer< IAllocatorTS >;
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <typename A>
	AsyncCSMessageProducer<A>::AsyncCSMessageProducer (RC<Allocator_t> a0, RC<Allocator_t> a1) __NE___ :
		_dbAlloc{ RVRef(a0), RVRef(a1) }
	{
		CHECK( _dbAlloc[0] and _dbAlloc[1] );
		CHECK( _outputMsg.Init( _GetAllocator() ));
	}

/*
=================================================
	CreateMsg
=================================================
*/
	template <typename A>
	template <typename T>
	typename AsyncCSMessageProducer<A>::template Msg<T>  AsyncCSMessageProducer<A>::CreateMsg (EClientLocalID clientId, Bytes extraSize) __NE___
	{
		_guard.lock_shared();
		return Msg<T>{ CSMessageCtor<T>::CreateForEncode( _GetAllocator(), clientId, extraSize ), _guard, extraSize };
	}

	template <typename A>
	template <typename T>
	typename AsyncCSMessageProducer<A>::template Msg<T>  AsyncCSMessageProducer<A>::CreateMsgOpt (EClientLocalID clientId, Bytes extraSize) __NE___
	{
		if_unlikely( not _guard.try_lock_shared( 200 ))
			return Default;

		return Msg<T>{ CSMessageCtor<T>::CreateForEncode( _GetAllocator(), clientId, extraSize ), _guard, extraSize };
	}

/*
=================================================
	AddMessage
=================================================
*/
	template <typename A>
	template <typename T>
	bool  AsyncCSMessageProducer<A>::AddMessage (Msg<T> &msg) __NE___
	{
		ASSERT( msg );
		bool	res = _outputMsg.Emplace( _GetAllocator(), CSMessagePtr{msg} );

		msg._Unlock();
		return res;
	}

/*
=================================================
	Produce
=================================================
*/
	template <typename A>
	ChunkList<CSMessagePtr>  AsyncCSMessageProducer<A>::Produce (const FrameUID frameId) __NE___
	{
		const uint					new_fid = frameId.Remap2();
		ChunkList<CSMessagePtr>		result;
		{
			EXLOCK( _guard );

			const uint	bits		= _index.load();
			const uint	prev_id		= bits & 1;
			const uint	prev_fid	= (bits >> 2) & 1;

			if ( new_fid != prev_fid )
			{
				// New 'frameId' indicates that all previous messages are sent and memory can be reused.
				const uint	id = (new_fid == prev_fid ? prev_id : ((prev_id+1) & 1));

				_index.store( id | (new_fid << 2) );
				_dbAlloc[id]->Discard();
			}

			result = _outputMsg.Release();
			CHECK( _outputMsg.Init( _GetAllocator() ));
		}
		return result;
	}


} // AE::Networking
