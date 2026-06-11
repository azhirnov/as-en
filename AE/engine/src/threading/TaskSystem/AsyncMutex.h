// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	AsyncMutex implemented on top of TaskSystem.
	Can be used only inside coroutine.

	TODO: v2
		- if locked add task to internal queue
		- create coroutine which will execute tasks sequentially
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
		using Task = _Coro_::AsyncTaskImpl;

		struct ExclusiveLock
		{
		private:
			Task *			_cur	= null;
			AsyncMutex *	_am		= null;

			friend class AsyncMutex;
			ExclusiveLock (AsyncMutex& am, Task* cur)			__NE___ : _cur{cur}, _am{&am} {}

		public:
			ExclusiveLock ()									__NE___ {}
			ExclusiveLock (ExclusiveLock &&other)				__NE___ : _cur{other._cur}, _am{other._am} { other._cur = null; }
			ExclusiveLock& operator = (ExclusiveLock &&other)	__NE___	{ _cur = other._cur;  _am = other._am;  other._cur = null;  return *this; }

			ExclusiveLock (const ExclusiveLock &)				__NE___ = delete;
			ExclusiveLock& operator = (const ExclusiveLock &)	__NE___ = delete;

			~ExclusiveLock ()									__NE___
			{
				if ( _cur != null )
					_am->_Unlock( _cur );
			}

			ND_ explicit operator bool ()						C_NE___	{ return _cur != null; }
		};

	private:
		struct Awaiter
		{
		private:
			Task *			_cur = null;
			AsyncMutex &	_am;

		public:
			explicit Awaiter (AsyncMutex &am)							__NE___	: _am{am} {}

			NdCx__ bool  await_ready ()									C_NE___	{ return false; }			// call 'await_suspend()' to get coroutine handle
			Nd__IF auto  await_resume ()								__NE___	{ return ExclusiveLock{ _am, _cur }; }	// return result of 'co_await'

			template <typename P>
			Nd__IF bool  await_suspend (std::coroutine_handle<P> curCoro)__NE___
			{
				// compatible with all 'promise_type' which is inherited from 'AsyncTaskImpl'
				StaticAssert( IsBaseOf< Task, P >);

				_cur = &curCoro.promise();

				AsyncTask	other = _am._TryLock( _cur );
				if ( other )
				{
					// not locked
					_cur = null;

					return _Coro_::CoroAwaiterImpl::AwaitSuspendImpl( curCoro.promise(), RVRef(other) );
				}

				return false;  // resume
			}
		};


	// variables
	private:
		AtomicRC< Task >	_currentTask;


	// methods
	public:
		AsyncMutex ()							__NE___	{}
		~AsyncMutex ()							__NE___	{}

		ND_ auto  operator co_await ()			__NE___	{ return Awaiter{ *this }; }

		ND_ bool  IsLocked ()					__NE___
		{
			return _currentTask.unsafe_get() != null;
		}

	private:
		ND_ AsyncTask  _TryLock (Task* task)	__NE___
		{
			AsyncTask	exp;
			Unused( _currentTask.CAS_Loop( INOUT exp, AsyncTask{task} ));
			return exp;
		}

		bool  _Unlock (Task* task)				__NE___
		{
			AsyncTask	exp {task};
			return _currentTask.CAS_Loop( INOUT exp, null );
		}
	};


	#define ASYNC_EXLOCK( _amutex_ )					\
		decltype(_amutex_)::ExclusiveLock	__lock;		\
		for (;;) {										\
			if ( __lock = (co_await _amutex_); __lock )	\
				break;									\
		}


} // AE::Threading
