// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Utils/TypeId.h"

namespace AE::Base
{
    class AnyTypeRef;
    class AnyTypeCRef;


    //
    // Reference to Any type
    //

    class AnyTypeRef
    {
    // types
    private:
        template <typename T>
        inline static constexpr bool    IsNtCopy = IsNothrowCopyCtor<T> and IsNothrowDefaultCtor<T>;


    // variables
    private:
        TypeId      _typeId;
        void *      _ref        = null;


    // methods
    public:
        AnyTypeRef ()                                       __NE___ {}
        AnyTypeRef (AnyTypeRef &&)                          __NE___ = default;
        AnyTypeRef (const AnyTypeRef &)                     __NE___ = default;

        AnyTypeRef&  operator = (const AnyTypeRef &rhs)     __NE___ = default;

        template <typename T,
                  DISABLEIF( IsConst<T> or IsSameTypes< T, AnyTypeRef > or IsSameTypes< T, AnyTypeCRef >)
                 >
        AnyTypeRef (T &value)                               __NE___ : _typeId{ TypeIdOf<T>() }, _ref{ std::addressof(value) } {}


        template <typename T>   ND_ bool    Is ()           C_NE___                 { return _typeId == TypeIdOf<T>(); }
        template <typename T>   ND_ bool    Is (const T &)  C_NE___                 { return _typeId == TypeIdOf<T>(); }

        template <typename T>   ND_ T *     GetIf ()        C_NE___                 { return Is<T>() ? static_cast<T*>( _ref ) : null; }
        template <typename T>   ND_ T       GetCopyIf ()    CNoExcept(IsNtCopy<T>)  { return Is<T>() ? *static_cast<T*>( _ref ) : T{}; }
        template <typename T>   ND_ T &     As ()           C_NE___                 { ASSERT( Is<T>() );  return *static_cast<T*>( _ref ); }

        ND_ TypeId          GetType ()                      C_NE___                 { return _typeId; }
        ND_ StringView      GetTypeName ()                  C_NE___                 { return _typeId.Name(); }
    };



    //
    // Const Reference to Any type
    //

    class AnyTypeCRef
    {
    // types
    private:
        template <typename T>
        inline static constexpr bool    IsNtCopy = IsNothrowCopyCtor<T> and IsNothrowDefaultCtor<T>;


    // variables
    private:
        TypeId          _typeId;
        void const *    _ref        = null;


    // methods
    public:
        AnyTypeCRef ()                                          __NE___ {}
        AnyTypeCRef (AnyTypeCRef &&)                            __NE___ = default;
        AnyTypeCRef (const AnyTypeCRef &)                       __NE___ = default;

        AnyTypeCRef&  operator = (const AnyTypeCRef &rhs)       __NE___ = default;

        template <typename T,
                  DISABLEIF( IsConst<T> or IsSameTypes< T, AnyTypeRef > or IsSameTypes< T, AnyTypeCRef >)
                 >
        AnyTypeCRef (T &value)                                  __NE___ : _typeId{ TypeIdOf<T>() }, _ref{ std::addressof(value) } {}

        template <typename T,
                  DISABLEIF( IsSameTypes< T, AnyTypeRef > or IsSameTypes< T, AnyTypeCRef >)
                 >
        explicit AnyTypeCRef (const T &value)                   __NE___ : _typeId{ TypeIdOf<T>() }, _ref{ std::addressof(value) } {}


        template <typename T>   ND_ bool        Is ()           C_NE___                 { return _typeId == TypeIdOf<T>(); }
        template <typename T>   ND_ bool        Is (const T &)  C_NE___                 { return _typeId == TypeIdOf<T>(); }

        template <typename T>   ND_ T const *   GetIf ()        C_NE___                 { return Is<T>() ? static_cast<T const*>( _ref ) : null; }
        template <typename T>   ND_ T           GetCopyIf ()    CNoExcept(IsNtCopy<T>)  { return Is<T>() ? *static_cast<T*>( _ref ) : T{}; }
        template <typename T>   ND_ T const &   As ()           C_NE___                 { ASSERT( Is<T>() );  return *static_cast<T const*>( _ref ); }

        ND_ TypeId          GetType ()                          C_NE___                 { return _typeId; }
        ND_ StringView      GetTypeName ()                      C_NE___                 { return _typeId.Name(); }
    };


} // AE::Base
