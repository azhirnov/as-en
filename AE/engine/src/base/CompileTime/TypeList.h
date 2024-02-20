// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/TypeListImpl.h"

namespace AE::Base
{

    //
    // Type List
    //

    template <typename... Types>
    struct TypeList
    {
    public:
        struct AsTuple { using          type                = Tuple< Types... >; };

        template <typename T>
        inline static constexpr usize   FirstIndex          = Base::_hidden_::TL_GetFirstIndex< T, 0, Types... >::value;

        template <typename T>
        inline static constexpr usize   LastIndex           = Base::_hidden_::TL_GetLastIndex< T, 0, Types... >::value;

        template <template <typename...> class Templ>
        inline static constexpr usize   FirstSpecializationOf = Base::_hidden_::TL_GetFirstSpecializationOf< Templ, 0, Types... >::value;

        template <typename T>
        inline static constexpr usize   Index               = FirstIndex<T>;

        inline static constexpr usize   Count               = sizeof...(Types);

        template <typename T>
        inline static constexpr bool    HasType             = (Index<T> != UMax);

        template <typename T>
        inline static constexpr bool    HasSingle           = HasType<T> and (FirstIndex<T> == LastIndex<T>);

        template <usize I>      using   GetT                = Base::_hidden_::TL_GetTypeByIndex< I, Types... >;                 // usage: GetT<0>::type
        template <usize I>      using   Get                 = typename Base::_hidden_::TL_GetTypeByIndex< I, Types... >::type;  // usage: Get<0>

        struct Front { using            type                = Get<0>; };
        struct Back  { using            type                = Get<Count-1>; };

        struct Self  { using            type                = TypeList< Types... >; };

        struct PopFront { using         type                = typename Base::_hidden_::TL_PopFront< TypeList<>, Types... >::type; };
        struct PopBack  { using         type                = typename Base::_hidden_::TL_PopBack< TypeList<>, Types... >::type; };

        template <typename T>   using   PushBack            = TypeList< Types..., T >;
        template <typename T>   using   PushFront           = TypeList< T, Types... >;

        template <template <typename> class Tmpl>
        using                           Apply               = TypeList< Tmpl< Types >... >;

        template <template <typename> class Tmpl>
        using                           Apply_t             = TypeList< typename Tmpl< Types >::type ... >;

        template <template <typename ...> class Tmpl>
        using                           Transform           = Tmpl< Types... >;

        template <template <typename> class Tmpl>
        static constexpr auto           ForEach_Or ()       __NE___ { return (... or Tmpl<Types>::value); }

        template <template <typename> class Tmpl>
        static constexpr auto           ForEach_And ()      __NE___ { return (... and Tmpl<Types>::value); }

        template <template <typename> class Tmpl>
        static constexpr auto           ForEach_Add ()      __NE___ { return (... + Tmpl<Types>::value); }

        template <template <typename> class Tmpl>
        static constexpr auto           ForEach_Max ()      __NE___ { return Math::Max( Tmpl<Types>::value... ); }

        template <template <typename> class Tmpl>
        static constexpr auto           ForEach_Min ()      __NE___ { return Math::Min( Tmpl<Types>::value... ); }


        template <typename FN>
        static constexpr void           Visit (FN&& fn)     __NE___ { return _RecursiveVisit<0>( FwdArg<FN>(fn) ); }

        template <typename FN>
        static constexpr void           VisitTh (FN&& fn)   __Th___ { return _RecursiveVisit2<0>( FwdArg<FN>(fn) ); }


    private:
        template <usize I, typename FN>
        static constexpr void  _RecursiveVisit (FN&& fn)    __NE___
        {
            if constexpr( I < Count )
            {
                using T = Get<I>;
                CheckNothrow( IsNoExcept( fn.template operator()<T,I>() ));

                fn.template operator()<T,I>();
                _RecursiveVisit< I+1 >( FwdArg<FN>(fn) );
            }
            Unused( fn );
        }

        template <usize I, typename FN>
        static constexpr void  _RecursiveVisit2 (FN&& fn)   __Th___
        {
            if constexpr( I < Count )
            {
                using T = Get<I>;
                CheckNothrow( not IsNoExcept( fn.template operator()<T,I>() )); // use 'Visit'

                fn.template operator()<T,I>();
                _RecursiveVisit2< I+1 >( FwdArg<FN>(fn) );
            }
            Unused( fn );
        }
    };


    template <typename... Types>
    struct TypeList< std::tuple<Types...> > final : TypeList< Types... >
    {};

    template <typename... Types>
    struct TypeList< Tuple<Types...> > final : TypeList< Types... >
    {};


namespace _hidden_
{
    template <typename T>
    struct _IsTypeList {
        static constexpr bool   value = false;
    };

    template <typename... Types>
    struct _IsTypeList< TypeList<Types...>> {
        static constexpr bool   value = true;
    };


    template <typename Left, typename Right>
    struct _Merge;

    template <typename ...LeftTypes, typename ...RightTypes>
    struct _Merge< TypeList<LeftTypes...>, TypeList<RightTypes...> >
    {
        using type = TypeList< LeftTypes..., RightTypes... >;
    };

    template <typename A>
    struct AreSameTypes
    {
        template <typename B>
        struct Impl {
            static constexpr bool   value = IsSameTypes< A, B >;
        };
    };

} // _hidden_

    template <typename T>
    static constexpr bool   IsTypeList = Base::_hidden_::_IsTypeList< T >::value;


    // Type traits //

    template <typename T, typename ...Types>
    static constexpr bool   AllAreSameTypes             = TypeList< Types... >::template ForEach_And< Base::_hidden_::AreSameTypes<T>::Impl >();

    template <typename ...Types>
    static constexpr bool   AllNothrowCopyCtor          = TypeList< Types... >::template ForEach_And< TNothrowCopyCtor >();

    template <typename ...Types>
    static constexpr bool   AllNothrowMoveCtor          = TypeList< Types... >::template ForEach_And< TNothrowMoveCtor >();

    template <typename ...Types>
    static constexpr bool   AllNothrowDefaultCtor       = TypeList< Types... >::template ForEach_And< TNothrowDefaultCtor >();

    template <typename ...Types>
    static constexpr bool   AllNothrowCopyAssignable    = TypeList< Types... >::template ForEach_And< TNothrowCopyAssignable >();

    template <typename ...Types>
    static constexpr bool   AllNothrowMoveAssignable    = TypeList< Types... >::template ForEach_And< TNothrowMoveAssignable >();

    template <typename ...Types>
    static constexpr bool   AllNothrowInvocable         = TypeList< Types... >::template ForEach_And< TNothrowInvocable >();

} // AE::Base

namespace AE::Base::TypeListUtils
{

    template <typename T>
    struct GetTypeSize {
        static constexpr usize  value = sizeof(T);
    };

    template <typename T>
    struct GetTypeAlign {
        static constexpr usize  value = alignof(T);
    };


    template <typename Left, typename Right>
    using Merge = typename Base::_hidden_::_Merge< Left, Right >::type;

} // AE::Base::TypeListUtils
