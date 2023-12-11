// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base::_hidden_
{
    //
    // TL_GetFirstIndex
    //
    template <typename RefType, usize I, typename ...Types>
    struct TL_GetFirstIndex;

    template <typename RefType, usize I>
    struct TL_GetFirstIndex< RefType, I >
    {
        inline static constexpr usize   value = UMax;
    };

    template <typename RefType, usize I, typename Head, typename... Tail>
    struct TL_GetFirstIndex< RefType, I, Head, Tail... >
    {
        inline static constexpr usize   value = Conditional< IsSameTypes<RefType, Head>,
                                                    std::integral_constant<usize, I>,
                                                    TL_GetFirstIndex< RefType, I+1, Tail... >>::value;
    };


    //
    // TL_GetLastIndex
    //
    template <typename RefType, usize I, typename ...Types>
    struct TL_GetLastIndex;

    template <typename RefType, usize I>
    struct TL_GetLastIndex< RefType, I >
    {
        inline static constexpr usize   value = UMax;
    };

    template <typename RefType, usize I, typename Head, typename... Tail>
    struct TL_GetLastIndex< RefType, I, Head, Tail... >
    {
        using result = TL_GetLastIndex< RefType, I+1, Tail... >;

        inline static constexpr usize   value = Conditional< result::value == UMax and IsSameTypes<RefType, Head>,
                                                    std::integral_constant<usize, I>,
                                                    result >::value;
    };


    //
    // TL_PopFront
    //
    template <typename TL, typename T0, typename ...Types>
    struct TL_PopFront {
        using type      = typename TL_PopFront< TL, Types... >::result;
        using result    = typename type::template PushFront< T0 >;
    };

    template <typename TL, typename T0>
    struct TL_PopFront< TL, T0 > {
        using type      = TL;
        using result    = typename TL::template PushFront<T0>;
    };


    //
    // TL_PopBack
    //
    template <typename TL, typename T0, typename ...Types>
    struct TL_PopBack {
        using type = typename TL_PopBack< typename TL::template PushBack<T0>, Types... >::type;
    };

    template <typename TL, typename T0>
    struct TL_PopBack< TL, T0 > {
        using type = TL;
    };


    //
    // TL_GetFirstSpecializationOf
    //
    template <template <typename...> class Templ, usize I, typename ...Types>
    struct TL_GetFirstSpecializationOf;

    template <template <typename...> class Templ, usize I>
    struct TL_GetFirstSpecializationOf< Templ, I >
    {
        inline static constexpr usize   value = UMax;
    };

    template <template <typename...> class Templ, usize I, typename Head, typename... Tail>
    struct TL_GetFirstSpecializationOf< Templ, I, Head, Tail... >
    {
        inline static constexpr usize   value = Conditional< IsSpecializationOf< Head, Templ >,
                                                    std::integral_constant<usize, I>,
                                                    TL_GetFirstSpecializationOf< Templ, I+1, Tail... >>::value;
    };


    //
    // TL_GetTypeByIndex
    //
    template <usize ReqIndex, usize I, typename ...Types>
    struct TL_GetTypeByIndex2;

    template <usize ReqIndex, usize I>
    struct TL_GetTypeByIndex2< ReqIndex, I >
    {
        StaticAssert( "index is out of range" );
    };

    template <usize ReqIndex, usize I, typename Head, typename... Tail>
    struct TL_GetTypeByIndex2< ReqIndex, I, Head, Tail... >
    {
        using  type = typename Conditional< ReqIndex == I,
                            TypeToType< Head >,
                            TL_GetTypeByIndex2< ReqIndex, I+1, Tail... >>::type;
    };

    template <usize ReqIndex, typename ...Types>
    struct TL_GetTypeByIndex {
        using  type = typename TL_GetTypeByIndex2< ReqIndex, 0, Types... >::type;
    };



} // AE::Base::_hidden_
