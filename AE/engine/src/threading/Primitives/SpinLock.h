// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Common.h"
#endif

namespace AE::Threading
{

	//
	// Spin Lock
	//

	template <bool IsRelaxedOrder>
	struct TSpinLock final : public Noncopyable
	{
	// types
	private:
		using Self = TSpinLock< IsRelaxedOrder >;


	// variables
	private:
		Atomic<uint>	_flag {0};		// 0 -- unlocked, 1 -- locked

		static constexpr auto	_AcquireOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	_ReleaseOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;


	// methods
	public:
		TSpinLock ()							__NE___ {}
		~TSpinLock ()							__NE___	{ ASSERT(is_unlocked()); }

		ND_ bool  is_unlocked ()				C_NE___	{ return _flag.load() == 0; }
		ND_ bool  is_locked ()					C_NE___	{ return _flag.load() != 0;	}

		// for std::lock_guard / std::unique_lock / std::scoped_lock
		ND_ bool  try_lock ()					__NE___;
		ND_ bool  try_lock (uint numAttempts)	__NE___;
			void  lock ()						__NE___;
			void  unlock ()						__NE___;
	};

	using SpinLock			= TSpinLock< false >;
	using SpinLockRelaxed	= TSpinLock< true >;



	//
	// Read-Write Spin Lock
	//

	template <bool ExlockRelaxedOrder, bool SharedlockRelaxedOrder>
	struct TRWSpinLock final : public Noncopyable
	{
	// variables
	private:
		Atomic<int>		_flag {0};		// 0 -- unlocked, -1 -- write lock, >0 -- read lock

		static constexpr auto	_Exlock_AcquireOrder	= ExlockRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	_Exlock_ReleaseOrder	= ExlockRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;
		static constexpr auto	_Shlock_AcquireOrder	= SharedlockRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	_Shlock_ReleaseOrder	= SharedlockRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;
		static constexpr auto	_MaxReadLocks			= 100;


	// methods
	public:
		TRWSpinLock ()														__NE___ {}
		~TRWSpinLock ()														__NE___	{ ASSERT(is_unlocked()); }

		ND_ bool  is_unlocked ()											C_NE___	{ return _flag.load() == 0; }
		ND_ bool  is_locked ()												C_NE___	{ return _flag.load() < 0; }
		ND_ bool  is_shared_locked ()										C_NE___	{ return _flag.load() > 0; }

	// exclusive (read-write) //
		ND_ bool  try_lock ()												__NE___;
		ND_ bool  try_lock (uint numAttempts, int minReaders = 3)			__NE___;

		// for std::lock_guard / std::unique_lock / std::scoped_lock
			void  lock ()													__NE___;
			void  unlock ()													__NE___;


	// shared (read-only) //
		ND_ bool  try_lock_shared ()										__NE___;
		ND_ bool  try_lock_shared (uint numAttempts, int minWriters = -1)	__NE___;

		// for std::shared_lock
			void  lock_shared ()											__NE___;
			void  unlock_shared ()											__NE___;


	// exclusive <-> shared //
		ND_ bool  try_shared_to_exclusive ()								__NE___;
		ND_ bool  try_shared_to_exclusive (uint numAttempts, int minReaders)__NE___;
			void  shared_to_exclusive ()									__NE___;

		ND_ bool  try_exclusive_to_shared ()								__NE___;
		ND_ bool  try_exclusive_to_shared (uint numAttempts, int minReaders)__NE___;
			void  exclusive_to_shared ()									__NE___;
	};

	using RWSpinLock		= TRWSpinLock< false, false >;
	using RWSpinLockRelaxed	= TRWSpinLock< true, true >;



	//
	// Spin Lock combined with value
	//

	template <typename ValueType, uint LockBit, bool IsRelaxedOrder>
	struct TValueWithSpinLockBit final : public Noncopyable
	{
	// types
	private:
		StaticAssert( LockBit < CT_SizeOfInBits<ValueType> );
		StaticAssert( not IsPointer<ValueType> or (LockBit == 0 and alignof(ValueType) > 1) );

		using Self		= TValueWithSpinLockBit< ValueType, LockBit, IsRelaxedOrder >;
		using UInt_t	= ToUnsignedInteger< ValueType >;
		using Value_t	= ValueType;
		using CValue_t	= ValueType const;

		static constexpr UInt_t	_LockMask	= UInt_t{1} << LockBit;


	// variables
	private:
		Atomic< Value_t >		_value {};

		static constexpr auto	_AcquireOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	_ReleaseOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;


	// methods
	public:
		TValueWithSpinLockBit ()								__NE___ {}
		explicit TValueWithSpinLockBit (Value_t v)				__NE___ : _value{v} { ASSERT( not _HasLockBit( _value.load() )); }
		~TValueWithSpinLockBit ()								__NE___	{ ASSERT(is_unlocked()); }

		ND_ bool		try_lock ()								__NE___;
		ND_ bool		try_lock (uint numAttempts)				__NE___;

		// for std::lock_guard / std::unique_lock / std::scoped_lock
			void		lock ()									__NE___;
			void		unlock ()								__NE___;

		ND_ bool		is_locked ()							C_NE___	{ return _HasLockBit( _value.load() ); }
		ND_ bool		is_unlocked ()							C_NE___	{ return not is_locked(); }

		ND_ Value_t		get ()									__NE___	{ return _RemoveLockBit( _value.load() ); }
		ND_ CValue_t	get ()									C_NE___	{ return _RemoveLockBit( _value.load() ); }

		// can be changed outside of lock/unlock scope
			void		set (Value_t val)						__NE___;
		ND_ bool		try_set (Value_t val, uint numAttempts)	__NE___;
		ND_ bool		try_set (Value_t val)					__NE___	{ return try_set( val, 2 ); }


	private:
		ND_ static bool		_HasLockBit (Value_t val)			__NE___	{ return (BitCast<UInt_t>(val) & _LockMask); }
		ND_ static Value_t	_SetLockBit (Value_t val)			__NE___	{ return BitCast<Value_t>((BitCast<UInt_t>(val) | _LockMask)); }
		ND_ static Value_t	_RemoveLockBit (Value_t val)		__NE___	{ return BitCast<Value_t>((BitCast<UInt_t>(val) & ~_LockMask)); }
		ND_ static Value_t	_CopyLockBit (Value_t a, Value_t b)	__NE___	{ return BitCast<Value_t>( (BitCast<UInt_t>(a) & ~_LockMask) | (BitCast<UInt_t>(b) & _LockMask) ); }
	};


	template <typename T> using PtrWithSpinLock			= TValueWithSpinLockBit< T*, 0, false >;
	template <typename T> using PtrWithSpinLockRelaxed	= TValueWithSpinLockBit< T*, 0, true >;

//-----------------------------------------------------------------------------



/*
=================================================
	try_lock
=================================================
*/
	template <bool B>
	bool  TSpinLock<B>::try_lock () __NE___
	{
		uint	exp = 0;
		return _flag.CAS_Loop( INOUT exp, 1, _AcquireOrder, EMemoryOrder::Relaxed );
	}

	template <bool B>
	bool  TSpinLock<B>::try_lock (const uint numAttempts) __NE___
	{
		ASSERT( numAttempts > 0 );

		for (uint i = 0; i < numAttempts; ++i)
		{
			uint	exp = 0;
			if ( _flag.CAS( INOUT exp, 1, _AcquireOrder, EMemoryOrder::Relaxed ))
				return true;

			ASSERT( exp == 0 or exp == 1 );	// check for mem corruption
			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	lock
=================================================
*/
	template <bool B>
	void  TSpinLock<B>::lock () __NE___
	{
		for (uint p = 0;; ++p)
		{
			if_likely( try_lock( ThreadUtils::SpinBeforeLock() ))
				return;

			ThreadUtils::ProgressiveSleep( p );
		}
	}

/*
=================================================
	unlock
=================================================
*/
	template <bool B>
	void  TSpinLock<B>::unlock () __NE___
	{
		#ifdef AE_DEBUG
		auto	old = _flag.exchange( 0, _ReleaseOrder );
		CHECK( old == 1 );
		#else
		_flag.store( 0, _ReleaseOrder );
		#endif
	}
//-----------------------------------------------------------------------------



/*
=================================================
	try_lock
----
	flag: 0 -> -1
	can stop spinning if already locked for multiple readers
=================================================
*/
	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_lock () __NE___
	{
		int	exp = 0;
		return _flag.CAS_Loop( INOUT exp, -1, _Exlock_AcquireOrder, EMemoryOrder::Relaxed );
	}

	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_lock (const uint numAttempts, const int minReaders) __NE___
	{
		ASSERT( numAttempts > 0 );

		int	exp = 0;
		for (uint i = 0; (i < numAttempts) and (exp < minReaders); ++i)
		{
			if ( _flag.CAS( INOUT exp, -1, _Exlock_AcquireOrder, EMemoryOrder::Relaxed ))
				return true;

			ASSERT( exp >= -1 and exp < _MaxReadLocks );	// check for mem corruption
			exp = 0;
			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	lock
=================================================
*/
	template <bool A, bool B>
	void  TRWSpinLock<A,B>::lock () __NE___
	{
		for (uint p = 0;; ++p)
		{
			if_likely( try_lock( ThreadUtils::SpinBeforeLock(), _MaxReadLocks ))
				return;

			ThreadUtils::ProgressiveSleep( p );
		}
	}

/*
=================================================
	unlock
=================================================
*/
	template <bool A, bool B>
	void  TRWSpinLock<A,B>::unlock () __NE___
	{
		#ifdef AE_DEBUG
		auto	old = _flag.exchange( 0, _Exlock_ReleaseOrder );
		CHECK( old == -1 );
		#else
		_flag.store( 0, _Exlock_ReleaseOrder );
		#endif
	}

/*
=================================================
	try_lock_shared
----
	flag: 0 -> +1
	can stop spinning if already locked for writing
=================================================
*/
	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_lock_shared () __NE___
	{
		return try_lock_shared( 3 );
	}

	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_lock_shared (const uint numAttempts, const int minWriters) __NE___
	{
		ASSERT( numAttempts > 0 );

		int	exp = 0;
		for (uint i = 0; (i < numAttempts) and (exp > minWriters); ++i)
		{
			if ( _flag.CAS( INOUT exp, exp + 1, _Shlock_AcquireOrder, EMemoryOrder::Relaxed ))
				return true;

			ASSERT( exp >= -1 and exp < _MaxReadLocks );	// check for mem corruption
			exp = (exp < 0 ? 0 : exp);
			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	lock_shared
=================================================
*/
	template <bool A, bool B>
	void  TRWSpinLock<A,B>::lock_shared () __NE___
	{
		for (uint p = 0;; ++p)
		{
			// disable early exit if already locked
			if_likely( try_lock_shared( ThreadUtils::SpinBeforeLock(), -2 ))
				return;

			ThreadUtils::ProgressiveSleep( p );
		}
	}

/*
=================================================
	unlock_shared
=================================================
*/
	template <bool A, bool B>
	void  TRWSpinLock<A,B>::unlock_shared () __NE___
	{
		int	old = _flag.fetch_sub( 1, _Shlock_ReleaseOrder );
		ASSERT( old >= 0 );  Unused( old );
	}

/*
=================================================
	try_shared_to_exclusive
----
	flag: +1 -> -1
=================================================
*/
	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_shared_to_exclusive () __NE___
	{
		int	exp = 1;
		return _flag.CAS_Loop( INOUT exp, -1 );
	}

	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_shared_to_exclusive (const uint numAttempts, const int minReaders) __NE___
	{
		ASSERT( numAttempts > 0 );

		int	exp = 1;
		for (uint i = 0; (i < numAttempts) and (exp < minReaders); ++i)
		{
			if ( _flag.CAS( INOUT exp, -1 ))
				return true;

			ASSERT( exp >= -1 and exp < _MaxReadLocks );	// check for mem corruption
			exp = 1;
			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	shared_to_exclusive
=================================================
*/
	template <bool A, bool B>
	void  TRWSpinLock<A,B>::shared_to_exclusive () __NE___
	{
		for (uint p = 0;; ++p)
		{
			if_likely( try_shared_to_exclusive( ThreadUtils::SpinBeforeLock(), _MaxReadLocks ))
				return;

			ThreadUtils::ProgressiveSleep( p );
		}
	}

/*
=================================================
	try_exclusive_to_shared
----
	flag: -1 -> +1
=================================================
*/
	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_exclusive_to_shared () __NE___
	{
		int	exp = -1;
		return _flag.CAS_Loop( INOUT exp, 1 );
	}

	template <bool A, bool B>
	bool  TRWSpinLock<A,B>::try_exclusive_to_shared (const uint numAttempts, const int minReaders) __NE___
	{
		ASSERT( numAttempts > 0 );

		int	exp = -1;
		for (uint i = 0; (i < numAttempts) and (exp < minReaders); ++i)
		{
			if ( _flag.CAS( INOUT exp, 1 ))
				return true;

			ASSERT( exp >= -1 and exp < _MaxReadLocks );	// check for mem corruption
			exp = -1;
			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	exclusive_to_shared
=================================================
*/
	template <bool A, bool B>
	void  TRWSpinLock<A,B>::exclusive_to_shared () __NE___
	{
		for (uint p = 0;; ++p)
		{
			if_likely( try_exclusive_to_shared( ThreadUtils::SpinBeforeLock(), _MaxReadLocks ))
				return;

			ThreadUtils::ProgressiveSleep( p );
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	try_lock
=================================================
*/
	template <typename V, uint L, bool B>
	bool  TValueWithSpinLockBit<V,L,B>::try_lock () __NE___
	{
		Value_t	exp = _RemoveLockBit( _value.load() );
		return _value.CAS_Loop( INOUT exp, _SetLockBit( exp ), _AcquireOrder, EMemoryOrder::Relaxed );
	}

	template <typename V, uint L, bool B>
	bool  TValueWithSpinLockBit<V,L,B>::try_lock (const uint numAttempts) __NE___
	{
		ASSERT( numAttempts > 0 );

		Value_t	exp = _value.load();
		for (uint i = 0; i < numAttempts; ++i)
		{
			exp = _RemoveLockBit( exp );

			if ( _value.CAS( INOUT exp, _SetLockBit( exp ), _AcquireOrder, EMemoryOrder::Relaxed ))
				return true;

			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	lock
=================================================
*/
	template <typename V, uint L, bool B>
	void  TValueWithSpinLockBit<V,L,B>::lock () __NE___
	{
		for (uint p = 0;; ++p)
		{
			if_likely( try_lock( ThreadUtils::SpinBeforeLock() ))
				return;

			ThreadUtils::ProgressiveSleep( p );
		}
	}

/*
=================================================
	unlock
=================================================
*/
	template <typename V, uint L, bool B>
	void  TValueWithSpinLockBit<V,L,B>::unlock () __NE___
	{
		Value_t	exp = _RemoveLockBit( _value.load() );
		Value_t	prev = _value.exchange( exp, _ReleaseOrder );
		Unused( prev );
		ASSERT( prev == _SetLockBit( exp ));
	}

/*
=================================================
	try_set
=================================================
*/
	template <typename V, uint L, bool B>
	bool  TValueWithSpinLockBit<V,L,B>::try_set (Value_t val, const uint numAttempts) __NE___
	{
		ASSERT( not _HasLockBit( val ));

		Value_t	exp = _value.load();

		for (uint i = 0; i < numAttempts; ++i)
		{
			val = _CopyLockBit( val, exp );

			if ( _value.CAS( INOUT exp, val, _ReleaseOrder, EMemoryOrder::Relaxed ))
				return true;

			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	set
=================================================
*/
	template <typename V, uint L, bool B>
	void  TValueWithSpinLockBit<V,L,B>::set (Value_t val) __NE___
	{
		for (;;)
		{
			if_likely( try_set( val, ThreadUtils::SpinBeforeLock() ))
				return;

			ThreadUtils::Sleep_1us();
		}
	}


} // AE::Threading
