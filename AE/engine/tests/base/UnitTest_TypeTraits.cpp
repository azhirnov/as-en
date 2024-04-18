// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include <span>
#include "UnitTest_Common.h"

namespace
{
	StaticAssert( sizeof(bool)	 == 1 );
	StaticAssert( sizeof(Bool32) == 4 );

	StaticAssert( sizeof(CharAnsi)	 == 1 );
	StaticAssert( sizeof(CharUtf8)	 == 1 );
	StaticAssert( sizeof(CharUtf16)	 == 2 );
	StaticAssert( sizeof(CharUtf32)	 == 4 );
	//StaticAssert( sizeof(wchar_t)	 == 2 );

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

	StaticAssert( not IsSameTypes< CharAnsi,  CharUtf8  >);
	StaticAssert( not IsSameTypes< CharAnsi,  CharUtf16 >);
	StaticAssert( not IsSameTypes< CharAnsi,  CharUtf32 >);
	StaticAssert( not IsSameTypes< CharAnsi,  wchar_t   >);

	StaticAssert( not IsSameTypes< CharUtf8,  CharUtf16 >);
	StaticAssert( not IsSameTypes< CharUtf8,  CharUtf32 >);
	StaticAssert( not IsSameTypes< CharUtf8,  wchar_t   >);

	StaticAssert( not IsSameTypes< CharUtf16, CharUtf32 >);
	StaticAssert( not IsSameTypes< CharUtf16, wchar_t   >);

	StaticAssert( not IsSameTypes< CharUtf32, wchar_t   >);

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
		StaticAssert(( IsSameTypes< int, T1 >));
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
			StaticAssert( IsSameTypes< decltype(a2), ArrayView<int> >);
		}{
			std::vector<int>	b1 {0,1,2};
			auto				b2 = ArrayView{b1.data(), b1.size()};
			StaticAssert( IsSameTypes< decltype(b2), ArrayView<int> >);
		}/*{
			int		c1[]	= {0,1,2};
			auto	c2		= ArrayView{c1};
			StaticAssert( IsSameTypes< decltype(c2), ArrayView<int> >);
		}*/{
			auto	d2		= ArrayView{ 0, 1, 2 };
			StaticAssert( IsSameTypes< decltype(d2), ArrayView<int> >);
		}/*{
			StaticArray< int, 5 >	e1 {0,1,2,3};
			auto					e2 = ArrayView{e1};
			StaticAssert( IsSameTypes< decltype(e2), ArrayView<int> >);
		}*/
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
}


extern void UnitTest_TypeTraits ()
{
	Test_IsSpecializationOf();
	Test_IsTrivial();
	Test_RemoveAllQualifiers();
	Test_TupleConcat();
	Test_TupleRef();
	Test_Nothrow();
	Test_ArrayView();
	Test_IsConst();
	Test_IsNothrowInvocable();

	TEST_PASSED();
}
