// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
/*
=================================================
	ScalarFunc
=================================================
*/
	struct ScalarFunc
	{
		template <typename T>	static T  Abs (T value)						{ return Base::Abs( value ); }
		template <typename T>	static T  Floor (T value)					{ return Base::Floor( value ); }
		template <typename T>	static T  Ceil (T value)					{ return Base::Ceil( value ); }
		template <typename T>	static T  Trunc (T value)					{ return Base::Trunc( value ); }
		template <typename T>	static T  Fract (T value)					{ return Base::Fract( value ); }

		template <typename T>	static T  Round (T value)					{ return Base::Round( value ); }
		template <typename T>	static auto  RoundToInt (T value)			{ return Base::RoundToInt( value ); }
		template <typename T>	static auto  RoundToUint (T value)			{ return Base::RoundToUint( value ); }

		template <typename T>	static T  AlignDown (T value, T align)		{ return Base::AlignDown( value, align ); }
		template <typename T>	static T  AlignUp (T value, T align)		{ return Base::AlignUp( value, align ); }
		template <typename T>	static bool  IsMultipleOf (T value, T align){ return Base::IsMultipleOf( value, align ); }

		template <typename T>	static T  SafeDiv (T lhs, T rhs, T def)		{ return Base::SafeDiv( lhs, rhs, def ); }
		template <typename T>	static T  DivCeil (T lhs, T rhs)			{ return Base::DivCeil( lhs, rhs ); }

		template <typename T>	static bool  All (T value)					{ return Base::All( value ); }
		template <typename T>	static bool  Any (T value)					{ return Base::Any( value ); }

		template <typename T>	static T  Pow (T x, T y)					{ return Base::Pow( x, y ); }
		template <typename T>	static T  Ln (T x)							{ return Base::Ln( x ); }
		template <typename T>	static T  Log (T x, T base)					{ return Base::Log( x, base ); }
		template <typename T>	static T  Log2 (T x)						{ return Base::Log2( x ); }
		template <typename T>	static T  Log10 (T x)						{ return Base::Log10( x ); }
		template <typename T>	static T  Exp (T x)							{ return Base::Exp( x ); }
		template <typename T>	static T  Exp2 (T x)						{ return Base::Exp2( x ); }
		template <typename T>	static T  Exp10 (T x)						{ return Base::Exp10( x ); }
		template <typename T>	static T  ExpMinus1 (T x)					{ return Base::ExpMinus1( x ); }

		template <typename T>	static int  IntLog2 (T x)					{ return Base::IntLog2( x ); }
		template <typename T>	static int  CeilIntLog2 (T x)				{ return Base::CeilIntLog2( x ); }
		template <typename T>	static int  BitScanReverse (T x)			{ return Base::BitScanReverse( x ); }
		template <typename T>	static int  BitScanForward (T x)			{ return Base::BitScanForward( x ); }
		template <typename T>	static uint  BitCount (T x)					{ using U = ToUnsignedInteger<T>; return uint(Base::BitCount( U(x) )); }
		template <typename T>	static bool  IsPowerOfTwo (T x)				{ return Base::IsPowerOfTwo( x ); }
		template <typename T>	static bool  IsSingleBitSet (T x)			{ return Base::IsSingleBitSet( x ); }

		template <typename T>	static bool  AllBits (T x, T y)				{ return Base::AllBits( x, y ); }
		template <typename T>	static bool  AnyBits (T x, T y)				{ return Base::AnyBits( x, y ); }

		template <typename T>	static T  ExtractBit (T& x)					{ return Base::ExtractBit( INOUT x ); }
		template <typename T>	static T  ExtractBitIndex (T& x)			{ return Base::ExtractBitIndex( INOUT x ); }

		template <typename T>	static T  BitRotateLeft (T x, uint shift)	{ return Base::BitRotateLeft( x, shift ); }
		template <typename T>	static T  BitRotateRight (T x, uint shift)	{ return Base::BitRotateRight( x, shift ); }

		template <typename T>	static T  Sin (T value)						{ return Base::Sin( TRadian<T>{value} ); }
		template <typename T>	static T  SinH (T value)					{ return Base::SinH( TRadian<T>{value} ); }
		template <typename T>	static T  ASin (T value)					{ return T(Base::ASin( value )); }
		template <typename T>	static T  Cos (T value)						{ return Base::Cos( TRadian<T>{value} ); }
		template <typename T>	static T  CosH (T value)					{ return Base::CosH( TRadian<T>{value} ); }
		template <typename T>	static T  ACos (T value)					{ return T(Base::ACos( value )); }
		template <typename T>	static T  Tan (T value)						{ return Base::Tan( TRadian<T>{value} ); }
		template <typename T>	static T  TanH (T value)					{ return Base::TanH( TRadian<T>{value} ); }
		template <typename T>	static T  ATan (T y, T x)					{ return T(Base::ATan( y, x )); }

		template <typename T>	static T  Square (T value)					{ return Base::Square( value ); }
		template <typename T>	static T  Sqrt (T value)					{ return Base::Sqrt( value ); }
		template <typename T>	static T  Mod (T x, T y)					{ return std::fmod( x, y ); }

		template <typename T>	static T  FloorPOT (T x)					{ return Base::FloorPOT( x ); }
		template <typename T>	static T  CeilPOT (T x)						{ return Base::CeilPOT( x ); }

		template <typename T>	static T  IsInfinity (T x)					{ return Base::IsInfinity( x ); }
		template <typename T>	static T  IsNaN (T x)						{ return Base::IsNaN( x ); }
		template <typename T>	static T  IsFinite (T x)					{ return Base::IsFinite( x ); }

		template <typename T>	static T  Lerp (T x, T y, T factor)			{ return Base::Lerp( x, y, factor ); }

		template <typename T>	static T  Min (T x, T y)					{ return Base::Min( x, y ); }
		template <typename T>	static T  Max (T x, T y)					{ return Base::Max( x, y ); }
		template <typename T>	static T  Clamp (T x, T min, T max)			{ return Base::Clamp( x, min, max ); }
		template <typename T>	static T  Wrap (T x, T min, T max)			{ return Base::Wrap( x, min, max ); }
		template <typename T>	static T  MirroredWrap (T x, T min, T max)	{ return Base::MirroredWrap( x, min, max ); }

		template <typename T>	static T  ToRad (T x)						{ return TRadian<T>::DegToRad() * x; }
		template <typename T>	static T  ToDeg (T x)						{ return TRadian<T>::RadToDeg() * x; }

		template <typename T>	static T  ToSNorm (T x)						{ return Base::ToSNorm( x ); }
		template <typename T>	static T  ToUNorm (T x)						{ return Base::ToUNorm( x ); }

		template <typename T>	static T  Average (T x, T y)				{ return Base::Average( x, y ); }

		template <typename T>	static T  Remap (T srcMin, T srcMax, T dstMin, T dstMax, T x)		{ return Base::Remap( Range<T>{srcMin, srcMax}, Range<T>{dstMin, dstMax}, x ); }
		template <typename T>	static T  RemapClamp (T srcMin, T srcMax, T dstMin, T dstMax, T x)	{ return Base::RemapClamp( Range<T>{srcMin, srcMax}, Range<T>{dstMin, dstMax}, x ); }

		template <typename T>	static T  Pi ()								{ return T{Base::TRadian<T>::Pi()}; }
	};

/*
=================================================
	BindBoolScalar
=================================================
*/
	template <typename T>
	static void  BindBoolScalar (const ScriptEnginePtr &)
	{
	}

/*
=================================================
	BindIntScalar
=================================================
*/
	template <typename T>
	static void  BindIntScalar (const ScriptEnginePtr &se)
	{
		AS_GLOBAL_FN( se, ScalarFunc::IntLog2<T>,			"IntLog2",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::CeilIntLog2<T>,		"CeilIntLog2",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::BitScanReverse<T>,	"BitScanReverse",	{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::BitScanForward<T>,	"BitScanForward",	{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::BitCount<T>,			"BitCount",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::IsPowerOfTwo<T>,		"IsPowerOfTwo",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::IsSingleBitSet<T>,	"IsSingleBitSet",	{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::AllBits<T>,			"AllBits",			{"x", "y"} );
		AS_GLOBAL_FN( se, ScalarFunc::AnyBits<T>,			"AnyBits",			{"x", "y"} );
		AS_GLOBAL_FN( se, ScalarFunc::ExtractBit<T>,		"ExtractBit",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ExtractBitIndex<T>,	"ExtractBitIndex",	{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::BitRotateLeft<T>,		"BitRotateLeft",	{"x", "shift"} );
		AS_GLOBAL_FN( se, ScalarFunc::BitRotateRight<T>,	"BitRotateRight",	{"x", "shift"} );
		AS_GLOBAL_FN( se, ScalarFunc::FloorPOT<T>,			"FloorPOT",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::CeilPOT<T>,			"CeilPOT",			{"x"} );

		AS_GLOBAL_FN( se, ScalarFunc::AlignDown<T>,			"AlignDown",		{"x", "align"} );
		AS_GLOBAL_FN( se, ScalarFunc::AlignUp<T>,			"AlignUp",			{"x", "align"} );
		AS_GLOBAL_FN( se, ScalarFunc::IsMultipleOf<T>,		"IsMultipleOf",		{"x", "align"} );

		AS_GLOBAL_FN( se, ScalarFunc::SafeDiv<T>,			"SafeDiv",			{"x", "y", "defVal"} );
		AS_GLOBAL_FN( se, ScalarFunc::DivCeil<T>,			"DivCeil",			{"x", "y"} );
	}

/*
=================================================
	BindFloatScalar
=================================================
*/
	template <typename T>
	static void  BindFloatScalar (const ScriptEnginePtr &se)
	{
		const String	suffix = IsSame<T,float> ? "" : "D";

		//
		AS_GLOBAL_FN( se, ScalarFunc::Floor<T>,			"Floor",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Ceil<T>,			"Ceil",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Trunc<T>,			"Trunc",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Fract<T>,			"Fract",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Round<T>,			"Round",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Mod<T>,			"Mod",			{"x", "y"} );

		//
		AS_GLOBAL_FN( se, ScalarFunc::Sqrt<T>,			"Sqrt",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Pow<T>,			"Pow",			{"x", "pow"} );
		AS_GLOBAL_FN( se, ScalarFunc::Ln<T>,			"Ln",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Log<T>,			"Log",			{"x", "base"} );
		AS_GLOBAL_FN( se, ScalarFunc::Log2<T>,			"Log2",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Log10<T>,			"Log10",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Exp<T>,			"Exp",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Exp2<T>,			"Exp2",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Exp10<T>,			"Exp10",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ExpMinus1<T>,		"ExpMinus1",	{"x"} );

		// trigonometry
		AS_GLOBAL_FN( se, ScalarFunc::Sin<T>,			"Sin",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::SinH<T>,			"SinH",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ASin<T>,			"ASin",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Cos<T>,			"Cos",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::CosH<T>,			"CosH",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ACos<T>,			"ACos",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::Tan<T>,			"Tan",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::TanH<T>,			"TanH",			{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ATan<T>,			"ATan",			{"y", "x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ToRad<T>,			"ToRad",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ToDeg<T>,			"ToDeg",		{"x"} );

		// interpolation
		AS_GLOBAL_FN( se, ScalarFunc::Lerp<T>,			"Lerp",			{"x", "y", "factor"} );

		AS_GLOBAL_FN( se, ScalarFunc::ToSNorm<T>,		"ToSNorm",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::ToUNorm<T>,		"ToUNorm",		{"x"} );

		AS_GLOBAL_FN( se, ScalarFunc::Remap<T>,			"Remap",		{"srcMin", "srcMax", "dstMin", "dstMax", "x"} );
		AS_GLOBAL_FN( se, ScalarFunc::RemapClamp<T>,	"RemapClamp",	{"srcMin", "srcMax", "dstMin", "dstMax", "x"} );

		AS_GLOBAL_FN( se, ScalarFunc::RoundToInt<T>,	"RoundToInt",	{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::RoundToUint<T>,	"RoundToUint",	{"x"} );

		AS_GLOBAL_FN( se, ScalarFunc::IsInfinity<T>,	"IsInfinity",	{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::IsNaN<T>,			"IsNaN",		{"x"} );
		AS_GLOBAL_FN( se, ScalarFunc::IsFinite<T>,		"IsFinite",		{"x"} );

		if constexpr( IsSame< T, float >)
			AS_GLOBAL_FN( se, ScalarFunc::Pi<T>,		"Pi",			{} );

		if constexpr( IsSame< T, double >)
			AS_GLOBAL_FN( se, ScalarFunc::Pi<T>,		"dPi",			{} );
	}

/*
=================================================
	BindIntFloatScalar
=================================================
*/
	template <typename T>
	static void  BindIntFloatScalar (const ScriptEnginePtr &se)
	{
		if constexpr( IsSignedInteger<T> or IsFloatPoint<T> )
		{
			AS_GLOBAL_FN( se, ScalarFunc::Abs<T>,			"Abs",			{"x"} );
			AS_GLOBAL_FN( se, ScalarFunc::MirroredWrap<T>,	"MirroredWrap",	{"x", "min", "max"} );
		}

		AS_GLOBAL_FN( se, ScalarFunc::Square<T>,		"Square",		{"x"} );

		AS_GLOBAL_FN( se, ScalarFunc::Min<T>,			"Min",			{"x", "y"} );
		AS_GLOBAL_FN( se, ScalarFunc::Max<T>,			"Max",			{"x", "y"} );
		AS_GLOBAL_FN( se, ScalarFunc::Clamp<T>,			"Clamp",		{"x", "min", "max"} );
		AS_GLOBAL_FN( se, ScalarFunc::Wrap<T>,			"Wrap",			{"x", "min", "max"} );

		AS_GLOBAL_FN( se, ScalarFunc::Average<T>,		"Average",		{"x", "y"} );
	}

} // namespace


/*
=================================================
	BindScalarMath
=================================================
*/
	void  CoreBindings::BindScalarMath (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		BindBoolScalar<bool>( se );

		BindIntFloatScalar<int>( se );
		BindIntScalar<int>( se );

		BindIntFloatScalar<uint>( se );
		BindIntScalar<uint>( se );

		BindIntFloatScalar<float>( se );
		BindFloatScalar<float>( se );
	}


} // AE::Scripting
