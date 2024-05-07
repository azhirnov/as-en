// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	NativeCmdBufferType - trivial type which initialized by zero/null and supports ZeroMem().
	BakedCommandsType	- trivial type which default state may be non-zero.
*/

#pragma once

namespace AE::Graphics
{
	//
	// Command buffer execution index
	//
	struct CmdBufExeIndex
	{
	// types
	public:
		enum class EMode : ubyte
		{
			FrontToBack,
			BackToFront,
			Exact,
			_Count
		};
	private:
		static constexpr uint	_MaxIdx = GraphicsConfig::MaxCmdBufPerBatch;

	// variables
	private:
		uint	_packed;

	// methods
	public:
		constexpr CmdBufExeIndex ()												__NE___ : CmdBufExeIndex{ 0, EMode::FrontToBack } {}
		constexpr CmdBufExeIndex (CmdBufExeIndex &&)							__NE___	= default;
		constexpr CmdBufExeIndex (const CmdBufExeIndex &)						__NE___	= default;
		constexpr CmdBufExeIndex (uint idx, EMode mode)							__NE___	: _packed{ uint(mode) | (idx << 8) } {}

		constexpr CmdBufExeIndex&  operator = (const CmdBufExeIndex &)			__NE___ = default;
		constexpr CmdBufExeIndex&  operator = (CmdBufExeIndex &&)				__NE___ = default;

		ND_ constexpr EMode		Mode ()											C_NE___	{ return EMode(_packed & 0xFF); }
		ND_ constexpr uint		Index ()										C_NE___	{ return _packed >> 8; }

		ND_ static constexpr CmdBufExeIndex  SearchFrontToBack (uint front)		__NE___	{ ASSERT_Lt( front,  _MaxIdx );  return CmdBufExeIndex{ front, EMode::FrontToBack }; }
		ND_ static constexpr CmdBufExeIndex  SearchBackToFront (uint back)		__NE___	{ ASSERT_Lt( back,   _MaxIdx );  return CmdBufExeIndex{ back,  EMode::BackToFront }; }
		ND_ static constexpr CmdBufExeIndex  Exact (uint index)					__NE___	{ ASSERT_Lt( index,  _MaxIdx );  return CmdBufExeIndex{ index, EMode::Exact }; }
		ND_ static constexpr CmdBufExeIndex  Anywhere ()						__NE___	{ return CmdBufExeIndex{}; }

		template <uint Front = 0>
		ND_ static constexpr CmdBufExeIndex  SearchFrontToBack ()				__NE___	{ StaticAssert( Front < _MaxIdx );  return CmdBufExeIndex{ Front, EMode::FrontToBack }; }
		template <uint Back = _MaxIdx-1>
		ND_ static constexpr CmdBufExeIndex  SearchBackToFront ()				__NE___	{ StaticAssert( Back  < _MaxIdx );  return CmdBufExeIndex{ Back,  EMode::BackToFront }; }
		template <uint Middle = _MaxIdx/2>
		ND_ static constexpr CmdBufExeIndex  SearchMiddleToFront ()				__NE___	{ return SearchBackToFront<Middle>(); }
		template <uint Middle = _MaxIdx/2>
		ND_ static constexpr CmdBufExeIndex  SearchMiddleToBack ()				__NE___	{ return SearchFrontToBack<Middle>(); }
		template <uint Index>
		ND_ static constexpr CmdBufExeIndex  Exact ()							__NE___	{ StaticAssert( Index < _MaxIdx );  return CmdBufExeIndex{ Index, EMode::Exact }; }
	};



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
		using Bits_t				= uint;

		StaticAssert( sizeof(NativeCmdBuffer_t) >= sizeof(BakedCommands_t) );
		StaticAssert( not IsSameTypes< NativeCmdBuffer_t, BakedCommands_t >);
		StaticAssert( IsZeroMemAvailable< NativeCmdBuffer_t >);
		StaticAssert( IsTriviallyDestructible< NativeCmdBuffer_t >);
		StaticAssert( IsTriviallyDestructible< BakedCommands_t >);

		// 1 bit reserved to indicate lock, see 'Lock()'
		StaticAssert( GraphicsConfig::MaxCmdBufPerBatch < CT_SizeOfInBits<Bits_t> );

		union Cmdbuf
		{
			NativeCmdBuffer_t		native;
			BakedCommands_t			baked;

			Cmdbuf () __NE___ {}
		};
		using Pool_t = StaticArray< Cmdbuf, GraphicsConfig::MaxCmdBufPerBatch >;


	// variables
	protected:
		Atomic<Bits_t>		_ready;			// 1 - command buffer is added to the '_pool' slot
		Atomic<Bits_t>		_cmdTypes;		// 0 - native cmd buffer, 1 - backed commands
		Atomic<Bits_t>		_available;		// free slots in '_pool', 0 - available
		Atomic<Bits_t>		_usedSlots;		// indices of used slots in '_pool'
		Pool_t				_pool;

		DEBUG_ONLY(
			mutable Atomic<Bits_t>	_usedSlotsUsedInIsFirst;
			mutable Atomic<Bits_t>	_usedSlotsUsedInIsLast;
		)
		DRC_ONLY( RWDataRaceCheck	_drCheck; )	// for '_pool'


	// methods
	public:
		LfCmdBufferPool ()											__NE___	{ Reset(); }

		// user api (thread safe)
		ND_ bool  IsEmpty ()										C_NE___	{ return _available.load() == 0; }
		ND_ bool  IsFirst (uint index)								C_NE___;
		ND_ bool  IsLast (uint index)								C_NE___;

		ND_ uint  Acquire (CmdBufExeIndex)							__NE___;
			void  Add (INOUT uint& idx, NativeCmdBuffer_t cmdbuf)	__NE___;
			void  Add (INOUT uint& idx, BakedCommands_t ctx)		__NE___;
			void  Complete (INOUT uint& idx)						__NE___	{ Add( INOUT idx, NativeCmdBuffer_t{} ); }

		// owner api
			void  Lock ()											__NE___;
			void  Reset ()											__NE___;
		ND_ bool  IsReady ()										C_NE___	{ return _ready.load() == UMax; }
		ND_ bool  IsLocked ()										C_NE___	{ return _available.load() == UMax; }

	protected:
		ND_ uint  _Acquire_Search (int requiredIdx, int step)		__NE___;
		ND_ uint  _Acquire_Exact (uint)								__NE___;

		template <typename T, typename FN>
			void  _GetNativeCommands (OUT T* cmdbufs, OUT uint &cmdbufCount, uint maxCount, FN &&fn)		__NE___;
			void  _GetNativeCommands (OUT NativeCmdBuffer_t* cmdbufs, OUT uint &cmdbufCount, uint maxCount)	__NE___;

		template <typename T, typename FN>
		ND_ bool  _GetBakedCommands (OUT T* cmdbufs, OUT uint &cmdbufCount, uint maxCount, FN &&fn)			__NE___;
	};


/*
=================================================
	Acquire
----
	thread safe
=================================================
*/
	template <typename A, typename B>
	uint  LfCmdBufferPool<A,B>::Acquire (CmdBufExeIndex exeIndex) __NE___
	{
		ASSERT( not IsReady() );

		const auto	mode	= exeIndex.Mode();
		const auto	idx		= exeIndex.Index();

		if_likely( AnyEqual( mode, CmdBufExeIndex::EMode::FrontToBack, CmdBufExeIndex::EMode::BackToFront ))
			return _Acquire_Search( int(idx), mode == CmdBufExeIndex::EMode::FrontToBack ? 1 : -1 );

		if ( mode == CmdBufExeIndex::EMode::Exact )
			return _Acquire_Exact( idx );

		StaticAssert( uint(CmdBufExeIndex::EMode::_Count) == 3 );
		DBG_WARNING( "unsupported mode" );

		return UMax;
	}

/*
=================================================
	_Acquire_Search
=================================================
*/
	template <typename A, typename B>
	uint  LfCmdBufferPool<A,B>::_Acquire_Search (int requiredIdx, int step) __NE___
	{
		const int	max_size	= int(GraphicsConfig::MaxCmdBufPerBatch);
		auto		available	= _available.load();

		for (; (requiredIdx >= 0) and (requiredIdx < max_size);)
		{
			const auto	bit = Bits_t{1} << requiredIdx;

			if ( available & bit )
			{
				requiredIdx += step;
				continue;
			}

			if_likely( _available.CAS( INOUT available, bit | available ))	// 0 -> 1
			{
				_usedSlots.fetch_or( bit );
				return uint(requiredIdx);
			}
		}

		DBG_WARNING( "pool overflow" );
		return UMax;
	}

/*
=================================================
	_Acquire_Exact
=================================================
*/
	template <typename A, typename B>
	uint  LfCmdBufferPool<A,B>::_Acquire_Exact (uint requiredIdx) __NE___
	{
		auto	available = _available.load();

		for (;;)
		{
			const auto	bit = Bits_t{1} << requiredIdx;

			if ( available & bit )
				break;

			if_likely( _available.CAS( INOUT available, bit | available ))	// 0 -> 1
			{
				_usedSlots.fetch_or( bit );
				return uint(requiredIdx);
			}
		}

		DBG_WARNING( "pool overflow" );
		return UMax;
	}

/*
=================================================
	IsFirst
=================================================
*/
	template <typename A, typename B>
	bool  LfCmdBufferPool<A,B>::IsFirst (const uint index) C_NE___
	{
		// it is safe to use 'IsFirst()' before 'Lock()' only if the execution index is constantly increasing
		//ASSERT( IsLocked() );

		const auto	used_slots	= _usedSlots.load();
		const uint	first_idx	= LowBit( used_slots );

		DEBUG_ONLY(
			auto	used_slots2 = _usedSlotsUsedInIsFirst.exchange( used_slots );
			if ( used_slots2 != 0 )
				ASSERT_Eq( first_idx, LowBit( used_slots2 ));
		)
		return index == first_idx;
	}

/*
=================================================
	IsLast
=================================================
*/
	template <typename A, typename B>
	bool  LfCmdBufferPool<A,B>::IsLast (const uint index) C_NE___
	{
		// it is safe to use 'IsFirst()' before 'Lock()' only if the execution index is constantly decreasing
		//ASSERT( IsLocked() );

		const auto	used_slots	= _usedSlots.load();
		const uint	last_idx	= HighBit( used_slots );

		DEBUG_ONLY(
			auto	used_slots2 = _usedSlotsUsedInIsLast.exchange( used_slots );
			if ( used_slots2 != 0 )
				ASSERT_Eq( last_idx, HighBit( used_slots2 ));
		)
		return index == last_idx;
	}

/*
=================================================
	Add
----
	thread safe for unique id
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::Add (INOUT uint& idx, NativeCmdBuffer_t cmdbuf) __NE___
	{
		ASSERT_Lt( idx, _pool.size() );

		if_likely( idx < _pool.size() )
		{
			DRC_SHAREDLOCK( _drCheck );
			const auto	bit = Bits_t{1} << idx;

			PlacementNew<NativeCmdBuffer_t>( OUT &_pool[idx].native, RVRef(cmdbuf) );

			MaybeUnused auto	old_bits = _ready.fetch_or( bit, EMemoryOrder::Release );
			ASSERT( (old_bits & bit) == 0 );
			ASSERT( (_cmdTypes.load() & bit) == 0 );
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
	void  LfCmdBufferPool<A,B>::Add (INOUT uint& idx, BakedCommands_t ctx) __NE___
	{
		ASSERT_Lt( idx, _pool.size() );

		if_likely( idx < _pool.size() )
		{
			DRC_SHAREDLOCK( _drCheck );
			const auto	bit = Bits_t{1} << idx;

			PlacementNew<BakedCommands_t>( OUT &_pool[idx].baked, RVRef(ctx) );
			_cmdTypes.fetch_or( bit );

			MaybeUnused auto	old_bits = _ready.fetch_or( bit, EMemoryOrder::Release );
			ASSERT( (old_bits & bit) == 0 );
		}
		idx = UMax;
	}

/*
=================================================
	Lock
----
	prevent for reserving new slots for command buffers, call for 'Acquire()' will fail.
	call once per frame.
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::Lock () __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		// set all bits to 1
		const auto	pending = _available.exchange( UMax );

		if ( pending == UMax )
			return;	// already locked

		ASSERT_Eq( _usedSlots.load(), pending );

		// set unused bits to 1
		_ready.fetch_or( ~pending );
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
	void  LfCmdBufferPool<A,B>::Reset () __NE___
	{
		DRC_EXLOCK( _drCheck );

		_ready.store( 0 );
		_cmdTypes.store( 0 );
		_available.store( 0 );
		_usedSlots.store( 0 );

		DEBUG_ONLY(
			_usedSlotsUsedInIsFirst.store( 0 );
			_usedSlotsUsedInIsLast.store( 0 );
		)

		UnsafeZeroMem( OUT _pool );
		MemoryBarrier( EMemoryOrder::Release );
	}

/*
=================================================
	_GetNativeCommands
----
	not thread safe !!!
=================================================
*/
	template <typename A, typename B>
	void  LfCmdBufferPool<A,B>::_GetNativeCommands (OUT NativeCmdBuffer_t* cmdbufs, OUT uint &cmdbufCount, const uint maxCount) __NE___
	{
		_GetNativeCommands( OUT cmdbufs, OUT cmdbufCount, maxCount,
			[](auto& dst, auto& src) { dst = RVRef(src); });

		MemoryBarrier( EMemoryOrder::Release );
	}

/*
=================================================
	_GetNativeCommands
----
	not thread safe !!!
=================================================
*/
	template <typename A, typename B>
	template <typename T, typename FN>
	void  LfCmdBufferPool<A,B>::_GetNativeCommands (OUT T* cmdbufs, OUT uint &cmdbufCount, MaybeUnused const uint maxCount, FN &&fn) __NE___
	{
		NonNull( cmdbufs );
		ASSERT_GE( maxCount, GraphicsConfig::MaxCmdBufPerBatch );

		cmdbufCount = 0;

		DRC_EXLOCK( _drCheck );
		MemoryBarrier( EMemoryOrder::Acquire );

		ASSERT_Eq( _cmdTypes.load(), 0 );	// software command buffers are not supported here
		ASSERT( IsLocked() );
		ASSERT( IsReady() );

		for (uint i : BitIndexIterate( _usedSlots.load() ))
		{
			// command buffer can be null, if used 'Complete()'
			if_likely( _pool[i].native != NativeCmdBuffer_t{} )
			{
				ASSERT( cmdbufCount < maxCount );
				fn( OUT cmdbufs[cmdbufCount++], _pool[i].native );
			}
		}

		GFX_DBG_ONLY( Reset() );
	}

/*
=================================================
	_GetBakedCommands
----
	not thread safe !!!
=================================================
*/
	template <typename A, typename B>
	template <typename T, typename FN>
	bool  LfCmdBufferPool<A,B>::_GetBakedCommands (OUT T* cmdbufs, OUT uint &cmdbufCount, MaybeUnused const uint maxCount, FN &&fn) __NE___
	{
		NonNull( cmdbufs );
		ASSERT_GE( maxCount, GraphicsConfig::MaxCmdBufPerBatch );

		cmdbufCount = 0;

		const auto	cmd_types = _cmdTypes.load();
		if ( cmd_types == 0 )
			return true;

		DRC_EXLOCK( _drCheck );
		MemoryBarrier( EMemoryOrder::Acquire );

		ASSERT( IsLocked() );
		ASSERT( IsReady() );

		for (uint i : BitIndexIterate( cmd_types ))
		{
			ASSERT( cmdbufCount < maxCount );
			CHECK_ERR( fn( OUT cmdbufs[cmdbufCount++], _pool[i], i ));
		}

		// warning: don't forget to:
		//	- change '_cmdTypes' if baked cmdbuf converted to native
		//	- add memory barrier with 'EMemoryOrder::Release' to flush changes in '_pool'

		return true;
	}


} // AE::Graphics
