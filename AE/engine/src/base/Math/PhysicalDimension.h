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
	struct TPhysicalDimension
	{
		StaticAssert( SecondsDenom		> 0 );
		StaticAssert( KilogramsDenom	> 0 );
		StaticAssert( MetersDenom		> 0 );
		StaticAssert( AmperasDenom		> 0 );
		StaticAssert( KelvinsDenom		> 0 );
		StaticAssert( MolesDenom		> 0 );
		StaticAssert( CandelasDenom		> 0 );
		StaticAssert( CurrencyDenom		> 0 );
		StaticAssert( BitsDenom			> 0 );

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
			using type = TPhysicalDimension<values[0].num, values[0].den,
											values[1].num, values[1].den,
											values[2].num, values[2].den,
											values[3].num, values[3].den,
											values[4].num, values[4].den,
											values[5].num, values[5].den,
											values[6].num, values[6].den,
											values[7].num, values[7].den,
											values[8].num, values[8].den >;
		};


		template <typename Rhs>
		struct _Div {
			static constexpr FractionalI	values[] = {
				(seconds - Rhs::seconds),  (kilograms - Rhs::kilograms),  (meters - Rhs::meters),      (amperes - Rhs::amperes),
				(kelvins - Rhs::kelvins),  (moles - Rhs::moles),          (candelas - Rhs::candelas),  (currency - Rhs::currency),	(bits - Rhs::bits)
			};
			using type = TPhysicalDimension<values[0].num, values[0].den,
											values[1].num, values[1].den,
											values[2].num, values[2].den,
											values[3].num, values[3].den,
											values[4].num, values[4].den,
											values[5].num, values[5].den,
											values[6].num, values[6].den,
											values[7].num, values[7].den,
											values[8].num, values[8].den >;
		};


		template <uint value>
		struct _Pow {
			using type = TPhysicalDimension<seconds  .num * value,	seconds  .den,
											kilograms.num * value,	kilograms.den,
											meters   .num * value,	meters   .den,
											amperes  .num * value,	amperes  .den,
											kelvins  .num * value,	kelvins  .den,
											moles    .num * value,	moles    .den,
											candelas .num * value,	candelas .den,
											currency .num * value,	currency .den,
											bits     .num * value,	bits     .den >;
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
	// Is Physical Dimension
	//
	namespace _hidden_
	{
		template <typename T>
		struct _IsPhysicalDimension : CT_False {};

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
		struct _IsPhysicalDimension< TPhysicalDimension<	SecondsNum,		SecondsDenom,
															KilogramsNum,	KilogramsDenom,
															MetersNum,		MetersDenom,
															AmperasNum,		AmperasDenom,
															KelvinsNum,		KelvinsDenom,
															MolesNum,		MolesDenom,
															CandelasNum,	CandelasDenom,
															CurrencyNum,	CurrencyDenom,
															BitsNum,		BitsDenom > > :
			CT_True {};
	}
	template <typename T>
	static constexpr bool	IsPhysicalDimension = Math::_hidden_::_IsPhysicalDimension<T>::value;



	//
	// Default Physical Dimensions
	//

	struct DefaultPhysicalDimensions
	{
		using NonDimensional			= TPhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;
		using Second					= TPhysicalDimension< 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// s		- SI
		using Kilogram					= TPhysicalDimension< 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// kg		- SI
		using Meter						= TPhysicalDimension< 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// m		- SI
		using Ampere					= TPhysicalDimension< 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// A		- SI
		using Kelvin					= TPhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1 >;	// K		- SI
		using Mole						= TPhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1 >;	// mol		- SI
		using Candela					= TPhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 >;	// cd		- SI
		using Currency					= TPhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1 >;	// $
		using Bit						= TPhysicalDimension< 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1 >;	// bit

		using Diopter					= NonDimensional::Div< Meter >;							// 1/m
		using Radian					= NonDimensional;										// rad = m / m		- planar angle	- SI
		using Steradian					= NonDimensional;										// sr = m^2 / m^2	- 3D angle		- SI
		using SquareMeter				= Meter::Pow< 2 >;										// m^2
		using CubicMeter				= Meter::Pow< 3 >;										// m^3
		using MeterPerSecond			= Meter::Div< Second >;									// m / s
		using MeterPerSquareSecond		= MeterPerSecond::Div< Second >;						// m / s^2
		using KilogramPerSecond			= Kilogram::Div< Second >;								// kg / s
		using KilogramMeterPerSecond	= MeterPerSecond::Mul< Kilogram >;						// kg * m / s
		using KilogramPerCubicMeter		= Kilogram::Div< CubicMeter >;							// kg / m^3
		using Newton					= Kilogram::Mul< MeterPerSquareSecond >;				// N = kg * m / s^2					- SI
		using Joule						= Newton::Mul< Meter >;									// J = kg * (m / s)^2				- SI
		using Pascal					= Kilogram::Div< Meter::Mul< Second::Pow<2> >>;			// Pa = kg / (m * s^2)				- SI
		using Hertz						= NonDimensional::Div< Second >;						// Hz = 1 / s						- SI
		using Watt						= Joule::Div< Second >;									// W = J / s						- SI
		using Coulomb					= Ampere::Mul< Second >;								// C = A * s						- SI
		using Volt						= Joule::Div< Coulomb >;								// V = J / C						- SI
		using Ohm						= Volt::Div< Ampere >;									// Ohm = V / A						- SI
		using Farad						= Coulomb::Div< Volt >;									// F = C / V						- SI
		using Weber						= Volt::Mul< Second >;									// Wb = V * s						- SI
		using Henry						= Weber::Div< Ampere >;									// H = Wb / A						- SI
		using Tesla						= Weber::Div< SquareMeter >;							// T = Wb / m^2						- SI
		using Siemens					= NonDimensional::Div< Ohm >;							// S = 1 / Ohm						- SI
		using Lumen						= Candela::Mul< Steradian >;							// lm = cd * (steradian)			- SI
		using Lux						= Lumen::Div< SquareMeter >;							// lx = lm / m^2					- SI
		using AmperPerMeter				= Ampere::Div< Meter >;									// A / m
		using KilogramPerMole			= Kilogram::Div< Mole >;								// kg / mol
		using BitPerSecond				= Bit::Div< Second >;									// bit / s
		using Becquerel					= NonDimensional::Div< Second >;						// Bq = 1 / s						- SI
		using Gray						= Joule::Div< Kilogram >;								// Gy = J / kg						- SI
		using Sievert					= Joule::Div< Kilogram >;								// Sv = J / kg						- SI
		using Katal						= Mole::Div< Second >;									// kat = mol / s					- SI

		using GConstant					= Meter::Pow<3>::Div< Kilogram::Mul< Second::Pow<2> >>;	// G

		// Radiometric
		using RadiantEnergy				= Joule;												// Q
		using RadiantFlux				= Watt;													// W
		using Intensity					= Watt::Div< Steradian >;								// W / sr
		using Irradiance				= Watt::Div< SquareMeter >;								// W / m^2
		using Radiance					= Watt::Div< SquareMeter::Mul< Steradian >>;			// W / (m^2 * sr)

		// Photometric
		using LuminousEnergy			= RadiantEnergy;										// talbot (T)
		using LuminousFlux				= Lumen;												// lm
		using LuminousIntensity			= Lumen::Div< Steradian >;								// lm / sr = cd
		using Illuminance				= Lux;													// lx
		using Luminance					= Candela::Div< SquareMeter >;							// cd / m^2 = nit
	};



	//
	// Is***Units (base)
	//

	template <typename T>
	static constexpr bool	IsTimeUnits				= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Second >;

	template <typename T>
	static constexpr bool	IsMassUnits				= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Kilogram >;

	template <typename T>
	static constexpr bool	IsDistanceUnits			= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Meter >;

	template <typename T>
	static constexpr bool	IsElectricCurrentUnits	= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Ampere >;

	template <typename T>
	static constexpr bool	IsTemperatureUnits		= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Kelvin >;

	template <typename T>
	static constexpr bool	IsSubstanceAmountUnits	= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Mole >;

	template <typename T>
	static constexpr bool	IsLuminousIntensityUnits= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Candela >;

	template <typename T>
	static constexpr bool	IsCurrencyUnits			= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Currency >;

	template <typename T>
	static constexpr bool	IsInformationUnits		= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::Bit >;


	//
	// Is***Units (complex)
	//

	template <typename T>
	static constexpr bool	IsDensityUnits			= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::KilogramPerCubicMeter >;

	template <typename T>
	static constexpr bool	IsVelocityUnits			= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::MeterPerSecond >;

	template <typename T>
	static constexpr bool	IsAccelerationUnits		= T::Dimension_t::template Equal< typename DefaultPhysicalDimensions::MeterPerSquareSecond >;


	//
	// Is Same Dimensions
	//

	template <typename T1, typename T2>
	static constexpr bool	IsSameDimensions		= T1::Dimension_t::template Equal< typename T2::Dimension_t >;


} // AE::Math
