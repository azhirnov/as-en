// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    template <typename T>
    static void  TestVec2Align ()
    {
        using Self = Vec<T,2>;
        StaticAssert( offsetof(Self, x) + sizeof(T) == offsetof(Self, y) );
        StaticAssert( sizeof(T)*(2-1) == (offsetof(Self, y) - offsetof(Self, x)) );
    }

    template <typename T>
    static void  TestVec3Align ()
    {
        using Self = Vec<T,3>;
        StaticAssert( offsetof(Self, x) + sizeof(T) == offsetof(Self, y) );
        StaticAssert( offsetof(Self, y) + sizeof(T) == offsetof(Self, z) );
        StaticAssert( sizeof(T)*(3-1) == (offsetof(Self, z) - offsetof(Self, x)) );
    }

    template <typename T>
    static void  TestVec4Align ()
    {
        using Self = Vec<T,4>;
        StaticAssert( offsetof(Self, x) + sizeof(T) == offsetof(Self, y) );
        StaticAssert( offsetof(Self, y) + sizeof(T) == offsetof(Self, z) );
        StaticAssert( offsetof(Self, z) + sizeof(T) == offsetof(Self, w) );
        StaticAssert( sizeof(T)*(4-1) == (offsetof(Self, w) - offsetof(Self, x)) );
    }

    template <typename T>
    static void  TestVecAlign ()
    {
        // check if supported cast Vec to array
        TestVec2Align<T>();
        TestVec3Align<T>();
        TestVec4Align<T>();
    }


    static void  Vec_Test1 ()
    {
        TestVecAlign<float>();
        TestVecAlign<int>();
        TestVecAlign<double>();
        TestVecAlign<slong>();
        TestVecAlign<ushort>();

        StaticAssert( alignof(ushort3)  == sizeof(ushort)*4 );
        StaticAssert( alignof(uint3)    == sizeof(uint)*4 );
        StaticAssert( alignof(uint4)    == sizeof(uint)*4);

        StaticAssert( alignof(packed_uint3) == sizeof(uint) );
        StaticAssert( alignof(uint3) == 16 );
        StaticAssert( alignof(float4) == 16 );
    }


    static void  Vec_Test2 ()
    {
        TEST(All( float2(1.1f, 2.2f) == float2(0.1f, 0.2f) + float2(1.0f, 2.0f) ));
    }


    static void  Vec_Test3 ()
    {
        bool2   a0 = int2(2) > 0;                       TEST( All( a0 ));
        bool2   a1 = int2(2) == 2;                      TEST( All( a1 ));
        bool3   a2 = int3(1, 2, 3) > int3(2, 1, 4);
        bool3   a3 = a2 == bool3(false, true, false);   TEST( All( a3 ));
        bool3   a4 = uint3(1) < uint3(2);               TEST( All( a4 ));
    }


    static void  Vec_Test4 ()
    {
        // TODO: glm with SSE2 crashed here

        uint4   a0 = Min( uint4(1), uint4(2) );         TEST( All( a0 == uint4(1) ));
        uint4   a1 = Max( uint4(1), uint4(2) );         TEST( All( a1 == uint4(2) ));
    }


    static void  Vec_Test5 ()
    {
        auto    a0 = AlignUp( uint2{3}, uint2{2} );     TEST( All( a0 == uint2{4} ));
        auto    a1 = AlignDown( uint2{3}, uint2{2} );   TEST( All( a1 == uint2{2} ));
    }


    static void  VecSwizzle_Test1 ()
    {
        constexpr VecSwizzle    a1 = "XYZW"_vecSwizzle;
        StaticAssert( a1.Get() == 0x1234 );

        constexpr VecSwizzle    a2 = "yZx1"_vecSwizzle;
        StaticAssert( a2.Get() == 0x2316 );

        const int4  a3 = int3{6} * "010"_vecSwizzle;
        TEST( All( a3 == int4{0,1,0,0} ));

        const int4  a4 = int3{4,5,6} * "xy"_vecSwizzle;
        TEST( All( a4 == int4{4,5,0,0} ));

        const int4  a5 = int3{4,5,6} * "x01z"_vecSwizzle;
        TEST( All( a5 == int4{4,0,1,6} ));

        const int4  a6 = int3{7} * "0-0+"_vecSwizzle;
        TEST( All( a6 == int4{0,-1,0,1} ));
    }


    static void  Quat_Test1 ()
    {
        Quat    b0  = Quat::RotateY( 0_deg );       // +Z
        Quat    b1  = Quat::LookAt( b0.AxisZ(), b0.AxisY() );
        Quat    b2  = Quat::RotateY( 360_deg );     // +Z
        float3  b4  = b0.AxisX();
        float3  b5  = b0.AxisY();
        float3  b6  = b0.AxisZ();
        float3  b7  = b0.Axis();
        TEST( All( Equal( b4, float3{1.f, 0.f, 0.f}, 1_pct )));
        TEST( All( Equal( b5, float3{0.f, 1.f, 0.f}, 1_pct )));
        TEST( All( Equal( b6, float3{0.f, 0.f, 1.f}, 1_pct )));
        TEST( All( Equal( b4, b2.AxisX(), 1_pct )));
        TEST( All( Equal( b5, b2.AxisY(), 1_pct )));
        TEST( All( Equal( b6, b2.AxisZ(), 1_pct )));
        TEST( All( Equal( b7, float3{0.f, 0.f, 1.f}, 1_pct )));
        TEST( All( Equal( b4, b1.AxisX(), 1_pct )));
        TEST( All( Equal( b5, b1.AxisY(), 1_pct )));
        TEST( All( Equal( b6, b1.AxisZ(), 1_pct )));

        Quat    c0  = Quat::RotateY( 90_deg );      // -X
        Quat    c1  = Quat::LookAt( c0.AxisZ(), c0.AxisY() );
        Quat    c2  = Quat::RotateY( -270_deg );    // -X
        float3  c4  = c0.AxisX();
        float3  c5  = c0.AxisY();
        float3  c6  = c0.AxisZ();
        float3  c7  = c0.Axis();
        TEST( All( Equal( c4, float3{ 0.f, 0.f, 1.f}, 1_pct )));
        TEST( All( Equal( c5, float3{ 0.f, 1.f, 0.f}, 1_pct )));
        TEST( All( Equal( c6, float3{-1.f, 0.f, 0.f}, 1_pct )));
        TEST( All( Equal( c4, c2.AxisX(), 1_pct )));
        TEST( All( Equal( c5, c2.AxisY(), 1_pct )));
        TEST( All( Equal( c6, c2.AxisZ(), 1_pct )));
        TEST( All( Equal( c7, float3{ 0.f, 1.f, 0.f}, 1_pct )));
        TEST( All( Equal( c4, c1.AxisX(), 1_pct )));
        TEST( All( Equal( c5, c1.AxisY(), 1_pct )));
        TEST( All( Equal( c6, c1.AxisZ(), 1_pct )));

        Quat    d0  = Quat::RotateY( -90_deg );     // +X
        Quat    d1  = Quat::LookAt( d0.AxisZ(), d0.AxisY() );
        Quat    d2  = Quat::RotateY( 270_deg );     // +X
        float3  d4  = d0.AxisX();
        float3  d5  = d0.AxisY();
        float3  d6  = d0.AxisZ();
        float3  d7  = d0.Axis();
        TEST( All( Equal( d4, float3{0.f, 0.f, -1.f}, 1_pct )));
        TEST( All( Equal( d5, float3{0.f, 1.f,  0.f}, 1_pct )));
        TEST( All( Equal( d6, float3{1.f, 0.f,  0.f}, 1_pct )));
        TEST( All( Equal( d4, d2.AxisX(), 1_pct )));
        TEST( All( Equal( d5, d2.AxisY(), 1_pct )));
        TEST( All( Equal( d6, d2.AxisZ(), 1_pct )));
        TEST( All( Equal( d7, float3{0.f, -1.f, 0.f}, 1_pct )));
        TEST( All( Equal( d4, d1.AxisX(), 1_pct )));
        TEST( All( Equal( d5, d1.AxisY(), 1_pct )));
        TEST( All( Equal( d6, d1.AxisZ(), 1_pct )));

        Quat    e0  = Quat::RotateY( 180_deg );     // -Z
        Quat    e1  = Quat::LookAt( e0.AxisZ(), e0.AxisY() );
        Quat    e2  = Quat::RotateY( -180_deg );    // -Z
        float3  e4  = e0.AxisX();
        float3  e5  = e0.AxisY();
        float3  e6  = e0.AxisZ();
        float3  e7  = e0.Axis();
        TEST( All( Equal( e4, float3{-1.f, 0.f,  0.f}, 1_pct )));
        TEST( All( Equal( e5, float3{ 0.f, 1.f,  0.f}, 1_pct )));
        TEST( All( Equal( e6, float3{ 0.f, 0.f, -1.f}, 1_pct )));
        TEST( All( Equal( e4, e2.AxisX(), 1_pct )));
        TEST( All( Equal( e5, e2.AxisY(), 1_pct )));
        TEST( All( Equal( e6, e2.AxisZ(), 1_pct )));
        TEST( All( Equal( e7, float3{ 0.f, 1.f,  0.f}, 1_pct )));
        TEST( All( Equal( e4, e1.AxisX(), 1_pct )));
        TEST( All( Equal( e5, e1.AxisY(), 1_pct )));
        TEST( All( Equal( e6, e1.AxisZ(), 1_pct )));

        Quat    f0  = Quat::RotateX( 90_deg );      // -Y
        Quat    f1  = Quat::LookAt( f0.AxisZ(), f0.AxisY() );
        Quat    f2  = Quat::RotateX( -270_deg );    // -Y
        float3  f4  = f0.AxisX();
        float3  f5  = f0.AxisY();
        float3  f6  = f0.AxisZ();
        float3  f7  = f0.Axis();
        TEST( All( Equal( f4, float3{1.f, 0.f,  0.f}, 1_pct )));
        TEST( All( Equal( f5, float3{0.f, 0.f, -1.f}, 1_pct )));
        TEST( All( Equal( f6, float3{0.f, 1.f,  0.f}, 1_pct )));
        TEST( All( Equal( f4, f2.AxisX(), 1_pct )));
        TEST( All( Equal( f5, f2.AxisY(), 1_pct )));
        TEST( All( Equal( f6, f2.AxisZ(), 1_pct )));
        TEST( All( Equal( f7, float3{1.f, 0.f,  0.f}, 1_pct )));
        TEST( All( Equal( f4, f1.AxisX(), 1_pct )));
        TEST( All( Equal( f5, f1.AxisY(), 1_pct )));
        TEST( All( Equal( f6, f1.AxisZ(), 1_pct )));

        Quat    g0  = Quat::RotateX( -90_deg );     // +Y
        Quat    g1  = Quat::LookAt( g0.AxisZ(), g0.AxisY() );
        Quat    g2  = Quat::RotateX( 270_deg );     // +Y
        float3  g4  = g0.AxisX();
        float3  g5  = g0.AxisY();
        float3  g6  = g0.AxisZ();
        float3  g7  = g0.Axis();
        TEST( All( Equal( g4, float3{ 1.f,  0.f, 0.f}, 1_pct )));
        TEST( All( Equal( g5, float3{ 0.f,  0.f, 1.f}, 1_pct )));
        TEST( All( Equal( g6, float3{ 0.f, -1.f, 0.f}, 1_pct )));
        TEST( All( Equal( g4, g2.AxisX(), 1_pct )));
        TEST( All( Equal( g5, g2.AxisY(), 1_pct )));
        TEST( All( Equal( g6, g2.AxisZ(), 1_pct )));
        TEST( All( Equal( g7, float3{-1.f,  0.f, 0.f}, 1_pct )));
        TEST( All( Equal( g4, g1.AxisX(), 1_pct )));
        TEST( All( Equal( g5, g1.AxisY(), 1_pct )));
        TEST( All( Equal( g6, g1.AxisZ(), 1_pct )));
    }


    static void  Quat_Test2 ()
    {
        auto    a0 = Quat::Rotate2(Rad3{ 0_deg, 0_deg, 0_deg });
        TEST( All( Equal( a0, Quat::Identity() )));

        auto    a1 = Quat::Rotate2(Rad3{ 0_deg, 0_deg, 90_deg });
        auto    b1 = Quat::RotateZ( 90_deg );
        TEST( All( Equal( a1, b1 )));

        auto    a2 = Quat::Rotate2(Rad3{ 0_deg, 90_deg, 0_deg });
        auto    b2 = Quat::RotateY( 90_deg );
        TEST( All( Equal( a2, b2 )));

        auto    a3 = Quat::Rotate2(Rad3{ 90_deg, 0_deg, 0_deg });
        auto    b3 = Quat::RotateX( 90_deg );
        TEST( All( Equal( a3, b3 )));
    }


    static void  Quat_Test3 ()
    {
        const float3    up {0.f, 1.f, 0.f};

        float3  a0  { 0.780720f, 0.390360f, 0.487950f };
        Quat    a1  = Quat::LookAt( a0, up );
        float3  a2  = a1.ToDirection();
        TEST( All( Equal( a0, a2, 1_pct )));

        float3  b0  { -0.984594f, 0.172200f, -0.030361f };
        Quat    b1  = Quat::LookAt( b0, up );
        float3  b2  = b1.ToDirection();
        TEST( All( Equal( b0, b2, 1_pct )));
    }


    static void  Spherical_Test1 ()
    {
        float3  a0 { 0.780720f, 0.390360f, 0.487950f };
        auto    b0 = Spherical::FromCartesian( a0 ).first;
        float3  c0 = b0.ToCartesian();
        TEST( All( Equal( a0, c0, 1_pct )));
        /*
        for (Rad phi; phi < Rad::Pi2(); phi += 0.1f)
        for (Rad theta; theta < Rad::Pi(); theta += 0.1f)
        {
            Spherical   s       { phi, theta };
            float3      n       = s.ToCartesian();
            auto        [sp, r] = Spherical::FromCartesian( n );

            TEST( All( Equal( s, sp, 1_pct )));
            TEST( Equal( r, 1.f, 1_pct ));
        }*/
    }
}


extern void UnitTest_Math_Vec ()
{
    Vec_Test1();
    Vec_Test2();
    Vec_Test3();
    Vec_Test4();
    Vec_Test5();

    VecSwizzle_Test1();

    Quat_Test1();
    Quat_Test2();
    Quat_Test3();

    Spherical_Test1();

    TEST_PASSED();
}
