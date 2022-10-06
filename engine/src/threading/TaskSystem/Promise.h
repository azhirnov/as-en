// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"
#include "base/CompileTime/FunctionInfo.h"

namespace AE::Threading
{
	struct PromiseNullResult {
		constexpr PromiseNullResult () {}
	};

	static constexpr PromiseNullResult  CancelPromise = {};



	//
	// Promise Result
	//

	template <typename T>
	struct PromiseResult
	{
		STATIC_ASSERT( not IsSameTypes< PromiseNullResult, T >);

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
		PromiseResult (const PromiseNullResult &) {}

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

		PromiseResult (const PromiseNullResult &) {}

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
		STATIC_ASSERT( not IsSameTypes< PromiseNullResult, T >);

	// types
	public:
		using Value_t	= T;
		using Self		= Promise< T >;
		using EThread	= IAsyncTask::EThread;
		
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
		template <typename Fn, typename ...Deps>
		auto  _Then (Fn &&fn, EThread thread, const Tuple<Deps...> &dependsOn);

		template <typename Fn, typename ...Deps>
		Promise (Fn &&fn, bool except, EThread thread, const Tuple<Deps...> &dependsOn);

		ND_ T  _Result () const;

		template <typename Fn, typename ...Deps>
		friend auto  MakePromise (Fn &&fn, EThread thread, const Tuple<Deps...> &dependsOn);
		
		template <typename Fn, typename ...Deps>
		friend auto  MakePromise (Fn &&fn, const Tuple<Deps...> &dependsOn);
		
		template <typename ...Types>
		friend auto  MakePromiseFromTuple (const Tuple<Types...> &t, EThread thread);

		template <typename B>
		friend class Promise;
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
		Func_t			_func;
		Result_t		_result;
		const bool		_isExept;


	// methods
	public:
		template <typename Fn>
		_InternalImpl (Fn &&fn, bool except, EThread thread);

		ND_ decltype(auto)  Result () const
		{
			ASSERT( Status() == EStatus::Completed );
			ThreadFence( EMemoryOrder::Acquire );

			if constexpr( not IsVoid<T> )
				return _result.Value();
		}

		ND_ bool  IsExcept () const	{ return _isExept; }

	private:
		void Run () override;
		void OnCancel () override;
	};
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
	struct ResultToPromise< PromiseNullResult > {
		using type = Promise< void >;
	};

}	// _hidden_
//-----------------------------------------------------------------------------



/*
=================================================
	Then
=================================================
*/
	template <typename T>
	template <typename Fn>
	inline auto  Promise<T>::Then (Fn &&fn, EThread thread)
	{
		using FI		= FunctionInfo< Fn >;
		using Result	= typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;

		if_likely( _impl )
		{
			if ( _impl->IsExcept() )
				return _Then( FwdArg<Fn>(fn), thread, Tuple{WeakDep{_impl}} );
			else
				return _Then( FwdArg<Fn>(fn), thread, Tuple{StrongDep{_impl}} );
		}
		return Result{};
	}
	
/*
=================================================
	_Then
=================================================
*/
	template <typename T>
	template <typename Fn, typename ...Deps>
	inline auto  Promise<T>::_Then (Fn &&fn, EThread thread, const Tuple<Deps...> &dependsOn)
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
							dependsOn };
		}
		else
		if constexpr( IsVoid<T> )
		{
			STATIC_ASSERT( FI::args::Count == 0 );
		
			return Result{ FwdArg<Fn>(fn), false, thread, dependsOn };
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
							dependsOn };
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
							dependsOn };
		}
	}

/*
=================================================
	Except
=================================================
*/
	template <typename T>
	template <typename Fn>
	inline auto  Promise<T>::Except (Fn &&fn, EThread thread)
	{
		using FI		= FunctionInfo< Fn >;
		using Result	= typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;
		
		STATIC_ASSERT( FI::args::Count == 0 );
		
		if_likely( _impl )
		{
			if constexpr( IsVoid< typename FI::result > )
			{
				return Result{	[fn = FwdArg<Fn>(fn)] () {
									fn();
									return PromiseResult<void>{};
								},
								true,
								thread,
								Tuple{StrongDep{_impl}} };
			}
			else
			{
				return Result{ FwdArg<Fn>(fn), true, thread, Tuple{StrongDep{_impl}} };
			}
		}
		return Result{};
	}
//-----------------------------------------------------------------------------


/*
=================================================
	MakePromise
=================================================
*/
	template <typename Fn, typename ...Deps>
	forceinline auto  MakePromise (Fn &&fn, IAsyncTask::EThread thread, const Tuple<Deps...> &dependsOn = Default)
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
							dependsOn };
		}
		else
		{
			return Result{ FwdArg<Fn>(fn), false, thread, dependsOn };
		}
	}
	
	template <typename Fn, typename ...Deps>
	ND_ forceinline auto  MakePromise (Fn &&fn, const Tuple<Deps...> &dependsOn = Default)
	{
		return MakePromise( FwdArg<Fn>(fn), IAsyncTask::EThread::Worker, dependsOn );
	}

/*
=================================================
	MakePromiseFromTuple
=================================================
*/
	template <typename ...Types>
	ND_ forceinline auto  MakePromiseFromTuple (const Tuple<Types...> &t, IAsyncTask::EThread thread = IAsyncTask::EThread::Worker)
	{
		return	MakePromise(
					[t] () {
						return	t.Apply( [] (auto&& ...args) {
									return MakeTuple( args._Result() ... );
								});
					},
					thread,
					t.Apply( [] (auto&& ...args) { return MakeTuple( AsyncTask{args}... ); })
				);
	}

}	// AE::Threading
