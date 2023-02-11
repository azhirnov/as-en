// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "base/CompileTime/Math.h"
# include "base/Platforms/Platform.h"
# include "base/Utils/Helpers.h"
# include "threading/Primitives/Atomic.h"
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

		static constexpr auto	AcquireOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	ReleaseOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;


	// methods
	public:
		TSpinLock ()				__NE___ {}

		TSpinLock (const Self &)	= delete;
		TSpinLock (Self &&)			= delete;
		
		~TSpinLock ()				__NE___
		{
			ASSERT( _flag.load() == 0 );
		}

		
		ND_ bool  is_unlocked ()	__NE___	{ return _flag.load() == 0; }
		ND_ bool  is_locked ()		__NE___	{ return _flag.load() != 0;	}


		ND_ bool  try_lock ()		__NE___
		{
			uint	exp = 0;
			return _flag.compare_exchange_strong( INOUT exp, 1, AcquireOrder, EMemoryOrder::Relaxed );
		}

		
		// for std::lock_guard / std::unique_lock / std::scoped_lock
		void  lock ()				__NE___
		{
			uint	exp = 0;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, 1, AcquireOrder, EMemoryOrder::Relaxed );
				 ++i)
			{
				if_unlikely( i > ThreadUtils::SpinBeforeLock() )
				{
					i = 0;
					ThreadUtils::YieldOrSleep();
				}

				exp = 0;
				ThreadUtils::Pause();
			}
		}

		void  unlock ()				__NE___
		{
		  #ifdef AE_DEBUG
			ASSERT( _flag.exchange( 0, ReleaseOrder ) == 1 );
		  #else
			_flag.store( 0, ReleaseOrder );
		  #endif
		}
	};

	
	using SpinLock			= TSpinLock< false >;
	using SpinLockRelaxed	= TSpinLock< true >;


	
	//
	// Read-Write Spin Lock
	//
	
	template <bool IsRelaxedOrder>
	struct TRWSpinLock final : public Noncopyable
	{
	// variables
	private:
		Atomic<int>		_flag {0};		// 0 -- unlocked, -1 -- write lock, >0 -- read lock
		
		static constexpr auto	AcquireOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	ReleaseOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;


	// methods
	public:
		TRWSpinLock ()					__NE___ {}
		~TRWSpinLock ()					__NE___	{ ASSERT( _flag.load() == 0 ); }
		
		ND_ bool  is_unlocked ()		__NE___	{ return _flag.load() == 0; }
		ND_ bool  is_locked ()			__NE___	{ return _flag.load() < 0; }
		ND_ bool  is_shared_locked ()	__NE___	{ return _flag.load() > 0; }


	//-------------------------------------------------
	// exclusive (read-write)

		ND_ bool  try_lock ()			__NE___
		{
			int	exp = 0;
			return _flag.compare_exchange_strong( INOUT exp, -1, AcquireOrder, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard / std::unique_lock / std::scoped_lock
		void  lock ()					__NE___
		{
			int	exp = 0;
			for (uint i = 0; not _flag.compare_exchange_weak( INOUT exp, -1, AcquireOrder, EMemoryOrder::Relaxed ); ++i)
			{
				if_unlikely( i > ThreadUtils::SpinBeforeLock() )
				{
					i = 0;
					ThreadUtils::YieldOrSleep();
				}

				exp = 0;
				ThreadUtils::Pause();
			}
		}
		
		// for std::lock_guard / std::unique_lock / std::scoped_lock
		void  unlock ()					__NE___
		{
		  #ifdef AE_DEBUG
			ASSERT( _flag.exchange( 0, ReleaseOrder ) == -1 );
		  #else
			_flag.store( 0, ReleaseOrder );
		  #endif
		}
		

	//-------------------------------------------------
	// shared (read-only)
		
		ND_ bool  try_lock_shared ()	__NE___
		{
			int	exp = 0;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, exp + 1, AcquireOrder, EMemoryOrder::Relaxed );
				 ++i)
			{
				if_unlikely( exp < 0 or i > 100 )
					return false;
				
				ThreadUtils::Pause();
			}
			return true;
		}


		// for std::shared_lock
		void  lock_shared ()			__NE___
		{
			int	exp = 0;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, exp + 1, AcquireOrder, EMemoryOrder::Relaxed );
				 ++i)
			{
				if_unlikely( i > ThreadUtils::SpinBeforeLock() )
				{
					i = 0;
					ThreadUtils::YieldOrSleep();
				}

				exp = (exp < 0 ? 0 : exp);
				ThreadUtils::Pause();
			}
		}

		// for std::shared_lock
		void  unlock_shared ()			__NE___
		{
			int	old = _flag.fetch_sub( 1, ReleaseOrder );
			ASSERT( old >= 0 );
			Unused( old );
		}

		
	//-------------------------------------------------
	// exclusive <-> shared

		ND_ bool  try_shared_to_exclusive () __NE___
		{
			int	exp = 1;
			return _flag.compare_exchange_strong( INOUT exp, -1 );
		}

		void  shared_to_exclusive ()	__NE___
		{
			int	exp = 1;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, -1 );
				 ++i)
			{
				if_unlikely( i > ThreadUtils::SpinBeforeLock() )
				{
					i = 0;
					ThreadUtils::YieldOrSleep();
				}

				exp = 1;
				ThreadUtils::Pause();
			}
		}

		void  exclusive_to_shared ()	__NE___
		{
			int	exp = -1;
			Unused( _flag.compare_exchange_strong( INOUT exp, 1 ));
			ASSERT( exp == -1 );
		}
	};


	using RWSpinLock		= TRWSpinLock< false >;
	using RWSpinLockRelaxed	= TRWSpinLock< true >;



	//
	// Spin Lock combined with value
	//

	template <typename ValueType, uint LockBit, bool IsRelaxedOrder>
	struct TValueWithSpinLockBit final : public Noncopyable
	{
	// types
	private:
		STATIC_ASSERT( LockBit < CT_SizeOfInBits<ValueType> );
		STATIC_ASSERT( not IsPointer<ValueType> or (LockBit == 0 and alignof(ValueType) > 1) );

		using Self		= TValueWithSpinLockBit< ValueType, LockBit, IsRelaxedOrder >;
		using UInt_t	= ToUnsignedInteger< ValueType >;

		static constexpr UInt_t	_LockMask	= UInt_t{1} << LockBit;


	// variables
	private:
		Atomic< ValueType >		_value {};

		static constexpr auto	AcquireOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	ReleaseOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;


	// methods
	public:
		TValueWithSpinLockBit ()					__NE___ {}
		explicit TValueWithSpinLockBit (ValueType v)__NE___ : _value{v} { ASSERT( not _HasLockBit( _value.load() )); }

		TValueWithSpinLockBit (const Self &)		= delete;
		TValueWithSpinLockBit (Self &&)				= delete;
		
		~TValueWithSpinLockBit ()					__NE___	{ ASSERT( not _HasLockBit( _value.load() )); }

		ND_ bool  try_lock ()						__NE___
		{
			ValueType	exp = _RemoveLockBit( _value.load() );
			return _value.compare_exchange_strong( INOUT exp, _SetLockBit( exp ), AcquireOrder, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard
		void  lock ()								__NE___
		{
			ValueType	exp = _RemoveLockBit( _value.load() );
			for (uint i = 0;
				 not _value.CAS( INOUT exp, _SetLockBit( exp ), AcquireOrder, EMemoryOrder::Relaxed );
				 ++i)
			{
				if_unlikely( i > ThreadUtils::SpinBeforeLock() )
				{
					i = 0;
					ThreadUtils::YieldOrSleep();
				}
				
				exp = _RemoveLockBit( exp );
				ThreadUtils::Pause();
			}
		}

		void  unlock ()								__NE___
		{
			ValueType	exp = _RemoveLockBit( _value.load() );
			ValueType	prev = _value.exchange( exp, ReleaseOrder );
			Unused( prev );
			ASSERT( prev == _SetLockBit( exp ));
		}

		ND_ bool			IsLocked ()				C_NE___	{ return _HasLockBit( _value.load() ); }

		ND_ ValueType		get ()					__NE___	{ return _RemoveLockBit( _value.load() ); }
		ND_ ValueType const	get ()					C_NE___	{ return _RemoveLockBit( _value.load() ); }


		void  set (ValueType val)					__NE___
		{
			ASSERT( IsLocked() );
			ASSERT( not _HasLockBit( val ));

						val = _SetLockBit( val );
			ValueType	exp = _value.load();

			for (uint i = 0; not _value.CAS( INOUT exp, val ); ++i)
			{
				if_unlikely( i > ThreadUtils::SpinBeforeLock() )
				{
					i = 0;
					ThreadUtils::YieldOrSleep();
				}
				
				ASSERT( _HasLockBit( exp ));
				ThreadUtils::Pause();
			}
		}


	private:
		ND_ static bool  _HasLockBit (ValueType val)		__NE___
		{
			return (BitCast<UInt_t>(val) & _LockMask);
		}

		ND_ static ValueType  _SetLockBit (ValueType val)	__NE___
		{
			return BitCast< ValueType >((BitCast<UInt_t>(val) | _LockMask));
		}

		ND_ static ValueType  _RemoveLockBit (ValueType val)__NE___
		{
			return BitCast< ValueType >((BitCast<UInt_t>(val) & ~_LockMask));
		}
	};
	

	template <typename T> using PtrWithSpinLock			= TValueWithSpinLockBit< T*, 0, false >;
	template <typename T> using PtrWithSpinLockRelaxed	= TValueWithSpinLockBit< T*, 0, true >;


} // AE::Threading
