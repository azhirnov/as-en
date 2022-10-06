// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Platforms/ThreadUtils.h"
#include "threading/Primitives/Atomic.h"

#if AE_ENABLE_DATA_RACE_CHECK

# if defined(AE_PLATFORM_ANDROID) or defined(AE_CI_BUILD)
#	define DRC_CHECK( /* expr */... )	CHECK_FATAL( __VA_ARGS__ )
# else
#	define DRC_CHECK( /* expr */... )	CHECK_ERR( __VA_ARGS__ )
# endif

# define DRC_EXLOCK( /* sync_obj */... )		EXLOCK( __VA_ARGS__ )
# define DRC_SHAREDLOCK( /* sync_obj */... )	SHAREDLOCK( __VA_ARGS__ )
# define DRC_ONLY( /* code */... )				__VA_ARGS__
# define DRC_WRAP( _value_, _syncObj_ )			decltype(_syncObj_)::Wrapper<decltype(_value_)>{ _value_, _syncObj_ }


namespace AE::Threading
{

	//
	// Data Race Check
	//

	struct DataRaceCheck
	{
	// types
	public:
		template <typename T>
		struct Wrapper
		{
			T &					ref;
			DataRaceCheck &		drCheck;

			T&  operator -> () const	{ CHECK( drCheck.IsLocked() );  return &ref; }
		};


	// variables
	private:
		mutable Atomic<usize>	_tid  {0};


	// methods
	public:
		DataRaceCheck () {}

		ND_ bool  Lock () const
		{
			const usize	id		= ThreadUtils::GetIntID();
			usize		curr	= _tid.load();
					
			for (;;)
			{
				if ( curr == id )
					return false; // recursive lock, don't call 'Unlock'

				DRC_CHECK( curr == 0 );		// locked by another thread - race condition detected!

				if ( _tid.CAS( INOUT curr, id ))
					return true;
			}
		}

		void  Unlock () const
		{
			usize	prev = _tid.exchange( 0 );
			Unused( prev );
			ASSERT( prev == ThreadUtils::GetIntID() );	// must be unlocked in the same thread
		}

		ND_ bool  IsLocked () const
		{
			return _tid.load() == ThreadUtils::GetIntID();
		}
	};



	//
	// Read/Write Data Race Check
	//

	struct RWDataRaceCheck
	{
	// types
	public:
		template <typename T>
		struct Wrapper
		{
			T &					ref;
			RWDataRaceCheck &	drCheck;

			T&			operator -> ()			{ CHECK( drCheck.IsExclusiveLocked() );	return &ref; }
			T const&	operator -> () const	{ CHECK( drCheck.IsSharedLocked() );	return &ref; }
		};


	// variables
	private:
		mutable Atomic<usize>	_lockWrite		{0};
		mutable Atomic<int>		_readCounter	{0};	// < 0 if has write locks, == 0 if no locks, > 0 if has read locks


	// methods
	public:
		RWDataRaceCheck () {}


		ND_ bool  LockExclusive ()
		{
			// lock for writing
			{
				const usize	id		= ThreadUtils::GetIntID();
				usize		curr	= _lockWrite.load();
					
				for (;;)
				{
					if ( curr == id )
						break; // recursive lock

					DRC_CHECK( curr == 0 );		// locked by another thread - race condition detected!

					if ( _lockWrite.CAS( INOUT curr, id ))
						break;
				}
			}

			// check that there is no readers
			for (int expected = _readCounter.load();;)
			{
				DRC_CHECK( expected <= 0 );	// has read lock(s) - race condition detected!

				if ( _readCounter.CAS( INOUT expected, expected - 1 ))	// 0 -> -1
					break;
			}
			return true;
		}

		void  UnlockExclusive ()
		{
			auto	prev_read = _readCounter.fetch_add( 1 );	// -1 -> 0
			ASSERT( prev_read <= 0 );

			// don't unlock if it is recursive lock
			if ( prev_read == -1 )
			{
				auto	prev_write = _lockWrite.exchange( 0 );		// unlock
				ASSERT( prev_write == ThreadUtils::GetIntID() );	// must be unlocked in the same thread
				Unused( prev_read, prev_write );
			}
		}

		ND_ bool  IsExclusiveLocked () const
		{
			return _readCounter.load() < 0 and _lockWrite.load() == ThreadUtils::GetIntID();
		}


		ND_ bool  LockShared () const
		{
			const usize	id = ThreadUtils::GetIntID();
			
			for (int expected = _readCounter.load();;)
			{
				// if has exclusive lock in current thread
				if ( expected < 0 and _lockWrite.load() == id )
				{
					return false;	// don't call 'UnlockShared'
				}
				
				DRC_CHECK( expected >= 0 );	// has write lock(s) - race condition detected!

				if ( _readCounter.CAS( INOUT expected, expected + 1 )) // 0 -> 1
					break;
			}
			return true;
		}

		void  UnlockShared () const
		{
			auto	prev_read = _readCounter.fetch_sub( 1 );	// 1 -> 0
			Unused( prev_read );
			ASSERT( prev_read > 0 );
		}

		ND_ bool  IsSharedLocked () const
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
		mutable Atomic<usize>	_tid  {ThreadUtils::GetIntID()};


	// methods
	public:
		SingleThreadCheck () {}

		void  Reset ()
		{
			_tid.store( ThreadUtils::GetIntID() );
		}

		ND_ bool  Lock () const
		{
			const usize	id		= ThreadUtils::GetIntID();
			usize		exp		= _tid.load();
			
			DRC_CHECK( exp == id );	// used in different thread
			return true;
		}

		void  Unlock () const {}
	};


}	// AE::Threading

#undef DRC_CHECK

namespace std
{
	template <>
	struct unique_lock< AE::Threading::DataRaceCheck >
	{
	private:
		AE::Threading::DataRaceCheck &	_lock;
		bool							_locked	= false;

	public:
		explicit unique_lock (AE::Threading::DataRaceCheck &ref) : _lock{ref}
		{
			_locked = _lock.Lock();
		}

		unique_lock (const unique_lock &) = delete;
		unique_lock (unique_lock &&) = delete;

		~unique_lock ()
		{
			if ( _locked )
				_lock.Unlock();
		}
	};

	template <>
	struct unique_lock< const AE::Threading::DataRaceCheck > :
		unique_lock< AE::Threading::DataRaceCheck >
	{
		explicit unique_lock (const AE::Threading::DataRaceCheck &ref) :
			unique_lock< AE::Threading::DataRaceCheck >{ const_cast<AE::Threading::DataRaceCheck &>(ref) }
		{}
	};


	template <>
	struct unique_lock< AE::Threading::RWDataRaceCheck >
	{
	private:
		AE::Threading::RWDataRaceCheck &	_lock;
		bool								_locked	= false;

	public:
		explicit unique_lock (AE::Threading::RWDataRaceCheck &ref) : _lock{ref}
		{
			_locked = _lock.LockExclusive();
		}

		unique_lock (const unique_lock &) = delete;
		unique_lock (unique_lock &&) = delete;

		~unique_lock ()
		{
			if ( _locked )
				_lock.UnlockExclusive();
		}
	};

	template <>
	struct unique_lock< const AE::Threading::RWDataRaceCheck > :
		unique_lock< AE::Threading::RWDataRaceCheck >
	{
		explicit unique_lock (const AE::Threading::RWDataRaceCheck &ref) :
			unique_lock< AE::Threading::RWDataRaceCheck >{ const_cast<AE::Threading::RWDataRaceCheck &>(ref) }
		{}
	};


	template <>
	struct shared_lock< AE::Threading::RWDataRaceCheck >
	{
	private:
		AE::Threading::RWDataRaceCheck &	_lock;
		bool								_locked	= false;

	public:
		explicit shared_lock (AE::Threading::RWDataRaceCheck &ref) : _lock{ref}
		{
			_locked = _lock.LockShared();
		}

		shared_lock (const shared_lock &) = delete;
		shared_lock (shared_lock &&) = delete;

		~shared_lock ()
		{
			if ( _locked )
				_lock.UnlockShared();
		}
	};

	template <>
	struct shared_lock< const AE::Threading::RWDataRaceCheck > :
		shared_lock< AE::Threading::RWDataRaceCheck >
	{
		explicit shared_lock (const AE::Threading::RWDataRaceCheck &ref) :
			shared_lock< AE::Threading::RWDataRaceCheck >{ const_cast<AE::Threading::RWDataRaceCheck &>(ref) }
		{}
	};


	template <>
	struct unique_lock< AE::Threading::SingleThreadCheck >
	{
	private:
		AE::Threading::SingleThreadCheck &	_lock;
		bool								_locked	= false;

	public:
		explicit unique_lock (AE::Threading::SingleThreadCheck &ref) : _lock{ref}
		{
			_locked = _lock.Lock();
		}

		unique_lock (const unique_lock &) = delete;
		unique_lock (unique_lock &&) = delete;

		~unique_lock ()
		{
			if ( _locked )
				_lock.Unlock();
		}
	};

	template <>
	struct unique_lock< const AE::Threading::SingleThreadCheck > :
		unique_lock< AE::Threading::SingleThreadCheck >
	{
		explicit unique_lock (const AE::Threading::SingleThreadCheck &ref) :
			unique_lock< AE::Threading::SingleThreadCheck >{ const_cast<AE::Threading::SingleThreadCheck &>(ref) }
		{}
	};

}	// std

#else // AE_ENABLE_DATA_RACE_CHECK

# define DRC_EXLOCK( ... )
# define DRC_SHAREDLOCK( ... )
# define DRC_ONLY( ... )
# define DRC_WRAP( _value_, _syncObj_ )		AE::Threading::DRC_Dummy_Wrap<decltype(_value_)>{ _value_ }

namespace AE::Threading
{

	template <typename T>
	struct DRC_Dummy_Wrap
	{
		T &		ref;

		T&  operator -> () const	{ return &ref; }
	};

} // AE::Threading

#endif	// not AE_ENABLE_DATA_RACE_CHECK