// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Matrix.h"
#include "base/Math/MatrixStorage.h"
#include "UnitTest_Common.h"


namespace
{
    static void  MatrixStorage_Test1 ()
    {
        using CMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::ColumnMajor >;
        using RMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::RowMajor >;

        STATIC_ASSERT( VecSize<CMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( VecSize<CMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(CMat4x2_t) == sizeof(float) * 4 * 2 );

        STATIC_ASSERT( VecSize<RMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( VecSize<RMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( sizeof(RMat4x2_t) == sizeof(float) * 4 * 2 );
    };


    static void  MatrixStorage_Test2 ()
    {
        using CMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::ColumnMajor, sizeof(float)*4 >;
        using RMat4x2_t = MatrixStorage< float, 4, 2, EMatrixOrder::RowMajor, sizeof(float)*4 >;

        STATIC_ASSERT( VecSize<CMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( VecSize<CMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( sizeof(CMat4x2_t) == sizeof(float) * 4 * 4 );

        STATIC_ASSERT( VecSize<RMat4x2_t::Row_t> == 4 );
        STATIC_ASSERT( VecSize<RMat4x2_t::Column_t> == 2 );
        STATIC_ASSERT( sizeof(RMat4x2_t) == sizeof(float) * 4 * 2 );
    };


    static void  MatrixStorage_Test3 ()
    {
        using float3x4_t = MatrixStorage< float, 3, 4, EMatrixOrder::ColumnMajor >;

        const float3x4_t  m1{};
        Unused( m1 );

        const float3x4_t  m2{ float4(1.0f, 2.0f, 3.0f, 4.0f),
                              float4(5.0f, 6.0f, 7.0f, 8.0f),
                              float4(9.0f, 10.0f, 11.0f, 12.0f) };

        TEST( All( m2[0] == float4(1.0f, 2.0f, 3.0f, 4.0f) ));
        TEST( All( m2[1] == float4(5.0f, 6.0f, 7.0f, 8.0f) ));
        TEST( All( m2[2] == float4(9.0f, 10.0f, 11.0f, 12.0f) ));

        const float3x4_t  m3{ 1.0f, 2.0f, 3.0f, 4.0f,
                              5.0f, 6.0f, 7.0f, 8.0f,
                              9.0f, 10.0f, 11.0f, 12.0f };

        TEST( All( m3[0] == float4(1.0f, 2.0f, 3.0f, 4.0f) ));
        TEST( All( m3[1] == float4(5.0f, 6.0f, 7.0f, 8.0f) ));
        TEST( All( m3[2] == float4(9.0f, 10.0f, 11.0f, 12.0f) ));
    }


    static void  MatrixStorage_Test4 ()
    {
        using float3x4_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor >;

        const float3x4_t  m1{};
        Unused( m1 );

        const float3x4_t  m2{ float3(1.0f, 2.0f, 3.0f),
                              float3(4.0f, 5.0f, 6.0f),
                              float3(7.0f, 8.0f, 9.0f),
                              float3(10.0f, 11.0f, 12.0f) };

        TEST( All( m2[0] == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m2[1] == float3(4.0f, 5.0f, 6.0f) ));
        TEST( All( m2[2] == float3(7.0f, 8.0f, 9.0f) ));
        TEST( All( m2[3] == float3(10.0f, 11.0f, 12.0f) ));

        const float3x4_t  m3{ 1.0f, 2.0f, 3.0f,
                              4.0f, 5.0f, 6.0f,
                              7.0f, 8.0f, 9.0f,
                              10.0f, 11.0f, 12.0f };

        TEST( All( m3[0] == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m3[1] == float3(4.0f, 5.0f, 6.0f) ));
        TEST( All( m3[2] == float3(7.0f, 8.0f, 9.0f) ));
        TEST( All( m3[3] == float3(10.0f, 11.0f, 12.0f) ));
    }


    static void  MatrixStorage_Test5 ()
    {
        using cm_float3x4_t = MatrixStorage< float, 3, 4, EMatrixOrder::ColumnMajor >;
        using rm_float3x4_t = MatrixStorage< float, 3, 4, EMatrixOrder::RowMajor >;

        cm_float3x4_t   m1{ float3x4{float4{1.0f, 2.0f, 3.0f, 4.0f},
                                     float4{5.0f, 6.0f, 7.0f, 8.0f},
                                     float4{9.0f, 10.f, 11.f, 12.f}} };

        TEST( All( m1[0] == float4(1.0f, 2.0f, 3.0f, 4.0f) ));
        TEST( All( m1[1] == float4(5.0f, 6.0f, 7.0f, 8.0f) ));
        TEST( All( m1[2] == float4(9.0f, 10.f, 11.f, 12.f) ));

        float3x3 m2 = float3x3{m1};
        TEST( All( m2[0] == float3(1.0f, 2.0f, 3.0f) ));
        TEST( All( m2[1] == float3(5.0f, 6.0f, 7.0f) ));
        TEST( All( m2[2] == float3(9.0f, 10.f, 11.f) ));

        rm_float3x4_t   m3{ m2 };
        TEST( All( m3[0] == float3(1.0f, 5.0f, 9.0f) ));
        TEST( All( m3[1] == float3(2.0f, 6.0f, 10.f) ));
        TEST( All( m3[2] == float3(3.0f, 7.0f, 11.f) ));
        TEST( All( m3[3] == float3(0.0f, 0.0f, 0.0f) ));

        cm_float3x4_t   m4{ m3 };
        TEST( All( m4[0] == float4(1.0f, 2.0f, 3.0f, 0.0f) ));
        TEST( All( m4[1] == float4(5.0f, 6.0f, 7.0f, 0.0f) ));
        TEST( All( m4[2] == float4(9.0f, 10.f, 11.f, 0.0f) ));
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
}


extern void UnitTest_Math_Matrix ()
{
    MatrixStorage_Test1();
    MatrixStorage_Test2();
    MatrixStorage_Test3();
    MatrixStorage_Test4();
    MatrixStorage_Test5();

    //Matrix_Test1();   // TODO

    TEST_PASSED();
}
