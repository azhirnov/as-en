// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Common.h"

namespace AE::ECS
{

    template <typename T>
    struct WriteAccess
    {
        STATIC_ASSERT( not IsEmpty<T> );
    private:
        T*  _elements;

    public:
        explicit WriteAccess (T* elems) __NE___ : _elements{ elems } { ASSERT( _elements ); }

        ND_ T&  operator [] (usize i)   C_NE___ { return _elements[i]; }
    };


    template <typename T>
    struct ReadAccess
    {
        STATIC_ASSERT( not IsEmpty<T> );
    private:
        T const*  _elements;

    public:
        explicit ReadAccess (T const* elems)    __NE___ : _elements{ elems } { ASSERT( _elements ); }

        ND_ T const&  operator [] (usize i)     C_NE___ { return _elements[i]; }
    };


    template <typename T>
    struct OptionalWriteAccess
    {
        STATIC_ASSERT( not IsEmpty<T> );
    private:
        T*  _elements;          // can be null

    public:
        explicit OptionalWriteAccess (T* elems) __NE___ : _elements{ elems } {}

        ND_ T&  operator [] (usize i)           C_NE___ { ASSERT( _elements != null ); return _elements[i]; }
        ND_ explicit operator bool ()           C_NE___ { return _elements != null; }
    };


    template <typename T>
    struct OptionalReadAccess
    {
        STATIC_ASSERT( not IsEmpty<T> );
    private:
        T const*  _elements;    // can be null

    public:
        explicit OptionalReadAccess (T const* elems)    __NE___ : _elements{ elems } {}

        ND_ T const&  operator [] (usize i)             C_NE___ { ASSERT( _elements != null ); return _elements[i]; }
        ND_ explicit operator bool ()                   C_NE___ { return _elements != null; }
    };


    template <typename ...Types>
    struct Subtractive {};

    template <typename ...Types>
    struct Require {};

    template <typename ...Types>
    struct RequireAny {};


} // AE::ECS
