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
	class CoroutineAwaiter;


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
			
		// methods
		public:
			promise_type ()											__NE___	: IAsyncTask{ ETaskQueue::Worker } {}

			ND_ Coroutine_t			get_return_object ()			__NE___	{ return Coroutine_t{ *this }; }

			ND_ std::suspend_always	initial_suspend ()				C_NE___	{ return {}; }					// delayed start
			ND_ std::suspend_always	final_suspend ()				C_NE___	{ return {}; }					// must not be 'suspend_never'
			
				void				return_value (ResultType value)	__NE___	{ _value = RVRef(value); }		// set value by 'co_return'

				void				unhandled_exception ()			C_Th___	{ throw; }						// rethrow exceptions

				StringView			DbgName ()						C_NE_OV	{ return "coroutine"; }

			
		public:
			void  Cancel ()											__NE___	{ Unused( IAsyncTask::_SetCancellationState() ); }
				
			template <typename ...Deps>
			void  Continue (const Tuple<Deps...> &deps)				__NE___	{ return IAsyncTask::Continue( deps ); }


		private:
			void  Run () __Th_OV
			{
				auto	coro_handle = Handle_t::from_promise( *this );
				coro_handle.resume();	// throw

				if_unlikely( bool{coro_handle} and not coro_handle.done() )
					ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Failed ));
			}

			void  _ReleaseObject () __NE_OV
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
		explicit CoroutineImpl (promise_type &p)			__NE___ : _coro{ p.GetRC<promise_type>() } {}
		explicit CoroutineImpl (Handle_t handle)			__NE___ : _coro{ handle.Promise().GetRC<promise_type>() } {}
		~CoroutineImpl ()									__NE___ {}

		CoroutineImpl (CoroutineImpl &&)					__NE___ = default;
		CoroutineImpl (const CoroutineImpl &)				__NE___ = default;

		CoroutineImpl&  operator = (CoroutineImpl &&)		__NE___ = default;
		CoroutineImpl&  operator = (const CoroutineImpl &)	__NE___ = default;

		explicit operator AsyncTask ()						C_NE___	{ return _coro; }
		explicit operator bool ()							C_NE___	{ return bool{_coro}; }
		
		auto  operator co_await ()							C_NE___;

	private:
		template <typename T>
		friend class CoroutineAwaiter;

		ND_ IAsyncTask::EStatus	_Status ()					C_NE___	{ return _coro ? _coro->Status() : IAsyncTask::EStatus::Canceled; }
		ND_ ResultType			_Result ()					C_NE___	{ ASSERT( _coro );  ASSERT( _coro->Status() == IAsyncTask::EStatus::Completed );  return RVRef(_coro->_value); }

	private:
		friend class TaskScheduler;
		void  _SetQueueType (ETaskQueue type)				__NE___	{ _coro->_SetQueueType( type ); }
	};

	
	template <>
	class CoroutineImpl <void> final {};	// not supported
		
	template <typename ResultType>
	using Coroutine = CoroutineImpl< ResultType >;



	//
	// Coroutine Awaiter
	//

	template <typename T>
	class CoroutineAwaiter
	{
	// variables
	private:
		Coroutine<T> const&		_coro;

	// methods
	public:
		explicit CoroutineAwaiter (const Coroutine<T> &coro) : _coro{coro} {}

		ND_ bool	await_ready ()		C_NE___	{ return false; }

		// return promise result
		ND_ T		await_resume ()		__NE___	{ return _coro._Result(); }
		
		// return task to scheduler with new dependencies
		template <typename P>
		ND_ bool  await_suspend (std::coroutine_handle<P> curCoro) __NE___
		{
			STATIC_ASSERT( IsSpecializationOf< typename P::Coroutine_t, CoroutineImpl >);

			using EStatus = IAsyncTask::EStatus;
				
			const EStatus	stat = _coro._Status();

			if ( stat == EStatus::Completed )
				return false;	// resume

			if ( stat > EStatus::_Interropted )
			{
				curCoro.promise().Cancel();
				return true;	// suspend & cancel
			}
				
			curCoro.promise().Continue( Tuple{ AsyncTask{_coro} });

			// task may be cancelled
			if_unlikely( curCoro.promise().IsFinished() )
				return false;	// resume

			return true;	// suspend
		}
	};
	

	template <typename ...Types>
	class CoroutineAwaiter< Tuple< Types... >>
	{
	// variables
	private:
		Tuple< Coroutine<Types>... > const&		_deps;

	// methods
	public:
		explicit CoroutineAwaiter (const Tuple<Coroutine<Types>...> &deps) __NE___ : _deps{deps} {}
		
		// pause coroutine execution if dependencies are not complete
		ND_ bool  await_ready ()				C_NE___	{ return false; }	// TODO: check all deps for early exit
		
		// return promise results
		ND_ Tuple< Types... >  await_resume ()	__NE___
		{
			return	_deps.Apply( [] (auto&& ...args) {
						return Tuple<Types...>{ args._Result() ... };
					});
		}
		
		// return task to scheduler with new dependencies
		template <typename P>
		ND_ bool  await_suspend (std::coroutine_handle<P> curCoro) __NE___
		{
			STATIC_ASSERT( IsSpecializationOf< typename P::Coroutine_t, CoroutineImpl >);
			
			using EStatus = IAsyncTask::EStatus;

			const auto	stats_arr	= _deps.Apply( [] (auto&& ...args) { return StaticArray< EStatus, sizeof...(Types) >{ args._Status() ... }; });
			const auto	stats		= ArrayView<EStatus>{ stats_arr };
			
			if ( stats.AllEqual( EStatus::Completed ))
				return false;	// resume

			if ( stats.AllGreater( EStatus::_Finished ) and
				 stats.AllGreater( EStatus::_Interropted ))
			{
				curCoro.promise().Cancel();
				return true;	// suspend & cancel
			}

			curCoro.promise().Continue( _deps.Apply( [] (auto&& ...args) { return Tuple{ AsyncTask{args} ... }; }));
			
			// task may be cancelled
			if_unlikely( curCoro.promise().IsFinished() )
				return false;	// resume

			return true;	// suspend
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
		return CoroutineAwaiter<ResultType>{ *this };
	}

} // _hidden_
//-----------------------------------------------------------------------------



	template <typename ResultType>
	using Coroutine = Threading::_hidden_::Coroutine<ResultType>;
	
/*
=================================================
	operator co_await (coroutine)
=================================================
*/
	template <typename ...Types>
	ND_ Threading::_hidden_::CoroutineAwaiter<Tuple<Types...>>  operator co_await (const Tuple<Coroutine<Types>...> &deps) __NE___
	{
		return Threading::_hidden_::CoroutineAwaiter<Tuple<Types...>>{ deps };
	}


} // AE::Threading

#endif // AE_HAS_COROUTINE
