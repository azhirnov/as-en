// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
#endif

#ifdef AE_PLATFORM_UNIX_BASED
# include <semaphore.h>
#endif

#include "threading/Primitives/Semaphore.h"

#if (AE_SEMAPHORE_MODE == 0)
namespace AE::Threading::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	WinSemaphore::WinSemaphore (uint desired, uint maxValue) __NE___
	{
		ASSERT( desired <= maxValue );

		_handle = ::CreateSemaphoreA( null, Min( desired, maxValue ), maxValue, null );
		CHECK( _handle != null );
	}

/*
=================================================
	destructor
=================================================
*/
	WinSemaphore::~WinSemaphore () __NE___
	{
		if ( _handle != null )
			::CloseHandle( HANDLE{_handle} );
	}

/*
=================================================
	Acquire
=================================================
*/
	void  WinSemaphore::Acquire () __NE___
	{
		ASSERT( _handle != null );

		auto ret = ::WaitForSingleObject( HANDLE{_handle}, INFINITE );
		Unused( ret );
		ASSERT( ret == WAIT_OBJECT_0 );
	}

/*
=================================================
	Release
=================================================
*/
	void  WinSemaphore::Release (const uint update) __NE___
	{
		ASSERT( _handle != null );
		ASSERT( update >= 1 );
		ASSERT( update <= Max() );

		::ReleaseSemaphore( HANDLE{_handle}, update, null );
	}

/*
=================================================
	TryAcquire
=================================================
*/
	bool  WinSemaphore::TryAcquire () __NE___
	{
		ASSERT( _handle != null );

		return	_handle != null and
				::WaitForSingleObject( HANDLE{_handle}, 0 ) == WAIT_OBJECT_0;
	}

} // AE::Threading::_hidden_
#endif // AE_SEMAPHORE_MODE == 0
//-----------------------------------------------------------------------------


#if (AE_SEMAPHORE_MODE == 1)
namespace AE::Threading::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	PThreadSemaphore::PThreadSemaphore (uint desired, uint maxValue) __NE___
	{
		StaticAssert( sizeof(sem_t) == sizeof(_sem) );
		StaticAssert( alignof(sem_t) == alignof(decltype(_sem)) );
		ASSERT( desired <= maxValue );

		// 0 - shared between threads
		int ret = ::sem_init( OUT _sem.Ptr<sem_t>(), 0, Min( desired, maxValue ));

		if_unlikely( ret != 0 )
			UNIX_CHECK_DEV( "sem_init() failed: " );
	}

/*
=================================================
	destructor
=================================================
*/
	PThreadSemaphore::~PThreadSemaphore () __NE___
	{
		::sem_destroy( _sem.Ptr<sem_t>() );
	}

/*
=================================================
	Acquire
=================================================
*/
	void  PThreadSemaphore::Acquire () __NE___
	{
		int ret = ::sem_wait( _sem.Ptr<sem_t>() );

		if_unlikely( ret != 0 )
			UNIX_CHECK_DEV( "sem_wait() failed: " );
	}

/*
=================================================
	Release
=================================================
*/
	void  PThreadSemaphore::Release (const uint update) __NE___
	{
		ASSERT( update >= 1 );
		ASSERT( update <= Max() );

		for (uint i = 0; i < update; ++i)
		{
			int ret = ::sem_post( _sem.Ptr<sem_t>() );

			if_unlikely( ret != 0 )
				UNIX_CHECK_DEV( "sem_post() failed: " );
		}
	}

/*
=================================================
	TryAcquire
=================================================
*/
	bool  PThreadSemaphore::TryAcquire () __NE___
	{
		return ::sem_trywait( _sem.Ptr<sem_t>() ) == 0;
	}

} // AE::Threading::_hidden_
#endif // AE_SEMAPHORE_MODE == 1
