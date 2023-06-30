// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Exceptions:
        - internal object may throw exceptions (in copy-ctor)
*/

#pragma once

#include "base/Memory/MemUtils.h"

namespace AE::Base
{

    //
    // In Place Storage
    //

    template <typename T>
    struct InPlace final
    {
    // types
    public:
        using Self      = InPlace< T >;
        using Value_t   = T;


    // variables
    private:
        union {
            T           _value;
            ubyte       _unused [ sizeof(T) ];  // don't use it!
        };
        DEBUG_ONLY(
            bool        _isCreated = false;
        )


    // methods
    public:
        InPlace ()                                          __NE___
        {
            DEBUG_ONLY( DbgFreeMem( OUT _value ));
        }

        InPlace (const Self &other)                         noexcept(IsNothrowCopyCtor<T>) :
            _value{ other.AsRef() }
            DEBUG_ONLY(, _isCreated{ true })
        {}

        InPlace (Self && other)                             __NE___ :
            _value{ other.AsRVRef() }
            DEBUG_ONLY(, _isCreated{ true })
        {}

        ~InPlace ()                                         __NE___
        {
            STATIC_ASSERT( alignof(Self) >= alignof(T) );
            ASSERT( not _isCreated );
        }


        Self&  operator = (const Self &rhs)                 = delete;
        Self&  operator = (Self&& rhs)                      = delete;


        template <typename ...Args>
        Self&  Create (Args&& ...args)                      noexcept(IsNothrowCtor<T, Args...>)
        {
            ASSERT( not _isCreated );
            new (&_value) T{ FwdArg<Args &&>( args )... };  // throw

            DEBUG_ONLY( _isCreated = true;)
            return *this;
        }

        void  Destroy ()                                    __NE___
        {
            ASSERT( _isCreated );
            _value.~T();

            DEBUG_ONLY(
                _isCreated = false;
                DbgFreeMem( _value );
            )
        }


        template <typename Fn>
        Self&  CustomCtor (const Fn &fn)                    noexcept(IsNothrowInvocable<Fn>)
        {
            ASSERT( not _isCreated );
            fn( OUT _value );   // throw

            DEBUG_ONLY( _isCreated = true;)
            return *this;
        }

        template <typename Fn>
        void  CustomDtor (const Fn &fn)                     noexcept(IsNothrowInvocable<Fn>)
        {
            ASSERT( _isCreated );
            fn( OUT _value );

            DEBUG_ONLY(
                _isCreated = false;
                DbgFreeMem( _value );
            )
        }


        ND_ T *         operator -> ()                      __NE___ { ASSERT( _isCreated );  return &_value; }
        ND_ T const*    operator -> ()                      C_NE___ { ASSERT( _isCreated );  return &_value; }

        ND_ T &         operator * ()                       __NE___ { ASSERT( _isCreated );  return _value; }
        ND_ T const&    operator * ()                       C_NE___ { ASSERT( _isCreated );  return _value; }

        ND_ T *         operator & ()                       __NE___ { ASSERT( _isCreated );  return &_value; }
        ND_ T const*    operator & ()                       C_NE___ { ASSERT( _isCreated );  return &_value; }

        ND_ T &         AsRef ()                            __NE___ { ASSERT( _isCreated );  return _value; }
        ND_ T const&    AsRef ()                            C_NE___ { ASSERT( _isCreated );  return _value; }

        ND_ T &&        AsRVRef ()                          rvNE___ { ASSERT( _isCreated );  return RVRef(_value); }


        DEBUG_ONLY(
            ND_ bool    IsCreated ()                        C_NE___ { return _isCreated; }
        )
    };


} // AE::Base
