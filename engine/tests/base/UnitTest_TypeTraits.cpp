// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"
#include "base/Math/Vec.h"
#include "base/Math/Matrix.h"


extern void UnitTest_TypeTraits ()
{
	// IsSpecializationOf
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

	// IsPOD / IsTrivial
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

	// RemoveAllQualifiers
	{
		using T1 = RemoveAllQualifiers< const int* const* const& >;
		STATIC_ASSERT(( IsSameTypes< int, T1 >));
	}

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

	// nothrow
	{
		struct TrivialThrowable
		{
			int	i;

			TrivialThrowable () noexcept(false) = default;
			explicit TrivialThrowable (int ii) noexcept(false) : i{ii} {}
		};

		struct NontrivialNothrowable
		{
			String	str;

			NontrivialNothrowable () noexcept { str = "11111"; }
			explicit NontrivialNothrowable (const String &s) noexcept : str{s} {}

			~NontrivialNothrowable () noexcept {}
		};
		
		STATIC_ASSERT( IsNothrowCtor< TrivialThrowable, int >);
		STATIC_ASSERT( IsNothrowCtor< NontrivialNothrowable, String >);
	}

	TEST_PASSED();
}
