// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	AsyncMutex implemented on top of TaskSystem.

	Can be used only inside AsyncTask.
*/

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Async Mutex
	//
	class AsyncMutex
	{
	// types
	public:
		struct ExclusiveLock
		{
		private:
			IAsyncTask *		_cur;
			AsyncMutex &		_am;

		public:
			ExclusiveLock (AsyncMutex& am, IAsyncTask* cur)		__NE___ :
				_cur{cur}, _am{am}
			{}

			ND_ AsyncTask  Lock ()								__NE___
			{
				AsyncTask	res = _am.TryLock( _cur );
				if ( res )
					_cur = null;	// not locked
				return res;
			}

			~ExclusiveLock ()									__NE___
			{
				if ( _cur != null )
					_am.Unlock( _cur );
			}
		};


	// variables
	private:
		AtomicRC< IAsyncTask >		_currentTask;


	// methods
	public:
		AsyncMutex ()								__NE___	{}
		~AsyncMutex ()								__NE___	{}

		ND_ AsyncTask  TryLock (IAsyncTask* task)	__NE___
		{
			AsyncTask	exp;
			Unused( _currentTask.CAS_Loop( INOUT exp, task ));
			return exp;
		}

		bool  Unlock (IAsyncTask* task)				__NE___
		{
			AsyncTask	exp = task;
			return _currentTask.CAS_Loop( INOUT exp, null );
		}

		ND_ bool  IsLocked ()						__NE___
		{
			return _currentTask.unsafe_get() != null;
		}
	};



#	define AE_PRIVATE_ASYNC_EXCLUSIVE_LOCK( _amutex_, _index_ )													\
		decltype(_amutex_)::ExclusiveLock  AE_PRIVATE_UNITE_RAW( __asyncExLock, _index_ ){ _amutex_, this };	\
																												\
		StaticAssert( AE::Base::IsBaseOfNoQual< AE::Threading::IAsyncTask, decltype(*this) >);					\
		ASSERT( this->DbgIsRunning() );																			\
		ASSERT( AE::Base::StringView{"Run"} == AE_FUNCTION_NAME );												\
																												\
		if ( AE::Threading::AsyncTask  task = AE_PRIVATE_UNITE_RAW( __asyncExLock, _index_ ).Lock() ) {			\
			return this->Continue( Tuple{WeakDep{task}} );														\
		}

#	define ASYNC_EXLOCK( _amutex_ ) \
		AE_PRIVATE_ASYNC_EXCLUSIVE_LOCK( _amutex_, __COUNTER__ )


} // AE::Threading
