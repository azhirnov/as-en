// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/ArrayUtils.h"
#include "base/Memory/MemUtils.h"
#include "base/SIMD/MemUtils.h"
#include "base/Containers/NtStringView.h"
#include "base/Algorithms/Utf8.h"

namespace AE::Base
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;

/*
=================================================
	operator << (String &, String)
	operator << (String &, StringView)
	operator << (String &, CStyleString)
	operator << (String &, char)
=================================================
*/
	template <typename T, typename A1, typename A2>
	BasicString<T,A1>&&  operator << (BasicString<T,A1> &&lhs, const BasicString<T,A2> &rhs) __Th___
	{
		return RVRef( RVRef(lhs).append( rhs.data(), rhs.size() ));
	}

	template <typename T, typename A1, typename A2>
	BasicString<T,A1>&  operator << (BasicString<T,A1> &lhs, const BasicString<T,A2> &rhs) __Th___
	{
		return lhs.append( rhs.data(), rhs.size() );
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const BasicStringView<T> &rhs) __Th___
	{
		return RVRef( RVRef(lhs).append( rhs.data(), rhs.size() ));
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const BasicStringView<T> &rhs) __Th___
	{
		return lhs.append( rhs.data(), rhs.size() );
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const NtBasicStringView<T> &rhs) __Th___
	{
		return RVRef( RVRef(lhs).append( rhs.c_str(), rhs.size() ));
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const NtBasicStringView<T> &rhs) __Th___
	{
		return lhs.append( rhs.c_str(), rhs.size() );
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, T const * const rhs) __Th___
	{
		return	rhs != null ?
					RVRef( RVRef(lhs).append( rhs )) :
					RVRef(lhs);
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator << (BasicString<T,A> &lhs, T const * const rhs) __Th___
	{
		return	rhs != null ?
					lhs.append( rhs ) :
					lhs;
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator << (BasicString<T,A> &&lhs, const T rhs) __Th___
	{
		return RVRef( RVRef(lhs) += rhs );
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator << (BasicString<T,A> &lhs, const T rhs) __Th___
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
	BasicString<T,A1>&&  operator >> (const BasicString<T,A2> &lhs, BasicString<T,A1> &&rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return RVRef(rhs);
	}

	template <typename T, typename A1, typename A2>
	BasicString<T,A1>&  operator >> (const BasicString<T,A2> &lhs, BasicString<T,A1> &rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return rhs;
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator >> (const BasicStringView<T> &lhs, BasicString<T,A> &&rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator >> (const BasicStringView<T> &lhs, BasicString<T,A> &rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return rhs;
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator >> (const NtBasicStringView<T> &lhs, BasicString<T,A> &&rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator >> (const NtBasicStringView<T> &lhs, BasicString<T,A> &rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs.begin(), lhs.end() );
		return rhs;
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator >> (T const * const lhs, BasicString<T,A> &&rhs) __Th___
	{
		rhs.insert( 0u, lhs );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator >> (T const * const lhs, BasicString<T,A> &rhs) __Th___
	{
		rhs.insert( 0u, lhs );
		return rhs;
	}

	template <typename T, typename A>
	BasicString<T,A>&&  operator >> (const T lhs, BasicString<T,A> &&rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs );
		return RVRef(rhs);
	}

	template <typename T, typename A>
	BasicString<T,A>&  operator >> (const T lhs, BasicString<T,A> &rhs) __Th___
	{
		rhs.insert( rhs.begin(), lhs );
		return rhs;
	}

/*
=================================================
	operator << (String &,		CStyleString)
	operator << (String &,		char)
	operator >> (CStyleString,	String &)
	operator >> (char,			String &)
=================================================
*/
#if not defined(__cpp_char8_t)
	inline U8String&&  operator << (U8String &&lhs, char const * const rhs) __Th___
	{
		return	rhs != null ?
					RVRef( RVRef(lhs).append( Cast<CharUtf8>(rhs) )) :
					RVRef(lhs);
	}

	inline U8String&  operator << (U8String &lhs, char const * const rhs) __Th___
	{
		return	rhs != null ?
					lhs.append( Cast<CharUtf8>(rhs) ) :
					lhs;
	}

	inline U8String&&  operator << (U8String &&lhs, const char rhs) __Th___
	{
		return RVRef( RVRef(lhs) += CharUtf8(rhs) );
	}

	inline U8String&  operator << (U8String &lhs, const char rhs) __Th___
	{
		return (lhs += CharUtf8(rhs));
	}

	inline U8String&&  operator >> (char const * const lhs, U8String &&rhs) __Th___
	{
		rhs.insert( 0u, Cast<CharUtf8>(lhs) );
		return RVRef(rhs);
	}

	inline U8String&  operator >> (char const * const lhs, U8String &rhs) __Th___
	{
		rhs.insert( 0u, Cast<CharUtf8>(lhs) );
		return rhs;
	}

	inline U8String&&  operator >> (const char lhs, U8String &&rhs) __Th___
	{
		rhs.insert( rhs.begin(), CharUtf8(lhs) );
		return RVRef(rhs);
	}

	inline U8String&  operator >> (const char lhs, U8String &rhs) __Th___
	{
		rhs.insert( rhs.begin(), CharUtf8(lhs) );
		return rhs;
	}
#endif
//-----------------------------------------------------------------------------



/*
=================================================
	FindChar
----
	faster than StringView{}.find( ... )
	returns >= 'size' if not found
=================================================
*
	Nd__IF usize  FindChar (const char* ptr, const usize first, const usize size, const char ch) __NE___
	{
	#if 1
		return FindChar_SIMD( ptr + first, ptr + size - first, ch );
	#else
		auto* p = std::memchr( ptr + first, ch, size - first );
		return (p != null ? usize(Cast<char>(p) - ptr) : size);
	#endif
	}

	Nd__IF usize  FindChar (StringView str, const char ch, const usize first = 0) __NE___
	{
	#if 1
		return FindChar( str.data(), first, str.size(), ch );
	#else
		return Min( str.find( ch, first ), str.size() );
	#endif
	}

/*
=================================================
	FindChar
----
	faster than StringView{}.find( ... )
	returns >= 'end' if not found
=================================================
*/
	Nd__IF const char*  FindChar (char const* begin, char const* end, const char ch) __NE___
	{
		return FindChar_SIMD( begin, end, ch );
	}

	Nd__IF char*  FindChar (char* begin, char const* end, const char ch) __NE___
	{
		return const_cast<char*>( FindChar_SIMD( begin, end, ch ));
	}

	Nd__IF const char*  FindChar (StringView str, const char ch, const usize first = 0) __NE___
	{
		return FindChar( str.data()+first, str.data()+str.size(), ch );
	}

/*
=================================================
	IsNullTerminated
=================================================
*/
	template <typename StringType>
	ND_ bool  IsNullTerminated (const StringType &str) __NE___
	{
		return str.empty() or str.data()[ str.size() ] == 0;
	}

/*
=================================================
	IsUpperCase
=================================================
*/
	Nd__IF bool  IsUpperCase (const char c) __NE___
	{
		return (c >= 'A') and (c <= 'Z');
	}

/*
=================================================
	IsLowerCase
=================================================
*/
	Nd__IF bool  IsLowerCase (const char c) __NE___
	{
		return (c >= 'a') and (c <= 'z');
	}

/*
=================================================
	ToLowerCase
=================================================
*/
	Nd__IF char  ToLowerCase (const char c) __NE___
	{
		return IsUpperCase( c ) ? (c - 'A' + 'a') : c;
	}

/*
=================================================
	ToUpperCase
=================================================
*/
	Nd__IF char  ToUpperCase (const char c) __NE___
	{
		return IsLowerCase( c ) ? (c - 'a' + 'A') : c;
	}

/*
=================================================
	SubString
----
	same as string_view::substr but without exceptions
=================================================
*/
	template <typename T>
	Nd__IF BasicStringView<T>  SubString (BasicStringView<T> src, usize pos, usize count = UMax) __NE___
	{
		pos		= Min( pos, src.size() );
		count	= Min( count, src.size()-pos );
		return BasicStringView<T>{ src.data() + pos, count };
	}

	template <typename T>
	Nd__IF BasicStringView<T>  SubString (const BasicString<T> &src, usize pos, usize count = UMax) __NE___
	{
		return SubString( BasicStringView<T>{src}, pos, count );
	}

	template <typename T>
	Nd__IF BasicStringView<T>  SubString2 (BasicStringView<T> src, usize begin, usize end) __NE___
	{
		ASSERT( begin <= end );
		ASSERT( end <= src.size() );

		end		= Min( end, src.size() );
		begin	= Min( begin, end );
		return BasicStringView<T>{ src.data() + begin, end - begin };
	}

	template <typename T>
	Nd__IF BasicStringView<T>  SubString2 (const BasicString<T> &src, usize begin, usize end) __NE___
	{
		return SubString2( BasicStringView<T>{src}, begin, end );
	}

/*
=================================================
	SubString
----
	replacement for C++20 string_view ctor
=================================================
*/
	template <typename T>
	Nd__IF BasicStringView<T>  SubString (const T* first, const T* last) __NE___
	{
		ASSERT( first <= last );
		first = Min( first, last );
		return BasicStringView<T>{ first, usize(last - first) };
	}

	template <typename T>
	Nd__IF BasicStringView<T>  SubString (BasicStringView<T> src, const T* first, const T* last) __NE___
	{
		ASSERT( first <= last );
		ASSERT( first >= src.data() );
		ASSERT( last <= src.data()+src.size() );

		first = Max( first, src.data() );
		last  = Min( last,  src.data()+src.size() );
		first = Min( first, last );
		return BasicStringView<T>{ first, usize(last - first) };
	}

	template <typename T>
	Nd__IF BasicStringView<T>  SubString (const BasicString<T> &src, const T* first, const T* last) __NE___
	{
		return SubString( BasicStringView<T>{src}, first, last );
	}

/*
=================================================
	EqualIC
----
	returns 'true' if strings are equal,
	comparison is case insensitive.
=================================================
*/
	Nd__In bool  EqualIC (StringView lhs, StringView rhs) __NE___
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
	FindString
----
	faster than StringView{}.find( ... )
	returns 'str.end()' if not found
=================================================
*/
	Nd__In const char*  FindString (StringView str, StringView substr, usize first = 0) __NE___
	{
		if ( str.size() < substr.size() )
			return str.data() + str.size();

		const char*		end = str.data() + str.size() - substr.size() + 1;
		const char*		ptr = str.data() + first;

		for_likely(; ptr < end;)
		{
			ptr = FindChar( ptr, end, substr[0] );
			if_likely( ptr < end )
			{
				usize j = 1;
				for (;(j < substr.size()) and (substr[j] == ptr[j]); ++j)	// TODO: optimzie
				{}
				if_unlikely( j >= substr.size() )
					return ptr;
				++ptr;
			}
		}
		return str.data() + str.size();
	}

/*
=================================================
	FindStringIC
----
	faster than StringView{}.find( ... )
	returns 'str.size()' if not found,
	comparison is case insensitive.
=================================================
*/
	Nd__In const char*  FindStringIC (StringView str, StringView substr, const usize first = 0) __NE___
	{
		if ( str.size() < substr.size() )
			return str.data() + str.size();

		const char*		end = str.data() + str.size() - substr.size() + 1;
		const char		up	= ToUpperCase( substr[0] );
		const char		low	= ToLowerCase( substr[0] );

		for_likely( const char* ptr = str.data() + first; ptr < end; )
		{
			ptr = FindChar( ptr, end, up );
			if_likely( ptr < end )
			{
				usize j = 1;
				for (;(j < substr.size()) and (ToUpperCase( substr[j] ) == ToUpperCase( ptr[j] )); ++j)
				{}
				if_unlikely( j >= substr.size() )
					return ptr;
				++ptr;
			}
		}

		if_likely( up == low )
			return str.data() + str.size();

		for_likely( const char* ptr = str.data() + first; ptr < end; )
		{
			ptr = FindChar( ptr, end, low );
			if_likely( ptr < end )
			{
				usize j = 1;
				for (;(j < substr.size()) and (ToLowerCase( substr[j] ) == ToLowerCase( ptr[j] )); ++j)
				{}
				if_unlikely( j >= substr.size() )
					return ptr;
				++ptr;
			}
		}
		return str.data() + str.size();
	}

/*
=================================================
	HasSubString
----
	returns 'true' if 'str' has substring 'substr',
	comparison is case sensitive.
=================================================
*/
	Nd__In bool  HasSubString (StringView str, StringView substr) __NE___
	{
		auto*	end = str.data() + str.size();
		return FindString( str, substr ) < end;
	}

/*
=================================================
	HasSubStringIC
----
	returns 'true' if 'str' has substring 'substr',
	comparison is case insensitive.
=================================================
*/
	Nd__In bool  HasSubStringIC (StringView str, StringView substr) __NE___
	{
		auto*	end = str.data() + str.size();
		return FindStringIC( str, substr ) < end;
	}

/*
=================================================
	StartsWith
----
	returns 'true' if 'str' starts with substring 'substr',
	comparison is case sensitive.
=================================================
*/
	Nd__In bool  StartsWith (StringView str, StringView substr) __NE___
	{
		if ( str.length() < substr.length() )
			return false;

		return std::memcmp( str.data(), substr.data(), substr.length() ) == 0;
	}

/*
=================================================
	StartsWithIC
----
	returns 'true' if 'str' starts with substring 'substr',
	comparison is case insensitive.
=================================================
*/
	Nd__In bool  StartsWithIC (StringView str, StringView substr) __NE___
	{
		if ( str.length() < substr.length() )
			return false;

		for_likely (usize i = 0; i < substr.length(); ++i)
		{
			if_unlikely( ToLowerCase( str[i] ) != ToLowerCase( substr[i] ))
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
	Nd__In bool  EndsWith (StringView str, StringView substr) __NE___
	{
		if ( str.length() < substr.length() )
			return false;

		return std::memcmp( str.data()+str.size() -substr.length(), substr.data(), substr.length() ) == 0;
	}

/*
=================================================
	EndsWithIC
----
	returns 'true' if 'str' ends with substring 'substr',
	comparison is case insensitive.
=================================================
*/
	Nd__In bool  EndsWithIC (StringView str, StringView substr) __NE___
	{
		if ( str.length() < substr.length() )
			return false;

		for_likely(usize i = 1; i <= substr.length(); ++i)
		{
			if_unlikely( ToLowerCase(str[str.length() - i]) != ToLowerCase(substr[substr.length() - i]) )
				return false;
		}
		return true;
	}

/*
=================================================
	FindAndReplace
----
	returns number of replaced symbols/substrings
=================================================
*/
	inline uint  FindAndReplace (INOUT MutableArrayView<char> str, const char oldSymb, const char newSymb) __NE___
	{
		uint		count	= 0;
		auto*		ptr		= str.data();
		const auto*	end		= str.data() + str.size();

		for_likely(;;)
		{
			ptr = FindChar( ptr, end, oldSymb );
			if_likely( ptr < end )
			{
				*ptr = newSymb;
				++ptr;
				++count;
			}else
				break;
		}
		return count;
	}

	inline uint  FindAndReplace (INOUT String &str, const char oldSymb, const char newSymb) __NE___
	{
		return FindAndReplace( MutableArrayView<char>{ str.data(), str.size() }, oldSymb, newSymb );
	}

	inline uint  FindAndReplace (INOUT String& str, StringView oldStr, StringView newStr) __Th___
	{
		String::size_type	pos		= 0;
		uint				count	= 0;

		while ( (pos = StringView{str}.find( oldStr, pos )) != StringView::npos )
		{
			str.replace( pos, oldStr.length(), newStr.data() );  // throw
			pos += newStr.length();
			++count;
		}
		return count;
	}

/*
=================================================
	WCharToAnsi
----
	Converts wide string to ansi string (7bit), unsupported symbols replaced by 'defaultChar'.
	Returns 'true' if 'src' is already ansi string and 'false' otherwise.
=================================================
*/
	template <typename T>
	__Cz__ bool  WCharToAnsi (OUT CharAnsi* dst, const T* src, usize len, const CharAnsi defaultChar = CharAnsi('?')) __NE___
	{
		NonNull( dst );
		NonNull( src );

		bool	res = true;
		for (usize i = 0; i < len; ++i)
		{
			res		&= (src[i] <= 0x7F);
			dst[i]	 = (src[i] <= 0x7F ? CharAnsi(src[i]) : defaultChar);
		}
		dst[len] = CharAnsi{0};
		return res;
	}

/*
=================================================
	Utf8ToAnsi
	Converts utf8 string to ansi string (7bit), unsupported symbols replaced by 'defaultChar'.
	Returns 'true' if 'src' is already ansi string and 'false' otherwise.
=================================================
*/
#ifdef AE_ENABLE_UTF8PROC
	__CzIn bool  Utf8ToAnsi (OUT CharAnsi* dst, const CharUtf8* src, INOUT usize &len, const CharAnsi defaultChar = CharAnsi('?')) __NE___
	{
		NonNull( dst );
		NonNull( src );

		bool	res = true;
		usize	i	= 0;
		for (usize pos = 0; (pos < len) and (i < len); ++i)
		{
			CharUtf32	utf = Utf8Decode( src, len, INOUT pos );
			res		&= (utf <= 0x7F);
			dst[i]	 = (utf <= 0x7F ? CharAnsi(utf) : defaultChar);
		}
		dst[i]	= CharAnsi{0};
		len		= i;
		return res;
	}
#endif

/*
=================================================
	ToAnsiString
----
	See 'WCharToAnsi' and 'Utf8ToAnsi'.
=================================================
*/
	template <typename R, typename T>
	ND_ BasicString<R>  ToAnsiString (BasicStringView<T> str, const R defaultChar = R('?')) __Th___
	{
		if constexpr( IsSame< T, CharAnsi >)
			return BasicString<R>{str};
		else
		if constexpr( IsSame< T, wchar_t > or IsSame< T, CharUtf32 >)
		{
			BasicString<R>	result;
			result.resize( str.size() );	// throw
			WCharToAnsi( OUT result.data(), str.data(), str.length(), defaultChar );
			return result;
		}
	  #ifdef AE_ENABLE_UTF8PROC
		else
		if constexpr( IsSame< T, CharUtf8 >)
		{
			usize			len		= str.length();
			BasicString<R>	result;
			result.resize( str.size() );	// throw

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
	NdCx__ bool  IsAnsiString (const T* ptr, usize length) __NE___
	{
		for (usize i = 0; i < length; ++i)
		{
			if_unlikely( ToNearUInt(ptr[i]) > 0x7F )
				return false;
		}
		return true;
	}

	template <typename T>
	NdCx__ bool  IsAnsiString (BasicStringView<T> str) __NE___
	{
		return IsAnsiString( str.data(), str.length() );
	}

	template <typename T, typename A>
	NdCx__ bool  IsAnsiString (const BasicString<T,A> &str) __NE___
	{
		return IsAnsiString( str.c_str(), str.length() );
	}

/*
=================================================
	StringLessThan
=================================================
*/
	template <typename T>
	NdCx__ bool  StringLessThan (BasicStringView<T> lhs, BasicStringView<T> rhs) __NE___
	{
		return std::lexicographical_compare( lhs.begin(), lhs.end(), rhs.begin(), rhs.end() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	FromChars
=================================================
*/
	struct FromCharsResult
	{
		uint	processed	: 31;
		uint	ok			: 1;

		explicit FromCharsResult (const std::from_chars_result &res, const void* begin) __NE___ :
			processed{ uint( usize(res.ptr) - usize(begin) )},
			ok{ res.ec == std::errc{} }
		{}

		ND_ explicit operator bool ()	C_NE___	{ return ok; }
	};

	template <typename T>
	ND_ FromCharsResult  FromChars (OUT T &result, StringView str) __NE___
	{
		if constexpr( IsInteger<T> )
		{
			ASSERT( str.empty() or (str[0] != '-' and str[0] != '+') );

			usize	off	= 0;

			if ( str.size() > 2 and ((str[0] == '0') and (ToLowerCase(str[1]) == 'x')) )
				off = 2;
			else
			if ( str.size() > 1 and str[0] == '#' )
				off = 1;

			auto	err	= std::from_chars( str.data() + off, str.data() + str.size(), OUT result, off ? 16 : 10 );
			return	FromCharsResult{ err, str.data() };
		}

	  #ifdef AE_COMPILER_MSVC
		if constexpr( IsFloatPoint<T> )
		{
			auto	err	= std::from_chars( str.data(), str.data() + str.size(), OUT result, std::chars_format::general );
			return	FromCharsResult{ err, str.data() };
		}
	  #endif
	}

	template <typename T, ENABLEIF( IsInteger<T> )>
	ND_ FromCharsResult  FromChars (OUT T &val, StringView str, int base) __NE___
	{
		ASSERT( base == 10 or base == 16 );
		ASSERT( not StartsWith( str, "0x" ));

		auto	err	= std::from_chars( str.data(), str.data() + str.size(), OUT val, base );
		return	FromCharsResult{ err, str.data() };
	}

/*
=================================================
	StringTo***
=================================================
*/
	Nd__In uint  StringToUInt (StringView str, int base = 10) __NE___
	{
		uint	val = 0;
		auto	ok	= FromChars( OUT val, str, base );	ASSERT( ok );	Unused( ok );
		return val;
	}

	Nd__In int  StringToInt (StringView str, int base = 10) __NE___
	{
		int		val = 0;
		auto	ok	= FromChars( OUT val, str, base );	ASSERT( ok );	Unused( ok );
		return val;
	}

	Nd__In ulong  StringToUInt64 (StringView str, int base = 10) __NE___
	{
		ulong	val = 0;
		auto	ok	= FromChars( OUT val, str, base );	ASSERT( ok );	Unused( ok );
		return val;
	}

#ifdef AE_COMPILER_MSVC
	Nd__In float  StringToFloat (StringView str) __NE___
	{
		float	val = 0.0f;
		auto	ok	= FromChars( OUT val, str );	ASSERT( ok );	Unused( ok );
		return val;
	}

	Nd__In double  StringToDouble (StringView str) __NE___
	{
		double	val = 0.0;
		auto	ok	= FromChars( OUT val, str );	ASSERT( ok );	Unused( ok );
		return val;
	}
#endif

} // AE::Base
