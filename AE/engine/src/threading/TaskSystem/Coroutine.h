// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Primitives/CoroutineHandle.h"
#include "threading/TaskSystem/AsyncTask.h"

#ifdef AE_HAS_COROUTINE

namespace AE::Threading
{
namespace _hidden_
{
	template <typename T>
	class CoroutineImpl_Awaiter;


	//
	// Coroutine
	//

	template <typename ResultType>
	class CoroutineImpl final
	{
	// types
	public:
		class promise_type;
		using Handle_t	= std::coroutine_handle< promise_type >;
		using Result_t	= ResultType;

		//
		// promise_type
		//
		class promise_type final : public IAsyncTask
		{
			friend class CoroutineImpl;

		// types
		public:
			using Coroutine_t = CoroutineImpl<ResultType>;


		// variables
		private:
			ResultType		_value;

		  #ifdef AE_DEBUG
			String			_dbgName;
		  #endif


		// methods
		public:
			promise_type ()											__NE___	: IAsyncTask{ ETaskQueue::PerFrame } {}

			ND_ Coroutine_t			get_return_object ()			__NE___	{ return Coroutine_t{ *this }; }

			ND_ std::suspend_always	initial_suspend ()				C_NE___	{ return {}; }					// delayed start
			ND_ std::suspend_always	final_suspend ()				C_NE___	{ return {}; }					// must not be 'suspend_never'

				void				return_value (ResultType value)	__NE___	{ _value = RVRef(value); }		// set value by 'co_return'

				void				unhandled_exception ()			C_Th___	{ throw; }						// rethrow exceptions

			#ifdef AE_DEBUG
				StringView			DbgName ()						C_NE_OV	{ return _dbgName; }
			#else
				StringView			DbgName ()						C_NE_OV	{ return "Coroutine<>"; }
			#endif

			ND_ static void*		operator new   (usize size)		__NE___	{ return NothrowAllocatable::operator new( size ); }


		public:
				void  Cancel ()										__NE___	{ Unused( IAsyncTask::_SetCancellationState() ); }
				void  Fail ()										__NE___	{ IAsyncTask::OnFailure(); }
			ND_ bool  IsCanceled ()									__NE___	{ return IAsyncTask::IsCanceled(); }

				template <typename ...Deps>
				void  Continue (const Tuple<Deps...> &deps)			__NE___	{ return IAsyncTask::Continue( deps ); }


		private:
			void  Run ()											__Th_OV
			{
				auto	coro_handle = Handle_t::from_promise( *this );
				coro_handle.resume();	// throw

				if_unlikely( bool{coro_handle} and not coro_handle.done() )
					ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Failed ));
			}

			void  _ReleaseObject ()									__NE_OV
			{
				MemoryBarrier( EMemoryOrder::Acquire );
				ASSERT( IsFinished() );

				auto	coro_handle = Handle_t::from_promise( *this );

				// internally calls 'promise_type' dtor
				coro_handle.destroy();
			}
		};


	// variables
	private:
		RC<promise_type>	_coro;


	// methods
	public:
		CoroutineImpl ()									__NE___ {}
		explicit CoroutineImpl (promise_type &p)			__NE___ : _coro{ p.template GetRC<promise_type>() } {}
		explicit CoroutineImpl (Handle_t handle)			__NE___ : _coro{ handle.promise().template GetRC<promise_type>() } {}
		~CoroutineImpl ()									__NE___ {}

		CoroutineImpl (CoroutineImpl &&)					__NE___ = default;
		CoroutineImpl (const CoroutineImpl &)				__NE___ = default;

		CoroutineImpl&  operator = (CoroutineImpl &&)		__NE___ = default;
		CoroutineImpl&  operator = (const CoroutineImpl &)	__NE___ = default;

		ND_ explicit operator AsyncTask ()					C_NE___	{ return _coro; }
		ND_ explicit operator bool ()						C_NE___	{ return bool{_coro}; }

		auto  operator co_await ()							C_NE___;

	private:
		template <typename T>
		friend class CoroutineImpl_Awaiter;

		ND_ ResultType	_Result ()							C_NE___	{ ASSERT( _coro );  ASSERT( _coro->Status() == IAsyncTask::EStatus::Completed );  return RVRef(_coro->_value); }

	private:
		friend class Threading::TaskScheduler;
		void  _InitCoro (ETaskQueue type, StringView name)	__NE___
		{
			_coro->_SetQueueType( type );
			Unused( name );
			DEBUG_ONLY( _coro->_dbgName = String{name}; )
		}
	};


	template <>
	class CoroutineImpl <void> final {};	// not supported

	template <typename ResultType>
	using Coroutine = CoroutineImpl< ResultType >;



	//
	// Coroutine Awaiter (same as AsyncTaskCoro_Awaiter)
	//

	template <typename T>
	class CoroutineImpl_Awaiter
	{
	// variables
	private:
		Coroutine<T> const&		_dep;

	// methods
	public:
		explicit CoroutineImpl_Awaiter (const Coroutine<T> &coro)	__NE___ : _dep{coro} {}

		ND_ bool	await_ready ()									C_NE___	{ return false; }

		// return promise result
		ND_ T		await_resume ()									__NE___	{ return _dep._Result(); }

		// return task to scheduler with new dependencies
		template <typename P>
		ND_ bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			StaticAssert( IsSpecializationOf< typename P::Coroutine_t, CoroutineImpl >);

			return AsyncTaskCoro_AwaiterImpl::AwaitSuspendImpl( curCoro, AsyncTask{_dep} );
		}
	};


	template <typename ...Types>
	class CoroutineImpl_Awaiter< Tuple< Types... >>
	{
	// variables
	private:
		Tuple< Coroutine<Types>... > const&		_deps;

	// methods
	public:
		explicit CoroutineImpl_Awaiter (const Tuple<Coroutine<Types>...> &deps)	__NE___ : _deps{deps} {}

		ND_ bool  await_ready ()												C_NE___	{ return false; }

		// return promise results
		ND_ Tuple< Types... >  await_resume ()									__NE___
		{
			return	_deps.Apply( [] (auto&& ...args) __NE___ {
						return Tuple<Types...>{ args._Result() ... };
					});
		}

		// return task to scheduler with new dependencies
		template <typename P>
		ND_ bool  await_suspend (std::coroutine_handle<P> curCoro)				__NE___
		{
			StaticAssert( IsSpecializationOf< typename P::Coroutine_t, CoroutineImpl >);

			return AsyncTaskCoro_AwaiterImpl::AwaitSuspendImpl2( curCoro, _deps );
		}
	};


/*
=================================================
	operator co_await
=================================================
*/
	template <typename ResultType>
	auto  CoroutineImpl<ResultType>::operator co_await () C_NE___
	{
		return CoroutineImpl_Awaiter<ResultType>{ *this };
	}

} // _hidden_
//-----------------------------------------------------------------------------



	template <typename ResultType>
	using Coroutine = Threading::_hidden_::Coroutine< ResultType >;

/*
=================================================
	operator co_await (coroutine)
=================================================
*/
	template <typename ...Types>
	ND_ Threading::_hidden_::CoroutineImpl_Awaiter<Tuple<Types...>>  operator co_await (const Tuple<Coroutine<Types>...> &deps) __NE___
	{
		return Threading::_hidden_::CoroutineImpl_Awaiter<Tuple<Types...>>{ deps };
	}


} // AE::Threading

#endif // AE_HAS_COROUTINE
