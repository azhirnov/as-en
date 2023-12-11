// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "CPP_VM/VirtualMachine.h"

namespace LFAS::CPP
{

    using TMemoryOrder = std::memory_order;

    struct EMemoryOrder
    {
        static constexpr std::memory_order  Acquire                 = std::memory_order_acquire;
        static constexpr std::memory_order  Release                 = std::memory_order_release;
        static constexpr std::memory_order  AcquireRelease          = std::memory_order_acq_rel;
        static constexpr std::memory_order  Relaxed                 = std::memory_order_relaxed;
        static constexpr std::memory_order  SequentiallyConsistent  = std::memory_order_seq_cst;
    };

    void  MemoryBarrier (TMemoryOrder);
    void  CompilerBarrier (TMemoryOrder);



    //
    // Atomic
    //

    template <typename T>
    class StdAtomic
    {
        StaticAssert( std::is_trivially_copyable_v<T> );
        StaticAssert( std::is_copy_constructible_v<T> );
        StaticAssert( std::is_move_constructible_v<T> );
        StaticAssert( std::is_copy_assignable_v<T> );
        StaticAssert( std::is_move_assignable_v<T> );

    // types
    public:
        using value_type        = T;

        static constexpr bool   is_always_lock_free = true;


    // variables
    private:
        mutable std::shared_mutex   _guard;
        T                           _value;


    // methods
    public:
        explicit StdAtomic (T initial = T{0});
        ~StdAtomic ();

        StdAtomic (const StdAtomic<T> &) = delete;
        StdAtomic (StdAtomic<T> &&) = delete;

        void operator = (const StdAtomic<T> &) = delete;
        void operator = (StdAtomic<T> &&) = delete;


        void  store (T val, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);

        ND_ T  load (TMemoryOrder order = EMemoryOrder::SequentiallyConsistent) const;

        T  exchange (T newValue, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);

        bool  compare_exchange_weak (INOUT T &expected, T desired, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);
        bool  compare_exchange_strong (INOUT T &expected, T desired, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);

        bool  compare_exchange_weak (INOUT T &expected, T desired, TMemoryOrder success, TMemoryOrder failure);
        bool  compare_exchange_strong (INOUT T &expected, T desired, TMemoryOrder success, TMemoryOrder failure);

        T  fetch_add (T val, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);
        T  fetch_sub (T val, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);

        T  fetch_and (T val, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);
        T  fetch_or  (T val, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);
        T  fetch_xor (T val, TMemoryOrder order = EMemoryOrder::SequentiallyConsistent);

    private:
        static void _Fence (VirtualMachine &vm, TMemoryOrder order);
    };



/*
=================================================
    MemoryBarrier
----
    emulates 'std::atomic_thread_fence'
=================================================
*/
    inline void  MemoryBarrier (TMemoryOrder order)
    {
        VirtualMachine& vm = VirtualMachine::Instance();
        BEGIN_ENUM_CHECKS();
        switch ( order )
        {
            case EMemoryOrder::Acquire :                vm.ThreadFenceAcquire();            break;
            case EMemoryOrder::Release :                vm.ThreadFenceRelease();            break;
            case EMemoryOrder::SequentiallyConsistent :
            case EMemoryOrder::AcquireRelease :         vm.ThreadFenceAcquireRelease();     break;
            case EMemoryOrder::Relaxed :                vm.ThreadFenceRelaxed();            break;
            case std::memory_order::consume :
            default :                                   CHECK( !"unknown memory order" );   break;
        }
        END_ENUM_CHECKS();

        vm.Yield();
    }

/*
=================================================
    CompilerBarrier
----
    emulates 'std::atomic_signal_fence'
=================================================
*/
    inline void  CompilerBarrier (TMemoryOrder)
    {
        CHECK( !"TODO" );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    template <typename T>
    StdAtomic<T>::StdAtomic (T initial) :
        _value{ initial }
    {
        EXLOCK( _guard );
        VirtualMachine::Instance().AtomicCreate( &_value );
    }

/*
=================================================
    destructor
=================================================
*/
    template <typename T>
    StdAtomic<T>::~StdAtomic ()
    {
        EXLOCK( _guard );
        VirtualMachine::Instance().AtomicDestroy( &_value );
    }

/*
=================================================
    store
=================================================
*/
    template <typename T>
    void  StdAtomic<T>::store (T val, TMemoryOrder order)
    {
        auto&   vm = VirtualMachine::Instance();
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            _value = val;
            _Fence( vm, order );
        }
        vm.Yield();
    }

/*
=================================================
    load
=================================================
*/
    template <typename T>
    T  StdAtomic<T>::load (TMemoryOrder order) const
    {
        T       result;
        auto&   vm      = VirtualMachine::Instance();
        {
            SHAREDLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            result = _value;
            _Fence( vm, order );
        }
        vm.Yield();
        return result;
    }

/*
=================================================
    exchange
=================================================
*/
    template <typename T>
    T  StdAtomic<T>::exchange (T newValue, TMemoryOrder order)
    {
        T       result;
        auto&   vm      = VirtualMachine::Instance();
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            result = _value;
            _value = newValue;

            _Fence( vm, order );
        }
        vm.Yield();
        return result;
    }

/*
=================================================
    compare_exchange_weak
=================================================
*/
    template <typename T>
    bool  StdAtomic<T>::compare_exchange_weak (INOUT T &expected, T desired, TMemoryOrder order)
    {
        return compare_exchange_weak( INOUT expected, desired, order, EMemoryOrder::Relaxed );
    }

    template <typename T>
    bool  StdAtomic<T>::compare_exchange_weak (INOUT T &expected, const T desired, TMemoryOrder success, TMemoryOrder failure)
    {
        auto&   vm      = VirtualMachine::Instance();
        bool    result  = true;
        do
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( (success == EMemoryOrder::SequentiallyConsistent) or
                                                          (failure == EMemoryOrder::SequentiallyConsistent) );
            bool    is_equal    = (expected == _value);

            expected = _value;

            if ( not is_equal or vm.AtomicCompareExchangeWeakFalsePositive( &_value ))
            {
                _Fence( vm, failure );
                result = false;
                break;
            }

            _value = desired;
            _Fence( vm, success );

        } while (0);

        vm.Yield();
        return result;
    }

/*
=================================================
    compare_exchange_strong
=================================================
*/
    template <typename T>
    bool  StdAtomic<T>::compare_exchange_strong (INOUT T &expected, T desired, TMemoryOrder order)
    {
        return compare_exchange_strong( INOUT expected, desired, order, EMemoryOrder::Relaxed );
    }

    template <typename T>
    bool  StdAtomic<T>::compare_exchange_strong (INOUT T &expected, const T desired, TMemoryOrder success, TMemoryOrder failure)
    {
        auto&   vm      = VirtualMachine::Instance();
        bool    result  = true;
        do
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( (success == EMemoryOrder::SequentiallyConsistent) or
                                                          (failure == EMemoryOrder::SequentiallyConsistent) );
            bool    is_equal    = (expected == _value);

            expected = _value;

            if ( not is_equal )
            {
                _Fence( vm, failure );
                result = false;
                break;
            }

            _value = desired;
            _Fence( vm, success );

        } while (0);

        vm.Yield();
        return result;
    }

/*
=================================================
    fetch_add
=================================================
*/
    template <typename T>
    T  StdAtomic<T>::fetch_add (const T val, TMemoryOrder order)
    {
        StaticAssert( IsInteger<T> );

        T       result;
        auto&   vm = VirtualMachine::Instance();
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            result  = _value;
            _value += val;

            _Fence( vm, order );
        }
        vm.Yield();
        return result;
    }

/*
=================================================
    fetch_sub
=================================================
*/
    template <typename T>
    T  StdAtomic<T>::fetch_sub (const T val, TMemoryOrder order)
    {
        StaticAssert( IsInteger<T> );

        T       result;
        auto&   vm = VirtualMachine::Instance();
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            result  = _value;
            _value -= val;

            _Fence( vm, order );
        }
        vm.Yield();
        return result;
    }

/*
=================================================
    fetch_and
=================================================
*/
    template <typename T>
    T  StdAtomic<T>::fetch_and (const T val, TMemoryOrder order)
    {
        StaticAssert( IsInteger<T> );

        T       result;
        auto&   vm = VirtualMachine::Instance();
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            result  = _value;
            _value &= val;

            _Fence( vm, order );
        }
        vm.Yield();
        return result;
    }

/*
=================================================
    fetch_or
=================================================
*/
    template <typename T>
    T  StdAtomic<T>::fetch_or (const T val, TMemoryOrder order)
    {
        StaticAssert( IsInteger<T> );

        T       result;
        auto&   vm = VirtualMachine::Instance();
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            result  = _value;
            _value |= val;

            _Fence( vm, order );
        }
        vm.Yield();
        return result;
    }

/*
=================================================
    fetch_xor
=================================================
*/
    template <typename T>
    T  StdAtomic<T>::fetch_xor (const T val, TMemoryOrder order)
    {
        StaticAssert( IsInteger<T> );

        T       result;
        auto&   vm = VirtualMachine::Instance();
        {
            EXLOCK( _guard );
            auto    global_lock = vm.GetAtomicGlobalLock( order == EMemoryOrder::SequentiallyConsistent );

            result  = _value;
            _value ^= val;

            _Fence( vm, order );
        }
        vm.Yield();
        return result;
    }

/*
=================================================
    _Fence
=================================================
*/
    template <typename T>
    void  StdAtomic<T>::_Fence (VirtualMachine &vm, TMemoryOrder order)
    {
        BEGIN_ENUM_CHECKS();
        switch ( order )
        {
            case EMemoryOrder::Acquire :                vm.ThreadFenceAcquire();            break;
            case EMemoryOrder::Release :                vm.ThreadFenceRelease();            break;
            case EMemoryOrder::SequentiallyConsistent :
            case EMemoryOrder::AcquireRelease :         vm.ThreadFenceAcquireRelease();     break;
            case EMemoryOrder::Relaxed :                vm.ThreadFenceRelaxed();            break;
            default :                                   CHECK( !"unknown memory order" );   break;
        }
        END_ENUM_CHECKS();
    }


} // LFAS::CPP

#include "base/Utils/Atomic.h"
