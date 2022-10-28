// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Fractional.h"

namespace AE::Math
{


	//
	// Physical Dimension
	//

	template <int SecondsNum,	int SecondsDenom,
			  int KilogramsNum,	int KilogramsDenom,
			  int MetersNum,	int MetersDenom,
			  int AmperasNum,	int AmperasDenom,
			  int KelvinsNum,	int KelvinsDenom,
			  int MolesNum,		int MolesDenom,
			  int CandelasNum,	int CandelasDenom,
			  int CurrencyNum,	int CurrencyDenom,
			  int BitsNum,		int BitsDenom
			>
	struct PhysicalDimension
	{
		STATIC_ASSERT( SecondsDenom		> 0 );
		STATIC_ASSERT( KilogramsDenom	> 0 );
		STATIC_ASSERT( MetersDenom		> 0 );
		STATIC_ASSERT( AmperasDenom		> 0 );
		STATIC_ASSERT( KelvinsDenom		> 0 );
		STATIC_ASSERT( MolesDenom		> 0 );
		STATIC_ASSERT( CandelasDenom	> 0 );
		STATIC_ASSERT( CurrencyDenom	> 0 );
		STATIC_ASSERT( BitsDenom		> 0 );

		//				SI
		static constexpr FractionalI	seconds		{ SecondsNum,	SecondsDenom };		// time
		static constexpr FractionalI	kilograms	{ KilogramsNum,	KilogramsDenom };	// mass
		static constexpr FractionalI	meters		{ MetersNum,	MetersDenom };		// length
		static constexpr FractionalI	amperes		{ AmperasNum,	AmperasDenom };		// electric current
		static constexpr FractionalI	kelvins		{ KelvinsNum,	KelvinsDenom };		// temperature
		static constexpr FractionalI	moles		{ MolesNum,		MolesDenom };		// amount of substance
		static constexpr FractionalI	candelas	{ CandelasNum,	CandelasDenom };	// luminous intensity

		//				non-SI
		static constexpr FractionalI	currency	{ CurrencyNum,	CurrencyDenom };	// monetary unit
		static constexpr FractionalI	bits		{ BitsNum,		BitsDenom };		// unit of information


		template <typename Rhs>
		inline static constexpr bool  Equal	=  (seconds		== Rhs::seconds		and
												kilograms	== Rhs::kilograms	and
												meters		== Rhs::meters		and
												amperes		== Rhs::amperes		and
												kelvins		== Rhs::kelvins		and
												moles		== Rhs::moles		and
												candelas	== Rhs::candelas	and
												currency	== Rhs::currency	and
												bits		== Rhs::bits);
		
		template <typename Rhs>
		struct _Mul {
			static constexpr FractionalI	values[] = {
				(seconds + Rhs::seconds),  (kilograms + Rhs::kilograms),  (meters + Rhs::meters),      (amperes + Rhs::amperes),
				(kelvins + Rhs::kelvins),  (moles + Rhs::moles),          (candelas + Rhs::candelas),  (currency + Rhs::currency),	(bits + Rhs::bits)
			};
			using type = PhysicalDimension<	values[0].numerator, values[0].denominator,
											values[1].numerator, values[1].denominator,
											values[2].numerator, values[2].denominator,
											values[3].numerator, values[3].denominator,
											values[4].numerator, values[4].denominator,
											values[5].numerator, values[5].denominator,
											values[6].numerator, values[6].denominator,
											values[7].numerator, values[7].denominator,
											values[8].numerator, values[8].denominator >;
		};

		
		template <typename Rhs>
		struct _Div {
			static constexpr FractionalI	values[] = {
				(seconds - Rhs::seconds),  (kilograms - Rhs::kilograms),  (meters - Rhs::meters),      (amperes - Rhs::amperes),
				(kelvins - Rhs::kelvins),  (moles - Rhs::moles),          (candelas - Rhs::candelas),  (currency - Rhs::currency),	(bits - Rhs::bits)
			};
			using type = PhysicalDimension<	values[0].numerator, values[0].denominator,
											values[1].numerator, values[1].denominator,
											values[2].numerator, values[2].denominator,
											values[3].numerator, values[3].denominator,
											values[4].numerator, values[4].denominator,
											values[5].numerator, values[5].denominator,
											values[6].numerator, values[6].denominator,
											values[7].numerator, values[7].denominator,
											values[8].numerator, values[8].denominator >;
		};
		

		template <uint value>
		struct _Pow {
			using type = PhysicalDimension<	seconds.numerator	* value,	seconds.denominator,
											kilograms.numerator	* value,	kilograms.denominator,
											meters.numerator	* value,	meters.denominator,
											amperes.numerator	* value,	amperes.denominator,
											kelvins.numerator	* value,	kelvins.denominator,
											moles.numerator		* value,	moles.denominator,
											candelas.numerator	* value,	candelas.denominator,
											currency.numerator	* value,	currency.denominator,
											bits.numerator		* value,	bits.denominator >;
		};
		
		
		template <typename Rhs>	using Mul  = typename _Mul< Rhs >::type;
		template <typename Rhs>	using Div  = typename _Div< Rhs >::type;
		template <uint value>	using Pow  = typename _Pow< value >::type;


		static constexpr bool  IsNonDimensional =	seconds.IsZero()	and kilograms.IsZero()	and
													meters.IsZero()		and amperes.IsZero()	and
													kelvins.IsZero()	and moles.IsZero()		and
													candelas.IsZero()	and currency.IsZero()	and
													bits.IsZero();
	};



	//
	// Default Physical Dimensions
	//

	struct DefaultPhysicalDimensions
	{
		using NonDimensional	= PhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;
		using Second			= PhysicalDimension< 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// s
		using Kilogram			= PhysicalDimension< 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// kg
		using Meter				= PhysicalDimension< 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// m
		using Ampere			= PhysicalDimension< 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// A
		using Kelvin			= PhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// K
		using Mole				= PhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1 >;	// mol
		using Candela			= PhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 >;	// cd
		using Currency			= PhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1 >;	// $
		using Bits				= PhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1 >;	// bits

		using SquareMeter				= Meter::Pow< 2 >;										// m^2
		using CubicMeter				= Meter::Pow< 3 >;										// m^3
		using MeterPerSecond			= Meter::Div< Second >;									// m / s
		using MeterPerSquareSecond		= MeterPerSecond::Div< Second >;						// m / s^2
		using KilogramPerSecond			= Kilogram::Div< Second >;								// kg / s
		using KilogramMeterPerSecond	= MeterPerSecond::Mul< Kilogram >;						// kg * m / s
		using KilogramPerCubicMeter		= Kilogram::Div< CubicMeter >;							// kg / m^3
		using Newton					= Kilogram::Mul< MeterPerSquareSecond >;				// N = kg * m / s^2
		using Joule						= Newton::Mul< Meter >;									// J = kg * (m / s)^2
		using Pascal					= Kilogram::Div< Meter::Mul< Second::Pow<2> >>;			// Pa = kg / (m * s^2)
		using Hertz						= NonDimensional::Div< Second >;						// Hz = 1 / s
		using Watt						= Joule::Div< Second >;									// W = J / s
		using Coulomb					= Ampere::Mul< Second >;								// C = A * s
		using Volt						= Joule::Div< Coulomb >;								// V = J / C
		using Ohm						= Volt::Div< Ampere >;									// Ohm = V / A
		using Farad						= Coulomb::Div< Volt >;									// F = C / V
		using Weber						= Volt::Mul< Second >;									// Wb = V * s
		using Henry						= Weber::Div< Ampere >;									// H = Wb / A
		using Tesla						= Weber::Div< SquareMeter >;							// T = Wb / m^2
		using Siemens					= NonDimensional::Div< Ohm >;							// S = 1 / Ohm
		using Lumen						= Candela;												// lm = cd * (sr)
		using Lux						= Lumen::Div< SquareMeter >;							// lx = lm / m^2
		using AmperPerMeter				= Ampere::Div< Meter >;									// A/m
		using KilogramPerMole			= Kilogram::Div< Mole >;								// kg/mol
	};



	//
	// Is***Units (base)
	//

	template <typename T>
	static constexpr bool	IsTimeUnits				= T::Dimension_t::template Equal< DefaultPhysicalDimensions::Second >::value;

	template <typename T>
	static constexpr bool	IsMassUnits				= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Kilogram >::value;

	template <typename T>
	static constexpr bool	IsDistanceUnits			= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Meter >::value;
	
	template <typename T>
	static constexpr bool	IsElectricCurrentUnits	= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Ampere >::value;
	
	template <typename T>
	static constexpr bool	IsTemperatureUnits		= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Kelvin >::value;
	
	template <typename T>
	static constexpr bool	IsSubstanceAmountUnits	= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Mole >::value;
	
	template <typename T>
	static constexpr bool	IsLuminousIntensityUnits= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Candela >::value;
	
	template <typename T>
	static constexpr bool	IsCurrencyUnits			= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Currency >::value;
	
	template <typename T>
	static constexpr bool	IsInformationUnits		= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Bits >::value;

	
	//
	// Is***Units (complex)
	//

	template <typename T>
	static constexpr bool	IsDensityUnits			= T::Dimension_t::template Equal< DefaultPhysicalDimensions::KilogramPerCubicMeter >::value;

	template <typename T>
	static constexpr bool	IsVelocityUnits			= T::Dimension_t::template Equal< DefaultPhysicalDimensions::MeterPerSecond >::value;
	
	template <typename T>
	static constexpr bool	IsAccelerationUnits		= T::Dimension_t::template Equal< DefaultPhysicalDimensions::MeterPerSquareSecond >::value;
	

	//
	// Is Same Dimensions
	//

	template <typename T1, typename T2>
	static constexpr bool	IsSameDimensions		= T1::Dimension_t::template Equal< T2::Dimension_t >::value;


} // AE::Math
