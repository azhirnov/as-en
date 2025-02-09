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

		template <typename T>	static T  ExtractBit (T& x)					{ return Base::ExtractBit( x ); }
		template <typename T>	static T  ExtractBitIndex (T& x)				{ return Base::ExtractBitIndex( x ); }

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
		se->AddFunction( &ScalarFunc::IntLog2<T>,			"IntLog2",			{"x"} );
		se->AddFunction( &ScalarFunc::CeilIntLog2<T>,		"CeilIntLog2",		{"x"} );
		se->AddFunction( &ScalarFunc::BitScanReverse<T>,	"BitScanReverse",	{"x"} );
		se->AddFunction( &ScalarFunc::BitScanForward<T>,	"BitScanForward",	{"x"} );
		se->AddFunction( &ScalarFunc::BitCount<T>,			"BitCount",			{"x"} );
		se->AddFunction( &ScalarFunc::IsPowerOfTwo<T>,		"IsPowerOfTwo",		{"x"} );
		se->AddFunction( &ScalarFunc::IsSingleBitSet<T>,	"IsSingleBitSet",	{"x"} );
		se->AddFunction( &ScalarFunc::AllBits<T>,			"AllBits",			{"x", "y"} );
		se->AddFunction( &ScalarFunc::AnyBits<T>,			"AnyBits",			{"x", "y"} );
		se->AddFunction( &ScalarFunc::ExtractBit<T>,		"ExtractBit",		{"x"} );
		se->AddFunction( &ScalarFunc::ExtractBitIndex<T>,	"ExtractBitIndex",	{"x"} );
		se->AddFunction( &ScalarFunc::BitRotateLeft<T>,		"BitRotateLeft",	{"x", "shift"} );
		se->AddFunction( &ScalarFunc::BitRotateRight<T>,	"BitRotateRight",	{"x", "shift"} );
		se->AddFunction( &ScalarFunc::FloorPOT<T>,			"FloorPOT",			{"x"} );
		se->AddFunction( &ScalarFunc::CeilPOT<T>,			"CeilPOT",			{"x"} );

		se->AddFunction( &ScalarFunc::AlignDown<T>,			"AlignDown",		{"x", "align"} );
		se->AddFunction( &ScalarFunc::AlignUp<T>,			"AlignUp",			{"x", "align"} );
		se->AddFunction( &ScalarFunc::IsMultipleOf<T>,		"IsMultipleOf",		{"x", "align"} );

		se->AddFunction( &ScalarFunc::SafeDiv<T>,			"SafeDiv",			{"x", "y", "defVal"} );
		se->AddFunction( &ScalarFunc::DivCeil<T>,			"DivCeil",			{"x", "y"} );
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
		se->AddFunction( &ScalarFunc::Floor<T>,			"Floor",		{"x"} );
		se->AddFunction( &ScalarFunc::Ceil<T>,			"Ceil",			{"x"} );
		se->AddFunction( &ScalarFunc::Trunc<T>,			"Trunc",		{"x"} );
		se->AddFunction( &ScalarFunc::Fract<T>,			"Fract",		{"x"} );
		se->AddFunction( &ScalarFunc::Round<T>,			"Round",		{"x"} );
		se->AddFunction( &ScalarFunc::Mod<T>,			"Mod",			{"x", "y"} );

		//
		se->AddFunction( &ScalarFunc::Sqrt<T>,			"Sqrt",			{"x"} );
		se->AddFunction( &ScalarFunc::Pow<T>,			"Pow",			{"x", "pow"} );
		se->AddFunction( &ScalarFunc::Ln<T>,			"Ln",			{"x"} );
		se->AddFunction( &ScalarFunc::Log<T>,			"Log",			{"x", "base"} );
		se->AddFunction( &ScalarFunc::Log2<T>,			"Log2",			{"x"} );
		se->AddFunction( &ScalarFunc::Log10<T>,			"Log10",		{"x"} );
		se->AddFunction( &ScalarFunc::Exp<T>,			"Exp",			{"x"} );
		se->AddFunction( &ScalarFunc::Exp2<T>,			"Exp2",			{"x"} );
		se->AddFunction( &ScalarFunc::Exp10<T>,			"Exp10",		{"x"} );
		se->AddFunction( &ScalarFunc::ExpMinus1<T>,		"ExpMinus1",	{"x"} );

		// trigonometry
		se->AddFunction( &ScalarFunc::Sin<T>,			"Sin",			{"x"} );
		se->AddFunction( &ScalarFunc::SinH<T>,			"SinH",			{"x"} );
		se->AddFunction( &ScalarFunc::ASin<T>,			"ASin",			{"x"} );
		se->AddFunction( &ScalarFunc::Cos<T>,			"Cos",			{"x"} );
		se->AddFunction( &ScalarFunc::CosH<T>,			"CosH",			{"x"} );
		se->AddFunction( &ScalarFunc::ACos<T>,			"ACos",			{"x"} );
		se->AddFunction( &ScalarFunc::Tan<T>,			"Tan",			{"x"} );
		se->AddFunction( &ScalarFunc::TanH<T>,			"TanH",			{"x"} );
		se->AddFunction( &ScalarFunc::ATan<T>,			"ATan",			{"y", "x"} );
		se->AddFunction( &ScalarFunc::ToRad<T>,			"ToRad",		{"x"} );
		se->AddFunction( &ScalarFunc::ToDeg<T>,			"ToDeg",		{"x"} );

		// interpolation
		se->AddFunction( &ScalarFunc::Lerp<T>,			"Lerp",			{"x", "y", "factor"} );

		se->AddFunction( &ScalarFunc::ToSNorm<T>,		"ToSNorm",		{"x"} );
		se->AddFunction( &ScalarFunc::ToUNorm<T>,		"ToUNorm",		{"x"} );

		se->AddFunction( &ScalarFunc::Remap<T>,			"Remap",		{"srcMin", "srcMax", "dstMin", "dstMax", "x"} );
		se->AddFunction( &ScalarFunc::RemapClamp<T>,	"RemapClamp",	{"srcMin", "srcMax", "dstMin", "dstMax", "x"} );

		se->AddFunction( &ScalarFunc::RoundToInt<T>,	"RoundToInt",	{"x"} );
		se->AddFunction( &ScalarFunc::RoundToUint<T>,	"RoundToUint",	{"x"} );

		se->AddFunction( &ScalarFunc::IsInfinity<T>,	"IsInfinity",	{"x"} );
		se->AddFunction( &ScalarFunc::IsNaN<T>,			"IsNaN",		{"x"} );
		se->AddFunction( &ScalarFunc::IsFinite<T>,		"IsFinite",		{"x"} );

		if constexpr( IsSame< T, float >)
			se->AddFunction( &ScalarFunc::Pi<T>,		"Pi",			{} );

		if constexpr( IsSame< T, double >)
			se->AddFunction( &ScalarFunc::Pi<T>,		"dPi",			{} );
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
			se->AddFunction( &ScalarFunc::Abs<T>,			"Abs",			{"x"} );
			se->AddFunction( &ScalarFunc::MirroredWrap<T>,	"MirroredWrap",	{"x", "min", "max"} );
		}

		se->AddFunction( &ScalarFunc::Square<T>,		"Square",		{"x"} );

		se->AddFunction( &ScalarFunc::Min<T>,			"Min",			{"x", "y"} );
		se->AddFunction( &ScalarFunc::Max<T>,			"Max",			{"x", "y"} );
		se->AddFunction( &ScalarFunc::Clamp<T>,			"Clamp",		{"x", "min", "max"} );
		se->AddFunction( &ScalarFunc::Wrap<T>,			"Wrap",			{"x", "min", "max"} );

		se->AddFunction( &ScalarFunc::Average<T>,		"Average",		{"x", "y"} );
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
