// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    This virtual machine emulates C++ memory model that described in standard.
    https://en.cppreference.com/w/cpp/atomic/memory_order

    Can be used for testing lock-free algorithms.
*/

#pragma once

#include "base/Defines/StdInclude.h"
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>

#include "base/Utils/Helpers.h"
#include "base/Containers/ArrayView.h"
#include "base/Math/Random.h"

#include "Utils/MemRanges.h"

// exclusive lock
#ifndef EXLOCK
#   define EXLOCK( ... ) \
        std::scoped_lock    AE_PRIVATE_UNITE_RAW( __scopeLock, __COUNTER__ ) { __VA_ARGS__ }
#endif

// shared lock
#ifndef SHAREDLOCK
#   define SHAREDLOCK( _syncObj_ ) \
        std::shared_lock    AE_PRIVATE_UNITE_RAW( __sharedLock, __COUNTER__ ) { _syncObj_ }
#endif


namespace LFAS::CPP
{

    //
    // Virtual Machine
    //

    class VirtualMachine final : public Noncopyable
    {
    // types
    public:
        struct AtomicGlobalLock
        {
        private:
            std::mutex *    _lock   = null; // global lock for all atomics
            bool            _locked = false;

        public:
            AtomicGlobalLock () {}
            explicit AtomicGlobalLock (std::mutex *lock);
            AtomicGlobalLock (const AtomicGlobalLock &) = delete;
            AtomicGlobalLock (AtomicGlobalLock &&);
            ~AtomicGlobalLock ();
        };


        class Script;
        using ScriptPtr = SharedPtr<Script>;
        using ScriptFn  = Function< void () >;


    private:
        struct AtomicInfo
        {
            bool        isDestroyed = true;
        };
        using AtomicMap_t   = HashMap< const void*, AtomicInfo >;

        using ThreadID_t    = std::thread::id;
        using MemRanges     = MemRangesTempl< ThreadID_t >;

        struct StorageInfo
        {
            Bytes       size;
            MemRanges   ranges;
            bool        isDestroyed = true;

            explicit StorageInfo (Bytes size) : size{size} {}
        };
        using StorageMap_t  = HashMap< const void*, StorageInfo >;


    // variables
    private:
        std::mutex                  _atomicGlobalGuard;     // used for Sequentially-consistent ordering

        std::shared_mutex           _atomicMapGuard;
        AtomicMap_t                 _atomicMap;

        mutable std::shared_mutex   _storageMapGuard;
        StorageMap_t                _storageMap;

        std::mutex                  _randomGuard;
        Random                      _random;

        std::atomic<uint>           _scriptCounter  {0};


    // methods
    private:
        VirtualMachine ();
        ~VirtualMachine ();

    public:
        template <typename FN>
        ND_ ScriptPtr  CreateScript (FN &&fn);
        template <typename FN>
        ND_ ScriptPtr  CreateScript (StringView name, FN &&fn);
        ND_ ScriptPtr  CreateScript (StringView name, ScriptFn &&fn);

        void  RunParallel (ArrayView<ScriptPtr> scripts, secondsf timeout);


    // memory order
        void  ThreadFenceAcquire ();
        void  ThreadFenceRelease ();
        void  ThreadFenceAcquireRelease ();
        void  ThreadFenceRelaxed ();

        void  Yield ();

        void  CheckForUncommittedChanges () const;


    // atomics
        void  AtomicCreate (const void *ptr);
        void  AtomicDestroy (const void *ptr);

        ND_ bool  AtomicCompareExchangeWeakFalsePositive (const void *ptr);

        ND_ AtomicGlobalLock  GetAtomicGlobalLock (bool isSequentiallyConsistent);


    // memory ranges
        void  StorageCreate (const void *ptr, Bytes size);
        void  StorageDestroy (const void *ptr);
        void  StorageReadAccess (const void *ptr, Bytes offset, Bytes size);
        void  StorageWriteAccess (const void *ptr, Bytes offset, Bytes size);


    // instance
        static bool  CreateInstance ();
        static bool  DestroyInstance ();
        ND_ static VirtualMachine&  Instance ();
    };



    template <typename FN>
    inline VirtualMachine::ScriptPtr  VirtualMachine::CreateScript (StringView name, FN &&fn)
    {
        return CreateScript( name, ScriptFn{FwdArg<FN>(fn)} );
    }

    template <typename FN>
    inline VirtualMachine::ScriptPtr  VirtualMachine::CreateScript (FN &&fn)
    {
        return CreateScript( Default, ScriptFn{FwdArg<FN>(fn)} );
    }

} // LFAS::CPP
