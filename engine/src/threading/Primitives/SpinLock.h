// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/Atomic.h"
# include "base/Utils/Noncopyable.h"
#endif
#include "base/Platforms/Platform.h"


namespace AE::Threading
{
namespace _hidden_
{
	static constexpr uint	SpinLock_SpinBeforeLock = 10'000;
}


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
		TSpinLock ()
		{}

		TSpinLock (const Self &) = delete;
		TSpinLock (Self &&) = delete;
		
		~TSpinLock ()
		{
			ASSERT( _flag.load(EMemoryOrder::Relaxed) == 0 );
		}

		ND_ forceinline bool  try_lock ()
		{
			uint	exp = 0;
			return _flag.compare_exchange_strong( INOUT exp, 1, AcquireOrder, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard
		forceinline void  lock ()
		{
			uint	exp = 0;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, 1, AcquireOrder, EMemoryOrder::Relaxed );
				 ++i)
			{
				if_unlikely( i > _hidden_::SpinLock_SpinBeforeLock )
				{
					i = 0;
					ThreadUtils::Yield();
				}
				exp = 0;
			}
		}

		forceinline void  unlock ()
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
		RWSpinLock ()
		{}
		
		~RWSpinLock ()
		{
			ASSERT( _flag.load() == 0 );
		}

		ND_ forceinline bool  try_lock ()
		{
			int	exp = 0;
			return _flag.compare_exchange_strong( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard / std::unique_lock
		forceinline void  lock ()
		{
			int	exp = 0;
			for (uint i = 0; not _flag.compare_exchange_weak( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed ); ++i)
			{
				if_unlikely( i > _hidden_::SpinLock_SpinBeforeLock )
				{
					i = 0;
					ThreadUtils::Yield();
				}
				exp = 0;
			}
		}
		
		// for std::lock_guard / std::unique_lock
		forceinline void  unlock ()
		{
		#ifdef AE_DEBUG
			ASSERT( _flag.exchange( 0, EMemoryOrder::Release ) == -1 );
		#else
			_flag.store( 0, EMemoryOrder::Release );
		#endif
		}


		ND_ forceinline bool  try_lock_shared ()
		{
			int	exp = 0;
			for (; not _flag.compare_exchange_weak( INOUT exp, exp + 1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );)
			{
				if_unlikely( exp < 0 )
					return false;
			}
			return true;
		}


		// for std::shared_lock
		forceinline void  lock_shared ()
		{
			int	exp = 0;
			for (uint i = 0;
				 not _flag.compare_exchange_weak( INOUT exp, exp + 1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
				 ++i)
			{
				if_unlikely( i > _hidden_::SpinLock_SpinBeforeLock )
				{
					i = 0;
					ThreadUtils::Yield();
				}
				exp = (exp < 0 ? 0 : exp);
			}
		}

		// for std::shared_lock
		forceinline void  unlock_shared ()
		{
			int	old = _flag.fetch_sub( 1, EMemoryOrder::Release );
			ASSERT( old >= 0 );
			Unused( old );
		}


		ND_ forceinline bool  try_shared_to_exclusive ()
		{
			int	exp = 1;
			return _flag.compare_exchange_strong( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed );
		}

		forceinline void  shared_to_exclusive ()
		{
			int	exp = 1;
			for (uint i = 0; not _flag.compare_exchange_weak( INOUT exp, -1, EMemoryOrder::Acquire, EMemoryOrder::Relaxed ); ++i)
			{
				if_unlikely( i > _hidden_::SpinLock_SpinBeforeLock )
				{
					i = 0;
					ThreadUtils::Yield();
				}
				exp = 1;
			}
		}


		forceinline void  exclusive_to_shared ()
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
		TPtrSpinLock () {}
		explicit TPtrSpinLock (PtrType* ptr) : _ptr{ptr} {}

		TPtrSpinLock (const Self &) = delete;
		TPtrSpinLock (Self &&) = delete;
		
		~TPtrSpinLock ()
		{
			ASSERT( not _HasLockBit( _ptr.load() ));
		}

		ND_ forceinline bool  try_lock ()
		{
			PtrType*	exp = _RemoveLockBit( _ptr.load() );
			return _ptr.compare_exchange_strong( INOUT exp, _SetLockBit( exp ), AcquireOrder, EMemoryOrder::Relaxed );
		}


		// for std::lock_guard
		forceinline void  lock ()
		{
			PtrType*	exp = _RemoveLockBit( _ptr.load() );
			for (uint i = 0;
				 not _ptr.CAS( INOUT exp, _SetLockBit( exp ), AcquireOrder, EMemoryOrder::Relaxed );
				 ++i)
			{
				if_unlikely( i > _hidden_::SpinLock_SpinBeforeLock )
				{
					i = 0;
					ThreadUtils::Yield();
				}
				exp = _RemoveLockBit( exp );
			}
		}

		forceinline void  unlock ()
		{
			PtrType*	exp = _RemoveLockBit( _ptr.load() );
			PtrType*	prev = _ptr.exchange( exp, ReleaseOrder );
			Unused( prev );
			ASSERT( prev == _SetLockBit( exp ));
		}

		ND_ bool			IsLocked () const	{ return _HasLockBit( _ptr.load() ); }

		ND_ PtrType*		get ()				{ return _RemoveLockBit( _ptr.load() ); }
		ND_ PtrType const*	get ()		const	{ return _RemoveLockBit( _ptr.load() ); }


		void  set (PtrType* ptr)
		{
			ASSERT( IsLocked() );
			ASSERT( not _HasLockBit( ptr ));

						ptr = _SetLockBit( ptr );
			PtrType*	exp = _ptr.load();

			for (uint i = 0; not _ptr.CAS( INOUT exp, ptr ); ++i)
			{
				if_unlikely( i > _hidden_::SpinLock_SpinBeforeLock )
				{
					i = 0;
					ThreadUtils::Yield();
				}
				ASSERT( _HasLockBit( exp ));
			}
		}


	private:
		ND_ static bool  _HasLockBit (PtrType* ptr)
		{
			return (usize(ptr) & usize{1});
		}

		ND_ static PtrType*  _SetLockBit (PtrType* ptr)
		{
			return reinterpret_cast< PtrType *>((usize(ptr) | usize{1}));
		}

		ND_ static PtrType*  _RemoveLockBit (PtrType* ptr)
		{
			return reinterpret_cast< PtrType *>((usize(ptr) & ~usize{1}));
		}
	};
	

	template <typename T> using PtrSpinLock			= TPtrSpinLock< T, false >;
	template <typename T> using PtrSpinLockRelaxed	= TPtrSpinLock< T, true >;


}	// AE::Threading
