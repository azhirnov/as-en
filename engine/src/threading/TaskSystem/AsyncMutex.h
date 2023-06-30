// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/RingBuffer.h"
#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

    //
    // Async Mutex
    //

    class AsyncMutex
    {
    // vaariables
    private:
        AtomicRC< IAsyncTask >      _currentTask    {null};


    // methods
    public:
        AsyncMutex ()                               __NE___ {}
        ~AsyncMutex ()                              __NE___ {}

        ND_ AsyncTask  TryLock (IAsyncTask* task)   __NE___
        {
            AsyncTask   exp;
            Unused( _currentTask.CAS_Loop( INOUT exp, task ));
            return exp;
        }

        bool  Unlock (IAsyncTask* task)             __NE___
        {
            AsyncTask   exp = task;
            return _currentTask.CAS_Loop( INOUT exp, null );
        }

        ND_ bool  IsLocked ()                       __NE___
        {
            return _currentTask.unsafe_get() != null;
        }
    };



    //
    // Async Mutex V2
    //
    /*
    class AsyncMutexV2
    {
    // vaariables
    private:
        Mutex                   _guard;
        RingBuffer<AsyncTask>   _taskList;


    // methods
    public:
        AsyncMutexV2 ()                             __NE___ {}
        ~AsyncMutexV2 ()                                __NE___ {}

        ND_ bool  LockOrWait (AsyncTask task)   __NE___
        {
            EXLOCK( _guard );

            _taskList.push_back( RVRef(task) );
        }
    };



    //
    // Async Shared Mutex
    //
    /*
    class AsyncSharedMutex
    {
    // vaariables
    private:
        Atomic< ssize >             _readCnt        {0};
        AtomicRC< IAsyncTask >      _currentTask    {null};


    // methods
    public:
        AsyncSharedMutex ()                             __NE___ {}
        ~AsyncSharedMutex ()                            __NE___ {}

        ND_ AsyncTask  TryLock (IAsyncTask* task)       __NE___
        {
            AsyncTask   exp;
            Unused( _currentTask.CAS_Loop( INOUT exp, task ));
            return exp;
        }

        bool  Unlock (IAsyncTask* task)                 __NE___
        {
            AsyncTask   exp = task;
            return _currentTask.CAS_Loop( INOUT exp, null );
        }

        ND_ bool  IsLocked ()                           __NE___
        {
            return _currentTask.unsafe_get() != null;
        }

        ND_ AsyncTask  TryLockShared (IAsyncTask* task) __NE___
        {
            AsyncTask   exp;
            Unused( _currentTask.CAS_Loop( INOUT exp, task ));
            return exp;
        }

        bool  UnlockShared (IAsyncTask* task)           __NE___
        {
            AsyncTask   exp = task;
            return _currentTask.CAS_Loop( INOUT exp, null );
        }
    };*/
//-----------------------------------------------------------------------------

namespace _hidden_
{

    //
    // Async Exclusive Lock
    //
    template <typename AsyncMutexType>
    struct AsyncExclusiveLock
    {
    private:
        IAsyncTask *        _cur;
        AsyncMutexType &    _am;

    public:
        explicit AsyncExclusiveLock (AsyncMutexType& am, IAsyncTask* cur)   __NE___ :
            _cur{cur}, _am{am}
        {}

        ND_ AsyncTask  Lock ()                                              __NE___
        {
            AsyncTask   res = _am.TryLock( _cur );
            if ( res )
                _cur = null;    // not locked
            return res;
        }

        ~AsyncExclusiveLock ()                                              __NE___
        {
            if ( _cur != null )
                _am.Unlock( _cur );
        }
    };


    //
    // Async Shared Lock
    //
    template <typename AsyncMutexType>
    struct AsyncSharedLock
    {
    private:
        IAsyncTask *        _cur;
        AsyncMutexType &    _am;

    public:
        explicit AsyncSharedLock (AsyncMutexType& am, IAsyncTask* cur)  __NE___ :
            _cur{cur}, _am{am}
        {}

        ND_ AsyncTask  Lock ()                                          __NE___
        {
            AsyncTask   res = _am.TryLockShared( _cur );
            if ( res )
                _cur = null;    // not locked
            return res;
        }

        ~AsyncSharedLock ()                                             __NE___
        {
            if ( _cur != null )
                _am.UnlockShared( _cur );
        }
    };

} // _hidden_


#   define AE_PRIVATE_ASYNC_EXCLUSIVE_LOCK( _amutex_, _index_ )                                                         \
        AE::Threading::_hidden_::AsyncExclusiveLock  AE_PRIVATE_UNITE_RAW( __asyncExLock, _index_ ){ _amutex_, this };  \
                                                                                                                        \
        STATIC_ASSERT( AE::Base::IsBaseOf< AE::Threading::IAsyncTask, AE::Base::RemoveAllQualifiers<decltype(*this)> >);\
        ASSERT( this->DbgIsRunning() );                                                                                 \
        ASSERT( AE::Base::StringView{"Run"} == AE_FUNCTION_NAME );                                                      \
                                                                                                                        \
        if ( AE::Threading::AsyncTask  task = AE_PRIVATE_UNITE_RAW( __asyncExLock, _index_ ).Lock() ) {                 \
            return this->Continue( Tuple{task} );                                                                       \
        }

#   define AE_PRIVATE_ASYNC_SHARED_LOCK( _amutex_, _index_ )                                                            \
        AE::Threading::_hidden_::AsyncExclusiveLock  AE_PRIVATE_UNITE_RAW( __asyncShLock, _index_ ){ _amutex_, this };  \
                                                                                                                        \
        STATIC_ASSERT( AE::Base::IsBaseOf< AE::Threading::IAsyncTask, AE::Base::RemoveAllQualifiers<decltype(*this)> >);\
        ASSERT( this->DbgIsRunning() );                                                                                 \
        ASSERT( AE::Base::StringView{"Run"} == AE_FUNCTION_NAME );                                                      \
                                                                                                                        \
        if ( AE::Threading::AsyncTask  task = AE_PRIVATE_UNITE_RAW( __asyncShLock, _index_ ).Lock() ) {                 \
            return this->Continue( Tuple{task} );                                                                       \
        }

#   define ASYNC_EXLOCK( _amutex_ ) \
        AE_PRIVATE_ASYNC_EXCLUSIVE_LOCK( _amutex_, __COUNTER__ )

#   define ASYNC_SHAREDLOCK( _amutex_ ) \
        AE_PRIVATE_ASYNC_SHARED_LOCK( _amutex_, __COUNTER__ )


} // AE::Threading
