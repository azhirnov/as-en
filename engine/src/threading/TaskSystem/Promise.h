// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    TODO: moke noexcept + return calnceled task?
*/

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


    static constexpr Threading::_hidden_::PromiseNullResult  CancelPromise = {};



    //
    // Promise Result
    //

    template <typename T>
    struct PromiseResult
    {
        STATIC_ASSERT( not IsSameTypes< Threading::_hidden_::PromiseNullResult, T >);

    // types
    private:
        using NullResult_t = Threading::_hidden_::PromiseNullResult;

    // variables
    private:
        union {
            T           _value;
            ubyte       _data [ sizeof(T) ];    // don't use it!
        };
        bool            _hasValue   = false;


    // methods
    public:
        PromiseResult ()                                                = delete;

        PromiseResult (T &&value)                                       __NE___ : _value{RVRef(value)}, _hasValue{true} {}
        PromiseResult (const T &value)                                  __Th___ : _value{value}, _hasValue{true} {}
        PromiseResult (const NullResult_t &)                            __NE___ {}

        PromiseResult (const PromiseResult<T> &other)                   __Th___;
        PromiseResult (PromiseResult<T> &&other)                        __NE___;

        PromiseResult<T>&  operator = (const PromiseResult<T> &rhs)     __Th___;
        PromiseResult<T>&  operator = (PromiseResult<T> &&rhs)          __NE___;
        PromiseResult<T>&  operator = (const T &rhs)                    __Th___;
        PromiseResult<T>&  operator = (T &&rhs)                         __NE___;

        ~PromiseResult ()                                               __NE___ { STATIC_ASSERT( std::is_nothrow_destructible_v<T> );  if ( _hasValue ) _value.~T(); }

        ND_ bool        HasValue ()                                     C_NE___ { return _hasValue; }
        ND_ T const&    Value ()                                        C_NE___ { ASSERT( _hasValue );  return _value; }
        ND_ T &         Value ()                                        __NE___ { ASSERT( _hasValue );  return _value; }
    };


    template <>
    struct PromiseResult< void >
    {
    // types
    private:
        using NullResult_t = Threading::_hidden_::PromiseNullResult;

    // variables
    private:
        bool        _hasValue   = false;

    // methods
    public:
        PromiseResult ()                                                __NE___ : _hasValue{true} {}

        PromiseResult (const NullResult_t &)                            __NE___ {}

        PromiseResult (const PromiseResult<void> &)                     __NE___ = default;
        PromiseResult (PromiseResult<void> &&)                          __NE___ = default;

        PromiseResult<void>&  operator = (const PromiseResult<void> &)  __NE___ = default;
        PromiseResult<void>&  operator = (PromiseResult<void> &&)       __NE___ = default;

        ND_ bool    HasValue ()                                         C_NE___ { return _hasValue; }
    };



    //
    // Promise
    //

    template <typename T>
    class Promise final
    {
        STATIC_ASSERT( not IsSameTypes< Threading::_hidden_::PromiseNullResult, T >);

    // types
    public:
        using Value_t   = T;
        using Self      = Promise< T >;

        enum class FunctionArg {};
        enum class ValueArg {};
        enum class CompleteValueArg {};

        class _InternalImpl;
        using _InternalImplPtr = RC< _InternalImpl >;


    // variables
    private:
        _InternalImplPtr    _impl;


    // methods
    public:
        Promise ()                                                                  __NE___ {}

        Promise (Self &&)                                                           __NE___ = default;
        Promise (const Self &)                                                      __NE___ = default;

        Self& operator = (Self &&)                                                  __NE___ = default;
        Self& operator = (const Self &)                                             __NE___ = default;

        template <typename Fn>
        auto  Then (Fn &&fn,
                    StringView dbgName      = Default,
                    ETaskQueue queueType    = ETaskQueue::PerFrame)                 __Th___;

        template <typename Fn>
        auto  Except (Fn &&fn,
                      StringView dbgName    = Default,
                      ETaskQueue queueType  = ETaskQueue::PerFrame)                 __Th___;

        bool  Cancel ()                                                             __NE___;

        template <typename Fn>
        bool  WithResult (Fn && fn)                                                 __Th___;

        explicit operator AsyncTask ()                                              C_NE___ { return _impl; }
        explicit operator bool ()                                                   C_NE___ { return bool{_impl}; }

        ND_ IAsyncTask::EStatus  Status ()                                          C_NE___;

    private:
        template <typename Fn>
        auto  _Then (Fn &&fn, StringView, ETaskQueue queueType)                     __Th___;

        template <typename Fn>
        auto  _Except (Fn &&fn, StringView, ETaskQueue queueType)                   __Th___;

        template <typename A>
        Promise (A &&val, bool except, StringView, ETaskQueue, ValueArg)            __Th___;

        template <typename A>
        Promise (A &&val, bool except, StringView, ETaskQueue, CompleteValueArg)    __Th___;

        template <typename Fn>
        Promise (Fn &&fn, bool except, StringView, ETaskQueue, FunctionArg)         __Th___;

        ND_ T  _Result ()                                                           C_NE___;

        template <typename Fn, typename ...Deps>
        friend auto  MakePromise (Fn &&, const Tuple<Deps...> &, StringView, ETaskQueue)        __Th___;

        template <typename ...Args>
        friend auto  MakePromiseFrom (Args&& ...)                                               __Th___;

        template <typename A>
        friend auto  MakePromiseFromArray (Array<Promise<A>>, StringView, ETaskQueue)           __Th___;

        template <typename A>
        friend auto  MakePromiseFromArray (Array< Promise< Array<A> >>, StringView, ETaskQueue) __Th___;

        template <typename Fn>
        friend auto  MakeDelayedPromise (Fn &&fn, StringView, ETaskQueue)                       __Th___;

        template <typename B, typename ...Deps>
        friend auto  MakePromiseFromValue (B &&, const Tuple<Deps...> &, StringView, ETaskQueue) __Th___;

        template <typename B>
        friend auto  MakeDelayedPromiseFromValue (B &&, StringView, ETaskQueue)                 __Th___;

        template <typename B>
        friend class Promise;

        #ifdef AE_HAS_COROUTINE
        template <typename B>
        friend class Threading::_hidden_::PromiseAwaiter;
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
        using Result_t  = PromiseResult< T >;
        using Func_t    = Function< PromiseResult<T> () >;


    // variables
    private:
        Result_t        _result;
        Func_t          _func;
        const bool      _isExept;

      #ifdef AE_DEBUG
        String          _dbgName;
      #endif


    // methods
    public:
        template <typename Fn>
        _InternalImpl (Fn &&fn, bool except, StringView, ETaskQueue, Promise<T>::FunctionArg)           __NE___;

        template <typename A>
        _InternalImpl (A && value, bool except, StringView, ETaskQueue, Promise<T>::ValueArg)           __NE___;

        template <typename A>
        _InternalImpl (A && value, bool except, StringView, ETaskQueue, Promise<T>::CompleteValueArg)   __NE___;

        ND_ decltype(auto)  Result ()                   C_NE___
        {
            ASSERT( Status() == EStatus::Completed );
            MemoryBarrier( EMemoryOrder::Acquire );

            if constexpr( not IsVoid<T> )
                return _result.Value();
        }

        ND_ bool  IsExcept ()                           C_NE___ { return _isExept; }

      #ifdef AE_DEBUG
        StringView  DbgName ()                          C_NE_OV { return _dbgName; }
      #else
        StringView  DbgName ()                          C_NE_OV { return "Promise"; }
      #endif


    private:
        void  Run ()                                    __Th_OV;
        void  OnCancel ()                               __NE_OV;
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
        Promise<T> const&   _promise;

    // methods
    public:
        explicit PromiseAwaiter (const Promise<T> &promise) __NE___ : _promise{promise} {}

        // pause coroutine execution if dependency is not complete
        ND_ bool    await_ready ()                          C_NE___ { return _promise._impl ? _promise._impl->IsFinished() : true; }

        // return promise result
        ND_ T       await_resume ()                         __NE___ { return _promise._Result(); }

        // return task to scheduler with new dependencies
        template <typename P>
        ND_ bool    await_suspend (std::coroutine_handle<P> curCoro) __NE___
        {
            return AsyncTaskCoro_AwaiterImpl::AwaitSuspendImpl( curCoro, _promise._impl );
        }
    };


    template <typename ...Types>
    class PromiseAwaiter< Tuple< Types... >>
    {
    // variables
    private:
        Tuple< Promise<Types>... > const&   _deps;

    // methods
    public:
        explicit PromiseAwaiter (const Tuple<Promise<Types>...> &deps) __NE___ : _deps{deps} {}

        ND_ bool  await_ready ()                                        C_NE___ { return false; }

        // return promise results
        ND_ Tuple< Types... >  await_resume ()                          __NE___
        {
            return  _deps.Apply( [] (auto&& ...args) {
                        return Tuple<Types...>{ args._Result() ... };
                    });
        }

        // return task to scheduler with new dependencies
        ND_ bool  await_suspend (std::coroutine_handle< AsyncTaskCoro::promise_type > curCoro) __NE___
        {
            return AsyncTaskCoro_AwaiterImpl::AwaitSuspendImpl2( curCoro, _deps );
        }
    };

} // _hidden_

/*
=================================================
    operator co_await (promise)
=================================================
*/
    template <typename T>
    ND_ Threading::_hidden_::PromiseAwaiter<T>  operator co_await (const Promise<T> &promise) __NE___
    {
        return Threading::_hidden_::PromiseAwaiter<T>{ promise };
    }

    template <typename ...Types>
    ND_ Threading::_hidden_::PromiseAwaiter<Tuple<Types...>>  operator co_await (const Tuple<Promise<Types>...> &deps) __NE___
    {
        return Threading::_hidden_::PromiseAwaiter<Tuple<Types...>>{ deps };
    }

#endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    template <typename T>
    PromiseResult<T>::PromiseResult (const PromiseResult<T> &other) __Th___ :
        _hasValue{ other._hasValue }
    {
        if ( _hasValue )
            PlacementNew<T>( &_value, other._value );   // throw
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
    PromiseResult<T>&  PromiseResult<T>::operator = (const PromiseResult<T> &rhs) __Th___
    {
        if ( _hasValue )
            _value.~T();

        _hasValue = rhs._hasValue;

        if ( _hasValue )
            PlacementNew<T>( &_value, rhs._value );     // throw

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
    PromiseResult<T>&  PromiseResult<T>::operator = (const T &rhs) __Th___
    {
        if ( _hasValue )
            _value.~T();

        _hasValue = true;
        PlacementNew<T>( &_value, rhs );    // throw

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
    struct ResultToPromise< Threading::_hidden_::PromiseNullResult > {
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
    Promise<T>::Promise (Fn &&fn, bool except, StringView dbgName, ETaskQueue queueType, FunctionArg flag) __Th___ :
        _impl{ MakeRC<_InternalImpl>( FwdArg<Fn>(fn), except, dbgName, queueType, flag )}
    {}

    template <typename T>
    template <typename A>
    Promise<T>::Promise (A &&value, bool except, StringView dbgName, ETaskQueue queueType, ValueArg flag) __Th___ :
        _impl{ MakeRC<_InternalImpl>( FwdArg<A>(value), except, dbgName, queueType, flag )}
    {}

    template <typename T>
    template <typename A>
    Promise<T>::Promise (A &&value, bool except, StringView dbgName, ETaskQueue queueType, CompleteValueArg flag) __Th___ :
        _impl{ MakeRC<_InternalImpl>( FwdArg<A>(value), except, dbgName, queueType, flag )}
    {}

/*
=================================================
    _Then
=================================================
*/
    template <typename T>
    template <typename Fn>
    auto  Promise<T>::_Then (Fn &&fn, StringView dbgName, ETaskQueue queueType) __Th___
    {
        using FI        = FunctionInfo< Fn >;
        using Result    = typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;

        if constexpr( IsVoid<T> and IsVoid< typename FI::result > )
        {
            STATIC_ASSERT( FI::args::Count == 0 );

            return Result{  [fn = FwdArg<Fn>(fn)] () {
                                fn();
                                return PromiseResult<void>{};
                            },
                            false,
                            dbgName,
                            queueType,
                            typename Result::FunctionArg{} };
        }
        else
        if constexpr( IsVoid<T> )
        {
            STATIC_ASSERT( FI::args::Count == 0 );

            return Result{  FwdArg<Fn>(fn),
                            false,
                            dbgName,
                            queueType,
                            typename Result::FunctionArg{} };
        }
        else
        if constexpr( IsVoid< typename FI::result > )
        {
            STATIC_ASSERT( FI::args::Count == 1 );
            STATIC_ASSERT( IsSameTypes< typename FI::args::template Get<0>, const T& >,
                           "argument type must be 'const T&'" );

            return Result{  [fn = FwdArg<Fn>(fn), in = _impl] () {
                                fn( in->Result() );
                                return PromiseResult<void>{};
                            },
                            false,
                            dbgName,
                            queueType,
                            typename Result::FunctionArg{} };
        }
        else
        {
            STATIC_ASSERT( FI::args::Count == 1 );
            STATIC_ASSERT( IsSameTypes< typename FI::args::template Get<0>, const T& >);

            return Result{  [fn = FwdArg<Fn>(fn), in = _impl] () {
                                return fn( in->Result() );
                            },
                            false,
                            dbgName,
                            queueType,
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
    auto  Promise<T>::Then (Fn &&fn, StringView dbgName, ETaskQueue queueType) __Th___
    {
        using FI        = FunctionInfo< Fn >;
        using Result    = typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;

        if_likely( _impl )
        {
          #ifdef AE_DEBUG
            String  temp;
            if ( dbgName.empty() ) {
                (temp = _impl->DbgName()) += "-Then";
                dbgName = temp;
            }
          #endif

            auto    result = _Then( FwdArg<Fn>(fn), dbgName, queueType );   // throw

            if ( _impl->IsExcept() )
            {
                // on error promise will NOT be marked as cancelled
                Scheduler().Run( AsyncTask{result}, Tuple{WeakDep{_impl}} );
            }else{
                // on error promise will be marked as cancelled
                Scheduler().Run( AsyncTask{result}, Tuple{StrongDep{_impl}} );
            }

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
    auto  Promise<T>::_Except (Fn &&fn, StringView dbgName, ETaskQueue queueType) __Th___
    {
        using FI        = FunctionInfo< Fn >;
        using Result    = typename Threading::_hidden_::ResultToPromise< typename FI::result >::type;

        STATIC_ASSERT( FI::args::Count == 0 );

        if constexpr( IsVoid< typename FI::result > )
        {
            return Result{  [fn = FwdArg<Fn>(fn)] () {
                                fn();
                                return PromiseResult<void>{};
                            },
                            true,
                            dbgName,
                            queueType,
                            typename Result::FunctionArg{} };
        }
        else
        {
            return Result{  FwdArg<Fn>(fn),
                            true,
                            dbgName,
                            queueType,
                            typename Result::FunctionArg{} };
        }
    }

    template <typename T>
    template <typename Fn>
    auto  Promise<T>::Except (Fn &&fn, StringView dbgName, ETaskQueue queueType) __Th___
    {
        if_likely( _impl )
        {
          #ifdef AE_DEBUG
            String  temp;
            if ( dbgName.empty() ) {
                (temp = _impl->DbgName()) += "-Except";
                dbgName = temp;
            }
          #endif

            auto    result = _Except( FwdArg<Fn>(fn), dbgName, queueType );  // throw

            // on error promise will be marked as cancelled.
            Scheduler().Run( AsyncTask{result}, Tuple{StrongDep{_impl}} );

            return result;
        }
        else
        {
            using FI     = FunctionInfo< Fn >;
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

/*
=================================================
    Status
=================================================
*/
    template <typename T>
    IAsyncTask::EStatus  Promise<T>::Status () C_NE___
    {
        return _impl ? _impl->Status() : IAsyncTask::EStatus::Initial; 
    }

/*
=================================================
    WithResult
=================================================
*/
    template <typename T>
    template <typename Fn>
    bool  Promise<T>::WithResult (Fn && fn) __Th___
    {
        if ( _impl and _impl->IsCompleted() )
        {
            fn( _impl->Result() );  // may throw
            return true;
        }
        return false;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    template <typename T>
    template <typename Fn>
    Promise<T>::_InternalImpl::_InternalImpl (Fn &&fn, bool except, StringView dbgName, ETaskQueue queueType, Promise<T>::FunctionArg) __NE___ :
        IAsyncTask{ queueType },
        _result{ CancelPromise },
        _func{ FwdArg<Fn>(fn) },
        _isExept{ except }
        #ifdef AE_DEBUG
        , _dbgName{ dbgName.empty() ? "Promise" : dbgName }
        #endif
    {
        ASSERT( _func );
        Unused( dbgName );
    }

    template <typename T>
    template <typename A>
    Promise<T>::_InternalImpl::_InternalImpl (A &&value, bool except, StringView dbgName, ETaskQueue queueType, Promise<T>::ValueArg) __NE___ :
        IAsyncTask{ queueType },
        _result{ FwdArg<A>(value) },
        _isExept{ except }
        #ifdef AE_DEBUG
        , _dbgName{ dbgName.empty() ? "Promise" : dbgName }
        #endif
    {
        Unused( dbgName );
    }

    template <typename T>
    template <typename A>
    Promise<T>::_InternalImpl::_InternalImpl (A &&value, bool except, StringView dbgName, ETaskQueue queueType, Promise<T>::CompleteValueArg) __NE___ :
        IAsyncTask{ queueType },
        _result{ FwdArg<A>(value) },
        _isExept{ except }
        #ifdef AE_DEBUG
        , _dbgName{ dbgName.empty() ? "Promise" : dbgName }
        #endif
    {
        _MakeCompleted();
        Unused( dbgName );
    }

/*
=================================================
    Run
=================================================
*/
    template <typename T>
    void  Promise<T>::_InternalImpl::Run () __Th___
    {
        if_likely( (not _isExept) & bool(_func) )
        {
            _result = _func();  // TODO: may throw?
            _func   = null;

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
            CATCH(
                _result = _func();  // may throw
            )
            _func   = null;
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
    ND_ auto  MakeDelayedPromiseFromValue (T &&         value,
                                           StringView   dbgName     = Default,
                                           ETaskQueue   queueType   = ETaskQueue::PerFrame) __Th___
    {
        STATIC_ASSERT( not std::is_invocable_v<T> );

        using Value_t   = RemoveReference< T >;
        using Result    = typename Threading::_hidden_::ResultToPromise< Value_t >::type;

        STATIC_ASSERT( not IsVoid< Value_t >);

        return Result{  FwdArg<T>(value),
                        false,
                        dbgName,
                        queueType,
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
    ND_ auto  MakePromiseFromValue (T &&                    value,
                                    const Tuple<Deps...> &  dependsOn   = Default,
                                    StringView              dbgName     = Default,
                                    ETaskQueue              queueType   = ETaskQueue::PerFrame) __Th___
    {
        STATIC_ASSERT( not std::is_invocable_v<T> );

        using Value_t   = RemoveReference< T >;
        using Result    = typename Threading::_hidden_::ResultToPromise< Value_t >::type;

        STATIC_ASSERT( not IsVoid< Value_t >);

        if constexpr( sizeof...(Deps) == 0 )
        {
            Unused( dependsOn );

            // return completed promise if there are no dependencies
            return Result{  FwdArg<T>(value),
                            false,
                            dbgName,
                            queueType,
                            typename Result::CompleteValueArg{} };
        }
        else
        {
            Result  result = MakeDelayedPromiseFromValue( FwdArg<T>(value), dbgName, queueType );

            // on error promise will be marked as cancelled
            Unused( Scheduler().Run( AsyncTask{result}, dependsOn ));

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
    ND_ auto  MakeDelayedPromise (Fn &&         fn,
                                  StringView    dbgName     = Default,
                                  ETaskQueue    queueType   = ETaskQueue::PerFrame) __Th___
    {
        STATIC_ASSERT( std::is_invocable_v<Fn> );

        using Value_t   = typename FunctionInfo< Fn >::result;
        using Result    = typename Threading::_hidden_::ResultToPromise< Value_t >::type;

        if constexpr( IsVoid< Value_t >)
        {
            return Result{  [fn = FwdArg<Fn>(fn)] () {
                                fn();
                                return PromiseResult<void>{};
                            },
                            false,
                            dbgName,
                            queueType,
                            typename Result::FunctionArg{} };
        }
        else
        {
            return Result{ FwdArg<Fn>(fn), false, dbgName, queueType, typename Result::FunctionArg{} };
        }
    }

/*
=================================================
    MakePromise
=================================================
*/
    template <typename Fn, typename ...Deps>
    ND_ auto  MakePromise (Fn &&                    fn,
                           const Tuple<Deps...> &   dependsOn   = Default,
                           StringView               dbgName     = Default,
                           ETaskQueue               queueType   = ETaskQueue::PerFrame) __Th___
    {
        auto    result = MakeDelayedPromise( FwdArg<Fn>(fn), dbgName, queueType );

        // on error promise will be marked as cancelled
        Unused( Scheduler().Run( AsyncTask{result}, dependsOn ));

        return result;
    }

/*
=================================================
    MakePromiseFrom
=================================================
*/
    template <typename ...Args>
    ND_ auto  MakePromiseFrom (Args&& ...args) __Th___
    {
        return  MakePromise(
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
    ND_ auto  MakePromiseFromArray (Array<Promise<T>>   args,
                                    StringView          dbgName     = Default,
                                    ETaskQueue          queueType   = ETaskQueue::PerFrame) __Th___
    {
        Array<AsyncTask>    deps;       // TODO: optimize
        deps.reserve( args.size() );

        for (auto& a : args) {
            deps.push_back( AsyncTask{a} );
        }

        return  MakePromise(
                    [data = RVRef(args)] ()
                    {
                        Array<T>    temp;       // TODO: optimize
                        temp.reserve( data.size() );

                        for (auto& d : data) {
                            temp.push_back( d._Result() );
                        }
                        return Tuple{ temp };
                    },
                    Tuple{ deps },
                    dbgName,
                    queueType );
    }

    template <typename T>
    ND_ auto  MakePromiseFromArray (Array< Promise< Array<T> >> args,
                                    StringView                  dbgName     = Default,
                                    ETaskQueue                  queueType   = ETaskQueue::PerFrame) __Th___
    {
        Array<AsyncTask>    deps;       // TODO: optimize
        deps.reserve( args.size() );

        for (auto& a : args) {
            deps.push_back( AsyncTask{a} );
        }

        return  MakePromise(
                    [data = RVRef(args)] ()
                    {
                        Array<T>    temp;       // TODO: optimize
                        temp.reserve( data.size() );

                        for (auto& d : data)
                        {
                            auto    copy = d._Result();
                            for (auto& c : copy) {
                                temp.push_back( RVRef(c) );
                            }
                        }
                        return temp;
                    },
                    Tuple{ deps },
                    dbgName,
                    queueType );
    }


} // AE::Threading
