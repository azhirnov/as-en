// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  PhysicalDimensions_Test1 ()
	{
		using Dim = DefaultPhysicalDimensions;

		StaticAssert( Dim::MeterPerSecond::meters.num == 1 );
		StaticAssert( Dim::MeterPerSecond::meters.den == 1 );
		StaticAssert( Dim::MeterPerSecond::seconds.num == -1 );
		StaticAssert( Dim::MeterPerSecond::seconds.den == 1 );

		using M2 = DefaultPhysicalDimensions::Meter::Pow<2>;

		StaticAssert( M2::meters.num == 2 );
		StaticAssert( M2::meters.den == 1 );
		StaticAssert( M2::seconds.num == 0 );
		StaticAssert( M2::seconds.den == 1 );
	}


	static void  PhysicalDimensions_Test2 ()
	{
		using PQ = DefaultPhysicalQuantity<float>;

		StaticAssert( IsTimeUnits< PQ::Second >);
		StaticAssert( not IsTimeUnits< PQ::Kilogram >);
		StaticAssert( not IsTimeUnits< PQ::Meter >);
		StaticAssert( not IsTimeUnits< PQ::Ampere >);
		StaticAssert( not IsTimeUnits< PQ::Kelvin >);
		StaticAssert( not IsTimeUnits< PQ::Mole >);
		StaticAssert( not IsTimeUnits< PQ::Candela >);
		StaticAssert( not IsTimeUnits< PQ::Currency >);
		StaticAssert( not IsTimeUnits< PQ::Bit >);

		StaticAssert( IsMassUnits< PQ::Kilogram >);
		StaticAssert( IsDistanceUnits< PQ::Meter >);
		StaticAssert( IsElectricCurrentUnits< PQ::Ampere >);
		StaticAssert( IsTemperatureUnits< PQ::Kelvin >);
		StaticAssert( IsSubstanceAmountUnits< PQ::Mole >);
		StaticAssert( IsLuminousIntensityUnits< PQ::Candela >);
		StaticAssert( IsCurrencyUnits< PQ::Currency >);

		StaticAssert( IsInformationUnits< PQ::Bit >);
		StaticAssert( IsInformationUnits< PQ::KibiBit >);
		StaticAssert( IsInformationUnits< PQ::KibiByte >);
		StaticAssert( not IsInformationUnits< PQ::BitPerSecond >);
		StaticAssert( not IsInformationUnits< PQ::KibiBytePerSecond >);

		StaticAssert( IsDensityUnits< PQ::KilogramPerCubicMeter >);
		StaticAssert( IsVelocityUnits< PQ::MeterPerSecond >);
		StaticAssert( IsAccelerationUnits< PQ::MeterPerSquareSecond >);
	}


	static void  PhysicalQuantity_Test1 ()
	{
		using PQ				= DefaultPhysicalQuantity<float>;
		using Meters			= PQ::Meter;
		using SquareMeters		= PQ::SquareMeter;
		using MetersPerSecond	= PQ::MeterPerSecond;
		using Seconds			= PQ::Second;
		using Milliseconds		= PQ::Millisecond;
		using Accel				= PQ::MeterPerSquareSecond;

		Meters				a1 = Meters{1.0f} + Meters{2.0f};						TEST( BitEqual( a1.GetNonScaled(), 3.0f ));
		auto				a2 = Meters{10.0f} / Seconds{2.0f};						TEST( BitEqual( a2.GetNonScaled(), 5.0f ));
		MetersPerSecond		a3 = Meters{14.0f} / Milliseconds{7.0f};				TEST( BitEqual( a3.GetNonScaled(), 2000.0f ));
		Accel				a4 = Meters{100.0f} / Seconds{2.0f} / Seconds{5.0f};	TEST( BitEqual( a4.GetNonScaled(), 10.0f ));
		SquareMeters		a5 = Meters{22.0f} * Meters{33.0f};						TEST( BitEqual( a5.GetNonScaled(), 726.0f ));
		SquareMeters		a6 = Meters{22.0f}.Pow<2>();							TEST( BitEqual( a6.GetNonScaled(), 484.0f ));

		TEST_Eq( ToString( a2, 2, False{} ), "5.00[m / s^-1]" );
		TEST_Eq( ToString( a3, 2, False{} ), "2000.00[m / s^-1]" );

		StaticAssert( IsTriviallySerializable< Meters >);
		StaticAssert( IsTriviallyDestructible< Meters >);
		StaticAssert( IsZeroMemAvailable< Meters >);
		StaticAssert( IsMemCopyAvailable< Meters >);
	}


	static void  PhysicalQuantity_Test2 ()
	{
		using PQ			= DefaultPhysicalQuantity<double>;
		using SolarMass		= PQ::SolarMass;
		using LightYears	= PQ::LightYear;
		using GConstant		= PQ::GConstant;
		using Kilograms		= PQ::Kilogram;
		using Accel			= PQ::MeterPerSquareSecond;

		Kilograms	a1 = SolarMass{3.0e+6};						TEST( BitEqual( a1.GetNonScaled(),	1.98847e+30 * 3.0e+6 ));
		auto		a2 = GConstant{1.0} * SolarMass{3.0e+6};	TEST( BitEqual( a2.GetScaled(),		6.6740831e-11 * 1.98847e+30 * 3.0e+6 ));
		Accel		a3 = a2 / LightYears{26000.0}.Pow<2>();		TEST( Equal( a3.GetScaled(),		6.58e-15,  10_pct ));

		TEST_Eq( ToString( GConstant{1.0},	2, True{"exponent"} ), "6.67e-11[m^3 / (s^-2 * kg^-1)]" );
		TEST_Eq( ToString( a2,				2, True{"exponent"} ), "3.98e+26[m^3 / s^-2]" );
		TEST_Eq( ToString( a3,				2, True{"exponent"} ), "6.58e-15[m / s^-2]" );
	}


	static void  PhysicalQuantity_Test3 ()
	{
		using PQ			= DefaultPhysicalQuantity<double>;
		using Seconds		= PQ::Second;
		using Nanoseconds	= PQ::Nanosecond;
		using Hours			= PQ::Hour;
		using FBytes		= PQ::Byte;

		Seconds			a0 {secondsd{1.0}};			TEST( BitEqual( a0.GetNonScaled(), 1.0 ));
		Nanoseconds		a1 {secondsd{1.0}};			TEST( BitEqual( a1.GetNonScaled(), 1.0e+9 ));
		Hours			a2 {secondsd{1.0e+10}};		TEST( BitEqual( a2.GetNonScaled(), 1.0e+10 / (60.0 * 60.0) ));
		Seconds			a3 {seconds{22}};			TEST( BitEqual( a3.GetScaled(), 22.0 ));
		Seconds			a4 {milliseconds{3}};		TEST( BitEqual( a4.GetScaled(), 3.0e-3 ));
		Seconds			a5 {nanoseconds{14}};		TEST( BitEqual( a5.GetScaled(), 14.0e-9 ));

		nanoseconds		b0 {Seconds{1.0}};			TEST( Equal( slong(b0.count()), slong{1'000'000'000} ));

		FBytes			c0 {Bytes{1u << 20}};		TEST( BitEqual( c0.GetScaled(), double(1u<<20) ));

		Bytes			d0 {FBytes{1.0e+9}};		TEST( Equal( ulong{d0}, ulong{1'000'000'000} ));
	}


	static void  PhysicalQuantityVec_Test1 ()
	{
		using PQ				= DefaultPhysicalQuantity<float>;
		using Meters			= PQ::Meter;
		using Seconds			= PQ::Second;
		using MetersPerSecond	= PQ::MeterPerSecond;
		using LightYears		= PQ::LightYear;
		using Meters3			= PhysicalQuantitySIMDVec< Meters, 3 >;
		using Seconds3			= PhysicalQuantity_FromVec< float3, DefaultPhysicalDimensions::Second >;
		using MetersPerSecond3	= PhysicalQuantitySIMDVec< MetersPerSecond, 3 >;
		using LightYears3		= PhysicalQuantitySIMDVec< LightYears, 3 >;

		MetersPerSecond3	a1 = Meters3{ 1.0f, 2.0f, 3.0f } / Seconds3{Seconds{4.0f}};
		Unused( a1 );

		float				b2 = Distance( float3{2.0f, 3.0f, 8.0f}, float3{-4.0f, 1.0f, 5.0f} );
		LightYears			a2 = Distance( LightYears3{2.0f, 3.0f, 8.0f}, LightYears3{-4.0f, 1.0f, 5.0f} );
		TEST( BitEqual( a2.GetNonScaled(), b2 ));

		float				b3 = DistanceSq( float3{2.0f, 3.0f, 8.0f}, float3{-4.0f, 1.0f, 5.0f} );
		auto				a3 = DistanceSq( LightYears3{2.0f, 3.0f, 8.0f}, LightYears3{-4.0f, 1.0f, 5.0f} );
		TEST( BitEqual( a3.GetNonScaled(), b3 ));

		LightYears			a4 = Min( a2, LightYears{1.0f} + LightYears{2.0f} );
		TEST( BitEqual( a4.GetNonScaled(), 3.0f ));
	}


	static void  PhysicalQuantityMatrix_Test1 ()
	{
		using PQ					= DefaultPhysicalQuantity<float>;
		using Meters				= PQ::Meter;
		using Seconds				= PQ::Second;
		using MetersPerSecond		= PQ::MeterPerSecond;
		using Meters3				= PhysicalQuantitySIMDVec< Meters, 3 >;
		using Seconds3				= PhysicalQuantitySIMDVec< Seconds, 3 >;
		using MetersPerSecond3		= PhysicalQuantitySIMDVec< MetersPerSecond, 3 >;
		using Meters3x3				= PhysicalQuantitySIMDMatrix< Meters, 3, 3 >;
		using Seconds3x3			= PhysicalQuantitySIMDMatrix< Seconds, 3, 3 >;
		using MetersPerSecond3x3	= PhysicalQuantitySIMDMatrix< MetersPerSecond, 3, 3 >;

		Meters3x3	m = Meters3x3::FromScalar( Meters{1.0f} );
		Unused( m );
	}
}


extern void UnitTest_Math_PhysicalQuantity ()
{
	PhysicalDimensions_Test1();
	PhysicalDimensions_Test2();

	PhysicalQuantity_Test1();
	PhysicalQuantity_Test2();
	PhysicalQuantity_Test3();

	PhysicalQuantityVec_Test1();

	PhysicalQuantityMatrix_Test1();

	TEST_PASSED();
}
