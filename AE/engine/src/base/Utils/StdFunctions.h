// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Base
{

/*
=================================================
    Unused
=================================================
*/
    template <typename... Args>
    constexpr void  Unused (Args&& ...) __NE___ {}

/*
=================================================
    ArgRef (same as std::ref)
=================================================
*/
    template <typename T>
    ND_ constexpr std::reference_wrapper<T>  ArgRef (T &arg) __NE___
    {
        return std::reference_wrapper<T>{ arg };
    }

/*
=================================================
    RVRef (same as std::move)
=================================================
*/
    template <typename T>
    ND_ AE_INTRINSIC constexpr RemoveReference<T>&&  RVRef (T &arg) __NE___
    {
        return static_cast< RemoveReference<T>&& >( arg );
    }

/*
=================================================
    FwdArg (same as std::forward)
=================================================
*/
    template <typename T>
    ND_ AE_INTRINSIC constexpr T&&  FwdArg (RemoveReference<T> &arg) __NE___
    {
        return static_cast< T&& >( arg );
    }

    template <typename T>
    ND_ AE_INTRINSIC constexpr T&&  FwdArg (RemoveReference<T> &&arg) __NE___
    {
        StaticAssert( not IsLValueRef<T> );
        return static_cast< T&& >( arg );
    }


} // AE::Base
