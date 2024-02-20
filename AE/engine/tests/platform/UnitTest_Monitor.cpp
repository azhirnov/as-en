// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/Monitor.h"
#include "../shared/UnitTest_Shared.h"
using namespace AE::App;

namespace
{
    using EOrientation = Monitor::EOrientation;


    static void  Monitor_Test1 ()
    {
        int2    a0 = Monitor::_Rotate( EOrientation::Orient_0_deg, int2{200, 100}, int2{1080, 2160} );
        TEST_Eq( a0, int2(200, 100) );

        int2    a1 = Monitor::_Rotate( EOrientation::Orient_90_deg, int2{200, 100}, int2{1080, 2160} );
        TEST_Eq( a1, int2(100, 1080-200) );

        int2    a2 = Monitor::_Rotate( EOrientation::Orient_180_deg, int2{200, 100}, int2{1080, 2160} );
        TEST_Eq( a2, int2(1080-200, 2160-100) );

        int2    a3 = Monitor::_Rotate( EOrientation::Orient_270_deg, int2{200, 100}, int2{1080, 2160} );
        TEST_Eq( a3, int2(2160-100, 200) );

        RectI   b0 = Monitor::_Rotate( EOrientation::Orient_0_deg, RectI{0, 0, 200, 100}, int2{1080, 2160} );
        TEST( b0.IsValid() );
        TEST_Eq( b0, RectI(0, 0, 200, 100) );

        RectI   b1 = Monitor::_Rotate( EOrientation::Orient_90_deg, RectI{0, 0, 200, 100}, int2{1080, 2160} );
        TEST( b1.IsValid() );
        TEST_Eq( b1, RectI(0, 1080-200, 100, 1080) );

        RectI   b2 = Monitor::_Rotate( EOrientation::Orient_180_deg, RectI{0, 0, 200, 100}, int2{1080, 2160} );
        TEST( b2.IsValid() );
        TEST_Eq( b2, RectI(1080-200, 2160-100, 1080, 2160) );

        RectI   b3 = Monitor::_Rotate( EOrientation::Orient_270_deg, RectI{0, 0, 200, 100}, int2{1080, 2160} );
        TEST( b3.IsValid() );
        TEST_Eq( b3, RectI(2160-100, 0, 2160, 200) );
    }


    static void  Monitor_Test2 ()
    {
        Monitor     m;
        m.orient = EOrientation::Orient_0_deg;

        float2x2    a0 = m.RotationMatrix();
        float2      b0 = a0 * float2{1.f, 1.f};
        TEST( All( Equal( b0, float2{1.f, 1.f} )));

        m.orient = EOrientation::Orient_90_deg;

        float2x2    a1 = m.RotationMatrix();
        float2      b1 = a1 * float2{1.f, 1.f};
        TEST( All( Equal( b1, float2{-1.f, 1.f} )));

        m.orient = EOrientation::Orient_180_deg;

        float2x2    a2 = m.RotationMatrix();
        float2      b2 = a2 * float2{1.f, 1.f};
        TEST( All( Equal( b2, float2{-1.f, -1.f} )));

        m.orient = EOrientation::Orient_270_deg;

        float2x2    a3 = m.RotationMatrix();
        float2      b3 = a3 * float2{1.f, 1.f};
        TEST( All( Equal( b3, float2{1.f, -1.f} )));
    }
}


extern void UnitTest_Monitor ()
{
    Monitor_Test1();
    Monitor_Test2();

    TEST_PASSED();
}
