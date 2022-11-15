// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/FunctionInfo.h"
#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/Primitives/CoroutineHandle.h"

namespace AE::Threading
{
namespace _hidden_
{
	template <typename T>
	class PromiseAwaiter;

	struct PromiseNullResult {
		constexpr PromiseNullResult () {}
	};

} // _hidden_


	static constexpr _hidden_::PromiseNullResult  CancelPromise = {};



	//
	// Promise Result
	//

	template <typename T>
	struct PromiseResult
	{
		STATIC_ASSERT( not IsSameTypes< _hidden_::PromiseNullResult, T >);

	// variables
	private:
		union {
			T			_value;
			ubyte		_data [ sizeof(T) ];	// don't use it!
		};
		bool			_hasValue	= false;


	// methods
	public:
		PromiseResult () = delete;

		PromiseResult (T &&value) __NE___ : _value{RVRef(value)}, _hasValue{true} {}
		PromiseResult (const T &value) : _value{value}, _hasValue{true} {}
		PromiseResult (const _hidden_::PromiseNullResult &) __NE___ {}

		PromiseResult (const PromiseResult<T> &other);
		PromiseResult (PromiseResult<T> &&other) __NE___;

		PromiseResult<T>&  operator = (const PromiseResult<T> &rhs);
		PromiseResult<T>&  operator = (PromiseResult<T> &&rhs) __NE___;
		PromiseResult<T>&  operator = (const T &rhs);
		PromiseResult<T>&  operator = (T &&rhs) __NE___;

		~PromiseResult ()				__NE___	{ STATIC_ASSERT( std::is_nothrow_destructible_v<T> );  if ( _hasValue ) _value.~T(); }

		ND_ bool		HasValue ()		C_NE___	{ return _hasValue; }
		ND_ T const&	Value ()		C_NE___	{ ASSERT( _hasValue );  return _value; }
		ND_ T &			Value ()		__NE___	{ ASSERT( _hasValue );  return _value; }
	};

	
	template <>
	struct PromiseResult< void >
	{
	// variables
	private:
		bool		_hasValue	= false;

	// methods
	public:
		PromiseResult () __NE___ : _hasValue{true} {}

		PromiseResult (const _hidden_::PromiseNullResult &) __NE___ {}

		PromiseResult (const PromiseResult<void> &) __NE___ = default;
		PromiseResult (PromiseResult<void> &&) __NE___ = default;
		
		PromiseResult<void>&  operator = (const PromiseResult<void> &) __NE___ = default;
		PromiseResult<void>&  operator = (PromiseResult<void> &&) __NE___ = default;

		ND_ bool	HasValue ()	C_NE___	{ return _hasValue; }
	};



	//
	// Promise
	//

	template <typename T>
	class Promise final
	{
		STATIC_ASSERT( not IsSameTypes< _hidden_::PromiseNullResult, T >);

	// types
	public:
		using Value_t	= T;
		using Self		= Promise< T >;

		enum class FunctionArg {};
		enum class ValueArg {};
		enum class CompleteValueArg {};
		
		class _InternalImpl;
		using _InternalImplPtr = RC< _InternalImpl >;


	// variables
	private:
		_InternalImplPtr	_impl;


	// methods
	public:
		Promise ()													__NE___ {}

		Promise (Self &&)											__NE___ = default;
		Promise (const Self &)										__NE___ = default;

		Self& operator = (Self &&)									__NE___ = default;
		Self& operator = (const Self &)								__NE___ = default;

		template <typename Fn>
		auto  Then (Fn &&fn, EThread thread = EThread::Worker)		__TH___;

		template <typename Fn>
		auto  Except (Fn &&fn, EThread thread = EThread::Worker)	__TH___;

		bool  Cancel ()												__NE___;

		explicit operator AsyncTask ()								C_NE___	{ return _impl; }
		
	private:
		template <typename Fn>
		auto  _Then (Fn &&fn, EThread thread)						__TH___;
		
		template <typename Fn>
		auto  _Except (Fn &&fn, EThread thread)						__TH___;

		template <typename A>
		Promise (A &&val, bool except, EThread thread, ValueArg)	__TH___;
		
		template <typename A>
		Promise (A &&val, bool except, EThread thread, CompleteValueArg) __TH___;

		template <typename Fn>
		Promise (Fn &&fn, bool except, EThread thread, FunctionArg)	__TH___;

		ND_ T  _Result () C_NE___;

		template <typename Fn, typename ...Deps>
		friend auto  MakePromise (Fn &&fn, const Tuple<Deps...> &dependsOn, EThread thread) __TH___;
		
		template <typename ...Args>
		friend auto  MakePromiseFrom (Args&& ...args)				__TH___;
		
		template <typename A>
		friend auto  MakePromiseFromArray (Array<Promise<A>> args, EThread thread) __TH___;

		template <typename A>
		friend auto  MakePromiseFromArray (Array<Promise<Array<A>>> args, EThread thread) __TH___;
		
		template <typename Fn>
		friend auto  MakeDelayedPromise (Fn &&fn, EThread thread)	__TH___;
		
		template <typename B, typename ...Deps>
		friend auto  MakePromiseFromValue (B && val, const Tuple<Deps...> &dependsOn, EThread thread) __TH___;

		template <typename B>
		friend auto  MakeDelayedPromiseFromValue (B && val, EThread thread) __TH___;
		
		template <typename B>
		friend class Promise;
		
		#ifdef AE_HAS_COROUTINE
		template <typename B>
		friend class _hidden_::PromiseAwaiter;
		#endif
	};



	//
	// Promise implementation
	//

	template <typename T>
	class Promise<T>::_InternalImpl final : public IAsyncTask
	{
	// types
	private:
		using Result_t	= PromiseResult< T >;
		using Func_t	= Function< PromiseResult<T> () >;


	// variables
	private:
		Result_t		_result;
		Func_t			_func;
		const bool		_isExept;


	// methods
	public:
		template <typename Fn>
		_InternalImpl (Fn &&fn, bool except, EThread thread, Promise<T>::FunctionArg)			__NE___;

		template <typename A>
		_InternalImpl (A && value, bool except, EThread thread, Promise<T>::ValueArg)			__NE___;
		
		template <typename A>
		_InternalImpl (A && value, bool except, EThread thread, Promise<T>::CompleteValueArg)	__NE___;

		ND_ decltype(auto)  Result ()	C_NE___
		{
			ASSERT( Status() == EStatus::Completed );
			ThreadFence( EMemoryOrder::Acquire );

			if constexpr( not IsVoid<T> )
				return _result.Value();
		}

		ND_ bool  IsExcept ()	C_NE___	{ return _isExept; }
		
		StringView  DbgName ()	C_NE_OV	{ return "Promise"; }

	private:
		void  Run ()			override;
		void  OnCancel ()		__NE_OV;
	};
//-----------------------------------------------------------------------------



#ifdef AE_HAS_COROUTINE
namespace _hidden_
{
	//
	// PromiseAwaiter
	//

	template <typename T>
	class PromiseAwaiter
	{
	// variables
	private:
		Promise<T> const&	_promise;

	// methods
	public:
		explicit PromiseAwaiter (const Promise<T> &promise) __NE___ : _promise{promise} {}

		// pause coroutine execution if dependency is not complete
		ND_ bool	await_ready ()	 C_NE___	{ return _promise._impl ? _promise._impl->IsFinished() : true; }

		// return promise result
		ND_ T		await_resume ()	 __NE___	{ return _promise._Result(); }
		
		// return task to scheduler with new dependencies
		void  await_suspend (std::coroutine_handle< AsyncTaskCoroutine::promise_type > curCoro) __NE___
		{
			auto	task = curCoro.promise().GetTask();
			CHECK_ERRV( task );
			CoroutineRunnerTask::ContinueTask( *task, Tuple{_promise._impl} );
		}
	};
	

	template <typename ...Types>
	class PromiseAwaiter< Tuple< Types... >>
	{
	// variables
	private:
		Tuple< Promise<Types>... > const&	_deps;

	// methods
	public:
		explicit PromiseAwaiter (const Tuple<Promise<Types>...> &deps) __NE___ : _deps{deps} {}
		
		// pause coroutine execution if dependencies are not complete
		ND_ bool  await_ready ()	C_NE___		{ return false; }	// TODO: check all deps for early exit
		
		// return promise results
		ND_ Tuple< Types... >  await_resume () __NE___
		{
			return	_deps.Apply( [] (auto&& ...args) {
						return Tuple<Types...>{ args._Result() ... };
					});
		}
		
		// return task to scheduler with new dependencies
		void  await_suspend (std::coroutine_handle< AsyncTaskCoroutine::promise_type > curCoro) __NE___
		{
			auto	task = curCoro.promise().GetTask();
			CHECK_ERRV( task );
			CoroutineRunnerTask::ContinueTask( *task, _deps.Apply( [] (auto&& ...args) { return Tuple{ AsyncTask{args} ... }; }) );
		}
	};

} // _hidden_

/*
=================================================
	operator co_await (promise)
=================================================
*/
	template <typename T>
	ND_ _hidden_::PromiseAwaiter<T>  operator co_await (const Promise<T> &promise) __NE___
	{
		return _hidden_::PromiseAwaiter<T>{ promise };
	}
	
	template <typename ...Types>
	ND_ _hidden_::PromiseAwaiter<Tuple<Types...>>  operator co_await (const Tuple<Promise<Types>...> &deps) __NE___
	{
		return _hidden_::PromiseAwaiter<Tuple<Types...>>{ deps };
	}

#endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------

	
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	PromiseResult<T>::PromiseResult (const PromiseResult<T> &other) :
		_hasValue{ other._hasValue }
	{
		if ( _hasValue )
			PlacementNew<T>( &_value, other._value );
	}

	template <typename T>
	PromiseResult<T>::PromiseResult (PromiseResult<T> &&other) __NE___ :
		_hasValue{ other._hasValue }
	{
		STATIC_ASSERT( IsNothrowMoveCtor<T> );
		if ( _hasValue )
			PlacementNew<T>( &_value, RVRef(other._value) );
	}

/*
=================================================
	operator =
=================================================
*/
	template <typename T>
	PromiseResult<T>&  PromiseResult<T>::operator = (const PromiseResult<T> &rhs)
	{
		if ( _hasValue )
			_value.~T();

		_hasValue = rhs._hasValue;

		if ( _hasValue )
			PlacementNew<T>( &_value, rhs._value );		// throw

		return *this;
	}
	
	template <typename T>
	PromiseResult<T>&  PromiseResult<T>::operator = (PromiseResult<T> &&rhs) __NE___
	{
		STATIC_ASSERT( std::is_nothrow_move_assignable_v<T> );

		if ( _hasValue )
			_value.~T();

		_hasValue = rhs._hasValue;

		if ( _hasValue )
		{
			PlacementNew<T>( &_value, RVRef(rhs._value) );
			rhs._value.~T();
			rhs._hasValue = false;
		}
		return *this;
	}
	
	template <typename T>
	PromiseResult<T>&  PromiseResult<T>::operator = (const T &rhs)
	{
		if ( _hasValue )
			_value.~T();

		_hasValue = true;
		PlacementNew<T>( &_value, rhs );

		return *this;
	}
	
	template <typename T>
	PromiseResult<T>&  PromiseResult<T>::operator = (T &&rhs) __NE___
	{
		STATIC_ASSERT( std::is_nothrow_move_assignable_v<T> );

		if ( _hasValue )
			_value.~T();
		
		_hasValue = true;
		PlacementNew<T>( &_value, RVRef(rhs) );

		return *this;
	}
//-----------------------------------------------------------------------------
	

/*
=================================================
	ResultToPromise
=================================================
*/
namespace _hidden_
{
	template <typename T>
	struct ResultToPromise {
		using type = Promise< T >;
	};

	template <typename T>
	struct ResultToPromise< PromiseResult<T> > {
		using type = Promise< T >;
	};

	template <>
	struct ResultToPromise< _hidden_::PromiseNullResult > {
		using type = Promise< void >;
	};

} // _hidden_
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	template <typename Fn>
	Promise<T>::Promise (Fn &&fn, bool except, EThread thread, FunctionArg flag) __TH___ :
		_impl{ MakeRC<_InternalImpl>( FwdArg<Fn>(fn), except, thread, flag )}
	{}

	template <typename T>
	template <typename A>
	Promise<T>::Promise (A &&value, bool except, EThread thread, ValueArg flag) __TH___ :
		_impl{ MakeRC<_InternalImpl>( FwdArg<A>(value), except, thread, flag )}
	{}
	
	template <typename T>
	template <typename A>
	Promise<T>::Promise (A &&value, bool except, EThread thread, CompleteValueArg flag) __TH___ :
		_impl{ MakeRC<_InternalImpl>( FwdArg<A>(value), except, thread, flag )}
	{}

/*
=================================================
	_Then
=================================================
*/
	template <typename T>
	template <typename Fn>
	auto  Promise<T>::_Then (Fn &&fn, EThread thread) __TH___
	{
		using FI		= FunctionInfo< Fn >;
		using Result	= typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;

		if constexpr( IsVoid<T> and IsVoid< typename FI::result > )
		{
			STATIC_ASSERT( FI::args::Count == 0 );
		
			return Result{	[fn = FwdArg<Fn>(fn)] () {
								fn();
								return PromiseResult<void>{};
							},
							false,
							thread,
							typename Result::FunctionArg{} };
		}
		else
		if constexpr( IsVoid<T> )
		{
			STATIC_ASSERT( FI::args::Count == 0 );
		
			return Result{ FwdArg<Fn>(fn), false, thread, typename Result::FunctionArg{} };
		}
		else
		if constexpr( IsVoid< typename FI::result > )
		{
			STATIC_ASSERT( FI::args::Count == 1 );
			STATIC_ASSERT( IsSameTypes< typename FI::args::template Get<0>, const T& >);

			return Result{	[fn = FwdArg<Fn>(fn), in = _impl] () {
								fn( in->Result() );
								return PromiseResult<void>{};
							},
							false,
							thread,
							typename Result::FunctionArg{} };
		}
		else
		{
			STATIC_ASSERT( FI::args::Count == 1 );
			STATIC_ASSERT( IsSameTypes< typename FI::args::template Get<0>, const T& >);

			return Result{	[fn = FwdArg<Fn>(fn), in = _impl] () {
								return fn( in->Result() );
							},
							false,
							thread,
							typename Result::FunctionArg{} };
		}
	}

/*
=================================================
	Then
=================================================
*/
	template <typename T>
	template <typename Fn>
	auto  Promise<T>::Then (Fn &&fn, EThread thread) __TH___
	{
		using FI		= FunctionInfo< Fn >;
		using Result	= typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;

		if_likely( _impl )
		{
			auto	result = _Then( FwdArg<Fn>(fn), thread );	// throw

			if ( _impl->IsExcept() )
				Scheduler().Run( AsyncTask{result}, Tuple{WeakDep{_impl}} );
			else
				Scheduler().Run( AsyncTask{result}, Tuple{StrongDep{_impl}} );

			return result;
		}
		return Result{};
	}

/*
=================================================
	Except
=================================================
*/
	template <typename T>
	template <typename Fn>
	auto  Promise<T>::_Except (Fn &&fn, EThread thread) __TH___
	{
		using FI		= FunctionInfo< Fn >;
		using Result	= typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;
		
		STATIC_ASSERT( FI::args::Count == 0 );
		
		if constexpr( IsVoid< typename FI::result > )
		{
			return Result{	[fn = FwdArg<Fn>(fn)] () {
								fn();
								return PromiseResult<void>{};
							},
							true,
							thread,
							typename Result::FunctionArg{} };
		}
		else
		{
			return Result{ FwdArg<Fn>(fn), true, thread, typename Result::FunctionArg{} };
		}
	}

	template <typename T>
	template <typename Fn>
	auto  Promise<T>::Except (Fn &&fn, EThread thread) __TH___
	{
		if_likely( _impl )
		{
			auto	result = _Except( FwdArg<Fn>(fn), thread );		// throw
			Scheduler().Run( AsyncTask{result}, Tuple{StrongDep{_impl}} );
			return result;
		}
		else
		{
			using FI	 = FunctionInfo< Fn >;
			using Result = typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;
			return Result{};
		}
	}
	
/*
=================================================
	Cancel
=================================================
*/
	template <typename T>
	bool  Promise<T>::Cancel () __NE___
	{
		return Scheduler().Cancel( _impl );
	}
	
/*
=================================================
	_Result
=================================================
*/
	template <typename T>
	T  Promise<T>::_Result () C_NE___
	{
		return _impl ? _impl->Result() : Default;
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	template <typename Fn>
	Promise<T>::_InternalImpl::_InternalImpl (Fn &&fn, bool except, EThread thread, Promise<T>::FunctionArg) __NE___ :
		IAsyncTask{ thread },
		_result{ CancelPromise },
		_func{ FwdArg<Fn>(fn) },
		_isExept{ except }
	{
		ASSERT( _func );
	}
	
	template <typename T>
	template <typename A>
	Promise<T>::_InternalImpl::_InternalImpl (A &&value, bool except, EThread thread, Promise<T>::ValueArg) __NE___ :
		IAsyncTask{ thread },
		_result{ FwdArg<A>(value) },
		_isExept{ except }
	{}

	template <typename T>
	template <typename A>
	Promise<T>::_InternalImpl::_InternalImpl (A &&value, bool except, EThread thread, Promise<T>::CompleteValueArg) __NE___ :
		IAsyncTask{ thread },
		_result{ FwdArg<A>(value) },
		_isExept{ except }
	{
		_MakeCompleted();
	}

/*
=================================================
	Run
=================================================
*/
	template <typename T>
	void  Promise<T>::_InternalImpl::Run ()
	{
		if_likely( (not _isExept) & bool(_func) )
		{
			_result	= _func();	// TODO: may throw?
			_func	= null;

			// set failed state
			if_unlikely( not _result.HasValue() )
			{
				OnFailure();
			}
		}
	}
	
/*
=================================================
	OnCancel
=================================================
*/
	template <typename T>
	void  Promise<T>::_InternalImpl::OnCancel () __NE___
	{
		if_likely( _isExept & bool(_func) )
		{
			_result	= _func();	// TODO: may throw?
			_func	= null;
		}
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	MakeDelayedPromiseFromValue
----
	explicitly call 'Scheduler::Run()' to add task to the queue
=================================================
*/
	template <typename T>
	ND_ auto  MakeDelayedPromiseFromValue (T && value, EThread thread = EThread::Worker) __TH___
	{
		STATIC_ASSERT( not std::is_invocable_v<T> );
		
		using Value_t	= RemoveReference< T >;
		using Result	= typename Threading::_hidden_::ResultToPromise< Value_t >::type;
			
		STATIC_ASSERT( not IsVoid< Value_t >);

		return Result{	FwdArg<T>(value),
						false,
						thread,
						typename Result::ValueArg{} };
	}
	
/*
=================================================
	MakePromiseFromValue
----
	if 'dependsOn' is empty then promise is already complete
=================================================
*/
	template <typename T, typename ...Deps>
	ND_ auto  MakePromiseFromValue (T && value, const Tuple<Deps...> &dependsOn = Default, EThread thread = EThread::Worker) __TH___
	{
		STATIC_ASSERT( not std::is_invocable_v<T> );
		
		using Value_t	= RemoveReference< T >;
		using Result	= typename Threading::_hidden_::ResultToPromise< Value_t >::type;
			
		STATIC_ASSERT( not IsVoid< Value_t >);

		if constexpr( sizeof...(Deps) == 0 )
		{
			Unused( dependsOn );

			// return completed promise if there are no dependencies
			return Result{	FwdArg<T>(value),
							false,
							thread,
							typename Result::CompleteValueArg{} };
		}
		else
		{
			Result	result = MakeDelayedPromiseFromValue( FwdArg<T>(value), thread );

			CHECK( Scheduler().Run( AsyncTask{result}, dependsOn ));

			return result;
		}
	}
	
/*
=================================================
	MakeDelayedPromise
----
	explicitly call 'Scheduler::Run()' to add task to the queue
=================================================
*/
	template <typename Fn>
	ND_ auto  MakeDelayedPromise (Fn &&fn, EThread thread = EThread::Worker) __TH___
	{
		STATIC_ASSERT( std::is_invocable_v<Fn> );
		
		using Value_t	= typename FunctionInfo< Fn >::result;
		using Result	= typename Threading::_hidden_::ResultToPromise< Value_t >::type;

		if constexpr( IsVoid< Value_t >)
		{
			return Result{	[fn = FwdArg<Fn>(fn)] () {
								fn();
								return PromiseResult<void>{};
							},
							false,
							thread,
							typename Result::FunctionArg{} };
		}
		else
		{
			return Result{ FwdArg<Fn>(fn), false, thread, typename Result::FunctionArg{} };
		}
	}

/*
=================================================
	MakePromise
=================================================
*/
	template <typename Fn, typename ...Deps>
	ND_ auto  MakePromise (Fn &&fn, const Tuple<Deps...> &dependsOn = Default, EThread thread = EThread::Worker) __TH___
	{
		auto	result = MakeDelayedPromise( FwdArg<Fn>(fn), thread );

		Scheduler().Run( AsyncTask{result}, dependsOn );

		return result;
	}
	
/*
=================================================
	MakePromiseFrom
=================================================
*/
	template <typename ...Args>
	ND_ auto  MakePromiseFrom (Args&& ...args) __TH___
	{
		return	MakePromise(
					[args...] () {
						return Tuple{ args._Result() ... };
					},
					Tuple{ AsyncTask{args} ... });
	}
	
/*
=================================================
	MakePromiseFromArray
----
	TODO: optimize
=================================================
*/
	template <typename T>
	ND_ auto  MakePromiseFromArray (Array<Promise<T>> args, EThread thread = EThread::Worker) __TH___
	{
		Array<AsyncTask>	deps;
		deps.reserve( args.size() );

		for (auto& a : args) {
			deps.push_back( AsyncTask{a} );
		}

		return	MakePromise(
					[data = RVRef(args)] ()
					{
						Array<T>	temp;
						temp.reserve( data.size() );

						for (auto& d : data) {
							temp.push_back( d._Result() );
						}
						return Tuple{ temp };
					},
					Tuple{ deps },
					thread );
	}
	
	template <typename T>
	ND_ auto  MakePromiseFromArray (Array<Promise<Array<T>>> args, EThread thread = EThread::Worker) __TH___
	{
		Array<AsyncTask>	deps;
		deps.reserve( args.size() );

		for (auto& a : args) {
			deps.push_back( AsyncTask{a} );
		}

		return	MakePromise(
					[data = RVRef(args)] ()
					{
						Array<T>	temp;
						temp.reserve( data.size() );

						for (auto& d : data)
						{
							auto	copy = d._Result();
							for (auto& c : copy) {
								temp.push_back( RVRef(c) );
							}
						}
						return temp;
					},
					Tuple{ deps },
					thread );
	}


} // AE::Threading
