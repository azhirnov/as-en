// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    AE_SEMAPHORE_MODE:
        0 - WinAPI semaphore
        1 - pthread semaphore
        2 - C++20 semaphore
        3 - emulation
*/

#pragma once

#include "threading/Common.h"

#ifndef AE_SEMAPHORE_MODE
#   define AE_SEMAPHORE_MODE    4
#endif

#if not defined(AE_SEMAPHORE_MODE) and defined(__cpp_lib_semaphore)
#   define AE_SEMAPHORE_MODE    2   // C++20
#endif

#if not defined(AE_SEMAPHORE_MODE) and defined(AE_PLATFORM_WINDOWS)
#   define AE_SEMAPHORE_MODE    0   // winapi
#endif

#if not defined(AE_SEMAPHORE_MODE) and (defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID))
#   define AE_SEMAPHORE_MODE    1   // pthread
#endif

#if not defined(AE_SEMAPHORE_MODE) and defined(AE_PLATFORM_APPLE)
#   define AE_SEMAPHORE_MODE    3   // dispatch_semaphore
#endif


#if (AE_SEMAPHORE_MODE == 0)
namespace AE::Threading
{
namespace _hidden_
{
    class WinSemaphore : public Noncopyable
    {
    // variables
    private:
        void*       _handle     = null;     // HANDLE

    // methods
    protected:
        WinSemaphore (uint desired, uint maxValue)  __NE___;
    public:
        ~WinSemaphore ()                            __NE___;

            void  Acquire ()                        __NE___;
            void  Release (uint update = 1)         __NE___;

        ND_ bool  TryAcquire ()                     __NE___;
    };

} // _hidden_

    template <uint LeastMaxValue>
    class Semaphore : public Threading::_hidden_::WinSemaphore
    {
        StaticAssert( LeastMaxValue > 1 );

    // methods
    public:
        explicit Semaphore (uint desired)       __NE___ : WinSemaphore{ desired, LeastMaxValue } {}

        ND_ static constexpr uint  Max ()       __NE___ { return LeastMaxValue; }
    };

} // AE::Threading
//-----------------------------------------------------------------------------


#elif (AE_SEMAPHORE_MODE == 1)
namespace AE::Threading
{
namespace _hidden_
{
    class PThreadSemaphore : public Noncopyable
    {
    // variables
    private:
        UntypedStorage< sizeof(uint)*4, alignof(uint) >     _sem;   // sem_t

    // methods
    protected:
        PThreadSemaphore (uint desired, uint maxValue)  __NE___;
    public:
        ~PThreadSemaphore ()                            __NE___;

            void  Acquire ()                            __NE___;
            void  Release (uint update = 1)             __NE___;

        ND_ bool  TryAcquire ()                         __NE___;
    };

} // _hidden_

    template <uint LeastMaxValue>
    class Semaphore : public Threading::_hidden_::PThreadSemaphore
    {
        StaticAssert( LeastMaxValue > 1 );

    // methods
    public:
        explicit Semaphore (uint desired)       __NE___ : PThreadSemaphore{ desired, LeastMaxValue } {}

        ND_ static constexpr uint  Max ()       __NE___ { return LeastMaxValue; }
    };

} // AE::Threading
//-----------------------------------------------------------------------------


#elif (AE_SEMAPHORE_MODE == 2)
namespace AE::Threading
{
    template <uint LeastMaxValue>
    class Semaphore : public Noncopyable
    {
        StaticAssert( LeastMaxValue > 1 );

    // variables
    private:
        std::counting_semaphore<LeastMaxValue>  _sem;

    // methods
    public:
        explicit Semaphore (uint desired)       __NE___ : _sem{desired} {}

            void  Acquire ()                    __NE___ { _sem.acquire(); }
            void  Release (uint update = 1)     __NE___ { _sem.release( update ); }

        ND_ bool  TryAcquire ()                 __NE___ { return _sem.try_acquire(); }

        ND_ static constexpr uint  Max ()       __NE___ { return LeastMaxValue; }
    };

} // AE::Threading
//-----------------------------------------------------------------------------


#elif (AE_SEMAPHORE_MODE == 3)
namespace AE::Threading
{
namespace _hidden_
{
    class AppleSemaphore : public Noncopyable
    {
    // variables
    private:
        void *      _sem    = null;     // dispatch_semaphore_t

    // methods
    protected:
        AppleSemaphore (uint desired, uint maxValue)    __NE___;
    public:
        ~AppleSemaphore ()                              __NE___;

            void  Acquire ()                            __NE___;
            void  Release (uint update = 1)             __NE___;

        ND_ bool  TryAcquire ()                         __NE___;
    };

} // _hidden_

    template <uint LeastMaxValue>
    class Semaphore : public Threading::_hidden_::AppleSemaphore
    {
        StaticAssert( LeastMaxValue > 1 );

    // methods
    public:
        explicit Semaphore (uint desired)       __NE___ : AppleSemaphore{ desired, LeastMaxValue } {}

        ND_ static constexpr uint  Max ()       __NE___ { return LeastMaxValue; }
    };

} // AE::Threading
//-----------------------------------------------------------------------------


#elif (AE_SEMAPHORE_MODE == 4)
namespace AE::Threading
{
    template <uint LeastMaxValue>
    class Semaphore : public Noncopyable
    {
        StaticAssert( LeastMaxValue > 1 );

    // variables
    private:
        int                 _counter;
        ConditionVariable   _cv;
        Mutex               _mtx;


    // methods
    public:
        explicit Semaphore (uint desired)       __NE___ : _counter{int(Min( desired, LeastMaxValue ))} { ASSERT( desired <= LeastMaxValue ); }

            void  Acquire ()                    __NE___;
            void  Release (uint update = 1)     __NE___;
        ND_ bool  TryAcquire ()                 __NE___;

        ND_ static constexpr uint  Max ()       __NE___ { return LeastMaxValue; }
    };

    template <uint V>
    void  Semaphore<V>::Acquire () __NE___
    {
        std::unique_lock    lock {_mtx};

        for (; _counter <= 0; ) {
            _cv.wait( lock );
        }
        --_counter;
    }

    template <uint V>
    void  Semaphore<V>::Release (const uint update) __NE___
    {
        EXLOCK( _mtx );

        ASSERT( update >= 1 );
        ASSERT( update <= Max() - _counter );

        _counter += update;

        if ( _counter > 0 )
            _cv.notify_all();
    }

    template <uint V>
    bool  Semaphore<V>::TryAcquire () __NE___
    {
        EXLOCK( _mtx );

        if ( _counter <= 0 )
            return false;

        --_counter;
        return true;
    }

} // AE::Threading
//-----------------------------------------------------------------------------

#else
#   error not supported!

#endif // AE_SEMAPHORE_MODE


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  if AE_SEMAPHORE_MODE == 0
#   pragma detect_mismatch( "AE_SEMAPHORE_MODE", "0" )
#  elif AE_SEMAPHORE_MODE == 1
#   pragma detect_mismatch( "AE_SEMAPHORE_MODE", "1" )
#  elif AE_SEMAPHORE_MODE == 2
#   pragma detect_mismatch( "AE_SEMAPHORE_MODE", "2" )
#  elif AE_SEMAPHORE_MODE == 3
#   pragma detect_mismatch( "AE_SEMAPHORE_MODE", "3" )
#  elif AE_SEMAPHORE_MODE == 4
#   pragma detect_mismatch( "AE_SEMAPHORE_MODE", "4" )
#  else
#   error fix me!
#  endif

#endif // AE_CPP_DETECT_MISMATCH
