// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

    //
    // On Destroy
    //
    struct OnDestroy
    {
    private:
        Function<void ()>   _fn;

    public:
        explicit OnDestroy (Function<void ()> &&fn)         __NE___ : _fn{ RVRef(fn) } {}
        ~OnDestroy ()                                       __NE___ { _fn(); }
    };
#   define ON_DESTROY( _fn_ )   AE::Base::OnDestroy  AE_PRIVATE_UNITE_RAW( __atFnExit, __COUNTER__ ) { _fn_ }



    //
    // Non-copyable
    //
    class Noncopyable
    {
    public:
        Noncopyable ()                                      __NE___ = default;

        Noncopyable (const Noncopyable &)                           = delete;
        Noncopyable (Noncopyable &&)                                = delete;

        Noncopyable& operator = (const Noncopyable &)               = delete;
        Noncopyable& operator = (Noncopyable &&)                    = delete;
    };



    //
    // Movable Only
    //
    class MovableOnly
    {
    public:
        MovableOnly ()                                      __NE___ = default;

        MovableOnly (MovableOnly &&)                        __NE___ = default;
        MovableOnly& operator = (MovableOnly &&)            __NE___ = default;

        MovableOnly (const MovableOnly &)                           = delete;
        MovableOnly& operator = (const MovableOnly &)               = delete;
    };



    //
    // Non-instanceable base class
    //
    class Noninstanceable
    {
    protected:
        Noninstanceable ()                                          = delete;
        //~Noninstanceable ()                                       = delete;

        Noninstanceable (const Noninstanceable &)                   = delete;
        Noninstanceable (Noninstanceable &&)                        = delete;

        Noninstanceable& operator = (const Noninstanceable &)       = delete;
        Noninstanceable& operator = (Noninstanceable &&)            = delete;
    };



    //
    // Non Allocatable
    //
    class NonAllocatable
    {
    private:
        ND_ static void*  operator new   (usize)                                __NE___ { return null; }
        ND_ static void*  operator new[] (usize)                                __NE___ { return null; }
        ND_ static void*  operator new   (usize, std::align_val_t)              __NE___ { return null; }
        ND_ static void*  operator new[] (usize, std::align_val_t)              __NE___ { return null; }

    public:
            static void   operator delete   (void*)                             __NE___ {}
            static void   operator delete[] (void*)                             __NE___ {}
            static void   operator delete   (void*, std::align_val_t)           __NE___ {}
            static void   operator delete[] (void*, std::align_val_t)           __NE___ {}
            static void   operator delete   (void*, usize)                      __NE___ {}
            static void   operator delete[] (void*, usize)                      __NE___ {}
            static void   operator delete   (void*, usize, std::align_val_t)    __NE___ {}
            static void   operator delete[] (void*, usize, std::align_val_t)    __NE___ {}
    };



    //
    // Nothrow Allocatable
    //
    class NothrowAllocatable
    {
    public:
        ND_ static void*  operator new   (usize size)                           __NE___ { return ::operator new( size, std::nothrow ); }
        ND_ static void*  operator new[] (usize size)                           __NE___ { return ::operator new( size, std::nothrow ); }
        ND_ static void*  operator new   (usize size, std::align_val_t align)   __NE___ { return ::operator new( size, align, std::nothrow ); }
        ND_ static void*  operator new[] (usize size, std::align_val_t align)   __NE___ { return ::operator new( size, align, std::nothrow ); }

            // placement new
        ND_ static void*  operator new (usize, void* where)                     __NE___ { return where; }
    };



    //
    // Scoped Set Value
    //
    template <typename T>
    class ScopedSet : public Noncopyable
    {
    private:
        T &     _ref;
        T       _finalValue;

    public:
        ScopedSet (T& ref, T initial, T final)                  __NE___ :
            _ref{ref}, _finalValue{RVRef(final)}
        {
            _ref = RVRef(initial);
        }

        ~ScopedSet ()                                           __NE___
        {
            _ref = RVRef(_finalValue);
        }
    };

#   define SCOPED_SET( _ref_, _initial_, _final_ ) \
        AE::Base::ScopedSet     AE_PRIVATE_UNITE_RAW( __scopedSet, __COUNTER__ ) { _ref_, _initial_, _final_ }



    //
    // As Pointer
    //
    template <typename T>
    struct AsPointer
    {
    private:
        T   _value;

    public:
        template <typename B>
        explicit AsPointer (B &&val)                            __NE___ : _value{ FwdArg<B>(val) } {}

        AsPointer ()                                                    = delete;
        AsPointer (AsPointer<T> &&)                             __NE___ = default;
        AsPointer (const AsPointer<T> &)                        __NE___ = default;

        AsPointer<T>&  operator = (AsPointer<T> &&)             __NE___ = default;
        AsPointer<T>&  operator = (const AsPointer<T> &)        __NE___ = default;

        ND_ explicit constexpr operator bool ()                 C_NE___ { return true; }

        ND_ T*          operator -> ()                          __NE___ { return &_value; }
        ND_ T const*    operator -> ()                          C_NE___ { return &_value; }
    };

    namespace _hidden_
    {
        template <typename T>
        struct _ToPtr {
            using type  = AsPointer<T>;
        };

        template <typename T>
        struct _ToPtr< T* > {
            using type  = T*;
        };
    }

    template <typename T>
    using ToPointer = typename Base::_hidden_::_ToPtr<T>::type;



    //
    // All Combinations
    //
    template <auto FirstValue, auto AllBits>
    struct AllCombinationsInRange
    {
    private:
        using EnumType = decltype(AllBits);

        struct Iterator
        {
            template <auto A, auto B>
            friend struct AllCombinationsInRange;

        private:
            EnumType    _value;

            explicit Iterator (EnumType val)                    __NE___ : _value{val} {}

        public:
            Iterator (const Iterator &)                         __NE___ = default;

                Iterator&   operator ++ ()                      __NE___ { _value = EnumType( ulong(_value) + 1 );  return *this; }
                Iterator    operator ++ (int)                   __NE___ { auto tmp = _value;  ++(*this);  return Iterator{ tmp }; }
            ND_ EnumType    operator * ()                       C_NE___ { return _value; }

            ND_ bool        operator == (Iterator rhs)          C_NE___ { return _value == rhs._value; }
            ND_ bool        operator != (Iterator rhs)          C_NE___ { return _value != rhs._value; }
        };


    public:
        ND_ Iterator    begin ()                                __NE___ { return Iterator{ FirstValue }; }
        ND_ Iterator    end ()                                  __NE___ { return Iterator{ AllBits }; }
    };

    template <auto AllBits>
    struct AllCombinations : AllCombinationsInRange< decltype(AllBits){0}, AllBits >
    {};


} // AE::Base
