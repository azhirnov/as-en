// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Used to cancel task sequence from bottom to top in stack.

	Task System already has functions to cancel task sequence from top to bottom.
*/

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Thread-safe Fixed Task Stack
	//

	template <uint MaxStackSize = 15>
	class TsFixedTaskStack
	{
	// types
	private:
		static constexpr uint	c_MaxStackSize	= MaxStackSize;

		// reserve 1 bit for spin lock
		using UsedBits_t	= BitSizeToUInt< Max( c_MaxStackSize + 1, 32u )>;

		using Stack_t		= StaticArray< AsyncTask, c_MaxStackSize >;


		struct ScopedPush
		{
			TsFixedTaskStack &		_self;
			AsyncTask::Value_t *	_task;
			bool					_pushed;

			ScopedPush (TsFixedTaskStack &ref, AsyncTask task)		__NE___ : _self{ref}, _task{task.get()} { _pushed = _self.Push( RVRef(task) ); }
			~ScopedPush ()											__NE___	{ if ( _pushed ) _self.PopIf( _task ); }
		};

		struct ScopeLock
		{
			TsFixedTaskStack &		_self;
			const UsedBits_t		_prevBits;
			UsedBits_t				_bits;

			ScopeLock (TsFixedTaskStack &ref, UsedBits_t bits)		__NE___ : _self{ref}, _prevBits{bits}, _bits{bits} {}
			~ScopeLock ()											__NE___	{ _self._Unlock( _prevBits, _bits ); }

			ND_ int		HighBitIndex ()								C_NE___	{ return Base::HighBitIndex( _bits ); }
				void	Insert (uint idx)							__NE___	{ _bits = SetBit( _bits, true, idx ); }
				void	Remove (uint idx)							__NE___	{ _bits = SetBit( _bits, false, idx ); }
		};

		using InlCoro	= InlineCoro< ETaskQueue::Background >;


	// variables
	private:
		Atomic<UsedBits_t>		_usedBitsAndLock	{0};
		Stack_t					_stack				{};


	// methods
	public:
		TsFixedTaskStack ()																	__NE___	{}
		~TsFixedTaskStack ()																__NE___ { ASSERT( IsEmpty() ); }

		// Returns 'false' on overflow or null task
			bool		Push (AsyncTask task)												__NE___;
		ND_ AsyncTask	Pop ()																__NE___;
			bool		PopIf (AsyncTask task)												__NE___	{ return PopIf( task.get() ); }
			bool		PopIf (AsyncTask::Value_t *task)									__NE___;

		template <typename CoroType, typename ...Deps>
			CoroType	RunAndPush (CoroType				task,
									const Tuple<Deps...> &	deps	= Default,
									const SourceLoc &		loc		= SourceLoc::current())	__NE___;

		template <typename CoroType, typename ...Deps>
			CoroType    RunAndPush (ETaskQueue				queueType,
									CoroType				task,
									const Tuple<Deps...> &	deps	= Default,
									StringView				dbgName	= Default,
									const SourceLoc &		loc		= SourceLoc::current())	__NE___;

		ND_ ScopedPush	PushScoped (AsyncTask task)											__NE___	{ return ScopedPush{ *this, RVRef(task) }; }

		ND_ bool		IsEmpty ()															C_NE___;

		ND_	InlCoro		AsyncCancelAll ()													__NE___	{ return _AsyncCancelAll( false, {} ); }

		// Will wait until ref count become 1
		ND_	InlCoro		AsyncCancelAll (nanoseconds timeout)								__NE___	{ return _AsyncCancelAll( true, HighResClock::now() + timeout ); }
		ND_	InlCoro		AsyncCancelAll (HighResClock::time_point endTime)					__NE___	{ return _AsyncCancelAll( true, endTime ); }

			void		CancelAll ()														__NE___;

	private:
		ND_ ScopeLock	_Lock ()															__NE___;
			void		_Unlock (UsedBits_t, UsedBits_t)									__NE___;

			void		_Extract (MutableArrayView<AsyncTask> &)							__NE___;

		ND_	InlCoro		_AsyncCancelAll (bool waitForZeroRefs,
										 HighResClock::time_point endTime)					__NE___;

		ND_ static UsedBits_t	_RemoveLockBit (UsedBits_t bits)							__NE___	{ return SetBit<c_MaxStackSize>( bits, false ); }
		ND_ static UsedBits_t	_AddLockBit (UsedBits_t bits)								__NE___	{ return SetBit<c_MaxStackSize>( bits, true ); }
		ND_ static bool			_IsLocked (UsedBits_t bits)									__NE___	{ return HasBit<c_MaxStackSize>( bits ); }
	};


/*
=================================================
	Push
=================================================
*/
	template <uint S>
	bool  TsFixedTaskStack<S>::Push (AsyncTask task) __NE___
	{
		CHECK_ERR( task );

		ScopeLock	locked	= _Lock();
		int			idx		= locked.HighBitIndex() + 1;

		CHECK_ERR( idx < int(c_MaxStackSize) );	// overflow
		locked.Insert( idx );

		_stack[idx] = RVRef(task);
		return true;
	}

/*
=================================================
	Pop
=================================================
*/
	template <uint S>
	AsyncTask  TsFixedTaskStack<S>::Pop () __NE___
	{
		ScopeLock	locked	= _Lock();
		int			idx		= locked.HighBitIndex();

		if ( idx <= 0 )
			return null;

		locked.Remove( idx );

		AsyncTask	res = RVRef(_stack[idx]);
		return res;
	}

/*
=================================================
	PopIf
=================================================
*/
	template <uint S>
	bool  TsFixedTaskStack<S>::PopIf (AsyncTask::Value_t* task) __NE___
	{
		ScopeLock	locked	= _Lock();
		int			idx		= locked.HighBitIndex();

		if ( idx <= 0 )
			return false;

		if ( _stack[idx] != task )
			return false;

		locked.Remove( idx );

		_stack[idx] = null;
		return true;
	}

/*
=================================================
	IsEmpty
=================================================
*/
	template <uint S>
	bool  TsFixedTaskStack<S>::IsEmpty () C_NE___
	{
		auto	used = _RemoveLockBit( _usedBitsAndLock.load() );
		return	used == 0;
	}

/*
=================================================
	_Lock
=================================================
*/
	template <uint S>
	TsFixedTaskStack<S>::ScopeLock  TsFixedTaskStack<S>::_Lock () __NE___
	{
		const uint	c_NumAttempts	= ThreadUtils::SpinBeforeLock();
		UsedBits_t	expected		= _usedBitsAndLock.load();
		UsedBits_t	locked;

		for (uint p = 0;; ++p)
		{
			for (uint i = 0; i < c_NumAttempts; ++i)
			{
				expected = _RemoveLockBit( expected );
				locked	 = _AddLockBit( expected );

				if ( _usedBitsAndLock.CAS( INOUT expected, locked, EMemoryOrder::AcquireRelease, EMemoryOrder::Relaxed ))
				{
					return ScopeLock{ *this, _RemoveLockBit( locked )};
				}
			}

			ThreadUtils::ProgressiveSleep( p );
		}
	}

/*
=================================================
	_Unlock
=================================================
*/
	template <uint S>
	void  TsFixedTaskStack<S>::_Unlock (const UsedBits_t prevBits, const UsedBits_t newBits) __NE___
	{
		ASSERT( not _IsLocked( newBits ));

		const uint	c_NumAttempts	= ThreadUtils::SpinBeforeLock();
		UsedBits_t	expected		= _AddLockBit( prevBits );

		for (uint p = 0;; ++p)
		{
			for (uint i = 0; i < c_NumAttempts; ++i)
			{
				expected = _AddLockBit( expected );

				if ( _usedBitsAndLock.CAS( INOUT expected, newBits, EMemoryOrder::AcquireRelease, EMemoryOrder::Relaxed ))
				{
					return;
				}

				// someone change bits when they are locked
				ASSERT( expected == _AddLockBit( prevBits ));
			}

			ThreadUtils::ProgressiveSleep( p );
		}
	}

/*
=================================================
	RunAndPush
=================================================
*/
	template <uint S>
	template <typename CoroType, typename ...Deps>
	CoroType  TsFixedTaskStack<S>::RunAndPush  (CoroType				task,
												const Tuple<Deps...> &	deps,
												const SourceLoc &		loc) __NE___
	{
		Push( task );
		return Scheduler().Run( RVRef(task), deps, loc );
	}

	template <uint S>
	template <typename CoroType, typename ...Deps>
	CoroType  TsFixedTaskStack<S>::RunAndPush (ETaskQueue				queueType,
											   CoroType					task,
											   const Tuple<Deps...> &	deps,
											   StringView				dbgName,
											   const SourceLoc &		loc) __NE___
	{
		Push( task );
		return Scheduler().Run( queueType, RVRef(task), deps, dbgName, loc );
	}

/*
=================================================
	CancelAll
=================================================
*/
	template <uint S>
	void  TsFixedTaskStack<S>::CancelAll () __NE___
	{
		Stack_t		buf {};
		for (;;)
		{
			MutableArrayView<AsyncTask>		arr {buf};

			_Extract( OUT arr );

			if ( arr.empty() )
				break;

			for (auto& t : arr)
			{
				Scheduler().Cancel( *t );
				t = null;
			}
		}
	}

/*
=================================================
	_AsyncCancelAll
=================================================
*/
	template <uint S>
	TsFixedTaskStack<S>::InlCoro  TsFixedTaskStack<S>::_AsyncCancelAll (const bool waitForZeroRefs, HighResClock::time_point endTime) __NE___
	{
		Stack_t		buf {};
		for (;;)
		{
			MutableArrayView<AsyncTask>		arr {buf};

			_Extract( OUT arr );

			if ( arr.empty() )
				break;

			for (auto& task : arr)
			{
				Scheduler().Cancel( *task );
			}

			co_await Tuple{WeakDepArray{ arr }};

			// Custom dependency may keep pointer to task until some event happens
			for (; waitForZeroRefs; )
			{
				bool	has_non_zero = false;

				for (auto& task : arr) {
					has_non_zero |= task.use_count() > 1;
				}

				if ( not has_non_zero or HighResClock::now() > endTime )
					break;

				Coro_Delay( milliseconds{100} );
			}

			#ifdef AE_DEBUG
			for (auto& task : arr) {
				 ASSERT_Eq( task.use_count(), 1 );
			}
			#endif
		}
		co_return;
	}

/*
=================================================
	_Extract
=================================================
*/
	template <uint S>
	void  TsFixedTaskStack<S>::_Extract (MutableArrayView<AsyncTask> &outArr) __NE___
	{
		ScopeLock	locked	= _Lock();
		auto		bits	= Bitfield{ locked._bits };
		usize		i		= 0;

		for (; bits.Any() and i < outArr.size(); ++i)
		{
			int	idx = bits.ExtractLowBitIndex();
			ASSERT( idx >= 0 );
			ASSERT( _stack[idx] != null );

			outArr[i] = RVRef(_stack[idx]);
		}

		outArr.resize( i );
		locked._bits = 0;
	}


} // AE::Threading
