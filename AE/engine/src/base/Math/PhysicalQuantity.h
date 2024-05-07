// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Radian.h"
#include "base/Math/PhysicalDimension.h"
#include "base/Math/PhysicalQuantity_Scale.h"

namespace AE::Math
{

	template <typename ValueType,
			  typename Dimension,
			  typename ValueScale = PhysicalQuantity_Scale::Integer<ValueType, 1>
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
		StaticAssert( IsScalar<ValueType> );
		//StaticAssert( IsFloatPoint<ValueType> );

	// types
	public:
		using Value_t		= ValueType;
		using Scale_t		= ValueScale;
		using Dimension_t	= Dimension;
		using Self			= PhysicalQuantity< Value_t, Dimension, Scale_t >;
		using Inversed_t	= PhysicalQuantity< Value_t,
												DefaultPhysicalDimensions::NonDimensional::template Div< Dimension_t >,
												typename PhysicalQuantity_Scale::template Inverse< Scale_t > >;

	private:
		static constexpr bool	_IsTimeUnit = Dimension_t::template Equal< DefaultPhysicalDimensions::Second >;
		static constexpr bool	_IsInfoUnit	= Dimension_t::template Equal< DefaultPhysicalDimensions::Bit >;


	// variables
	private:
		Value_t		_value;


	// methods
	public:
		constexpr PhysicalQuantity ()							__NE___ : _value{0} {}
		constexpr explicit PhysicalQuantity (Value_t value)		__NE___ : _value{value} {}

		template <typename S>
		constexpr PhysicalQuantity (const PhysicalQuantity<Value_t, Dimension_t, S> &other) __NE___ :
			_value{ other.template ToScale<Scale_t>().GetNonScaled() }
		{}

		template <typename T, typename S>
		explicit constexpr PhysicalQuantity (const PhysicalQuantity<T, Dimension_t, S> &other) __NE___ :
			_value{Value_t( other.template ToScale<Scale_t>().GetNonScaled() )}
		{}

		constexpr PhysicalQuantity (const Self &)					__NE___	= default;
		constexpr PhysicalQuantity (Self &&)						__NE___	= default;

			constexpr Self&		operator = (const Self &)			__NE___	= default;
			constexpr Self&		operator = (Self &&)				__NE___	= default;

		ND_ constexpr Self		operator - ()						C_NE___	{ return Self{ -_value }; }

		ND_ constexpr bool		operator == (const Self &rhs)		C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool		operator != (const Self &rhs)		C_NE___	{ return _value != rhs._value; }
		ND_ constexpr bool		operator >  (const Self &rhs)		C_NE___	{ return _value >  rhs._value; }
		ND_ constexpr bool		operator >= (const Self &rhs)		C_NE___	{ return _value >= rhs._value; }
		ND_ constexpr bool		operator <  (const Self &rhs)		C_NE___	{ return _value <  rhs._value; }
		ND_ constexpr bool		operator <= (const Self &rhs)		C_NE___	{ return _value <= rhs._value; }

			constexpr Self&		operator += (const Self &rhs)		__NE___	{ _value += rhs._value;  return *this; }
			constexpr Self&		operator -= (const Self &rhs)		__NE___	{ _value -= rhs._value;  return *this; }

		ND_ constexpr Self		operator +  (const Self &rhs)		C_NE___	{ return Self( _value + rhs._value ); }
		ND_ constexpr Self		operator -  (const Self &rhs)		C_NE___	{ return Self( _value - rhs._value ); }

			constexpr Self&		operator *= (Value_t rhs)			__NE___	{ _value *= rhs;  return *this; }
			constexpr Self&		operator /= (Value_t rhs)			__NE___	{ _value /= rhs;  return *this; }

		ND_ constexpr Self		operator *  (Value_t rhs)			C_NE___	{ return Self( _value * rhs ); }
		ND_ constexpr Self		operator /  (Value_t rhs)			C_NE___	{ return Self( _value / rhs ); }

		ND_ constexpr Value_t	GetNonScaled ()						C_NE___	{ return _value; }
		ND_ constexpr Value_t&	GetNonScaledRef ()					__NE___	{ return _value; }
		ND_ constexpr Value_t	GetScaled ()						C_NE___	{ return _value * Scale_t::Value; }


		template <typename S>
		ND_ constexpr auto  operator +  (const PhysicalQuantity<Value_t, Dimension_t, S> &rhs) C_NE___
		{
			using Scale = PhysicalQuantity_Scale::template Add< Scale_t, S >;
			return PhysicalQuantity< Value_t, Dimension_t, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}

		template <typename S>
		ND_ constexpr auto  operator -  (const PhysicalQuantity<Value_t, Dimension_t, S> &rhs) C_NE___
		{
			using Scale = PhysicalQuantity_Scale::template Sub< Scale_t, S >;
			return PhysicalQuantity< Value_t, Dimension_t, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}

		template <typename D, typename S>
		ND_ constexpr auto  operator *  (const PhysicalQuantity<Value_t,D,S> &rhs) C_NE___
		{
			using Scale = PhysicalQuantity_Scale::template Mul< Scale_t, S >;
			return PhysicalQuantity< Value_t, typename Dimension_t::template Mul<D>, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}

		template <typename D, typename S>
		ND_ constexpr auto  operator /  (const PhysicalQuantity<Value_t,D,S> &rhs) C_NE___
		{
			using Scale = PhysicalQuantity_Scale::template Div< Scale_t, S >;
			return PhysicalQuantity< Value_t, typename Dimension_t::template Div<D>, Scale >{ Scale::Get( _value, rhs.GetNonScaled() )};
		}


		ND_ friend constexpr Self  operator * (Value_t lhs, const Self &rhs) __NE___
		{
			return Self( lhs * rhs.GetNonScaled() );
		}

		template <typename DstScale>
		ND_ constexpr auto  ToScale () C_NE___
		{
			const auto	scale = Scale_t::Value / Value_t(DstScale::Value);
			return PhysicalQuantity< Value_t, Dimension_t, DstScale >{ _value * scale };
		}

		ND_ friend constexpr Inversed_t  operator / (Value_t lhs, const Self &rhs) __NE___
		{
			return Inversed_t{ lhs / rhs.GetNonScaled() };
		}

		template <int IntPower>
		ND_ constexpr auto  Pow () C_NE___
		{
			using Dim	= typename Dimension_t::template Pow< IntPower >;
			using Scale	= PhysicalQuantity_Scale::template Pow< Scale_t, IntPower >;
			return PhysicalQuantity< Value_t, Dim, Scale >{ Math::Pow( _value, Value_t{IntPower} )};
		}


	// only for time units //
		template <typename Rep, typename Period,
				  ENABLEIF( IsScalar<Rep> and _IsTimeUnit )
				 >
		explicit constexpr PhysicalQuantity (const std::chrono::duration< Rep, Period > &other) __NE___ :
			_value{ TimeCast< std::chrono::duration< Value_t >>( other ).count() / Scale_t::Value }
		{}

		template <typename Rep, typename Period,
				  ENABLEIF( IsScalar<Rep> and _IsTimeUnit )
				 >
		ND_ explicit constexpr operator std::chrono::duration< Rep, Period > () __NE___ {
			return TimeCast< std::chrono::duration< Rep, Period >>( std::chrono::duration< Value_t >{ GetScaled() });
		}


	// only for information units //
		template <typename T,
				  ENABLEIF( IsScalar<T> and _IsInfoUnit )
				 >
		explicit constexpr PhysicalQuantity (const TByte<T> &other) __NE___ :
			_value{ T{other} / Scale_t::Value }
		{}

		template <typename T,
				  ENABLEIF( IsScalar<T> and _IsInfoUnit )
				 >
		ND_ explicit constexpr operator TByte<T> () __NE___ {
			return TByte<T>{ T(GetScaled() / 8) };
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
		StaticAssert( IsScalar<ValueType> );
		//StaticAssert( IsFloatPoint<ValueType> );

	// types
	public:
		using Value_t		= ValueType;
		using Scale_t		= ValueScale;
		using Dimension_t	= DefaultPhysicalDimensions::NonDimensional;
		using Self			= PhysicalQuantity< Value_t, Dimension_t, Scale_t >;
		using Inversed_t	= PhysicalQuantity< Value_t,
												Dimension_t,
												typename PhysicalQuantity_Scale::template Inverse< Scale_t > >;

	// variables
	private:
		Value_t		_value;


	// methods
	public:
		constexpr PhysicalQuantity ()								__NE___ : _value{0} {}
		constexpr explicit PhysicalQuantity (Value_t value)			__NE___ : _value{value} {}

		template <typename S>
		constexpr PhysicalQuantity (const PhysicalQuantity<Value_t, Dimension_t, S> &other) __NE___ :
			_value{ other.template ToScale<Scale_t>().GetNonScaled() }
		{}

		template <typename T, typename S>
		explicit constexpr PhysicalQuantity (const PhysicalQuantity<T, Dimension_t, S> &other) __NE___ :
			_value{T( other.template ToScale<Scale_t>().GetNonScaled() )}
		{}

		constexpr PhysicalQuantity (const Self &)					__NE___	= default;
		constexpr PhysicalQuantity (Self &&)						__NE___	= default;

			constexpr Self&		operator = (const Self &)			__NE___	= default;
			constexpr Self&		operator = (Self &&)				__NE___	= default;

		ND_ constexpr operator Value_t ()							C_NE___	{ return GetScaled(); }

		ND_ constexpr bool		operator == (const Self &rhs)		C_NE___	{ return _value == rhs.GetNonScaled(); }
		ND_ constexpr bool		operator != (const Self &rhs)		C_NE___	{ return _value != rhs.GetNonScaled(); }
		ND_ constexpr bool		operator >  (const Self &rhs)		C_NE___	{ return _value >  rhs.GetNonScaled(); }
		ND_ constexpr bool		operator >= (const Self &rhs)		C_NE___	{ return _value >= rhs.GetNonScaled(); }
		ND_ constexpr bool		operator <  (const Self &rhs)		C_NE___	{ return _value <  rhs.GetNonScaled(); }
		ND_ constexpr bool		operator <= (const Self &rhs)		C_NE___	{ return _value <= rhs.GetNonScaled(); }

			constexpr Self&		operator += (const Self &rhs)		__NE___	{ _value += rhs.GetNonScaled();  return *this; }
			constexpr Self&		operator -= (const Self &rhs)		__NE___	{ _value -= rhs.GetNonScaled();  return *this; }

		ND_ constexpr Self		operator +  (const Self &rhs)		C_NE___	{ return Self{ _value + rhs.GetNonScaled() }; }
		ND_ constexpr Self		operator -  (const Self &rhs)		C_NE___	{ return Self{ _value - rhs.GetNonScaled() }; }

			constexpr Self&		operator *= (Value_t rhs)			__NE___	{ _value *= rhs;  return *this; }
			constexpr Self&		operator /= (Value_t rhs)			__NE___	{ _value /= rhs;  return *this; }

		ND_ constexpr Self		operator *  (Value_t rhs)			C_NE___	{ return Self{ _value * rhs }; }
		ND_ constexpr Self		operator /  (Value_t rhs)			C_NE___	{ return Self{ _value / rhs }; }

		ND_ constexpr Value_t	GetNonScaled ()						C_NE___	{ return _value; }
		ND_ constexpr Value_t	GetScaled ()						C_NE___	{ return _value * Scale_t::Value; }


		ND_ friend constexpr Self	operator * (Value_t left, const Self &right) __NE___
		{
			return Self{ left * right.GetNonScaled() };
		}

		ND_ friend constexpr Inversed_t  operator / (Value_t left, const Self &right) __NE___
		{
			return Inversed_t{ left / right.GetNonScaled() };
		}

		template <typename DstScale>
		ND_ constexpr auto  ToScale () C_NE___
		{
			const auto	scale = Scale_t::Value / Value_t(DstScale::Value);
			return PhysicalQuantity< Value_t, Dimension_t, DstScale >{ _value * scale };
		}

		template <int IntPower>
		ND_ constexpr auto  Pow () C_NE___
		{
			using Scale	= PhysicalQuantity_Scale::template Pow< Scale_t, IntPower >;
			return PhysicalQuantity< Value_t, Dimension_t, Scale >{ Math::Pow( _value, Value_t{IntPower} )};
		}


	// radians //
		constexpr explicit PhysicalQuantity (TRadian<Value_t> rad)	__NE___ : _value{Value_t{rad}} {}

		ND_ constexpr explicit operator TRadian<Value_t> ()			C_NE___	{ return TRadian<Value_t>{GetScaled()}; }
	};


/*
=================================================
	IsPhysicalQuantity
=================================================
*/
	namespace _hidden_
	{
		template <typename T>
		struct _IsPhysicalQuantity {
			static constexpr bool	value = false;
		};

		template <typename ValueType, typename Dimension, typename ValueScale>
		struct _IsPhysicalQuantity< PhysicalQuantity< ValueType, Dimension, ValueScale >> {
			static constexpr bool	value = true;
		};
	}
	template <typename T>
	static constexpr bool	IsPhysicalQuantity = Math::_hidden_::_IsPhysicalQuantity<T>::value;

/*
=================================================
	Lerp
=================================================
*/
	template <typename T, typename Dimension, typename LhsScale, typename RhsScale>
	ND_ constexpr auto  Lerp (const PhysicalQuantity<T, Dimension, LhsScale> &a,
							  const PhysicalQuantity<T, Dimension, RhsScale> &b, const T factor) __NE___
	{
		return a * (T{1} - factor) + b * factor;
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename T, typename Dimension, typename Scale>
	ND_ constexpr bool  Equal (const PhysicalQuantity<T, Dimension, Scale> &a,
								const PhysicalQuantity<T, Dimension, Scale> &b,
								const T err = Epsilon<T>()) __NE___
	{
		return Math::Equal( a.GetNonScaled(), b.GetNonScaled(), err );
	}

	template <typename T, typename Dimension, typename LhsScale, typename RhsScale, typename ErrScale>
	ND_ constexpr bool  Equal (const PhysicalQuantity<T, Dimension, LhsScale> &a,
								const PhysicalQuantity<T, Dimension, RhsScale> &b,
								const PhysicalQuantity<T, Dimension, ErrScale> &err) __NE___
	{
		return Math::Equal( a.GetScaled(), b.GetScaled(), err.GetScaled() );	// TODO: average scale
	}

	template <typename T, typename Dimension, typename LhsScale, typename RhsScale>
	ND_ constexpr bool  Equal (const PhysicalQuantity<T, Dimension, LhsScale> &a,
								const PhysicalQuantity<T, Dimension, RhsScale> &b,
								const Percent err) __NE___
	{
		return Math::Equal( a.GetScaled(), b.GetScaled(), err );
	}

/*
=================================================
	BitEqual
=================================================
*/
	template <typename T, typename Dimension, typename Scale>
	ND_ constexpr EnableIf<IsFloatPoint<T>, bool>  BitEqual (const PhysicalQuantity<T, Dimension, Scale> &a,
															 const PhysicalQuantity<T, Dimension, Scale> &b,
															 const EnabledBitCount bitCount) __NE___
	{
		return Math::BitEqual( a.GetNonScaled(), b.GetNonScaled(), bitCount );
	}

	template <typename T, typename Dimension, typename Scale>
	ND_ constexpr EnableIf<IsFloatPoint<T>, bool>  BitEqual (const PhysicalQuantity<T, Dimension, Scale> &a,
															 const PhysicalQuantity<T, Dimension, Scale> &b) __NE___
	{
		return Math::BitEqual( a.GetNonScaled(), b.GetNonScaled() );
	}

/*
=================================================
	IsZero
=================================================
*/
	template <typename T, typename Dimension, typename Scale>
	ND_ constexpr bool  IsZero (PhysicalQuantity<T, Dimension, Scale> x) __NE___
	{
		return Math::IsZero( x.GetNonScaled() );
	}
//-----------------------------------------------------------------------------



	//
	// Default Physical Quantity
	//

	template <typename T>
	struct DefaultPhysicalQuantity
	{
		struct Scale
		{
			struct Minute					{ static constexpr T  Value = T(60); };						// m
			struct Hour						{ static constexpr T  Value = T(60 * 60); };				// h
			struct Day						{ static constexpr T  Value = T(24 * 60 * 60); };			// d
			struct Year						{ static constexpr T  Value = T(36'525 * 24 * 6 * 6); };	// y

			struct Deca						{ static constexpr T  Value = T(10); };
			struct Hecto					{ static constexpr T  Value = T(1.0e+2); };
			struct Kilo						{ static constexpr T  Value = T(1.0e+3); };
			struct Mega						{ static constexpr T  Value = T(1.0e+6); };
			struct Giga						{ static constexpr T  Value = T(1.0e+9); };
			struct Tera						{ static constexpr T  Value = T(1.0e+12); };
			struct Peta						{ static constexpr T  Value = T(1.0e+15); };
			struct Exa						{ static constexpr T  Value = T(1.0e+18); };
			struct Zetta					{ static constexpr T  Value = T(1.0e+21); };
			struct Yotta					{ static constexpr T  Value = T(1.0e+24); };
			struct Ronna					{ static constexpr T  Value = T(1.0e+27); };
			struct Quetta					{ static constexpr T  Value = T(1.0e+30); };

			struct Kibi						{ static constexpr T  Value = T(1ull<<10); };				// kilo  / kibi
			struct Mebi						{ static constexpr T  Value = T(1ull<<20); };				// mega  / mebi
			struct Gibi						{ static constexpr T  Value = T(1ull<<30); };				// giga  / gibi
			struct Tebi						{ static constexpr T  Value = T(1ull<<40); };				// tera  / tebi
			struct Pebi						{ static constexpr T  Value = T(1ull<<50); };				// peta  / pebi
			struct Exbi						{ static constexpr T  Value = T(1ull<<60); };				// exa   / exbi
			struct Zebi						{ static constexpr T  Value = T(1ull<<60) * T(1ull<<10); };	// zetta / zebi
			struct Yobi						{ static constexpr T  Value = T(1ull<<60) * T(1ull<<20); };	// yotta / yobi

			struct Byte						{ static constexpr T  Value = T(8.0); };
			struct KibiByte					{ static constexpr T  Value = T(8ull<<10); };				// kilo  / kibi
			struct MebiByte					{ static constexpr T  Value = T(8ull<<20); };				// mega  / mebi
			struct GibiByte					{ static constexpr T  Value = T(8ull<<30); };				// giga  / gibi
			struct TebiByte					{ static constexpr T  Value = T(8ull<<40); };				// tera  / tebi
			struct PebiByte					{ static constexpr T  Value = T(8ull<<50); };				// peta  / pebi
			struct ExbiByte					{ static constexpr T  Value = T(8ull<<50) * T(1ull<<10); };	// exa   / exbi
			struct ZebiByte					{ static constexpr T  Value = T(8ull<<50) * T(1ull<<20); };	// zetta / zebi
			struct YobiByte					{ static constexpr T  Value = T(8ull<<50) * T(1ull<<30); };	// yotta / yobi

			struct Deci						{ static constexpr T  Value = T(1.0e-1); };
			struct Centi					{ static constexpr T  Value = T(1.0e-2); };
			struct Milli					{ static constexpr T  Value = T(1.0e-3); };
			struct Micro					{ static constexpr T  Value = T(1.0e-6); };
			struct Nano						{ static constexpr T  Value = T(1.0e-9); };
			struct Pico						{ static constexpr T  Value = T(1.0e-12); };
			struct Femto					{ static constexpr T  Value = T(1.0e-15); };
			struct Atto						{ static constexpr T  Value = T(1.0e-18); };
			struct Zepto					{ static constexpr T  Value = T(1.0e-21); };
			struct Yocto					{ static constexpr T  Value = T(1.0e-24); };

			struct Liter					{ static constexpr T  Value = T(1.0e-3); };					// m^3	L

			struct Pound					{ static constexpr T  Value = T(0.453592); };				// lbs to kg

			struct ElectronVolt				{ static constexpr T  Value = T(1.602176620898e-19); };		// J	eV
			struct Dalton					{ static constexpr T  Value = T(1.66053904020e-27); };		// kg	Da	amu
			struct AstronomicalUnit			{ static constexpr T  Value = T(149597870700.0); };			// m	au

			struct Angstrom					{ static constexpr T  Value = T(1.0e-10); };				// m	A
			struct Bar						{ static constexpr T  Value = T(1.0e+5); };					// Pa	bar
			struct Atmosphere				{ static constexpr T  Value = T(101325); };					// Pa

			struct SpeedOfLight				{ static constexpr T  Value = T(299'792'458.0); };			// m/s	c					- SI
			struct SpeedOfGravity			{ static constexpr T  Value = T(299'792'458.0); };			// m/s	?
			struct Parsec					{ static constexpr T  Value = T(3.0856776e+16); };			// m	pc
			using  LightMinute				= PhysicalQuantity_Scale::template Mul< SpeedOfLight, Minute >;
			using  LightHour				= PhysicalQuantity_Scale::template Mul< SpeedOfLight, Hour >;
			using  LightDay					= PhysicalQuantity_Scale::template Mul< SpeedOfLight, Day >;
			using  LightYear				= PhysicalQuantity_Scale::template Mul< SpeedOfLight, Year >;

			struct GravitationalConstant	{ static constexpr T  Value = T(6.6740831e-11); };			// m^3 / (s^2 * kg)
			struct GravitationalAcceleration{ static constexpr T  Value = T(9.80665); };				// m / s^2

			struct AvogadroConstant			{ static constexpr T  Value = T(6.02214076e+23); };			// 1 / mol					- SI
			struct Pi						{ static constexpr T  Value = T(3.14159265358979323846); };	// Pi
			struct VacuumPermeabilityConst	{ static constexpr T  Value = T(1.25663706212191919e-6); };	// N / A^2

			struct GasConstant				{ static constexpr T  Value = T(8.314459848); };			// R = J / (K * mol)
			struct EarthMass				{ static constexpr T  Value = T(5.9722e+24); };				// kg
			struct SolarMass				{ static constexpr T  Value = T(1.98847e+30); };			// kg
			struct SolarLuminosity			{ static constexpr T  Value = T(3.828e+26); };				// W
			struct SolarRadius				{ static constexpr T  Value = T(6.957e+8); };				// m
			struct EarthRadius				{ static constexpr T  Value = T(6.3781+6); };				// m

			struct PlanckConst				{ static constexpr T  Value = T(6.62607015e-34); };			// m^2 * kg / s = J*s		- SI
			struct PlanckConstDiv2Pi		{ static constexpr T  Value = T(1.05457180e-34); };			// m^2 * kg / s = J*s

			struct HyperfineTransitionFrequencyOfCesium
											{ static constexpr T  Value = T(9'192'631'770.0); };		// Hz						- SI
			struct ElementaryCharge			{ static constexpr T  Value = T{1.602176634e-19}; };		// C						- SI
			struct BoltzmannConst			{ static constexpr T  Value = T{1.380649e-23}; };			// J / K					- SI
			struct LuminousEfficacyOf540THzRadiation
											{ static constexpr T  Value = T{683}; };					// lm / W					- SI

			struct Horsepower				{ static constexpr T  Value = T(735.5); };
			struct DegToRad					{ static constexpr T  Value = TRadian<T>::RadToDeg(); };
		};


		using Dim						= DefaultPhysicalDimensions;

		using NonDimensional			= PhysicalQuantity< T, Dim::NonDimensional >;
		using Second					= PhysicalQuantity< T, Dim::Second >;					// s
		using Kilogram					= PhysicalQuantity< T, Dim::Kilogram >;					// kg
		using Meter						= PhysicalQuantity< T, Dim::Meter >;					// m
		using Ampere					= PhysicalQuantity< T, Dim::Ampere >;					// A
		using Kelvin					= PhysicalQuantity< T, Dim::Kelvin >;					// K
		using Mole						= PhysicalQuantity< T, Dim::Mole >;						// mol
		using Candela					= PhysicalQuantity< T, Dim::Candela >;					// cd
		using Currency					= PhysicalQuantity< T, Dim::Currency >;					// $
		using Bit						= PhysicalQuantity< T, Dim::Bit >;						// bit

		using Diopter					= PhysicalQuantity< T, Dim::Diopter >;					// 1 / m
		using Radian					= PhysicalQuantity< T, Dim::Radian >;					// rad
		using Steradian					= PhysicalQuantity< T, Dim::Steradian >;				// sr
		using Frequency					= PhysicalQuantity< T, Dim::Hertz >;					// 1 / s
		using SquareMeter				= PhysicalQuantity< T, Dim::SquareMeter >;				// m^2
		using CubicMeter				= PhysicalQuantity< T, Dim::CubicMeter >;				// m^3
		using MeterPerSecond			= PhysicalQuantity< T, Dim::MeterPerSecond >;			// m / s
		using MeterPerSquareSecond		= PhysicalQuantity< T, Dim::MeterPerSquareSecond >;		// m / s^2
		using KilogramPerSecond			= PhysicalQuantity< T, Dim::KilogramPerSecond >;		// kg / s
		using KilogramMeterPerSecond	= PhysicalQuantity< T, Dim::KilogramMeterPerSecond >;	// kg * m / s
		using KilogramPerCubicMeter		= PhysicalQuantity< T, Dim::KilogramPerCubicMeter >;	// p = kg / m^3
		using Newton					= PhysicalQuantity< T, Dim::Newton >;					// N
		using NewtonMeter				= PhysicalQuantity< T, Dim::Joule >;					// N * m
		using Joule						= PhysicalQuantity< T, Dim::Joule >;					// J
		using Pascal					= PhysicalQuantity< T, Dim::Pascal >;					// Pa
		using Hertz						= PhysicalQuantity< T, Dim::Hertz >;					// Hz
		using Watt						= PhysicalQuantity< T, Dim::Watt >;						// W
		using Coulomb					= PhysicalQuantity< T, Dim::Coulomb >;					// C
		using Volt						= PhysicalQuantity< T, Dim::Volt >;						// V
		using Ohm						= PhysicalQuantity< T, Dim::Ohm >;						// Ohm
		using Farad						= PhysicalQuantity< T, Dim::Farad >;					// F
		using Weber						= PhysicalQuantity< T, Dim::Weber >;					// Wb
		using Henry						= PhysicalQuantity< T, Dim::Henry >;					// H
		using Tesla						= PhysicalQuantity< T, Dim::Tesla >;					// T
		using Siemens					= PhysicalQuantity< T, Dim::Siemens >;					// S
		using Lumen						= PhysicalQuantity< T, Dim::Lumen >;					// lm
		using Lux						= PhysicalQuantity< T, Dim::Lux >;						// lx
		using AmperPerMeter				= PhysicalQuantity< T, Dim::AmperPerMeter >;			// A / m
		using KilogramPerMole			= PhysicalQuantity< T, Dim::KilogramPerMole >;			// kg / mol
		using BitPerSecond				= PhysicalQuantity< T, Dim::BitPerSecond >;				// bit / s


		using Degree					= PhysicalQuantity< T, Dim::Radian, typename Scale::DegToRad >;			// deg

		using Nanometer					= PhysicalQuantity< T, Dim::Meter, typename Scale::Nano >;				// nm
		using Micrometer				= PhysicalQuantity< T, Dim::Meter, typename Scale::Micro >;				// um
		using Millimeter				= PhysicalQuantity< T, Dim::Meter, typename Scale::Milli >;				// mm
		using Centimeter				= PhysicalQuantity< T, Dim::Meter, typename Scale::Centi >;				// cm
		using Kilometer					= PhysicalQuantity< T, Dim::Meter, typename Scale::Kilo >;				// km
		using Megameter					= PhysicalQuantity< T, Dim::Meter, typename Scale::Mega >;				// Mm
		using Gigameter					= PhysicalQuantity< T, Dim::Meter, typename Scale::Giga >;				// Gm
		using Terameter					= PhysicalQuantity< T, Dim::Meter, typename Scale::Tera >;				// Tm
		using Angstrom					= PhysicalQuantity< T, Dim::Meter, typename Scale::Angstrom >;			// A
		using LightSecond				= PhysicalQuantity< T, Dim::Meter, typename Scale::SpeedOfLight >;		// c*s
		using LightMinute				= PhysicalQuantity< T, Dim::Meter, typename Scale::LightMinute >;		// c*min
		using LightHour					= PhysicalQuantity< T, Dim::Meter, typename Scale::LightHour >;			// c*h
		using LightDay					= PhysicalQuantity< T, Dim::Meter, typename Scale::LightDay >;			// c*d
		using LightYear					= PhysicalQuantity< T, Dim::Meter, typename Scale::LightYear >;			// c*y
		using Parsec					= PhysicalQuantity< T, Dim::Meter, typename Scale::Parsec >;			// pc
		using AstronomicalUnit			= PhysicalQuantity< T, Dim::Meter, typename Scale::AstronomicalUnit >;	// au
		using SolarRadius				= PhysicalQuantity< T, Dim::Meter, typename Scale::SolarRadius >;		// m
		using EarthRadius				= PhysicalQuantity< T, Dim::Meter, typename Scale::EarthRadius >;		// m
		using Wavelength				= Nanometer;															// λ

		using Liter						= PhysicalQuantity< T, Dim::CubicMeter, typename Scale::Liter >;		// L
		using Pound						= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Pound >;			// kg

		using LightVelocity				= PhysicalQuantity< T, Dim::MeterPerSecond, typename Scale::SpeedOfLight >;		// c
		using GravityVelocity			= PhysicalQuantity< T, Dim::MeterPerSecond, typename Scale::SpeedOfGravity >;	// ?
		using AstronomicalUnitPerSecond	= PhysicalQuantity< T, Dim::MeterPerSecond, typename Scale::AstronomicalUnit >;	// AU / s

		using Nanogram					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Pico >;			// ng
		using Microgram					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Nano >;			// ug
		using Milligram					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Micro >;			// mg
		using Gram						= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Milli >;			// g
		using Tone						= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Kilo >;			// t
		using Kilotone					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Mega >;			// kt
		using Megatone					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::Giga >;			// Mt
		using EarthMass					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::EarthMass >;		// EM
		using SolarMass					= PhysicalQuantity< T, Dim::Kilogram, typename Scale::SolarMass >;		// SM

		using Nanosecond				= PhysicalQuantity< T, Dim::Second, typename Scale::Nano >;				// ns
		using Microsecond				= PhysicalQuantity< T, Dim::Second, typename Scale::Micro >;			// us
		using Millisecond				= PhysicalQuantity< T, Dim::Second, typename Scale::Milli >;			// ms
		using Minute					= PhysicalQuantity< T, Dim::Second, typename Scale::Minute >;			// min
		using Hour						= PhysicalQuantity< T, Dim::Second, typename Scale::Hour >;				// h
		using Day						= PhysicalQuantity< T, Dim::Second, typename Scale::Day >;				// d
		using Year						= PhysicalQuantity< T, Dim::Second, typename Scale::Year >;				// y
		using ThousandYears				= PhysicalQuantity< T, Dim::Second, PhysicalQuantity_Scale::Mul< typename Scale::Year, typename Scale::Kilo >>;
		using MillionYears				= PhysicalQuantity< T, Dim::Second, PhysicalQuantity_Scale::Mul< typename Scale::Year, typename Scale::Mega >>;

		using ElectronVolt				= PhysicalQuantity< T, Dim::Joule, typename Scale::ElectronVolt >;		// eV
		using NanoJoule					= PhysicalQuantity< T, Dim::Joule, typename Scale::Nano >;				// nJ
		using MicroJoule				= PhysicalQuantity< T, Dim::Joule, typename Scale::Micro >;				// uJ
		using MilliJoule				= PhysicalQuantity< T, Dim::Joule, typename Scale::Milli >;				// mJ
		using KiloJoule					= PhysicalQuantity< T, Dim::Joule, typename Scale::Kilo >;				// KJ
		using MegaJoule					= PhysicalQuantity< T, Dim::Joule, typename Scale::Mega >;				// MJ
		using GigaJoule					= PhysicalQuantity< T, Dim::Joule, typename Scale::Giga >;				// GJ

		using NanoWatt					= PhysicalQuantity< T, Dim::Watt, typename Scale::Nano >;				// nW
		using MicroWatt					= PhysicalQuantity< T, Dim::Watt, typename Scale::Micro >;				// uW
		using MilliWatt					= PhysicalQuantity< T, Dim::Watt, typename Scale::Milli >;				// mW
		using KiloWatt					= PhysicalQuantity< T, Dim::Watt, typename Scale::Kilo >;				// KW
		using MegaWatt					= PhysicalQuantity< T, Dim::Watt, typename Scale::Mega >;				// MW
		using GigaWatt					= PhysicalQuantity< T, Dim::Watt, typename Scale::Giga >;				// GW
		using Horsepower				= PhysicalQuantity< T, Dim::Watt, typename Scale::Horsepower >;

		using Bar						= PhysicalQuantity< T, Dim::Pascal, typename Scale::Bar >;				// bar
		using Atmosphere				= PhysicalQuantity< T, Dim::Pascal, typename Scale::Atmosphere >;		// atm

		using GAcceleration				= PhysicalQuantity< T, Dim::MeterPerSquareSecond, typename Scale::GravitationalAcceleration >;	// g
		using GConstant					= PhysicalQuantity< T, Dim::GConstant, typename Scale::GravitationalConstant >;	// G
		using SolarLuminosity			= PhysicalQuantity< T, Dim::Watt, typename Scale::SolarLuminosity >;			// SL

		using KibiBit					= PhysicalQuantity< T, Dim::Bit, typename Scale::Kibi >;
		using MebiBit					= PhysicalQuantity< T, Dim::Bit, typename Scale::Mebi >;
		using GibiBit					= PhysicalQuantity< T, Dim::Bit, typename Scale::Gibi >;
		using KibiBitPerSecond			= PhysicalQuantity< T, Dim::BitPerSecond, typename Scale::Kibi >;
		using MebiBitPerSecond			= PhysicalQuantity< T, Dim::BitPerSecond, typename Scale::Mebi >;
		using GibiBitPerSecond			= PhysicalQuantity< T, Dim::BitPerSecond, typename Scale::Gibi >;

		using Byte						= PhysicalQuantity< T, Dim::Bit, typename Scale::Byte >;
		using KibiByte					= PhysicalQuantity< T, Dim::Bit, typename Scale::KibiByte >;
		using MebiByte					= PhysicalQuantity< T, Dim::Bit, typename Scale::MebiByte >;
		using GibiByte					= PhysicalQuantity< T, Dim::Bit, typename Scale::GibiByte >;
		using BytePerSecond				= PhysicalQuantity< T, Dim::BitPerSecond, typename Scale::Byte >;
		using KibiBytePerSecond			= PhysicalQuantity< T, Dim::BitPerSecond, typename Scale::KibiByte >;
		using MebiBytePerSecond			= PhysicalQuantity< T, Dim::BitPerSecond, typename Scale::MebiByte >;
		using GibiBytePerSecond			= PhysicalQuantity< T, Dim::BitPerSecond, typename Scale::GibiByte >;

		using PlanckConst				= PhysicalQuantity< T, Dim::Joule::Mul<Dim::Second>, typename Scale::PlanckConst >;			// h
		using PlanckConstDiv2Pi			= PhysicalQuantity< T, Dim::Joule::Mul<Dim::Second>, typename Scale::PlanckConstDiv2Pi >;	// h / 2Pi

		// Radiometric
		using RadiantEnergy				= PhysicalQuantity< T, Dim::RadiantEnergy >;							// Q
		using RadiantFlux				= PhysicalQuantity< T, Dim::RadiantFlux >;								// W
		using Intensity					= PhysicalQuantity< T, Dim::Intensity >;								// W / sr
		using Irradiance				= PhysicalQuantity< T, Dim::Irradiance >;								// W / m^2
		using Radiance					= PhysicalQuantity< T, Dim::Radiance >;									// W / (m^2 * sr)

		// Photometric
		using LuminousEnergy			= PhysicalQuantity< T, Dim::LuminousEnergy >;							// talbot (T)
		using LuminousFlux				= PhysicalQuantity< T, Dim::LuminousFlux >;								// lm
		using LuminousIntensity			= PhysicalQuantity< T, Dim::LuminousIntensity >;						// lm / sr = cd
		using Illuminance				= PhysicalQuantity< T, Dim::Illuminance >;								// lx
		using Luminance					= PhysicalQuantity< T, Dim::Luminance >;								// cd / m^2 = nit
	};


} // AE::Math


namespace AE::Base
{
	template <typename V, typename D, typename S>
	struct TMemCopyAvailable< PhysicalQuantity< V, D, S >>		{ static constexpr bool  value = IsMemCopyAvailable<V>; };

	template <typename V, typename D, typename S>
	struct TZeroMemAvailable< PhysicalQuantity< V, D, S >>		{ static constexpr bool  value = IsZeroMemAvailable<V>; };

	template <typename V, typename D, typename S>
	struct TTriviallySerializable< PhysicalQuantity< V, D, S >>	{ static constexpr bool  value = IsTriviallySerializable<V>; };

} // AE::Base
