// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Graphics
{

	//
	// Lock-free Command Buffer Pool
	//

	template <typename NativeCmdBufferType,
			  typename BakedCommandsType
			 >
	struct LfCmdBufferPool
	{
	// types
	public:
		using NativeCmdBuffer_t		= NativeCmdBufferType;
		using BakedCommands_t		= BakedCommandsType;

		STATIC_ASSERT( sizeof(NativeCmdBuffer_t) >= sizeof(BakedCommands_t) );

		union Cmdbuf
		{
			NativeCmdBuffer_t		native;
			BakedCommands_t			baked;

			Cmdbuf () {}
		};
		using Pool_t = StaticArray< Cmdbuf, GraphicsConfig::MaxCmdBufPerBatch >;


	// variables
	protected:
		Atomic<uint>	_ready;		// 1 - commands recording have been completed and added to pool
		Atomic<uint>	_cmdTypes;	// 0 - vulkan cmd buffer, 1 - backed commands
		Atomic<uint>	_counter;	// index in '_pool'
		uint			_count;		// number of commands in '_pool'
		Pool_t			_pool;

		DRC_ONLY( RWDataRaceCheck	_drCheck; )	// for '_pool' and '_count'


	// methods
	public:
		LfCmdBufferPool ();

		// user api (thread safe)
		ND_ uint  Current ()	const	{ return _counter.load(); }
		ND_ uint  Acquire ();
			void  Add (INOUT uint& idx, NativeCmdBuffer_t cmdbuf);
			void  Add (INOUT uint& idx, BakedCommands_t ctx);
			void  Complete (INOUT uint& idx);

		// owner api
			void  Lock ();
			void  Reset ();
		ND_ bool  IsReady ();
		ND_ bool  IsLocked ();
			
	protected:
			void  _GetCommands (OUT NativeCmdBuffer_t* cmdbufs, OUT uint &cmdbufCount, uint maxCount);
	};
	

/*
=================================================
	constructor
=================================================
*/
	template <typename A, typename B>
	LfCmdBufferPool<A,B>::LfCmdBufferPool ()
	{
		Reset();
	}

/*
=================================================
	Acquire
----
	thread safe
=================================================
*/
	template <typename A, typename B>
	uint  LfCmdBufferPool<A,B>::Acquire ()
	{
		ASSERT( not IsReady() );

		uint	idx = _counter.fetch_add( 1 );
		if_likely( idx < _pool.size() )
			return idx;

		DEV_WARNING( "counter overflow" );
		return UMax;
	}
	
/*
=================================================
	Add
----
	thread safe for unique id
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::Add (INOUT uint& idx, NativeCmdBuffer_t cmdbuf)
	{
		ASSERT( idx < _pool.size() );

		if_likely( idx < _pool.size() )
		{
			DRC_SHAREDLOCK( _drCheck );

			_pool[idx].native = cmdbuf;

			uint	old_bits = _ready.fetch_or( 1u << idx, EMemoryOrder::Release );
			ASSERT( (old_bits & (1u << idx)) == 0 );
			Unused( old_bits );
		}
		idx = UMax;
	}
	
/*
=================================================
	Add
----
	thread safe for unique id
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::Add (INOUT uint& idx, BakedCommands_t ctx)
	{
		ASSERT( idx < _pool.size() );

		if_likely( idx < _pool.size() )
		{
			DRC_SHAREDLOCK( _drCheck );

			PlacementNew<BakedCommands_t>( &_pool[idx].baked, RVRef(ctx) );
			_cmdTypes.fetch_or( 1u << idx );

			uint	old_bits = _ready.fetch_or( 1u << idx, EMemoryOrder::Release );
			ASSERT( (old_bits & (1u << idx)) == 0 );
			Unused( old_bits );
		}
		idx = UMax;
	}
	
/*
=================================================
	Complete
----
	thread safe for unique id
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::Complete (INOUT uint& idx)
	{
		ASSERT( idx < _pool.size() );

		if_likely( idx < _pool.size() )
		{
			uint	old_bits = _ready.fetch_or( 1u << idx );
			ASSERT( (old_bits & (1u << idx)) == 0 );
			Unused( old_bits );
		}
		idx = UMax;
	}

/*
=================================================
	Lock
----
	call once.
	not thread safe !!!
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::Lock ()
	{
		DRC_SHAREDLOCK( _drCheck );

		const uint	count = _counter.exchange( uint(_pool.size()) );
		_count = Min( count, uint(_pool.size()) );

		// set unused bits to 1
		uint	mask = ~ToBitMask<uint>(_count);
		if ( mask )
			_ready.fetch_or( mask );
	}

/*
=================================================
	Reset
----
	unlock and reset.
	not thread safe !!!
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::Reset ()
	{
		DRC_EXLOCK( _drCheck );

		DEBUG_ONLY({
			uint	types = _cmdTypes.load();
			while ( types != 0 )
			{
				int	i = ExtractBitLog2( INOUT types );
				ASSERT( not _pool[i].baked.IsValid() );
			}
		})

		ZeroMem( _pool.data(), Bytes::SizeOf(_pool) );
		_count = 0;
		
		_counter.store( 0 );
		_ready.store( 0 );
		_cmdTypes.store( 0 );

		ThreadFence( EMemoryOrder::Release );
	}
	
/*
=================================================
	IsReady
=================================================
*/
	template <typename A, typename B>
	bool  LfCmdBufferPool<A,B>::IsReady ()
	{
		return _ready.load() == UMax;
	}

/*
=================================================
	_GetCommands
----
	not thread safe !!!
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::_GetCommands (OUT NativeCmdBuffer_t* cmdbufs, OUT uint &cmdbufCount, uint maxCount)
	{
		ASSERT( cmdbufs != null );
		ASSERT( maxCount >= GraphicsConfig::MaxCmdBufPerBatch );
		Unused( maxCount );

		cmdbufCount = 0;

		DRC_EXLOCK( _drCheck );
		ThreadFence( EMemoryOrder::Acquire );

		ASSERT( _cmdTypes.load() == 0 );	// software command buffers is not supported here
		ASSERT( IsReady() );

		for (uint i = 0; i < _count; ++i)
		{
			// command buffer can be null
			if_likely( _pool[i].native != NativeCmdBuffer_t{} )
			{
				ASSERT( cmdbufCount < maxCount );
				cmdbufs[cmdbufCount++] = _pool[i].native;
			}
		}

		DEBUG_ONLY( Reset() );
	}


} // AE::Graphics
