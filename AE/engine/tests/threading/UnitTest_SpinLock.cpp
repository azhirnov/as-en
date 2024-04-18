// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  PtrSpinLock_Test1 ()
	{
		using LackablePtr	= PtrWithSpinLockRelaxed< int >;

		int			a = 0;
		LackablePtr	p {&a};

		TEST( p.get() == &a );

		p.lock();
		TEST( not p.try_lock() );
		TEST( p.is_locked() );
		TEST( p.get() == &a );
		p.unlock();

		TEST( p.get() == &a );
		TEST( p.try_lock() );
		p.unlock();

		int	b = 1;
		TEST( p.try_set( &b ));
		TEST( p.get() == &b );
		TEST( p.is_unlocked() );

		TEST( p.try_lock() );
		TEST( p.get() == &b );
		TEST( p.try_set( &a ));
		TEST( p.get() == &a );
		p.unlock();
	}


	static void  SpinLock_Test1 ()
	{
		SpinLock	guard;

		TEST( guard.try_lock() );
		TEST( guard.is_locked() );
		guard.unlock();

		TEST( guard.is_unlocked() );

		TEST( guard.try_lock() );
		TEST( guard.is_locked() );
		TEST( not guard.is_unlocked() );
		guard.unlock();

		TEST( guard.is_unlocked() );
	}


	static void  RWSpinLock_Test1 ()
	{
		RWSpinLock	guard;

		TEST( guard.try_lock() );
		TEST( guard.is_locked() );
		guard.unlock();

		TEST( guard.is_unlocked() );

		TEST( guard.try_lock() );
		TEST( guard.is_locked() );
		TEST( not guard.is_unlocked() );
		TEST( not guard.try_lock_shared() );
		guard.unlock();

		TEST( guard.is_unlocked() );

		TEST( guard.try_lock_shared() );
		TEST( not guard.is_unlocked() );
		TEST( not guard.is_locked() );
		TEST( guard.is_shared_locked() );
		guard.unlock_shared();

		TEST( guard.try_lock_shared() );
		TEST( guard.try_lock_shared() );
		TEST( guard.try_lock_shared() );

		TEST( not guard.try_lock() );

		guard.unlock_shared();
		guard.unlock_shared();
		guard.unlock_shared();

		TEST( guard.is_unlocked() );

		TEST( guard.try_lock() );
		TEST( guard.try_exclusive_to_shared() );
		TEST( guard.is_shared_locked() );
		guard.unlock_shared();

		TEST( guard.try_lock_shared() );
		TEST( guard.try_shared_to_exclusive() );
		TEST( guard.is_locked() );
		guard.unlock();
	}
}


extern void UnitTest_SpinLock ()
{
	PtrSpinLock_Test1();

	SpinLock_Test1();

	RWSpinLock_Test1();

	TEST_PASSED();
}
