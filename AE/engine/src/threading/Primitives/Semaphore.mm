// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"
#include <dispatch/dispatch.h>

#include "threading/Primitives/Semaphore.h"


#if (AE_SEMAPHORE_MODE == 3)
namespace AE::Threading::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	AppleSemaphore::AppleSemaphore (uint desired, uint maxValue) __NE___
	{
		ASSERT( desired <= maxValue );

		_sem = (__bridge void*)::dispatch_semaphore_create( Min( desired, maxValue ));
		CHECK( _sem != null );
	}

/*
=================================================
	destructor
=================================================
*/
	AppleSemaphore::~AppleSemaphore () __NE___
	{
		if ( _sem != null )
			::free( (__bridge dispatch_semaphore_t)_sem );
	}

/*
=================================================
	Acquire
=================================================
*/
	void  AppleSemaphore::Acquire () __NE___
	{
		ASSERT( _sem != null );

		auto ret = ::dispatch_semaphore_wait( (__bridge dispatch_semaphore_t)_sem, DISPATCH_TIME_FOREVER );
		CHECK( ret == KERN_SUCCESS );
	}

/*
=================================================
	Release
=================================================
*/
	void  AppleSemaphore::Release (const uint update) __NE___
	{
		ASSERT( _sem != null );
		ASSERT( update >= 1 );
		ASSERT( update <= Max() );

		for (uint i = 0; i < update; ++i) {
			::dispatch_semaphore_signal( (__bridge dispatch_semaphore_t)_sem );
		}
	}

/*
=================================================
	TryAcquire
=================================================
*/
	bool  AppleSemaphore::TryAcquire () __NE___
	{
		ASSERT( _sem != null );
		return	_sem != null and
				(::dispatch_semaphore_wait( (__bridge dispatch_semaphore_t)_sem, DISPATCH_TIME_NOW ) == KERN_SUCCESS);
	}

} // AE::Threading::_hidden_
#endif // AE_SEMAPHORE_MODE == 3
