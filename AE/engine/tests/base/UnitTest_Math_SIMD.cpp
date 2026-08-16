// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	template <typename T>
	using ScalarType = 	Conditional< IsFloatPoint<T>, T,
							Conditional< IsInteger<T>, T,
								Conditional< (IsAnyFloatPoint<T> and sizeof(T) < sizeof(float)), float, void >>>;

	#define AB(x)	A{B(x)}		// half{float( x )}

	template <uint X, uint Y, typename Fn, typename Simd>
	void  TestAllSwizzles2 (const Fn &checkSwizzle, const Simd &v)
	{
		checkSwizzle( v.template Swizzle<X,Y>(), X, Y );

		constexpr uint Max = 1;

		if constexpr( Y < Max )
			TestAllSwizzles2< X, Y + 1 >( checkSwizzle, v );
		else
		if constexpr( X < Max )
			TestAllSwizzles2< X + 1, 0 >( checkSwizzle, v );
	}

	template <uint X, uint Y, uint Z, uint W,
			  typename Fn, typename Simd>
	void  TestAllSwizzles4 (const Fn &checkSwizzle, const Simd &v)
	{
		checkSwizzle( v.template Swizzle<X,Y,Z,W>(), X, Y, Z, W );

		constexpr uint Max = 3;

		if constexpr( W < Max )
			TestAllSwizzles4< X, Y, Z, W + 1 >( checkSwizzle, v );
		else
		if constexpr( Z < Max )
			TestAllSwizzles4< X, Y, Z + 1, 0 >( checkSwizzle, v );
		else
		if constexpr( Y < Max )
			TestAllSwizzles4< X, Y + 1, 0, 0 >( checkSwizzle, v );
		else
		if constexpr( X < Max )
			TestAllSwizzles4< X + 1, 0, 0, 0 >( checkSwizzle, v );
	}
//-----------------------------------------------------------------------------


	template <typename Simd>
	static void  Test_Vec2 ()
	{
		using A = typename Simd::Scalar_t;
		using B = ScalarType<A>;
		using Arr_t = typename Simd::Array_t;

		// initialization
		{
			Arr_t	arr_zero;
			Arr_t	arr_one;
			Arr_t	arr_neg;
			Arr_t	arr_1234;

			for (usize i = 0; i < arr_zero.size(); ++i)
			{
				arr_zero[i] = AB(0);
				arr_one [i] = AB(1);
				arr_neg [i] = AB(-1);
				arr_1234[i] = AB(i+1);
			}

			const Simd	a1;	// zero
			const Simd	a2	{AB(1)};
			const Simd	a3	= Zero;
			const Simd	a4	{ AB(1), AB(2) };
			const A		aa [] = { AB(1), AB(2) };
			const Simd	a5	{ aa };
			const Simd	a6	{AB(-1)};

			TEST( a1.ToArray() == arr_zero );
			TEST( a2.ToArray() == arr_one  );
			TEST( a3.ToArray() == arr_zero );
			TEST( a4.ToArray() == arr_1234 );
			TEST( a5.ToArray() == arr_1234 );
			TEST( a6.ToArray() == arr_neg  );
		}

		if constexpr( IsInteger<B> )
		{
			const Simd	m0{ MSBMask<2,0>{} };
			const Simd	m1{ MSBMask<2,1>{} };
			const Simd	m2{ MSBMask<2,2>{} };
			const Simd	m3{ MSBMask<2,3>{} };
		}

		// get
		{
			const Simd	a1	{ AB(1), AB(2) };
			const Simd	a2	{ AB(-1), AB(-2) };

			TEST_Eq( B(a1.template get<0>()),	B(1) );
			TEST_Eq( B(a1.template get<1>()),	B(2) );

			TEST_Eq( B(a2.template get<0>()),	B(-1) );
			TEST_Eq( B(a2.template get<1>()),	B(-2) );
		}

		// set
		{
			const Simd	a { AB(1), AB(2) };
			const Simd	a1 = a.template set<0>( AB(10) );
			const Simd	a2 = a.template set<1>( AB(10) );

			TEST( a1.ToArray() == Arr_t{ AB(10), AB(2)  });
			TEST( a2.ToArray() == Arr_t{ AB(1),  AB(10) });
		}

		// swizzle
		if constexpr( Simd::Has_Swizzle() )
		{
			Arr_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckSwizzle = [&arr](Simd v, uint x, uint y)
			{{
				auto	a = v.ToArray();
				Arr_t	b = {arr[x], arr[y]};
				TEST( a == b );
			}};

			Simd	v {arr.data()};

			TestAllSwizzles2< 0, 0 >( CheckSwizzle, v );
		}

		// shuffle
		if constexpr( Simd::Has_Shuffle() )
		{
			StaticArray<A,4>	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckShuffle = [&arr](Simd v, uint v0, uint v1)
			{{
				auto	a = v.ToArray();
				Arr_t	b = {arr[v0], arr[v1]};
				TEST( a == b );
			}};

			Simd	a {arr.data()};
			Simd	b {arr.data() + 2};

			#define CHECK_SHUFFLE( v0,v1 )	CheckShuffle( a.template Shuffle<v0,v1>( b ), v0,v1 );
			CHECK_SHUFFLE( 2,2 );
			CHECK_SHUFFLE( 2,3 );
			CHECK_SHUFFLE( 2,0 );
			CHECK_SHUFFLE( 0,2 );
			CHECK_SHUFFLE( 3,1 );
			#undef CHECK_SHUFFLE
		}
	}


	template <typename Simd>
	static void  Test_Vec4 ()
	{
		using A = typename Simd::Scalar_t;
		using B = ScalarType<A>;
		using Arr_t = typename Simd::Array_t;

		// initialization
		{
			Arr_t	arr_zero;
			Arr_t	arr_one;
			Arr_t	arr_neg;
			Arr_t	arr_1234;

			for (usize i = 0; i < arr_zero.size(); ++i)
			{
				arr_zero[i] = AB(0);
				arr_one [i] = AB(1);
				arr_neg [i] = AB(-1);
				arr_1234[i] = AB(i+1);
			}

			const Simd	a1;	// zero
			const Simd	a2	{AB(1)};
			const Simd	a3	= Zero;
			const Simd	a4	{ AB(1), AB(2), AB(3), AB(4) };
			const A		aa [] = { AB(1), AB(2), AB(3), AB(4) };
			const Simd	a5	{ aa };
			const Simd	a6	{AB(-1)};

			TEST( a1.ToArray() == arr_zero );
			TEST( a2.ToArray() == arr_one  );
			TEST( a3.ToArray() == arr_zero );
			TEST( a4.ToArray() == arr_1234 );
			TEST( a5.ToArray() == arr_1234 );
			TEST( a6.ToArray() == arr_neg  );
		}

		if constexpr( IsInteger<B> )
		{
			const Simd	m0{ MSBMask<4,0>{} };
			const Simd	m1{ MSBMask<4,1>{} };
			const Simd	m2{ MSBMask<4,2>{} };
			const Simd	m3{ MSBMask<4,3>{} };
		}

		// get
		{
			const Simd	a1	{ AB(1),  AB(2),  AB(3),  AB(4) };
			const Simd	a2	{ AB(-1), AB(-2), AB(-3), AB(-4) };

			TEST_Eq( B(a1.template get<0>()),	B(1) );
			TEST_Eq( B(a1.template get<1>()),	B(2) );
			TEST_Eq( B(a1.template get<2>()),	B(3) );
			TEST_Eq( B(a1.template get<3>()),	B(4) );

			TEST_Eq( B(a2.template get<0>()),	B(-1) );
			TEST_Eq( B(a2.template get<1>()),	B(-2) );
			TEST_Eq( B(a2.template get<2>()),	B(-3) );
			TEST_Eq( B(a2.template get<3>()),	B(-4) );
		}

		// set
		{
			const Simd	a { AB(1), AB(2), AB(3), AB(4) };
			const Simd	a1 = a.template set<0>( AB(10) );
			const Simd	a2 = a.template set<1>( AB(10) );
			const Simd	a3 = a.template set<2>( AB(10) );
			const Simd	a4 = a.template set<3>( AB(10) );

			TEST( a1.ToArray() == Arr_t{ AB(10), AB(2),  AB(3),  AB(4)  });
			TEST( a2.ToArray() == Arr_t{ AB(1),  AB(10), AB(3),  AB(4)  });
			TEST( a3.ToArray() == Arr_t{ AB(1),  AB(2),  AB(10), AB(4)  });
			TEST( a4.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(10) });
		}

		// swizzle
		if constexpr( Simd::Has_Swizzle() )
		{
			Arr_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckSwizzle = [&arr](Simd v, uint x, uint y, uint z, uint w)
			{{
				auto	a = v.ToArray();
				Arr_t	b = {arr[x], arr[y], arr[z], arr[w]};
				TEST( a == b );
			}};

			Simd	v {arr.data()};

			TestAllSwizzles4< 0, 0, 0, 0 >( CheckSwizzle, v );
		}

		// shuffle
		if constexpr( Simd::Has_Shuffle() )
		{
			StaticArray<A,8>	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckShuffle = [&arr](Simd v, uint v0, uint v1, uint v2, uint v3)
			{{
				auto	a = v.ToArray();
				Arr_t	b = {arr[v0], arr[v1], arr[v2], arr[v3]};
				TEST( a == b );
			}};

			Simd	a {arr.data()};
			Simd	b {arr.data() + 4};

			#define CHECK_SHUFFLE( v0,v1,v2,v3 )	CheckShuffle( a.template Shuffle<v0,v1,v2,v3>( b ), v0,v1,v2,v3 );
			CHECK_SHUFFLE( 4,4,4,4 );
			CHECK_SHUFFLE( 4,5,6,7 );
			CHECK_SHUFFLE( 3,2,1,0 );
			CHECK_SHUFFLE( 1,4,0,2 );
			CHECK_SHUFFLE( 3,0,1,4 );
			#undef CHECK_SHUFFLE
		}
	}


	template <typename Simd>
	static void  Test_Vec8 ()
	{
		using A = typename Simd::Scalar_t;
		using B = ScalarType<A>;
		using Arr_t = typename Simd::Array_t;

		// initialization
		{
			Arr_t	arr_zero;
			Arr_t	arr_one;
			Arr_t	arr_neg;
			Arr_t	arr_1234;

			for (usize i = 0; i < arr_zero.size(); ++i)
			{
				arr_zero[i] = AB(0);
				arr_one [i] = AB(1);
				arr_neg [i] = AB(-1);
				arr_1234[i] = AB(i+1);
			}

			const Simd	a1;	// zero
			const Simd	a2	{AB(1)};
			const Simd	a3	= Zero;
			const Simd	a4	{ AB(1), AB(2), AB(3), AB(4), AB(5), AB(6), AB(7), AB(8) };
			const A		aa [] = { AB(1), AB(2), AB(3), AB(4), AB(5), AB(6), AB(7), AB(8) };
			const Simd	a5	{ aa };
			const Simd	a6	{AB(-1)};

			TEST( a1.ToArray() == arr_zero );
			TEST( a2.ToArray() == arr_one  );
			TEST( a3.ToArray() == arr_zero );
			TEST( a4.ToArray() == arr_1234 );
			TEST( a5.ToArray() == arr_1234 );
			TEST( a6.ToArray() == arr_neg  );
		}

		if constexpr( IsInteger<B> )
		{
			const Simd	m0{ MSBMask<8,0>{} };
			const Simd	m1{ MSBMask<8,1>{} };
			const Simd	m2{ MSBMask<8,2>{} };
			const Simd	m3{ MSBMask<8,3>{} };
		}

		// get
		{
			const Simd	a1	{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8) };
			const Simd	a2	{ AB(-1), AB(-2), AB(-3), AB(-4), AB(-5), AB(-6), AB(-7), AB(-8) };

			TEST_Eq( B(a1.template get<0>()),	B(1) );
			TEST_Eq( B(a1.template get<1>()),	B(2) );
			TEST_Eq( B(a1.template get<2>()),	B(3) );
			TEST_Eq( B(a1.template get<3>()),	B(4) );
			TEST_Eq( B(a1.template get<4>()),	B(5) );
			TEST_Eq( B(a1.template get<5>()),	B(6) );
			TEST_Eq( B(a1.template get<6>()),	B(7) );
			TEST_Eq( B(a1.template get<7>()),	B(8) );

			TEST_Eq( B(a2.template get<0>()),	B(-1) );
			TEST_Eq( B(a2.template get<1>()),	B(-2) );
			TEST_Eq( B(a2.template get<2>()),	B(-3) );
			TEST_Eq( B(a2.template get<3>()),	B(-4) );
			TEST_Eq( B(a2.template get<4>()),	B(-5) );
			TEST_Eq( B(a2.template get<5>()),	B(-6) );
			TEST_Eq( B(a2.template get<6>()),	B(-7) );
			TEST_Eq( B(a2.template get<7>()),	B(-8) );
		}

		// set
		{
			const Simd	a { AB(1), AB(2), AB(3), AB(4), AB(5), AB(6), AB(7), AB(8) };
			const Simd	a1 = a.template set<0>( AB(10) );
			const Simd	a2 = a.template set<1>( AB(10) );
			const Simd	a3 = a.template set<2>( AB(10) );
			const Simd	a4 = a.template set<3>( AB(10) );
			const Simd	a5 = a.template set<4>( AB(10) );
			const Simd	a6 = a.template set<5>( AB(10) );
			const Simd	a7 = a.template set<6>( AB(10) );
			const Simd	a8 = a.template set<7>( AB(10) );

			TEST( a1.ToArray() == Arr_t{ AB(10), AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8)  });
			TEST( a2.ToArray() == Arr_t{ AB(1),  AB(10), AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8)  });
			TEST( a3.ToArray() == Arr_t{ AB(1),  AB(2),  AB(10), AB(4),  AB(5),  AB(6),  AB(7),  AB(8)  });
			TEST( a4.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(10), AB(5),  AB(6),  AB(7),  AB(8)  });
			TEST( a5.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(10), AB(6),  AB(7),  AB(8)  });
			TEST( a6.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(10), AB(7),  AB(8)  });
			TEST( a7.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(10), AB(8)  });
			TEST( a8.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(10) });
		}

		// swizzle
		if constexpr( Simd::Has_Swizzle() )
		{
			Arr_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckSwizzle = [&arr](Simd v, uint v0, uint v1, uint v2, uint v3, uint v4, uint v5, uint v6, uint v7)
			{{
				auto	a = v.ToArray();
				Arr_t	b = {arr[v0], arr[v1], arr[v2], arr[v3], arr[v4], arr[v5], arr[v6], arr[v7]};
				TEST( a == b );
			}};

			Simd	v {arr.data()};

			#define CHECK_SWIZZLE( v0,v1,v2,v3,v4,v5,v6,v7 )	CheckSwizzle( v.template Swizzle<v0,v1,v2,v3,v4,v5,v6,v7>(), v0,v1,v2,v3,v4,v5,v6,v7 );
			CHECK_SWIZZLE( 0,0,0,0, 0,0,0,0 );
			CHECK_SWIZZLE( 4,5,6,7, 0,1,2,3 );
			CHECK_SWIZZLE( 4,4,5,5, 6,6,7,7 );

			CHECK_SWIZZLE( 7,3,0,5, 4,7,1,1 );
			CHECK_SWIZZLE( 2,3,7,6, 1,0,5,4 );
			#undef CHECK_SWIZZLE
		}

		// shuffle
		if constexpr( Simd::Has_Shuffle() )
		{
			StaticArray<A,16>	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckShuffle = [&arr](Simd v, uint v0, uint v1, uint v2, uint v3, uint v4, uint v5, uint v6, uint v7)
			{{
				auto	a = v.ToArray();
				Arr_t	b = {arr[v0], arr[v1], arr[v2], arr[v3], arr[v4], arr[v5], arr[v6], arr[v7]};
				TEST( a == b );
			}};

			Simd	a {arr.data()};
			Simd	b {arr.data() + 8};

			#define CHECK_SHUFFLE( v0,v1,v2,v3,v4,v5,v6,v7 )	CheckShuffle( a.template Shuffle<v0,v1,v2,v3,v4,v5,v6,v7>( b ), v0,v1,v2,v3,v4,v5,v6,v7 );

			// identity / pass-through
			CHECK_SHUFFLE( 0,1,2,3,		4,5,6,7 );			// 'a' unchanged
			CHECK_SHUFFLE( 8,9,10,11,	12,13,14,15 );		// 'b' unchanged

			// broadcast (splat)
			CHECK_SHUFFLE( 0,0,0,0,		0,0,0,0 );
			CHECK_SHUFFLE( 7,7,7,7,		7,7,7,7 );
			CHECK_SHUFFLE( 8,8,8,8,		8,8,8,8 );
			CHECK_SHUFFLE( 15,15,15,15,	15,15,15,15 );
			CHECK_SHUFFLE( 8,8,8,8,		0,0,0,0 );			// splat b[0] | a[0]
			CHECK_SHUFFLE( 3,3,3,3,		11,11,11,11 );		// splat a[3] | b[3]

			// reverse
			CHECK_SHUFFLE( 7,6,5,4,		3,2,1,0 );			// reverse 'a'
			CHECK_SHUFFLE( 15,14,13,12,	11,10,9,8 );		// reverse 'b'
			CHECK_SHUFFLE( 15,14,13,12,	3,2,1,0 );			// high 'b' reversed | low 'a' reversed

			// half selection / lane swap
			CHECK_SHUFFLE( 8,9,10,11,	0,1,2,3 );			// low(b) | low(a)
			CHECK_SHUFFLE( 12,13,14,15,	4,5,6,7 );			// high(b) | high(a)
			CHECK_SHUFFLE( 4,5,6,7,		0,1,2,3 );			// swap 128-bit lanes of 'a'
			CHECK_SHUFFLE( 12,13,14,15,	8,9,10,11 );		// swap 128-bit lanes of 'b'

			// interleave (unpack lo/hi style)
			CHECK_SHUFFLE( 0,8,1,9,		2,10,3,11 );		// interleave low halves
			CHECK_SHUFFLE( 4,12,5,13,	6,14,7,15 );		// interleave high halves

			// deinterleave
			CHECK_SHUFFLE( 0,2,4,6,		8,10,12,14 );		// even elements
			CHECK_SHUFFLE( 1,3,5,7,		9,11,13,15 );		// odd elements

			// alternating lanes (blend style)
			CHECK_SHUFFLE( 0,9,2,11,	4,13,6,15 );		// even lanes from 'a', odd from 'b'
			CHECK_SHUFFLE( 8,1,10,3,	12,5,14,7 );		// even lanes from 'b', odd from 'a'

			// rotate
			CHECK_SHUFFLE( 1,2,3,4,		5,6,7,0 );			// rotate 'a' left
			CHECK_SHUFFLE( 7,0,1,2,		3,4,5,6 );			// rotate 'a' right
			CHECK_SHUFFLE( 9,10,11,12,	13,14,15,8 );		// rotate 'b' left
			CHECK_SHUFFLE( 15,8,9,10,	11,12,13,14 );		// rotate 'b' right

			// duplicate pairs
			CHECK_SHUFFLE( 0,0,1,1,		2,2,3,3 );
			CHECK_SHUFFLE( 4,4,5,5,		6,6,7,7 );
			CHECK_SHUFFLE( 8,8,9,9,		10,10,11,11 );
			CHECK_SHUFFLE( 12,12,13,13,	14,14,15,15 );

			// boundary indices
			CHECK_SHUFFLE( 7,8,7,8,		7,8,7,8 );			// a[7]/b[0] boundary
			CHECK_SHUFFLE( 0,15,0,15,	0,15,0,15 );		// min/max indices

			// pseudo-random mixes
			CHECK_SHUFFLE( 12,4,9,13,	3,11,7,15 );
			CHECK_SHUFFLE( 5,14,0,9,	7,10,2,13 );
			CHECK_SHUFFLE( 15,0,14,1,	13,2,12,3 );
			CHECK_SHUFFLE( 6,2,15,8,	10,5,1,12 );

			#undef CHECK_SHUFFLE
		}
	}


	template <typename Simd>
	static void  Test_Vec16 ()
	{
		using A = typename Simd::Scalar_t;
		using B = ScalarType<A>;
		using Arr_t = typename Simd::Array_t;

		// initialization
		{
			Arr_t	arr_zero;
			Arr_t	arr_one;
			Arr_t	arr_neg;
			Arr_t	arr_1234;

			for (usize i = 0; i < arr_zero.size(); ++i)
			{
				arr_zero[i] = AB(0);
				arr_one [i] = AB(1);
				arr_neg [i] = AB(-1);
				arr_1234[i] = AB(i+1);
			}

			const Simd	a1;	// zero
			const Simd	a2	{AB(1)};
			const Simd	a3	= Zero;
			const Simd	a4	{ AB(1), AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),
							  AB(9), AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) };
			const A		aa [] = { AB(1), AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),
								  AB(9), AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) };
			const Simd	a5	{ aa };
			const Simd	a6	{AB(-1)};

			TEST( a1.ToArray() == arr_zero );
			TEST( a2.ToArray() == arr_one  );
			TEST( a3.ToArray() == arr_zero );
			TEST( a4.ToArray() == arr_1234 );
			TEST( a5.ToArray() == arr_1234 );
			TEST( a6.ToArray() == arr_neg  );
		}

		if constexpr( IsInteger<B> )
		{
			const Simd	m0{ MSBMask<16,0>{} };
			const Simd	m1{ MSBMask<16,1>{} };
			const Simd	m2{ MSBMask<16,2>{} };
		}

		// get
		{
			const Simd	a1	{ AB(1), AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),
							  AB(9), AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) };
			const Simd	a2	{ AB(-1), AB(-2),  AB(-3),  AB(-4),  AB(-5),  AB(-6),  AB(-7),  AB(-8),
							  AB(-9), AB(-10), AB(-11), AB(-12), AB(-13), AB(-14), AB(-15), AB(-16) };

			TEST_Eq( a1.template get<0>(),	B(1) );
			TEST_Eq( a1.template get<1>(),	B(2) );
			TEST_Eq( a1.template get<2>(),	B(3) );
			TEST_Eq( a1.template get<3>(),	B(4) );
			TEST_Eq( a1.template get<4>(),	B(5) );
			TEST_Eq( a1.template get<5>(),	B(6) );
			TEST_Eq( a1.template get<6>(),	B(7) );
			TEST_Eq( a1.template get<7>(),	B(8) );
			TEST_Eq( a1.template get<8>(),	B(9) );
			TEST_Eq( a1.template get<9>(),	B(10) );
			TEST_Eq( a1.template get<10>(),	B(11) );
			TEST_Eq( a1.template get<11>(),	B(12) );
			TEST_Eq( a1.template get<12>(),	B(13) );
			TEST_Eq( a1.template get<13>(),	B(14) );
			TEST_Eq( a1.template get<14>(),	B(15) );
			TEST_Eq( a1.template get<15>(),	B(16) );

			TEST_Eq( a2.template get<0>(),	B(-1) );
			TEST_Eq( a2.template get<1>(),	B(-2) );
			TEST_Eq( a2.template get<2>(),	B(-3) );
			TEST_Eq( a2.template get<3>(),	B(-4) );
			TEST_Eq( a2.template get<4>(),	B(-5) );
			TEST_Eq( a2.template get<5>(),	B(-6) );
			TEST_Eq( a2.template get<6>(),	B(-7) );
			TEST_Eq( a2.template get<7>(),	B(-8) );
			TEST_Eq( a2.template get<8>(),	B(-9) );
			TEST_Eq( a2.template get<9>(),	B(-10) );
			TEST_Eq( a2.template get<10>(),	B(-11) );
			TEST_Eq( a2.template get<11>(),	B(-12) );
			TEST_Eq( a2.template get<12>(),	B(-13) );
			TEST_Eq( a2.template get<13>(),	B(-14) );
			TEST_Eq( a2.template get<14>(),	B(-15) );
			TEST_Eq( a2.template get<15>(),	B(-16) );
		}

		// set
		{
			const Simd	a { AB(1), AB(2), AB(3), AB(4), AB(5), AB(6), AB(7), AB(8), AB(9), AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) };
			const Simd	a1  = a.template set< 0>( AB(99) );
			const Simd	a2  = a.template set< 1>( AB(99) );
			const Simd	a3  = a.template set< 2>( AB(99) );
			const Simd	a4  = a.template set< 3>( AB(99) );
			const Simd	a5  = a.template set< 4>( AB(99) );
			const Simd	a6  = a.template set< 5>( AB(99) );
			const Simd	a7  = a.template set< 6>( AB(99) );
			const Simd	a8  = a.template set< 7>( AB(99) );
			const Simd	a9  = a.template set< 8>( AB(99) );
			const Simd	a10 = a.template set< 9>( AB(99) );
			const Simd	a11 = a.template set<10>( AB(99) );
			const Simd	a12 = a.template set<11>( AB(99) );
			const Simd	a13 = a.template set<12>( AB(99) );
			const Simd	a14 = a.template set<13>( AB(99) );
			const Simd	a15 = a.template set<14>( AB(99) );
			const Simd	a16 = a.template set<15>( AB(99) );

			TEST( a1 .ToArray() == Arr_t{ AB(99), AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a2 .ToArray() == Arr_t{ AB(1),  AB(99), AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a3 .ToArray() == Arr_t{ AB(1),  AB(2),  AB(99), AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a4 .ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(99), AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a5 .ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(99), AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a6 .ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(99), AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a7 .ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(99), AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a8 .ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(99), AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a9 .ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(99), AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a10.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(99), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a11.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(99), AB(12), AB(13), AB(14), AB(15), AB(16) });
			TEST( a12.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(99), AB(13), AB(14), AB(15), AB(16) });
			TEST( a13.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(99), AB(14), AB(15), AB(16) });
			TEST( a14.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(99), AB(15), AB(16) });
			TEST( a15.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(99), AB(16) });
			TEST( a16.ToArray() == Arr_t{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(99) });
		}

		// swizzle
		if constexpr( Simd::Has_Swizzle() )
		{
			Arr_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckSwizzle = [&arr](Simd v, const List<uint> &idx)
			{{
				TEST_Eq( idx.size(), 16 );
				auto	a = v.ToArray();
				Arr_t	b;
				usize	i = 0;
				for (uint x : idx) b[i++] = arr[x];
				TEST( a == b );
			}};

			Simd	v {arr.data()};

			#define CHECK_SWIZZLE( ... )	CheckSwizzle( v.template Swizzle< __VA_ARGS__ >(), { __VA_ARGS__ });

			CHECK_SWIZZLE( 0,1,2,3, 4,5,6,7, 4,4,4,4, 0,1,0,1 );
			CHECK_SWIZZLE( 15,14,13,12, 11,10,9,8, 7,6,5,4, 3,2,1,0 );

			// identity
			CHECK_SWIZZLE( 0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15 );

			// broadcast single element to all lanes
			CHECK_SWIZZLE( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 );
			CHECK_SWIZZLE( 15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15 );
			CHECK_SWIZZLE( 7,7,7,7, 7,7,7,7, 7,7,7,7, 7,7,7,7 );

			// broadcast 128-bit lane
			CHECK_SWIZZLE( 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3 );
			CHECK_SWIZZLE( 12,13,14,15, 12,13,14,15, 12,13,14,15, 12,13,14,15 );

			// duplicate low / high half
			CHECK_SWIZZLE( 0,1,2,3, 4,5,6,7, 0,1,2,3, 4,5,6,7 );
			CHECK_SWIZZLE( 8,9,10,11, 12,13,14,15, 8,9,10,11, 12,13,14,15 );

			// swap halves
			CHECK_SWIZZLE( 8,9,10,11, 12,13,14,15, 0,1,2,3, 4,5,6,7 );

			// rotate left / right by 1
			CHECK_SWIZZLE( 1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,0 );
			CHECK_SWIZZLE( 15,0,1,2, 3,4,5,6, 7,8,9,10, 11,12,13,14 );

			// rotate by one 128-bit lane
			CHECK_SWIZZLE( 4,5,6,7, 8,9,10,11, 12,13,14,15, 0,1,2,3 );

			// swap adjacent pairs
			CHECK_SWIZZLE( 1,0,3,2, 5,4,7,6, 9,8,11,10, 13,12,15,14 );

			// reverse within each 128-bit lane
			CHECK_SWIZZLE( 3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12 );

			// interleave (zip) low and high halves
			CHECK_SWIZZLE( 0,8,1,9, 2,10,3,11, 4,12,5,13, 6,14,7,15 );

			// deinterleave (unzip) evens / odds
			CHECK_SWIZZLE( 0,2,4,6, 8,10,12,14, 1,3,5,7, 9,11,13,15 );

			// 4x4 matrix transpose
			CHECK_SWIZZLE( 0,4,8,12, 1,5,9,13, 2,6,10,14, 3,7,11,15 );

			// duplicate each element
			CHECK_SWIZZLE( 0,0,1,1, 2,2,3,3, 4,4,5,5, 6,6,7,7 );
			CHECK_SWIZZLE( 0,0,2,2, 4,4,6,6, 8,8,10,10, 12,12,14,14 );

			// alternating extremes
			CHECK_SWIZZLE( 0,15,1,14, 2,13,3,12, 4,11,5,10, 6,9,7,8 );

			// repeated cross-lane pattern
			CHECK_SWIZZLE( 3,11,3,11, 5,13,5,13, 0,8,0,8, 15,7,15,7 );

			// pseudo-random permutation (each index used once)
			CHECK_SWIZZLE( 5,2,9,0, 14,7,3,11, 6,13,1,8, 12,4,15,10 );
			#undef CHECK_SWIZZLE
		}

		// shuffle
		if constexpr( Simd::Has_Shuffle() )
		{
			StaticArray<A,32>	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckShuffle = [&arr](Simd v, const StaticArray<uint,16> &idx)
			{{
				auto	a = v.ToArray();
				Arr_t	b {};
				for (usize i = 0; i < a.size(); ++i)
					b[i] = arr[idx[i]];
				TEST( a == b );
			}};

			Simd	a {arr.data()};
			Simd	b {arr.data() + 16};

			#define CHECK_SHUFFLE( ... )	CheckShuffle( a.template Shuffle<__VA_ARGS__>( b ), StaticArray<uint,16>{ __VA_ARGS__ });

			// identity / pass-through
			CHECK_SHUFFLE( 0,1,2,3,			4,5,6,7,		8,9,10,11,		12,13,14,15 );		// 'a' unchanged
			CHECK_SHUFFLE( 16,17,18,19,		20,21,22,23,	24,25,26,27,	28,29,30,31 );		// 'b' unchanged

			// broadcast (splat)
			CHECK_SHUFFLE( 0,0,0,0,			0,0,0,0,		0,0,0,0,		0,0,0,0 );
			CHECK_SHUFFLE( 15,15,15,15,		15,15,15,15,	15,15,15,15,	15,15,15,15 );
			CHECK_SHUFFLE( 16,16,16,16,		16,16,16,16,	16,16,16,16,	16,16,16,16 );
			CHECK_SHUFFLE( 31,31,31,31,		31,31,31,31,	31,31,31,31,	31,31,31,31 );
			CHECK_SHUFFLE( 16,16,16,16,		16,16,16,16,	0,0,0,0,		0,0,0,0 );			// splat b[0] | a[0]
			CHECK_SHUFFLE( 3,3,3,3,			3,3,3,3,		19,19,19,19,	19,19,19,19 );		// splat a[3] | b[3]

			// reverse
			CHECK_SHUFFLE( 15,14,13,12,		11,10,9,8,		7,6,5,4,		3,2,1,0 );			// reverse 'a'
			CHECK_SHUFFLE( 31,30,29,28,		27,26,25,24,	23,22,21,20,	19,18,17,16 );		// reverse 'b'
			CHECK_SHUFFLE( 31,30,29,28,		27,26,25,24,	7,6,5,4,		3,2,1,0 );			// high 'b' reversed | low 'a' reversed

			// half selection / lane swap
			CHECK_SHUFFLE( 16,17,18,19,		20,21,22,23,	0,1,2,3,		4,5,6,7 );			// low(b)  | low(a)
			CHECK_SHUFFLE( 24,25,26,27,		28,29,30,31,	8,9,10,11,		12,13,14,15 );		// high(b) | high(a)
			CHECK_SHUFFLE( 8,9,10,11,		12,13,14,15,	0,1,2,3,		4,5,6,7 );			// swap halves of 'a'
			CHECK_SHUFFLE( 24,25,26,27,		28,29,30,31,	16,17,18,19,	20,21,22,23 );		// swap halves of 'b'
			CHECK_SHUFFLE( 12,13,14,15,		8,9,10,11,		4,5,6,7,		0,1,2,3 );			// reverse 128-bit lanes of 'a'
			CHECK_SHUFFLE( 28,29,30,31,		24,25,26,27,	20,21,22,23,	16,17,18,19 );		// reverse 128-bit lanes of 'b'

			// interleave (unpack lo/hi style)
			CHECK_SHUFFLE( 0,16,1,17,		2,18,3,19,		4,20,5,21,		6,22,7,23 );		// interleave low halves
			CHECK_SHUFFLE( 8,24,9,25,		10,26,11,27,	12,28,13,29,	14,30,15,31 );		// interleave high halves

			// deinterleave
			CHECK_SHUFFLE( 0,2,4,6,			8,10,12,14,		16,18,20,22,	24,26,28,30 );		// even elements
			CHECK_SHUFFLE( 1,3,5,7,			9,11,13,15,		17,19,21,23,	25,27,29,31 );		// odd elements

			// alternating lanes (blend style)
			CHECK_SHUFFLE( 0,17,2,19,		4,21,6,23,		8,25,10,27,		12,29,14,31 );		// even lanes from 'a', odd from 'b'
			CHECK_SHUFFLE( 16,1,18,3,		20,5,22,7,		24,9,26,11,		28,13,30,15 );		// even lanes from 'b', odd from 'a'

			// rotate
			CHECK_SHUFFLE( 1,2,3,4,			5,6,7,8,		9,10,11,12,		13,14,15,0 );		// rotate 'a' left
			CHECK_SHUFFLE( 15,0,1,2,		3,4,5,6,		7,8,9,10,			11,12,13,14 );		// rotate 'a' right
			CHECK_SHUFFLE( 17,18,19,20,		21,22,23,24,	25,26,27,28,	29,30,31,16 );		// rotate 'b' left
			CHECK_SHUFFLE( 31,16,17,18,		19,20,21,22,	23,24,25,26,	27,28,29,30 );		// rotate 'b' right

			// duplicate pairs
			CHECK_SHUFFLE( 0,0,1,1,			2,2,3,3,		4,4,5,5,		6,6,7,7 );
			CHECK_SHUFFLE( 8,8,9,9,			10,10,11,11,	12,12,13,13,	14,14,15,15 );
			CHECK_SHUFFLE( 16,16,17,17,		18,18,19,19,	20,20,21,21,	22,22,23,23 );
			CHECK_SHUFFLE( 24,24,25,25,		26,26,27,27,	28,28,29,29,	30,30,31,31 );

			// boundary indices
			CHECK_SHUFFLE( 15,16,15,16,		15,16,15,16,	15,16,15,16,	15,16,15,16 );		// a[15]/b[0] boundary
			CHECK_SHUFFLE( 0,31,0,31,		0,31,0,31,		0,31,0,31,		0,31,0,31 );		// min/max indices

			// pseudo-random mixes
			CHECK_SHUFFLE( 12,20,9,25,		3,27,7,31,		0,18,14,22,		5,29,11,16 );
			CHECK_SHUFFLE( 31,4,17,9,		0,22,13,27,		6,19,30,11,		24,2,15,16 );
			CHECK_SHUFFLE( 5,30,0,25,		7,18,2,29,		12,21,14,17,	9,28,3,20 );
			CHECK_SHUFFLE( 0,31,1,30,		2,29,3,28,		4,27,5,26,		6,25,7,24 );		// converging extremes

			#undef CHECK_SHUFFLE
		}
	}


	template <typename Simd>
	static void  Test_Vec32 ()
	{
		using A = typename Simd::Scalar_t;
		using B = ScalarType<A>;
		using Arr_t = typename Simd::Array_t;

		// initialization
		{
			Arr_t	arr_zero;
			Arr_t	arr_one;
			Arr_t	arr_neg;
			Arr_t	arr_1234;

			for (usize i = 0; i < arr_zero.size(); ++i)
			{
				arr_zero[i] = AB(0);
				arr_one [i] = AB(1);
				arr_neg [i] = AB(-1);
				arr_1234[i] = AB(i+1);
			}

			const Simd	a1;	// zero
			const Simd	a2	{AB(1)};
			const Simd	a3	= Zero;
			const Simd	a4	{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),
							  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16),
							  AB(17), AB(18), AB(19), AB(20), AB(21), AB(22), AB(23), AB(24),
							  AB(25), AB(26), AB(27), AB(28), AB(29), AB(30), AB(31), AB(32) };
			const A		aa [] = { AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),
								  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16),
								  AB(17), AB(18), AB(19), AB(20), AB(21), AB(22), AB(23), AB(24),
								  AB(25), AB(26), AB(27), AB(28), AB(29), AB(30), AB(31), AB(32) };
			const Simd	a5	{ aa };
			const Simd	a6	{AB(-1)};

			TEST( a1.ToArray() == arr_zero );
			TEST( a2.ToArray() == arr_one  );
			TEST( a3.ToArray() == arr_zero );
			TEST( a4.ToArray() == arr_1234 );
			TEST( a5.ToArray() == arr_1234 );
			TEST( a6.ToArray() == arr_neg  );
		}

		if constexpr( IsInteger<B> )
		{
			const Simd	m0{ MSBMask<32,0>{} };
			const Simd	m1{ MSBMask<32,1>{} };
		}

		// set/get
		{
			const Simd	a1	{ AB(1),  AB(2),  AB(3),  AB(4),  AB(5),  AB(6),  AB(7),  AB(8),
							  AB(9),  AB(10), AB(11), AB(12), AB(13), AB(14), AB(15), AB(16),
							  AB(17), AB(18), AB(19), AB(20), AB(21), AB(22), AB(23), AB(24),
							  AB(25), AB(26), AB(27), AB(28), AB(29), AB(30), AB(31), AB(32) };
			const Simd	a2	{ AB(-1),  AB(-2),  AB(-3),  AB(-4),  AB(-5),  AB(-6),  AB(-7),  AB(-8),
							  AB(-9),  AB(-10), AB(-11), AB(-12), AB(-13), AB(-14), AB(-15), AB(-16),
							  AB(-17), AB(-18), AB(-19), AB(-20), AB(-21), AB(-22), AB(-23), AB(-24),
							  AB(-25), AB(-26), AB(-27), AB(-28), AB(-29), AB(-30), AB(-31), AB(-32) };

			TEST_Eq( a1.template get<0>(),	B(1) );
			TEST_Eq( a1.template get<1>(),	B(2) );
			TEST_Eq( a1.template get<2>(),	B(3) );
			TEST_Eq( a1.template get<3>(),	B(4) );
			TEST_Eq( a1.template get<4>(),	B(5) );
			TEST_Eq( a1.template get<5>(),	B(6) );
			TEST_Eq( a1.template get<6>(),	B(7) );
			TEST_Eq( a1.template get<7>(),	B(8) );
			TEST_Eq( a1.template get<8>(),	B(9) );
			TEST_Eq( a1.template get<9>(),	B(10) );
			TEST_Eq( a1.template get<10>(),	B(11) );
			TEST_Eq( a1.template get<11>(),	B(12) );
			TEST_Eq( a1.template get<12>(),	B(13) );
			TEST_Eq( a1.template get<13>(),	B(14) );
			TEST_Eq( a1.template get<14>(),	B(15) );
			TEST_Eq( a1.template get<15>(),	B(16) );
			TEST_Eq( a1.template get<16>(),	B(17) );
			TEST_Eq( a1.template get<17>(),	B(18) );
			TEST_Eq( a1.template get<18>(),	B(19) );
			TEST_Eq( a1.template get<19>(),	B(20) );
			TEST_Eq( a1.template get<20>(),	B(21) );
			TEST_Eq( a1.template get<21>(),	B(22) );
			TEST_Eq( a1.template get<22>(),	B(23) );
			TEST_Eq( a1.template get<23>(),	B(24) );
			TEST_Eq( a1.template get<24>(),	B(25) );
			TEST_Eq( a1.template get<25>(),	B(26) );
			TEST_Eq( a1.template get<26>(),	B(27) );
			TEST_Eq( a1.template get<27>(),	B(28) );
			TEST_Eq( a1.template get<28>(),	B(29) );
			TEST_Eq( a1.template get<29>(),	B(30) );
			TEST_Eq( a1.template get<30>(),	B(31) );
			TEST_Eq( a1.template get<31>(),	B(32) );

			TEST_Eq( a2.template get<0>(),	B(-1) );
			TEST_Eq( a2.template get<1>(),	B(-2) );
			TEST_Eq( a2.template get<2>(),	B(-3) );
			TEST_Eq( a2.template get<3>(),	B(-4) );
			TEST_Eq( a2.template get<4>(),	B(-5) );
			TEST_Eq( a2.template get<5>(),	B(-6) );
			TEST_Eq( a2.template get<6>(),	B(-7) );
			TEST_Eq( a2.template get<7>(),	B(-8) );
			TEST_Eq( a2.template get<8>(),	B(-9) );
			TEST_Eq( a2.template get<9>(),	B(-10) );
			TEST_Eq( a2.template get<10>(),	B(-11) );
			TEST_Eq( a2.template get<11>(),	B(-12) );
			TEST_Eq( a2.template get<12>(),	B(-13) );
			TEST_Eq( a2.template get<13>(),	B(-14) );
			TEST_Eq( a2.template get<14>(),	B(-15) );
			TEST_Eq( a2.template get<15>(),	B(-16) );
			TEST_Eq( a2.template get<16>(),	B(-17) );
			TEST_Eq( a2.template get<17>(),	B(-18) );
			TEST_Eq( a2.template get<18>(),	B(-19) );
			TEST_Eq( a2.template get<19>(),	B(-20) );
			TEST_Eq( a2.template get<20>(),	B(-21) );
			TEST_Eq( a2.template get<21>(),	B(-22) );
			TEST_Eq( a2.template get<22>(),	B(-23) );
			TEST_Eq( a2.template get<23>(),	B(-24) );
			TEST_Eq( a2.template get<24>(),	B(-25) );
			TEST_Eq( a2.template get<25>(),	B(-26) );
			TEST_Eq( a2.template get<26>(),	B(-27) );
			TEST_Eq( a2.template get<27>(),	B(-28) );
			TEST_Eq( a2.template get<28>(),	B(-29) );
			TEST_Eq( a2.template get<29>(),	B(-30) );
			TEST_Eq( a2.template get<30>(),	B(-31) );
			TEST_Eq( a2.template get<31>(),	B(-32) );
		}

		// swizzle
		if constexpr( Simd::Has_Swizzle() )
		{
			Arr_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckSwizzle = [&arr](Simd v, const List<uint> &idx)
			{{
				TEST_Eq( idx.size(), 32 );
				auto	a = v.ToArray();
				Arr_t	b;
				usize	i = 0;
				for (uint x : idx) b[i++] = arr[x];
				TEST( a == b );
			}};

			#define CHECK_SWIZZLE( ... )	CheckSwizzle( v.template Swizzle< __VA_ARGS__ >(), { __VA_ARGS__ });

			Simd	v {arr.data()};

			// identity
			CHECK_SWIZZLE( 0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31 );

			// full reverse
			CHECK_SWIZZLE( 31,30,29,28, 27,26,25,24, 23,22,21,20, 19,18,17,16, 15,14,13,12, 11,10,9,8, 7,6,5,4, 3,2,1,0 );

			// broadcast single element to all lanes
			CHECK_SWIZZLE( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 );
			CHECK_SWIZZLE( 31,31,31,31, 31,31,31,31, 31,31,31,31, 31,31,31,31, 31,31,31,31, 31,31,31,31, 31,31,31,31, 31,31,31,31 );
			CHECK_SWIZZLE( 15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15 );

			// broadcast 128-bit lane
			CHECK_SWIZZLE( 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3 );
			CHECK_SWIZZLE( 28,29,30,31, 28,29,30,31, 28,29,30,31, 28,29,30,31, 28,29,30,31, 28,29,30,31, 28,29,30,31, 28,29,30,31 );

			// duplicate low / high half
			CHECK_SWIZZLE( 0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15 );
			CHECK_SWIZZLE( 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31, 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31 );

			// swap halves
			CHECK_SWIZZLE( 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31, 0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15 );

			// rotate left / right by 1
			CHECK_SWIZZLE( 1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16, 17,18,19,20, 21,22,23,24, 25,26,27,28, 29,30,31,0 );
			CHECK_SWIZZLE( 31,0,1,2, 3,4,5,6, 7,8,9,10, 11,12,13,14, 15,16,17,18, 19,20,21,22, 23,24,25,26, 27,28,29,30 );

			// rotate by one 128-bit lane
			CHECK_SWIZZLE( 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31, 0,1,2,3 );

			// swap adjacent pairs
			CHECK_SWIZZLE( 1,0,3,2, 5,4,7,6, 9,8,11,10, 13,12,15,14, 17,16,19,18, 21,20,23,22, 25,24,27,26, 29,28,31,30 );

			// reverse within each 128-bit lane
			CHECK_SWIZZLE( 3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12, 19,18,17,16, 23,22,21,20, 27,26,25,24, 31,30,29,28 );

			// interleave (zip) low and high halves
			CHECK_SWIZZLE( 0,16,1,17, 2,18,3,19, 4,20,5,21, 6,22,7,23, 8,24,9,25, 10,26,11,27, 12,28,13,29, 14,30,15,31 );

			// deinterleave (unzip) evens / odds
			CHECK_SWIZZLE( 0,2,4,6, 8,10,12,14, 16,18,20,22, 24,26,28,30, 1,3,5,7, 9,11,13,15, 17,19,21,23, 25,27,29,31 );

			// 4x4 transpose within each half
			CHECK_SWIZZLE( 0,4,8,12, 1,5,9,13, 2,6,10,14, 3,7,11,15, 16,20,24,28, 17,21,25,29, 18,22,26,30, 19,23,27,31 );

			// duplicate each element
			CHECK_SWIZZLE( 0,0,1,1, 2,2,3,3, 4,4,5,5, 6,6,7,7, 8,8,9,9, 10,10,11,11, 12,12,13,13, 14,14,15,15 );

			// alternating extremes
			CHECK_SWIZZLE( 0,31,1,30, 2,29,3,28, 4,27,5,26, 6,25,7,24, 8,23,9,22, 10,21,11,20, 12,19,13,18, 14,17,15,16 );

			// repeated cross-lane pattern
			CHECK_SWIZZLE( 3,27,3,27, 5,29,5,29, 0,16,0,16, 31,15,31,15, 7,23,7,23, 12,20,12,20, 1,30,1,30, 8,24,8,24 );

			// strided gather (step 3 mod 32)
			CHECK_SWIZZLE( 0,3,6,9, 12,15,18,21, 24,27,30,1, 4,7,10,13, 16,19,22,25, 28,31,2,5, 8,11,14,17, 20,23,26,29 );

			// pseudo-random permutation (each index used once)
			CHECK_SWIZZLE( 5,22,9,0, 17,30,3,12, 25,7,14,28, 1,19,10,24, 31,6,13,20, 2,27,16,8, 29,11,23,4, 18,26,15,21 );
			#undef CHECK_SWIZZLE
		}

		// shuffle
		if constexpr( Simd::Has_Shuffle() )
		{
			StaticArray<A,64>	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	CheckShuffle = [&arr](Simd v, const StaticArray<uint,32> &idx)
			{{
				auto	a = v.ToArray();
				Arr_t	b {};
				for (usize i = 0; i < a.size(); ++i)
					b[i] = arr[idx[i]];
				TEST( a == b );
			}};

			Simd	a {arr.data()};
			Simd	b {arr.data() + 32};

			#define CHECK_SHUFFLE( ... )	CheckShuffle( a.template Shuffle<__VA_ARGS__>( b ), StaticArray<uint,32>{ __VA_ARGS__ } );

			// identity / pass-through
			CHECK_SHUFFLE( 0,1,2,3,			4,5,6,7,		8,9,10,11,		12,13,14,15,
						   16,17,18,19,		20,21,22,23,	24,25,26,27,	28,29,30,31 );		// 'a' unchanged
			CHECK_SHUFFLE( 32,33,34,35,		36,37,38,39,	40,41,42,43,	44,45,46,47,
						   48,49,50,51,		52,53,54,55,	56,57,58,59,	60,61,62,63 );		// 'b' unchanged

			// broadcast (splat)
			CHECK_SHUFFLE( 0,0,0,0,			0,0,0,0,		0,0,0,0,		0,0,0,0,
						   0,0,0,0,			0,0,0,0,		0,0,0,0,		0,0,0,0 );
			CHECK_SHUFFLE( 31,31,31,31,		31,31,31,31,	31,31,31,31,	31,31,31,31,
						   31,31,31,31,		31,31,31,31,	31,31,31,31,	31,31,31,31 );
			CHECK_SHUFFLE( 32,32,32,32,		32,32,32,32,	32,32,32,32,	32,32,32,32,
						   32,32,32,32,		32,32,32,32,	32,32,32,32,	32,32,32,32 );
			CHECK_SHUFFLE( 63,63,63,63,		63,63,63,63,	63,63,63,63,	63,63,63,63,
						   63,63,63,63,		63,63,63,63,	63,63,63,63,	63,63,63,63 );
			CHECK_SHUFFLE( 32,32,32,32,		32,32,32,32,	32,32,32,32,	32,32,32,32,
						   0,0,0,0,			0,0,0,0,		0,0,0,0,		0,0,0,0 );			// splat b[0] | a[0]
			CHECK_SHUFFLE( 3,3,3,3,			3,3,3,3,		3,3,3,3,		3,3,3,3,
						   35,35,35,35,		35,35,35,35,	35,35,35,35,	35,35,35,35 );		// splat a[3] | b[3]

			// reverse
			CHECK_SHUFFLE( 31,30,29,28,		27,26,25,24,	23,22,21,20,	19,18,17,16,
						   15,14,13,12,		11,10,9,8,		7,6,5,4,		3,2,1,0 );			// reverse 'a'
			CHECK_SHUFFLE( 63,62,61,60,		59,58,57,56,	55,54,53,52,	51,50,49,48,
						   47,46,45,44,		43,42,41,40,	39,38,37,36,	35,34,33,32 );		// reverse 'b'
			CHECK_SHUFFLE( 63,62,61,60,		59,58,57,56,	55,54,53,52,	51,50,49,48,
						   15,14,13,12,		11,10,9,8,		7,6,5,4,		3,2,1,0 );			// high 'b' reversed | low 'a' reversed

			// half selection / lane swap
			CHECK_SHUFFLE( 32,33,34,35,		36,37,38,39,	40,41,42,43,	44,45,46,47,
						   0,1,2,3,			4,5,6,7,		8,9,10,11,		12,13,14,15 );		// low(b)  | low(a)
			CHECK_SHUFFLE( 48,49,50,51,		52,53,54,55,	56,57,58,59,	60,61,62,63,
						   16,17,18,19,		20,21,22,23,	24,25,26,27,	28,29,30,31 );		// high(b) | high(a)
			CHECK_SHUFFLE( 16,17,18,19,		20,21,22,23,	24,25,26,27,	28,29,30,31,
						   0,1,2,3,			4,5,6,7,		8,9,10,11,		12,13,14,15 );		// swap halves of 'a'
			CHECK_SHUFFLE( 48,49,50,51,		52,53,54,55,	56,57,58,59,	60,61,62,63,
						   32,33,34,35,		36,37,38,39,	40,41,42,43,	44,45,46,47 );		// swap halves of 'b'
			CHECK_SHUFFLE( 16,17,18,19,		20,21,22,23,	24,25,26,27,	28,29,30,31,
						   32,33,34,35,		36,37,38,39,	40,41,42,43,	44,45,46,47 );		// concat shift across operands: high(a) | low(b)
			CHECK_SHUFFLE( 28,29,30,31,		24,25,26,27,	20,21,22,23,	16,17,18,19,
						   12,13,14,15,		8,9,10,11,		4,5,6,7,		0,1,2,3 );			// reverse 128-bit lanes of 'a'
			CHECK_SHUFFLE( 60,61,62,63,		56,57,58,59,	52,53,54,55,	48,49,50,51,
						   44,45,46,47,		40,41,42,43,	36,37,38,39,	32,33,34,35 );		// reverse 128-bit lanes of 'b'
			CHECK_SHUFFLE( 24,25,26,27,		28,29,30,31,	16,17,18,19,	20,21,22,23,
						   8,9,10,11,		12,13,14,15,	0,1,2,3,		4,5,6,7 );			// reverse 256-bit lanes of 'a'
			CHECK_SHUFFLE( 56,57,58,59,		60,61,62,63,	48,49,50,51,	52,53,54,55,
						   40,41,42,43,		44,45,46,47,	32,33,34,35,	36,37,38,39 );		// reverse 256-bit lanes of 'b'

			// interleave (unpack lo/hi style)
			CHECK_SHUFFLE( 0,32,1,33,		2,34,3,35,		4,36,5,37,		6,38,7,39,
						   8,40,9,41,		10,42,11,43,	12,44,13,45,	14,46,15,47 );		// interleave low halves
			CHECK_SHUFFLE( 16,48,17,49,		18,50,19,51,	20,52,21,53,	22,54,23,55,
						   24,56,25,57,		26,58,27,59,	28,60,29,61,	30,62,31,63 );		// interleave high halves

			// deinterleave
			CHECK_SHUFFLE( 0,2,4,6,			8,10,12,14,		16,18,20,22,	24,26,28,30,
						   32,34,36,38,		40,42,44,46,	48,50,52,54,	56,58,60,62 );		// even elements
			CHECK_SHUFFLE( 1,3,5,7,			9,11,13,15,		17,19,21,23,	25,27,29,31,
						   33,35,37,39,		41,43,45,47,	49,51,53,55,	57,59,61,63 );		// odd elements

			// alternating lanes (blend style)
			CHECK_SHUFFLE( 0,33,2,35,		4,37,6,39,		8,41,10,43,		12,45,14,47,
						   16,49,18,51,		20,53,22,55,	24,57,26,59,	28,61,30,63 );		// even lanes from 'a', odd from 'b'
			CHECK_SHUFFLE( 32,1,34,3,		36,5,38,7,		40,9,42,11,		44,13,46,15,
						   48,17,50,19,		52,21,54,23,	56,25,58,27,	60,29,62,31 );		// even lanes from 'b', odd from 'a'

			// rotate
			CHECK_SHUFFLE( 1,2,3,4,			5,6,7,8,		9,10,11,12,		13,14,15,16,
						   17,18,19,20,		21,22,23,24,	25,26,27,28,	29,30,31,0 );		// rotate 'a' left
			CHECK_SHUFFLE( 31,0,1,2,		3,4,5,6,		7,8,9,10,		11,12,13,14,
						   15,16,17,18,		19,20,21,22,	23,24,25,26,	27,28,29,30 );		// rotate 'a' right
			CHECK_SHUFFLE( 33,34,35,36,		37,38,39,40,	41,42,43,44,	45,46,47,48,
						   49,50,51,52,		53,54,55,56,	57,58,59,60,	61,62,63,32 );		// rotate 'b' left
			CHECK_SHUFFLE( 63,32,33,34,		35,36,37,38,	39,40,41,42,	43,44,45,46,
						   47,48,49,50,		51,52,53,54,	55,56,57,58,	59,60,61,62 );		// rotate 'b' right

			// duplicate pairs
			CHECK_SHUFFLE( 0,0,1,1,			2,2,3,3,		4,4,5,5,		6,6,7,7,
						   8,8,9,9,			10,10,11,11,	12,12,13,13,	14,14,15,15 );
			CHECK_SHUFFLE( 16,16,17,17,		18,18,19,19,	20,20,21,21,	22,22,23,23,
						   24,24,25,25,		26,26,27,27,	28,28,29,29,	30,30,31,31 );
			CHECK_SHUFFLE( 32,32,33,33,		34,34,35,35,	36,36,37,37,	38,38,39,39,
						   40,40,41,41,		42,42,43,43,	44,44,45,45,	46,46,47,47 );
			CHECK_SHUFFLE( 48,48,49,49,		50,50,51,51,	52,52,53,53,	54,54,55,55,
						   56,56,57,57,		58,58,59,59,	60,60,61,61,	62,62,63,63 );

			// boundary indices
			CHECK_SHUFFLE( 31,32,31,32,		31,32,31,32,	31,32,31,32,	31,32,31,32,
						   31,32,31,32,		31,32,31,32,	31,32,31,32,	31,32,31,32 );		// a[31]/b[0] boundary
			CHECK_SHUFFLE( 0,63,0,63,		0,63,0,63,		0,63,0,63,		0,63,0,63,
						   0,63,0,63,		0,63,0,63,		0,63,0,63,		0,63,0,63 );		// min/max indices

			// pseudo-random mixes
			CHECK_SHUFFLE( 12,36,9,57,		3,43,7,63,		0,50,14,38,		5,61,11,32,
						   20,44,17,49,		27,35,23,55,	16,58,30,40,	21,47,29,34 );
			CHECK_SHUFFLE( 63,4,33,41,		0,54,13,59,		6,35,62,11,		56,2,47,48,
						   31,38,17,45,		26,53,8,60,		22,37,51,1,		44,19,58,28 );
			CHECK_SHUFFLE( 0,63,1,62,		2,61,3,60,		4,59,5,58,		6,57,7,56,
						   8,55,9,54,		10,53,11,52,	12,51,13,50,	14,49,15,48 );		// converging extremes
			CHECK_SHUFFLE( 31,32,30,33,		29,34,28,35,	27,36,26,37,	25,38,24,39,
						   23,40,22,41,		21,42,20,43,	19,44,18,45,	17,46,16,47 );		// diverging from center

			#undef CHECK_SHUFFLE
		}
	}


	template <typename Scalar, typename Simd>
	static void  AllEqual (const Simd a, const Scalar b)
	{
		for (auto& c : a.ToArray()) {
			TEST_Eq( Scalar(c), b );
		}
	}

	template <typename Scalar, typename Simd>
	static void  AllEqual (const Simd a, const Scalar b, EnabledBitCount acc)
	{
		for (auto& c : a.ToArray()) {
			TEST(BitEqual( Scalar(c), b, acc ));
		}
	}


	template <typename Simd>
	static void  Test_FloatRound ()
	{
		StaticAssert( Simd::Has_Rounding() );

		using A = typename Simd::Scalar_t;
		using B = ScalarType< A >;

		for (B f = B(-2.0); f < B(3.0); f += B(0.05))
		{
			A a = A(f);
			AllEqual( Simd{a}.Floor(), Floor(B(a)) );
		}
		for (B f = B(-2.0); f < B(3.0); f += B(0.05))
		{
			A a = A(f);
			AllEqual( Simd{a}.Ceil(), Ceil(B(a)) );
		}
		for (B f = B(-2.0); f < B(3.0); f += B(0.05))
		{
			A a = A(f);
			AllEqual( Simd{a}.Trunc(), Trunc(B(a)) );
		}
		for (B f = B(-2.0); f < B(3.0); f += B(0.05))
		{
			A a = A(f);
			AllEqual( Simd{a}.Round(), Round(B(a)) );
		}
		for (B f = B(-2.0); f < B(3.0); f += B(0.05))
		{
			A a = A(f);
			AllEqual( Simd{a}.RoundEven(), RoundEven(B(a)) );
		}
	}


	template <typename Simd>
	static void  Test_FloatArithmetic ()
	{
		StaticAssert( Simd::Has_Arithmetic() );
		StaticAssert( Simd::Has_PreciseSqrt() or Simd::Has_ApproxInvSqrt() );
		StaticAssert( Simd::Has_PreciseDiv() or Simd::Has_ApproxReciprocal() );

		using A = typename Simd::Scalar_t;
		using B = ScalarType< A >;

		const auto	acc = sizeof(A) < 4 ? EnabledBitCount(20) : EnabledBitCount(CT_SizeOfInBits<A>);

		{
			const Simd	a1 = Simd{B(1.0)} + Simd{B(0.1)};	AllEqual( a1, B(1.1),  acc );
			const Simd	a2 = Simd{B(1.0)} - Simd{B(0.1)};	AllEqual( a2, B(0.9),  acc );
			const Simd	a3 = Simd{B(1.0)} * Simd{B(8.0)};	AllEqual( a3, B(8.0),  acc );
			const Simd	a5 = Simd{B(1.0)} + B(0.1);			AllEqual( a5, B(1.1),  acc );
			const Simd	a6 = Simd{B(1.0)} - B(0.1);			AllEqual( a6, B(0.9),  acc );
			const Simd	a7 = Simd{B(1.0)} * B(0.1);			AllEqual( a7, B(0.1),  acc );
		}{
			const Simd	a1 = Simd{B(-1.234)}.Abs();
			const Simd	a2 = Simd{B(-1.234)}.Max( Simd{B(2.2)});
			const Simd	a3 = Simd{B(-1.234)}.Min( Simd{B(-3.3)});
			const Simd	a4 = Simd{B(-1.234)}.Max( AB(2.2) );
			const Simd	a5 = Simd{B(-1.234)}.Min( AB(-3.3) );

			AllEqual( a1, B(1.234), acc );
			AllEqual( a2, B(2.2),   acc );
			AllEqual( a3, B(-3.3),  acc );
			AllEqual( a4, B(2.2),   acc );
			AllEqual( a5, B(-3.3),  acc );
		}{
			const auto	acc1	= sizeof(A) >= 8 ?	EnabledBitCount(62) :
								  sizeof(A) >= 4 ?	EnabledBitCount(30) :
													EnabledBitCount(14);

			const Simd	a0	= Lerp( Simd{B(2.0)}, Simd{B(5.0)}, Simd{B(0.1)} );
			const Simd	a1	= Lerp( Simd{B(2.0)}, Simd{B(5.0)}, Simd{B(0.7)} );
			const B		ref0 = Lerp( B(2.0), B(5.0), B(0.1) );
			const B		ref1 = Lerp( B(2.0), B(5.0), B(0.7) );

			AllEqual( a0, ref0, acc1 );
			AllEqual( a1, ref1, acc1 );
		}{
			const Simd	a0 = Select( typename Simd::Bool_t{true},  Simd{B(-1.0)}, Simd{B(-7.0)} );
			const Simd	a1 = Select( typename Simd::Bool_t{false}, Simd{B(-1.0)}, Simd{B(-7.0)} );

			AllEqual( a0, B(-1.0), acc );
			AllEqual( a1, B(-7.0), acc );
		}
		if constexpr( Simd::Has_Greater() )
		{
			const Simd	a0 = SelectF( Simd{B(1.0)}, Simd{B(4.0)},  Simd{B(-1.0)}, Simd{B(-7.0)} );	// 1 < 4
			const Simd	a1 = SelectF( Simd{B(2.0)}, Simd{B(1.0)},  Simd{B(-1.0)}, Simd{B(-7.0)} );	// 2 < 1
			const Simd	a2 = Select( Simd{B(1.0)}.Less(Simd{B(4.0)}), Simd{B(-1.0)}, Simd{B(-7.0)} );
			const Simd	a3 = Select( Simd{B(2.0)}.Less(Simd{B(1.0)}), Simd{B(-1.0)}, Simd{B(-7.0)} );

			AllEqual( a0, B(-1.0), acc );
			AllEqual( a1, B(-7.0), acc );
			AllEqual( a2, B(-1.0), acc );
			AllEqual( a3, B(-7.0), acc );
		}
		if constexpr( Simd::Has_PreciseDiv() )
		{
			const Simd	a1 = Simd{B(1.0)}.PreciseDiv( Simd{B(0.1)} );	AllEqual( a1, B(10.0), acc );
			const Simd	a2 = Simd{B(1.0)}.PreciseDiv( AB(0.1) );		AllEqual( a2, B(10.0), acc );
		}
		if constexpr( Simd::Has_PreciseSqrt() )
		{
			const Simd	a1 = Simd{B(2.0)}.PreciseSqrt();				AllEqual( a1, Sqrt(B(2.0)), acc );
		}
		if constexpr( Simd::Has_PreciseInvSqrt() )
		{
			const auto	acc1 = sizeof(A) >= 8 ? EnabledBitCount(62) : EnabledBitCount(30);

			const Simd	a1 = Simd{B(2.0)}.PreciseInvSqrt();				AllEqual( a1, InvSqrt(B(2.0)), acc1 );
		}
		if constexpr( Simd::Has_ApproxReciprocal() )
		{
			const auto	acc1 = sizeof(A) < 4 ? EnabledBitCount(16) : EnabledBitCount(20);
			const Simd	a1 = Simd{B(0.1)}.Reciprocal();					AllEqual( a1, B(10.0),		acc1 );
			const Simd	a2 = Simd{B(0.1)}.FastDiv( Simd{B(0.02)} );		AllEqual( a2, B(0.1/0.02),  acc1 );
		}
		if constexpr( Simd::Has_ApproxInvSqrt() )
		{
			const auto	acc1 = sizeof(A) < 4 ? EnabledBitCount(16) :
								(AE_SIMD_NEON ? EnabledBitCount(16) : EnabledBitCount(20));

			const Simd	a1 = Simd{B(2.0)}.FastInvSqrt();				AllEqual( a1, B(1.0)/Sqrt(B(2.0)),  acc1 );
			const Simd	a2 = Simd{B(2.0)}.FastSqrt();					AllEqual( a2, Sqrt(B(2.0)),			acc1 );
		}
		if constexpr( Simd::Has_MulAdd() )
		{
			const Simd	a1 = MulAdd( Simd{B(1.1)}, Simd{B(2.2)}, Simd{B(3.3)} );
			const Simd	a2 = NegMulAdd( Simd{B(1.1)}, Simd{B(2.2)}, Simd{B(3.3)} );
			const auto	acc1 = sizeof(A) < 4 ? EnabledBitCount(16) : EnabledBitCount(30);

			AllEqual( a1, B(5.72f), acc1 );
			AllEqual( a2, B(0.88f), acc1 );
		}
		if constexpr( Simd::Has_FusedMulAdd() )
		{
			const Simd	a1 = FusedMulAdd( Simd{B(1.1)}, Simd{B(2.2)}, Simd{B(3.3)} );
			const Simd	a2 = FusedNegMulAdd( Simd{B(1.1)}, Simd{B(2.2)}, Simd{B(3.3)} );	// -a * b + c
			const Simd	a3 = FusedMulSub( Simd{B(1.1)}, Simd{B(2.2)}, Simd{B(3.3)} );		// a * b - c
			const auto	acc1 = sizeof(A) < 4 ? EnabledBitCount(16) : EnabledBitCount(30);

			AllEqual( a1, B(5.72f),  acc1 );
			AllEqual( a2, B(0.88f),  acc1 );
			AllEqual( a3, B(-0.88f), acc1 );
		}
		if constexpr( Simd::Has_BitEqual() )
		{
			// TODO
		}
		if constexpr( Simd::Has_ReduceAdd() )
		{
			typename Simd::Array_t	arr;
			A						sum = A(0);
			for (usize i = 0; i < arr.size(); ++i) {
				arr[i] = AB(i+1);
				sum += arr[i];
			}

			const auto	a1 = Simd{arr.data()}.ReduceAddScalar();	TEST( BitEqual( B(a1), B(sum), acc ));
		}
		if constexpr( Simd::Has_ReduceMinMax() )
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	a1 = Simd{arr.data()}.ReduceMinScalar();	TEST( a1 == arr.front() );
			const auto	a2 = Simd{arr.data()}.ReduceMaxScalar();	TEST( a2 == arr.back() );
		}
		if constexpr( Simd::Has_InclusiveAdd() )
		{
			typename Simd::Array_t	arr;
			typename Simd::Array_t	scan;
			A						sum = A(0);
			for (usize i = 0; i < arr.size(); ++i) {
				arr[i]	= AB(i+1);
				sum		+= arr[i];
				scan[i]	= sum;
			}

			const auto	a1 = Simd{arr.data()}.InclusiveAdd().ToArray();
			for (usize i = 0; i < scan.size(); ++i) {
				TEST( BitEqual( B(a1[i]), B(scan[i]), acc ));
			}
		}
		if constexpr( Simd::Has_InclusiveAdd() )
		{
			typename Simd::Array_t	arr;
			typename Simd::Array_t	scan;
			A						sum = A(0);
			for (usize i = 0; i < arr.size(); ++i) {
				scan[i]	= sum;
				arr[i]	= AB(i+1);
				sum		+= arr[i];
			}

			const auto	a1 = Simd{arr.data()}.ExclusiveAdd().ToArray();
			for (usize i = 0; i < scan.size(); ++i) {
				TEST( BitEqual( B(a1[i]), B(scan[i]), acc ));
			}
		}
		if constexpr( Simd::Has_Trigonometry() )
		{
			const auto	acc1	= sizeof(A) >= 8 ? EnabledBitCount(62) : EnabledBitCount(30);
			const auto	angle0	= B(23_deg);
			const auto	angle1	= B(-37_deg);
			const auto	angle2	= B(117_deg);
			const auto	angle3	= B(291_deg);
			const auto	angle4	= B(462_deg);
			{
				const auto	a0 = Simd{A(angle0)}.Sin();
				const auto	a1 = Simd{A(angle1)}.Sin();
				const auto	a2 = Simd{A(angle2)}.Sin();
				const auto	a3 = Simd{A(angle3)}.Sin();
				const auto	a4 = Simd{A(angle4)}.Sin();

				AllEqual( a0, B(std::sin(angle0)), acc1 );
				AllEqual( a1, B(std::sin(angle1)), acc1 );
				AllEqual( a2, B(std::sin(angle2)), acc1 );
				AllEqual( a3, B(std::sin(angle3)), acc1 );
				AllEqual( a4, B(std::sin(angle4)), acc1 );
			}{
				const auto	a0 = Simd{A(angle0)}.Cos();
				const auto	a1 = Simd{A(angle1)}.Cos();
				const auto	a2 = Simd{A(angle2)}.Cos();
				const auto	a3 = Simd{A(angle3)}.Cos();
				const auto	a4 = Simd{A(angle4)}.Cos();

				AllEqual( a0, B(std::cos(angle0)), acc1 );
				AllEqual( a1, B(std::cos(angle1)), acc1 );
				AllEqual( a2, B(std::cos(angle2)), acc1 );
				AllEqual( a3, B(std::cos(angle3)), acc1 );
				AllEqual( a4, B(std::cos(angle4)), acc1 );
			}{
				const auto	a0 = Simd{A(angle0)}.Tan();
				const auto	a1 = Simd{A(angle1)}.Tan();
				const auto	a2 = Simd{A(angle2)}.Tan();
				const auto	a3 = Simd{A(angle3)}.Tan();
				const auto	a4 = Simd{A(angle4)}.Tan();

				AllEqual( a0, B(std::tan(angle0)), acc1 );
				AllEqual( a1, B(std::tan(angle1)), acc1 );
				AllEqual( a2, B(std::tan(angle2)), acc1 );
				AllEqual( a3, B(std::tan(angle3)), acc1 );
				AllEqual( a4, B(std::tan(angle4)), acc1 );
			}
		}
		if constexpr( Simd::Has_Exponential() )
		{
			const auto	acc1 = sizeof(A) >= 8 ? EnabledBitCount(62) : EnabledBitCount(30);
			{
				const auto	v0 = B(1.283);
				const auto	v1 = B(2.951);
				const auto	v2 = B(3.873);
				const auto	v3 = B(9.457);

				const auto	a0 = Simd{A(v0)}.Exp();
				const auto	a1 = Simd{A(v1)}.Exp();
				const auto	a2 = Simd{A(v2)}.Exp();
				const auto	a3 = Simd{A(v3)}.Exp();

				AllEqual( a0, B(std::exp(v0)), acc1 );
				AllEqual( a1, B(std::exp(v1)), acc1 );
				AllEqual( a2, B(std::exp(v2)), acc1 );
				AllEqual( a3, B(std::exp(v3)), acc1 );
			}{
				const auto	v0 = B(1.283);
				const auto	v1 = B(2.951);
				const auto	v2 = B(3.873);
				const auto	v3 = B(9.457);

				const auto	a0 = Simd{A(v0)}.Exp2();
				const auto	a1 = Simd{A(v1)}.Exp2();
				const auto	a2 = Simd{A(v2)}.Exp2();
				const auto	a3 = Simd{A(v3)}.Exp2();

				AllEqual( a0, B(std::exp2(v0)), acc1 );
				AllEqual( a1, B(std::exp2(v1)), acc1 );
				AllEqual( a2, B(std::exp2(v2)), acc1 );
				AllEqual( a3, B(std::exp2(v3)), acc1 );
			}{
				const auto	v0 = B(1.283);
				const auto	v1 = B(2.951);
				const auto	v2 = B(3.873);
				const auto	v3 = B(9.457);

				const auto	a0 = Simd{A(v0)}.Exp10();
				const auto	a1 = Simd{A(v1)}.Exp10();
				const auto	a2 = Simd{A(v2)}.Exp10();
				const auto	a3 = Simd{A(v3)}.Exp10();

				AllEqual( a0, B(Exp10(v0)), acc1 );
				AllEqual( a1, B(Exp10(v1)), acc1 );
				AllEqual( a2, B(Exp10(v2)), acc1 );
				AllEqual( a3, B(Exp10(v3)), acc1 );
			}{
				const auto	v0 = B(1.283);
				const auto	v1 = B(2.951);
				const auto	v2 = B(3.873);
				const auto	v3 = B(9.457);

				const auto	a0 = Simd{A(v0)}.Ln();
				const auto	a1 = Simd{A(v1)}.Ln();
				const auto	a2 = Simd{A(v2)}.Ln();
				const auto	a3 = Simd{A(v3)}.Ln();

				AllEqual( a0, B(Ln(v0)), acc1 );
				AllEqual( a1, B(Ln(v1)), acc1 );
				AllEqual( a2, B(Ln(v2)), acc1 );
				AllEqual( a3, B(Ln(v3)), acc1 );
			}{
				const auto	v0 = B(1.283);
				const auto	v1 = B(2.951);
				const auto	v2 = B(3.873);
				const auto	v3 = B(9.457);

				const auto	a0 = Simd{A(v0)}.Log2();
				const auto	a1 = Simd{A(v1)}.Log2();
				const auto	a2 = Simd{A(v2)}.Log2();
				const auto	a3 = Simd{A(v3)}.Log2();

				AllEqual( a0, B(Log2(v0)), acc1 );
				AllEqual( a1, B(Log2(v1)), acc1 );
				AllEqual( a2, B(Log2(v2)), acc1 );
				AllEqual( a3, B(Log2(v3)), acc1 );
			}{
				const auto	v0 = B(1.283);
				const auto	v1 = B(2.951);
				const auto	v2 = B(3.873);
				const auto	v3 = B(9.457);

				const auto	a0 = Simd{A(v0)}.Log10();
				const auto	a1 = Simd{A(v1)}.Log10();
				const auto	a2 = Simd{A(v2)}.Log10();
				const auto	a3 = Simd{A(v3)}.Log10();

				AllEqual( a0, B(Log10(v0)), acc1 );
				AllEqual( a1, B(Log10(v1)), acc1 );
				AllEqual( a2, B(Log10(v2)), acc1 );
				AllEqual( a3, B(Log10(v3)), acc1 );
			}
		}
	}


	template <typename Simd>
	static void  Test_IntArithmetic ()
	{
		StaticAssert( Simd::Has_Arithmetic() );

		using A			= typename Simd::Scalar_t;
		using Shift64_t	= typename Simd::Shift64_t;

		{
			const Simd	a1 = Simd{A(2)} + Simd{A(5)};
			const Simd	a2 = Simd{A(2)} - Simd{A(1)};
			const Simd	a3 = Simd{A(2)} + A(4);
			const Simd	a4 = Simd{A(2)} - A(1);
			const Simd	a5 = Simd{A(2)} - A(3);

			AllEqual( a1, A(2+5) );
			AllEqual( a2, A(2-1) );
			AllEqual( a3, A(2+4) );
			AllEqual( a4, A(2-1) );

			if constexpr( IsSigned<A> ){
				AllEqual( a5, A(-1) );
			}else{
				AllEqual( a5, A(UMax) );
			}
		}{
			const Simd	a0 = Select( typename Simd::Bool_t{true},  Simd{A(2)}, Simd{A(4)} );
			const Simd	a1 = Select( typename Simd::Bool_t{false}, Simd{A(2)}, Simd{A(4)} );

			AllEqual( a0, A(2) );
			AllEqual( a1, A(4) );
		}
		if constexpr( Simd::Has_MinMax() )
		{
			const Simd	a1 = Simd{A(22)}.Max( Simd{A(11)});
			const Simd	a2 = Simd{A(22)}.Min( Simd{A(11)});
			const Simd	a3 = Simd{A(22)}.Max( A(11) );
			const Simd	a4 = Simd{A(22)}.Min( A(11) );

			AllEqual( a1, A(22) );
			AllEqual( a2, A(11) );
			AllEqual( a3, A(22) );
			AllEqual( a4, A(11) );
		}
		if constexpr( IsSigned<A> and Simd::Has_MinMax() )
		{
			const Simd	a1 = Simd{A(-22)}.Max( Simd{A(11)});
			const Simd	a2 = Simd{A(-22)}.Min( Simd{A(11)});
			const Simd	a3 = Simd{A(-22)}.Max( A(11) );
			const Simd	a4 = Simd{A(-22)}.Min( A(11) );

			AllEqual( a1, A(11) );
			AllEqual( a2, A(-22) );
			AllEqual( a3, A(11) );
			AllEqual( a4, A(-22) );
		}
		if constexpr( Simd::Has_Div() )
		{
			const Simd	a1 = Simd{A(40)} / Simd{A(2)};	AllEqual( a1, A(40/2) );
		}
		if constexpr( Simd::Has_Mul() )
		{
			const Simd	a1 = Simd{A(2)} * Simd{A(7)};	AllEqual( a1, A(2*7) );
			const Simd	a2 = Simd{A(2)} * A(3);			AllEqual( a2, A(2*3) );

			if constexpr( IsSigned<A> )
			{
				const Simd	b1 = Simd{A(2)} * A(-3);	AllEqual( b1, A(2*-3) );
			}
			if constexpr( IsUnsigned<A> )
			{
				const A		d1 = (A(UMax)/2)*2;
				const A		d2 = (A(UMax)/3)*2;
				const Simd	c1 = Simd{A(UMax)/2} * A(2);	AllEqual( c1, d1 );
				const Simd	c2 = Simd{A(UMax)/3} * A(2);	AllEqual( c2, d2 );
			}
		}

		const auto	Shift64b = [] (int shift)
		{{
			typename Shift64_t::Array_t	arr;
			arr.fill( 0 );
			arr[0] = shift;
			return Shift64_t{ arr.data() };
		}};
		const auto	ShiftVec = [] (uint shift0, uint shift1)
		{{
			using U = typename Simd::Unsigned_t::Scalar_t;
			typename Simd::Unsigned_t::Array_t	arr;
			arr.fill( U(shift1) );
			arr[0] = U(shift0);
			return typename Simd::Unsigned_t{ arr.data() };
		}};

		if constexpr( Simd::Has_ScalarShift_Arithmetic() )
		{
			if constexpr( IsSame< A, sshort >)
			{
				const Simd	a1 = Simd{A(-80)}.template RShift_Arith< 1 >();	// same as /2
				const Simd	a2 = Simd{A(-80)}.template RShift_Arith< 3 >();	// same as /8
				const Simd	a3 = Simd{A(-80)}.RShift_Arith( Shift64b( 1 ));
				const Simd	a4 = Simd{A(-8)}.template RShift_Arith< 3 >();
				const Simd	a5 = Simd{A(-8)}.template RShift_Arith< 4 >();

				AllEqual( a1, A(-40) );
				AllEqual( a2, A(-10) );
				AllEqual( a3, A(-40) );
				AllEqual( a4, A(-1) );
				AllEqual( a5, A(-1) );
			}
			if constexpr( IsSame< A, sint >)
			{
				const Simd	a1 = Simd{A(-8'000'000)}.template RShift_Arith< 1 >();	// same as /2
				const Simd	a2 = Simd{A(-8'000'000)}.template RShift_Arith< 3 >();	// same as /8
				const Simd	a3 = Simd{A(-8'000'000)}.RShift_Arith( Shift64b( 1 ));
				const Simd	a4 = Simd{A(-8)}.template RShift_Arith< 4 >();

				AllEqual( a1, A(-4'000'000) );
				AllEqual( a2, A(-1'000'000) );
				AllEqual( a3, A(-4'000'000) );
				AllEqual( a4, A(-1) );
			}
			if constexpr( IsSame< A, slong >)
			{
				const Simd	a1 = Simd{A(-8'000'000'000'000ll)}.template RShift_Arith< 1 >();	// same as /2
				const Simd	a2 = Simd{A(-8'000'000'000'000ll)}.template RShift_Arith< 3 >();	// same as /8
				const Simd	a3 = Simd{A(-8'000'000'000'000ll)}.RShift_Arith( Shift64b( 1 ));

				AllEqual( a1, A(-4'000'000'000'000ll) );
				AllEqual( a2, A(-1'000'000'000'000ll) );
				AllEqual( a3, A(-4'000'000'000'000ll) );
			}
			{
				const Simd	a1 = Simd{A(80)}.template RShift_Arith< 1 >();	// same as /2
				const Simd	a2 = Simd{A(80)}.template RShift_Arith< 3 >();	// same as /8
				const Simd	a3 = Simd{A(80)}.RShift_Arith( Shift64b( 1 ));
				const Simd	a4 = Simd{A(8)}.template RShift_Arith< 3 >();
				const Simd	a5 = Simd{A(8)}.template RShift_Arith< 4 >();

				AllEqual( a1, A(40) );
				AllEqual( a2, A(10) );
				AllEqual( a3, A(40) );
				AllEqual( a4, A(1) );
				AllEqual( a5, A(0) );
			}
		}
		if constexpr( Simd::Has_ScalarShift_Logic() )
		{
			const Simd	b1 = Simd{A(80)}.template RShift_Logic< 1 >();	// same as /2
			const Simd	b2 = Simd{A(80)}.template RShift_Logic< 3 >();	// same as /8
			const Simd	b3 = Simd{A(80)}.RShift_Logic( Shift64b( 1 ));
			const Simd	b4 = Simd{A(8)}.template RShift_Logic< 3 >();
			const Simd	b5 = Simd{A(8)}.template RShift_Logic< 4 >();

			const Simd	c1 = Simd{A(80)}.template LShift_Logic< 1 >();	// same as *2
			const Simd	c2 = Simd{A(80)}.template LShift_Logic< 3 >();	// same as *8
			const Simd	c3 = Simd{A(80)}.LShift_Logic( Shift64b( 1 ));

			AllEqual( b1, A(40) );
			AllEqual( b2, A(10) );
			AllEqual( b3, A(40) );
			AllEqual( b4, A(1) );
			AllEqual( b5, A(0) );

			AllEqual( c1, A(80*2) );
			AllEqual( c2, A(80*8) );
			AllEqual( c3, A(80<<1) );
		}

		if constexpr( Simd::Has_VecLShift_Logic() )
		{
			const Simd	a1 = Simd{A(80)}.LShift_LogicV( ShiftVec( 1, 2 ));

			TEST( a1.template get<0>() == A(80<<1) );
			TEST( a1.template get<1>() == A(80<<2) );
		}
		if constexpr( Simd::Has_VecRShift_Logic() )
		{
			const Simd	a1 = Simd{A(80)}.RShift_LogicV( ShiftVec( 1, 2 ));

			TEST( a1.template get<0>() == A(80>>1) );
			TEST( a1.template get<1>() == A(80>>2) );
		}
		if constexpr( Simd::Has_VecRShift_Arithmetic() )
		{
			const Simd	a1 = Simd{A(80)}.RShift_ArithV( ShiftVec( 1, 2 ));

			TEST( a1.template get<0>() == A(80/2) );
			TEST( a1.template get<1>() == A(80/4) );
		}
		if constexpr( Simd::Has_VecLShift_Arithmetic() )
		{
			const Simd	a1 = Simd{A(8)}.LShift_ArithV( ShiftVec( 1, 2 ));

			TEST( a1.template get<0>() == A(8*2) );
			TEST( a1.template get<1>() == A(8*4) );
		}
		if constexpr( IsSigned<A> and Simd::Has_VecRShift_Arithmetic() )
		{
			const Simd	a1 = Simd{A(-80)}.RShift_ArithV( ShiftVec( 1, 2 ));

			TEST( a1.template get<0>() == A(-80/2) );
			TEST( a1.template get<1>() == A(-80/4) );
		}
		if constexpr( IsSigned<A> and Simd::Has_VecLShift_Arithmetic() )
		{
			const Simd	a1 = Simd{A(-8)}.LShift_ArithV( ShiftVec( 1, 2 ));

			TEST( a1.template get<0>() == A(-8*2) );
			TEST( a1.template get<1>() == A(-8*4) );
		}

	#if 0 // with assert
		if constexpr( IsSame< A, sshort >)
		{
			const Simd	b0 = Simd{A( 80)}.RShift_Arith( Shift64b( -1 ));
			const Simd	b1 = Simd{A(-80)}.RShift_Arith( Shift64b( -1 ));
			AllEqual( b0, A(0) );
			AllEqual( b1, A(-1) );
		}
		if constexpr( IsSame< A, sint >)
		{
			const Simd	b0 = Simd{A( 8'000'000)}.RShift_Arith( Shift64b( -1 ));
			const Simd	b1 = Simd{A(-8'000'000)}.RShift_Arith( Shift64b( -1 ));
			AllEqual( b0, A(0) );
			AllEqual( b1, A(-1) );
		}
		if constexpr( IsSame< A, slong >)
		{
			const Simd	b0 = Simd{A( 8'000'000'000'000ll)}.RShift_Arith( Shift64b( -1 ));
			const Simd	b1 = Simd{A(-8'000'000'000'000ll)}.RShift_Arith( Shift64b( -1 ));
			AllEqual( b0, A(0) );
			AllEqual( b1, A(-1) );
		}
		if constexpr( sizeof(A) >= 2 )
		{
			const Simd	a1 = Simd{A(80)}.RShift_Logic( Shift64b( -1 ));
			AllEqual( a1, A(0) );
		}
	#endif

		if constexpr( Simd::Has_ReduceAdd() )
		{
			const auto	a1 = Simd{A(1)}.ReduceAddScalar();		TEST_Eq( a1, A(VecSize<Simd>) );
		}
		if constexpr( Simd::Has_ReduceAddExt() )
		{
			const auto	a1 = Simd{A(1)}.ReduceAddExtScalar();		TEST_Eq( a1, A(VecSize<Simd>) );
		}
		if constexpr( Simd::Has_ReduceMinMax() )
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = A(i+1);

			const auto	a1 = Simd{arr.data()}.ReduceMinScalar();	TEST( a1 == arr.front() );
			const auto	a2 = Simd{arr.data()}.ReduceMaxScalar();	TEST( a2 == arr.back() );
		}
		if constexpr( Simd::Has_InclusiveAdd() )
		{
			typename Simd::Array_t	arr;
			typename Simd::Array_t	scan;
			A						sum = A(0);
			for (usize i = 0; i < arr.size(); ++i) {
				arr[i]	= A(i+1);
				sum		+= arr[i];
				scan[i]	= sum;
			}

			const auto	a1 = Simd{arr.data()}.InclusiveAdd().ToArray();
			for (usize i = 0; i < scan.size(); ++i) {
				TEST_Eq( a1[i], scan[i] );
			}
		}
		if constexpr( Simd::Has_InclusiveAdd() )
		{
			typename Simd::Array_t	arr;
			typename Simd::Array_t	scan;
			A						sum = A(0);
			for (usize i = 0; i < arr.size(); ++i) {
				scan[i]	= sum;
				arr[i]	= A(i+1);
				sum		+= arr[i];
			}

			const auto	a1 = Simd{arr.data()}.ExclusiveAdd().ToArray();
			for (usize i = 0; i < scan.size(); ++i) {
				TEST( a1[i] == scan[i] );
			}
		}
	}


	template <typename Simd>
	static void  Test_FloatComparison ()
	{
		StaticAssert( Simd::Has_Equal() );

		using A = typename Simd::Scalar_t;
		using B = ScalarType< A >;

		typename Simd::Array_t	arr1;	arr1.fill( AB(10) );		arr1[0] = AB(1.0);	arr1[1] = AB(2.0);
		typename Simd::Array_t	arr2;	arr2.fill( AB(20) );		arr2[0] = AB(9.0);	arr2[1] = AB(8.0);
		typename Simd::Array_t	arr3;	arr3.fill( AB(30) );		arr3[0] = AB(1.0);	arr3[1] = AB(6.0);

		const Simd	aa{ arr1.data() };
		const Simd	bb{ arr2.data() };
		const Simd	ab{ arr3.data() };

		const auto	c0 = aa == aa;		TEST( c0.All() );		TEST( c0.Any() );		TEST( c0.ToBitfield().All() );	TEST( c0.ToBitfield().Any() );
		const auto	c1 = aa == ab;		TEST( c1.Any() );		TEST( not c1.All() );	TEST( c1.ToBitfield().Any() );	TEST( not c1.ToBitfield().All() );
		const auto	c2 = aa == bb;		TEST( c2.None() );								TEST( c2.ToBitfield().None() );
		const auto	c3 = not c0;		TEST( c3.None() );								TEST( c3.ToBitfield().None() );
		const auto	c4 = aa != ab;		TEST( c4.Any() );		TEST( not c4.All() );	TEST( c4.ToBitfield().Any() );	TEST( not c4.ToBitfield().All() );
		const auto	c5 = c1 & c4;		TEST( c5.None() );								TEST( c5.ToBitfield().None() );
		const auto	c6 = c1 | c4;		TEST( c6.All() );		TEST( c6.Any() );		TEST( c6.ToBitfield().All() );	TEST( c6.ToBitfield().Any() );
		const auto	c7 = c1 ^ c4;		TEST( c7.All() );		TEST( c7.Any() );		TEST( c7.ToBitfield().All() );	TEST( c7.ToBitfield().Any() );

		const Simd	d0 = aa.EqualF( aa );	TEST( B(d0.template get<0>()) == B(1.0) and B(d0.template get<1>()) == B(1.0) );

		if constexpr( Simd::Has_Greater() )
		{
			const auto	c8 = aa < bb;		TEST( c8.All() );		TEST( c8.Any() );
			const auto	c9 = aa > bb;		TEST( c9.None() );
			const auto	c10 = aa < ab;		TEST( c10.Any() );		TEST( not c10.All() );
			const auto	c11 = aa >= ab;		TEST( c11.Any() );		TEST( not c11.All() );

			const auto	d1 = aa.LessF( ab );	TEST( B(d1.template get<0>()) == B(0.0) and B(d1.template get<1>()) == B(1.0) );
		}

		const auto	m0 = c0.ToBitfield();
		for (uint i = 0; i < m0.Count(); ++i) {
			TEST( m0.get(i) );
		}

		const auto	m1 = c1.ToBitfield();
		TEST( m1.template get<0>() );
		TEST( not m1.template get<1>() );
	}


	template <typename Simd>
	static void  Test_IntComparison ()
	{
		StaticAssert( Simd::Has_Equal() );

		using A = typename Simd::Scalar_t;

		typename Simd::Array_t	arr1;	arr1.fill( A(10) );		arr1[0] = A(1);		arr1[1] = A(-2);
		typename Simd::Array_t	arr2;	arr2.fill( A(20) );		arr2[0] = A(9);		arr2[1] = A(8);
		typename Simd::Array_t	arr3;	arr3.fill( A(30) );		arr3[0] = A(1);		arr3[1] = A(6);

		const Simd	aa{ arr1.data() };
		const Simd	bb{ arr2.data() };
		const Simd	ab{ arr3.data() };

		const auto	c0 = aa == aa;			TEST( c0.All() );		TEST( c0.Any() );		TEST( c0.ToBitfield().All() );	TEST( c0.ToBitfield().Any() );
		const auto	c1 = aa == ab;			TEST( c1.Any() );		TEST( not c1.All() );	TEST( c1.ToBitfield().Any() );	TEST( not c1.ToBitfield().All() );
		const auto	c2 = aa == bb;			TEST( c2.None() );								TEST( c2.ToBitfield().None() );
		const auto	c3 = c0.BitInverse();	TEST( c3.None() );								TEST( c3.ToBitfield().None() );
		const auto	c4 = aa != ab;			TEST( c4.Any() );		TEST( not c4.All() );	TEST( c4.ToBitfield().Any() );	TEST( not c4.ToBitfield().All() );
		const auto	c5 = c1 & c4;			TEST( c5.None() );								TEST( c5.ToBitfield().None() );
		const auto	c6 = c1 | c4;			TEST( c6.All() );		TEST( c6.Any() );		TEST( c6.ToBitfield().All() );	TEST( c6.ToBitfield().Any() );
		const auto	c7 = c1 ^ c4;			TEST( c7.All() );		TEST( c7.Any() );		TEST( c7.ToBitfield().All() );	TEST( c7.ToBitfield().Any() );

		if constexpr( Simd::Has_Greater() )
		{
			const auto	c8 = aa < bb;			TEST( c8.All() );		TEST( c8.Any() );
			const auto	c9 = aa > bb;			TEST( c9.None() );
			const auto	c10 = aa < ab;			TEST( c10.Any() );		TEST( not c10.All() );
			const auto	c11 = aa >= ab;			TEST( c11.Any() );		TEST( not c11.All() );

			const auto	c12 = Simd{A(1)}  <  Simd{A(-1)};	TEST( c12.None() );
			const auto	c13 = Simd{A(1)}  >  Simd{A(-1)};	TEST( c13.All() );
			const auto	c14 = Simd{A(-1)} >= Simd{A(-1)};	TEST( c14.All() );
			const auto	c15 = Simd{A(-1)} <= Simd{A(-1)};	TEST( c15.All() );
		}

		const auto	m0 = c0.ToBitfield();
		for (uint i = 0; i < m0.Count(); ++i) {
			TEST( m0.get(i) );
		}

		const auto	m1 = c1.ToBitfield();
		TEST( m1.template get<0>() );
		TEST( not m1.template get<1>() );
	}


	template <typename Simd>
	static void  Test_UIntComparison ()
	{
		StaticAssert( Simd::Has_Equal() );

		using A = typename Simd::Scalar_t;

		typename Simd::Array_t	arr1;	arr1.fill( A(10) );		arr1[0] = A(1);		arr1[1] = A(2);
		typename Simd::Array_t	arr2;	arr2.fill( A(20) );		arr2[0] = A(9);		arr2[1] = A(8);
		typename Simd::Array_t	arr3;	arr3.fill( A(30) );		arr3[0] = A(1);		arr3[1] = A(6);

		const Simd	aa{ arr1.data() };
		const Simd	bb{ arr2.data() };
		const Simd	ab{ arr3.data() };

		const auto	c0 = aa == aa;			TEST( c0.All() );		TEST( c0.Any() );		TEST( c0.ToBitfield().All() );	TEST( c0.ToBitfield().Any() );
		const auto	c1 = aa == ab;			TEST( c1.Any() );		TEST( not c1.All() );	TEST( c1.ToBitfield().Any() );	TEST( not c1.ToBitfield().All() );
		const auto	c2 = aa == bb;			TEST( c2.None() );								TEST( c2.ToBitfield().None() );
		const auto	c3 = c0.BitInverse();	TEST( c3.None() );								TEST( c3.ToBitfield().None() );
		const auto	c4 = aa != ab;			TEST( c4.Any() );		TEST( not c4.All() );	TEST( c4.ToBitfield().Any() );	TEST( not c4.ToBitfield().All() );
		const auto	c5 = c1 & c4;			TEST( c5.None() );								TEST( c5.ToBitfield().None() );
		const auto	c6 = c1 | c4;			TEST( c6.All() );		TEST( c6.Any() );		TEST( c6.ToBitfield().All() );	TEST( c6.ToBitfield().Any() );
		const auto	c7 = c1 ^ c4;			TEST( c7.All() );		TEST( c7.Any() );		TEST( c7.ToBitfield().All() );	TEST( c7.ToBitfield().Any() );

		if constexpr( Simd::Has_Greater() )
		{
			const auto	c8 = aa < bb;			TEST( c8.All() );		TEST( c8.Any() );
			const auto	c9 = aa > bb;			TEST( c9.None() );
			const auto	c10 = aa < ab;			TEST( c10.Any() );		TEST( not c10.All() );
			const auto	c11 = aa >= ab;			TEST( c11.Any() );		TEST( not c11.All() );

			const auto	c12 = Simd{A(1)} <  Simd{A(UMax)};		TEST( c12.All() );
			const auto	c13 = Simd{A(1)} >  Simd{A(UMax)};		TEST( c13.None() );
			const auto	c14 = Simd{A(1)} >= Simd{A(UMax)};		TEST( c14.None() );
			const auto	c15 = Simd{A(1)} <= Simd{A(UMax)};		TEST( c15.All() );
			const auto	c16 = Simd{A(UMax)-1} < Simd{A(UMax)};	TEST( c16.All() );
		}

		const auto	m0 = c0.ToBitfield();
		for (uint i = 0; i < m0.Count(); ++i) {
			TEST( m0.get(i) );
		}

		const auto	m1 = c1.ToBitfield();
		TEST( m1.template get<0>() );
		TEST( not m1.template get<1>() );
	}


	template <typename Simd>
	static void  Test_FloatBitCast ()
	{
		using A = typename Simd::Scalar_t;
		using B = ScalarType< A >;

		if constexpr( Simd::template Has_BitCast< typename Simd::SimdUInt_t >() )
		{
			using U = typename Simd::SimdUInt_t::Scalar_t;

			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			Simd	v	{arr.data()};
			auto	u	= v.template BitCast< typename Simd::SimdUInt_t >();
			auto	res	= u.ToArray();

			for (usize i = 0; i < arr.size(); ++i)
			{
				auto	ref = BitCast<U>( arr[i] );
				TEST_Eq( ref, res[i] );
			}
		}
	}


	template <typename Dst, typename Src, typename DstArr, usize Cnt>
	static void  CompareConversion2 (const StaticArray<Src,Cnt> &srcArr, const DstArr &dstArr)
	{
		StaticAssert( HasSimdType< DstArr > or IsStaticArray< DstArr >);
		StaticAssert( IsAnyScalar< Dst >);
		StaticAssert( IsAnyScalar< Src >);

		if constexpr( HasSimdType< DstArr >)
			StaticAssert( IsSame< typename DstArr::Scalar_t, Dst >);

		if constexpr( IsStaticArray< DstArr >)
			StaticAssert( IsSame< typename DstArr::value_type::Scalar_t, Dst >);

		StaticArray<Dst,Cnt>	dst_arr;
		StaticAssert( sizeof(dstArr) >= sizeof(dst_arr) );
		std::memcpy( OUT dst_arr.data(), &dstArr, sizeof(dst_arr) );

		for (usize i = 0; i < Cnt; ++i)
		{
			const Src	src	= srcArr[i];
			Dst			ref;

			if constexpr( IsSame< Dst, half > and IsInteger< Src >)
			{
			  #if AE_SIMD_F16C or defined(AE_SIMD_NEON)
				ref = Dst( float(src) );
			  #else
				ref = Dst{}.SetFast( float(src) );
			  #endif
			}else{
				ref = Dst(src);
			}

			const auto	dst = dst_arr[i];

			if constexpr( IsSame< Dst, half >)
			{
				if ( ref.IsNaN() or ref.IsInfinity() ){
					TEST( dst == half::Max() or dst.IsNaN() or dst.IsInfinity() );
				}else
				if ( Abs(float(src)) > float(half::Max())*0.5f ){
					TEST( (ref.AsInteger() ^ dst.AsInteger()) <= 3 );	// allow 1-2 bit difference
				}else{
					TEST_Eq( ref, dst );
				}
			}else{
				TEST_Eq( ref, dst );
			}
		}
	}


	template <typename DstScalar, typename Src, typename Simd, usize Cnt>
	static void  CompareConversion (const StaticArray<Src,Cnt> &arr, const Simd &v)
	{
		if constexpr( Simd::template Has_Convert< DstScalar >() )
		{
			auto	cv = v.template Convert< DstScalar >();
			CompareConversion2< DstScalar >( arr, cv );
		}
	}


	template <typename Simd, typename A>
	static void  Test_TypeConversion ()
	{
		using B = ScalarType< A >;

		if constexpr( IsSame< A, float >)
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	v = Simd{arr.data()};

			CompareConversion<double>( arr, v );
			CompareConversion<half>	 ( arr, v );
			CompareConversion<int>	 ( arr, v );

			StaticAssert( not Simd::template Has_Convert<float>() );
		}

		if constexpr( IsSame< A, half >)
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	v = Simd{arr.data()};

			CompareConversion<float>( arr, v );

			StaticAssert( not Simd::template Has_Convert<half>() );
		}

		if constexpr( IsSame< A, double >)
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	v = Simd{arr.data()};

			CompareConversion<float>( arr, v );
			CompareConversion<int>	( arr, v );

			StaticAssert( not Simd::template Has_Convert<double>() );
		}

		if constexpr( IsInteger<A> )
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = AB(i+1);

			const auto	v = Simd{arr.data()};

			CompareConversion<float> ( arr, v );
			CompareConversion<double>( arr, v );
			CompareConversion<half>	 ( arr, v );
			CompareConversion<sbyte> ( arr, v );
			CompareConversion<ubyte> ( arr, v );
			CompareConversion<short> ( arr, v );
			CompareConversion<ushort>( arr, v );
			CompareConversion<int>	 ( arr, v );
			CompareConversion<uint>	 ( arr, v );
			CompareConversion<slong> ( arr, v );
			CompareConversion<ulong> ( arr, v );

			StaticAssert( not Simd::template Has_Convert<A>() );
		}

		if constexpr( IsSignedInteger<A> )
		{
			if constexpr( Simd::Has_MinMax() )
			{
				typename Simd::Unsigned_t	u = Simd{}.ToUnsigned();
				Unused( u );
			}
		}
		if constexpr( IsUnsignedInteger<A> )
		{
			if constexpr( Simd::Signed_t::Has_MinMax() )
			{
				typename Simd::Signed_t		s = Simd{}.ToSigned();
				Unused( s );
			}
		}

		if constexpr( IsSignedInteger<A> )
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = A(-B(i+1));

			const auto	v = Simd{arr.data()};

			CompareConversion<float> ( arr, v );
			CompareConversion<double>( arr, v );
			CompareConversion<half>	 ( arr, v );
			CompareConversion<sbyte> ( arr, v );
			CompareConversion<ubyte> ( arr, v );
			CompareConversion<short> ( arr, v );
			CompareConversion<ushort>( arr, v );
			CompareConversion<int>	 ( arr, v );
			CompareConversion<uint>	 ( arr, v );
			CompareConversion<slong> ( arr, v );
			CompareConversion<ulong> ( arr, v );
		}

		if constexpr( IsSignedInteger<A> )
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = A(MinValue<A>()+i);

			const auto	v = Simd{arr.data()};

			CompareConversion<float> ( arr, v );
			CompareConversion<double>( arr, v );
			CompareConversion<half>	 ( arr, v );
			CompareConversion<sbyte> ( arr, v );
			CompareConversion<ubyte> ( arr, v );
			CompareConversion<short> ( arr, v );
			CompareConversion<ushort>( arr, v );
			CompareConversion<int>	 ( arr, v );
			CompareConversion<uint>	 ( arr, v );
			CompareConversion<slong> ( arr, v );
			CompareConversion<ulong> ( arr, v );
		}

		if constexpr( IsInteger<A> )
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = A(0x843a'35e7'23b0'f72dull * (i+1));

			const auto	v = Simd{arr.data()};

			CompareConversion<float> ( arr, v );
			CompareConversion<double>( arr, v );
			CompareConversion<half>	 ( arr, v );
			CompareConversion<sbyte> ( arr, v );
			CompareConversion<ubyte> ( arr, v );
			CompareConversion<short> ( arr, v );
			CompareConversion<ushort>( arr, v );
			CompareConversion<int>	 ( arr, v );
			CompareConversion<uint>	 ( arr, v );
			CompareConversion<slong> ( arr, v );
			CompareConversion<ulong> ( arr, v );
		}

		if constexpr( IsUnsignedInteger<A> )
		{
			typename Simd::Array_t	arr;
			for (usize i = 0; i < arr.size(); ++i)
				arr[i] = A(MaxValue<A>()-i);

			const auto	v = Simd{arr.data()};

			CompareConversion<float> ( arr, v );
			CompareConversion<double>( arr, v );
			CompareConversion<half>	 ( arr, v );
			CompareConversion<sbyte> ( arr, v );
			CompareConversion<ubyte> ( arr, v );
			CompareConversion<short> ( arr, v );
			CompareConversion<ushort>( arr, v );
			CompareConversion<int>	 ( arr, v );
			CompareConversion<uint>	 ( arr, v );
			CompareConversion<slong> ( arr, v );
			CompareConversion<ulong> ( arr, v );
		}
	}


	template <typename Simd>
	static void  Test_All ()
	{
		using A = typename Simd::Scalar_t;
		using Bool_t = typename Simd::Bool_t;

		if constexpr( VecSize<Simd> == 2 )
			Test_Vec2<Simd>();
		else
		if constexpr( VecSize<Simd> == 4 )
			Test_Vec4<Simd>();
		else
		if constexpr( VecSize<Simd> == 8 )
			Test_Vec8<Simd>();
		else
		if constexpr( VecSize<Simd> == 16 )
			Test_Vec16<Simd>();
		else
		if constexpr( VecSize<Simd> == 32 )
			Test_Vec32<Simd>();
		else
			TEST( false );

		// parts
		{
			StaticAssert( Simd::parts > 0 );
			constexpr auto	l_size	= sizeof(Simd) / Simd::parts;

			auto	l0 = Simd{}.template Part<0>();
			StaticAssert( sizeof(l0) == l_size );

			if constexpr( Simd::parts > 1 )
			{
				auto	l1 = Simd{}.template Part<1>();
				StaticAssert( sizeof(l1) == l_size );
			}
			if constexpr( Simd::parts > 2 )
			{
				auto	l2 = Simd{}.template Part<2>();
				StaticAssert( sizeof(l2) == l_size );
			}
			if constexpr( Simd::parts > 3 )
			{
				auto	l3 = Simd{}.template Part<3>();
				StaticAssert( sizeof(l3) == l_size );
			}
		}

		Test_TypeConversion<Simd, A>();

		if constexpr( IsAnyFloatPoint<A> )
		{
			if constexpr( Simd::Has_Arithmetic() )
				Test_FloatArithmetic<Simd>();

			if constexpr( Simd::Has_Equal() )
				Test_FloatComparison<Simd>();

			Test_FloatBitCast<Simd>();

			if constexpr( Simd::Has_Rounding() )
				Test_FloatRound< Simd >();
		}
		else
		if constexpr( IsSignedInteger<A> )
		{
			if constexpr( Simd::Has_Arithmetic() )
				Test_IntArithmetic<Simd>();

			if constexpr( Simd::Has_Equal() )
				Test_IntComparison<Simd>();
		}
		else
		if constexpr( IsUnsignedInteger<A> )
		{
			if constexpr( Simd::Has_Arithmetic() )
				Test_IntArithmetic<Simd>();

			if constexpr( Simd::Has_Equal() )
				Test_UIntComparison<Simd>();
		}
		else
			TEST( false );
	}
//-----------------------------------------------------------------------------



	static void  Test_SimdFloat4 ()
	{
	#ifdef AE_SIMD_SimdFloat4
		StaticAssert( IsSame< UnwrapType<SimdFloat4>, float >);
		StaticAssert( not IsScalar< SimdFloat4 >);
		StaticAssert( VecSize<SimdFloat4> == 4 );
		StaticAssert( HasSimdType<SimdFloat4> );

		Test_All< SimdFloat4 >();
		{
			const SimdFloat4	v {1.f};
		  #ifdef AE_SIMD_SimdDouble2
			const SimdDouble2	a0	= v.ToDouble<0>();
			const SimdDouble2	a1	= v.ToDouble<1>();	Unused( a0, a1 );
		  #endif
		  #ifdef AE_SIMD_SimdTInt128
			const SimdInt4		a2	= v.ToInt();		Unused( a2 );
		  #endif
		  #ifdef AE_SIMD_SimdHalf4
			const SimdHalf4		a3	= v.ToHalf();		Unused( a3 );
		  #elif defined(AE_SIMD_SimdHalf8)
			const SimdHalf8		a3	= v.ToHalf();		Unused( a3 );
		  #endif
		}
		TEST_PASSED();

	#else
		StaticAssert( not HasSimdType<SimdFloat4> );
		AE_LOGI( "SimdFloat4 - not supported" );
	#endif
	}


	static void  Test_SimdDouble2 ()
	{
	#ifdef AE_SIMD_SimdDouble2
		StaticAssert( IsSame< UnwrapType<SimdDouble2>, double >);
		StaticAssert( not IsScalar< SimdDouble2 >);
		StaticAssert( VecSize<SimdDouble2> == 2 );
		StaticAssert( HasSimdType<SimdDouble2> );

		Test_All< SimdDouble2 >();
		{
			const SimdDouble2	v {1.0};
		  #ifdef AE_SIMD_SimdFloat4
			const SimdFloat4	a0	= v.ToFloat();		Unused( a0 );
		  #endif
		  #ifdef AE_SIMD_SimdTInt128
			const SimdInt4		a2	= SimdInt4{v.ToInt()};	Unused( a2 );
		  #endif
		}
		TEST_PASSED();

	#else
		StaticAssert( not HasSimdType<SimdDouble2> );
		AE_LOGI( "SimdDouble2 - not supported" );
	#endif
	}


	static void  Test_SimdByte8 ()
	{
	#ifdef AE_SIMD_SimdTInt64
		StaticAssert( IsSame< UnwrapType<SimdByte8>, sbyte >);
		StaticAssert( not IsScalar< SimdByte8 >);
		StaticAssert( VecSize<SimdByte8> == 8 );
		StaticAssert( HasSimdType<SimdByte8> );
		StaticAssert( HasSimdType<SimdUByte8> );

		Test_All< SimdByte8 >();
		Test_All< SimdUByte8 >();

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdByte8> );
		StaticAssert( not HasSimdType<SimdUByte8> );
		AE_LOGI( "SimdByte8, SimdUByte8 - not supported" );
	#endif
	}


	static void  Test_SimdShort4 ()
	{
	#ifdef AE_SIMD_SimdTInt64
		StaticAssert( IsSame< UnwrapType<SimdShort4>, sshort >);
		StaticAssert( not IsScalar< SimdShort4 >);
		StaticAssert( VecSize<SimdShort4> == 4 );
		StaticAssert( HasSimdType<SimdShort4> );
		StaticAssert( HasSimdType<SimdUShort4> );

		Test_All< SimdShort4 >();
		Test_All< SimdUShort4 >();

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdShort4> );
		StaticAssert( not HasSimdType<SimdUShort4> );
		AE_LOGI( "SimdShort4, SimdUShort4 - not supported" );
	#endif
	}


	static void  Test_SimdInt2 ()
	{
	#ifdef AE_SIMD_SimdTInt64
		StaticAssert( IsSame< UnwrapType<SimdUInt2>, uint >);
		StaticAssert( not IsScalar< SimdUInt2 >);
		StaticAssert( VecSize<SimdUInt2> == 2 );
		StaticAssert( HasSimdType<SimdInt2> );
		StaticAssert( HasSimdType<SimdUInt2> );

		Test_All< SimdInt2 >();
		Test_All< SimdUInt2 >();

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdInt2> );
		StaticAssert( not HasSimdType<SimdUInt2> );
		AE_LOGI( "SimdInt2, SimdUInt2 - not supported" );
	#endif
	}


	static void  Test_SimdByte16 ()
	{
	#ifdef AE_SIMD_SimdTInt128
		StaticAssert( IsSame< UnwrapType<SimdByte16>, sbyte >);
		StaticAssert( not IsScalar< SimdByte16 >);
		StaticAssert( VecSize<SimdByte16> == 16 );
		StaticAssert( HasSimdType<SimdByte16> );
		StaticAssert( HasSimdType<SimdUByte16> );

		Test_All< SimdByte16 >();
		Test_All< SimdUByte16 >();

		#if (AE_SIMD_SSE >= 41) or (AE_SIMD_NEON > 0)
		{
			const SimdByte16	v {sbyte(1)};
		  #ifdef AE_SIMD_SimdFloat4
			const SimdFloat4	a0 = v.ToFloat<0>();
			const SimdFloat4	a1 = v.ToFloat<1>();
			const SimdFloat4	a2 = v.ToFloat<2>();
			const SimdFloat4	a3 = v.ToFloat<3>();		Unused( a0, a1, a2, a3 );
		  #endif
		  #ifdef AE_SIMD_SimdDouble2
			const SimdDouble2	b0 = v.ToDouble<0>();
			const SimdDouble2	b1 = v.ToDouble<1>();
			const SimdDouble2	b2 = v.ToDouble<2>();
			const SimdDouble2	b3 = v.ToDouble<3>();
			const SimdDouble2	b4 = v.ToDouble<4>();
			const SimdDouble2	b5 = v.ToDouble<5>();
			const SimdDouble2	b6 = v.ToDouble<6>();
			const SimdDouble2	b7 = v.ToDouble<7>();		Unused( b0, b1, b2, b3, b4, b5, b6, b7 );
		  #endif
			const SimdShort8	c0 = v.ToShort<0>();
			const SimdShort8	c1 = v.ToShort<1>();		Unused( c0, c1 );
			const SimdInt4		d0 = v.ToInt<0>();
			const SimdInt4		d1 = v.ToInt<1>();
			const SimdInt4		d2 = v.ToInt<2>();
			const SimdInt4		d3 = v.ToInt<3>();			Unused( d0, d1, d2, d3 );
			const SimdLong2		e0 = v.ToLong<0>();
			const SimdLong2		e1 = v.ToLong<1>();
			const SimdLong2		e2 = v.ToLong<2>();
			const SimdLong2		e3 = v.ToLong<3>();
			const SimdLong2		e4 = v.ToLong<4>();
			const SimdLong2		e5 = v.ToLong<5>();
			const SimdLong2		e6 = v.ToLong<6>();
			const SimdLong2		e7 = v.ToLong<7>();			Unused( e0, e1, e2, e3, e4, e5, e6, e7 );
		}
		#endif

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdByte16> );
		StaticAssert( not HasSimdType<SimdUByte16> );
		AE_LOGI( "SimdByte16, SimdUByte16 - not supported" );
	#endif
	}


	static void  Test_SimdShort8 ()
	{
	#ifdef AE_SIMD_SimdTInt128
		StaticAssert( IsSame< UnwrapType<SimdShort8>, sshort >);
		StaticAssert( not IsScalar< SimdShort8 >);
		StaticAssert( VecSize<SimdShort8> == 8 );
		StaticAssert( HasSimdType<SimdShort8> );
		StaticAssert( HasSimdType<SimdUShort8> );

		Test_All< SimdShort8 >();
		Test_All< SimdUShort8 >();

		#if (AE_SIMD_SSE >= 41) or (AE_SIMD_NEON > 0)
		{
			const SimdShort8	v {short(1)};
		  #ifdef AE_SIMD_SimdFloat4
			const SimdFloat4	a0 = v.ToFloat<0>();
			const SimdFloat4	a1 = v.ToFloat<1>();		Unused( a0, a1 );
		  #endif
		  #ifdef AE_SIMD_SimdDouble2
			const SimdDouble2	b0 = v.ToDouble<0>();
			const SimdDouble2	b1 = v.ToDouble<1>();
			const SimdDouble2	b2 = v.ToDouble<2>();
			const SimdDouble2	b3 = v.ToDouble<3>();		Unused( b0, b1, b2, b3 );
		  #endif
			const SimdInt4		d0 = v.ToInt<0>();
			const SimdInt4		d1 = v.ToInt<1>();			Unused( d0, d1 );
			const SimdLong2		e0 = v.ToLong<0>();
			const SimdLong2		e1 = v.ToLong<1>();
			const SimdLong2		e2 = v.ToLong<2>();
			const SimdLong2		e3 = v.ToLong<3>();			Unused( e0, e1, e2, e3 );
		}
		#endif

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdShort8> );
		StaticAssert( not HasSimdType<SimdUShort8> );
		AE_LOGI( "SimdShort8, SimdUShort8 - not supported" );
	#endif
	}


	static void  Test_SimdInt4 ()
	{
	#ifdef AE_SIMD_SimdTInt128
		StaticAssert( IsSame< UnwrapType<SimdUInt4>, uint >);
		StaticAssert( not IsScalar< SimdUInt4 >);
		StaticAssert( VecSize<SimdUInt4> == 4 );
		StaticAssert( HasSimdType<SimdInt4> );
		StaticAssert( HasSimdType<SimdUInt4> );

		Test_All< SimdInt4 >();
		Test_All< SimdUInt4 >();
		{
			const SimdInt4	v {int(1)};
		  #ifdef AE_SIMD_SimdFloat4
			const SimdFloat4	a0 = v.ToFloat<0>();		Unused( a0 );
		  #endif
		  #ifdef AE_SIMD_SimdDouble2
			const SimdDouble2	b0 = v.ToDouble<0>();
			const SimdDouble2	b1 = v.ToDouble<1>();		Unused( b0, b1 );
		  #endif
		  #if (AE_SIMD_SSE >= 41) or (AE_SIMD_NEON > 0)
			const SimdLong2		e0 = v.ToLong<0>();
			const SimdLong2		e1 = v.ToLong<1>();			Unused( e0, e1 );
		  #endif
		}
		// arithmetic
		/*{
			const auto [b0, b1]	= SimdUInt4{1, 2, 3, 4}.MulExt( SimdUInt4{2, 2, 3, 3} );
			TEST(All( packed_ulong2{b0} == packed_ulong2{2, 4} ));
			TEST(All( packed_ulong2{b1} == packed_ulong2{9, 12} ));
		}*/

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdInt4> );
		StaticAssert( not HasSimdType<SimdUInt4> );
		AE_LOGI( "SimdInt4, SimdUInt4 - not supported" );
	#endif
	}


	static void  Test_SimdLong2 ()
	{
	#ifdef AE_SIMD_SimdTInt128
		StaticAssert( IsSame< UnwrapType<SimdULong2>, ulong >);
		StaticAssert( not IsScalar< SimdULong2 >);
		StaticAssert( VecSize<SimdULong2> == 2 );
		StaticAssert( HasSimdType<SimdLong2> );
		StaticAssert( HasSimdType<SimdULong2> );

		Test_All< SimdLong2 >();
		Test_All< SimdULong2 >();

		// set/get
		{
			const ulong		x = 0x1234'5678'9011'2233ull;
			const ulong		y = 0x1928'3746'5263'7485ull;

			const SimdULong2	a1{ x, y };
			TEST_Eq( a1.get<0>(), x );
			TEST_Eq( a1.get<1>(), y );

			const SimdULong2	a3 = a1.Swizzle<0,1>();
			const SimdULong2	a4 = a1.Swizzle<1,0>();
			const SimdULong2	a5 = a1.Swizzle<1,1>();

			TEST(All( packed_ulong2{a3} == packed_ulong2{x, y} ));
			TEST(All( packed_ulong2{a4} == packed_ulong2{y, x} ));
			TEST(All( packed_ulong2{a5} == packed_ulong2{y, y} ));
		}

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdLong2> );
		StaticAssert( not HasSimdType<SimdULong2> );
		AE_LOGI( "SimdLong2, SimdULong2 - not supported" );
	#endif
	}


	static void  Test_SimdFloat8 ()
	{
	#ifdef AE_SIMD_SimdFloat8
		StaticAssert( IsSame< UnwrapType<SimdFloat8>, float >);
		StaticAssert( not IsScalar< SimdFloat8 >);
		StaticAssert( VecSize<SimdFloat8> == 8 );
		StaticAssert( HasSimdType<SimdFloat8> );

		Test_All< SimdFloat8 >();
		{
			const SimdFloat8	v {1.f};
		  #ifdef AE_SIMD_SimdHalf8
			const SimdHalf8		a0 = v.ToHalf();			Unused( a0 );
		  #endif
		  #ifdef AE_SIMD_SimdTInt256
			const SimdInt8		a1 = v.ToInt();				Unused( a1 );
		  #endif
			const SimdDouble4	a2 = v.ToDouble<0>();
			const SimdDouble4	a3 = v.ToDouble<1>();		Unused( a2, a3 );
		}

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdFloat8> );
		AE_LOGI( "SimdFloat8 - not supported" );
	#endif
	}


	static void  Test_SimdDouble4 ()
	{
	#ifdef AE_SIMD_SimdDouble4
		StaticAssert( IsSame< UnwrapType<SimdDouble4>, double >);
		StaticAssert( not IsScalar< SimdDouble4 >);
		StaticAssert( VecSize<SimdDouble4> == 4 );
		StaticAssert( HasSimdType<SimdDouble4> );

		Test_All< SimdDouble4 >();
		{
			const SimdDouble4	v {1.0};
		  #ifdef AE_SIMD_SimdFloat4
			const SimdFloat4	a0 = v.ToFloat();		Unused( a0 );
		  #endif
		  #ifdef AE_SIMD_SimdTInt256
			const SimdInt4		a1 = v.ToInt();			Unused( a1 );
		  #endif
		}

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdDouble4> );
		AE_LOGI( "SimdDouble4 - not supported" );
	#endif
	}


	static void  Test_SimdByte32 ()
	{
	#ifdef AE_SIMD_SimdTInt256
		StaticAssert( IsSame< UnwrapType<SimdByte32>, sbyte >);
		StaticAssert( not IsScalar< SimdByte32 >);
		StaticAssert( VecSize<SimdByte32> == 32 );
		StaticAssert( HasSimdType<SimdByte32> );
		StaticAssert( HasSimdType<SimdUByte32> );

		Test_All< SimdByte32 >();
		Test_All< SimdUByte32 >();

		#if AE_SIMD_AVX >= 2
		{
			const SimdByte32	v {sbyte(1)};
		  #ifdef AE_SIMD_SimdFloat8
			const SimdFloat8	a0 = v.ToFloat<0>();
			const SimdFloat8	a1 = v.ToFloat<1>();
			const SimdFloat8	a2 = v.ToFloat<2>();
			const SimdFloat8	a3 = v.ToFloat<3>();		Unused( a0, a1, a2, a3 );
		  #endif
		  #ifdef AE_SIMD_SimdDouble4
			const SimdDouble4	b0 = v.ToDouble<0>();
			const SimdDouble4	b1 = v.ToDouble<1>();
			const SimdDouble4	b2 = v.ToDouble<2>();
			const SimdDouble4	b3 = v.ToDouble<3>();
			const SimdDouble4	b4 = v.ToDouble<4>();
			const SimdDouble4	b5 = v.ToDouble<5>();
			const SimdDouble4	b6 = v.ToDouble<6>();
			const SimdDouble4	b7 = v.ToDouble<7>();		Unused( b0, b1, b2, b3, b4, b5, b6, b7 );
		  #endif
			const SimdShort16	c0 = v.ToShort<0>();
			const SimdShort16	c1 = v.ToShort<1>();		Unused( c0, c1 );
			const SimdInt8		d0 = v.ToInt<0>();
			const SimdInt8		d1 = v.ToInt<1>();
			const SimdInt8		d2 = v.ToInt<2>();
			const SimdInt8		d3 = v.ToInt<3>();			Unused( d0, d1, d2, d3 );
			const SimdLong4		e0 = v.ToLong<0>();
			const SimdLong4		e1 = v.ToLong<1>();
			const SimdLong4		e2 = v.ToLong<2>();
			const SimdLong4		e3 = v.ToLong<3>();
			const SimdLong4		e4 = v.ToLong<4>();
			const SimdLong4		e5 = v.ToLong<5>();
			const SimdLong4		e6 = v.ToLong<6>();
			const SimdLong4		e7 = v.ToLong<7>();			Unused( e0, e1, e2, e3, e4, e5, e6, e7 );
		}
		#endif

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdByte32> );
		StaticAssert( not HasSimdType<SimdUByte32> );
		AE_LOGI( "SimdByte32, SimdUByte32 - not supported" );
	#endif
	}


	static void  Test_SimdShort16 ()
	{
	#ifdef AE_SIMD_SimdTInt256
		StaticAssert( IsSame< UnwrapType<SimdShort16>, sshort >);
		StaticAssert( not IsScalar< SimdShort16 >);
		StaticAssert( VecSize<SimdShort16> == 16 );
		StaticAssert( HasSimdType<SimdShort16> );
		StaticAssert( HasSimdType<SimdUShort16> );

		Test_All< SimdShort16 >();
		Test_All< SimdUShort16 >();

		#if AE_SIMD_AVX >= 2
		{
			const SimdShort16	v {short(1)};
		  #ifdef AE_SIMD_SimdFloat8
			const SimdFloat8	a0 = v.ToFloat<0>();
			const SimdFloat8	a1 = v.ToFloat<1>();		Unused( a0, a1 );
		  #endif
		  #ifdef AE_SIMD_SimdDouble4
			const SimdDouble4	b0 = v.ToDouble<0>();
			const SimdDouble4	b1 = v.ToDouble<1>();
			const SimdDouble4	b2 = v.ToDouble<2>();
			const SimdDouble4	b3 = v.ToDouble<3>();		Unused( b0, b1, b2, b3 );
		  #endif
			const SimdInt8		d0 = v.ToInt<0>();
			const SimdInt8		d1 = v.ToInt<1>();			Unused( d0, d1 );
			const SimdLong4		e0 = v.ToLong<0>();
			const SimdLong4		e1 = v.ToLong<1>();
			const SimdLong4		e2 = v.ToLong<2>();
			const SimdLong4		e3 = v.ToLong<3>();			Unused( e0, e1, e2, e3 );
		}
		#endif

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdShort16> );
		StaticAssert( not HasSimdType<SimdUShort16> );
		AE_LOGI( "SimdShort16, SimdUShort16 - not supported" );
	#endif
	}


	static void  Test_SimdInt8 ()
	{
	#ifdef AE_SIMD_SimdTInt256
		StaticAssert( IsSame< UnwrapType<SimdUInt8>, uint >);
		StaticAssert( not IsScalar< SimdUInt8 >);
		StaticAssert( VecSize<SimdUInt8> == 8 );
		StaticAssert( HasSimdType<SimdInt8> );
		StaticAssert( HasSimdType<SimdUInt8> );

		Test_All< SimdInt8 >();
		Test_All< SimdUInt8 >();
		{
			const SimdInt8		v {int(1)};
		  #ifdef AE_SIMD_SimdFloat8
			const SimdFloat8	a0 = v.ToFloat<0>();		Unused( a0 );
		  #endif
		  #ifdef AE_SIMD_SimdDouble4
			const SimdDouble4	b0 = v.ToDouble<0>();
			const SimdDouble4	b1 = v.ToDouble<1>();		Unused( b0, b1 );
		  #endif
		  #if AE_SIMD_AVX >= 2
			const SimdLong4		e0 = v.ToLong<0>();
			const SimdLong4		e1 = v.ToLong<1>();			Unused( e0, e1 );
		  #endif
		}

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdInt8> );
		StaticAssert( not HasSimdType<SimdUInt8> );
		AE_LOGI( "SimdInt8, SimdUInt8 - not supported" );
	#endif
	}


	static void  Test_SimdLong4 ()
	{
	#ifdef AE_SIMD_SimdTInt256
		StaticAssert( IsSame< UnwrapType<SimdULong4>, ulong >);
		StaticAssert( not IsScalar< SimdULong4 >);
		StaticAssert( VecSize<SimdULong4> == 4 );
		StaticAssert( HasSimdType<SimdLong4> );
		StaticAssert( HasSimdType<SimdULong4> );

		Test_All< SimdLong4 >();
		Test_All< SimdULong4 >();

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdLong4> );
		StaticAssert( not HasSimdType<SimdULong4> );
		AE_LOGI( "SimdLong4, SimdULong4 - not supported" );
	#endif
	}


	static void  Test_SimdHalf4 ()
	{
	#ifdef AE_SIMD_SimdHalf4
		StaticAssert( IsSame< UnwrapType<SimdHalf4>, half >);
		StaticAssert( not IsScalar< SimdHalf4 >);
		StaticAssert( VecSize<SimdHalf4> == 4 );
		StaticAssert( HasSimdType<SimdHalf4> );

		Test_All< SimdHalf4 >();

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdHalf4> );
		AE_LOGI( "SimdHalf4 - not supported" );
	#endif
	}


	static void  Test_SimdHalf8 ()
	{
	#ifdef AE_SIMD_SimdHalf8
		StaticAssert( IsSame< UnwrapType<SimdHalf8>, half >);
		StaticAssert( not IsScalar< SimdHalf8 >);
		StaticAssert( VecSize<SimdHalf8> == 8 );
		StaticAssert( HasSimdType<SimdHalf8> );

		Test_All< SimdHalf8 >();
		{
			SimdHalf8	a0{ 1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f };
			SimdHalf8	a1{ SimdFloat4{ 1.1f, 2.2f, 3.3f, 4.4f }, SimdFloat4{ 5.5f, 6.6f, 7.7f, 8.8f }};

			auto		b0	= a0.ToFloat<0>();
			auto		b1	= a0.ToFloat<1>();
			auto		b2	= a1.ToFloat<0>();
			auto		b3	= a1.ToFloat<1>();
			const auto	acc	= EnabledBitCount(19);

			TEST( All(BitEqual( packed_float4{b0}, packed_float4{1.1f, 2.2f, 3.3f, 4.4f}, acc )));
			TEST( All(BitEqual( packed_float4{b1}, packed_float4{5.5f, 6.6f, 7.7f, 8.8f}, acc )));
			TEST( All(BitEqual( packed_float4{b2}, packed_float4{1.1f, 2.2f, 3.3f, 4.4f}, acc )));
			TEST( All(BitEqual( packed_float4{b3}, packed_float4{5.5f, 6.6f, 7.7f, 8.8f}, acc )));
		}
		TEST_PASSED();

	#else
		StaticAssert( not HasSimdType<SimdHalf8> );
		AE_LOGI( "SimdHalf8 - not supported" );
	#endif
	}


	static void  Test_SimdHalf16 ()
	{
	#ifdef AE_SIMD_SimdHalf16
		StaticAssert( IsSame< UnwrapType<SimdHalf16>, half >);
		StaticAssert( not IsScalar< SimdHalf16 >);
		StaticAssert( VecSize<SimdHalf16> == 16 );
		StaticAssert( HasSimdType<SimdHalf16> );

		Test_All< SimdHalf16 >();

		TEST_PASSED();
	#else
		StaticAssert( not HasSimdType<SimdHalf16> );
		AE_LOGI( "SimdHalf16 - not supported" );
	#endif
	}


	static void  Test_FloatConversion ()
	{
	#if defined(AE_SIMD_SimdFloat4) and defined(AE_SIMD_SimdTInt128)

		const float	max  = float(half::Max());

		const auto	Test = [] (float f0, float f1, float f2, float f3)
		{{
			SimdFloat4		a		{ f0, f1, f2, f3 };
			SimdUShort8		b		= SimdFloatConversion::FloatToHalf( a );
			const auto		c		= b.ToArray();
			const ushort	ref[]	= { half{}.SetFast(a.get<0>()).AsInteger(), half{}.SetFast(a.get<1>()).AsInteger(),
										half{}.SetFast(a.get<2>()).AsInteger(), half{}.SetFast(a.get<3>()).AsInteger() };

			CHECK_Eq( c[0], ref[0] );
			CHECK_Eq( c[1], ref[1] );
			CHECK_Eq( c[2], ref[2] );
			CHECK_Eq( c[3], ref[3] );

			const auto	acc = 2.0e-5f;
			float	h0 = BitCast<half>(c[0]).GetFast();
			float	h1 = BitCast<half>(c[1]).GetFast();
			float	h2 = BitCast<half>(c[2]).GetFast();
			float	h3 = BitCast<half>(c[3]).GetFast();

			CHECK( Equal( h0, f0, acc ) or Equal( h0, f0, 1_pct ));
			CHECK( Equal( h1, f1, acc ) or Equal( h1, f1, 1_pct ));
			CHECK( Equal( h2, f2, acc ) or Equal( h2, f2, 1_pct ));
			CHECK( Equal( h3, f3, acc ) or Equal( h3, f3, 1_pct ));
		}};

		for (float f = 0.f, s = 2.0e-5f; f < 0.1f; f += s * 4.f)
		{
			Test( f, f+s, f+s*2.f, f+s*3.f );
			Test( -f, -(f+s), -(f+s*2.f), -(f+s*3.f) );
		}

		for (float f = 0.1f, s = 0.2f; f < max; f += s * 4.f)
		{
			Test( f, f+s, f+s*2.f, f+s*3.f );
			Test( -f, -(f+s), -(f+s*2.f), -(f+s*3.f) );
		}

		for (uint i = 0; i < 0xFFFF; i+=4)
		{
			SimdUShort8		a		{ ushort(i), ushort(i+1), ushort(i+2), ushort(i+3), 0, 0, 0, 0 };
			SimdFloat4		b		= SimdFloatConversion::HalfToFloat( a );
			const auto		f		= b.ToArray();
			const auto		h		= a.ToArray();
			const float		ref[]	= { BitCast<half>(h[0]).GetFast(), BitCast<half>(h[1]).GetFast(),
										BitCast<half>(h[2]).GetFast(), BitCast<half>(h[3]).GetFast() };

			CHECK( BitCast<uint>(f[0]) == BitCast<uint>(ref[0]) );
			CHECK( BitCast<uint>(f[1]) == BitCast<uint>(ref[1]) );
			CHECK( BitCast<uint>(f[2]) == BitCast<uint>(ref[2]) );
			CHECK( BitCast<uint>(f[3]) == BitCast<uint>(ref[3]) );
		}

		// nan, inf
		{
			SimdFloat4		a		{ Float32Bits::NaN().AsFloatPoint(), Float32Bits::Inf().AsFloatPoint(),
									  Float32Bits::NegInf().AsFloatPoint(), Float32Bits::SmallestSubnormal().AsFloatPoint() };
			SimdUShort8		b		= SimdFloatConversion::FloatToHalf( a );
			const auto		c		= b.ToArray();
			const ushort	ref[]	= { half{}.SetFast(a.get<0>()).AsInteger(), half{}.SetFast(a.get<1>()).AsInteger(),
										half{}.SetFast(a.get<2>()).AsInteger(), half{}.SetFast(a.get<3>()).AsInteger() };

			CHECK_Eq( c[0], ref[0] );
			CHECK_Eq( c[1], ref[1] );
			CHECK_Eq( c[2], ref[2] );
			CHECK_Eq( c[3], ref[3] );
		}

		TEST_PASSED();
	#endif
	}


	static void  Test_SimdRuntimeConfig ()
	{
	#if defined(AE_SIMD_SimdFloat4) and defined(AE_SimdRuntimeConfig) and defined(AE_CFG_DEBUG)
		const SimdRuntimeConfig::State	prev_state = SimdRuntimeConfig::GetState();

		SimdRuntimeConfig::DenormalFlushToZero( true );

		TEST( SimdRuntimeConfig::GetState().DenormalFlushToZero() );

		// scalar
		{
			float	a {Float32Bits::SmallestNormal().AsFloatPoint()};
			TEST( not Float32Bits{a}.IsSubnormal() );

			for (;;)
			{
				a = a * 0.5f;

				Float32Bits	b {a};

				if ( b.IsZero() )
					break;

				TEST( not b.IsSubnormal() );
			}
		}

		// vector
		{
			SimdFloat4	a {Float32Bits::SmallestNormal().AsFloatPoint()};
			TEST( not Float32Bits{a.get<0>()}.IsSubnormal() );

			for (;;)
			{
				a = a * 0.5f;

				Float32Bits	b {a.get<0>()};

				if ( b.IsZero() )
					break;

				TEST( not b.IsSubnormal() );
			}
		}

		SimdRuntimeConfig::DenormalFlushToZero( false );

		TEST( not SimdRuntimeConfig::GetState().DenormalFlushToZero() );

		// scalar
		{
			float	a {Float32Bits::SmallestNormal().AsFloatPoint()};
			for (;;)
			{
				a = a * 0.5f;

				Float32Bits	b {a};
				TEST( not b.IsZero() );

				if ( b.IsSubnormal() )
					break;
			}
		}

		// vector
		{
			SimdFloat4	a {Float32Bits::SmallestNormal().AsFloatPoint()};
			for (;;)
			{
				a = a * 0.5f;

				Float32Bits	b {a.get<0>()};
				TEST( not b.IsZero() );

				if ( b.IsSubnormal() )
					break;
			}
		}

		#if 0
			SimdRuntimeConfig::ResetExceptionState();
			SimdRuntimeConfig::ExceptionMask( Default );	// enable all
			{
				SimdFloat4	a {Float32Bits::SmallestNormal().AsFloatPoint()};
				a = a * 0.5f;		// throw exception
			}
		#endif

		SimdRuntimeConfig::SetState( prev_state );

		TEST_PASSED();
	#endif
	}
}


extern void UnitTest_Math_SIMD ()
{
	Test_FloatConversion();
	Test_SimdRuntimeConfig();

	Test_SimdInt2();
	Test_SimdShort4();
	Test_SimdByte8();

	Test_SimdFloat4();
	Test_SimdInt4();
	Test_SimdDouble2();
	Test_SimdLong2();
	Test_SimdShort8();
	Test_SimdByte16();

	Test_SimdFloat8();
	Test_SimdInt8();
	Test_SimdDouble4();
	Test_SimdLong4();
	Test_SimdShort16();
	Test_SimdByte32();

	Test_SimdHalf4();
	Test_SimdHalf8();
	Test_SimdHalf16();

	// TODO: AVX512

	TEST_PASSED();
}
