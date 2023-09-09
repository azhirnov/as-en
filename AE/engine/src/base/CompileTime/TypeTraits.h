// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Base
{
namespace _hidden_
{
    template <typename T> struct _IsScalar      { static constexpr bool  value = std::is_scalar_v<T>; };
    template <typename T> struct _IsFloatPoint  { static constexpr bool  value = std::is_floating_point_v<T>; };
    template <typename T> struct _IsSigned      { static constexpr bool  value = std::is_signed_v<T>; };

} // _hidden_


    template <typename T>
    static constexpr bool   IsFloatPoint            = std::is_floating_point_v<T>;

    template <typename T>
    static constexpr bool   IsAnyFloatPoint         = Base::_hidden_::_IsFloatPoint<T>::value;  // software or hardware

    template <typename T>
    static constexpr bool   IsSignedFloatPoint      = IsAnyFloatPoint<T> and Base::_hidden_::_IsSigned<T>::value;

    template <typename T>
    static constexpr bool   IsUnsignedFloatPoint    = IsAnyFloatPoint<T> and not Base::_hidden_::_IsSigned<T>::value;

    template <typename T>
    static constexpr bool   IsInteger               = std::is_integral_v<T>;

    template <typename T>
    static constexpr bool   IsSignedInteger         = std::is_integral_v<T> and std::is_signed_v<T>;

    template <typename T>
    static constexpr bool   IsUnsignedInteger       = std::is_integral_v<T> and std::is_unsigned_v<T>;

    template <typename T>
    static constexpr bool   IsSigned                = std::is_signed_v<T>;

    template <typename T>
    static constexpr bool   IsUnsigned              = std::is_unsigned_v<T>;

    template <typename T>
    static constexpr bool   IsScalar                = Base::_hidden_::_IsScalar<T>::value;

    template <typename T>
    static constexpr bool   IsEnum                  = std::is_enum_v<T>;

    template <typename T>
    static constexpr bool   IsScalarOrEnum          = IsScalar<T> or IsEnum<T>;

    template <typename T>
    static constexpr bool   IsTrivial               = std::is_trivially_destructible_v<T>       and
                                                      std::is_trivially_move_assignable_v<T>    and
                                                      std::is_trivially_move_constructible_v<T>;

    template <typename T>
    static constexpr bool   IsPOD                   = std::is_trivially_destructible_v<T>           and
                                                      std::is_trivially_move_assignable_v<T>        and
                                                      std::is_trivially_move_constructible_v<T>     and
                                                      std::is_trivially_constructible_v<T>          and
                                                      std::is_trivially_copyable_v<T>               and
                                                      std::is_trivially_copy_assignable_v<T>        and
                                                      std::is_trivially_default_constructible_v<T>;

    template <typename T>
    static constexpr bool   IsPointer               = std::is_pointer_v<T>;

    template <typename T>
    static constexpr bool   IsNullPtr               = std::is_null_pointer_v<T>;

    template <typename T>
    static constexpr bool   IsLValueRef             = std::is_lvalue_reference_v<T>;

    template <typename T>
    static constexpr bool   IsRValueRef             = std::is_rvalue_reference_v<T>;    // &&

    template <typename T>
    static constexpr bool   IsReference             = std::is_reference_v<T>;

    template <typename T>
    static constexpr bool   IsClass                 = std::is_class_v<T>;

    template <typename T>
    static constexpr bool   IsUnion                 = std::is_union_v<T>;

    template <typename T>
    static constexpr bool   IsConst                 = std::is_const_v<T>;

    template <typename T>
    static constexpr bool   IsConstRef              = std::is_const_v< std::remove_reference_t<T> >;

    template <typename T>
    static constexpr bool   IsConstPtr              = std::is_const_v< std::remove_pointer_t<T> >;

    template <typename T>
    static constexpr bool   IsAnyConst              = IsConst<T> or IsConstRef<T> or IsConstPtr<T>;

    template <typename T>
    static constexpr bool   IsVolatile              = std::is_volatile_v<T>;

    template <typename T1, typename T2>
    static constexpr bool   IsSameTypes             = std::is_same_v<T1, T2>;

    template <typename T>
    static constexpr bool   IsVoid                  = std::is_void_v<T>;

    template <typename Base, typename Derived>
    static constexpr bool   IsBaseOf                = std::is_base_of_v< Base, Derived >;

    template <typename T>
    static constexpr bool   IsEmpty                 = std::is_empty_v<T>;

    template <typename From, typename To>
    static constexpr bool   IsConvertible           = std::is_convertible_v<From, To>;  // TODO: is_nothrow_convertible (C++20)

    template <typename T>
    static constexpr bool   IsArithmetic            = std::is_arithmetic_v<T>;

    template <typename T>
    static constexpr bool   IsAbstract              = std::is_abstract_v<T>;

    template <typename T>
    static constexpr bool   IsDefaultConstructible  = std::is_default_constructible_v<T>;

    template <typename T>
    static constexpr bool   IsCopyConstructible     = std::is_copy_constructible_v<T>;

    template <typename T>
    static constexpr bool   IsCopyAssignable        = std::is_copy_assignable_v<T>;

    template <typename T>
    static constexpr bool   IsMoveAssignable        = std::is_move_assignable_v<T>;


    template <typename T>
    static constexpr bool   IsNothrowable           = std::is_nothrow_move_constructible_v<T>       and
                                                      std::is_nothrow_default_constructible_v<T>    and
                                                      std::is_nothrow_copy_assignable_v<T>          and
                                                      std::is_nothrow_copy_constructible_v<T>       and
                                                      std::is_nothrow_destructible_v<T>             and
                                                      std::is_nothrow_move_assignable_v<T>;
    template <typename T>
    static constexpr bool   IsNothrowCopyCtor       = std::is_nothrow_copy_constructible_v<T>       or
                                                      std::is_trivially_copy_constructible_v<T>;
    template <typename T>
    static constexpr bool   IsNothrowDtor           = std::is_nothrow_destructible_v<T>             or
                                                      std::is_trivially_destructible_v<T>;
    template <typename T>
    static constexpr bool   IsNothrowMoveCtor       = std::is_nothrow_move_constructible_v<T>       or
                                                      std::is_trivially_move_constructible_v<T>;
    template <typename T>
    static constexpr bool   IsNothrowDefaultCtor    = std::is_nothrow_default_constructible_v<T>    or
                                                      std::is_trivially_default_constructible_v<T>;
    template <typename T>
    static constexpr bool   IsNothrowCopyAssignable = std::is_nothrow_copy_assignable_v<T>          or
                                                      (IsNothrowCopyCtor<T> and IsNothrowDtor<T>);      // if used dtor + ctor
    template <typename T>
    static constexpr bool   IsNothrowMoveAssignable = std::is_nothrow_move_assignable_v<T>          or
                                                      (IsNothrowMoveCtor<T> and IsNothrowDtor<T>);      // if used dtor + move ctor

    template <typename T, typename ...Args>
    static constexpr bool   IsNothrowCtor           = std::is_nothrow_constructible_v< T, Args... >     or
                                                      std::is_trivially_constructible_v< T, Args... >   or
                                                      (std::is_constructible_v< T, Args... > and std::is_trivial_v<T>);


    template <typename T>                   struct IsNothrowDtor_t              { static constexpr bool  value = IsNothrowDtor<T>; };
    template <typename T>                   struct IsNothrowCopyCtor_t          { static constexpr bool  value = IsNothrowCopyCtor<T>; };
    template <typename T>                   struct IsNothrowMoveCtor_t          { static constexpr bool  value = IsNothrowMoveCtor<T>; };
    template <typename T>                   struct IsNothrowDefaultCtor_t       { static constexpr bool  value = IsNothrowDefaultCtor<T>; };
    template <typename T>                   struct IsNothrowMoveAssignable_t    { static constexpr bool  value = IsNothrowMoveAssignable<T>; };
    template <typename T>                   struct IsNothrowCopyAssignable_t    { static constexpr bool  value = IsNothrowCopyAssignable<T>; };
    template <typename T, typename ...Args> struct IsNothrowCtor_t              { static constexpr bool  value = IsNothrowCtor< T, Args... >; };


    template <typename Fn>
    static constexpr bool   IsNothrowInvocable      = std::is_nothrow_invocable_v<Fn>;

    template <typename Fn> struct IsNothrowInvocable_t { static constexpr bool  value = IsNothrowInvocable<Fn>; };



    template <typename T>
    using RemovePointer     = std::remove_pointer_t<T>;

    template <typename T>
    using RemoveReference   = std::remove_reference_t<T>;

    template <typename T>
    using RemoveCVRef       = std::remove_cv_t< std::remove_reference_t<T> >;

    template <typename T>
    using RemoveCV          = std::remove_cv_t<T>;  // remove 'const', 'volatile', 'const volatile'

    template <typename T>
    using AddConst          = std::add_const_t<T>;

    template <typename T>
    using RemoveConst       = std::remove_const_t<T>;

    template <typename T>
    using RemoveArray       = std::remove_extent_t<T>;


    template <typename T>   using InPlaceType   = std::in_place_type_t<T>;
    template <typename T>   constexpr InPlaceType<T> InPlaceObj {};


    template <bool Test, typename Type = void>
    using EnableIf      = std::enable_if_t< Test, Type >;

    template <bool Test, typename Type = void>
    using DisableIf     = std::enable_if_t< !Test, Type >;


    template <bool Test, typename IfTrue, typename IfFalse>
    using Conditional   = std::conditional_t< Test, IfTrue, IfFalse >;


    template <typename T>   ND_ constexpr usize     CT_SizeofInBits (const T&)  __NE___ { return sizeof(T) << 3; }
    template <typename T>   static constexpr usize  CT_SizeOfInBits             =       sizeof(T) << 3;


    template <usize Bits>
    using BitSizeToUInt     = Conditional< Bits <= CT_SizeOfInBits<ubyte>, ubyte,
                                Conditional< Bits <= CT_SizeOfInBits<ushort>, ushort,
                                    Conditional< Bits <= CT_SizeOfInBits<uint>, uint,
                                        Conditional< Bits <= CT_SizeOfInBits<ulong>, ulong,
                                            void >>>>;

    template <usize Bits>
    using BitSizeToInt      = Conditional< Bits <= CT_SizeOfInBits<sbyte>, sbyte,
                                Conditional< Bits <= CT_SizeOfInBits<sshort>, sshort,
                                    Conditional< Bits <= CT_SizeOfInBits<sint>, sint,
                                        Conditional< Bits <= CT_SizeOfInBits<slong>, slong,
                                            void >>>>;


    template <usize ByteCount>
    using ByteSizeToUInt    = BitSizeToUInt< ByteCount * 8 >;

    template <usize ByteCount>
    using ByteSizeToInt     = BitSizeToInt< ByteCount * 8 >;


    template <typename T>
    using ToUnsignedInteger = BitSizeToUInt< CT_SizeOfInBits<T> >;

    template <typename T>
    using ToSignedInteger   = BitSizeToInt< CT_SizeOfInBits<T> >;


    namespace _hidden_
    {
        template <typename T, template <typename...> class Templ>
        struct _IsSpecializationOf : std::bool_constant<false> {};

        template <template <typename...> class Templ, typename... Args>
        struct _IsSpecializationOf< Templ<Args...>, Templ > : std::bool_constant<true> {};

        template <template <typename ...> class Left, template <typename ...> class Right>
        struct _IsSameTemplates         { static const bool  value = false; };

        template <template <typename ...> class T>
        struct _IsSameTemplates< T, T > { static const bool  value = true; };

    } // _hidden_


    template <typename T, template <typename...> class Templ>
    static constexpr bool   IsSpecializationOf  = Base::_hidden_::_IsSpecializationOf< T, Templ >::value;

    template <template <typename ...> class Left, template <typename ...> class Right>
    static constexpr bool   IsSameTemplates     = Base::_hidden_::_IsSameTemplates< Left, Right >::value;


    namespace _hidden_
    {
        template <typename T>
        struct RemoveAllQual
        {
            using _NextType = RemoveReference< RemovePointer< RemoveArray< RemoveCV< T >>>>;
            using type      = typename Conditional< (IsSameTypes< _NextType, T >),
                                TypeToType<T>,
                                DeferredTemplate2< RemoveAllQual, _NextType > >::type;
        };
    }
    template <typename T>
    using RemoveAllQualifiers   = typename Base::_hidden_::RemoveAllQual<T>::type;


    template <typename Base, typename Derived>
    static constexpr bool   IsBaseOfNoQual      = std::is_base_of_v< Base, RemoveAllQualifiers<Derived> >;

    template <typename T1, typename T2>
    static constexpr bool   IsSameTypesNoQual   = std::is_same_v< RemoveAllQualifiers<T1>, RemoveAllQualifiers<T2> >;


    namespace _hidden_
    {
        template <typename T>
        struct _IsStaticArray {
            static constexpr bool   value = std::is_array_v<T>;
        };

        template <typename T, usize S>
        struct _IsStaticArray< std::array<T,S> > {
            static constexpr bool   value = true;
        };
    }
    template <typename T>
    static constexpr bool   IsStaticArray = Base::_hidden_::_IsStaticArray<T>::value;

    namespace _hidden_
    {
        template <typename T>
        struct _IsStdVector {
            static constexpr bool   value = false;
        };

        template <typename T, typename A>
        struct _IsStdVector< std::vector<T,A> > {
            static constexpr bool   value = true;
        };
    }
    template <typename T>
    static constexpr bool   IsArray = Base::_hidden_::_IsStdVector<T>::value;


    template <typename T>
    static constexpr bool   IsDuration = IsSpecializationOf< T, std::chrono::duration >;


    template <typename Fn, typename ...Args>
    using ResultOf  = std::invoke_result_t< Fn, Args... >;


/*
=================================================
    IndexSequence
=================================================
*/
    template <usize ...Vals>
    using IndexSequence = std::index_sequence< Vals... >;

    template <usize Size>
    using MakeIndexSequence = std::make_index_sequence< Size >;

    namespace _hidden_
    {
        template <usize Size, usize Base, usize ...Vals>
        struct _MakeIndexRange
        {
            using type = typename _MakeIndexRange< Size - 1, Base+1, Base, Vals... >::type;
        };

        template <usize Base, usize ...Vals>
        struct _MakeIndexRange< 0, Base, Vals... >
        {
            using type = IndexSequence< Vals... >;
        };
    }

    template <usize Base, usize Size>
    using MakeIndexRange = typename Base::_hidden_::_MakeIndexRange< Size, Base >::type;

/*
=================================================
    Min/Max value
=================================================
*/
    template <typename T>
    ND_ constexpr auto  MaxValue ()
    {
        return std::numeric_limits< RemoveAllQualifiers<T> >::max();
    }

    template <typename T>
    ND_ constexpr auto  MinValue ()
    {
        return std::numeric_limits< RemoveAllQualifiers<T> >::min();
    }

/*
=================================================
    Infinity
=================================================
*/
    template <typename T>
    ND_ constexpr auto  Infinity ()
    {
        STATIC_ASSERT( IsAnyFloatPoint<T> );
        return std::numeric_limits< RemoveAllQualifiers<T> >::infinity();
    }

/*
=================================================
    NaN
=================================================
*/
    template <typename T>
    ND_ constexpr auto  NaN ()
    {
        STATIC_ASSERT( IsAnyFloatPoint<T> );
        return std::numeric_limits< RemoveAllQualifiers<T> >::quiet_NaN();
    }

/*
=================================================
    IsMemCopyAvailable
----
    allow to use MemCopy and MemMove
=================================================
*/
    template <typename T>
    struct TMemCopyAvailable {
        static constexpr bool   value = IsTrivial<T>;
    };

    template <typename T, usize I>
    struct TMemCopyAvailable< T[I] > {
        static constexpr bool   value = TMemCopyAvailable<T>::value;
    };

    template <typename T, usize I>
    struct TMemCopyAvailable< const T[I] > {
        static constexpr bool   value = TMemCopyAvailable<T>::value;
    };

    template <typename T>
    static constexpr bool   IsMemCopyAvailable = TMemCopyAvailable<T>::value;


/*
=================================================
    IsZeroMemAvailable
----
    allow to use ZeroMem
=================================================
*/
    template <typename T>
    struct TZeroMemAvailable {
        static constexpr bool   value = IsTrivial<T>;
    };

    template <typename T, usize I>
    struct TZeroMemAvailable< T[I] > {
        static constexpr bool   value = TZeroMemAvailable<T>::value;
    };

    template <typename T, usize I>
    struct TZeroMemAvailable< const T[I] > {
        static constexpr bool   value = TZeroMemAvailable<T>::value;
    };

    template <typename T>
    static constexpr bool   IsZeroMemAvailable = TZeroMemAvailable<T>::value;

/*
=================================================
    IsTriviallySerializable
----
    Allow to use MemCopy to/from file when serializing / deserializing.
    Objects must not have pointers and non-trivial fields.
=================================================
*/
    template <typename T>
    struct TTriviallySerializable
    {
        using A = RemoveCV<T>;
        static constexpr bool   value = IsTrivial<A>        and
                                        not IsPointer<A>    and
                                        not IsReference<A>;
    };

    template <typename T, usize I>
    struct TTriviallySerializable< T[I] > {
        static constexpr bool   value = TTriviallySerializable< RemoveCV<T> >::value;
    };

    template <typename T, usize I>
    struct TTriviallySerializable< const T[I] > {
        static constexpr bool   value = TTriviallySerializable< RemoveCV<T> >::value;
    };


    template <typename T>
    static constexpr bool   IsTriviallySerializable = TTriviallySerializable< RemoveCV<T> >::value;


} // AE::Base
