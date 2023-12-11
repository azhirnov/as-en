// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    AE_BARRIER_MODE:
        0 - WinAPI native barrier implementation, requires Windows 8 desktop.
        1 - implementation based only on atomics.
        2 - implementation based on boost::fibers::barrier, shows same performance as native WinAPI barrier.
        3 - wrapper around std::barrier, requires C++20.
*/

#pragma once

#include "threading/Common.h"

// even if __cpp_lib_barrier is supported it requires macos 11.0
#if defined(AE_PLATFORM_MACOS) and (AE_PLATFORM_TARGET_VERSION_MAJOR < 11)
#   define AE_BARRIER_MODE  2
#endif

#if not defined(AE_BARRIER_MODE) and defined(__cpp_lib_barrier)
#   define AE_BARRIER_MODE  3
#endif

#if not defined(AE_BARRIER_MODE) and defined(AE_PLATFORM_WINDOWS) and (AE_PLATFORM_TARGET_VERSION_MAJOR >= 8)
#   define AE_BARRIER_MODE  0
#endif

#ifndef AE_BARRIER_MODE
#   define AE_BARRIER_MODE  2   // or 1
#endif


#if (AE_BARRIER_MODE == 0)

namespace AE::Threading
{

    //
    // Barrier (requires Windows 8)
    //

    class Barrier : public Noncopyable
    {
    // variables
    private:
        UntypedStorage< 32, 8 >     _data;


    // methods
    public:
        explicit Barrier (usize numThreads)     __NE___;
        ~Barrier ()                             __NE___;

        Barrier (Barrier &&)                    = delete;
        Barrier (const Barrier &)               = delete;

        Barrier&  operator = (const Barrier &)  = delete;
        Barrier&  operator = (Barrier &&)       = delete;

        void  wait ()                           __NE___;

        ND_ static constexpr usize  max ()      __NE___ { return uint{UMax}; }
    };

} // AE::Threading


#elif (AE_BARRIER_MODE == 1)

namespace AE::Threading
{

    //
    // Barrier
    //

    class alignas(AE_CACHE_LINE) Barrier : public Noncopyable
    {
    // types
    private:
        struct Bitfield {
            uint    counter_1   : 15;
            uint    counter_2   : 15;
            uint    index       : 1;
        };


    // variables
    private:
        Atomic< Bitfield >  _counter;
        const uint          _numThreads;


    // methods
    public:
        explicit Barrier (usize numThreads)     __NE___ :
            _counter{Bitfield{ 0, 0, 0 }}, _numThreads{CheckCast<uint>(numThreads)}
        {
            ASSERT( numThreads > 0 );
        }

        Barrier (Barrier &&)                    = delete;
        Barrier (const Barrier &)               = delete;

        Barrier&  operator = (const Barrier &)  = delete;
        Barrier&  operator = (Barrier &&)       = delete;

        void  wait ()                           __NE___
        {
            // flush cache
            MemoryBarrier( EMemoryOrder::Release );

            const Bitfield  old_value   = _counter.load();
            Bitfield        expected    = old_value;
            Bitfield        new_value   = old_value;

            // increment counter
            old_value.index ? ++new_value.counter_2 : ++new_value.counter_1;

            for (; not _counter.CAS( INOUT expected, new_value );)
            {
                new_value = expected;
                old_value.index ? ++new_value.counter_2 : ++new_value.counter_1;

                ThreadUtils::Pause();
            }


            // wait for other threads
            new_value.index = ~old_value.index;
            if ( old_value.index ) {
                new_value.counter_1 = expected.counter_1;
                new_value.counter_2 = 0;
                expected.counter_2  = _numThreads;
            }else{
                new_value.counter_1 = 0;
                new_value.counter_2 = expected.counter_2;
                expected.counter_1  = _numThreads;
            }

            for (uint p = 0;; ++p)
            {
                for (uint i = 0; i < ThreadUtils::SpinBeforeLock(); ++i)
                {
                    if_likely(  _counter.CAS( INOUT expected, new_value ) or
                                expected.index != old_value.index )
                    {
                        // invalidate cache
                        MemoryBarrier( EMemoryOrder::Acquire );
                        return;
                    }

                    old_value.index ? (expected.counter_2 = _numThreads) : (expected.counter_1 = _numThreads);

                    ThreadUtils::Pause();
                }
                ThreadUtils::ProgressiveSleep( p );
            }
        }

        ND_ static constexpr usize  max ()      __NE___ { return uint{UMax}; }
    };

} // AE::Threading


#elif (AE_BARRIER_MODE == 2)

namespace AE::Threading
{

    //
    // Barrier (based on boost::fibers::barrier)
    //

    class Barrier : public Noncopyable
    {
    // variables
    private:
        usize                   _value;
        usize                   _cycle;
        const usize             _numThreads;
        Mutex                   _mutex;
        std::condition_variable _cv;


    // methods
    public:
        explicit Barrier (usize numThreads)     __NE___:
            _value{numThreads}, _cycle{0}, _numThreads{numThreads}
        {
            ASSERT( numThreads > 0 );
        }

        Barrier (Barrier &&)                    = delete;
        Barrier (const Barrier &)               = delete;

        Barrier&  operator = (const Barrier &)  = delete;
        Barrier&  operator = (Barrier &&)       = delete;

        void  wait ()                           __NE___
        {
            std::unique_lock    lock{ _mutex };

            if ( (--_value) == 0 )
            {
                ++_cycle;
                _value = _numThreads;

                lock.unlock();
                _cv.notify_all();
                return;
            }

            _cv.wait( lock, [this, cycle = _cycle] () { return cycle != _cycle; });
        }

        ND_ static constexpr usize  max ()      __NE___ { return UMax; }
    };

} // AE::Threading


#elif (AE_BARRIER_MODE == 3)

namespace AE::Threading
{

    //
    // Barrier (wraps std::barrier from C++20)
    //

    class Barrier : public Noncopyable
    {
    // variables
    private:
        std::barrier<>  _barrier;


    // methods
    public:
        explicit Barrier (ssize numThreads)     __NE___ : _barrier{ptrdiff_t(numThreads)}
        {
            ASSERT( numThreads > 0 );
        }

        Barrier (Barrier &&)                    = delete;
        Barrier (const Barrier &)               = delete;

        Barrier&  operator = (const Barrier &)  = delete;
        Barrier&  operator = (Barrier &&)       = delete;

        void  wait ()                           __NE___
        {
            _barrier.arrive_and_wait();
        }

        ND_ static constexpr ssize  max ()      __NE___ { return std::barrier<>::max(); }
    };

} // AE::Threading

#else
#   error not supported!

#endif // AE_BARRIER_MODE


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  if AE_BARRIER_MODE == 0
#   pragma detect_mismatch( "AE_BARRIER_MODE", "0" )
#  elif AE_BARRIER_MODE == 1
#   pragma detect_mismatch( "AE_BARRIER_MODE", "1" )
#  elif AE_BARRIER_MODE == 2
#   pragma detect_mismatch( "AE_BARRIER_MODE", "2" )
#  elif AE_BARRIER_MODE == 3
#   pragma detect_mismatch( "AE_BARRIER_MODE", "3" )
#  else
#   error fix me!
#  endif

#endif // AE_CPP_DETECT_MISMATCH
