// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/FileSystem.h"

namespace AE::Base
{

    //
    // Raw pointer to Path
    //
    template <typename T>
    struct PtrToPath
    {
    // types
    public:
        using Self  = PtrToPath<T>;

    // variables
    private:
        const T*    _ptr    = null;

    // methods
    public:
        PtrToPath (const T* ptr)            __NE___ : _ptr{ptr} {}
        PtrToPath (Self &&)                 __NE___ = default;
        PtrToPath (const Self &)            __NE___ = default;

        ND_ operator Path ()                C_NE___ { return _ptr != null ? Path{_ptr} : Default; }

        ND_ bool  operator == (Self rhs)    C_NE___ { return _ptr == rhs._ptr; }
        ND_ bool  operator != (Self rhs)    C_NE___ { return _ptr != rhs._ptr; }

        ND_ Self  operator + (usize rhs)    C_NE___ { return Self{ _ptr + rhs }; }
    };



    //
    // Raw pointer array to Path
    //
    template <typename T>
    struct PtrToPathArray
    {
    // types
    public:
        using Self  = PtrToPathArray<T>;

    // variables
    private:
        const T* const*     _arr    = null;
        const usize         _count  = 0;

    // methods
    public:
        PtrToPathArray (const T* const* arr, usize count)   __NE___ : _arr{arr}, _count{count} {}

        ND_ operator Array<Path> ()                         C_NE___
        {
            Array<Path>  result {_count};
            for (usize i = 0; i < _count; ++i) { if ( _arr[i] != null ) result[i] = Path{_arr[i]}; }
            return result;
        }
    };



    //
    // Raw pointer array to String
    //
    template <typename T>
    struct PtrToStringArray
    {
    // types
    public:
        using Self  = PtrToStringArray<T>;

    // variables
    private:
        const T* const*     _arr    = null;
        const usize         _count  = 0;

    // methods
    public:
        PtrToStringArray (const T* const* arr, usize count) __NE___ : _arr{arr}, _count{count} {}

        ND_ operator Array<BasicString<T>> ()               C_NE___
        {
            Array<BasicString<T>>  result {_count};
            for (usize i = 0; i < _count; ++i) { if ( _arr[i] != null ) result[i] = BasicString<T>{_arr[i]}; }
            return result;
        }
    };


} // AE::Base
