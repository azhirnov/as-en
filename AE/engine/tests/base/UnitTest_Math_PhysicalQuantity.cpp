// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  PhysicalDimensions_Test1 ()
    {
        using Dim = DefaultPhysicalDimensions;

        STATIC_ASSERT( Dim::MeterPerSecond::meters.num == 1 );
        STATIC_ASSERT( Dim::MeterPerSecond::meters.den == 1 );
        STATIC_ASSERT( Dim::MeterPerSecond::seconds.num == -1 );
        STATIC_ASSERT( Dim::MeterPerSecond::seconds.den == 1 );

        using M2 = DefaultPhysicalDimensions::Meter::Pow<2>;

        STATIC_ASSERT( M2::meters.num == 2 );
        STATIC_ASSERT( M2::meters.den == 1 );
        STATIC_ASSERT( M2::seconds.num == 0 );
        STATIC_ASSERT( M2::seconds.den == 1 );
    }


    static void  PhysicalQuantity_Test1 ()
    {
        using Meter             = DefaultPhysicalQuantity<float>::Meter;
        using SquareMeter       = DefaultPhysicalQuantity<float>::SquareMeter;
        using MeterPerSecond    = DefaultPhysicalQuantity<float>::MeterPerSecond;
        using Second            = DefaultPhysicalQuantity<float>::Second;
        using Millisecond       = DefaultPhysicalQuantity<float>::Millisecond;
        using Accel             = DefaultPhysicalQuantity<float>::MeterPerSquareSecond;

        Meter           a1 = Meter{1.0f} + Meter{2.0f};                     TEST(Equals( a1.GetNonScaled(), 3.0f ));
        auto            a2 = Meter{10.0f} / Second{2.0f};                   TEST(Equals( a2.GetNonScaled(), 5.0f ));
        MeterPerSecond  a3 = Meter{14.0f} / Millisecond{7.0f};              TEST(Equals( a3.GetNonScaled(), 2000.0f, 0.001f ));
        Accel           a4 = Meter{100.0f} / Second{2.0f} / Second{5.0f};   TEST(Equals( a4.GetNonScaled(), 10.0f ));
        SquareMeter     a5 = Meter{22.0f} * Meter{33.0f};                   TEST(Equals( a5.GetNonScaled(), 726.0f ));
        SquareMeter     a6 = Meter{22.0f}.Pow<2>();                         TEST(Equals( a6.GetNonScaled(), 484.0f ));

        TEST_Eq( ToString( a2, 2, False{} ), "5.00[m / s^-1]" );
        TEST_Eq( ToString( a3, 2, False{} ), "2000.00[m / s^-1]" );
    }


    static void  PhysicalQuantity_Test2 ()
    {
        using SolarMass     = DefaultPhysicalQuantity<double>::SolarMass;
        using LightYear     = DefaultPhysicalQuantity<double>::LightYear;
        using GConstant     = DefaultPhysicalQuantity<double>::GConstant;
        using Kilogram      = DefaultPhysicalQuantity<double>::Kilogram;
        using Accel         = DefaultPhysicalQuantity<double>::MeterPerSquareSecond;

        Kilogram    a1 = SolarMass{3.0e+6};                     TEST(Equals( a1.GetNonScaled(), 1.98847e+30 * 3.0e+6 ));
        auto        a2 = GConstant{1.0} * SolarMass{3.0e+6};    TEST(Equals( a2.GetScaled(), 6.6740831e-11 * 1.98847e+30 * 3.0e+6 ));
        Accel       a3 = a2 / LightYear{26000.0}.Pow<2>();      TEST(Equals( a3.GetScaled(), 6.58e-15 ));

        TEST_Eq( ToString( GConstant{1.0},  2, True{"exponent"} ), "6.67e-11[m^3 / (s^-2 * kg^-1)]" );
        TEST_Eq( ToString( a2,              2, True{"exponent"} ), "3.98e+26[m^3 / s^-2]" );
        TEST_Eq( ToString( a3,              2, True{"exponent"} ), "6.58e-15[m / s^-2]" );
    }


    static void  PhysicalQuantityVec_Test1 ()
    {
        using Meter             = DefaultPhysicalQuantity<float>::Meter;
        using Second            = DefaultPhysicalQuantity<float>::Second;
        using MeterPerSecond    = DefaultPhysicalQuantity<float>::MeterPerSecond;
        using LightYear         = DefaultPhysicalQuantity<float>::LightYear;
        using Meter3            = PhysicalQuantitySIMDVec< Meter, 3 >;
        using Second3           = PhysicalQuantity_FromVec< float3, DefaultPhysicalDimensions::Second >;
        using MeterPerSecond3   = PhysicalQuantitySIMDVec< MeterPerSecond, 3 >;
        using LightYear3        = PhysicalQuantitySIMDVec< LightYear, 3 >;

        MeterPerSecond3     a1 = Meter3{ 1.0f, 2.0f, 3.0f } / Second3{Second{4.0f}};
        Unused( a1 );

        float               b2 = Distance( float3{2.0f, 3.0f, 8.0f}, float3{-4.0f, 1.0f, 5.0f} );
        LightYear           a2 = Distance( LightYear3{2.0f, 3.0f, 8.0f}, LightYear3{-4.0f, 1.0f, 5.0f} );
        TEST(Equals( a2.GetNonScaled(), b2 ));

        float               b3 = DistanceSq( float3{2.0f, 3.0f, 8.0f}, float3{-4.0f, 1.0f, 5.0f} );
        auto                a3 = DistanceSq( LightYear3{2.0f, 3.0f, 8.0f}, LightYear3{-4.0f, 1.0f, 5.0f} );
        TEST(Equals( a3.GetNonScaled(), b3 ));

        LightYear           a4 = Min( a2, LightYear{1.0f} + LightYear{2.0f} );
        TEST(Equals( a4.GetNonScaled(), 3.0f ));
    }


    static void  PhysicalQuantityMatrix_Test1 ()
    {
        using Meter             = DefaultPhysicalQuantity<float>::Meter;
        using Second            = DefaultPhysicalQuantity<float>::Second;
        using MeterPerSecond    = DefaultPhysicalQuantity<float>::MeterPerSecond;
        using Meter3            = PhysicalQuantitySIMDVec< Meter, 3 >;
        using Second3           = PhysicalQuantitySIMDVec< Second, 3 >;
        using MeterPerSecond3   = PhysicalQuantitySIMDVec< MeterPerSecond, 3 >;
        using Meter3x3          = PhysicalQuantitySIMDMatrix< Meter, 3, 3 >;
        using Second3x3         = PhysicalQuantitySIMDMatrix< Second, 3, 3 >;
        using MeterPerSecond3x3 = PhysicalQuantitySIMDMatrix< MeterPerSecond, 3, 3 >;

        Meter3x3    m = Meter3x3::FromScalar( Meter{1.0f} );
        Unused( m );
    }
}


extern void UnitTest_Math_PhysicalQuantity ()
{
    PhysicalDimensions_Test1();
    PhysicalQuantity_Test1();
    PhysicalQuantity_Test2();
    PhysicalQuantityVec_Test1();
    PhysicalQuantityMatrix_Test1();

    TEST_PASSED();
}
