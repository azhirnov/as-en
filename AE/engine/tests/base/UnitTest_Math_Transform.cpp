// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    using Transform_t = TTransformation<float>;

    static void  Transformation_Test1 ()
    {
        Quat        q = Quat::RotateX( 45.0_deg );
        float3      p = float3{10.0f, -3.0f, 2.5f};
        float4x4    m = float4x4::Translated( p ) * float4x4{q};

        Transform_t t{ m };

        TEST( All( BitEqual( q, t.orientation )));
        TEST( All( BitEqual( p, t.position )));
        TEST( All( BitEqual( 1.0f, t.scale )));
    }


    static void  Transformation_Test2 ()
    {
        const Transform_t   tr{ float3{1.0f, 2.0f, 3.0f},
                                Quat::Rotate({ 45.0_deg, 0.0_deg, 10.0_deg }),
                                2.0f };

        const float4x4  mat = tr.ToMatrix();

        const float3    point0{ -1.0f, -1.0f, -1.0f };
        const float3    point1{ 1.0f, 1.0f, 1.0f };

        const float3    mat_point0  = float3(mat * float4( point0, 1.0f ));
        const float3    mat_point1  = float3(mat * float4( point1, 1.0f ));

        const float3    tr_point0   = tr.ToGlobalPosition( point0 );
        const float3    tr_point1   = tr.ToGlobalPosition( point1 );

        TEST( All( BitEqual( mat_point0, tr_point0 )));
        TEST( All( BitEqual( mat_point1, tr_point1 )));
    }


    static void  Transformation_Test3 ()
    {
        const Transform_t   tr1{ float3{1.0f, 2.0f, -3.0f},
                                 Quat::Rotate({ 45.0_deg, 0.0_deg, 10.0_deg }),
                                 2.0f };
        const Transform_t tr2 = tr1.Inversed();
        const Transform_t tr3 = tr2.Inversed();

        TEST( All( BitEqual( tr1.position, tr3.position )));
        TEST( All( BitEqual( tr1.orientation, tr3.orientation )));
        TEST( BitEqual( tr1.scale, tr3.scale ));
    }


    static void  Transformation_Test4 ()
    {
        const Transform_t   tr1{ float3{1.0f, 2.0f, 3.0f},
                                 Quat::Identity(),
                                 2.0f };
        const Transform_t   tr2{ float3{0.0f, -5.f, 0.f},
                                 Quat::Rotate({ 45.0_deg, 0.0_deg, 10.0_deg }),
                                 1.0f };
        const Transform_t   tr3 = tr1 + tr2;

        const float4x4  mat1 = tr1.ToMatrix();
        const float4x4  mat2 = tr2.ToMatrix();
        const float4x4  mat3 = tr3.ToMatrix();
        const float4x4  mat4 = mat1 * mat2;

        TEST( BitEqual( mat3, mat4 ));
    }
}


extern void UnitTest_Math_Transformation ()
{
    Transformation_Test1();
    Transformation_Test2();
    Transformation_Test3();
    Transformation_Test4();

    TEST_PASSED();
}
