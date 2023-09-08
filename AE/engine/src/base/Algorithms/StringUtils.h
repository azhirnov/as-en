// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Math.h"
#include "base/Math/Vec.h"
#include "base/Math/Rectangle.h"
#include "base/Math/VecSwizzle.h"
#include "base/Math/Bytes.h"
#include "base/Math/Color.h"
#include "base/Math/BitMath.h"
#include "base/Math/POTValue.h"
#include "base/Utils/Version.h"

#include "base/Algorithms/ArrayUtils.h"
#include "base/Memory/MemUtils.h"
#include "base/Containers/NtStringView.h"
#include "base/Math/PhysicalQuantity.h"
#include "base/Utils/FileSystem.h"
#include "base/Algorithms/Utf8.h"

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
    forceinline BasicString<T,A1>&&  operator << (BasicString<T,A1> &&lhs, const BasicString<T,A2> &rhs) __Th___
    {
        return RVRef( RVRef(lhs).append( rhs.data(), rhs.size() ));
    }

    template <typename T, typename A1, typename A2>
    forceinline BasicString<T,A1>&  operator << (BasicString<T,A1> &lhs, const BasicString<T,A2> &rhs) __Th___
    {
        return lhs.append( rhs.data(), rhs.size() );
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const BasicStringView<T> &rhs) __Th___
    {
        return RVRef( RVRef(lhs).append( rhs.data(), rhs.size() ));
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const BasicStringView<T> &rhs) __Th___
    {
        return lhs.append( rhs.data(), rhs.size() );
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const NtBasicStringView<T> &rhs) __Th___
    {
        return RVRef( RVRef(lhs).append( rhs.c_str(), rhs.size() ));
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const NtBasicStringView<T> &rhs) __Th___
    {
        return lhs.append( rhs.c_str(), rhs.size() );
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, T const * const rhs) __Th___
    {
        return RVRef( RVRef(lhs).append( rhs ));
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, T const * const rhs) __Th___
    {
        return lhs.append( rhs );
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const T rhs) __Th___
    {
        return RVRef( RVRef(lhs) += rhs );
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const T rhs) __Th___
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
    forceinline BasicString<T,A1>&&  operator >> (const BasicString<T,A2> &lhs, BasicString<T,A1> &&rhs) __Th___
    {
        rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
        return RVRef(rhs);
    }

    template <typename T, typename A1, typename A2>
    forceinline BasicString<T,A1>&  operator >> (const BasicString<T,A2> &lhs, BasicString<T,A1> &rhs) __Th___
    {
        rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
        return rhs;
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator >> (const BasicStringView<T> &lhs, BasicString<T,A> &&rhs) __Th___
    {
        rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
        return RVRef(rhs);
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator >> (const BasicStringView<T> &lhs, BasicString<T,A> &rhs) __Th___
    {
        rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
        return rhs;
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator >> (const NtBasicStringView<T> &lhs, BasicString<T,A> &&rhs) __Th___
    {
        rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
        return RVRef(rhs);
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator >> (const NtBasicStringView<T> &lhs, BasicString<T,A> &rhs) __Th___
    {
        rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
        return rhs;
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator >> (T const * const lhs, BasicString<T,A> &&rhs) __Th___
    {
        rhs.insert( rhs.begin(), lhs );
        return RVRef(rhs);
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator >> (T const * const lhs, BasicString<T,A> &rhs) __Th___
    {
        rhs.insert( 0u, lhs );
        return rhs;
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&&  operator >> (const T lhs, BasicString<T,A> &&rhs) __Th___
    {
        rhs.insert( 0u, lhs );
        return RVRef(rhs);
    }

    template <typename T, typename A>
    forceinline BasicString<T,A>&  operator >> (const T lhs, BasicString<T,A> &rhs) __Th___
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
    ND_ forceinline bool  IsNullTerminated (const StringType &str) __NE___
    {
        return str.empty() or str.data()[ str.size() ] == 0;
    }

/*
=================================================
    IsUpperCase
=================================================
*/
    ND_ forceinline bool  IsUpperCase (const char c) __NE___
    {
        return (c >= 'A' and c <= 'Z');
    }

/*
=================================================
    IsLowerCase
=================================================
*/
    ND_ forceinline bool  IsLowerCase (const char c) __NE___
    {
        return (c >= 'a' and c <= 'z');
    }

/*
=================================================
    ToLowerCase
=================================================
*/
    ND_ forceinline const char  ToLowerCase (const char c) __NE___
    {
        return IsUpperCase( c ) ? (c - 'A' + 'a') : c;
    }

/*
=================================================
    ToUpperCase
=================================================
*/
    ND_ forceinline const char  ToUpperCase (const char c) __NE___
    {
        return IsLowerCase( c ) ? (c - 'a' + 'A') : c;
    }

/*
=================================================
    EqualIC
----
    returns 'true' if strings are equal,
    comparison is case insensitive.
=================================================
*/
    ND_ inline bool  EqualIC (StringView lhs, StringView rhs) __NE___
    {
        if ( lhs.size() != rhs.size() )
            return false;

        for (usize i = 0; i < lhs.length(); ++i)
        {
            if_unlikely( ToLowerCase( lhs[i] ) != ToLowerCase( rhs[i] ))
                return false;
        }
        return true;
    }

/*
=================================================
    HasSubString
----
    returns 'true' if 'str' has substring 'substr',
    comparison is case sensitive.
=================================================
*/
    ND_ inline bool  HasSubString (StringView str, StringView substr) __NE___
    {
        return (str.find( substr ) != StringView::npos);
    }

/*
=================================================
    HasSubStringIC
----
    returns 'true' if 'str' has substring 'substr',
    comparison is case insensitive.
=================================================
*/
    ND_ inline bool  HasSubStringIC (StringView str, StringView substr) __NE___
    {
        if ( str.empty() or substr.empty() )
            return false;

        for (usize i = 0, j = 0; i < str.length(); ++i)
        {
            while ( (i+j < str.length()) and
                    (j < substr.length()) and
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
    comparison is case sensitive.
=================================================
*/
    ND_ inline bool  StartsWith (StringView str, StringView substr) __NE___
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
    comparison is case insensitive.
=================================================
*/
    ND_ inline bool  StartsWithIC (StringView str, StringView substr) __NE___
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
    comparison is case sensitive.
=================================================
*/
    ND_ inline bool  EndsWith (StringView str, StringView substr) __NE___
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
    comparison is case insensitive.
=================================================
*/
    ND_ inline bool  EndsWithIC (StringView str, StringView substr) __NE___
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
    inline uint  FindAndReplace (INOUT String& str, char oldSymb, char newSymb) __Th___
    {
        String::size_type   pos     = 0;
        uint                count   = 0;

        while ( (pos = StringView{str}.find( oldSymb, pos )) != StringView::npos )
        {
            str[pos] = newSymb;
            ++pos;
            ++count;
        }
        return count;
    }

    inline uint  FindAndReplace (INOUT String& str, StringView oldStr, StringView newStr) __Th___
    {
        String::size_type   pos     = 0;
        uint                count   = 0;

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
    WCharToAnsi
=================================================
*/
    template <typename T>
    constexpr bool  WCharToAnsi (OUT CharAnsi* dst, const T* src, usize len, const CharAnsi defaultChar = CharAnsi('?')) __NE___
    {
        ASSERT( dst != null );
        ASSERT( src != null );

        bool    res = true;
        for (usize i = 0; i < len; ++i)
        {
            res     &= (src[i] <= 0x7F);
            dst[i]   = (src[i] <= 0x7F ? CharAnsi(src[i]) : defaultChar);
        }
        dst[len] = CharAnsi{0};
        return res;
    }

/*
=================================================
    Utf8ToAnsi
=================================================
*/
#ifdef AE_ENABLE_UTF8PROC
    template <typename T>
    constexpr bool  Utf8ToAnsi (OUT CharAnsi* dst, const CharUtf8* src, INOUT usize &len, const CharAnsi defaultChar = CharAnsi('?')) __NE___
    {
        ASSERT( dst != null );
        ASSERT( src != null );

        bool    res = true;
        usize   i   = 0;
        for (usize pos = 0; (pos < len) & (i < len); ++i)
        {
            CharUtf32   utf = Utf8Decode( src, len, INOUT pos );
            res     &= (utf <= 0x7F);
            dst[i]   = (utf <= 0x7F ? CharAnsi(utf) : defaultChar);
        }
        dst[i]  = CharAnsi{0};
        len     = i;
        return res;
    }
#endif

/*
=================================================
    ToAnsiString
=================================================
*/
    template <typename R, typename T>
    ND_ BasicString<R>  ToAnsiString (BasicStringView<T> str, const R defaultChar = R('?')) __Th___
    {
        if constexpr( IsSameTypes< T, CharAnsi >)
            return BasicString<R>{str};
        else
        if constexpr( IsSameTypes< T, wchar_t > or IsSameTypes< T, CharUtf32 >)
        {
            BasicString<R>  result;
            result.resize( str.size() );    // throw
            WCharToAnsi( OUT result.data(), str.data(), str.length(), defaultChar );
            return result;
        }
      #ifdef AE_ENABLE_UTF8PROC
        else
        if constexpr( IsSameTypes< T, CharUtf8 >)
        {
            usize           len     = str.length();
            BasicString<R>  result;
            result.resize( str.size() );    // throw

            Utf8ToAnsi( OUT result.data(), str.data(), INOUT len, defaultChar );
            result.resize( len );

            return result;
        }
      #endif
    }

    template <typename R, typename T>
    ND_ BasicString<R>  ToAnsiString (const T* str, const R defaultChar = R('?')) __Th___
    {
        return ToAnsiString<R>( BasicStringView<T>{ str }, defaultChar );
    }

    template <typename R, typename T, typename A>
    ND_ BasicString<R>  ToAnsiString (const BasicString<T,A> &str, const R defaultChar = R('?')) __Th___
    {
        return ToAnsiString<R>( BasicStringView<T>{ str }, defaultChar );
    }

/*
=================================================
    IsAnsiString
=================================================
*/
    template <typename T>
    ND_ constexpr bool  IsAnsiString (const T *ptr, usize length) __NE___
    {
        for (usize i = 0; i < length; ++i)
        {
            if_unlikely( ToNearUInt(ptr[i]) > 0x7F )
                return false;
        }
        return true;
    }

    template <typename T>
    ND_ constexpr bool  IsAnsiString (BasicStringView<T> str) __NE___
    {
        return IsAnsiString( str.data(), str.length() );
    }

    template <typename T, typename A>
    ND_ constexpr bool  IsAnsiString (const BasicString<T,A> &str) __NE___
    {
        return IsAnsiString( str.c_str(), str.length() );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ToString
=================================================
*/
    ND_ forceinline String  ToString (String value) __Th___
    {
        return value;
    }

    ND_ forceinline String  ToString (const char value[]) __Th___
    {
        return String{value};
    }

    template <typename T>
    ND_ EnableIf<not IsEnum<T>, String>  ToString (const T &value) __Th___
    {
        return std::to_string( value );
    }
    /*
    template <typename E>
    ND_ EnableIf<IsEnum<E>, String>  ToString (const E &value) __Th___
    {
        using T = Conditional< (sizeof(E) > sizeof(uint)), uint, ulong >;

        return std::to_string( T(value) );
    }*/

    ND_ forceinline String  ToString (bool value) __Th___
    {
        return value ? "true" : "false";
    }

/*
=================================================
    ToString
=================================================
*/
    template <int Radix, typename T>
    ND_ EnableIf< IsEnum<T> or IsInteger<T>, String>  ToString (const T &value) __Th___
    {
        if constexpr( Radix == 10 )
        {
            return std::to_string( value );
        }
        else
        if constexpr( Radix == 16 and sizeof(T) > sizeof(uint) )
        {
            std::stringstream   str;
            str << std::hex << ulong{BitCast<ToUnsignedInteger<T>>(value)};
            //str << std::resetiosflags( std::ios_base::dec ) << std::setiosflags( std::ios_base::hex | std::ios_base::uppercase )
            //  << ulong{BitCast<ToUnsignedInteger<T>>(value)};
            return str.str();
        }
        else
        if constexpr( Radix == 16 )
        {
            std::stringstream   str;
            //str << std::resetiosflags( std::ios_base::dec ) << std::setiosflags( std::ios_base::hex | std::ios_base::uppercase )
            str << std::hex << uint{BitCast<ToUnsignedInteger<T>>(value)};
            return str.str();
        }
    }

/*
=================================================
    ToString (float / double)
=================================================
*/
    ND_ inline String  ToString (double value, uint fractParts, Bool exponent = False{}) __Th___
    {
        ASSERT( (fractParts > 0) and (fractParts < 100) );
        fractParts = Clamp( fractParts, 1u, 99u );

        const char  fmt[8]  = {'%', '0', '.', char('0' + fractParts / 10), char('0' + fractParts % 10), (exponent ? 'e' : 'f'), '\0' };
        char        buf[32] = {};

        const int   len = std::snprintf( buf, CountOf(buf), fmt, value );
        ASSERT( len > 0 );
        Unused( len );
        return buf;
    }

    ND_ inline String  ToString (float value, uint fractParts, Bool exponent = False{}) __Th___
    {
        return ToString( double(value), fractParts, exponent );
    }

/*
=================================================
    ToString (Vec)
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ String  ToString (const TVec<T,I,Q> &value) __Th___
    {
        String  str = "( ";

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
    ToString (VecSwizzle)
=================================================
*/
    ND_ inline String  ToString (const VecSwizzle &value) __Th___
    {
        auto        sw          = value.ToVec();
        const char  symbols []  = "0XYZW0+-";
        char        str [5]     = { symbols[sw.x], symbols[sw.y], symbols[sw.z], symbols[sw.w] };
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
        String  str = "( "s;
        for (uint i = 0; i < 4; ++i)
        {
            if_likely( i > 0 ) str << ", ";
            str << ToString( value[i] );
        }
        str << " )";
        return str;
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, String>  ToString (const RGBAColor<T> &value, uint fractParts) __Th___
    {
        String  str = "( "s;
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
    ND_ String  ToString (const TBytes<T> &value) __Th___
    {
        const T kb  = SafeLeftBitShift( T{1}, 12 );
        const T mb  = SafeLeftBitShift( T{1}, 22 );
        const T gb  = SafeLeftBitShift( T{1}, 32 );
        const T tb  = SafeLeftBitShift( T{1}, 42 );
        const T val = T(value);

        String  str;

        if ( val < kb ) str << ToString( val ) << " b";                             else
        if ( val < mb ) str << ToString( SafeRightBitShift( val, 10 )) << " Kb";    else
        if ( val < gb ) str << ToString( SafeRightBitShift( val, 20 )) << " Mb";    else
        if ( val < tb ) str << ToString( SafeRightBitShift( val, 30 )) << " Gb";    else
                        str << ToString( SafeRightBitShift( val, 40 )) << " Tb";

        return str;
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
    ND_ String  ToString (const std::chrono::duration<T,Duration> &value, uint precission = 2) __Th___
    {
        using SecondsD_t  = std::chrono::duration<double>;
        using MicroSecD_t = std::chrono::duration<double, std::micro>;

        const double    time     = TimeCast<SecondsD_t>( value ).count();
        const double    abs_time = Abs( time );
        String          str;

        if ( abs_time > 59.0 * 60.0 )
            str << ToString( time * (1.0/3600.0), precission ) << " h";
        else
        if ( abs_time > 59.0 )
            str << ToString( time * (1.0/60.0), precission ) << " m";
        else
        if ( abs_time > 1.0e-1 )
            str << ToString( time, precission ) << " s";
        else
        if ( abs_time > 1.0e-4 )
            str << ToString( time * 1.0e+3, precission ) << " ms";
        else
        if ( abs_time > 1.0e-7 )
            str << ToString( TimeCast<MicroSecD_t>( value ).count(), precission ) << " us";
        else
            str << ToString( TimeCast<nanosecondsd>( value ).count(), precission ) << " ns";

        return str;
    }

/*
=================================================
    ToString (Path)
=================================================
*/
    ND_ inline String  ToString (const Path &path) __Th___
    {
        return ToAnsiString<char>( path.native() );
    }

    ND_ inline String  ToString (const WString &str) __Th___
    {
        return ToAnsiString<char>( str );
    }

    ND_ inline String  ToString (const WStringView &str) __Th___
    {
        return ToAnsiString<char>( str );
    }

    ND_ inline String  ToString (const wchar_t *str) __Th___
    {
        return ToAnsiString<char>( WStringView{str} );
    }

/*
=================================================
    ToString (Fractional)
=================================================
*/
    template <typename T>
    ND_ String  ToString (Fractional<T> value) __Th___
    {
        String  str = ToString( value.num );

        if ( value.num != 0 and value.den > 1 )
            str << '/' << ToString( value.den );

        return str;
    }

/*
=================================================
    ToString (TPhysicalDimension)
=================================================
*/
    template <int SecondsNum,   int SecondsDenom,
              int KilogramsNum, int KilogramsDenom,
              int MetersNum,    int MetersDenom,
              int AmperasNum,   int AmperasDenom,
              int KelvinsNum,   int KelvinsDenom,
              int MolesNum,     int MolesDenom,
              int CandelasNum,  int CandelasDenom,
              int CurrencyNum,  int CurrencyDenom,
              int BitsNum,      int BitsDenom
            >
    ND_ String  ToString (TPhysicalDimension< SecondsNum,   SecondsDenom,
                                              KilogramsNum, KilogramsDenom,
                                              MetersNum,    MetersDenom,
                                              AmperasNum,   AmperasDenom,
                                              KelvinsNum,   KelvinsDenom,
                                              MolesNum,     MolesDenom,
                                              CandelasNum,  CandelasDenom,
                                              CurrencyNum,  CurrencyDenom,
                                              BitsNum,      BitsDenom >  value) __Th___
    {
        using Dim = decltype(value);

        String  str_nom;
        String  str_den;
        uint    dim_cnt[2] = {};

        const auto  Append = [&] (FractionalI frac, StringView name)
        {{
            if ( frac.IsZero() )
                return;

            String& str = frac.IsPositive() ? str_nom : str_den;
            uint&   cnt = dim_cnt[ frac.IsPositive() ];

            if ( not str.empty() )
                str << " * ";

            str << name;
            if ( not (frac.IsInteger() and frac.num == 1) )
                str << '^' << ToString( frac );

            ++cnt;
        }};

        Append( Dim::seconds,   "s"     );
        Append( Dim::kilograms, "kg"    );
        Append( Dim::meters,    "m"     );
        Append( Dim::amperes,   "A"     );
        Append( Dim::kelvins,   "K"     );
        Append( Dim::moles,     "mol"   );
        Append( Dim::candelas,  "cd"    );
        Append( Dim::currency,  "$"     );
        Append( Dim::bits,      "bits"  );

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
    ND_ EnableIf<IsInteger<V>, String>  ToString (const PhysicalQuantity<V,D,S> &value) __Th___
    {
        return ToString( value.GetScaled() ) << '[' << ToString( D{} ) << ']';
    }

    template <typename V, typename D, typename S>
    ND_ EnableIf<IsFloatPoint<V>, String>  ToString (const PhysicalQuantity<V,D,S> &value, uint fractParts = 2, Bool exponent = True{}) __Th___
    {
        return ToString( value.GetScaled(), fractParts, exponent ) << '[' << ToString( D{} ) << ']';
    }

/*
=================================================
    ToString (PhysicalQuantity)
=================================================
*/
    template <typename V, typename D, typename S>
    ND_ EnableIf<IsInteger<V>, String>  ToDebugString (const PhysicalQuantity<V,D,S> &value) __Th___
    {
        return ToString( value.GetNonScaled() ) << '*' << ToString( S::Value ) << '[' << ToString( D{} ) << ']';
    }

    template <typename V, typename D, typename S>
    ND_ EnableIf<IsFloatPoint<V>, String>  ToDebugString (const PhysicalQuantity<V,D,S> &value, uint fractParts = 2, Bool exponent = True{}) __Th___
    {
        return ToString( value.GetNonScaled(), fractParts, exponent ) << '*' << ToString( S::Value, fractParts, exponent ) << '[' << ToString( D{} ) << ']';
    }

/*
=================================================
    ToString (Version)
=================================================
*/
    template <ulong UID>
    ND_ String  ToString (TVersion2<UID> value) __Th___
    {
        return ToString( value.major ) << '.' << ToString( value.minor );
    }

    template <ulong UID>
    ND_ String  ToString (TVersion3<UID> value) __Th___
    {
        return ToString( value.major ) << '.' << ToString( value.minor ) << '.' << ToString( value.patch );
    }
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
        str.reserve( str.size() + count );

        for (usize i = 0; i < count; ++i)
            str << value;
    }

    inline void  InsertToString (INOUT String &str, const usize count, const char value = ' ') __Th___
    {
        ASSERT( value != 0 );
        str.reserve( str.size() + count );

        for (usize i = 0; i < count; ++i)
            str.insert( str.begin(), value );
    }

    inline void  AppendToString (INOUT String &str, const usize first, const usize count, const bool initial, const char value1 = '.', const char value2 = ' ') __Th___
    {
        ASSERT( value1 != 0 );
        ASSERT( value2 != 0 );

        str.reserve( str.size() + (first < count ? (count - first) : 0) );

        for (usize i = first; i < count; ++i)
            str << ((initial and i&1) ? value1 : value2);
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

        String  tmp     = ToString<Radix>( value );
        String  str;    str.reserve( (align > tmp.size() ? 0 : tmp.size() - align) + tmp.size() );

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
    1111 -> 11'11
=================================================
*/
    ND_ inline String  DivStringBySteps (StringView inStr, const usize stepSize = 3, const char spaceChar = '\'') __Th___
    {
        String  str;    str.resize( inStr.size() + ((inStr.size()-1) / stepSize) );

        usize   i = (inStr.length() % stepSize);
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
    ND_ inline String  ToString_HMS (const double sec) __Th___
    {
        return  ToString( uint(Floor( sec / 3600.0 )) ) << ':' <<
                FormatAlignedI<10>( uint(Floor( sec / 60.0 )) % 60, 2, '0' ) << ':' <<
                FormatAlignedI<10>( uint(sec) % 60, 2, '0' );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    StringTo***
=================================================
*/
    template <typename T>
    ND_ usize  StringToValue (StringView str, OUT T &result) __NE___
    {
        if constexpr( IsInteger<T> )
        {
            ASSERT( str.empty() or (str[0] != '-' and str[0] != '+') );

            usize   off = 0;

            if ( str.size() > 2 and ((str[0] == '0') & (ToLowerCase(str[1]) == 'x')) )
                off = 2;
            else
            if ( str.size() > 1 and str[0] == '#' )
                off = 1;

            auto    err = std::from_chars( str.data() + off, str.data() + str.size(), OUT result, off ? 16 : 10 );
            ASSERT( err.ec == std::errc() );

            return usize(err.ptr) - usize(str.data() + off);
        }

      #ifdef AE_COMPILER_MSVC
        if constexpr( IsFloatPoint<T> )
        {
            auto    err = std::from_chars( str.data(), str.data() + str.size(), OUT result, std::chars_format::general );
            ASSERT( err.ec == std::errc() );

            return usize(err.ptr) - usize(str.data());
        }
      #endif
    }

    ND_ inline int  StringToInt (StringView str, int base = 10) __NE___
    {
        ASSERT( base == 10 or base == 16 );
        ASSERT( not StartsWith( str, "0x" ));

        int     val = 0;
        auto    err = std::from_chars( str.data(), str.data() + str.size(), OUT val, base );

        ASSERT( err.ec == std::errc() );
        return val;
    }

    ND_ inline uint  StringToUInt (StringView str, int base = 10) __NE___
    {
        ASSERT( base == 10 or base == 16 );
        ASSERT( not StartsWith( str, "0x" ));

        uint    val = 0;
        auto    err = std::from_chars( str.data(), str.data() + str.size(), OUT val, base );

        ASSERT( err.ec == std::errc() );
        return val;
    }

    ND_ inline ulong  StringToUInt64 (StringView str, int base = 10) __NE___
    {
        ASSERT( base == 10 or base == 16 );
        ASSERT( not StartsWith( str, "0x" ));

        ulong   val = 0;
        auto    err = std::from_chars( str.data(), str.data() + str.size(), OUT val, base );

        ASSERT( err.ec == std::errc() );
        return val;
    }

#ifdef AE_COMPILER_MSVC
    ND_ inline float  StringToFloat (StringView str) __NE___
    {
        float   val = 0.0f;
        Unused( StringToValue( str, OUT val ));
        return val;
    }

    ND_ inline double  StringToDouble (StringView str) __NE___
    {
        double  val = 0.0;
        Unused( StringToValue( str, OUT val ));
        return val;
    }
#endif

} // AE::Base
