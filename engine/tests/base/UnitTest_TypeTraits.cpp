// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include <span>
#include "UnitTest_Common.h"
#include "base/Math/Vec.h"
#include "base/Math/Matrix.h"

namespace
{
	static void  Test_IsSpecializationOf ()
	{
		using T1 = ArrayView<int>;
		STATIC_ASSERT( IsSpecializationOf< T1, ArrayView > );
		STATIC_ASSERT( not IsSpecializationOf< T1, std::tuple > );

		using T2 = ArrayView<int> *;
		STATIC_ASSERT( not IsSpecializationOf< T2, ArrayView > );
		STATIC_ASSERT( not IsSpecializationOf< T2, std::tuple > );

		using T3 = std::tuple< int >;
		STATIC_ASSERT( IsSpecializationOf< T3, std::tuple > );
		STATIC_ASSERT( not IsSpecializationOf< T3, ArrayView > );
	}


	static void  Test_IsTrivial ()
	{
		STATIC_ASSERT( IsPOD<int> );
		
		STATIC_ASSERT( IsZeroMemAvailable<int> );
		STATIC_ASSERT( IsZeroMemAvailable<int2> );
		STATIC_ASSERT( IsZeroMemAvailable<float3> );
		STATIC_ASSERT( IsZeroMemAvailable<float4x4> );
		STATIC_ASSERT( IsZeroMemAvailable<float[4]> );
		STATIC_ASSERT( IsZeroMemAvailable<const int[8]> );
		
		STATIC_ASSERT( IsMemCopyAvailable<int> );
		STATIC_ASSERT( IsMemCopyAvailable<int2> );
		STATIC_ASSERT( IsMemCopyAvailable<float3> );
		STATIC_ASSERT( IsMemCopyAvailable<float4x4> );
		STATIC_ASSERT( IsMemCopyAvailable<float[4]> );
		STATIC_ASSERT( IsMemCopyAvailable<const int[8]> );
		
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

		STATIC_ASSERT( IsPOD<PODType> );
		STATIC_ASSERT( not IsPOD<TrivialType> );
		
		STATIC_ASSERT( IsTrivial<PODType> );
		STATIC_ASSERT( IsTrivial<TrivialType> );
	}


	static void  Test_RemoveAllQualifiers ()
	{
		using T1 = RemoveAllQualifiers< const int* const* const& >;
		STATIC_ASSERT(( IsSameTypes< int, T1 >));
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


	static void  Test_Nothrow ()
	{
		struct TrivialThrowable
		{
			int	i;

			TrivialThrowable () __Th___ = default;
			explicit TrivialThrowable (int ii) __Th___ : i{ii} {}
		};

		struct NontrivialNothrowable
		{
			String	str;

			NontrivialNothrowable () __NE___ { str = "11111"; }
			explicit NontrivialNothrowable (const String &s) __NE___ : str{s} {}

			~NontrivialNothrowable () __NE___ {}
		};
		
		STATIC_ASSERT( IsNothrowCtor< TrivialThrowable, int >);
		STATIC_ASSERT( IsNothrowCtor< NontrivialNothrowable, String >);
	}


	static void  Test_ArrayView ()
	{
		std::span<int>	s;
		{
			std::vector<int>	a1 {0,1,2};
			auto				a2 = ArrayView{a1};
			STATIC_ASSERT( IsSameTypes< decltype(a2), ArrayView<int> >);
		}{
			std::vector<int>	b1 {0,1,2};
			auto				b2 = ArrayView{b1.data(), b1.size()};
			STATIC_ASSERT( IsSameTypes< decltype(b2), ArrayView<int> >);
		}/*{
			int		c1[]	= {0,1,2};
			auto	c2		= ArrayView{c1};
			STATIC_ASSERT( IsSameTypes< decltype(c2), ArrayView<int> >);
		}*/{
			auto	d2		= ArrayView{ 0, 1, 2 };
			STATIC_ASSERT( IsSameTypes< decltype(d2), ArrayView<int> >);
		}
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

	TEST_PASSED();
}
