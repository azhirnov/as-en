// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Math.h"
#include "base/Math/Vec.h"
#include "base/Math/Bytes.h"
#include "base/Math/Color.h"
#include "base/Math/BitMath.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Memory/MemUtils.h"
#include "base/Containers/NtStringView.h"
#include "base/Math/PhysicalQuantity.h"
#include "base/Utils/FileSystem.h"

#include <sstream>
#include <charconv>

namespace AE::Base
{
	using namespace std::string_literals;

/*
=================================================
	operator << (String &, String)
	operator << (String &, StringView)
	operator << (String &, CStyleString)
	operator << (String &, char)
=================================================
*/
	template <typename T, typename A1, typename A2>
	forceinline BasicString<T,A1>&&  operator << (BasicString<T,A1> &&lhs, const BasicString<T,A2> &rhs)
	{
		return RVRef( RVRef(lhs).append( rhs.data(), rhs.size() ));
	}

	template <typename T, typename A1, typename A2>
	forceinline BasicString<T,A1>&  operator << (BasicString<T,A1> &lhs, const BasicString<T,A2> &rhs)
	{
		return lhs.append( rhs.data(), rhs.size() );
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const BasicStringView<T> &rhs)
	{
		return RVRef( RVRef(lhs).append( rhs.data(), rhs.size() ));
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const BasicStringView<T> &rhs)
	{
		return lhs.append( rhs.data(), rhs.size() );
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const NtBasicStringView<T> &rhs)
	{
		return RVRef( RVRef(lhs).append( rhs.c_str(), rhs.size() ));
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const NtBasicStringView<T> &rhs)
	{
		return lhs.append( rhs.c_str(), rhs.size() );
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, T const * const rhs)
	{
		return RVRef( RVRef(lhs).append( rhs ));
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, T const * const rhs)
	{
		return lhs.append( rhs );
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const T rhs)
	{
		return RVRef( RVRef(lhs) += rhs );
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const T rhs)
	{
		return (lhs += rhs);
	}

/*
=================================================
	operator >> (String,       String &)
	operator >> (StringView,   String &)
	operator >> (CStyleString, String &)
	operator >> (char,         String &)
=================================================
*/
	template <typename T, typename A1, typename A2>
	forceinline BasicString<T,A1>&&  operator >> (const BasicString<T,A2> &lhs, BasicString<T,A1> &&rhs)
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return RVRef(rhs);
	}

	template <typename T, typename A1, typename A2>
	forceinline BasicString<T,A1>&  operator >> (const BasicString<T,A2> &lhs, BasicString<T,A1> &rhs)
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return rhs;
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator >> (const BasicStringView<T> &lhs, BasicString<T,A> &&rhs)
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator >> (const BasicStringView<T> &lhs, BasicString<T,A> &rhs)
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return rhs;
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator >> (const NtBasicStringView<T> &lhs, BasicString<T,A> &&rhs)
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator >> (const NtBasicStringView<T> &lhs, BasicString<T,A> &rhs)
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return rhs;
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator >> (T const * const lhs, BasicString<T,A> &&rhs)
	{
		rhs.insert( rhs.begin(), lhs );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator >> (T const * const lhs, BasicString<T,A> &rhs)
	{
		rhs.insert( 0u, lhs );
		return rhs;
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&&  operator >> (const T lhs, BasicString<T,A> &&rhs)
	{
		rhs.insert( 0u, lhs );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	forceinline BasicString<T,A>&  operator >> (const T lhs, BasicString<T,A> &rhs)
	{
		rhs.insert( 0u, lhs );
		return rhs;
	}
//-----------------------------------------------------------------------------

	

/*
=================================================
	IsNullTerminated
=================================================
*/
	template <typename StringType>
	ND_ forceinline bool  IsNullTerminated (const StringType &str)
	{
		return str.empty() or str.data()[ str.size() ] == 0;
	}

/*
=================================================
	IsUpperCase
=================================================
*/
	ND_ forceinline const char  IsUpperCase (const char c)
	{
		return (c >= 'A' and c <= 'Z');
	}

/*
=================================================
	IsLowerCase
=================================================
*/
	ND_ forceinline const char  IsLowerCase (const char c)
	{
		return (c >= 'a' and c <= 'z');
	}

/*
=================================================
	ToLowerCase
=================================================
*/
	ND_ forceinline const char  ToLowerCase (const char c)
	{
		return IsUpperCase( c ) ? (c - 'A' + 'a') : c;
	}

/*
=================================================
	ToUpperCase
=================================================
*/
	ND_ forceinline const char  ToUpperCase (const char c)
	{
		return IsLowerCase( c ) ? (c - 'a' + 'A') : c;
	}

/*
=================================================
	HasSubString
----
	returns 'true' if 'str' has substring 'substr',
	comparison is case sansitive.
=================================================
*/
	ND_ inline bool  HasSubString (StringView str, StringView substr)
	{
		return (str.find( substr ) != StringView::npos);
	}

/*
=================================================
	HasSubStringIC
----
	returns 'true' if 'str' has substring 'substr',
	comparison is case insansitive.
=================================================
*/
	ND_ inline bool  HasSubStringIC (StringView str, StringView substr)
	{
		if ( str.empty() or substr.empty() )
			return false;

		for (usize i = 0, j = 0; i < str.length(); ++i)
		{
			while ( i+j < str.length() and j < substr.length() and
					ToLowerCase( substr[j] ) == ToLowerCase( str[i+j] ))
			{
				++j;
				if_unlikely( j >= substr.length() )
					return true;
			}
			j = 0;
		}
		return false;
	}
	
/*
=================================================
	StartsWith
----
	returns 'true' if 'str' starts with substring 'substr',
	comparison is case sansitive.
=================================================
*/
	ND_ inline bool  StartsWith (StringView str, StringView substr)
	{
		if ( str.length() < substr.length() )
			return false;

		for (usize i = 0; i < substr.length(); ++i)
		{
			if_unlikely( str[i] != substr[i] )
				return false;
		}
		return true;
	}
	
/*
=================================================
	StartsWithIC
----
	returns 'true' if 'str' starts with substring 'substr',
	comparison is case insansitive.
=================================================
*/
	ND_ inline bool  StartsWithIC (StringView str, StringView substr)
	{
		if ( str.length() < substr.length() )
			return false;

		for (usize i = 0; i < substr.length(); ++i)
		{
			if_unlikely( ToLowerCase(str[i]) != ToLowerCase(substr[i]) )
				return false;
		}
		return true;
	}
	
/*
=================================================
	EndsWith
----
	returns 'true' if 'str' ends with substring 'substr',
	comparison is case sansitive.
=================================================
*/
	ND_ inline bool  EndsWith (StringView str, StringView substr)
	{
		if ( str.length() < substr.length() )
			return false;

		for (usize i = 1; i <= substr.length(); ++i)
		{
			if_unlikely( str[str.length() - i] != substr[substr.length() - i] )
				return false;
		}
		return true;
	}
	
/*
=================================================
	EndsWithIC
----
	returns 'true' if 'str' ends with substring 'substr',
	comparison is case insansitive.
=================================================
*/
	ND_ inline bool  EndsWithIC (StringView str, StringView substr)
	{
		if ( str.length() < substr.length() )
			return false;

		for (usize i = 1; i <= substr.length(); ++i)
		{
			if_unlikely( ToLowerCase(str[str.length() - i]) != ToLowerCase(substr[substr.length() - i]) )
				return false;
		}
		return true;
	}
	
/*
=================================================
	FindAndReplace
=================================================
*/
	inline uint  FindAndReplace (INOUT String& str, StringView oldStr, StringView newStr)
	{
		String::size_type	pos		= 0;
		uint				count	= 0;

		while ( (pos = StringView{str}.find( oldStr, pos )) != StringView::npos )
		{
			str.replace( pos, oldStr.length(), newStr.data() );
			pos += newStr.length();
			++count;
		}
		return count;
	}

/*
=================================================
	ToAnsiString
=================================================
*/
	template <typename R, typename T>
	inline BasicString<R>  ToAnsiString (BasicStringView<T> str, const R defaultChar = R('?'))
	{
		BasicString<R>	result;
		result.resize( str.size() );

		for (usize i = 0; i < str.size(); ++i)
		{
			result[i] = str[i] <= 0x7F ? R(str[i] & 0x7F) : defaultChar;
		}
		return result;
	}

	template <typename R, typename T>
	inline BasicString<R>  ToAnsiString (const T* str, const R defaultChar = R('?'))
	{
		return ToAnsiString<R>( BasicStringView<T>{ str }, defaultChar );
	}
	
	template <typename R, typename T, typename A>
	inline BasicString<R>  ToAnsiString (const BasicString<T,A> &str, const R defaultChar = R('?'))
	{
		return ToAnsiString<R>( BasicStringView<T>{ str }, defaultChar );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ToString
=================================================
*/
	template <typename T>
	ND_ forceinline EnableIf<not IsEnum<T>, String>  ToString (const T &value)
	{
		return std::to_string( value );
	}
	/*
	template <typename E>
	ND_ forceinline EnableIf<IsEnum<E>, String>  ToString (const E &value)
	{
		using T = Conditional< (sizeof(E) > sizeof(uint)), uint, ulong >;

		return std::to_string( T(value) );
	}*/

	ND_ forceinline String  ToString (const bool &value)
	{
		return value ? "true" : "false";
	}
	
/*
=================================================
	ToString
=================================================
*/
	template <int Radix, typename T>
	ND_ forceinline EnableIf< IsEnum<T> or IsInteger<T>, String>  ToString (const T &value)
	{
		if constexpr ( Radix == 10 )
		{
			return std::to_string( value );
		}
		else
		if constexpr ( Radix == 16 and sizeof(T) > sizeof(uint) )
		{
			std::stringstream	str;
			str << std::hex << ulong{BitCast<ToUnsignedInteger<T>>(value)};
			//str << std::resetiosflags( std::ios_base::dec ) << std::setiosflags( std::ios_base::hex | std::ios_base::uppercase )
			//	<< ulong{BitCast<ToUnsignedInteger<T>>(value)};
			return str.str();
		}
		else
		if constexpr ( Radix == 16 )
		{
			std::stringstream	str;
			//str << std::resetiosflags( std::ios_base::dec ) << std::setiosflags( std::ios_base::hex | std::ios_base::uppercase )
			str << std::hex << uint{BitCast<ToUnsignedInteger<T>>(value)};
			return str.str();
		}
	}

/*
=================================================
	ToString (double)
=================================================
*/
	ND_ inline String  ToString (const double &value, uint fractParts, Bool exponent = False{})
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

/*
=================================================
	ToString (Vec)
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ inline String  ToString (const TVec<T,I,Q> &value)
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
	
/*
=================================================
	ToString (RGBAColor)
=================================================
*/
	template <typename T>
	ND_ inline String  ToString (const RGBAColor<T> &value)
	{
		String	str = "( "s;

		for (uint i = 0; i < 4; ++i)
		{
			if_likely( i > 0 )
				str << ", ";
			str << ToString( value[i] );
		}
		str << " )";
		return str;
	}

/*
=================================================
	ToString (Bytes)
=================================================
*/
	template <typename T>
	ND_ inline String  ToString (const TBytes<T> &value)
	{
		const T	kb	= SafeLeftBitShift( T{1}, 12 );
		const T mb	= SafeLeftBitShift( T{1}, 22 );
		const T	gb	= SafeLeftBitShift( T{1}, 32 );
		const T	tb	= SafeLeftBitShift( T{1}, 42 );
		const T	val	= T(value);

		String	str;

		if ( val < kb )	str << ToString( val ) << " b";								else
		if ( val < mb )	str << ToString( SafeRightBitShift( val, 10 )) << " Kb";	else
		if ( val < gb )	str << ToString( SafeRightBitShift( val, 20 )) << " Mb";	else
		if ( val < tb )	str << ToString( SafeRightBitShift( val, 30 )) << " Gb";	else
						str << ToString( SafeRightBitShift( val, 40 )) << " Tb";
		
		return str;
	}

/*
=================================================
	ToString (chrono::duration)
=================================================
*/
	template <typename T, typename Duration>
	ND_ inline String  ToString (const std::chrono::duration<T,Duration> &value, uint precission = 2)
	{
		using SecondsD_t  = std::chrono::duration<double>;
		using MicroSecD_t = std::chrono::duration<double, std::micro>;
		using NanoSecD_t  = std::chrono::duration<double, std::nano>;

		const auto	time = std::chrono::duration_cast<SecondsD_t>( value ).count();
		String		str;

		if ( time > 59.0 * 60.0 )
			str << ToString( time * (1.0/3600.0), precission ) << " h";
		else
		if ( time > 59.0 )
			str << ToString( time * (1.0/60.0), precission ) << " m";
		else
		if ( time > 1.0e-1 )
			str << ToString( time, precission ) << " s";
		else
		if ( time > 1.0e-4 )
			str << ToString( time * 1.0e+3, precission ) << " ms";
		else
		if ( time > 1.0e-7 )
			str << ToString( std::chrono::duration_cast<MicroSecD_t>( value ).count(), precission ) << " us";
		else
			str << ToString( std::chrono::duration_cast<NanoSecD_t>( value ).count(), precission ) << " ns";

		return str;
	}

/*
=================================================
	ToString (Path)
=================================================
*/
	ND_ inline String  ToString (const Path &path)
	{
		return ToAnsiString<char>( path.native() );	// TODO: utf8 to ansi?
	}

	ND_ inline String  ToString (const WString &str)
	{
		return ToAnsiString<char>( str );
	}
	
/*
=================================================
	ToString (Fractional)
=================================================
*/
	template <typename T>
	ND_ inline String  ToString (Fractional<T> value)
	{
		String	str = ToString( value.numerator );

		if ( value.numerator != 0 and value.denominator > 1 )
			str << '/' << ToString( value.denominator );

		return str;
	}

/*
=================================================
	ToString (PhysicalDimension)
=================================================
*/
	template <int SecondsNum,	int SecondsDenom,
			  int KilogramsNum,	int KilogramsDenom,
			  int MetersNum,	int MetersDenom,
			  int AmperasNum,	int AmperasDenom,
			  int KelvinsNum,	int KelvinsDenom,
			  int MolesNum,		int MolesDenom,
			  int CandelasNum,	int CandelasDenom,
			  int CurrencyNum,	int CurrencyDenom
			>
	ND_ inline String  ToString (PhysicalDimension< SecondsNum,		SecondsDenom,
													KilogramsNum,	KilogramsDenom,
													MetersNum,		MetersDenom,
													AmperasNum,		AmperasDenom,
													KelvinsNum,		KelvinsDenom,
													MolesNum,		MolesDenom,
													CandelasNum,	CandelasDenom,
													CurrencyNum,	CurrencyDenom > value)
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
			if ( not (frac.IsInteger() and frac.numerator == 1) )
				str << '^' << ToString( frac );

			++cnt;
		}};

		Append( Dim::seconds,	"s" );
		Append( Dim::kilograms,	"kg" );
		Append( Dim::meters,	"m" );
		Append( Dim::amperes,	"A" );
		Append( Dim::kelvins,	"K" );
		Append( Dim::moles,		"mol" );
		Append( Dim::candelas,	"cd" );
		Append( Dim::currency,	"$" );

		if ( dim_cnt[1] > 1 )
			str_nom = '(' + str_nom + ')';
		
		if ( dim_cnt[0] > 1 )
			str_den = '(' + str_den + ')';
		
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
	template <typename V, typename D, typename S>
	ND_ inline String  ToString (const PhysicalQuantity<V,D,S> &value)
	{
		return ToString( value.GetScaled(), 2, True{"exponent"} ) << '[' << ToString( D{} ) << ']';
	}

	template <typename V, typename D, typename S>
	ND_ inline String  ToDebugString (const PhysicalQuantity<V,D,S> &value)
	{
		return ToString( value.GetNonScaled(), 2, True{"exponent"} ) << '*' << ToString( S::Value, 2, True{"exponent"} ) << '[' << ToString( D{} ) << ']';
	}
//-----------------------------------------------------------------------------
	


/*
=================================================
	AppendToString
=================================================
*/
	inline void  AppendToString (INOUT String &str, usize count, char value = ' ')
	{
		str.reserve( str.size() + count );

		for (usize i = 0; i < count; ++i)
			str << value;
	}
	
	inline void  InsertToString (INOUT String &str, usize count, char value = ' ')
	{
		str.reserve( str.size() + count );

		for (usize i = 0; i < count; ++i)
			str.insert( str.begin(), value );
	}

	inline void  AppendToString (INOUT String &str, const usize first, const usize count, const bool initial, const char value1 = '.', const char value2 = ' ')
	{
		str.reserve( str.size() + (first < count ? (count - first) : 0) );

		for (usize i = first; i < count; ++i)
			str << ((initial and i&1) ? value1 : value2);
	}

/*
=================================================
	FormatAlignedI
=================================================
*/
	template <uint Radix, typename T>
	ND_ inline String  FormatAlignedI (T value, usize align, char alignChar)
	{
		String	tmp = ToString<Radix>( value );
		String	str;

		for (usize i = tmp.size(); i < align; ++i) {
			str << alignChar;
		}
		str << tmp;
		return str;
	}
//-----------------------------------------------------------------------------

	

/*
=================================================
	StringTo***
=================================================
*/
	ND_ inline int  StringToInt (StringView str, int base = 10)
	{
		ASSERT( base == 10 or base == 16 );
		int		val = 0;
		std::from_chars( str.data(), str.data() + str.size(), OUT val, base );
		return val;
	}
	
	ND_ inline uint  StringToUInt (StringView str, int base = 10)
	{
		ASSERT( base == 10 or base == 16 );
		uint	val = 0;
		std::from_chars( str.data(), str.data() + str.size(), OUT val, base );
		return val;
	}
	
	ND_ inline ulong  StringToUInt64 (StringView str, int base = 10)
	{
		ASSERT( base == 10 or base == 16 );
		ulong	val = 0;
		std::from_chars( str.data(), str.data() + str.size(), OUT val, base );
		return val;
	}
	
#ifdef AE_COMPILER_MSVC
	ND_ inline float  StringToFloat (StringView str)
	{
		float	val = 0.0f;
		std::from_chars( str.data(), str.data() + str.size(), OUT val, std::chars_format::general );
		return val;
	}
	
	ND_ inline double  StringToDouble (StringView str)
	{
		double	val = 0.0;
		std::from_chars( str.data(), str.data() + str.size(), OUT val, std::chars_format::general );
		return val;
	}
#endif

}	// AE::Base
