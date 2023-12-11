// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

// exclusive lock
# ifndef EXLOCK
#   define EXLOCK( .../* sync objects*/ ) \
        std::scoped_lock    AE_PRIVATE_UNITE_RAW( __exLock, __COUNTER__ ) { __VA_ARGS__ }
#endif

// shared lock
#ifndef SHAREDLOCK
#   define SHAREDLOCK( _syncObj_ ) \
        std::shared_lock    AE_PRIVATE_UNITE_RAW( __sharedLock, __COUNTER__ ) { _syncObj_ }
#endif


# ifndef AE_LFAS_ENABLED
namespace AE::Base
{
    using Mutex                 = std::mutex;
    using SharedMutex           = std::shared_mutex;
    using RecursiveMutex        = std::recursive_mutex;
    using ConditionVariable     = std::condition_variable;

    struct EMemoryOrder
    {
        static constexpr std::memory_order  Acquire         = std::memory_order_acquire;
        static constexpr std::memory_order  Release         = std::memory_order_release;
        static constexpr std::memory_order  AcquireRelease  = std::memory_order_acq_rel;
        static constexpr std::memory_order  Relaxed         = std::memory_order_relaxed;
    };

/*
=================================================
    MemoryBarrier
----
    for non-atomic and relaxed atomic accesses.
    MemoryBarrier( Acquire ) - invalidate cache
    MemoryBarrier( Release ) - flush cache
=================================================
*/
    forceinline void  MemoryBarrier (std::memory_order order) __NE___
    {
        ASSERT( order != EMemoryOrder::Relaxed );
        return std::atomic_thread_fence( order );
    }

/*
=================================================
    CompilerBarrier
----
    Take effect only for compiler and CPU instruction reordering.
    CompilerBarrier( Acquire ) - don't reorder with previous code
    CompilerBarrier( Release ) - don't reorder with next code
=================================================
*/
    forceinline void  CompilerBarrier (std::memory_order order) __NE___
    {
        ASSERT( order != EMemoryOrder::Relaxed );
        return std::atomic_signal_fence( order );
    }

} // AE::Base
#endif // AE_LFAS_ENABLED


namespace AE::Base
{
#  ifndef AE_DISABLE_THREADS
    using StdThread             = std::thread;
#  else
    using StdThread             = void;
#  endif
    using ThreadID              = std::thread::id;
    using ThreadHandle          = std::thread::native_handle_type;


    //
    // Defer Exclusive Lock
    //
    template <typename T>
    struct DeferExLock final : std::unique_lock<T>
    {
        explicit DeferExLock (T &mtx) __NE___ :
            std::unique_lock<T>{ mtx, std::defer_lock } {}
    };


    //
    // Defer Shared Lock
    //
    template <typename T>
    struct DeferSharedLock final : std::shared_lock<T>
    {
        explicit DeferSharedLock (T &mtx) __NE___ :
            std::shared_lock<T>{ mtx, std::defer_lock } {}
    };


} // AE::Base
