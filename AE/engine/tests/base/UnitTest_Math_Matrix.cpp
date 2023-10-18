// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  MatrixStorage_Test1 ()
    {
        // 4 columns, 2 rows
        using CMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::ColumnMajor >;
        using RMat4x2_t = MatrixStorage< float, 2, 4, EMatrixOrder::RowMajor >;

        STATIC_ASSERT( VecSize<CMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( VecSize<CMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(CMat4x2_t) == sizeof(float) * 4 * 2 );

        STATIC_ASSERT( VecSize<RMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( VecSize<RMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(RMat4x2_t) == sizeof(float) * 4 * 2 );

        STATIC_ASSERT( CMat4x2_t::Dimension().columns == 4 );
        STATIC_ASSERT( CMat4x2_t::Dimension().rows == 2 );
        STATIC_ASSERT( CMat4x2_t::Dimension() == RMat4x2_t::Dimension() );
    };


    static void  MatrixStorage_Test2 ()
    {
        // 4 columns, 2 rows
        using CMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::ColumnMajor, sizeof(float)*4 >;
        using RMat4x2_t = MatrixStorage< float, 2, 4, EMatrixOrder::RowMajor, sizeof(float)*4 >;

        STATIC_ASSERT( VecSize<CMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( VecSize<CMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(CMat4x2_t) == sizeof(float) * 4 * 4 );

        STATIC_ASSERT( VecSize<RMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( VecSize<RMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(RMat4x2_t) == sizeof(float) * 4 * 2 );

        STATIC_ASSERT( CMat4x2_t::Dimension().columns == 4 );
        STATIC_ASSERT( CMat4x2_t::Dimension().rows == 2 );
        STATIC_ASSERT( CMat4x2_t::Dimension() == RMat4x2_t::Dimension() );
    };


    static void  MatrixStorage_Test3 ()
    {
        // 4 columns, 3 rows
        using CMat4x3_t = MatrixStorage< float, 4, 3, EMatrixOrder::ColumnMajor >;
        using RMat4x3_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor >;

        STATIC_ASSERT( VecSize<CMat4x3_t::Column_t> == 3 );
        STATIC_ASSERT( VecSize<CMat4x3_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(CMat4x3_t) == sizeof(float) * 4 * 3 );

        STATIC_ASSERT( VecSize<RMat4x3_t::Column_t> == 3 );
        STATIC_ASSERT( VecSize<RMat4x3_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(RMat4x3_t) == sizeof(float) * 4 * 3 );

        STATIC_ASSERT( CMat4x3_t::Dimension().columns == 4 );
        STATIC_ASSERT( CMat4x3_t::Dimension().rows == 3 );
        STATIC_ASSERT( CMat4x3_t::Dimension() == RMat4x3_t::Dimension() );
    };


    static void  MatrixStorage_Test4 ()
    {
        // 4 columns, 3 rows
        using CMat4x3_t = MatrixStorage< float, 4, 3, EMatrixOrder::ColumnMajor, sizeof(float)*4 >;
        using RMat4x3_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor, sizeof(float)*4 >;

        STATIC_ASSERT( VecSize<CMat4x3_t::Column_t> == 3 );
        STATIC_ASSERT( VecSize<CMat4x3_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(CMat4x3_t) == sizeof(float) * 4 * 4 );

        STATIC_ASSERT( VecSize<RMat4x3_t::Column_t> == 3 );
        STATIC_ASSERT( VecSize<RMat4x3_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(RMat4x3_t) == sizeof(float) * 4 * 3 );

        STATIC_ASSERT( CMat4x3_t::Dimension().columns == 4 );
        STATIC_ASSERT( CMat4x3_t::Dimension().rows == 3 );
        STATIC_ASSERT( CMat4x3_t::Dimension() == RMat4x3_t::Dimension() );
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

        cm_float3x4_t   m1{ float3x4{float4{1.0f, 2.0f, 3.0f, 4.0f},
                                     float4{5.0f, 6.0f, 7.0f, 8.0f},
                                     float4{9.0f, 10.f, 11.f, 12.f}} };

        TEST( All( m1.get<0>() == float4(1.0f, 2.0f, 3.0f, 4.0f) ));
        TEST( All( m1.get<1>() == float4(5.0f, 6.0f, 7.0f, 8.0f) ));
        TEST( All( m1.get<2>() == float4(9.0f, 10.f, 11.f, 12.f) ));

        float3x3        m2{ m1 };
        TEST( All( m2.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m2.get<1>() == float3(5.0f, 6.0f, 7.0f) ));
        TEST( All( m2.get<2>() == float3(9.0f, 10.f, 11.f) ));

        rm_float3x4_t   m3{ m2 };
        TEST( All( m3.get<0>() == float3(1.0f, 5.0f, 9.0f) ));
        TEST( All( m3.get<1>() == float3(2.0f, 6.0f, 10.f) ));
        TEST( All( m3.get<2>() == float3(3.0f, 7.0f, 11.f) ));
        TEST( All( m3.get<3>() == float3(0.0f, 0.0f, 0.0f) ));

        cm_float3x4_t   m4{ m3 };
        TEST( All( m4.get<0>() == float4(1.0f, 2.0f, 3.0f, 0.0f) ));
        TEST( All( m4.get<1>() == float4(5.0f, 6.0f, 7.0f, 0.0f) ));
        TEST( All( m4.get<2>() == float4(9.0f, 10.f, 11.f, 0.0f) ));

        rm_float3x4_t   m5{ m1 };
        TEST( All( m5.get<0>() == float3(1.0f, 5.0f,  9.0f) ));
        TEST( All( m5.get<1>() == float3(2.0f, 6.0f, 10.0f) ));
        TEST( All( m5.get<2>() == float3(3.0f, 7.0f, 11.0f) ));
        TEST( All( m5.get<3>() == float3(4.0f, 8.0f, 12.0f) ));

        float4x3        m6{ m5 };
        TEST( All( m6.get<0>() == float3(1.0f,  2.0f,  3.0f) ));
        TEST( All( m6.get<1>() == float3(5.0f,  6.0f,  7.0f) ));
        TEST( All( m6.get<2>() == float3(9.0f, 10.0f, 11.0f) ));
        TEST( All( m6.get<3>() == float3(0.0f,  0.0f,  0.0f) ));

        float4x3        m7{ m1 };
        TEST( All( m7.get<0>() == float3(1.0f,  2.0f,  3.0f) ));
        TEST( All( m7.get<1>() == float3(5.0f,  6.0f,  7.0f) ));
        TEST( All( m7.get<2>() == float3(9.0f, 10.0f, 11.0f) ));
        TEST( All( m7.get<3>() == float3(0.0f,  0.0f,  0.0f) ));
    }


    static void  MatrixStorage_Test8 ()
    {
        using cm_float4x3_t = MatrixStorage< float, 4, 3, EMatrixOrder::ColumnMajor >;
        using rm_float4x3_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor >;

        cm_float4x3_t   m1{ float4x3{float3{1.0f, 2.0f, 3.0f},
                                     float3{4.0f, 5.0f, 6.0f},
                                     float3{7.0f, 8.0f, 9.0f},
                                     float3{10.f, 11.f, 12.f}} };

        TEST( All( m1.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m1.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
        TEST( All( m1.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
        TEST( All( m1.get<3>() == float3(10.f, 11.f, 12.f) ));

        rm_float4x3_t   m2{ m1 };
        TEST( All( m2.get<0>() == float4(1.0f, 4.0f, 7.0f, 10.f) ));
        TEST( All( m2.get<1>() == float4(2.0f, 5.0f, 8.0f, 11.f) ));
        TEST( All( m2.get<2>() == float4(3.0f, 6.0f, 9.0f, 12.f) ));

        cm_float4x3_t   m3{ m2 };
        TEST( All( m3.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m3.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
        TEST( All( m3.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
        TEST( All( m3.get<3>() == float3(10.f, 11.f, 12.f) ));

        float4x3        m4{ m1 };
        TEST( All( m4.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m4.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
        TEST( All( m4.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
        TEST( All( m4.get<3>() == float3(10.f, 11.f, 12.f) ));

        float4x3    m5{ m2 };
        TEST( All( m4.get<0>() == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m4.get<1>() == float3(4.0f, 5.0f, 6.0f) ));
        TEST( All( m4.get<2>() == float3(7.0f, 8.0f, 9.0f) ));
        TEST( All( m4.get<3>() == float3(10.f, 11.f, 12.f) ));

        rm_float4x3_t   m6{ m2 };
        TEST( All( m6.get<0>() == float4(1.0f, 4.0f, 7.0f, 10.f) ));
        TEST( All( m6.get<1>() == float4(2.0f, 5.0f, 8.0f, 11.f) ));
        TEST( All( m6.get<2>() == float4(3.0f, 6.0f, 9.0f, 12.f) ));
    }

    /*
    static void  Matrix_Test1 ()
    {
        const float3    v{ 0.0f, 0.0f, 0.5f };

        float3x3    m0 = float3x3::ToCubeFace( 0 );
        float3x3    m1 = float3x3::ToCubeFace( 1 );
        float3x3    m2 = float3x3::ToCubeFace( 2 );
        float3x3    m3 = float3x3::ToCubeFace( 3 );
        float3x3    m4 = float3x3::ToCubeFace( 4 );
        float3x3    m5 = float3x3::ToCubeFace( 5 );

        float3      v0 = m0 * v;
        float3      v1 = m1 * v;
        float3      v2 = m2 * v;
        float3      v3 = m3 * v;
        float3      v4 = m4 * v;
        float3      v5 = m5 * v;

        TEST(All( v0 == float3{0.0f} ));    // TODO
    }*/


    static void  Matrix_Test2 ()
    {
        for (float a = -400.f; a < 500.f; a += 10.f)
        {
            Rad angle = Rad::FromDeg( a );

            const auto  ref0 = float4x4{glm::rotate( glm::mat4x4{1.f}, float(angle), glm::vec3{1.f, 0.f, 0.f} )};
            const auto  ref1 = float4x4{glm::rotate( glm::mat4x4{1.f}, float(angle), glm::vec3{0.f, 1.f, 0.f} )};
            const auto  ref2 = float4x4{glm::rotate( glm::mat4x4{1.f}, float(angle), glm::vec3{0.f, 0.f, 1.f} )};

            const auto  m0 = float4x4::RotateX( angle );
            const auto  m1 = float4x4::RotateY( angle );
            const auto  m2 = float4x4::RotateZ( angle );

            TEST( BitEqual( m0, ref0 ));
            TEST( BitEqual( m1, ref1 ));
            TEST( BitEqual( m2, ref2 ));
        }
    }


    static void  Matrix_Test3 ()
    {
        const float n   = 0.1f;
        const float f   = 100.f;

        const auto  TestNearFar = [n, f] (const float4x4 &p)
        {{
            float4  a = p * float4{0.f, 0.f, n, 1.f};   a /= a.w;
            float4  b = p * float4{0.f, 0.f, f, 1.f};   b /= b.w;
            TEST( Equals( a.z, 0.f ));
            TEST( Equals( b.z, 1.f ));
        }};
        const auto  TestNearFarInverseZ = [n, f] (const float4x4 &p)
        {{
            float4  a = p * float4{0.f, 0.f, n, 1.f};   a /= a.w;
            float4  b = p * float4{0.f, 0.f, f, 1.f};   b /= b.w;
            TEST( Equals( a.z, 1.f ));
            TEST( Equals( b.z, 0.f ));
        }};
        const auto  TestInfinite = [n] (const float4x4 &p)
        {{
            float   i = 1.0e+8f;
            float4  a = p * float4{0.f, 0.f, n, 1.f};   a /= a.w;
            float4  b = p * float4{0.f, 0.f, i, 1.f};   b /= b.w;
            TEST( Equals( a.z, 0.f ));
            TEST( Equals( b.z, 1.f ));

            i = 6710886.50f;
            for (float z = n + 0.1f, prev = 0.f; z < i; z *= 2.f)
            {
                float4  c = p * float4{0.f, 0.f, z, 1.f};   c /= c.w;
                TEST( prev < c.z );
                prev = c.z;
            }
        }};
        const auto  TestInfiniteInverseZ = [n] (const float4x4 &p)
        {{
            float   i = 1.0e+8f;
            float4  a = p * float4{0.f, 0.f, n, 1.f};   a /= a.w;
            float4  b = p * float4{0.f, 0.f, i, 1.f};   b /= b.w;
            TEST( Equals( a.z, 1.f ));
            TEST( Equals( b.z, 0.f ));
        }};

        auto    p0 = float4x4::Perspective( 60_deg, 1.5f, float2{n, f} );
        TestNearFar( p0 );

        auto    p1 = float4x4::ReverseZTransform() * float4x4::Perspective( 60_deg, 1.5f, float2{n, f} );
        TestNearFarInverseZ( p1 );

        auto    p2 = float4x4::Perspective( 60_deg, float2{800.f, 600.f}, float2{n, f} );
        TestNearFar( p2 );

        auto    p3 = float4x4::Frustum( RectF{0.f, 0.f, 800.f, 600.f}, float2{n, f} );
        TestNearFar( p3 );

        auto    p4 = float4x4::InfiniteFrustum( RectF{0.f, 0.f, 800.f, 600.f}, n );
        TestInfinite( p4 );

        auto    p5 = float4x4::InfinitePerspective( 60_deg, 1.5f, n );
        TestInfinite( p5 );

        auto    p6 = float4x4::ReverseZTransform() * float4x4::InfiniteFrustum( RectF{0.f, 0.f, 800.f, 600.f}, n );
        TestInfiniteInverseZ( p6 );

        auto    p7 = float4x4::ReverseZTransform() * float4x4::InfinitePerspective( 60_deg, 1.5f, n );
        TestInfiniteInverseZ( p7 );
    }


    static void  Matrix_Test4 ()
    {
        Unused( float4x4::Translated( float3{} ));
        Unused( float4x3::Translated( float3{} ));
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

    //Matrix_Test1();   // TODO
    Matrix_Test2();
    Matrix_Test3();
    Matrix_Test4();

    TEST_PASSED();
}
