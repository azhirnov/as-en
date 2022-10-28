// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/PhysicalDimension.h"
#include "base/Math/ValueScaleTempl.h"

namespace AE::Math
{

	template <typename ValueType,
			  typename Dimension,
			  typename ValueScale = ValueScaleTempl::Integer<ValueType, 1>
			 >
	struct PhysicalQuantity;



	//
	// Physical Quantity
	//

	template <typename ValueType,
			  typename Dimension,
			  typename ValueScale
			 >
	struct PhysicalQuantity
	{
		STATIC_ASSERT( IsFloatPoint<ValueType> and IsScalar<ValueType> );

	// types
	public:
		using Value_t		= ValueType;
		using Scale_t		= ValueScale;
		using Dimension_t	= Dimension;
		using Self			= PhysicalQuantity< Value_t, Dimension, Scale_t >;
		using Inversed		= PhysicalQuantity< Value_t,
												DefaultPhysicalDimensions::NonDimensional::template Div< Dimension_t >,
												typename ValueScaleTempl::template Inverse< Scale_t > >;

	// variables
	private:
		Value_t		_value;


	// methods
	public:
			constexpr PhysicalQuantity () : _value{0} {}

			constexpr explicit PhysicalQuantity (Value_t value) : _value{value} {}

			template <typename S>
			constexpr PhysicalQuantity (const PhysicalQuantity<Value_t, Dimension_t, S> &other) :
				_value{ other.template ToScale<Scale_t>().GetNonScaled() }
			{}

		ND_ constexpr Self  operator - ()					const	{ return Self{ -_value }; }

		ND_ constexpr bool	operator == (const Self &rhs)	const	{ return _value == rhs._value; }
		ND_ constexpr bool	operator != (const Self &rhs)	const	{ return _value != rhs._value; }
		ND_ constexpr bool	operator >  (const Self &rhs)	const	{ return _value >  rhs._value; }
		ND_ constexpr bool	operator >= (const Self &rhs)	const	{ return _value >= rhs._value; }
		ND_ constexpr bool	operator <  (const Self &rhs)	const	{ return _value <  rhs._value; }
		ND_ constexpr bool	operator <= (const Self &rhs)	const	{ return _value <= rhs._value; }

			constexpr Self&	operator += (const Self &rhs)			{ _value += rhs._value;  return *this; }
			constexpr Self&	operator -= (const Self &rhs)			{ _value -= rhs._value;  return *this; }

		ND_ constexpr Self	operator +  (const Self &rhs)	const	{ return Self( _value + rhs._value ); }
		ND_ constexpr Self	operator -  (const Self &rhs)	const	{ return Self( _value - rhs._value ); }

			constexpr Self&	operator *= (Value_t rhs)				{ _value *= rhs;  return *this; }
			constexpr Self&	operator /= (Value_t rhs)				{ _value /= rhs;  return *this; }

		ND_ constexpr Self	operator *  (Value_t rhs)		const	{ return Self( _value * rhs ); }
		ND_ constexpr Self	operator /  (Value_t rhs)		const	{ return Self( _value / rhs ); }
		
		ND_ constexpr Value_t	GetNonScaled ()				const	{ return _value; }
		ND_ constexpr Value_t&	GetNonScaledRef ()					{ return _value; }
		ND_ constexpr Value_t	GetScaled ()				const	{ return _value * Scale_t::Value; }
		

		template <typename S>
		ND_ constexpr auto  operator +  (const PhysicalQuantity<Value_t, Dimension_t, S> &rhs) const
		{
			using Scale = ValueScaleTempl::template Add< Scale_t, S >;
			return PhysicalQuantity< Value_t, Dimension_t, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}
		
		template <typename S>
		ND_ constexpr auto  operator -  (const PhysicalQuantity<Value_t, Dimension_t, S> &rhs) const
		{
			using Scale = ValueScaleTempl::template Sub< Scale_t, S >;
			return PhysicalQuantity< Value_t, Dimension_t, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}
		
		template <typename D, typename S>
		ND_ constexpr auto  operator *  (const PhysicalQuantity<Value_t,D,S> &rhs) const
		{
			using Scale = ValueScaleTempl::template Mul< Scale_t, S >;
			return PhysicalQuantity< Value_t, typename Dimension_t::template Mul<D>, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}
		
		template <typename D, typename S>
		ND_ constexpr auto  operator /  (const PhysicalQuantity<Value_t,D,S> &rhs) const
		{
			using Scale = ValueScaleTempl::template Div< Scale_t, S >; 
			return PhysicalQuantity< Value_t, typename Dimension_t::template Div<D>, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}


		ND_ friend constexpr Self  operator * (Value_t lhs, const Self &rhs)
		{
			return Self( lhs * rhs.GetNonScaled() );
		}

		template <typename ToValueScale>
		ND_ constexpr PhysicalQuantity< Value_t, Dimension_t, ToValueScale >  ToScale () const
		{
			const auto	val = Scale_t::Value / ToValueScale::Value;
			return PhysicalQuantity< Value_t, Dimension_t, ToValueScale >{ _value * val };
		}

		ND_ friend constexpr Inversed  operator / (Value_t lhs, const Self &rhs)
		{
			return Inversed{ lhs / rhs.GetNonScaled() };
		}
		
		template <int IntPower>
		ND_ constexpr auto  Pow () const
		{
			using Dim	= typename Dimension_t::template Pow< IntPower >;
			using Scale	= ValueScaleTempl::template Pow< Scale_t, IntPower >;
			return PhysicalQuantity< Value_t, Dim, Scale >{ Math::Pow( _value, Value_t{IntPower} )};
		}
	};
	


	//
	// Non-dimensional Physical Quantity
	//

	template <typename ValueType,
			  typename ValueScale
			 >
	struct PhysicalQuantity< ValueType, DefaultPhysicalDimensions::NonDimensional, ValueScale >
	{
		STATIC_ASSERT( IsFloatPoint<ValueType> and IsScalar<ValueType> );

	// types
	public:
		using Value_t		= ValueType;
		using Scale_t		= ValueScale;
		using Dimension_t	= DefaultPhysicalDimensions::NonDimensional;
		using Self			= PhysicalQuantity< Value_t, Dimension_t, Scale_t >;
		using Inversed		= PhysicalQuantity< Value_t,
												Dimension_t,
												typename ValueScaleTempl::template Inverse< Scale_t > >;

	// variables
	private:
		Value_t		_value;


	// methods
	public:
			constexpr PhysicalQuantity () : _value{0} {}

			constexpr explicit PhysicalQuantity (Value_t value) : _value{value} {}
		
			constexpr PhysicalQuantity (const Self &) = default;
			constexpr Self&	operator = (const Self &) = default;
			
		ND_ constexpr operator Value_t ()					const	{ return GetScaled(); }

		ND_ constexpr bool	operator == (const Self &rhs)	const	{ return _value == rhs.GetNonScaled(); }
		ND_ constexpr bool	operator != (const Self &rhs)	const	{ return _value != rhs.GetNonScaled(); }
		ND_ constexpr bool	operator >  (const Self &rhs)	const	{ return _value >  rhs.GetNonScaled(); }
		ND_ constexpr bool	operator >= (const Self &rhs)	const	{ return _value >= rhs.GetNonScaled(); }
		ND_ constexpr bool	operator <  (const Self &rhs)	const	{ return _value <  rhs.GetNonScaled(); }
		ND_ constexpr bool	operator <= (const Self &rhs)	const	{ return _value <= rhs.GetNonScaled(); }

			constexpr Self&	operator += (const Self &rhs)			{ _value += rhs.GetNonScaled();  return *this; }
			constexpr Self&	operator -= (const Self &rhs)			{ _value -= rhs.GetNonScaled();  return *this; }
		
		ND_ constexpr Self	operator +  (const Self &rhs)	const	{ return Self{ _value + rhs.GetNonScaled() }; }
		ND_ constexpr Self	operator -  (const Self &rhs)	const	{ return Self{ _value - rhs.GetNonScaled() }; }

			constexpr Self&	operator *= (Value_t rhs)				{ _value *= rhs;  return *this; }
			constexpr Self&	operator /= (Value_t rhs)				{ _value /= rhs;  return *this; }
		
		ND_ constexpr Self	operator *  (Value_t rhs)		const	{ return Self{ _value * rhs }; }
		ND_ constexpr Self	operator /  (Value_t rhs)		const	{ return Self{ _value / rhs }; }
		
		ND_ constexpr Value_t	GetNonScaled ()				const	{ return _value; }
		ND_ constexpr Value_t	GetScaled ()				const	{ return _value * Scale_t::Value; }


		ND_ friend constexpr Self	operator * (Value_t left, const Self &right)
		{
			return Self{ left * right.GetNonScaled() };
		}
	
		ND_ friend constexpr Inversed	operator / (Value_t left, const Self &right)
		{
			return Inversed{ left / right.GetNonScaled() };
		}

		template <int IntPower>
		ND_ constexpr auto  Pow () const
		{
			using Scale	= ValueScaleTempl::template Pow< Scale_t, IntPower >;
			return PhysicalQuantity< Value_t, Dimension_t, Scale >{ Math::Pow( _value, Value_t{IntPower} )};
		}
	};


	
/*
=================================================
	Lerp
=================================================
*/
	template <typename T, typename Dimension, typename LhsScale, typename RhsScale, typename B>
	ND_ constexpr auto  Lerp (const PhysicalQuantity<T, Dimension, LhsScale> &a,
							  const PhysicalQuantity<T, Dimension, RhsScale> &b, const B& factor)
	{
		return a * (T{1} - T{factor}) + b * T(factor);
	}
//-----------------------------------------------------------------------------

	

	//
	// Default Physical Quantitiy
	//

	template <typename T>
	struct DefaultPhysicalQuantity
	{
		struct Scale
		{
			struct Minute			{ static constexpr T  Value = T(60); };						// m
			struct Hour				{ static constexpr T  Value = T(60 * 60); };				// h
			struct Day				{ static constexpr T  Value = T(24 * 60 * 60); };			// d
			struct Year				{ static constexpr T  Value = T(36525 * 24 * 6 * 6); };		// y

			struct Deca				{ static constexpr T  Value = T(10); };
			struct Hecto			{ static constexpr T  Value = T(1.0e+2); };
			struct Kilo				{ static constexpr T  Value = T(1.0e+3); };
			struct Mega				{ static constexpr T  Value = T(1.0e+6); };
			struct Giga				{ static constexpr T  Value = T(1.0e+9); };
			struct Tera				{ static constexpr T  Value = T(1.0e+12); };
			struct Peta				{ static constexpr T  Value = T(1.0e+15); };
			struct Exa				{ static constexpr T  Value = T(1.0e+18); };
			struct Zetta			{ static constexpr T  Value = T(1.0e+21); };
			struct Yotta			{ static constexpr T  Value = T(1.0e+24); };
			
			struct Kibi				{ static constexpr T  Value = T(1024.0); };			// kilo  / kibi
			struct Mebi				{ static constexpr T  Value = T(1048.0e+3); };		// mega  / mebi
			struct Gibi				{ static constexpr T  Value = T(1073.7e+6); };		// giga  / gibi
			struct Tebi				{ static constexpr T  Value = T(1099.5e+9); };		// tera  / tebi
			struct Pebi				{ static constexpr T  Value = T(1125.9e+12); };		// peta  / pebi
			struct Exbi				{ static constexpr T  Value = T(1152.9e+15); };		// exa   / exbi
			struct Zebi				{ static constexpr T  Value = T(1180.6e+18); };		// zetta / zebi
			struct Yobi				{ static constexpr T  Value = T(1208.9e+21); };		// yotta / yobi

			struct Deci				{ static constexpr T  Value = T(1.0e-1); };
			struct Centi			{ static constexpr T  Value = T(1.0e-2); };
			struct Milli			{ static constexpr T  Value = T(1.0e-3); };
			struct Micro			{ static constexpr T  Value = T(1.0e-6); };
			struct Nano				{ static constexpr T  Value = T(1.0e-9); };
			struct Pico				{ static constexpr T  Value = T(1.0e-12); };
			struct Femto			{ static constexpr T  Value = T(1.0e-15); };
			struct Atto				{ static constexpr T  Value = T(1.0e-18); };
			struct Zepto			{ static constexpr T  Value = T(1.0e-21); };
			struct Yocto			{ static constexpr T  Value = T(1.0e-24); };

			struct Litre			{ static constexpr T  Value = T(1.0e-3); };					// m^3	L

			struct ElectronVolt		{ static constexpr T  Value = T(1.602176620898e-19); };		// J	eV
			struct Dalton			{ static constexpr T  Value = T(1.66053904020e-27); };		// kg	Da	amu
			struct AstronomicalUnit	{ static constexpr T  Value = T(149597870700.0); };			// m	au

			struct Angstrom			{ static constexpr T  Value = T(1.0e-10); };				// m	A
			struct Bar				{ static constexpr T  Value = T(1.0e+5); };					// Pa	bar
			struct Atmosphere		{ static constexpr T  Value = T(101325); };					// Pa

			struct SpeedOfLight		{ static constexpr T  Value = T(299792458); };				// m/s	c
			struct SpeedOfGravity	{ static constexpr T  Value = T(299792458); };				// m/s	?
			struct Parsec			{ static constexpr T  Value = T(3.0856776e+16); };			// m	pc
			using LightMinute		= ValueScaleTempl::template Mul< SpeedOfLight, Minute >;
			using LightHour			= ValueScaleTempl::template Mul< SpeedOfLight, Hour >;
			using LightDay			= ValueScaleTempl::template Mul< SpeedOfLight, Day >;
			using LightYear			= ValueScaleTempl::template Mul< SpeedOfLight, Year >;

			struct GravitationalConstant	{ static constexpr T  Value = T(6.6740831e-11); };	// m^3 / (s^2 * kg)
			struct GravitationalAcceleration{ static constexpr T  Value = T(9.80665); };		// m / s^2

			struct AvogadroConstant			{ static constexpr T  Value = T(6.02214076e+23); };						// 1 / mol
			struct Pi						{ static constexpr T  Value = T(3.14159265358979323846); };				// Pi
			struct VacuumPermeabilityConst	{ static constexpr T  Value = Pi::Value * T(4 * 1.0000000008220e-7); };	// kg * m / (s * A)^2

			struct GasConstant				{ static constexpr T  Value = T(8.314459848); };	// R = J / (K * mol)
			struct EarthMass				{ static constexpr T  Value = T(5.9722e+24); };		// kg
			struct SolarMass				{ static constexpr T  Value = T(1.98847e+30); };	// kg
			struct SolarLuminosity			{ static constexpr T  Value = T(3.828e+26); };		// W
			struct SolarRadius				{ static constexpr T  Value = T(6.957+8); };		// m
		};


		using Dim						= DefaultPhysicalDimensions;

		using NonDimensional			= PhysicalQuantity< T, Dim::NonDimensional >;
		using Seconds					= PhysicalQuantity< T, Dim::Second >;					// s
		using Kilograms					= PhysicalQuantity< T, Dim::Kilogram >;					// kg
		using Meters					= PhysicalQuantity< T, Dim::Meter >;					// m
		using Ampers					= PhysicalQuantity< T, Dim::Ampere >;					// A
		using Kelvins					= PhysicalQuantity< T, Dim::Kelvin >;					// K
		using Moles						= PhysicalQuantity< T, Dim::Mole >;						// mol
		using Candelas					= PhysicalQuantity< T, Dim::Candela >;					// cd
		using Currencies				= PhysicalQuantity< T, Dim::Currency >;					// $
		
		using SquareMeters				= PhysicalQuantity< T, Dim::SquareMeter >;				// m^2
		using CubicMeters				= PhysicalQuantity< T, Dim::CubicMeter >;				// m^3
		using MetersPerSeconds			= PhysicalQuantity< T, Dim::MeterPerSecond >;			// m/s
		using MetersPerSquareSeconds	= PhysicalQuantity< T, Dim::MeterPerSquareSecond >;		// m/s^2
		using KilogramsPerSeconds		= PhysicalQuantity< T, Dim::KilogramPerSecond >;		// kg / s
		using KilogramMetersPerSeconds	= PhysicalQuantity< T, Dim::KilogramMeterPerSecond >;	// kg * m/s
		using KilogramsPerCubicMeters	= PhysicalQuantity< T, Dim::KilogramPerCubicMeter >;	// p = kg / m^3
		using Newtons					= PhysicalQuantity< T, Dim::Newton >;					// N
		using NewtonMeters				= PhysicalQuantity< T, Dim::Joule >;					// N*m
		using Joules					= PhysicalQuantity< T, Dim::Joule >;					// J
		using Pascals					= PhysicalQuantity< T, Dim::Pascal >;					// Pa
		using Hertz						= PhysicalQuantity< T, Dim::Hertz >;					// Hz
		using Watts						= PhysicalQuantity< T, Dim::Watt >;						// W
		using Coulombs					= PhysicalQuantity< T, Dim::Coulomb >;					// C
		using Volts						= PhysicalQuantity< T, Dim::Volt >;						// V
		using Ohms						= PhysicalQuantity< T, Dim::Ohm >;						// Ohm
		using Farads					= PhysicalQuantity< T, Dim::Farad >;					// F
		using Webers					= PhysicalQuantity< T, Dim::Weber >;					// Wb
		using Henries					= PhysicalQuantity< T, Dim::Henry >;					// H
		using Teslas					= PhysicalQuantity< T, Dim::Tesla >;					// T
		using Siemens					= PhysicalQuantity< T, Dim::Siemens >;					// S
		using Lumens					= PhysicalQuantity< T, Dim::Lumen >;					// lm
		using Lux						= PhysicalQuantity< T, Dim::Lux >;						// lx
		using AmpersPerMeters			= PhysicalQuantity< T, Dim::AmperPerMeter >;			// A / m
		using KilogramsPerMoles			= PhysicalQuantity< T, Dim::KilogramPerMole >;			// kg / mol

		using Nanometers				= PhysicalQuantity< T, Dim::Meter, typename Scale::Nano >;				// nm
		using Micrometers				= PhysicalQuantity< T, Dim::Meter, typename Scale::Micro >;				// um
		using Millimeters				= PhysicalQuantity< T, Dim::Meter, typename Scale::Milli >;				// mm
		using Centimeters				= PhysicalQuantity< T, Dim::Meter, typename Scale::Centi >;				// cm
		using Kilometers				= PhysicalQuantity< T, Dim::Meter, typename Scale::Kilo >;				// km
		using Megameters				= PhysicalQuantity< T, Dim::Meter, typename Scale::Mega >;				// Mm
		using Gigameters				= PhysicalQuantity< T, Dim::Meter, typename Scale::Giga >;				// Gm
		using Angstroms					= PhysicalQuantity< T, Dim::Meter, typename Scale::Angstrom >;			// A
		using LightSeconds				= PhysicalQuantity< T, Dim::Meter, typename Scale::SpeedOfLight >;		// c*s
		using LightMinutes				= PhysicalQuantity< T, Dim::Meter, typename Scale::LightMinute >;		// c*min
		using LightHours				= PhysicalQuantity< T, Dim::Meter, typename Scale::LightHour >;			// c*h
		using LightDays					= PhysicalQuantity< T, Dim::Meter, typename Scale::LightDay >;			// c*d
		using LightYears				= PhysicalQuantity< T, Dim::Meter, typename Scale::LightYear >;			// c*y
		using Parsecs					= PhysicalQuantity< T, Dim::Meter, typename Scale::Parsec >;			// pc
		using AstronomicalUnits			= PhysicalQuantity< T, Dim::Meter, typename Scale::AstronomicalUnit >;	// au
		using SolarRadius				= PhysicalQuantity< T, Dim::Meter, typename Scale::SolarRadius >;		// m

		using Liters					= PhysicalQuantity< T, Dim::CubicMeter, typename Scale::Litre >;		// L
		
		using LightSpeeds				= PhysicalQuantity< T, Dim::MeterPerSecond, typename Scale::SpeedOfLight >;			// c
		using GravitySpeeds				= PhysicalQuantity< T, Dim::MeterPerSecond, typename Scale::SpeedOfGravity >;		// ?
		using AstronomicalUnitsPerSeconds= PhysicalQuantity< T, Dim::MeterPerSecond, typename Scale::AstronomicalUnit >;	// AU / s

		using Nanograms					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Pico >;		// ng
		using Micrograms				= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Nano >;		// ug
		using Milligrams				= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Micro >;		// mg
		using Grams						= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Milli >;		// g
		using Tones						= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Kilo >;		// t
		using Kilotones					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Mega >;		// kt
		using Megatones					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Giga >;		// Mt
		using EarthMass					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::EarthMass >;	// EM
		using SolarMass					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::SolarMass >;	// SM
		
		using Nanoseconds				= PhysicalQuantity< T, Dim::Second, typename Scale::Nano >;			// ns
		using Microseconds				= PhysicalQuantity< T, Dim::Second, typename Scale::Micro >;		// us
		using Milliseconds				= PhysicalQuantity< T, Dim::Second, typename Scale::Milli >;		// ms
		using Minutes					= PhysicalQuantity< T, Dim::Second, typename Scale::Minute >;		// min
		using Hours						= PhysicalQuantity< T, Dim::Second, typename Scale::Hour >;			// h
		using Days						= PhysicalQuantity< T, Dim::Second, typename Scale::Day >;			// d
		using Years						= PhysicalQuantity< T, Dim::Second, typename Scale::Year >;			// y
		using ThousandYears				= PhysicalQuantity< T, Dim::Second, ValueScaleTempl::Mul< typename Scale::Year, ValueScaleTempl::Integer<T, 1000>> >;
		using MillionYears				= PhysicalQuantity< T, Dim::Second, ValueScaleTempl::Mul< typename Scale::Year, ValueScaleTempl::Integer<T, 1000000>> >;
		
		using ElectronVolts				= PhysicalQuantity< T, Dim::Joule, typename Scale::ElectronVolt >;	// eV
		using Nanojoules				= PhysicalQuantity< T, Dim::Joule, typename Scale::Nano >;			// nJ
		using Microjoules				= PhysicalQuantity< T, Dim::Joule, typename Scale::Micro >;			// uJ
		using Millijoules				= PhysicalQuantity< T, Dim::Joule, typename Scale::Milli >;			// mJ
		using Kilojoules				= PhysicalQuantity< T, Dim::Joule, typename Scale::Kilo >;			// KJ
		using Megajoules				= PhysicalQuantity< T, Dim::Joule, typename Scale::Mega >;			// MJ
		using Gigajoules				= PhysicalQuantity< T, Dim::Joule, typename Scale::Giga >;			// GJ
		
		using Nanowatts					= PhysicalQuantity< T, Dim::Watt, typename Scale::Nano >;			// nW
		using Microwatts				= PhysicalQuantity< T, Dim::Watt, typename Scale::Micro >;			// uW
		using Milliwatts				= PhysicalQuantity< T, Dim::Watt, typename Scale::Milli >;			// mW
		using Kilowatts					= PhysicalQuantity< T, Dim::Watt, typename Scale::Kilo >;			// KW
		using Megawatts					= PhysicalQuantity< T, Dim::Watt, typename Scale::Mega >;			// MW
		using Gigawatts					= PhysicalQuantity< T, Dim::Watt, typename Scale::Giga >;			// GW

		using Bars						= PhysicalQuantity< T, Dim::Pascal, typename Scale::Bar >;			// bar
		using Atmospheres				= PhysicalQuantity< T, Dim::Pascal, typename Scale::Atmosphere >;	// atm

		using _GConstDim				= Dim::Meter::template Pow<3>::template Div< Dim::Kilogram::template Mul< Dim::Second::template Pow<2> >>;

		using GAcceleration				= PhysicalQuantity< T, Dim::MeterPerSquareSecond, typename Scale::GravitationalAcceleration >;	// g
		using GConstant					= PhysicalQuantity< T, _GConstDim, typename Scale::GravitationalConstant >;	// G
		using SolarLuminosity			= PhysicalQuantity< T, Dim::Watt, typename Scale::SolarLuminosity >;		// SL
	};


} // AE::Math


namespace AE::Base
{
	template <typename V, typename D, typename S>
	struct TMemCopyAvailable< PhysicalQuantity< V, D, S >> { static constexpr bool  value = IsMemCopyAvailable<V>; };
	
	template <typename V, typename D, typename S>
	struct TZeroMemAvailable< PhysicalQuantity< V, D, S >> { static constexpr bool  value = IsZeroMemAvailable<V>; };
	
	template <typename V, typename D, typename S>
	struct TTrivialySerializable< PhysicalQuantity< V, D, S >> { static constexpr bool  value = IsTrivialySerializable<V>; };

} // AE::Base
