// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  MatrixStorage_Test1 ()
	{
		// 4 columns, 2 rows
		using CMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::ColumnMajor >;
		using RMat4x2_t = MatrixStorage< float, 2, 4, EMatrixOrder::RowMajor >;

		StaticAssert( VecSize<CMat4x2_t::Column_t> == 2 );
		StaticAssert( VecSize<CMat4x2_t::Row_t> == 4 );
		StaticAssert( sizeof(CMat4x2_t) == sizeof(float) * 4 * 2 );

		StaticAssert( VecSize<RMat4x2_t::Column_t> == 2 );
		StaticAssert( VecSize<RMat4x2_t::Row_t> == 4 );
		StaticAssert( sizeof(RMat4x2_t) == sizeof(float) * 4 * 2 );

		StaticAssert( CMat4x2_t::Dimension().columns == 4 );
		StaticAssert( CMat4x2_t::Dimension().rows == 2 );
		StaticAssert( CMat4x2_t::Dimension() == RMat4x2_t::Dimension() );
	};


	static void  MatrixStorage_Test2 ()
	{
		// 4 columns, 2 rows
		using CMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::ColumnMajor, sizeof(float)*4 >;
		using RMat4x2_t = MatrixStorage< float, 2, 4, EMatrixOrder::RowMajor, sizeof(float)*4 >;

		StaticAssert( VecSize<CMat4x2_t::Column_t> == 2 );
		StaticAssert( VecSize<CMat4x2_t::Row_t> == 4 );
		StaticAssert( sizeof(CMat4x2_t) == sizeof(float) * 4 * 4 );

		StaticAssert( VecSize<RMat4x2_t::Column_t> == 2 );
		StaticAssert( VecSize<RMat4x2_t::Row_t> == 4 );
		StaticAssert( sizeof(RMat4x2_t) == sizeof(float) * 4 * 2 );

		StaticAssert( CMat4x2_t::Dimension().columns == 4 );
		StaticAssert( CMat4x2_t::Dimension().rows == 2 );
		StaticAssert( CMat4x2_t::Dimension() == RMat4x2_t::Dimension() );
	};


	static void  MatrixStorage_Test3 ()
	{
		// 4 columns, 3 rows
		using CMat4x3_t = MatrixStorage< float, 4, 3, EMatrixOrder::ColumnMajor >;
		using RMat4x3_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor >;

		StaticAssert( VecSize<CMat4x3_t::Column_t> == 3 );
		StaticAssert( VecSize<CMat4x3_t::Row_t> == 4 );
		StaticAssert( sizeof(CMat4x3_t) == sizeof(float) * 4 * 3 );

		StaticAssert( VecSize<RMat4x3_t::Column_t> == 3 );
		StaticAssert( VecSize<RMat4x3_t::Row_t> == 4 );
		StaticAssert( sizeof(RMat4x3_t) == sizeof(float) * 4 * 3 );

		StaticAssert( CMat4x3_t::Dimension().columns == 4 );
		StaticAssert( CMat4x3_t::Dimension().rows == 3 );
		StaticAssert( CMat4x3_t::Dimension() == RMat4x3_t::Dimension() );
	};


	static void  MatrixStorage_Test4 ()
	{
		// 4 columns, 3 rows
		using CMat4x3_t = MatrixStorage< float, 4, 3, EMatrixOrder::ColumnMajor, sizeof(float)*4 >;
		using RMat4x3_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor, sizeof(float)*4 >;

		StaticAssert( VecSize<CMat4x3_t::Column_t> == 3 );
		StaticAssert( VecSize<CMat4x3_t::Row_t> == 4 );
		StaticAssert( sizeof(CMat4x3_t) == sizeof(float) * 4 * 4 );

		StaticAssert( VecSize<RMat4x3_t::Column_t> == 3 );
		StaticAssert( VecSize<RMat4x3_t::Row_t> == 4 );
		StaticAssert( sizeof(RMat4x3_t) == sizeof(float) * 4 * 3 );

		StaticAssert( CMat4x3_t::Dimension().columns == 4 );
		StaticAssert( CMat4x3_t::Dimension().rows == 3 );
		StaticAssert( CMat4x3_t::Dimension() == RMat4x3_t::Dimension() );
	};


	static void  MatrixStorage_Test5 ()
	{
		// 3 columns with float4
		using float3x4_t = MatrixStorage< float, 3, 4, EMatrixOrder::ColumnMajor >;

		const float3x4_t  m1{};
		Unused( m1 );

		const float3x4_t  m2{ float4(1.0f, 2.0f, 3.0f, 4.0f),
							  float4(5.0f, 6.0f, 7.0f, 8.0f),
							  float4(9.0f, 10.0f, 11.0f, 12.0f) };

		TEST( All( m2.get<0>() == float4(1.0f, 2.0f, 3.0f, 4.0f) ));
		TEST( All( m2.get<1>() == float4(5.0f, 6.0f, 7.0f, 8.0f) ));
		TEST( All( m2.get<2>() == float4(9.0f, 10.0f, 11.0f, 12.0f) ));

		const float3x4_t  m3{ 1.0f, 2.0f, 3.0f, 4.0f,
							  5.0f, 6.0f, 7.0f, 8.0f,
							  9.0f, 10.0f, 11.0f, 12.0f };

		TEST( All( m3.get<0>() == float4(1.0f, 2.0f, 3.0f, 4.0f) ));
		TEST( All( m3.get<1>() == float4(5.0f, 6.0f, 7.0f, 8.0f) ));
		TEST( All( m3.get<2>() == float4(9.0f, 10.0f, 11.0f, 12.0f) ));
	}


	static void  MatrixStorage_Test6 ()
	{
		// 4 rows with float3
		using float3x4_t = MatrixStorage< float, 4, 3, EMatrixOrder::RowMajor >;

		const float3x4_t  m1{};
		Unused( m1 );

		const float3x4_t  m2{ float3(1.0f, 2.0f, 3.0f),
							  float3(4.0f, 5.0f, 6.0f),
							  float3(7.0f, 8.0f, 9.0f),
							  float3(10.0f, 11.0f, 12.0f) };

		TEST( All( m2.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
		TEST( All( m2.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
		TEST( All( m2.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
		TEST( All( m2.get<3>() == float3(10.0f, 11.0f, 12.0f) ));

		const float3x4_t  m3{ 1.0f, 2.0f, 3.0f,
							  4.0f, 5.0f, 6.0f,
							  7.0f, 8.0f, 9.0f,
							  10.0f, 11.0f, 12.0f };

		TEST( All( m3.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
		TEST( All( m3.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
		TEST( All( m3.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
		TEST( All( m3.get<3>() == float3(10.0f, 11.0f, 12.0f) ));
	}


	static void  MatrixStorage_Test7 ()
	{
		using cm_float3x4_t = MatrixStorage< float, 3, 4, EMatrixOrder::ColumnMajor >;
		using rm_float3x4_t = MatrixStorage< float, 4, 3, EMatrixOrder::RowMajor >;

		cm_float3x4_t	m1{ float3x4{float4{1.0f, 2.0f, 3.0f, 4.0f},
									 float4{5.0f, 6.0f, 7.0f, 8.0f},
									 float4{9.0f, 10.f, 11.f, 12.f}} };

		TEST( All( m1.get<0>() == float4(1.0f, 2.0f, 3.0f, 4.0f) ));
		TEST( All( m1.get<1>() == float4(5.0f, 6.0f, 7.0f, 8.0f) ));
		TEST( All( m1.get<2>() == float4(9.0f, 10.f, 11.f, 12.f) ));

		float3x3		m2{ m1 };
		TEST( All( m2.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
		TEST( All( m2.get<1>() == float3(5.0f, 6.0f, 7.0f) ));
		TEST( All( m2.get<2>() == float3(9.0f, 10.f, 11.f) ));

		rm_float3x4_t	m3{ m2 };
		TEST( All( m3.get<0>() == float3(1.0f, 5.0f, 9.0f) ));
		TEST( All( m3.get<1>() == float3(2.0f, 6.0f, 10.f) ));
		TEST( All( m3.get<2>() == float3(3.0f, 7.0f, 11.f) ));
		TEST( All( m3.get<3>() == float3(0.0f, 0.0f, 0.0f) ));

		cm_float3x4_t	m4{ m3 };
		TEST( All( m4.get<0>() == float4(1.0f, 2.0f, 3.0f, 0.0f) ));
		TEST( All( m4.get<1>() == float4(5.0f, 6.0f, 7.0f, 0.0f) ));
		TEST( All( m4.get<2>() == float4(9.0f, 10.f, 11.f, 0.0f) ));

		rm_float3x4_t	m5{ m1 };
		TEST( All( m5.get<0>() == float3(1.0f, 5.0f,  9.0f) ));
		TEST( All( m5.get<1>() == float3(2.0f, 6.0f, 10.0f) ));
		TEST( All( m5.get<2>() == float3(3.0f, 7.0f, 11.0f) ));
		TEST( All( m5.get<3>() == float3(4.0f, 8.0f, 12.0f) ));

		float4x3		m6{ m5 };
		TEST( All( m6.get<0>() == float3(1.0f,  2.0f,  3.0f) ));
		TEST( All( m6.get<1>() == float3(5.0f,  6.0f,  7.0f) ));
		TEST( All( m6.get<2>() == float3(9.0f, 10.0f, 11.0f) ));
		TEST( All( m6.get<3>() == float3(0.0f,  0.0f,  0.0f) ));

		float4x3		m7{ m1 };
		TEST( All( m7.get<0>() == float3(1.0f,  2.0f,  3.0f) ));
		TEST( All( m7.get<1>() == float3(5.0f,  6.0f,  7.0f) ));
		TEST( All( m7.get<2>() == float3(9.0f, 10.0f, 11.0f) ));
		TEST( All( m7.get<3>() == float3(0.0f,  0.0f,  0.0f) ));
	}


	static void  MatrixStorage_Test8 ()
	{
		using cm_float4x3_t = MatrixStorage< float, 4, 3, EMatrixOrder::ColumnMajor >;
		using rm_float4x3_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor >;

		cm_float4x3_t	m1{ float4x3{float3{1.0f, 2.0f, 3.0f},
									 float3{4.0f, 5.0f, 6.0f},
									 float3{7.0f, 8.0f, 9.0f},
									 float3{10.f, 11.f, 12.f}} };

		TEST( All( m1.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
		TEST( All( m1.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
		TEST( All( m1.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
		TEST( All( m1.get<3>() == float3(10.f, 11.f, 12.f) ));

		rm_float4x3_t	m2{ m1 };
		TEST( All( m2.get<0>() == float4(1.0f, 4.0f, 7.0f, 10.f) ));
		TEST( All( m2.get<1>() == float4(2.0f, 5.0f, 8.0f, 11.f) ));
		TEST( All( m2.get<2>() == float4(3.0f, 6.0f, 9.0f, 12.f) ));

		cm_float4x3_t	m3{ m2 };
		TEST( All( m3.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
		TEST( All( m3.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
		TEST( All( m3.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
		TEST( All( m3.get<3>() == float3(10.f, 11.f, 12.f) ));

		float4x3		m4{ m1 };
		TEST( All( m4.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
		TEST( All( m4.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
		TEST( All( m4.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
		TEST( All( m4.get<3>() == float3(10.f, 11.f, 12.f) ));

		float4x3	m5{ m2 };
		TEST( All( m5.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
		TEST( All( m5.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
		TEST( All( m5.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
		TEST( All( m5.get<3>() == float3(10.f, 11.f, 12.f) ));

		rm_float4x3_t	m6{ m2 };
		TEST( All( m6.get<0>() == float4(1.0f, 4.0f, 7.0f, 10.f) ));
		TEST( All( m6.get<1>() == float4(2.0f, 5.0f, 8.0f, 11.f) ));
		TEST( All( m6.get<2>() == float4(3.0f, 6.0f, 9.0f, 12.f) ));
	}


	static void  Matrix_Test1 ()
	{
		const float3	v{ 0.0f, 0.0f, 0.5f };

		float3x3	m0 = float3x3::ToCubeFace( 0 );
		float3x3	m1 = float3x3::ToCubeFace( 1 );
		float3x3	m2 = float3x3::ToCubeFace( 2 );
		float3x3	m3 = float3x3::ToCubeFace( 3 );
		float3x3	m4 = float3x3::ToCubeFace( 4 );
		float3x3	m5 = float3x3::ToCubeFace( 5 );

		float3		v0 = m0 * v;
		float3		v1 = m1 * v;
		float3		v2 = m2 * v;
		float3		v3 = m3 * v;
		float3		v4 = m4 * v;
		float3		v5 = m5 * v;

		TEST(All( v0 == float3{0.0f} ));	// TODO

		Unused( v0, v1, v2, v3, v4, v5 );
	}


	static void  Matrix_Test2 ()
	{
		for (float a = -400.f; a < 500.f; a += 10.f)
		{
			Rad	angle = Rad::FromDeg( a );

			const auto	ref0 = float4x4{glm::rotate( glm::mat4x4{1.f}, float(angle), glm::vec3{1.f, 0.f, 0.f} )};
			const auto	ref1 = float4x4{glm::rotate( glm::mat4x4{1.f}, float(angle), glm::vec3{0.f, 1.f, 0.f} )};
			const auto	ref2 = float4x4{glm::rotate( glm::mat4x4{1.f}, float(angle), glm::vec3{0.f, 0.f, 1.f} )};

			const auto	m0 = float4x4::RotateX( angle );
			const auto	m1 = float4x4::RotateY( angle );
			const auto	m2 = float4x4::RotateZ( angle );

			TEST( BitEqual( m0, ref0 ));
			TEST( BitEqual( m1, ref1 ));
			TEST( BitEqual( m2, ref2 ));
		}
	}


	static void  Matrix_Test3 ()
	{
		const float	n	= 0.1f;
		const float	f	= 100.f;

		const auto	TestNearFar = [n, f] (const float4x4 &p)
		{{
			float4	a = p * float4{0.f, 0.f, n, 1.f};	a /= a.w;
			float4	b = p * float4{0.f, 0.f, f, 1.f};	b /= b.w;
			TEST( Equal( a.z, 0.f ));
			TEST( Equal( b.z, 1.f ));
		}};
		const auto	TestNearFarInverseZ = [n, f] (const float4x4 &p)
		{{
			float4	a = p * float4{0.f, 0.f, n, 1.f};	a /= a.w;
			float4	b = p * float4{0.f, 0.f, f, 1.f};	b /= b.w;
			TEST( Equal( a.z, 1.f ));
			TEST( Equal( b.z, 0.f ));
		}};
		const auto	TestInfinite = [n] (const float4x4 &p)
		{{
			float	i = 1.0e+8f;
			float4	a = p * float4{0.f, 0.f, n, 1.f};	a /= a.w;
			float4	b = p * float4{0.f, 0.f, i, 1.f};	b /= b.w;
			TEST( Equal( a.z, 0.f ));
			TEST( Equal( b.z, 1.f ));

			i = 6710886.50f;
			for (float z = n + 0.1f, prev = 0.f; z < i; z *= 2.f)
			{
				float4	c = p * float4{0.f, 0.f, z, 1.f};	c /= c.w;
				TEST( prev < c.z );
				prev = c.z;
			}
		}};
		const auto	TestInfiniteInverseZ = [n] (const float4x4 &p)
		{{
			float	i = 1.0e+8f;
			float4	a = p * float4{0.f, 0.f, n, 1.f};	a /= a.w;
			float4	b = p * float4{0.f, 0.f, i, 1.f};	b /= b.w;
			TEST( Equal( a.z, 1.f ));
			TEST( Equal( b.z, 0.f ));
		}};

		auto	p0 = float4x4::Perspective( 60_deg, 1.5f, float2{n, f} );
		TestNearFar( p0 );

		auto	p1 = float4x4::ReverseZTransform() * float4x4::Perspective( 60_deg, 1.5f, float2{n, f} );
		TestNearFarInverseZ( p1 );

		auto	p2 = float4x4::Perspective( 60_deg, float2{800.f, 600.f}, float2{n, f} );
		TestNearFar( p2 );

		auto	p3 = float4x4::Frustum( RectF{0.f, 0.f, 800.f, 600.f}, float2{n, f} );
		TestNearFar( p3 );

		auto	p4 = float4x4::InfiniteFrustum( RectF{0.f, 0.f, 800.f, 600.f}, n );
		TestInfinite( p4 );

		auto	p5 = float4x4::InfinitePerspective( 60_deg, 1.5f, n );
		TestInfinite( p5 );

		auto	p6 = float4x4::ReverseZTransform() * float4x4::InfiniteFrustum( RectF{0.f, 0.f, 800.f, 600.f}, n );
		TestInfiniteInverseZ( p6 );

		auto	p7 = float4x4::ReverseZTransform() * float4x4::InfinitePerspective( 60_deg, 1.5f, n );
		TestInfiniteInverseZ( p7 );
	}


	static void  Matrix_Test4 ()
	{
		float2	a0 = float2x2{} * float2{};
		float2	a1 = float3x2{} * float3{};
		float2	a2 = float4x2{} * float4{};
		Unused( a0, a1, a2 );
		
		float3	a3 = float2x3{} * float2{};
		float3	a4 = float3x3{} * float3{};
		float3	a5 = float4x3{} * float4{};
		Unused( a3, a4, a5 );
		
		float4	a6 = float2x4{} * float2{};
		float4	a7 = float3x4{} * float3{};
		float4	a8 = float4x4{} * float4{};
		Unused( a6, a7, a8 );

		float2	b0 = float2{} * float2x2{};
		float3	b1 = float2{} * float3x2{};
		float4	b2 = float2{} * float4x2{};
		Unused( b0, b1, b2 );
		
		float2	b3 = float3{} * float2x3{};
		float3	b4 = float3{} * float3x3{};
		float4	b5 = float3{} * float4x3{};
		Unused( b3, b4, b5 );
		
		float2	b6 = float4{} * float2x4{};
		float3	b7 = float4{} * float3x4{};
		float4	b8 = float4{} * float4x4{};
		Unused( b6, b7, b8 );

		Unused( float3x2::Translate( float2{} ));
		Unused( float4x4::Translate( float3{} ));
		Unused( float4x3::Translate( float3{} ));
	}


	static void  Matrix_Test5 ()
	{
		float3x3	b0	= float3x3::RotateY( 0_deg );		// +Z
		float3x3	b1	= float3x3::LookAt( b0.AxisZ(), b0.AxisY() );
		float3x3	b2	= float3x3::RotateY( 360_deg );		// +Z
		float3		b4	= b0.AxisX();
		float3		b5	= b0.AxisY();
		float3		b6	= b0.AxisZ();
		TEST( All( Equal( b4, float3{1.f, 0.f, 0.f}, 1_pct )));
		TEST( All( Equal( b5, float3{0.f, 1.f, 0.f}, 1_pct )));
		TEST( All( Equal( b6, float3{0.f, 0.f, 1.f}, 1_pct )));
		TEST( All( Equal( b4, b2.AxisX(), 1_pct )));
		TEST( All( Equal( b5, b2.AxisY(), 1_pct )));
		TEST( All( Equal( b6, b2.AxisZ(), 1_pct )));
		TEST( All( Equal( b4, b1.AxisX(), 1_pct )));
		TEST( All( Equal( b5, b1.AxisY(), 1_pct )));
		TEST( All( Equal( b6, b1.AxisZ(), 1_pct )));

		float3x3	c0	= float3x3::RotateY( 90_deg );		// -X
		float3x3	c1	= float3x3::LookAt( c0.AxisZ(), c0.AxisY() );
		float3x3	c2	= float3x3::RotateY( -270_deg );	// -X
		float3		c4	= c0.AxisX();
		float3		c5	= c0.AxisY();
		float3		c6	= c0.AxisZ();
		TEST( All( Equal( c4, float3{ 0.f, 0.f, 1.f}, 1_pct )));
		TEST( All( Equal( c5, float3{ 0.f, 1.f, 0.f}, 1_pct )));
		TEST( All( Equal( c6, float3{-1.f, 0.f, 0.f}, 1_pct )));
		TEST( All( Equal( c4, c2.AxisX(), 1_pct )));
		TEST( All( Equal( c5, c2.AxisY(), 1_pct )));
		TEST( All( Equal( c6, c2.AxisZ(), 1_pct )));
		TEST( All( Equal( c4, c1.AxisX(), 1_pct )));
		TEST( All( Equal( c5, c1.AxisY(), 1_pct )));
		TEST( All( Equal( c6, c1.AxisZ(), 1_pct )));

		float3x3	d0	= float3x3::RotateY( -90_deg );		// +X
		float3x3	d1	= float3x3::LookAt( d0.AxisZ(), d0.AxisY() );
		float3x3	d2	= float3x3::RotateY( 270_deg );		// +X
		float3		d4	= d0.AxisX();
		float3		d5	= d0.AxisY();
		float3		d6	= d0.AxisZ();
		TEST( All( Equal( d4, float3{0.f, 0.f, -1.f}, 1_pct )));
		TEST( All( Equal( d5, float3{0.f, 1.f,  0.f}, 1_pct )));
		TEST( All( Equal( d6, float3{1.f, 0.f,  0.f}, 1_pct )));
		TEST( All( Equal( d4, d2.AxisX(), 1_pct )));
		TEST( All( Equal( d5, d2.AxisY(), 1_pct )));
		TEST( All( Equal( d6, d2.AxisZ(), 1_pct )));
		TEST( All( Equal( d4, d1.AxisX(), 1_pct )));
		TEST( All( Equal( d5, d1.AxisY(), 1_pct )));
		TEST( All( Equal( d6, d1.AxisZ(), 1_pct )));

		float3x3	e0	= float3x3::RotateY( 180_deg );		// -Z
		float3x3	e1	= float3x3::LookAt( e0.AxisZ(), e0.AxisY() );
		float3x3	e2	= float3x3::RotateY( -180_deg );	// -Z
		float3		e4	= e0.AxisX();
		float3		e5	= e0.AxisY();
		float3		e6	= e0.AxisZ();
		TEST( All( Equal( e4, float3{-1.f, 0.f,  0.f}, 1_pct )));
		TEST( All( Equal( e5, float3{ 0.f, 1.f,  0.f}, 1_pct )));
		TEST( All( Equal( e6, float3{ 0.f, 0.f, -1.f}, 1_pct )));
		TEST( All( Equal( e4, e2.AxisX(), 1_pct )));
		TEST( All( Equal( e5, e2.AxisY(), 1_pct )));
		TEST( All( Equal( e6, e2.AxisZ(), 1_pct )));
		TEST( All( Equal( e4, e1.AxisX(), 1_pct )));
		TEST( All( Equal( e5, e1.AxisY(), 1_pct )));
		TEST( All( Equal( e6, e1.AxisZ(), 1_pct )));

		float3x3	f0	= float3x3::RotateX( 90_deg );		// -Y
		float3x3	f1	= float3x3::LookAt( f0.AxisZ(), f0.AxisY() );
		float3x3	f2	= float3x3::RotateX( -270_deg );	// -Y
		float3		f4	= f0.AxisX();
		float3		f5	= f0.AxisY();
		float3		f6	= f0.AxisZ();
		TEST( All( Equal( f4, float3{1.f, 0.f,  0.f}, 1_pct )));
		TEST( All( Equal( f5, float3{0.f, 0.f, -1.f}, 1_pct )));
		TEST( All( Equal( f6, float3{0.f, 1.f,  0.f}, 1_pct )));
		TEST( All( Equal( f4, f2.AxisX(), 1_pct )));
		TEST( All( Equal( f5, f2.AxisY(), 1_pct )));
		TEST( All( Equal( f6, f2.AxisZ(), 1_pct )));
		TEST( All( Equal( f4, f1.AxisX(), 1_pct )));
		TEST( All( Equal( f5, f1.AxisY(), 1_pct )));
		TEST( All( Equal( f6, f1.AxisZ(), 1_pct )));

		float3x3	g0	= float3x3::RotateX( -90_deg );		// +Y
		float3x3	g1	= float3x3::LookAt( g0.AxisZ(), g0.AxisY() );
		float3x3	g2	= float3x3::RotateX( 270_deg );		// +Y
		float3		g4	= g0.AxisX();
		float3		g5	= g0.AxisY();
		float3		g6	= g0.AxisZ();
		TEST( All( Equal( g4, float3{ 1.f,  0.f, 0.f}, 1_pct )));
		TEST( All( Equal( g5, float3{ 0.f,  0.f, 1.f}, 1_pct )));
		TEST( All( Equal( g6, float3{ 0.f, -1.f, 0.f}, 1_pct )));
		TEST( All( Equal( g4, g2.AxisX(), 1_pct )));
		TEST( All( Equal( g5, g2.AxisY(), 1_pct )));
		TEST( All( Equal( g6, g2.AxisZ(), 1_pct )));
		TEST( All( Equal( g4, g1.AxisX(), 1_pct )));
		TEST( All( Equal( g5, g1.AxisY(), 1_pct )));
		TEST( All( Equal( g6, g1.AxisZ(), 1_pct )));
	}

	
	static float  ToLinearDepth (float nonLinearUnormDepth, float near, float far)
	{
		// based on FastUnProjectZ
		float	a	= far / (far - near);
		float	b	= -near / (far - near);
		return	(a / (nonLinearUnormDepth - a) + 1.f) * b;
	}

	static float  ToNonlinearDepth (float linearUnormDepth, float near, float far)
	{
		// based on FastProjectZ
		float	a	= far - near;
		float	b	= far / a;
		float	c	= 1.0f - near / (linearUnormDepth * a + near);
		return	b * c;
	}


	static float  ToLinearDepthRevZ (float nonLinearUnormDepth, float near, float far)
	{
		// based on FastUnProjectZ
		nonLinearUnormDepth -= 1.0f;
		float	p22	= -far / (far - near);
		float	p32	= (far * near) / (far - near);
		float	world = p32 / (nonLinearUnormDepth - p22);
		return	(world - near) / (far - near);
	}

	static float  ToNonlinearDepthRevZ (float linearUnormDepth, float near, float far)
	{
		// based on FastProjectZ
		float	p22	= -far / (far - near);
		float	p32	= (far * near) / (far - near);
		float	world_z = linearUnormDepth * (far - near) + near;
		float	z = (p22 * world_z) + p32;
		return 1.0f + z / world_z;
	}


	static void  Matrix_Test6 ()
	{
		const float	n	= 1.0f;
		const float	f	= 100.f;

		for (float norm_z = 0.0f; norm_z < 1.0f; norm_z += 0.01f)
		{
			float	a = ToNonlinearDepth( norm_z, n, f );
			float	b = ToLinearDepth( a, n, f );
			CHECK( Equal( norm_z, b, 0.1_pct ));
		}
		for (float norm_z = 0.0f; norm_z < 1.0f; norm_z += 0.01f)
		{
			float	a = ToLinearDepth( norm_z, n, f );
			float	b = ToNonlinearDepth( a, n, f );
			CHECK( Equal( norm_z, b, 0.1_pct ));
		}
		
		for (float norm_z = 0.0f; norm_z < 1.0f; norm_z += 0.01f)
		{
			float	a = ToNonlinearDepthRevZ( norm_z, n, f );
			float	b = ToLinearDepthRevZ( a, n, f );
			CHECK( Equal( norm_z, b, 0.1_pct ));
		}
		for (float norm_z = 0.0f; norm_z < 1.0f; norm_z += 0.01f)
		{
			float	a = ToLinearDepthRevZ( norm_z, n, f );
			float	b = ToNonlinearDepthRevZ( a, n, f );
			CHECK( Equal( norm_z, b, 0.1_pct ));
		}

		const auto	Test = [n, f] (const float4x4 &p)
		{{
			for (float linear_norm = 0.0f; linear_norm <= 1.0f; linear_norm += 0.01f)
			{
				float	world_z = Lerp( n, f, linear_norm );
				float	a		= p.FastProjectZ( world_z );
				float	b		= p.FastUnProjectZ( a );
				CHECK( Equal( world_z, b, 0.1_pct ));
			}

			for (float linear_norm = -0.1f; linear_norm < 1.1f; linear_norm += 0.01f)
			{
				float	linear			= Lerp( n, f, linear_norm );
				float	non_linear		= p.ProjectToNormClipSpace( float3{ 0.f, 0.f, linear }).z;
				float	non_linear1		= p.FastProjectZ( linear );
				float	non_linear2		= ToNonlinearDepth( linear_norm, n, f );
				float	linear_norm2	= ToLinearDepth( non_linear, n, f );
				float	linear2			= p.FastUnProjectZ( non_linear );
			
				CHECK( Equal( non_linear, non_linear1, 0.1_pct ));
				CHECK( Equal( non_linear, non_linear2, 0.1_pct ));
				CHECK( Equal( linear_norm, linear_norm2, 0.1_pct ));
				CHECK( Equal( linear, linear2, 0.1_pct ));
			}
		}};

		auto	p0 = float4x4::Perspective( 60_deg, 1.5f, float2{n, f} );
		Test( p0 );


		const auto	TestInf = [n] (const float4x4 &p)
		{{
			const float	f = 2.0e+6f;

			for (float linear_norm = 0.0f; linear_norm <= 1.0f; linear_norm += 0.01f)
			{
				float	world_z = Lerp( n, f, linear_norm );
				float	a		= float4x4::FastProjectZInf( n, world_z );
				float	b		= float4x4::FastUnProjectZInf( n, a );

				if ( world_z < 1.0e+4f ){
					CHECK( Equal( world_z, b, 0.1_pct ));
				}else{
					CHECK( Equal( world_z, b, 10_pct ));
				}
			}

			for (float linear_norm = -0.01f; linear_norm < 1.01f; linear_norm += 0.01f)
			{
				float	linear			= Lerp( n, f, linear_norm );
				float	non_linear		= p.ProjectToNormClipSpace( float3{ 0.f, 0.f, linear }).z;
				float	non_linear1		= float4x4::FastProjectZInf( n, linear );
				float	non_linear2		= ToNonlinearDepth( linear_norm, n, f );
			//	float	linear_norm2	= ToLinearDepth( non_linear, n, f );
				float	linear2			= float4x4::FastUnProjectZInf( n, non_linear );
			
				CHECK( Equal( non_linear, non_linear1, 0.1_pct ));
				CHECK( Equal( non_linear, non_linear2, 0.1_pct ));
			//	CHECK( Equal( linear_norm, linear_norm2, 20_pct ));

				if ( linear > 0.0f and linear < 1.0e+4f ){
					CHECK( Equal( linear, linear2, 0.1_pct ));
				}else{
					CHECK( Equal( linear, linear2, 10_pct ));
				}
			}
		}};

		auto	p1 = float4x4::InfinitePerspective( 60_deg, 1.5f, n );
		TestInf( p1 );

		
		const auto	TestRevZ = [n, f] (const float4x4 &p)
		{{
			for (float linear_norm = 0.0f; linear_norm <= 1.0f; linear_norm += 0.01f)
			{
				float	world_z = Lerp( n, f, linear_norm );
				float	a		= p.FastProjectZ( world_z );
				float	b		= p.FastUnProjectZ( a );
				CHECK( Equal( world_z, b, 0.1_pct ));
			}

			for (float linear_norm = -0.1f; linear_norm < 1.1f; linear_norm += 0.01f)
			{
				float	linear			= Lerp( n, f, linear_norm );
				float	non_linear		= p.ProjectToNormClipSpace( float3{ 0.f, 0.f, linear }).z;
				float	non_linear1		= p.FastProjectZ( linear );
				float	non_linear2		= ToNonlinearDepthRevZ( linear_norm, n, f );
				float	linear_norm2	= ToLinearDepthRevZ( non_linear, n, f );
				float	linear2			= p.FastUnProjectZ( non_linear );
			
				CHECK( Equal( non_linear, non_linear1, 0.1_pct ));
				CHECK( Equal( non_linear, non_linear2, 0.1_pct ));
				CHECK( Equal( linear_norm, linear_norm2, 0.1_pct ));
				CHECK( Equal( linear, linear2, 0.1_pct ));
			}
		}};

		auto	p2 = float4x4::ReverseZTransform() * float4x4::Perspective( 60_deg, 1.5f, float2{n, f} );
		TestRevZ( p2 );


		const auto	TestRevZInf = [n] (const float4x4 &p)
		{{
			const float	f = 2.0e+6f;

			for (float linear_norm = 0.0f; linear_norm <= 1.0f; linear_norm += 0.01f)
			{
				float	world_z = Lerp( n, f, linear_norm );
				float	a		= float4x4::FastProjectRevZInf( n, world_z );
				float	b		= float4x4::FastUnProjectRevZInf( n, a );

				if ( world_z < 1.0e+4f ){
					CHECK( Equal( world_z, b, 0.1_pct ));
				}else{
					CHECK( Equal( world_z, b, 10_pct ));
				}
			}

			for (float linear_norm = -0.01f; linear_norm < 1.01f; linear_norm += 0.01f)
			{
				float	linear			= Lerp( n, f, linear_norm );
				float	non_linear		= p.ProjectToNormClipSpace( float3{ 0.f, 0.f, linear }).z;
				float	non_linear1		= float4x4::FastProjectRevZInf( n, linear );
			//	float	non_linear2		= ToNonlinearDepthRevZ( linear_norm, n, f );
			//	float	linear_norm2	= ToLinearDepthRevZ( non_linear, n, f );
				float	linear2			= float4x4::FastUnProjectRevZInf( n, non_linear );
			
				CHECK( Equal( non_linear, non_linear1, 0.1_pct ));
			//	CHECK( Equal( non_linear, non_linear2, 0.1_pct ));
			//	CHECK( Equal( linear_norm, linear_norm2, 20_pct ));

				if ( linear > 0.0f and linear < 1.0e+4f ){
					CHECK( Equal( linear, linear2, 0.1_pct ));
				}else{
					CHECK( Equal( linear, linear2, 10_pct ));
				}
			}
		}};

		auto	p3 = float4x4::ReverseZTransform() * float4x4::InfinitePerspective( 60_deg, 1.5f, n );
		TestRevZInf( p3 );
	}
}


extern void UnitTest_Math_Matrix ()
{
	MatrixStorage_Test1();
	MatrixStorage_Test2();
	MatrixStorage_Test3();
	MatrixStorage_Test4();
	MatrixStorage_Test5();
	MatrixStorage_Test6();
	MatrixStorage_Test7();
	MatrixStorage_Test8();

	//Matrix_Test1();	// TODO
	Matrix_Test2();
	Matrix_Test3();
	Matrix_Test4();
	Matrix_Test5();
	Matrix_Test6();

	TEST_PASSED();
}
