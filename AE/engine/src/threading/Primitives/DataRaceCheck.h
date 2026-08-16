// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	DataRaceCheck	- similar to recursive mutex.

	RWDataRaceCheck	- similar to recursive read/write lock.
*/

#pragma once

#include "threading/Primitives/Synchronized.h"

#if AE_ENABLE_DATA_RACE_CHECK

# if defined(AE_PLATFORM_ANDROID) or defined(AE_CI_BUILD_TEST)
#	define DATA_RACE_ERR( /* expr */... )		CHECK_FATAL_MSG( __VA_ARGS__, "race condition detected!" )
# else
#	define DATA_RACE_ERR( /* expr */... )		CHECK_ERR_MSG( __VA_ARGS__, "race condition detected!" )
# endif

# define DRC_EXLOCK( /* sync_obj */... )		EXLOCK( __VA_ARGS__ )
# define DRC_SHAREDLOCK( /* sync_obj */... )	SHAREDLOCK( __VA_ARGS__ )
# define DRC_ONLY( /* code */... )				__VA_ARGS__
# define DRC_CHECK( /* expr */... )				CHECK( __VA_ARGS__ )


namespace AE::Threading
{

	//
	// Data Race Check
	//

	struct DataRaceCheck
	{
	// variables
	private:
		mutable Atomic<usize>	_tid  {0};		// 0 - unlocked


	// methods
	public:
		DataRaceCheck ()		__NE___	{}

		ND_ bool  Lock ()		C_NE___
		{
			const usize	id		= ThreadUtils::GetIntID();
			usize		curr	= _tid.load();

			for (;;)
			{
				if ( curr == id )
					return false; // recursive lock, don't call 'Unlock'

				if_unlikely( curr != 0 )
				{
					_tid.store( UMax );			// trigger error on another thread too
					DATA_RACE_ERR( false );		// locked by another thread - race condition detected!
				}

				if_likely( _tid.CAS( INOUT curr, id ))
					return true;

				ThreadUtils::Pause();
			}
		}

		void  Unlock ()			C_NE___
		{
			usize	prev = _tid.exchange( 0 );
			CHECK_MSG( prev != UMax, "data race on another thread" );
			CHECK( prev == ThreadUtils::GetIntID() );					// must be unlocked in the same thread
		}

		ND_ bool  IsLocked ()	C_NE___
		{
			return _tid.load() == ThreadUtils::GetIntID();
		}
	};



	//
	// Read/Write Data Race Check
	//

	struct RWDataRaceCheck
	{
	// variables
	private:
		mutable Atomic<usize>	_lockWrite		{0};
		mutable Atomic<int>		_readCounter	{0};	// < 0 if has write locks, == 0 if no locks, > 0 if has read locks


	// methods
	public:
		RWDataRaceCheck ()				__NE___
		{}

		ND_ int   GetLockState ()		C_NE___
		{
			return _readCounter.load();	// <0 - locked for write, >0 - locked for read
		}


	// Exclusive //
		ND_ bool  LockExclusive ()		__NE___
		{
			// lock for writing
			{
				const usize	id		= ThreadUtils::GetIntID();
				usize		curr	= _lockWrite.load();

				for (;;)
				{
					if ( curr == id )
						break; // recursive lock

					if_unlikely( curr != 0 )
					{
						_lockWrite.store( UMax );	// trigger error on another thread too
						DATA_RACE_ERR( false );		// locked by another thread - race condition detected!
					}

					if_likely( _lockWrite.CAS( INOUT curr, id ))
						break;

					ThreadUtils::Pause();
				}
			}

			// check that there is no readers
			for (int expected = _readCounter.load();;)
			{
				if_unlikely( expected > 0 )
				{
					_readCounter.store( 0 );	// trigger error on another thread too
					DATA_RACE_ERR( false );		// has read lock(s) - race condition detected!
				}

				if_likely( _readCounter.CAS( INOUT expected, expected - 1 ))	// 0 -> -1
					break;

				ThreadUtils::Pause();
			}
			return true;
		}

		void  UnlockExclusive ()		__NE___
		{
			auto	prev_write	= _lockWrite.load();
			auto	prev_read	= _readCounter.fetch_add( 1 );	// -1 -> 0
			CHECK( prev_read <= 0 );
			CHECK( prev_write == ThreadUtils::GetIntID() );	// must be unlocked in the same thread

			// don't unlock if it is recursive lock
			if ( prev_read == -1 )
			{
				prev_write = _lockWrite.exchange( 0 );			// unlock
				CHECK( prev_write == ThreadUtils::GetIntID() );	// must be unlocked in the same thread
			}
		}

		ND_ bool  IsExclusiveLocked ()	C_NE___
		{
			return _readCounter.load() < 0 and _lockWrite.load() == ThreadUtils::GetIntID();
		}


	// Shared //
		ND_ bool  LockShared ()			C_NE___
		{
			const usize	id = ThreadUtils::GetIntID();

			for (int expected = _readCounter.load();;)
			{
				// if has exclusive lock in current thread
				if ( expected < 0 and _lockWrite.load() == id )
					return false;	// don't call 'UnlockShared'

				if_unlikely( expected < 0 )
				{
					_readCounter.store( 0 );	// trigger error on another thread too
					DATA_RACE_ERR( false );		// has write lock(s) - race condition detected!
				}

				if_likely( _readCounter.CAS( INOUT expected, expected + 1 )) // 0 -> 1
					break;

				ThreadUtils::Pause();
			}
			return true;
		}

		void  UnlockShared ()			C_NE___
		{
			usize	tid			= _lockWrite.load();
			auto	prev_read	= _readCounter.fetch_sub( 1 );	// 1 -> 0
			CHECK( prev_read > 0 );
			CHECK_MSG( tid != UMax, "data race on another thread" );
		}

		ND_ bool  IsSharedLocked ()		C_NE___
		{
			return _readCounter.load() > 0;
		}
	};



	//
	// Single Thread Check
	//

	struct SingleThreadCheck
	{
	// variables
	private:
		mutable Atomic<usize>	_tid;


	// methods
	public:
		SingleThreadCheck ()	__NE___
		{
			Reset();
		}

		void  Reset ()			__NE___
		{
			_tid.store( ThreadUtils::GetIntID() );
		}

		ND_ bool  Lock ()		C_NE___
		{
			const usize	id		= ThreadUtils::GetIntID();
			usize		exp		= _tid.load();

			DATA_RACE_ERR( exp == id );	// used in different thread
			return true;
		}

		void  Unlock ()			C_NE___
		{}
	};


namespace _hidden_
{

	//
	// Read/Write Data Race Check
	//

	struct RWDataRaceCheck_as_SharedMutex
	{
	private:
		RWDataRaceCheck		_drCheck;

	public:
			void  lock ()				__NE___	{ Unused( _drCheck.LockExclusive() ); }
			void  unlock ()				__NE___	{ _drCheck.UnlockExclusive(); }
		ND_ bool  try_lock ()			__NE___	{ return _drCheck.LockExclusive(); }

			void  lock_shared ()		__NE___	{ Unused( _drCheck.LockShared() ); }
			void  unlock_shared ()		__NE___	{ _drCheck.UnlockShared(); }
		ND_ bool  try_lock_shared ()	__NE___	{ return _drCheck.LockShared(); }
	};

} // _hidden_

	template <typename ...Types>
	using DRCSynchronized = Synchronized< Threading::_hidden_::RWDataRaceCheck_as_SharedMutex, Types... >;


} // AE::Threading

#undef DATA_RACE_ERR


template <>
struct std::scoped_lock< AE::Threading::DataRaceCheck >
{
private:
	AE::Threading::DataRaceCheck &	_lock;
	bool							_locked	= false;

public:
	explicit scoped_lock (AE::Threading::DataRaceCheck &ref) __NE___ : _lock{ref}
	{
		_locked = _lock.Lock();
	}

	scoped_lock (const scoped_lock &)	= delete;
	scoped_lock (scoped_lock &&)		= delete;

	~scoped_lock ()						__NE___
	{
		if ( _locked )
			_lock.Unlock();
	}
};

template <>
struct std::scoped_lock< const AE::Threading::DataRaceCheck > :
	scoped_lock< AE::Threading::DataRaceCheck >
{
	explicit scoped_lock (const AE::Threading::DataRaceCheck &ref) __NE___ :
		scoped_lock< AE::Threading::DataRaceCheck >{ const_cast<AE::Threading::DataRaceCheck &>(ref) }
	{}
};


template <>
struct std::scoped_lock< AE::Threading::RWDataRaceCheck >
{
private:
	AE::Threading::RWDataRaceCheck &	_lock;
	bool								_locked	= false;

public:
	explicit scoped_lock (AE::Threading::RWDataRaceCheck &ref) __NE___ : _lock{ref}
	{
		_locked = _lock.LockExclusive();
	}

	scoped_lock (const scoped_lock &)	= delete;
	scoped_lock (scoped_lock &&)		= delete;

	~scoped_lock ()						__NE___
	{
		if ( _locked )
			_lock.UnlockExclusive();
	}
};

template <>
struct std::scoped_lock< const AE::Threading::RWDataRaceCheck > :
	scoped_lock< AE::Threading::RWDataRaceCheck >
{
	explicit scoped_lock (const AE::Threading::RWDataRaceCheck &ref) __NE___ :
		scoped_lock< AE::Threading::RWDataRaceCheck >{ const_cast<AE::Threading::RWDataRaceCheck &>(ref) }
	{}
};


template <>
struct std::shared_lock< AE::Threading::RWDataRaceCheck >
{
private:
	AE::Threading::RWDataRaceCheck &	_lock;
	bool								_locked	= false;

public:
	explicit shared_lock (AE::Threading::RWDataRaceCheck &ref) __NE___ : _lock{ref}
	{
		_locked = _lock.LockShared();
	}

	shared_lock (const shared_lock &)	= delete;
	shared_lock (shared_lock &&)		= delete;

	~shared_lock ()						__NE___
	{
		if ( _locked )
			_lock.UnlockShared();
	}
};

template <>
struct std::shared_lock< const AE::Threading::RWDataRaceCheck > :
	shared_lock< AE::Threading::RWDataRaceCheck >
{
	explicit shared_lock (const AE::Threading::RWDataRaceCheck &ref) __NE___ :
		shared_lock< AE::Threading::RWDataRaceCheck >{ const_cast<AE::Threading::RWDataRaceCheck &>(ref) }
	{}
};


template <>
struct std::scoped_lock< AE::Threading::SingleThreadCheck >
{
private:
	AE::Threading::SingleThreadCheck &	_lock;
	bool								_locked	= false;

public:
	explicit scoped_lock (AE::Threading::SingleThreadCheck &ref) __NE___ : _lock{ref}
	{
		_locked = _lock.Lock();
	}

	scoped_lock (const scoped_lock &)	= delete;
	scoped_lock (scoped_lock &&)		= delete;

	~scoped_lock ()						__NE___
	{
		if ( _locked )
			_lock.Unlock();
	}
};

template <>
struct std::scoped_lock< const AE::Threading::SingleThreadCheck > :
	scoped_lock< AE::Threading::SingleThreadCheck >
{
	explicit scoped_lock (const AE::Threading::SingleThreadCheck &ref) __NE___ :
		scoped_lock< AE::Threading::SingleThreadCheck >{ const_cast<AE::Threading::SingleThreadCheck &>(ref) }
	{}
};


#else // AE_ENABLE_DATA_RACE_CHECK

# define DRC_EXLOCK( ... )			{}
# define DRC_SHAREDLOCK( ... )		{}
# define DRC_ONLY( ... )
# define DRC_CHECK( ... )			{}

namespace AE::Threading
{
namespace _hidden_
{
	struct DummySharedMutex
	{
	public:
			void  lock ()				__NE___	{}
			void  unlock ()				__NE___	{}
		ND_ bool  try_lock ()			__NE___	{ return true; }

			void  lock_shared ()		__NE___	{}
			void  unlock_shared ()		__NE___	{}
		ND_ bool  try_lock_shared ()	__NE___	{ return true; }
	};

} // _hidden_

	template <typename ...Types>
	using DRCSynchronized = Synchronized< Threading::_hidden_::DummySharedMutex, Types... >;


} // AE::Threading

#endif // not AE_ENABLE_DATA_RACE_CHECK
