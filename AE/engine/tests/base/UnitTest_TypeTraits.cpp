// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	StaticAssert( sizeof(bool)	 == 1 );
	StaticAssert( sizeof(Bool32) == 4 );
	StaticAssert( CHAR_BIT == 8 );

	StaticAssert( sizeof(CharAnsi)	 == 1 );
	StaticAssert( sizeof(CharUtf8)	 == 1 );
	StaticAssert( sizeof(CharUtf16)	 == 2 );
	StaticAssert( sizeof(CharUtf32)	 == 4 );

	#ifdef AE_PLATFORM_WINDOWS
	StaticAssert( sizeof(wchar_t) == 2 );
	#endif

	StaticAssert( sizeof(char)  == 1 );			StaticAssert( IsInteger<         char >);
	StaticAssert( sizeof(sbyte) == 1 );			StaticAssert( IsSignedInteger<   sbyte >);
	StaticAssert( sizeof(ubyte) == 1 );			StaticAssert( IsUnsignedInteger< ubyte >);

	StaticAssert( sizeof(short)  == 2 );
	StaticAssert( sizeof(sshort) == 2 );		StaticAssert( IsSignedInteger<   sshort >);
	StaticAssert( sizeof(ushort) == 2 );		StaticAssert( IsUnsignedInteger< ushort >);

	StaticAssert( sizeof(int)  == 4 );			StaticAssert( IsSignedInteger<   int  >);
	StaticAssert( sizeof(sint) == 4 );			StaticAssert( IsSignedInteger<   sint >);
	StaticAssert( sizeof(uint) == 4 );			StaticAssert( IsUnsignedInteger< uint >);

	StaticAssert( sizeof(slong) == 8 );			StaticAssert( IsSignedInteger<   slong >);
	StaticAssert( sizeof(ulong) == 8 );			StaticAssert( IsUnsignedInteger< ulong >);

	StaticAssert( sizeof(float)  == 4 );		StaticAssert( IsFloatPoint< float  >);
	StaticAssert( sizeof(double) == 8 );		StaticAssert( IsFloatPoint< double >);

	StaticAssert( sizeof(UFloat8)  == 1 );		StaticAssert( IsAnyFloatPoint< UFloat8 >);
	StaticAssert( sizeof(SFloat16) == 2 );		StaticAssert( IsAnyFloatPoint< SFloat16 >);
	StaticAssert( sizeof(UFloat16) == 2 );		StaticAssert( IsAnyFloatPoint< UFloat16 >);

	StaticAssert( not IsSame< CharAnsi,  CharUtf8  >);
	StaticAssert( not IsSame< CharAnsi,  CharUtf16 >);
	StaticAssert( not IsSame< CharAnsi,  CharUtf32 >);
	StaticAssert( not IsSame< CharAnsi,  wchar_t   >);

	StaticAssert( not IsSame< CharUtf8,  CharUtf16 >);
	StaticAssert( not IsSame< CharUtf8,  CharUtf32 >);
	StaticAssert( not IsSame< CharUtf8,  wchar_t   >);

	StaticAssert( not IsSame< CharUtf16, CharUtf32 >);
	StaticAssert( not IsSame< CharUtf16, wchar_t   >);

	StaticAssert( not IsSame< CharUtf32, wchar_t   >);

	#if AE_PLATFORM_BITS == 32
	StaticAssert( sizeof(ssize) == 4 );			StaticAssert( IsSignedInteger<   ssize >);
	StaticAssert( sizeof(usize) == 4 );			StaticAssert( IsUnsignedInteger< usize >);
	#endif

	#if AE_PLATFORM_BITS == 64
	StaticAssert( sizeof(ssize) == 8 );			StaticAssert( IsSignedInteger<   ssize >);
	StaticAssert( sizeof(usize) == 8 );			StaticAssert( IsUnsignedInteger< usize >);
	#endif


	static void  Test_IsSpecializationOf ()
	{
		using T1 = ArrayView<int>;
		StaticAssert( IsSpecializationOf< T1, ArrayView > );
		StaticAssert( not IsSpecializationOf< T1, std::tuple > );

		using T2 = ArrayView<int> *;
		StaticAssert( not IsSpecializationOf< T2, ArrayView > );
		StaticAssert( not IsSpecializationOf< T2, std::tuple > );

		using T3 = std::tuple< int >;
		StaticAssert( IsSpecializationOf< T3, std::tuple > );
		StaticAssert( not IsSpecializationOf< T3, ArrayView > );
	}


	static void  Test_IsCompleteType ()
	{
		struct Incomplete;
		struct Complete {};

		StaticAssert( not IsCompleteType< Incomplete >);
		StaticAssert( IsCompleteType< Complete >);
		StaticAssert( IsCompleteType< int >);

		StaticAssert( not AllTypesAreComplete< Incomplete, Complete, int >);
		StaticAssert( AllTypesAreComplete< Complete, int >);
	}


	static void  Test_IsTrivial ()
	{
		StaticAssert( IsZeroMemAvailable<int> );
		StaticAssert( IsZeroMemAvailable<int2> );
		StaticAssert( IsZeroMemAvailable<float3> );
		StaticAssert( IsZeroMemAvailable<float4x4> );
		StaticAssert( IsZeroMemAvailable<float[4]> );
		StaticAssert( IsZeroMemAvailable<const int[8]> );

		StaticAssert( IsMemCopyAvailable<int> );
		StaticAssert( IsMemCopyAvailable<int2> );
		StaticAssert( IsMemCopyAvailable<float3> );
		StaticAssert( IsMemCopyAvailable<float4x4> );
		StaticAssert( IsMemCopyAvailable<float[4]> );
		StaticAssert( IsMemCopyAvailable<const int[8]> );

		struct PODType
		{
			int		i;
			float	f;
		};

		struct TrivialType
		{
			int		i;
			float	f;

			TrivialType () : i{2} {}
			TrivialType (const TrivialType &) = default;
		};

		//StaticAssert( IsPOD<PODType> );
		//StaticAssert( not IsPOD<TrivialType> );

		StaticAssert( IsTrivial<PODType> );
		StaticAssert( IsTrivial<TrivialType> );
	}


	static void  Test_RemoveAllQualifiers ()
	{
		using T1 = RemoveAllQualifiers< const int* const* const& >;
		StaticAssert(( IsSame< int, T1 >));
	}


	static void  Test_Tuple()
	{
		StaticAssert( IsNoExcept( Tuple{ 1u, 2.2f } ));
		StaticAssert( not IsNoExcept( Tuple{ 0.9, -10, "aa"s } ));
	}


	static void  Test_TupleConcat ()
	{
		auto	t = TupleConcat( Tuple{ 1u, 2.2f }, Tuple{ 0.9, -10, "aa"s }, Tuple{ 9ull });
		TEST( t.Count() == 6 );
		TEST( t.Get<0>() == 1u );
		TEST( t.Get<1>() == 2.2f );
		TEST( t.Get<2>() == 0.9 );
		TEST( t.Get<3>() == -10 );
		TEST( t.Get<4>() == "aa"s );
		TEST( t.Get<5>() == 9ull );
	}


	static void  Test_TupleRef ()
	{
		int		a0	= 1;
		uint	a1	= 2;
		ulong	a2	= 3;
		float	a3	= 4.1f;
		auto	res	= TupleRef{ &a0, &a1, &a2, &a3 };

		a0 += 1;	a1 += 2;	a2 += 3;	a3 += 1.1f;

		TEST( res.Get<int>()	== a0 );
		TEST( res.Get<uint>()	== a1 );
		TEST( res.Get<ulong>()	== a2 );
		TEST( res.Get<float>()	== a3 );
		TEST( res.AllNonNull() );

		auto  [b0, b1, b2, b3] = RVRef(res);

		a0 += 1;	a1 += 2;	a2 += 3;	a3 += 1.1f;

		TEST( b0 == a0 );
		TEST( b1 == a1 );
		TEST( b2 == a2 );
		TEST( b3 == a3 );

		auto	res2	= TupleRef{ &a0, &a1, &a2, &a3, null };
		TEST( not res2.AllNonNull() );
		TEST( not res2.AllNull() );
	}


	struct TrivialThrowable
	{
		int	i;

		TrivialThrowable ()					__Th___ = default;
		explicit TrivialThrowable (int ii)	__Th___ : i{ii} {}
	};

	struct NontrivialNothrowable
	{
		String	str;

		NontrivialNothrowable ()						__NE___ { str = "11111"; }
		explicit NontrivialNothrowable (const String &s)__NE___ : str{s} {}
		~NontrivialNothrowable ()						__NE___ {}

		template <typename A, typename B>
		NontrivialNothrowable (A a, B b)				__NE___ { str = ToString(a) + ToString(b); }
	};

	struct NontrivialThrowable
	{
		int	i;

		NontrivialThrowable ()					__Th___	{}
		explicit NontrivialThrowable (int ii)	__Th___ : i{ii} {}
	};

	static void  Test_Nothrow ()
	{
		CheckNothrow( IsNothrowCtor< TrivialThrowable >);
		CheckNothrow( not IsNothrowCtor< TrivialThrowable, int >);
		CheckNothrow( IsNothrowCtor< NontrivialNothrowable >);
		CheckNothrow( IsNothrowCtor< NontrivialNothrowable, String >);
		CheckNothrow( IsNothrowCtor< NontrivialNothrowable, float, int >);
		CheckNothrow( not IsNothrowCtor< NontrivialThrowable >);
		CheckNothrow( not IsNothrowCtor< NontrivialThrowable, int >);
	}


	static void  Test_ArrayView ()
	{
		{
			std::vector<int>	a1 {0,1,2};
			auto				a2 = ArrayView{a1};
			StaticAssert( IsSame< decltype(a2), ArrayView<int> >);
		}{
			std::vector<int>	b1 {0,1,2};
			auto				b2 = ArrayView{b1.data(), b1.size()};
			StaticAssert( IsSame< decltype(b2), ArrayView<int> >);
		}{
			int		c1[]	= {0,1,2};
			auto	c2		= ArrayView{c1};
			StaticAssert( IsSame< decltype(c2), ArrayView<int> >);
		}{
			auto	d2		= ArrayView{List{ 0, 1, 2 }};
			StaticAssert( IsSame< decltype(d2), ArrayView<int> >);
		}{
			StaticArray< int, 5 >	e1 {0,1,2,3};
			auto					e2 = ArrayView{e1};
			StaticAssert( IsSame< decltype(e2), ArrayView<int> >);
		}
	}


	static void  Test_IsConst ()
	{
		StaticAssert( not IsConst<int> );
		StaticAssert( IsConst<const int> );
		StaticAssert( IsConst<int const> );
		StaticAssert( not IsConst<int const*> );
		StaticAssert( IsConstPtr<int const*> );
		StaticAssert( IsConstPtr<int* const*> );
		StaticAssert( not IsConstPtr<int* const> );
		StaticAssert( IsConst<int* const> );
		StaticAssert( IsConstRef<int const&> );
		StaticAssert( not IsConstRef<int &> );

		StaticAssert( IsAnyConst<const int> );
		StaticAssert( IsAnyConst<int const> );
		StaticAssert( IsAnyConst<int const*> );
		StaticAssert( IsAnyConst<int* const*> );
		StaticAssert( IsAnyConst<int* const> );
		StaticAssert( IsAnyConst<int const&> );
	}


	template <typename FN>
	static void  Test_IsNothrowInvocable_FnTh (FN &&fn)
	{
		CheckNothrow( not IsNothrowInvocable< FN >);
		Unused( fn );
	}

	template <typename FN>
	static void  Test_IsNothrowInvocable_FnNE (FN &&fn)
	{
		CheckNothrow( IsNothrowInvocable< FN >);
		Unused( fn );
	}

	struct Test_IsNothrowInvocable_B
	{
		template <typename T>
		void operator () () __NE___ {}
	};

	static void  Test_IsNothrowInvocable ()
	{
		CheckNothrow( not IsNothrowInvocable< void () >);
		CheckNothrow( IsNothrowInvocable< void () __NE___ >);
		CheckNothrow( IsNothrowInvocable< void (*) () __NE___ >);

		const auto	fn1 = [] () {};
		CheckNothrow( not IsNothrowInvocable< decltype(fn1) >);
		Test_IsNothrowInvocable_FnTh( fn1 );
		Test_IsNothrowInvocable_FnTh( [] () { return true; } );

		const auto	fn2 = [] () __NE___ {};
		CheckNothrow( IsNothrowInvocable< decltype(fn2) >);
		Test_IsNothrowInvocable_FnNE( fn2 );
		Test_IsNothrowInvocable_FnNE( [] () __NE___ { return true; } );

		struct A { void operator () () __NE___ {} };
		A a0;
		Test_IsNothrowInvocable_FnNE( A{} );
		Test_IsNothrowInvocable_FnNE( a0 );

		using B = Test_IsNothrowInvocable_B;
		CheckNothrow( IsNothrowInvocable< decltype(&B::template operator()<int>), B& >);
	}


	static void  Test_IsPointer ()
	{
		class Class {};

		StaticAssert(     IsPointer< int* >);
		StaticAssert(     IsPointer< int const* >);
		StaticAssert(     IsPointer< int const* const >);
		StaticAssert( not IsPointer< int >);
		StaticAssert( not IsPointer< int* &>);
		StaticAssert( not IsPointer< int* const& >);
		StaticAssert( not IsPointer< int (*) (int) >);
		StaticAssert( not IsPointer< int (Class::*) (int) >);
		StaticAssert( not IsPointer< int (Class::*) (int) const >);
		StaticAssert( not IsPointer< int (Class::*) >);
		
		StaticAssert(     IsAnyPointer< int* >);
		StaticAssert(     IsAnyPointer< int const* >);
		StaticAssert(     IsAnyPointer< int const* const >);
		StaticAssert( not IsAnyPointer< int >);
		StaticAssert( not IsAnyPointer< int* &>);
		StaticAssert( not IsAnyPointer< int* const& >);
		StaticAssert(     IsAnyPointer< int (*) (int) >);
		StaticAssert(     IsAnyPointer< int (Class::*) (int) >);
		StaticAssert(     IsAnyPointer< int (Class::*) (int) const >);
		StaticAssert(     IsAnyPointer< int (Class::*) >);
		
		StaticAssert( not IsFunctionPointer< int* >);
		StaticAssert( not IsFunctionPointer< int const* >);
		StaticAssert( not IsFunctionPointer< int const* const >);
		StaticAssert( not IsFunctionPointer< int >);
		StaticAssert( not IsFunctionPointer< int* &>);
		StaticAssert( not IsFunctionPointer< int* const& >);
		StaticAssert(     IsFunctionPointer< int (*) (int) >);
		StaticAssert( not IsFunctionPointer< int (Class::*) (int) >);
		StaticAssert( not IsFunctionPointer< int (Class::*) (int) const >);
		StaticAssert( not IsFunctionPointer< int (Class::*) >);
		
		StaticAssert( not IsMemberPointer< int* >);
		StaticAssert( not IsMemberPointer< int const* >);
		StaticAssert( not IsMemberPointer< int const* const >);
		StaticAssert( not IsMemberPointer< int >);
		StaticAssert( not IsMemberPointer< int* &>);
		StaticAssert( not IsMemberPointer< int* const& >);
		StaticAssert( not IsMemberPointer< int (*) (int) >);
		StaticAssert(     IsMemberPointer< int (Class::*) (int) >);
		StaticAssert(     IsMemberPointer< int (Class::*) (int) const >);
		StaticAssert(     IsMemberPointer< int (Class::*) >);

		StaticAssert( not IsMemberObjectPointer< int* >);
		StaticAssert( not IsMemberObjectPointer< int const* >);
		StaticAssert( not IsMemberObjectPointer< int const* const >);
		StaticAssert( not IsMemberObjectPointer< int >);
		StaticAssert( not IsMemberObjectPointer< int* &>);
		StaticAssert( not IsMemberObjectPointer< int* const& >);
		StaticAssert( not IsMemberObjectPointer< int (*) (int) >);
		StaticAssert( not IsMemberObjectPointer< int (Class::*) (int) >);
		StaticAssert( not IsMemberObjectPointer< int (Class::*) (int) const >);
		StaticAssert(     IsMemberObjectPointer< int (Class::*) >);
		
		StaticAssert( not IsMemberFunctionPointer< int* >);
		StaticAssert( not IsMemberFunctionPointer< int const* >);
		StaticAssert( not IsMemberFunctionPointer< int const* const >);
		StaticAssert( not IsMemberFunctionPointer< int >);
		StaticAssert( not IsMemberFunctionPointer< int* &>);
		StaticAssert( not IsMemberFunctionPointer< int* const& >);
		StaticAssert( not IsMemberFunctionPointer< int (*) (int) >);
		StaticAssert(     IsMemberFunctionPointer< int (Class::*) (int) >);
		StaticAssert(     IsMemberFunctionPointer< int (Class::*) (int) const >);
		StaticAssert( not IsMemberFunctionPointer< int (Class::*) >);
	}
	//-----------------------------------------------------


	AE_FLATTEN_FN void Test_Attributes_InlineFn ()
	{}

	static bool  g_Var1_NoSideEffects = true;

	AE_NOSIDEEFFECTS bool  NoSideEffects_Test1 (int in)
	{
		return in == 0 and g_Var1_NoSideEffects;
	}
	
	AE_NOSIDEEFFECTS bool  NoSideEffects_Test2 (int in)
	{
		if ( in == 1 )
			g_Var1_NoSideEffects ^= false;

		return in == 0 and g_Var1_NoSideEffects;
	}

	AE_NOSIDEEFFECTS bool  NoSideEffects_Test3 (int* in)
	{
		if ( *in == 1 )
			g_Var1_NoSideEffects ^= false;

		return *in == 0 and g_Var1_NoSideEffects;
	}

	static void  Test_Attributes ()
	{
		AE_INLINE_ALL	Unused( TupleConcat( Tuple{ 1u, 2.2f }, Tuple{ 0.9, -10, "aa"s }, Tuple{ 9ull }));
		AE_INLINE_CALLS	Unused( TupleConcat( Tuple{ 1u, 2.2f }, Tuple{ 0.9, -10, "aa"s }, Tuple{ 9ull }));

		AE_INLINE_ALL {
			auto	t = TupleConcat( Tuple{ 1u, 2.2f }, Tuple{ 0.9, -10, "aa"s }, Tuple{ 9ull });
			Unused( t.Get<0>() );
		}

		AE_INLINE_CALLS	{
			auto	t = TupleConcat( Tuple{ 1u, 2.2f }, Tuple{ 0.9, -10, "aa"s }, Tuple{ 9ull });
			Unused( t.Get<0>() );
		}

		Test_Attributes_InlineFn();

		Unused( NoSideEffects_Test1( 0 ));
		Unused( NoSideEffects_Test2( 1 ));

		int i = 0;
		Unused( NoSideEffects_Test3( &i ));
	}
	//-----------------------------------------------------


	template <typename T>
	ND_ int  Test_Requires1 (T &&fn)
	{
		constexpr bool	has_operatorBool = requires(const T& t) { t.operator bool(); };

		if constexpr( has_operatorBool )
		{
			if ( not fn )
				return -1;
		}

		return fn();
	}
	

	template <typename T1, typename T2>
	concept Test_RequiresAdd = requires(T1 a, T2 b)
	{
		{a + b} -> SameAs<T1>;
	};

	template <typename T>
	concept Test_HasIntMember1 = requires(T obj)
	{
		{ obj.m } -> SameAs<int>;
	};

	template <typename T>
	concept Test_HasIntMember2 = requires(T obj)
	{
		{ obj.m } -> SameAs<int &>;
	};

#ifdef __cpp_auto_cast
	template <typename T>
	concept Test_HasIntMember3 = requires(T obj)
	{
		{auto{ obj.m }} -> SameAs<int>;
	};
#endif

	template <typename T>
	concept Test_HasIntMember4 = requires(T obj)
	{
		{ &obj.m } -> SameAs<int *>;
	};
	
	template <typename T>
	concept Test_HasIntMember5 = requires
	{
		{ &T::m } -> SameAs< int T::* >;
	};


	static void  Test_Concepts1 ()
	{
		TEST_Eq( Test_Requires1( [](){ return 1; }), 1 );
		TEST_Eq( Test_Requires1( std::function<int()>{} ), -1 );

		StaticAssert( Test_RequiresAdd< int, int >);
		StaticAssert( Test_RequiresAdd< uint, int >);
		StaticAssert( Test_RequiresAdd< int, short >);
		StaticAssert( not Test_RequiresAdd< int, uint >);

		struct ClassA {
			int			m;
		};
		struct ClassB {
			const int	m;
		};
		struct ClassC {
			int&		m;
		};

		StaticAssert( not Test_HasIntMember1< ClassA >);
		StaticAssert( Test_HasIntMember2< ClassA >);
		#ifdef __cpp_auto_cast
			StaticAssert( Test_HasIntMember3< ClassA >);
		#endif
		StaticAssert( Test_HasIntMember4< ClassA >);
		StaticAssert( Test_HasIntMember5< ClassA >);
			
		StaticAssert( not Test_HasIntMember1< ClassB >);
		StaticAssert( not Test_HasIntMember2< ClassB >); // const int&
		#ifdef __cpp_auto_cast
			StaticAssert( Test_HasIntMember3< ClassB >);
		#endif
		StaticAssert( not Test_HasIntMember4< ClassB >); // const int*
		StaticAssert( not Test_HasIntMember5< ClassB >);
			
		StaticAssert( not Test_HasIntMember1< ClassC >);
		StaticAssert( Test_HasIntMember2< ClassC >);
		#ifdef __cpp_auto_cast
			StaticAssert( Test_HasIntMember3< ClassC >);
		#endif
		StaticAssert( Test_HasIntMember4< ClassC >);
		StaticAssert( not Test_HasIntMember5< ClassC >);
	}

#if 1
	template <typename T>
	constexpr bool  Test_RequiresInIfConstexpr ()
	{
		static_assert( requires{ typename T::type; });

		if constexpr( requires{ typename T::type; })
		{
			Unused( typename T::type{1} );
			return true;
		}else
			return false;
	}

	static void  Test_Concepts2 ()
	{
		struct TypeWithType {
			using type = int;
		};
		StaticAssert( Test_RequiresInIfConstexpr<TypeWithType>() );
	}
#endif
	//-----------------------------------------------------

	
	ND_ static constexpr bool  CxFunction (uint value) __NE___
	{
		ASSERT_Cx( value != 3 );
		if_consteval()
		{
			CvAssert( value != 0 );
			//CvAssert( value == 0 );	// must be compile-time error
			return value == 1;
		}else{
			ASSERT( value != 0 );
			return value == 2;
		}
	}

	static void  Test_IsConstEvaluated ()
	{
		constexpr bool	a = CxFunction( 1 );
		StaticAssert( a );

		auto	fn		= &CxFunction;
		uint	tmp1	= 2;
		uint*	tmp2	= &tmp1;

		const bool	b = fn( *tmp2 );
		TEST( b );

		StaticAssert( IsConstExpr( Unused(CxFunction( 1 )); ));
		StaticAssert( not IsConstExpr( Unused(CxFunction( 3 )) ));

		#if 0
			constexpr bool	c = CxFunction( 3 );	// must be compile-time error
		#endif
		#if 0
			tmp1 = 3;
			const bool	d = fn( *tmp2 );			// must be runtime error
			TEST( not d );
		#endif
	}
	//-----------------------------------------------------


	constexpr int  ConstInitFunc1 (bool b)
	{
		return int(b);
	}

	int  ConstInitFunc2 (bool b)
	{
		return int(b);
	}

	static int				s_ConstInit1 = 0;
//	static constinit int	s_ConstInit2 = s_ConstInit1;						// error
	static constinit int	s_ConstInit3 = ConstInitFunc1( true );
//	static constinit int	s_ConstInit4 = ConstInitFunc2( true );				// error
//	static constinit int	s_ConstInit5 = ConstInitFunc1( s_ConstInit3 == 0 );	// error
	//-----------------------------------------------------


	struct Test_ResultOf_Class
	{
		int		Get1 ();
		auto	Get2 ();
	};

	auto  Test_ResultOf_Class::Get2 () { return 1.f; }

	static void  Test_ResultOf ()
	{
		using A = std::invoke_result_t< decltype(&Test_ResultOf_Class::Get1), Test_ResultOf_Class >;
		using B = std::invoke_result_t< decltype(&Test_ResultOf_Class::Get2), Test_ResultOf_Class >;
		StaticAssert( IsSame< A, int >);
		StaticAssert( IsSame< B, float >);

	}
}
//-----------------------------------------------------

	struct StructBind
	{
		template <usize I>
		auto  get () C_NE___
		{
			if constexpr( I == 0 )	return int(1);			else
			if constexpr( I == 1 )	return String{"1234"};	else
			if constexpr( I == 2 )	return 2.0f;
		}
	};

	template <>
	struct std::tuple_size< StructBind >
	{
		static constexpr usize	value = 3;
	};

	template <usize I>
	struct std::tuple_element< I, StructBind >
	{
		using type = std::invoke_result_t< decltype(&StructBind::get<I>), StructBind >;
	};

namespace
{
	static void  Test_StructuredBinding ()
	{
		auto [a, b, c] = StructBind{};
		TEST( a == 1 );
		TEST( b == "1234" );
		TEST( c == 2.0f );
	}


	static void  Test_Lambda ()
	{
		struct EmptyClass1 {};
		struct EmptyClass2 { void operator () (int); };
		struct NotEmptyClass { char c; };

		StaticAssert( IsEmpty< EmptyClass1 >);
		StaticAssert( IsEmpty< EmptyClass2 >);
		StaticAssert( not IsEmpty< NotEmptyClass >);

		const auto	Lambda1 = [] (int) {};
		const auto	Lambda2 = [j = 0u] (int) { Unused(j); };
		
		StaticAssert( IsEmpty< decltype(Lambda1) >);
		StaticAssert( not IsEmpty< decltype(Lambda2) >);

		StaticAssert( IsClass< decltype(Lambda1) >);
		StaticAssert( IsClass< decltype(Lambda2) >);
	}


	template <typename T0> struct Tmpl1;
	template <typename T0, typename T1> struct Tmpl2;
	template <typename T0, typename T1, typename T2> struct Tmpl3;
	
	static void  Test_GetNumberOfTemplateArgs ()
	{
		StaticAssert( GetNumberOfTemplateArgs< Tmpl1 >() == 1 );
		StaticAssert( GetNumberOfTemplateArgs< Tmpl2 >() == 2 );
		StaticAssert( GetNumberOfTemplateArgs< Tmpl3 >() == 3 );
	}


	template <typename ExpectedType>
	struct Test_ReferenceCollapsing_St
	{
		template <typename T>
		static void  Test (T&& arg)
		{
			using R = decltype( FwdArg<T>(arg) );
			StaticAssert( IsSame< R, ExpectedType >);

			using E = ReferenceCollapsing<T&&>;
			StaticAssert( IsSame< R, E >);
		}

		template <typename ...Args>
		static void  Test2 (Args&& ...args)
		{
			using E = TypeList< ReferenceCollapsing<Args&&> ... >;
			StaticAssert( IsSame< E, ExpectedType >);

			Unused( args... );
		}
	};

	static void  Test_ReferenceCollapsing ()
	{
		int	value = 0;
		const int c_value = 0;
		int& ref = value;
		const int& c_ref = c_value;

		Test_ReferenceCollapsing_St< int& >::Test( value );
		Test_ReferenceCollapsing_St< const int& >::Test( c_value );
		Test_ReferenceCollapsing_St< int& >::Test( ref );
		Test_ReferenceCollapsing_St< const int& >::Test( c_ref );
		Test_ReferenceCollapsing_St< int&& >::Test( RVRef(value) );

		Test_ReferenceCollapsing_St< TypeList< int&, const int&, int&, const int&, int&& >>::Test2( value, c_value, ref, c_ref, RVRef(value) );
	}


	static void  Test_CheckTemplateArgs ()
	{
		{
			const auto	Lambda1 = [](auto, auto) {};

			StaticAssert( IsCallOperatorSpecializationWith< decltype(Lambda1), TypeList<int, int>, TypeList<float, float> >);
			StaticAssert( not IsCallOperatorSpecializationWith< decltype(Lambda1), TypeList<int>, TypeList<float, float> >);
			StaticAssert( not IsCallOperatorSpecializationWith< decltype(Lambda1), TypeList<int, int>, TypeList<float, float, float> >);
		}{
			const auto	Lambda1 = [](auto, auto) {};
			const auto	Lambda2 = [](const auto&, const auto&) {};
			const auto	Lambda3 = [](auto&, auto&) {};
			const auto	Lambda4 = [](auto&&, auto&&) {};
			const auto	Lambda5 = [](const auto, const auto) {};
			const auto	Lambda6 = [](auto, auto, auto) {};

			// IsTemplateArgConstRef
			StaticAssert(	not	IsTemplateArgConstRef< decltype(Lambda1), 2 >);
			StaticAssert(		IsTemplateArgConstRef< decltype(Lambda2), 2 >);
			StaticAssert(	not	IsTemplateArgConstRef< decltype(Lambda3), 2 >);
			StaticAssert(	not	IsTemplateArgConstRef< decltype(Lambda4), 2 >);
			StaticAssert(	not	IsTemplateArgConstRef< decltype(Lambda5), 2 >);
			StaticAssert(	not	IsTemplateArgConstRef< decltype(Lambda6), 1 >);
			
			// IsTemplateArgRef
			StaticAssert(	not	IsTemplateArgRef< decltype(Lambda1), 2 >);
			StaticAssert(	not	IsTemplateArgRef< decltype(Lambda2), 2 >);
			StaticAssert(		IsTemplateArgRef< decltype(Lambda3), 2 >);
			StaticAssert(	not	IsTemplateArgRef< decltype(Lambda4), 2 >);
			StaticAssert(	not	IsTemplateArgRef< decltype(Lambda5), 2 >);
			StaticAssert(	not	IsTemplateArgRef< decltype(Lambda6), 1 >);

			// IsTemplateArgRValueRef
			StaticAssert(	not	IsTemplateArgRValueRef< decltype(Lambda1), 2 >);
			StaticAssert(	not	IsTemplateArgRValueRef< decltype(Lambda2), 2 >);
			StaticAssert(	not	IsTemplateArgRValueRef< decltype(Lambda3), 2 >);
			StaticAssert(		IsTemplateArgRValueRef< decltype(Lambda4), 2 >);
			StaticAssert(	not	IsTemplateArgRValueRef< decltype(Lambda5), 2 >);
			StaticAssert(	not	IsTemplateArgRValueRef< decltype(Lambda6), 1 >);

			// IsTemplateArgWithoutRef
			StaticAssert(		IsTemplateArgWithoutRef< decltype(Lambda1), 2 >);
			StaticAssert(	not	IsTemplateArgWithoutRef< decltype(Lambda2), 2 >);
			StaticAssert(	not	IsTemplateArgWithoutRef< decltype(Lambda3), 2 >);
			StaticAssert(	not	IsTemplateArgWithoutRef< decltype(Lambda4), 2 >);
			StaticAssert(		IsTemplateArgWithoutRef< decltype(Lambda5), 2 >);
			StaticAssert(	not	IsTemplateArgWithoutRef< decltype(Lambda6), 1 >);
		}
	}
}
//-----------------------------------------------------


extern void UnitTest_TypeTraits ()
{
	Test_IsSpecializationOf();
	Test_IsCompleteType();
	Test_IsTrivial();
	Test_RemoveAllQualifiers();

	Test_Tuple();
	Test_TupleConcat();
	Test_TupleRef();

	Test_Nothrow();
	Test_ArrayView();
	Test_IsConst();
	Test_IsNothrowInvocable();
	Test_IsPointer();

	Test_Attributes();
	Test_Concepts1();
	Test_Concepts2();
	Test_IsConstEvaluated();
	Test_ResultOf();
	Test_StructuredBinding();
	Test_Lambda();
	Test_GetNumberOfTemplateArgs();
	Test_ReferenceCollapsing();
	Test_CheckTemplateArgs();

	Unused( s_ConstInit1, s_ConstInit3 );

	TEST_PASSED();
}
