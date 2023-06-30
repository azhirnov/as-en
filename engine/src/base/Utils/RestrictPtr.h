// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

    //
    // Restrict (unique) Pointer
    //

    template <typename T>
    class RstPtr final
    {
    // types
    public:
        using Self  = RstPtr<T>;


    // variables
    private:
      #if defined(AE_COMPILER_MSVC)
        T * __restrict      _ptr    = null;

      #elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
        T * __restrict__    _ptr    = null;

      #else
        T *                 _ptr    = null;
      # pragma message("'restrict' attribute is not supported")
      #endif


    // methods
    public:
        RstPtr ()                                       __NE___ {}
        explicit RstPtr (T* ptr)                        __NE___ : _ptr{ptr} {}
        RstPtr (std::nullptr_t)                         __NE___ {}

        RstPtr (Self &&)                                __NE___ = default;
        Self&  operator = (Self &&)                     __NE___ = default;

        RstPtr (const Self &)                           __NE___ = default;
        Self&  operator = (const Self &)                __NE___ = default;

        Self&  operator = (T* ptr)                      __NE___ { _ptr = ptr;  return *this; }

        ND_ explicit operator bool ()                   C_NE___ { return _ptr != null; }


      #if defined(AE_COMPILER_MSVC)

        ND_ __declspec(restrict) operator T* ()         __NE___ { return _ptr; }
        ND_ __declspec(restrict) operator T const* ()   C_NE___ { return _ptr; }

        ND_ __declspec(restrict) T*         get ()      __NE___ { return _ptr; }
        ND_ __declspec(restrict) T const*   get ()      C_NE___ { return _ptr; }


      #elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)

        ND_ operator T* __restrict__ ()                 __NE___ { return _ptr; }
        ND_ operator T const* __restrict__ ()           C_NE___ { return _ptr; }

        ND_ T* __restrict__                 get ()      __NE___ { return _ptr; }
        ND_ T const* __restrict__           get ()      C_NE___ { return _ptr; }


      #else
        ND_ operator T* ()                              __NE___ { return _ptr; }
        ND_ operator T const* ()                        C_NE___ { return _ptr; }

        ND_ T*                              get ()      __NE___ { return _ptr; }
        ND_ T const*                        get ()      C_NE___ { return _ptr; }

      #endif
    };


} // AE::Base
