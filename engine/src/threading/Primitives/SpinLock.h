// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Platforms/Platform.h"
#ifndef AE_LFAS_ENABLED
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
		StdAtomic<uint>		_flag { 0 };

		static constexpr auto	AcquireOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	ReleaseOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;


	// methods
	public:
		TSpinLock ()				__NE___ {}

		TSpinLock (const Self &)	= delete;
		TSpinLock (Self &&)			= delete;
		
		~TSpinLock ()				__NE___
		{
			ASSERT( _flag.load(EMemoryOrder::Relaxed) == 0 );
		}

		ND_ bool  try_lock ()		__NE___
		{
			uint	exp = 0;
			return _flag.compare_exchange_strong( INOUT exp, 1, AcquireOrder, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard
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

	struct RWSpinLock final : public Noncopyable
	{
	// variables
	private:
		StdAtomic<int>		_flag { 0 };	// 0 -- unlocked, -1 -- write lock, >0 -- read lock


	// methods
	public:
		RWSpinLock ()					__NE___ {}
		~RWSpinLock ()					__NE___	{ ASSERT( _flag.load() == 0 ); }

		ND_ bool  try_lock ()			__NE___
		{
			int	exp = 0;
			return _flag.compare_exchange_strong( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard / std::unique_lock
		void  lock ()					__NE___
		{
			int	exp = 0;
			for (uint i = 0; not _flag.compare_exchange_weak( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed ); ++i)
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
		
		// for std::lock_guard / std::unique_lock
		void  unlock ()					__NE___
		{
		  #ifdef AE_DEBUG
			ASSERT( _flag.exchange( 0, EMemoryOrder::Release ) == -1 );
		  #else
			_flag.store( 0, EMemoryOrder::Release );
		  #endif
		}

		
		ND_ bool  try_lock_shared ()	__NE___
		{
			int	exp = 0;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, exp + 1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
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
				 not _flag.compare_exchange_weak( INOUT exp, exp + 1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
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
			int	old = _flag.fetch_sub( 1, EMemoryOrder::Release );
			ASSERT( old >= 0 );
			Unused( old );
		}


		ND_ bool  try_shared_to_exclusive () __NE___
		{
			int	exp = 1;
			return _flag.compare_exchange_strong( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
		}

		void  shared_to_exclusive ()	__NE___
		{
			int	exp = 1;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
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
			_flag.compare_exchange_strong( INOUT exp, 1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
			ASSERT( exp == -1 );
		}
	};



	//
	// Spin Lock combined with pointer
	//

	template <typename PtrType, bool IsRelaxedOrder>
	struct TPtrSpinLock final : public Noncopyable
	{
		STATIC_ASSERT( alignof(PtrType) > 1, "1 byte align is not supported" );

	// types
	private:
		using Self = TPtrSpinLock< PtrType, IsRelaxedOrder >;


	// variables
	private:
		Atomic< PtrType *>	_ptr { null };

		static constexpr auto	AcquireOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Acquire;
		static constexpr auto	ReleaseOrder = IsRelaxedOrder ? EMemoryOrder::Relaxed : EMemoryOrder::Release;


	// methods
	public:
		TPtrSpinLock ()					__NE___ {}
		explicit TPtrSpinLock (PtrType* ptr) __NE___ : _ptr{ptr} {}

		TPtrSpinLock (const Self &)		= delete;
		TPtrSpinLock (Self &&)			= delete;
		
		~TPtrSpinLock ()				__NE___
		{
			ASSERT( not _HasLockBit( _ptr.load() ));
		}

		ND_ bool  try_lock ()			__NE___
		{
			PtrType*	exp = _RemoveLockBit( _ptr.load() );
			return _ptr.compare_exchange_strong( INOUT exp, _SetLockBit( exp ), AcquireOrder, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard
		void  lock ()					__NE___
		{
			PtrType*	exp = _RemoveLockBit( _ptr.load() );
			for (uint i = 0;
				 not _ptr.CAS( INOUT exp, _SetLockBit( exp ), AcquireOrder, EMemoryOrder::Relaxed );
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

		void  unlock ()					__NE___
		{
			PtrType*	exp = _RemoveLockBit( _ptr.load() );
			PtrType*	prev = _ptr.exchange( exp, ReleaseOrder );
			Unused( prev );
			ASSERT( prev == _SetLockBit( exp ));
		}

		ND_ bool			IsLocked () C_NE___	{ return _HasLockBit( _ptr.load() ); }

		ND_ PtrType*		get ()		__NE___	{ return _RemoveLockBit( _ptr.load() ); }
		ND_ PtrType const*	get ()		C_NE___	{ return _RemoveLockBit( _ptr.load() ); }


		void  set (PtrType* ptr)		__NE___
		{
			ASSERT( IsLocked() );
			ASSERT( not _HasLockBit( ptr ));

						ptr = _SetLockBit( ptr );
			PtrType*	exp = _ptr.load();

			for (uint i = 0; not _ptr.CAS( INOUT exp, ptr ); ++i)
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
		ND_ static bool  _HasLockBit (PtrType* ptr)			__NE___
		{
			return (usize(ptr) & usize{1});
		}

		ND_ static PtrType*  _SetLockBit (PtrType* ptr)		__NE___
		{
			return reinterpret_cast< PtrType *>((usize(ptr) | usize{1}));
		}

		ND_ static PtrType*  _RemoveLockBit (PtrType* ptr)	__NE___
		{
			return reinterpret_cast< PtrType *>((usize(ptr) & ~usize{1}));
		}
	};
	

	template <typename T> using PtrSpinLock			= TPtrSpinLock< T, false >;
	template <typename T> using PtrSpinLockRelaxed	= TPtrSpinLock< T, true >;


} // AE::Threading
