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

		PromiseResult (T &&value) : _value{RVRef(value)}, _hasValue{true} {}
		PromiseResult (const T &value) : _value{value}, _hasValue{true} {}
		PromiseResult (const _hidden_::PromiseNullResult &) {}

		PromiseResult (const PromiseResult<T> &other);
		PromiseResult (PromiseResult<T> &&other);

		PromiseResult<T>&  operator = (const PromiseResult<T> &rhs);
		PromiseResult<T>&  operator = (PromiseResult<T> &&rhs);
		PromiseResult<T>&  operator = (const T &rhs);
		PromiseResult<T>&  operator = (T &&rhs);

		~PromiseResult ()					{ if ( _hasValue ) _value.~T(); }

		ND_ bool		HasValue ()	const	{ return _hasValue; }
		ND_ T const&	Value ()	const	{ ASSERT( _hasValue );  return _value; }
		ND_ T &			Value ()			{ ASSERT( _hasValue );  return _value; }
	};

	
	template <>
	struct PromiseResult< void >
	{
	// variables
	private:
		bool		_hasValue	= false;

	// methods
	public:
		PromiseResult () : _hasValue{true} {}

		PromiseResult (const _hidden_::PromiseNullResult &) {}

		PromiseResult (const PromiseResult<void> &) = default;
		PromiseResult (PromiseResult<void> &&) = default;
		
		PromiseResult<void>&  operator = (const PromiseResult<void> &) = default;
		PromiseResult<void>&  operator = (PromiseResult<void> &&) = default;

		ND_ bool	HasValue ()	const	{ return _hasValue; }
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
		Promise () {}

		Promise (Self &&) = default;
		Promise (const Self &) = default;

		Self& operator = (Self &&) = default;
		Self& operator = (const Self &) = default;

		template <typename Fn>
		auto  Then (Fn &&fn, EThread thread = EThread::Worker);

		template <typename Fn>
		auto  Except (Fn &&fn, EThread thread = EThread::Worker);

		bool  Cancel ();

		explicit operator AsyncTask () const	{ return _impl; }
		
	private:
		template <typename Fn>
		auto  _Then (Fn &&fn, EThread thread);
		
		template <typename Fn>
		auto  _Except (Fn &&fn, EThread thread);

		template <typename A>
		Promise (A &&val, bool except, EThread thread, ValueArg);
		
		template <typename A>
		Promise (A &&val, bool except, EThread thread, CompleteValueArg);

		template <typename Fn>
		Promise (Fn &&fn, bool except, EThread thread, FunctionArg);

		ND_ T  _Result () const;

		template <typename Fn, typename ...Deps>
		friend auto  MakePromise (Fn &&fn, const Tuple<Deps...> &dependsOn, EThread thread);
		
		template <typename ...Args>
		friend auto  MakePromiseFrom (Args&& ...args);
		
		template <typename A>
		friend auto  MakePromiseFromArray (Array<Promise<A>> args, EThread thread);

		template <typename A>
		friend auto  MakePromiseFromArray (Array<Promise<Array<A>>> args, EThread thread);
		
		template <typename Fn>
		friend auto  MakeDelayedPromise (Fn &&fn, EThread thread);
		
		template <typename B, typename ...Deps>
		friend auto  MakePromiseFromValue (B && val, const Tuple<Deps...> &dependsOn, EThread thread);

		template <typename B>
		friend auto  MakeDelayedPromiseFromValue (B && val, EThread thread);
		
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
		_InternalImpl (Fn &&fn, bool except, EThread thread, Promise<T>::FunctionArg);

		template <typename A>
		_InternalImpl (A && value, bool except, EThread thread, Promise<T>::ValueArg);
		
		template <typename A>
		_InternalImpl (A && value, bool except, EThread thread, Promise<T>::CompleteValueArg);

		ND_ decltype(auto)  Result () const
		{
			ASSERT( Status() == EStatus::Completed );
			ThreadFence( EMemoryOrder::Acquire );

			if constexpr( not IsVoid<T> )
				return _result.Value();
		}

		ND_ bool  IsExcept () const	{ return _isExept; }
		
		StringView  DbgName () const override { return "Promise"; }

	private:
		void  Run () override;
		void  OnCancel () override;
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
		explicit PromiseAwaiter (const Promise<T> &promise) : _promise{promise} {}

		// pause coroutine execution if dependency is not complete
		bool	await_ready () const	{ return _promise._impl ? _promise._impl->IsFinished() : true; }

		// return promise result
		T		await_resume ()			{ return _promise._Result(); }
		
		// return task to scheduler with new dependencies
		void	await_suspend (std::coroutine_handle< AsyncTaskCoroutine::promise_type > h)
		{
			auto	task = h.promise().GetTask();
			CHECK_ERRV( task );
			Unused( CoroutineRunnerTask::ContinueTask( *task, Tuple{_promise._impl} ));
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
		explicit PromiseAwaiter (const Tuple<Promise<Types>...> &deps) : _deps{deps} {}
		
		// pause coroutine execution if dependencies are not complete
		bool  await_ready () const		{ return false; }	// TODO
		
		// return promise results
		Tuple< Types... >  await_resume ()
		{
			return	_deps.Apply( [] (auto&& ...args) {
						return Tuple<Types...>{ args._Result() ... };
					});
		}
		
		// return task to scheduler with new dependencies
		void  await_suspend (std::coroutine_handle< AsyncTaskCoroutine::promise_type > h)
		{
			auto	task = h.promise().GetTask();
			CHECK_ERRV( task );
			Unused( CoroutineRunnerTask::ContinueTask( *task, _deps.Apply( [] (auto&& ...args) { return Tuple{ AsyncTask{args} ... }; }) ));
		}
	};

} // _hidden_

/*
=================================================
	operator co_await (promise)
=================================================
*/
	template <typename T>
	ND_ _hidden_::PromiseAwaiter<T>  operator co_await (const Promise<T> &promise)
	{
		return _hidden_::PromiseAwaiter<T>{ promise };
	}
	
	template <typename ...Types>
	ND_ _hidden_::PromiseAwaiter<Tuple<Types...>>  operator co_await (const Tuple<Promise<Types>...> &deps)
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
	PromiseResult<T>::PromiseResult (PromiseResult<T> &&other) :
		_hasValue{ other._hasValue }
	{
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
			PlacementNew<T>( &_value, rhs._value );

		return *this;
	}
	
	template <typename T>
	PromiseResult<T>&  PromiseResult<T>::operator = (PromiseResult<T> &&rhs)
	{
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
	PromiseResult<T>&  PromiseResult<T>::operator = (T &&rhs)
	{
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
	Promise<T>::Promise (Fn &&fn, bool except, EThread thread, FunctionArg flag) :
		_impl{ MakeRC<_InternalImpl>( FwdArg<Fn>(fn), except, thread, flag )}
	{}

	template <typename T>
	template <typename A>
	Promise<T>::Promise (A &&value, bool except, EThread thread, ValueArg flag) :
		_impl{ MakeRC<_InternalImpl>( FwdArg<A>(value), except, thread, flag )}
	{}
	
	template <typename T>
	template <typename A>
	Promise<T>::Promise (A &&value, bool except, EThread thread, CompleteValueArg flag) :
		_impl{ MakeRC<_InternalImpl>( FwdArg<A>(value), except, thread, flag )}
	{}

/*
=================================================
	_Then
=================================================
*/
	template <typename T>
	template <typename Fn>
	auto  Promise<T>::_Then (Fn &&fn, EThread thread)
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
	auto  Promise<T>::Then (Fn &&fn, EThread thread)
	{
		using FI		= FunctionInfo< Fn >;
		using Result	= typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;

		if_likely( _impl )
		{
			auto	result = _Then( FwdArg<Fn>(fn), thread );

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
	auto  Promise<T>::_Except (Fn &&fn, EThread thread)
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
	auto  Promise<T>::Except (Fn &&fn, EThread thread)
	{
		if_likely( _impl )
		{
			auto	result = _Except( FwdArg<Fn>(fn), thread );
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
	bool  Promise<T>::Cancel ()
	{
		return Scheduler().Cancel( _impl );
	}
	
/*
=================================================
	_Result
=================================================
*/
	template <typename T>
	T  Promise<T>::_Result () const
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
	Promise<T>::_InternalImpl::_InternalImpl (Fn &&fn, bool except, EThread thread, Promise<T>::FunctionArg) :
		IAsyncTask{ thread },
		_result{ CancelPromise },
		_func{ FwdArg<Fn>(fn) },
		_isExept{ except }
	{
		ASSERT( _func );
	}
	
	template <typename T>
	template <typename A>
	Promise<T>::_InternalImpl::_InternalImpl (A &&value, bool except, EThread thread, Promise<T>::ValueArg) :
		IAsyncTask{ thread },
		_result{ FwdArg<A>(value) },
		_isExept{ except }
	{}

	template <typename T>
	template <typename A>
	Promise<T>::_InternalImpl::_InternalImpl (A &&value, bool except, EThread thread, Promise<T>::CompleteValueArg) :
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
			_result	= _func();
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
	void  Promise<T>::_InternalImpl::OnCancel ()
	{
		if_likely( _isExept & bool(_func) )
		{
			_result	= _func();
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
	ND_ auto  MakeDelayedPromiseFromValue (T && value, EThread thread = EThread::Worker)
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

	template <typename T, typename ...Deps>
	ND_ auto  MakePromiseFromValue (T && value, const Tuple<Deps...> &dependsOn = Default, EThread thread = EThread::Worker)
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
	ND_ auto  MakeDelayedPromise (Fn &&fn, EThread thread = EThread::Worker)
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
	ND_ auto  MakePromise (Fn &&fn, const Tuple<Deps...> &dependsOn = Default, EThread thread = EThread::Worker)
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
	ND_ auto  MakePromiseFrom (Args&& ...args)
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
	ND_ auto  MakePromiseFromArray (Array<Promise<T>> args, EThread thread = EThread::Worker)
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
	ND_ auto  MakePromiseFromArray (Array<Promise<Array<T>>> args, EThread thread = EThread::Worker)
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
