// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/PhysicalQuantity.h"
#include "base/Math/PhysicalQuantityVec.h"
#include "base/Math/PhysicalQuantityMatrix.h"
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
		using Meters			= DefaultPhysicalQuantity<float>::Meters;
		using SquareMeters		= DefaultPhysicalQuantity<float>::SquareMeters;
		using MetersPerSeconds	= DefaultPhysicalQuantity<float>::MetersPerSeconds;
		using Seconds			= DefaultPhysicalQuantity<float>::Seconds;
		using Milliseconds		= DefaultPhysicalQuantity<float>::Milliseconds;
		using Accel				= DefaultPhysicalQuantity<float>::MetersPerSquareSeconds;

		Meters				a1 = Meters{1.0f} + Meters{2.0f};						TEST(Equals( a1.GetNonScaled(), 3.0f ));
		MetersPerSeconds	a2 = Meters{10.0f} / Seconds{2.0f};						TEST(Equals( a2.GetNonScaled(), 5.0f ));
		MetersPerSeconds	a3 = Meters{14.0f} / Milliseconds{7.0f};				TEST(Equals( a3.GetNonScaled(), 2000.0f, 0.001f ));
		Accel				a4 = Meters{100.0f} / Seconds{2.0f} / Seconds{5.0f};	TEST(Equals( a4.GetNonScaled(), 10.0f ));
		SquareMeters		a5 = Meters{22.0f} * Meters{33.0f};						TEST(Equals( a5.GetNonScaled(), 726.0f ));
		SquareMeters		a6 = Meters{22.0f}.Pow<2>();							TEST(Equals( a6.GetNonScaled(), 484.0f ));
	}


	static void  PhysicalQuantity_Test2 ()
	{
		using SolarMass		= DefaultPhysicalQuantity<double>::SolarMass;
		using LightYears	= DefaultPhysicalQuantity<double>::LightYears;
		using GConstant		= DefaultPhysicalQuantity<double>::GConstant;
		using Kilograms		= DefaultPhysicalQuantity<double>::Kilograms;
		using Accel			= DefaultPhysicalQuantity<double>::MetersPerSquareSeconds;
		
		Kilograms	a1 = SolarMass{3.0e+6};						TEST(Equals( a1.GetNonScaled(), 1.98847e+30 * 3.0e+6 ));
		auto		a2 = GConstant{1.0} * SolarMass{3.0e+6};	TEST(Equals( a2.GetScaled(), 6.6740831e-11 * 1.98847e+30 * 3.0e+6 ));
		Accel		a3 = a2 / LightYears{26000.0}.Pow<2>();		TEST(Equals( a3.GetScaled(), 6.58e-15 ));

		TEST( ToString( GConstant{1.0}, 2, True{"exponent"} )	== "6.67e-11[m^3 / (s^-2 * kg^-1)]" );
		TEST( ToString( a2,				2, True{"exponent"} )	== "3.98e+26[m^3 / s^-2]" );
		TEST( ToString( a3,				2, True{"exponent"} )	== "6.58e-15[m / s^-2]" );
	}


	static void  PhysicalQuantityVec_Test1 ()
	{
		using Meters			= DefaultPhysicalQuantity<float>::Meters;
		using Seconds			= DefaultPhysicalQuantity<float>::Seconds;
		using MetersPerSeconds	= DefaultPhysicalQuantity<float>::MetersPerSeconds;
		using LightYears		= DefaultPhysicalQuantity<float>::LightYears;
		using Meters3			= PhysicalQuantitySIMDVec< Meters, 3 >;
		using Seconds3			= PhysicalQuantity_FromVec< float3, DefaultPhysicalDimensions::Second >;
		using MetersPerSeconds3	= PhysicalQuantitySIMDVec< MetersPerSeconds, 3 >;
		using LightYears3		= PhysicalQuantitySIMDVec< LightYears, 3 >;

		MetersPerSeconds3	a1 = Meters3{ 1.0f, 2.0f, 3.0f } / Seconds3{Seconds{4.0f}};
		Unused( a1 );

		float				b2 = Distance( float3{2.0f, 3.0f, 8.0f}, float3{-4.0f, 1.0f, 5.0f} );
		LightYears			a2 = Distance( LightYears3{2.0f, 3.0f, 8.0f}, LightYears3{-4.0f, 1.0f, 5.0f} );
		TEST(Equals( a2.GetNonScaled(), b2 ));
	
		float				b3 = DistanceSqr( float3{2.0f, 3.0f, 8.0f}, float3{-4.0f, 1.0f, 5.0f} );
		auto				a3 = DistanceSqr( LightYears3{2.0f, 3.0f, 8.0f}, LightYears3{-4.0f, 1.0f, 5.0f} );
		TEST(Equals( a3.GetNonScaled(), b3 ));

		LightYears			a4 = Min( a2, LightYears{1.0f} + LightYears{2.0f} );
		TEST(Equals( a4.GetNonScaled(), 3.0f ));
	}

	
	static void  PhysicalQuantityMatrix_Test1 ()
	{
		using Meters				= DefaultPhysicalQuantity<float>::Meters;
		using Seconds				= DefaultPhysicalQuantity<float>::Seconds;
		using MetersPerSeconds		= DefaultPhysicalQuantity<float>::MetersPerSeconds;
		using Meters3				= PhysicalQuantitySIMDVec< Meters, 3 >;
		using Seconds3				= PhysicalQuantitySIMDVec< Seconds, 3 >;
		using MetersPerSeconds3		= PhysicalQuantitySIMDVec< MetersPerSeconds, 3 >;
		using Meters3x3				= PhysicalQuantitySIMDMatrix< Meters, 3, 3 >;
		using Seconds3x3			= PhysicalQuantitySIMDMatrix< Seconds, 3, 3 >;
		using MetersPerSeconds3x3	= PhysicalQuantitySIMDMatrix< MetersPerSeconds, 3, 3 >;

		Meters3x3	m = Meters3x3::FromScalar( Meters{1.0f} );


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
