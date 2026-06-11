// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"

#ifdef AE_ENABLE_LOGS
# include "base/Math/Vec.h"
# include "base/Math/Quat.h"
# include "base/Math/Rectangle.h"
# include "base/Math/VecSwizzle.h"
# include "base/Math/Byte.h"
# include "base/Math/Color.h"
# include "base/Math/BitMath.h"
# include "base/Math/POTValue.h"
# include "base/Math/Range.h"
# include "base/Math/PhysicalQuantity.h"
# include "base/SIMD/Math.h"

# include "base/Utils/Version.h"
# include "base/Utils/EnumSet.h"

namespace AE::Base
{

/*
=================================================
	ToString
=================================================
*/
	Nd__In String  ToString (String value) __Th___
	{
		return RVRef(value);
	}

	Nd__In String  ToString (StringView value) __Th___
	{
		return String{value};
	}

	template <usize C>
	Nd__In String  ToString (const char (&value)[C]) __Th___
	{
		return String{value, C-1};
	}

	Nd__In String  ToString (const char* value) __Th___
	{
		if ( value != null )
			return String{value};
		else
			return String{};
	}

	template <typename T> requires(not IsEnum<T>)
	ND_ String  ToString (const T &value) __Th___
	{
		return std::to_string( value );
	}

	Nd__In StringView  ToString (const bool value) __Th___
	{
		return value ? "true" : "false";
	}

	template <typename T>
	Nd__In String  ToString (const CheckCast<T> &c) __Th___
	{
		return ToString( T{c} );
	}

/*
=================================================
	ToString
=================================================
*/
	template <int Radix, typename T> requires(IsEnum<T> or IsInteger<T>)
	ND_ String  ToString (const T &value) __Th___
	{
		if constexpr( Radix == 10 )
		{
			return std::to_string( value );
		}
		else
		if constexpr( Radix == 16 )
		{
			std::stringstream	str;
			str << std::hex << ToNearUInt( value );
			return str.str();
		}
		else
		if constexpr( Radix == 2 )
		{
			if ( value == T(0) ) return "0";
			return std::bitset< sizeof(T)*8 >{ value }.to_string();
		}
	}

/*
=================================================
	ToString (float / double)
=================================================
*/
namespace _hidden_
{
	template <typename T>
	Nd__In bool  ShouldAddExponent (T value) __NE___
	{
		if constexpr( sizeof(T) >= sizeof(float) )
		{
			using NL = std::numeric_limits< T >;
			StaticAssert( NL::is_specialized );

			T	x = Abs( value );
			return x < T(4.0e-2) and x > NL::min();
		}
		else
		if constexpr( sizeof(T) == sizeof(half) )
		{
			float	x = Abs( float(value) );
			return x < 4.0e-2f and x > 5.9e-8f;
		}
	}
}

	Nd__In String  ToString (const double value, uint fractParts, Bool exponent = False{}) __Th___
	{
		ASSERT( (fractParts > 0) and (fractParts < 100) );
		fractParts = Clamp( fractParts, 1u, 99u );

		const char	fmt[8]  = {'%', '0', '.', char('0' + fractParts / 10), char('0' + fractParts % 10), (exponent ? 'e' : 'f'), '\0' };
		char		buf[32] = {};

		const int	len = std::snprintf( buf, CountOf(buf), fmt, value );
		ASSERT( len > 0 );
		Unused( len );
		return buf;
	}

	Nd__In String  ToString (const double value) __Th___
	{
		return ToString( value, 2, Bool{_hidden_::ShouldAddExponent(value)} );
	}

	Nd__In String  ToString (const float value, const uint fractParts, Bool exponent = False{}) __Th___
	{
		return ToString( double(value), fractParts, exponent );
	}

	Nd__In String  ToString (const float value) __Th___
	{
		return ToString( double(value), 2, Bool{_hidden_::ShouldAddExponent(value)} );
	}

	Nd__In String  ToString (const half value, const uint fractParts, Bool exponent = False{}) __Th___
	{
		return ToString( double(value), fractParts, exponent );
	}

	Nd__In String  ToString (const half value) __Th___
	{
		return ToString( double(value), 2, Bool{_hidden_::ShouldAddExponent(value)} );
	}

  #if AE_SIMD_NEON_HALF
	Nd__In String  ToString (const float16_t value, const uint fractParts, Bool exponent = False{}) __Th___
	{
		return ToString( double(value), fractParts, exponent );
	}

	Nd__In String  ToString (const float16_t value) __Th___
	{
		return ToString( double(value), 2, Bool{_hidden_::ShouldAddExponent(value)} );
	}
  #endif

/*
=================================================
	ToString (Radian)
=================================================
*/
	template <typename T>
	ND_ auto  ToString (const TRadian<T> &value) __Th___
	{
		return ToString( T{value} );
	}

/*
=================================================
	ToString (Vec)
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ String  ToString (const TVec<T,I,Q> &value) __Th___
	{
		String	str = "( ";

		for (int i = 0; i < I; ++i)
		{
			if_likely( i > 0 )
				str << ", ";
			str << ToString( value[i] );
		}
		str << " )";
		return str;
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ String  ToString (const TVec<T,I,Q> &value, const uint fractParts, Bool exponent = False{}) __Th___
	{
		String	str = "( ";

		for (int i = 0; i < I; ++i)
		{
			if_likely( i > 0 )
				str << ", ";
			str << ToString( value[i], fractParts, exponent );
		}
		str << " )";
		return str;
	}

/*
=================================================
	ToString (Quat)
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ String  ToString (const TQuat<T,Q> &value) __Th___
	{
		String	str;
		str << "( w: " << ToString( value.w )
			<< " axis: "
			<< ToString( value.x ) << ", "
			<< ToString( value.y ) << ", "
			<< ToString( value.z ) << " )";
		return str;
	}

/*
=================================================
	ToString (VecSwizzle)
=================================================
*/
	Nd__In String  ToString (const VecSwizzle &value) __Th___
	{
		auto		sw			= value.ToVec();
		const char	symbols []	= "0XYZW0+-";
		char		str [5]		= { symbols[sw.x], symbols[sw.y], symbols[sw.z], symbols[sw.w] };
		return str;
	}

/*
=================================================
	ToString (RGBAColor)
=================================================
*/
	template <typename T>
	ND_ String  ToString (const RGBAColor<T> &value) __Th___
	{
		String	str = "( "s;
		for (uint i = 0; i < 4; ++i)
		{
			if_likely( i > 0 ) str << ", ";
			str << ToString( value[i] );
		}
		str << " )";
		return str;
	}

	template <typename T> requires(IsFloatPoint<T>)
	ND_ String  ToString (const RGBAColor<T> &value, uint fractParts) __Th___
	{
		String	str = "( "s;
		for (uint i = 0; i < 4; ++i)
		{
			if_likely( i > 0 ) str << ", ";
			str << ToString( value[i], fractParts );
		}
		str << " )";
		return str;
	}

/*
=================================================
	ToString (Rectangle)
=================================================
*/
	template <typename T>
	ND_ String  ToString (const Rectangle<T> &value) __Th___
	{
		return "( "s << ToString( value.left ) << ", " << ToString( value.top ) << ", "
					<< ToString( value.right ) << ", " << ToString( value.bottom ) << " )";
	}

/*
=================================================
	ToString (Bytes)
=================================================
*/
	template <typename T>
	ND_ String  ToString (const TByte<T> &value) __Th___
	{
		int			pot		= IntLog2( T{value} );
		uint		shift	= 0;
		StringView	suffix;

		if ( IsPowerOfTwo( T{value} ) and value > 0 )
		{
			if ( pot < 10 )	{ suffix = " B";	shift = 0;	}	else
			if ( pot < 20 )	{ suffix = " KiB";	shift = 10;	}	else
			if ( pot < 30 )	{ suffix = " MiB";	shift = 20;	}	else
			if ( pot < 40 )	{ suffix = " GiB";	shift = 30;	}	else
							{ suffix = " TiB";	shift = 40;	}
		}
		else
		{
			if ( pot < 12 )	{ suffix = " B";	shift = 0;	}	else
			if ( pot < 22 )	{ suffix = " KiB";	shift = 10;	}	else
			if ( pot < 32 )	{ suffix = " MiB";	shift = 20;	}	else
			if ( pot < 42 )	{ suffix = " GiB";	shift = 30;	}	else
							{ suffix = " TiB";	shift = 40;	}
		}
		return ToString( SafeRightBitShift( T{value}, shift )) << suffix;
	}

/*
=================================================
	ToString (TPowerOf2Value)
=================================================
*/
	template <typename T>
	ND_ String  ToString (const TPowerOf2Value<T> &value) __Th___
	{
		return ToString( ulong{value} );
	}

/*
=================================================
	ToString (chrono::duration)
=================================================
*/
	template <typename T, typename Duration>
	ND_ String  ToString (const std::chrono::duration<T,Duration> &value, const uint fractParts = 2) __Th___
	{
		double			time	= TimeCast<secondsd>( value ).count();
		const double	a		= Abs( time );
		StringView		suffix;

		if_unlikely( not IsFinite( time ))
			return "NaN";

		if ( a >= 60.0 * 60.0 )	{ time *= (1.0/3600.0);	suffix = " h";  }	else
		if ( a >= 60.0 )		{ time *= (1.0/60.0);	suffix = " m";  }	else
		if ( a > 0.1 )			{						suffix = " s";  }	else
		if ( a > 1.0e-4 )		{ time *= 1.0e+3;		suffix = " ms"; }	else
		if ( a > 1.0e-7 )		{ time *= 1.0e+6;		suffix = " us"; }	else
		if ( a > 1.0e-10 )		{ time *= 1.0e+9;		suffix = " ns"; }	else
								{ time *= 1.0e+12;		suffix = " ps"; }
		return ToString( time, fractParts ) << suffix;
	}

/*
=================================================
	ToString (Path)
=================================================
*/
	Nd__In String  ToString (const Path &path) __Th___
	{
		String	str; // ansi
      #ifdef AE_PLATFORM_WINDOWS
		Unused( ConvertString( OUT str, BasicStringView{path.lexically_normal().native()} ));
      #else
        const auto& src_str = path.lexically_normal().native();
        Unused( ConvertString( OUT str, BasicStringView<CharUtf8>{ Cast<CharUtf8>(src_str.c_str()), src_str.size() }));
      #endif
		FindAndReplace( INOUT str, '\\', '/' );
		return str;
	}

/*
=================================================
	ToString (U8String)
=================================================
*/
	Nd__In String  ToString (const U8String &str) __Th___
	{
		String	dst;
		Unused( ConvertString( OUT dst, BasicStringView{str} ));
		return dst;
	}

	Nd__In String  ToString (const U8StringView &str) __Th___
	{
		String	dst;
		Unused( ConvertString( OUT dst, str ));
		return dst;
	}

	Nd__In String  ToString (const CharUtf8* str) __Th___
	{
		String	dst;
		Unused( ConvertString( OUT dst, BasicStringView{str} ));
		return dst;
	}

/*
=================================================
	ToString (WString)
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
	Nd__In String  ToString (const WString &str) __Th___
	{
		String	dst;
		Unused( ConvertString( OUT dst, BasicStringView{str} ));
		return dst;
	}

	Nd__In String  ToString (const WStringView &str) __Th___
	{
		String	dst;
		Unused( ConvertString( OUT dst, BasicStringView{str} ));
		return dst;
	}

	Nd__In String  ToString (const wchar_t* str) __Th___
	{
		String	dst;
		Unused( ConvertString( OUT dst, BasicStringView{str} ));
		return dst;
	}
#endif

/*
=================================================
	ToString (Fractional)
=================================================
*/
	template <typename T>
	ND_ String  ToString (Fractional<T> value) __Th___
	{
		String	str = ToString( value.num );

		if ( value.num != 0 and value.den > 1 )
			str << '/' << ToString( value.den );

		return str;
	}

/*
=================================================
	ToString (TPhysicalDimension)
=================================================
*/
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
	ND_ String  ToString (TPhysicalDimension< SecondsNum,	SecondsDenom,
											  KilogramsNum,	KilogramsDenom,
											  MetersNum,	MetersDenom,
											  AmperasNum,	AmperasDenom,
											  KelvinsNum,	KelvinsDenom,
											  MolesNum,		MolesDenom,
											  CandelasNum,	CandelasDenom,
											  CurrencyNum,	CurrencyDenom,
											  BitsNum,		BitsDenom >  value) __Th___
	{
		using Dim = decltype(value);

		String	str_nom;
		String	str_den;
		uint	dim_cnt[2] = {};

		const auto	Append = [&] (FractionalI frac, StringView name)
		{{
			if ( frac.IsZero() )
				return;

			String&	str = frac.IsPositive() ? str_nom : str_den;
			uint&	cnt	= dim_cnt[ frac.IsPositive() ];

			if ( not str.empty() )
				str << " * ";

			str << name;
			if ( not (frac.IsInteger() and frac.num == 1) )
				str << '^' << ToString( frac );

			++cnt;
		}};

		Append( Dim::seconds,	"s"		);
		Append( Dim::kilograms,	"kg"	);
		Append( Dim::meters,	"m"		);
		Append( Dim::amperes,	"A"		);
		Append( Dim::kelvins,	"K"		);
		Append( Dim::moles,		"mol"	);
		Append( Dim::candelas,	"cd"	);
		Append( Dim::currency,	"$"		);
		Append( Dim::bits,		"bits"	);

		if ( dim_cnt[1] > 1 )
			('(' >> str_nom) << ')';

		if ( dim_cnt[0] > 1 )
			('(' >> str_den) << ')';

		if ( str_nom.empty() )
			str_nom << '1';

		if ( not str_den.empty() )
			str_nom << " / " << str_den;

		return str_nom;
	}

/*
=================================================
	ToString (PhysicalQuantity)
=================================================
*/
	template <typename V, typename D, typename S> requires(IsInteger<V>)
	ND_ String  ToString (const PhysicalQuantity<V,D,S> &value) __Th___
	{
		return ToString( value.GetScaled() ) << '[' << ToString( D{} ) << ']';
	}

	template <typename V, typename D, typename S> requires(IsFloatPoint<V>)
	ND_ String  ToString (const PhysicalQuantity<V,D,S> &value, uint fractParts, Bool exponent = True{}) __Th___
	{
		return ToString( value.GetScaled(), fractParts, exponent ) << '[' << ToString( D{} ) << ']';
	}

	template <typename V, typename D, typename S> requires(IsFloatPoint<V>)
	ND_ String  ToString (const PhysicalQuantity<V,D,S> &value) __Th___
	{
		return ToString( value, 2 );
	}

/*
=================================================
	ToString (PhysicalQuantity)
=================================================
*/
	template <typename V, typename D, typename S> requires(IsInteger<V>)
	ND_ String  ToDebugString (const PhysicalQuantity<V,D,S> &value) __Th___
	{
		return ToString( value.GetNonScaled() ) << '*' << ToString( S::Value ) << '[' << ToString( D{} ) << ']';
	}

	template <typename V, typename D, typename S> requires(IsFloatPoint<V>)
	ND_ String  ToDebugString (const PhysicalQuantity<V,D,S> &value, uint fractParts = 2, Bool exponent = True{}) __Th___
	{
		return ToString( value.GetNonScaled(), fractParts, exponent ) << '*' << ToString( S::Value, fractParts, exponent ) << '[' << ToString( D{} ) << ']';
	}

/*
=================================================
	ToString (Version)
=================================================
*/
	template <uint UID>
	ND_ String  ToString (TVersion2<UID> value) __Th___
	{
		return ToString( value.major ) << '.' << ToString( value.minor );
	}

	template <uint UID>
	ND_ String  ToString (TVersion3<UID> value) __Th___
	{
		return ToString( value.major ) << '.' << ToString( value.minor ) << '.' << ToString( value.patch );
	}

/*
=================================================
	ToString (Tuple)
=================================================
*/
namespace _hidden_
{
	struct TupleToString
	{
		String	str;

		template <typename T>
		void  operator () (const T &x) __Th___
		{
			if ( not str.empty() ) str << ", ";
			if constexpr( IsSpecializationOf< T, BasicString >		or
						  IsSpecializationOf< T, BasicStringView >	or
						  IsSame< T, Path >					or
						  (IsPointer<T> and IsChar<RemovePointer<T>>)
						 )
				str << '\'' << ToString( x ) << '\'';
			else
				str << ToString( x );
		}
	};
}

	template <typename ...Types>
	ND_ String  ToString (const Tuple<Types...> &t) __Th___
	{
		Base::_hidden_::TupleToString	tmp;
		t.ForEach( tmp );
		return RVRef(tmp.str);
	}

/*
=================================================
	ToString (Array)
=================================================
*/
namespace _hidden_
{
	template <typename T, typename ToStringFn>
	ND_ String  Array_ToString (ArrayView<T> arr, ToStringFn fn, StringView div) __Th___
	{
		String	str;
		for (const auto& item : arr)
		{
			if ( not str.empty() )
				str << div;

			str << fn( item );
		}
		return str;
	}
}

	template <typename T>
	ND_ String  ToString (ArrayView<T> arr, StringView (*fn)(const T &) = &ToString, StringView div = ", ") __Th___
	{
		return Base::_hidden_::Array_ToString( arr, fn, div );
	}

	template <typename T>
	ND_ String  ToString (ArrayView<T> arr, String (*fn)(const T &) = &ToString, StringView div = ", ") __Th___
	{
		return Base::_hidden_::Array_ToString( arr, fn, div );
	}

	template <typename T, usize S>
	ND_ String  ToString (const StaticArray<T,S> &arr, StringView (*fn)(const T &) = &ToString, StringView div = ", ") __Th___
	{
		return Base::_hidden_::Array_ToString( ArrayView<T>{arr}, fn, div );
	}

	template <typename T, usize S>
	ND_ String  ToString (const StaticArray<T,S> &arr, String (*fn)(const T &) = &ToString, StringView div = ", ") __Th___
	{
		return Base::_hidden_::Array_ToString( ArrayView<T>{arr}, fn, div );
	}

	template <typename T, typename A>
	ND_ String  ToString (const Array<T,A> &arr, StringView (*fn)(const T &) = &ToString, StringView div = ", ") __Th___
	{
		return Base::_hidden_::Array_ToString( ArrayView<T>{arr}, fn, div );
	}

	template <typename T, typename A>
	ND_ String  ToString (const Array<T,A> &arr, String (*fn)(const T &) = &ToString, StringView div = ", ") __Th___
	{
		return Base::_hidden_::Array_ToString( ArrayView<T>{arr}, fn, div );
	}

/*
=================================================
	ToString (EnumSet)
=================================================
*/
	template <typename E>
	ND_ String  ToString (EnumSet<E> bits, StringView (*fn)(E), StringView div = " | ") __Th___
	{
		String	str;
		for (; bits.Any();)
		{
			if ( not str.empty() )
				str << div;

			str << fn( bits.ExtractFirst() );
		}
		return str;
	}

/*
=================================================
	ToString (Range)
=================================================
*/
	template <typename T>
	ND_ String  ToString (const Range<T> &range) __Th___
	{
		return "["s << ToString( range.begin ) << "; " << ToString( range.end ) << "]";
	}

/*
=================================================
	ToStringSfx
=================================================
*/
	template <typename T> requires(IsUnsignedInteger<T>)
	ND_ String  ToStringSfx (T v) __Th___
	{
		char	suffix = 0;
		if ( v < T(10'000) )			{}																else
		if ( v < T(10'000'000) )		{ v = (v + T(500))			/ T(1000);			suffix = 'K'; }	else
		if ( v < T(10'000'000'000) )	{ v = (v + T(500'000))		/ T(1000'000);		suffix = 'M'; }	else
										{ v = (v + T(500'000'000))	/ T(1000'000'000);	suffix = 'G'; }
		String	str = ToString( v );
		if ( suffix ) str << suffix;
		return str;
	}

	template <typename T> requires(IsSignedInteger<T>)
	ND_ String  ToStringSfx (const T value) __Th___
	{
		T		v		= Abs(value);
		char	suffix	= 0;

		if ( v < T(10'000) )			{}																else
		if ( v < T(10'000'000) )		{ v = (v + T(500))			/ T(1000);			suffix = 'K'; }	else
		if ( v < T(10'000'000'000) )	{ v = (v + T(500'000))		/ T(1000'000);		suffix = 'M'; }	else
										{ v = (v + T(500'000'000))	/ T(1000'000'000);	suffix = 'G'; }
		String	str = ToString( v * Sign(value) );
		if ( suffix ) str << suffix;
		return str;
	}

	template <typename T> requires(IsFloatPoint<T>)
	ND_ String  ToStringSfx (T value, const uint addFractPart = 0) __Th___
	{
		const T	v			= Abs(value);
		char	suffix		= 0;
		uint	fp			= 1;

		if_unlikely( not IsFinite( value ))
			return "NaN";

		if ( BitEqual( v, T(0) ))
			return "0.0";

		if ( v >= T(1) )
		{
			if ( v < T(1.0e+3) )	{										fp += uint(v < T(10));		}	else
			if ( v < T(1.0e+6) )	{ value *= T(1.0e-3);	suffix = 'K';	fp += uint(v < T(1.0e+4));	}	else	// kilo
			if ( v < T(1.0e+9) )	{ value *= T(1.0e-6);	suffix = 'M';	fp += uint(v < T(1.0e+7));	}	else	// mega
			if ( v < T(1.0e+12) )	{ value *= T(1.0e-9);	suffix = 'G';	fp += uint(v < T(1.0e+10));	}	else	// giga
									{ value *= T(1.0e-12);	suffix = 'T';	fp += uint(v < T(1.0e+13));	}			// terra
		}
		else
		{
			if ( v > T(1.0e-3) )	{ value *= T(1.0e+3);	suffix = 'm';	fp += uint(v < T(1.0e-2));	}	else	// milli
			if ( v > T(1.0e-6) )	{ value *= T(1.0e+6);	suffix = 'u';	fp += uint(v < T(1.0e-5));	}	else	// micro
			if ( v > T(1.0e-9) )	{ value *= T(1.0e+9);	suffix = 'n';	fp += uint(v < T(1.0e-8));	}	else	// nano
									{ value *= T(1.0e+12);	suffix = 'p';	fp += uint(v < T(1.0e-11));	}			// pico
		}
		String	str = ToString( value, fp + addFractPart );
		if ( suffix ) str << suffix;
		return str;
	}

/*
=================================================
	ToString (Simd)
=================================================
*/
namespace _hidden_
{
	template <typename T>
	Nd__In String  SimdIntToString (const T &v) __Th___
	{
		auto	arr = v.ToArray();
		String	str = "{";
		for (usize i = 0; i < arr.size(); ++i) {
			str << (i ? ", " : "") << ToString( arr[i] );
		}
		return (str << "}");
	}

	template <typename T>
	Nd__In String  SimdFloatToString (const T &v, uint fractParts) __Th___
	{
		auto	arr = v.ToArray();
		String	str = "{";
		for (usize i = 0; i < arr.size(); ++i) {
			str << (i ? ", " : "") << ToString( arr[i], fractParts );
		}
		return (str << "}");
	}
}

#ifdef AE_SIMD_SimdFloat4
	Nd__In String  ToString (const SimdFloat4 &v, uint fractParts = 2)	__Th___ { return Base::_hidden_::SimdFloatToString( v, fractParts ); }
#endif
#ifdef AE_SIMD_SimdDouble2
	Nd__In String  ToString (const SimdDouble2 &v, uint fractParts = 2)	__Th___ { return Base::_hidden_::SimdFloatToString( v, fractParts ); }
#endif
#ifdef AE_SIMD_SimdTInt128
	template <typename T> ND_ String  ToString (const SimdTInt128<T> &v)	__Th___ { return Base::_hidden_::SimdIntToString( v ); }
#endif
#ifdef AE_SIMD_SimdFloat8
	Nd__In String  ToString (const SimdFloat8 &v, uint fractParts = 2)	__Th___ { return Base::_hidden_::SimdFloatToString( v, fractParts ); }
#endif
#ifdef AE_SIMD_SimdDouble4
	Nd__In String  ToString (const SimdDouble4 &v, uint fractParts = 2)	__Th___ { return Base::_hidden_::SimdFloatToString( v, fractParts ); }
#endif
#ifdef AE_SIMD_SimdTInt256
	template <typename T> ND_ String  ToString (const SimdTInt256<T> &v)	__Th___ { return Base::_hidden_::SimdIntToString( v ); }
#endif
//-----------------------------------------------------------------------------



/*
=================================================
	AppendToString
----
	'11' -> '11 . . . '
=================================================
*/
	inline void  AppendToString (INOUT String &str, const usize count, const char value = ' ') __Th___
	{
		ASSERT( value != 0 );
		ASSERT( ssize(count) >= 0 );	// unsigned overflow

		usize	pos = str.size();
		str.resize( pos + count );  // throw

		for (; pos < str.size(); ++pos)
			str[pos] = value;
	}

	inline void  InsertToString (INOUT String &str, const usize count, const char value = ' ') __Th___
	{
		ASSERT( value != 0 );
		ASSERT( ssize(count) >= 0 );	// unsigned overflow

		str.reserve( str.size() + count );  // throw

		for (usize i = 0; i < count; ++i)
			str.insert( str.begin(), value );
	}

	inline void  AppendToString (INOUT String &str, const usize first, const usize count, const bool initial, const char value1 = '.', const char value2 = ' ') __Th___
	{
		ASSERT( value1 != 0 );
		ASSERT( value2 != 0 );
		ASSERT( ssize(count) >= 0 );	// unsigned overflow

		usize	pos = str.size();
		str.resize( pos + (first < count ? (count - first) : 0) );  // throw

		for (usize i = first; i < count; ++i, ++pos)
			str[pos] = ((initial and i&1) ? value1 : value2);
	}

/*
=================================================
	FormatAlignedI
----
	11 -> 0011
=================================================
*/
	template <uint Radix, typename T>
	ND_ String  FormatAlignedI (T value, const usize align, const char alignChar) __Th___
	{
		ASSERT( alignChar != 0 );

		String	tmp		= ToString<Radix>( value );
		String	str;	str.reserve( (align > tmp.size() ? 0 : tmp.size() - align) + tmp.size() );  // throw

		for (usize i = tmp.size(); i < align; ++i) {
			str << alignChar;
		}
		str << tmp;
		return str;
	}

/*
=================================================
	DivStringBySteps
----
	1111  -> 1'111
	11111 -> 11'111
	from back to front
=================================================
*/
	Nd__In String  DivStringBySteps (StringView inStr, const usize stepSize = 3, const char spaceChar = '\'') __Th___
	{
		String	str;	str.resize( inStr.size() + ((inStr.size()-1) / stepSize) );  // throw

		usize	i = (inStr.length() % stepSize);
		i = stepSize - i;
		i = i >= stepSize ? 0 : i;

		for (usize a = 0, b = 0; a < inStr.length(); ++b)
		{
			if_likely( i++ < stepSize )
				str[b] = inStr[a++];
			else{
				str[b] = spaceChar;
				i = 0;
			}
		}
		return str;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ToString_HMS (time units)
=================================================
*/
	Nd__In String  ToString_HMS (const double sec) __Th___
	{
		return	ToString( uint(Floor( sec / 3600.0 )) ) << ':' <<
				FormatAlignedI<10>( uint(Floor( sec / 60.0 )) % 60, 2, '0' ) << ':' <<
				FormatAlignedI<10>( uint(sec) % 60, 2, '0' );
	}
//-----------------------------------------------------------------------------


} // AE::Base
#endif // AE_ENABLE_LOGS
