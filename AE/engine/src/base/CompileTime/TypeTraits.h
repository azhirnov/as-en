// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Base
{
	template <bool Value>
	using CT_Bool	= std::bool_constant<Value>;
	using CT_True	= std::bool_constant<true>;
	using CT_False	= std::bool_constant<false>;

	template <typename T> struct TIsScalar		: CT_Bool< std::is_scalar_v<T>			>{};
	template <typename T> struct TIsFloatPoint	: CT_Bool< std::is_floating_point_v<T>	>{};
	template <typename T> struct TIsSigned		: CT_Bool< std::is_signed_v<T>			>{};
	template <typename T> struct TIsUnsigned	: CT_Bool< std::is_unsigned_v<T>		>{};
	template <typename T> struct TIsInteger		: CT_Bool< std::is_integral_v<T> or std::is_enum_v<T> >{};
	template <typename T> struct TUnwrap		{ using type = T; };


	template <typename T>
	static constexpr bool	IsSigned				= TIsSigned<T>::value;

	template <typename T>
	static constexpr bool	IsUnsigned				= TIsUnsigned<T>::value;

	template <typename T>
	static constexpr bool	IsFloatPoint			= std::is_floating_point_v<T>;

	template <typename T>
	static constexpr bool	IsAnyFloatPoint			= TIsFloatPoint<T>::value;					// software or hardware

	template <typename T>
	static constexpr bool	IsSwFloatPoint			= IsFloatPoint<T> != IsAnyFloatPoint<T>;	// only software

	template <typename T>
	static constexpr bool	IsSignedFloatPoint		= IsAnyFloatPoint<T> and IsSigned<T>;

	template <typename T>
	static constexpr bool	IsUnsignedFloatPoint	= IsAnyFloatPoint<T> and IsUnsigned<T>;

	template <typename T>
	static constexpr bool	IsInteger				= std::is_integral_v<T>;

	template <typename T>
	static constexpr bool	IsAnyInteger			= TIsInteger<T>::value;						// software or hardware or enum

	template <typename T>
	static constexpr bool	IsSwInteger				= IsInteger<T> != IsAnyInteger<T>;			// only software

	template <typename T>
	static constexpr bool	IsSignedInteger			= IsInteger<T> and IsSigned<T>;

	template <typename T>
	static constexpr bool	IsUnsignedInteger		= IsInteger<T> and IsUnsigned<T>;

	template <typename T>
	static constexpr bool	IsAnySignedInteger		= IsAnyInteger<T> and IsSigned<T>;

	template <typename T>
	static constexpr bool	IsAnyUnsignedInteger	= IsAnyInteger<T> and IsUnsigned<T>;

	template <typename T>
	static constexpr bool	IsScalar				= std::is_scalar_v<T>;

	template <typename T>
	static constexpr bool	IsAnyScalar				= TIsScalar<T>::value;

	template <typename T>
	static constexpr bool	IsEnum					= std::is_enum_v<T>;

	template <typename T>
	static constexpr bool	IsTrivial				= std::is_trivially_destructible_v<T>			and
													  std::is_trivially_move_assignable_v<T>		and
													  std::is_trivially_move_constructible_v<T>;

	template <typename T>
	concept					IsPointer				=     std::is_pointer_v<T>						and
													  not std::is_function_v< std::remove_pointer_t<T> >;

	template <typename T>
	static constexpr bool	IsAnyPointer			= std::is_pointer_v<T>		or
													  std::is_member_pointer_v<T>;

	template <typename T>
	static constexpr bool	IsFunctionPointer		= std::is_pointer_v<T>							and		// global or static function
													  std::is_function_v< std::remove_pointer_t<T> >;

	template <typename T>
	static constexpr bool	IsMemberPointer			= std::is_member_pointer_v<T>;			// object or method

	template <typename T>
	static constexpr bool	IsMemberObjectPointer	= std::is_member_object_pointer_v<T>;	// only class field

	template <typename T>
	static constexpr bool	IsMemberFunctionPointer	= std::is_member_function_pointer_v<T>;	// only class method

	template <typename T>
	static constexpr bool	IsNullPtr				= std::is_null_pointer_v<T>;

	template <typename T>
	static constexpr bool	IsLValueRef				= std::is_lvalue_reference_v<T>;	// &

	template <typename T>
	static constexpr bool	IsRValueRef				= std::is_rvalue_reference_v<T>;	// &&

	template <typename T>
	static constexpr bool	IsReference				= std::is_reference_v<T>;			// & or &&

	template <typename T>
	static constexpr bool	IsClass					= std::is_class_v<T>;

	template <typename T>
	static constexpr bool	IsUnion					= std::is_union_v<T>;

	template <typename T>
	static constexpr bool	IsConst					= std::is_const_v<T>;

	template <typename T>
	static constexpr bool	IsConstRef				= std::is_const_v< std::remove_reference_t<T> >;

	template <typename T>
	static constexpr bool	IsConstPtr				= std::is_const_v< std::remove_pointer_t<T> >;

	template <typename T>
	static constexpr bool	IsAnyConst				= IsConst<T> or IsConstRef<T> or IsConstPtr<T>;

	template <typename T>
	static constexpr bool	IsVolatile				= std::is_volatile_v<T>;

	template <typename T1, typename T2>
	static constexpr bool	IsSame					= std::is_same_v<T1, T2>;

	template <typename T>
	static constexpr bool	IsVoid					= std::is_void_v<T>;

	template <typename Base, typename Derived>
	static constexpr bool	IsBaseOf				= std::is_base_of_v< Base, Derived >;

	template <typename Base, typename Derived>
	static constexpr bool	IsBaseOfNotSame			= std::is_base_of_v< Base, Derived >	and
													  not std::is_same_v< Base, Derived >;

	template <typename T>
	static constexpr bool	IsEmpty					= std::is_empty_v<T>;

	template <typename From, typename To>
	static constexpr bool	IsConvertible			= std::is_convertible_v<From, To>;	// TODO: is_nothrow_convertible (C++20)

	template <typename T>
	static constexpr bool	IsArithmetic			= std::is_arithmetic_v<T>;

	template <typename T>
	static constexpr bool	IsAbstract				= std::is_abstract_v<T>;

	template <typename T>
	static constexpr bool	IsDefaultConstructible	= std::is_default_constructible_v<T>;

	template <typename T>
	static constexpr bool	IsCopyConstructible		= std::is_copy_constructible_v<T>;

	template <typename T>
	static constexpr bool	IsCopyAssignable		= std::is_copy_assignable_v<T>;

	template <typename T>
	static constexpr bool	IsMoveAssignable		= std::is_move_assignable_v<T>;

	template <typename T>
	static constexpr bool	IsMoveConstructible		= std::is_move_constructible_v<T>;

	template <typename T, typename ...Args>
	static constexpr bool	IsConstructible			= std::is_constructible_v< T, Args... >;


#ifdef AE_ENABLE_EXCEPTIONS
	template <typename T>
	struct TNothrowCopyCtor : CT_Bool<
								std::is_nothrow_copy_constructible_v<T>		or
								std::is_trivially_copy_constructible_v<T>	>
	{ StaticAssert( IsCopyConstructible<T> ); };

	template <typename T>
	struct TNothrowMoveCtor : CT_Bool<
								std::is_nothrow_move_constructible_v<T>		or
								std::is_trivially_move_constructible_v<T>	>
	{ StaticAssert( IsMoveConstructible<T> ); };

	template <typename T>
	struct TNothrowDefaultCtor : CT_Bool<
									std::is_nothrow_default_constructible_v<T>	or
									std::is_trivially_default_constructible_v<T> >
	{ StaticAssert( IsDefaultConstructible<T> ); };

	template <typename T, typename ...Args>
	struct TNothrowCtor : CT_Bool<
							std::is_nothrow_constructible_v< T, Args... >	or
							std::is_trivially_constructible_v< T, Args... >	>
	{ StaticAssert( IsConstructible< T, Args... >); };

	template <typename T>
	struct TNothrowCopyAssignable : CT_Bool<
										std::is_nothrow_copy_assignable_v<T>	or
										TNothrowCopyCtor<T>::value				>{};	// if used dtor + ctor

	template <typename T>
	struct TNothrowMoveAssignable : CT_Bool<
										std::is_nothrow_move_assignable_v<T>	or
										TNothrowMoveCtor<T>::value				>{};	// if used dtor + move ctor

	template <typename Fn, typename ...Args>
	struct TNothrowInvocable : CT_Bool< std::is_nothrow_invocable_v< Fn, Args... >>
	{ StaticAssert( std::is_invocable_v< Fn, Args... >); };

	template <typename T>
	struct TNothrowable : CT_Bool<
							std::is_nothrow_move_constructible_v<T>		and
							std::is_nothrow_default_constructible_v<T>	and
							std::is_nothrow_copy_assignable_v<T>		and
							std::is_nothrow_copy_constructible_v<T>		and
							std::is_nothrow_destructible_v<T>			and
							std::is_nothrow_move_assignable_v<T>		>{};

#else

	template <typename T>
	struct TNothrowCopyCtor			: CT_True	{ StaticAssert( IsCopyConstructible<T> ); };

	template <typename T>
	struct TNothrowMoveCtor			: CT_True	{ StaticAssert( std::is_move_constructible_v<T> ); };

	template <typename T>
	struct TNothrowDefaultCtor		: CT_True	{ StaticAssert( IsDefaultConstructible<T> ); };

	template <typename T, typename ...Args>
	struct TNothrowCtor				: CT_True	{ StaticAssert( IsConstructible< T, Args... >); };

	template <typename T>
	struct TNothrowCopyAssignable	: CT_True	{};

	template <typename T>
	struct TNothrowMoveAssignable	: CT_True	{};

	template <typename Fn, typename ...Args>
	struct TNothrowInvocable		: CT_True	{ StaticAssert( std::is_invocable_v< Fn, Args... >); };

	template <typename T>
	struct TNothrowable				: CT_True	{};
#endif

	template <typename T>
	static constexpr bool	IsNothrowCopyCtor		= TNothrowCopyCtor<T>::value;

	template <typename ...Types>
	static constexpr bool	AllNothrowCopyCtor		= (... and IsNothrowCopyCtor<Types>);

	template <typename T>
	static constexpr bool	IsNothrowMoveCtor		= TNothrowMoveCtor<T>::value;
	
	template <typename ...Types>
	static constexpr bool	AllNothrowMoveCtor		= (... and IsNothrowMoveCtor<Types>);

	template <typename T>
	static constexpr bool	IsNothrowDefaultCtor	= TNothrowDefaultCtor<T>::value;

	template <typename ...Types>
	static constexpr bool	AllNothrowDefaultCtor	= (... and IsNothrowDefaultCtor<Types>);

	template <typename T, typename ...Args>
	static constexpr bool	IsNothrowCtor			= TNothrowCtor<T,Args...>::value;

	template <typename T>
	static constexpr bool	IsNothrowable			= TNothrowable<T>::value;

	template <typename T>
	static constexpr bool	IsNothrowCopyAssignable	= TNothrowCopyAssignable<T>::value;
	
	template <typename ...Types>
	static constexpr bool	AllNothrowCopyAssignable	= (... and IsNothrowCopyAssignable<Types>);

	template <typename T>
	static constexpr bool	IsNothrowMoveAssignable	= TNothrowMoveAssignable<T>::value;
	
	template <typename ...Types>
	static constexpr bool	AllNothrowMoveAssignable	= (... and IsNothrowMoveAssignable<Types>);

	template <typename Fn, typename ...Args>
	static constexpr bool	IsNothrowInvocable		= TNothrowInvocable< Fn, Args... >::value;


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


	template <bool Test, typename IfTrue, typename IfFalse>
	using Conditional		= std::conditional_t< Test, IfTrue, IfFalse >;


	template <typename T>	NdCx__ usize			CT_SizeofInBits (const T&)	__NE___ { return sizeof(T) << 3; }
	template <typename T>	static constexpr usize	CT_SizeOfInBits				=		sizeof(T) << 3;


	template <usize Bits>
	using BitSizeToUInt		= Conditional< Bits <= CT_SizeOfInBits<ubyte>, ubyte,
								Conditional< Bits <= CT_SizeOfInBits<ushort>, ushort,
									Conditional< Bits <= CT_SizeOfInBits<uint>, uint,
										Conditional< Bits <= CT_SizeOfInBits<ulong>, ulong,
											void >>>>;

	template <usize Bits>
	using BitSizeToInt		= Conditional< Bits <= CT_SizeOfInBits<sbyte>, sbyte,
								Conditional< Bits <= CT_SizeOfInBits<sshort>, sshort,
									Conditional< Bits <= CT_SizeOfInBits<sint>, sint,
										Conditional< Bits <= CT_SizeOfInBits<slong>, slong,
											void >>>>;

	template <usize Bits>
	using BitSizeToFloat	= Conditional< Bits <= CT_SizeOfInBits<float>, float,
								Conditional< Bits <= CT_SizeOfInBits<double>, double,
									void >>;


	template <usize ByteCount>
	using ByteSizeToUInt	= BitSizeToUInt< ByteCount * 8 >;

	template <usize ByteCount>
	using ByteSizeToInt		= BitSizeToInt< ByteCount * 8 >;

	template <usize ByteCount>
	using ByteSizeToFloat	= BitSizeToFloat< ByteCount * 8 >;


	template <typename T>
	using ToUnsignedInteger	= BitSizeToUInt< CT_SizeOfInBits<T> >;

	template <typename T>
	using ToSignedInteger	= BitSizeToInt< CT_SizeOfInBits<T> >;

	template <typename T>
	using ToFloatPoint		= BitSizeToInt< CT_SizeOfInBits<T> >;


	template <typename T>
	using ReferenceCollapsing	= Conditional< IsRValueRef<T>, T,
									Conditional< IsLValueRef<T>, T&, void >>;


	namespace _hidden_
	{
		template <typename T>
		struct RemoveAllQual
		{
			using _NextType	= RemoveReference< RemovePointer< RemoveArray< RemoveCV< T >>>>;
			using type		= typename Conditional< (IsSame< _NextType, T >),
								TypeToType<T>,
								DeferredTemplate2< RemoveAllQual, _NextType > >::type;
		};

		template <typename T, template <typename...> class Templ>
		struct _IsSpecializationOf		: CT_False {};

		template <template <typename...> class Templ, typename... Args>
		struct _IsSpecializationOf< Templ<Args...>, Templ > : CT_True {};

		template <template <typename ...> class Left, template <typename ...> class Right>
		struct _IsSameTemplates			: CT_False {};

		template <template <typename ...> class T>
		struct _IsSameTemplates< T, T >	: CT_True {};
		
		template <typename T, usize = sizeof(T)>
		CT_True   _IsCompleteType2 (T*);
		CT_False  _IsCompleteType2 (...);

		template <typename T>
		using _IsCompleteType = decltype(_IsCompleteType2( std::declval<T*>() ));

	} // _hidden_

	template <typename T>
	using RemoveAllQualifiers	= typename Base::_hidden_::RemoveAllQual<T>::type;


	template <typename T, template <typename...> class Templ>
	static constexpr bool	IsSpecializationOf	= Base::_hidden_::_IsSpecializationOf< T, Templ >::value;

	template <template <typename ...> class Left, template <typename ...> class Right>
	static constexpr bool	IsSameTemplates		= Base::_hidden_::_IsSameTemplates< Left, Right >::value;
	
	template <typename T>
	struct TIsCompleteType : Base::_hidden_::_IsCompleteType< RemoveAllQualifiers<T> > {};

	template <typename T>
	static constexpr bool	IsCompleteType		= TIsCompleteType< T >::value;
	
	template <typename ...Types>
	static constexpr bool	AllTypesAreComplete	= (... and IsCompleteType<Types>);


	template <typename Base, typename Derived>
	static constexpr bool	IsBaseOfNoQual		= std::is_base_of_v< Base, RemoveAllQualifiers<Derived> >;

	template <typename T1, typename T2>
	static constexpr bool	IsSameTypesNoQual	= std::is_same_v< RemoveAllQualifiers<T1>, RemoveAllQualifiers<T2> >;


	namespace _hidden_
	{
		template <typename T>
		struct _IsStaticArray					: CT_Bool< std::is_array_v<T> > {};

		template <typename T, usize S>
		struct _IsStaticArray< std::array<T,S> >: CT_True {};
	}
	template <typename T>
	static constexpr bool	IsStaticArray = Base::_hidden_::_IsStaticArray<T>::value;

	namespace _hidden_
	{
		template <typename T>
		struct _IsStdVector						: CT_False {};

		template <typename T, typename A>
		struct _IsStdVector< std::vector<T,A> >	: CT_True {};
	}
	template <typename T>
	static constexpr bool	IsArray = Base::_hidden_::_IsStdVector<T>::value;


	template <typename T>
	static constexpr bool	IsDuration = IsSpecializationOf< T, std::chrono::duration >;


	namespace _hidden_
	{
		template <typename T>
		static constexpr bool	_IsChar = (IsSame< T, CharAnsi >	or
										   IsSame< T, CharUtf8 >	or
										   IsSame< T, CharUtf16 >	or
										   IsSame< T, CharUtf32 >	or
										   IsSame< T, wchar_t >);
	}
	template <typename T>
	static constexpr bool	IsChar = Base::_hidden_::_IsChar< RemoveCV<T> >;


	namespace _hidden_
	{
		template <typename T>
		struct _UnwrapType
		{
			using _NextType	= typename TUnwrap< RemoveAllQualifiers<T> >::type;
			using type		= typename Conditional< (IsSame< _NextType, T >),
								TypeToType<T>,
								DeferredTemplate2< _UnwrapType, _NextType > >::type;
		};

		template <typename A>
		struct AreSameTypes
		{
			template <typename B>
			struct Impl : CT_Bool< IsSame< A, B > >{};
		};
	}
	template <typename T>
	using UnwrapType = typename TUnwrap< RemoveAllQualifiers<T> >::type;

	template <typename T>
	using UnwrapRecursive = typename Base::_hidden_::_UnwrapType<T>::type;


	template <typename T, typename ...Types>
	static constexpr bool	AllAreSameTypes		= (... and Base::_hidden_::AreSameTypes<T>::template Impl<Types>::value);

	namespace _hidden_
	{
		template <typename Lambda, int=(Lambda{}(), 0)>
		constexpr bool  _is_constexpr (Lambda)	{ return true; }
		constexpr bool  _is_constexpr (...)		{ return false; }
	}
	#define IsConstExpr( ... )	(AE::Base::_hidden_::_is_constexpr([]{ __VA_ARGS__; }))
//-----------------------------------------------------------------------------

	

	template <typename T>
	static constexpr bool	IsScalarOrEnum		= IsScalar<T> or IsEnum<T>;

	template <typename T>
	static constexpr bool	HasScalarBitOp		= (IsScalar<T> and IsInteger<T>) or IsEnum<T>;
//-----------------------------------------------------------------------------


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
	NdCx__ auto  MaxValue ()
	{
		using NL = std::numeric_limits< RemoveAllQualifiers<T> >;
		StaticAssert( NL::is_specialized );
		return NL::max();
	}

	template <typename T>
	NdCx__ auto  MinValue ()
	{
		using NL = std::numeric_limits< RemoveAllQualifiers<T> >;
		StaticAssert( NL::is_specialized );
		return NL::min();
	}

/*
=================================================
	Infinity
=================================================
*/
	template <typename T>
	NdCx__ auto  Infinity ()
	{
		StaticAssert( IsAnyFloatPoint<T> );
		using NL = std::numeric_limits< RemoveAllQualifiers<T> >;
		StaticAssert( NL::is_specialized );
		return NL::infinity();
	}

/*
=================================================
	NaN
=================================================
*/
	template <typename T>
	NdCx__ auto  NaN ()
	{
		StaticAssert( IsAnyFloatPoint<T> );
		using NL = std::numeric_limits< RemoveAllQualifiers<T> >;
		StaticAssert( NL::is_specialized );
		return NL::quiet_NaN();
	}

/*
=================================================
	IsInfinity / IsNaN / IsFinite (scalar)
=================================================
*/
	template <typename T> requires(IsFloatPoint<T>)
	ND_ bool  IsInfinity (const T x) __NE___
	{
		return std::isinf( x );
	}

	template <typename T> requires(IsFloatPoint<T>)
	ND_ bool  IsNaN (const T x) __NE___
	{
		return std::isnan( x );
	}

	template <typename T>  requires(IsFloatPoint<T>)
	ND_ bool  IsFinite (const T x) __NE___
	{
		return std::isfinite( x );
	}

/*
=================================================
	IsInfinity / IsNaN / IsFinite (chrono)
=================================================
*/
	template <typename Rep, typename Period> requires(IsFloatPoint<Rep>)
	ND_ bool  IsInfinity (const std::chrono::duration<Rep, Period> x) __NE___
	{
		return IsInfinity( x.count() );
	}

	template <typename Rep, typename Period> requires(IsFloatPoint<Rep>)
	ND_ bool  IsNaN (const std::chrono::duration<Rep, Period> x) __NE___
	{
		return IsNaN( x.count() );
	}

	template <typename Rep, typename Period> requires(IsFloatPoint<Rep>)
	ND_ bool  IsFinite (const std::chrono::duration<Rep, Period> x) __NE___
	{
		return IsFinite( x.count() );
	}

/*
=================================================
	IsMemCopyAvailable
----
	allow to use MemCopy and MemMove
=================================================
*/
	template <typename T>
	struct TMemCopyAvailable				: CT_Bool< IsTrivial<T> >{};

	template <typename T, usize I>
	struct TMemCopyAvailable< T[I] >		: CT_Bool< TMemCopyAvailable<T>::value >{};

	template <typename T, usize I>
	struct TMemCopyAvailable< const T[I] >	: CT_Bool< TMemCopyAvailable<T>::value >{};

	template <typename T>
	static constexpr bool	IsMemCopyAvailable = TMemCopyAvailable<T>::value;


/*
=================================================
	IsZeroMemAvailable
----
	Allow to use ZeroMem as a constructor.
=================================================
*/
	template <typename T>
	struct TZeroMemAvailable				: CT_Bool< IsTrivial<T> >{};

	template <typename T, usize I>
	struct TZeroMemAvailable< T[I] >		: CT_Bool< TZeroMemAvailable<T>::value >{};

	template <typename T, usize I>
	struct TZeroMemAvailable< const T[I] >	: CT_Bool< TZeroMemAvailable<T>::value >{};

	template <typename T>
	static constexpr bool	IsZeroMemAvailable = TZeroMemAvailable<T>::value;

/*
=================================================
	IsTriviallySerializable
----
	Allow to use MemCopy to/from file when serializing / deserializing.
	Objects must not have pointers and non-trivial fields.
=================================================
*/
	template <typename T>
	struct TTriviallySerializable : CT_Bool<
										IsTrivial<T>		and
										not IsAnyPointer<T>	and
										not IsReference<T>	>{};

	template <typename T, usize I>
	struct TTriviallySerializable< T[I] > : CT_Bool< TTriviallySerializable< RemoveCV<T> >::value >{};

	template <typename T, usize I>
	struct TTriviallySerializable< const T[I] > : CT_Bool< TTriviallySerializable< RemoveCV<T> >::value >{};


	template <typename T>
	static constexpr bool	IsTriviallySerializable = TTriviallySerializable< RemoveCV<T> >::value;

/*
=================================================
	IsTriviallyDestructible
----
	Allow to skip destructor.
=================================================
*/
	template <typename T>
	struct TTriviallyDestructible : CT_Bool<
										std::is_trivially_destructible_v<T> or
										IsZeroMemAvailable<T>				>{};

	template <typename T>
	static constexpr bool	IsTriviallyDestructible = TTriviallyDestructible< RemoveCV<T> >::value;

/*
=================================================
	IsTriviallyConstructible
----
	Allow to skip constructor.
=================================================
*/
	template <typename T>
	struct TTriviallyConstructible : CT_Bool<
										std::is_trivially_constructible_v<T> or
										IsZeroMemAvailable<T>				>{};

	template <typename T>
	static constexpr bool	IsTriviallyConstructible = TTriviallyConstructible< RemoveCV<T> >::value;
	
/*
=================================================
	IsTriviallyMovable
----
	allow to use MemCopy and MemMove only for move operation
=================================================
*/
	template <typename T>
	struct TTriviallyMovable				: CT_Bool< IsMemCopyAvailable<T> >{};

	template <typename T, usize I>
	struct TTriviallyMovable< T[I] >		: CT_Bool< TTriviallyMovable<T>::value >{};

	template <typename T, usize I>
	struct TTriviallyMovable< const T[I] >	: CT_Bool< TTriviallyMovable<T>::value >{};

	template <typename T>
	static constexpr bool	IsTriviallyMovable = TTriviallyMovable<T>::value;


} // AE::Base
