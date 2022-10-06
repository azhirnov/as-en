// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/TemplateUtils.h"

namespace AE::Base
{

	template <typename T>
	static constexpr bool	IsFloatPoint		= std::is_floating_point_v<T>;

	template <typename T>
	static constexpr bool	IsInteger			= std::is_integral_v<T>;
	
	template <typename T>
	static constexpr bool	IsSignedInteger		= std::is_integral_v<T> and std::is_signed_v<T>;
	
	template <typename T>
	static constexpr bool	IsUnsignedInteger	= std::is_integral_v<T> and std::is_unsigned_v<T>;

	template <typename T>
	static constexpr bool	IsSigned			= std::is_signed_v<T>;

	template <typename T>
	static constexpr bool	IsUnsigned			= std::is_unsigned_v<T>;

	template <typename T>
	static constexpr bool	IsScalar			= std::is_scalar_v<T>;
	
	template <typename T>
	static constexpr bool	IsEnum				= std::is_enum_v<T>;
	
	template <typename T>
	static constexpr bool	IsScalarOrEnum		= std::is_scalar_v<T> or std::is_enum_v<T>;

	template <typename T>
	static constexpr bool	IsTrivial			= std::is_trivially_destructible_v<T>		and
												  std::is_trivially_move_assignable_v<T>	and
												  std::is_trivially_move_constructible_v<T>;
	
	template <typename T>
	static constexpr bool	IsPOD				= std::is_trivially_destructible_v<T>			and
												  std::is_trivially_move_assignable_v<T>		and
												  std::is_trivially_move_constructible_v<T>		and
												  std::is_trivially_constructible_v<T>			and
												  std::is_trivially_copyable_v<T>				and
												  std::is_trivially_copy_assignable_v<T>		and
												  std::is_trivially_default_constructible_v<T>;

	template <typename T>
	static constexpr bool	IsPointer			= std::is_pointer_v<T>;

	template <typename T>
	static constexpr bool	IsNullPtr			= std::is_null_pointer_v<T>;

	template <typename T>
	static constexpr bool	IsLValueRef			= std::is_lvalue_reference_v<T>;
	
	template <typename T>
	static constexpr bool	IsRValueRef			= std::is_rvalue_reference_v<T>;
	
	template <typename T>
	static constexpr bool	IsReference			= std::is_reference_v<T>;

	template <typename T>
	static constexpr bool	IsClass				= std::is_class_v<T>;

	template <typename T>
	static constexpr bool	IsUnion				= std::is_union_v<T>;

	template <typename T>
	static constexpr bool	IsConst				= std::is_const_v<T>;
	
	template <typename T>
	static constexpr bool	IsVolatile			= std::is_volatile_v<T>;

	template <typename T1, typename T2>
	static constexpr bool	IsSameTypes			= std::is_same_v<T1, T2>;

	template <typename T>
	static constexpr bool	IsVoid				= std::is_void_v<T>;

	template <typename Base, typename Derived>
	static constexpr bool	IsBaseOf			= std::is_base_of_v< Base, Derived >;

	template <typename T>
	static constexpr bool	IsEmpty				= std::is_empty_v<T>;

	template <typename From, typename To>
	static constexpr bool	IsConvertible		= std::is_convertible_v<From, To>;	// TODO: is_nothrow_convertible (C++20)

	template <typename T>
	static constexpr bool	IsArithmetic		= std::is_arithmetic_v<T>;

	
	template <typename T>
	using RemovePointer		= std::remove_pointer_t<T>;
	
	template <typename T>
	using RemoveReference	= std::remove_reference_t<T>;

	template <typename T>
	using RemoveCVRef		= std::remove_cv_t< std::remove_reference_t<T> >;

	template <typename T>
	using RemoveCV			= std::remove_cv_t<T>;	// remove 'const', 'volatile', 'const volatile'

	template <typename T>
	using AddConst			= std::add_const_t<T>;

	template <typename T>
	using RemoveConst		= std::remove_const_t<T>;
	
	template <typename T>
	using RemoveArray		= std::remove_extent_t<T>;

	
	template <typename T>	using InPlaceType	= std::in_place_type_t<T>;
	template <typename T>	constexpr InPlaceType<T> InPlaceObj {};


	template <bool Test, typename Type = void>
	using EnableIf		= std::enable_if_t< Test, Type >;

	template <bool Test, typename Type = void>
	using DisableIf		= std::enable_if_t< !Test, Type >;


	template <bool Test, typename IfTrue, typename IfFalse>
	using Conditional	= std::conditional_t< Test, IfTrue, IfFalse >;


	template <usize Bits>
	using BitSizeToUInt		= Conditional< Bits <= sizeof(ubyte)*8, ubyte,
								Conditional< Bits <= sizeof(ushort)*8, ushort,
									Conditional< Bits <= sizeof(uint)*8, uint,
										Conditional< Bits <= sizeof(ulong)*8, ulong,
											void >>>>;

	template <usize Bits>
	using BitSizeToInt		= Conditional< Bits <= sizeof(sbyte)*8, sbyte,
								Conditional< Bits <= sizeof(sshort)*8, sshort,
									Conditional< Bits <= sizeof(sint)*8, sint,
										Conditional< Bits <= sizeof(slong)*8, slong,
											void >>>>;
	

	template <usize ByteCount>
	using ByteSizeToUInt	= BitSizeToUInt< ByteCount * 8 >;
	
	template <usize ByteCount>
	using ByteSizeToInt		= BitSizeToInt< ByteCount * 8 >;


	template <typename T>
	using ToUnsignedInteger	= BitSizeToUInt< sizeof(T)*8 >;
	
	template <typename T>
	using ToSignedInteger	= BitSizeToInt< sizeof(T)*8 >;

	
	namespace _hidden_
	{
		template <typename T, template <typename...> class Templ>
		struct is_specialization_of : std::bool_constant<false> {};

		template <template <typename...> class Templ, typename... Args>
		struct is_specialization_of< Templ<Args...>, Templ > : std::bool_constant<true> {};
		
		template <template <typename ...> class Left, template <typename ...> class Right>
		struct IsSameTemplates			{ static const bool  value = false; };

		template <template <typename ...> class T>
		struct IsSameTemplates< T, T >	{ static const bool  value = true; };

	}	// _hidden_

	
	template <typename T, template <typename...> class Templ>
	static constexpr bool	IsSpecializationOf	= Base::_hidden_::is_specialization_of< T, Templ >::value;
	
	template <template <typename ...> class Left, template <typename ...> class Right>
	static constexpr bool	IsSameTemplates		= Base::_hidden_::IsSameTemplates< Left, Right >::value;

	
	namespace _hidden_
	{
		template <typename T>
		struct RemoveAllQual
		{
			using _NextType	= RemoveReference< RemovePointer< RemoveArray< RemoveCV< T >>>>;
			using type		= typename Conditional< (IsSameTypes< _NextType, T >),
								TypeToType<T>,
								DeferredTemplate2< RemoveAllQual, _NextType > >::type;
		};
	}
	template <typename T>
	using RemoveAllQualifiers	= typename _hidden_::RemoveAllQual<T>::type;

	
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
	using MakeIndexRange = typename _hidden_::_MakeIndexRange< Size, Base >::type;

/*
=================================================
	Min/Max value
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr auto  MaxValue ()
	{
		return std::numeric_limits< RemoveAllQualifiers<T> >::max();
	}

	template <typename T>
	ND_ forceinline constexpr auto  MinValue ()
	{
		return std::numeric_limits< RemoveAllQualifiers<T> >::min();
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
		static constexpr bool	value = IsTrivial<T>;
	};
	
	template <typename T, usize I>
	struct TMemCopyAvailable< T[I] > {
		static constexpr bool	value = TMemCopyAvailable<T>::value;
	};
	
	template <typename T, usize I>
	struct TMemCopyAvailable< const T[I] > {
		static constexpr bool	value = TMemCopyAvailable<T>::value;
	};
	
	template <typename T>
	static constexpr bool	IsMemCopyAvailable = TMemCopyAvailable<T>::value;
	
	
/*
=================================================
	IsZeroMemAvailable
----
	allow to use ZeroMem
=================================================
*/
	template <typename T>
	struct TZeroMemAvailable {
		static constexpr bool	value = IsTrivial<T>;
	};
	
	template <typename T, usize I>
	struct TZeroMemAvailable< T[I] > {
		static constexpr bool	value = TZeroMemAvailable<T>::value;
	};
	
	template <typename T, usize I>
	struct TZeroMemAvailable< const T[I] > {
		static constexpr bool	value = TZeroMemAvailable<T>::value;
	};
	
	template <typename T>
	static constexpr bool	IsZeroMemAvailable = TZeroMemAvailable<T>::value;
	
/*
=================================================
	IsTrivialySerializable
----
	Allow to use MemCopy to/from file when serializing / deserializing.
	Objects must not have pointers and non-trivial fields.
=================================================
*/
	template <typename T>
	struct TTrivialySerializable
	{
		using A = RemoveCV<T>;
		static constexpr bool	value =	IsTrivial<A>		and
										not IsPointer<A>	and
										not IsReference<A>;
	};
	
	template <typename T, usize I>
	struct TTrivialySerializable< T[I] > {
		static constexpr bool	value = TTrivialySerializable< RemoveCV<T> >::value;
	};
	
	template <typename T, usize I>
	struct TTrivialySerializable< const T[I] > {
		static constexpr bool	value = TTrivialySerializable< RemoveCV<T> >::value;
	};
	

	template <typename T>
	static constexpr bool	IsTrivialySerializable = TTrivialySerializable< RemoveCV<T> >::value;


}	// AE::Base
