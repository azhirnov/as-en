// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    using Transform_t = Transformation<float>;

    constexpr float eps = 1.0e-4f;

    static void  Transformation_Test1 ()
    {
        QuatF       q = QuatF::RotateX( 45.0_deg );
        float3      p = float3{10.0f, -3.0f, 2.5f};
        float4x4    m = float4x4::Translated( p ) * float4x4{q};

        Transform_t t{ m };

        TEST( Equals( q.x, t.orientation.x, eps ));
        TEST( Equals( q.y, t.orientation.y, eps ));
        TEST( Equals( q.z, t.orientation.z, eps ));
        TEST( Equals( q.w, t.orientation.w, eps ));

        TEST( Equals( p.x, t.position.x, eps ));
        TEST( Equals( p.y, t.position.y, eps ));
        TEST( Equals( p.z, t.position.z, eps ));

        TEST( Equals( 1.0f, t.scale, eps ));
    }


    static void  Transformation_Test2 ()
    {
        const Transform_t   tr{ float3{1.0f, 2.0f, 3.0f},
                                QuatF::Rotate({ 45.0_deg, 0.0_deg, 10.0_deg }),
                                2.0f };

        const float4x4  mat = tr.ToMatrix();

        const float3    point0{ -1.0f, -1.0f, -1.0f };
        const float3    point1{ 1.0f, 1.0f, 1.0f };

        const float3    mat_point0  = float3(mat * float4( point0, 1.0f ));
        const float3    mat_point1  = float3(mat * float4( point1, 1.0f ));

        const float3    tr_point0   = tr.ToGlobalPosition( point0 );
        const float3    tr_point1   = tr.ToGlobalPosition( point1 );

        TEST( Equals( mat_point0.x, tr_point0.x, eps ));
        TEST( Equals( mat_point0.y, tr_point0.y, eps ));
        TEST( Equals( mat_point0.z, tr_point0.z, eps ));

        TEST( Equals( mat_point1.x, tr_point1.x, eps ));
        TEST( Equals( mat_point1.y, tr_point1.y, eps ));
        TEST( Equals( mat_point1.z, tr_point1.z, eps ));
    }


    static void  Transformation_Test3 ()
    {
        const Transform_t   tr1{ float3{1.0f, 2.0f, -3.0f},
                                 QuatF::Rotate({ 45.0_deg, 0.0_deg, 10.0_deg }),
                                 2.0f };
        const Transform_t tr2 = tr1.Inversed();
        const Transform_t tr3 = tr2.Inversed();

        TEST( All( Equals( tr1.position, tr3.position, eps )));
        TEST( All( Equals( tr1.orientation, tr3.orientation, eps )));
        TEST( Equals( tr1.scale, tr3.scale, eps ));
    }
}


extern void UnitTest_Math_Transformation ()
{
    Transformation_Test1();
    Transformation_Test2();
    Transformation_Test3();

    TEST_PASSED();
}
