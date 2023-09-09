// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/TypeList.h"

namespace AE::Base
{
    namespace _hidden_
    {
        template <typename... Types>    struct overloaded final : Types... { using Types::operator()...; };

        template <typename... Types>    overloaded (Types...) -> overloaded<Types...>;
    }

    template <typename ...Types>    using Union         = std::variant< Types... >;
                                    using NullUnion     = std::monostate;


    // Union -> TypeList
    template <typename... Types>
    struct TypeList< std::variant<Types...> > final : TypeList< Types... >
    {};

/*
=================================================
    Visit
=================================================
*/
    template <typename ...Types, typename ...Funcs>
    ND_ constexpr decltype(auto)  Visit (Union<Types...> &un, Funcs&&... fn) noexcept(AllNothrowInvocable< Funcs... >)
    {
        using namespace Base::_hidden_;
        return std::visit( overloaded{ FwdArg<Funcs &&>(fn)... }, un );
    }

    template <typename ...Types, typename ...Funcs>
    ND_ constexpr decltype(auto)  Visit (const Union<Types...> &un, Funcs&&... fn) noexcept(AllNothrowInvocable< Funcs... >)
    {
        using namespace Base::_hidden_;
        return std::visit( overloaded{ FwdArg<Funcs &&>(fn)... }, un );
    }

/*
=================================================
    HoldsAlternative
=================================================
*/
    template <typename T, typename ...Types>
    ND_ constexpr bool  HoldsAlternative (const Union<Types...> &un) __NE___
    {
        return std::holds_alternative<T>( un );
    }

    template <typename ...Types>
    ND_ constexpr bool  IsNullUnion (const Union<Types...> &un) __NE___
    {
        return std::holds_alternative< NullUnion >( un );
    }

/*
=================================================
    UnionGet
=================================================
*/
    template <typename T, typename ...Types>
    ND_ constexpr T*  UnionGet (Union<Types...> &un) __NE___
    {
        return std::get_if<T>( &un );
    }

    template <typename T, typename ...Types>
    ND_ constexpr T const*  UnionGet (const Union<Types...> &un) __NE___
    {
        return std::get_if<T>( &un );
    }

} // AE::Base
