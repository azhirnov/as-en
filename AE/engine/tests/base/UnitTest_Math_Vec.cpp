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
        const float3    up {0.f, -1.f, 0.f};

        float3  a0 { 0.780720f, 0.390360f, 0.487950f };
        auto    b0 = Quat::FromDirection( a0, up );
        auto    c0  = b0.ToDirection();
        TEST( All( Equal( a0, c0, 1_pct )));
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

    Spherical_Test1();

    TEST_PASSED();
}
