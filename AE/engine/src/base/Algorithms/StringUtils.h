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
//-----------------------------------------------------------------------------



/*
=================================================
	FindChar
----
	faster than StringView{}.find( ... )
	returns >= 'end' if not found
=================================================
*/
	template <typename T>
	Nd__IF const T*  FindChar (T const* begin, T const* end, const T ch) __NE___
	{
		if constexpr( sizeof(T) == 1 )
			return Cast<T>( FindChar8_SIMD( Cast<sbyte>(begin), Cast<sbyte>(end), sbyte(ch) ));
		else
		if constexpr( sizeof(T) == 2 )
			return Cast<T>( FindChar16_SIMD( Cast<ushort>(begin), Cast<ushort>(end), ushort(ch) ));
		else
		if constexpr( sizeof(T) == 4 )
			return Cast<T>( FindChar32_SIMD( Cast<uint>(begin), Cast<uint>(end), uint(ch) ));
	}

	template <typename T>
	Nd__IF T*  FindChar (T* begin, T const* end, const T ch) __NE___
	{
		if constexpr( sizeof(T) == 1 )
			return const_cast<T*>( Cast<T>( FindChar8_SIMD( Cast<sbyte>(begin), Cast<sbyte>(end), sbyte(ch) )));
		else
		if constexpr( sizeof(T) == 2 )
			return const_cast<T*>( Cast<T>( FindChar16_SIMD( Cast<ushort>(begin), Cast<ushort>(end), ushort(ch) )));
		else
		if constexpr( sizeof(T) == 4 )
			return const_cast<T*>( Cast<T>( FindChar32_SIMD( Cast<uint>(begin), Cast<uint>(end), uint(ch) )));
	}

	template <typename T>
	Nd__IF const T*  FindChar (BasicStringView<T> str, const T ch, const usize first = 0) __NE___
	{
		return FindChar( str.data()+first, str.data()+str.size(), ch );
	}

/*
=================================================
	IsNullTerminated
----
	warning: string view does not allow out of range access,
		but on most cases it is safe
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
	Nd__IF bool  IsUpperCase (const CharAnsi c) __NE___
	{
		return (c >= 'A') and (c <= 'Z');
	}

	Nd__IF bool  IsUpperCase (const CharUtf32 c) __NE___
	{
		return Utf32ToUpper( c );
	}

/*
=================================================
	IsLowerCase
=================================================
*/
	Nd__IF bool  IsLowerCase (const CharAnsi c) __NE___
	{
		return (c >= 'a') and (c <= 'z');
	}

	Nd__IF bool  IsLowerCase (const CharUtf32 c) __NE___
	{
		return Utf32IsLower( c );
	}

/*
=================================================
	ToLowerCase
=================================================
*/
	Nd__IF CharAnsi  ToLowerCase (const CharAnsi c) __NE___
	{
		return IsUpperCase( c ) ? (c - 'A' + 'a') : c;
	}

	Nd__IF CharUtf32  ToLowerCase (const CharUtf32 c) __NE___
	{
		return Utf32ToLower( c );
	}

/*
=================================================
	ToUpperCase
=================================================
*/
	Nd__IF CharAnsi  ToUpperCase (const CharAnsi c) __NE___
	{
		return IsLowerCase( c ) ? (c - 'a' + 'A') : c;
	}

	Nd__IF CharUtf32  ToUpperCase (const CharUtf32 c) __NE___
	{
		return Utf32ToUpper( c );
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
	Nd__IF BasicStringView<T>  SubStringBE (BasicStringView<T> src, usize begin, usize end) __NE___
	{
		ASSERT( begin <= end );
		ASSERT( end <= src.size() );

		end		= Min( end, src.size() );
		begin	= Min( begin, end );
		return BasicStringView<T>{ src.data() + begin, end - begin };
	}

	template <typename T>
	Nd__IF BasicStringView<T>  SubStringBE (const BasicString<T> &src, usize begin, usize end) __NE___
	{
		return SubStringBE( BasicStringView<T>{src}, begin, end );
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
namespace _hidden_ {
	template <typename T>
	bool  EqualICImpl (BasicStringView<T> lhs, BasicStringView<T> rhs) __NE___
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
}
	Nd__In bool  EqualIC (StringView    lhs, StringView    rhs) __NE___	{ return Base::_hidden_::EqualICImpl( lhs, rhs ); }
	Nd__In bool  EqualIC (U32StringView lhs, U32StringView rhs) __NE___	{ return Base::_hidden_::EqualICImpl( lhs, rhs ); }

/*
=================================================
	FindString
----
	faster than StringView{}.find( ... )
	returns 'str.end()' if not found
=================================================
*/
namespace _hidden_ {
	template <typename T>
	const T*  FindStringImpl (BasicStringView<T> str, BasicStringView<T> substr, usize first) __NE___
	{
		if ( str.size() < substr.size() )
			return str.data() + str.size();

		const T*	end = str.data() + str.size() - substr.size() + 1;
		const T*	ptr = str.data() + first;

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
}
	Nd__In const char*		FindString (StringView str, StringView substr, usize first)			__NE___	{ return Base::_hidden_::FindStringImpl( str, substr, first ); }
	Nd__In const CharUtf8*	FindString (U8StringView str, U8StringView substr, usize first)		__NE___	{ return Base::_hidden_::FindStringImpl( str, substr, first ); }
	Nd__In const CharUtf32*	FindString (U32StringView str, U32StringView substr, usize first)	__NE___	{ return Base::_hidden_::FindStringImpl( str, substr, first ); }

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
	HasChar
----
	same as 'string::contains()'
=================================================
*/
	template <typename T>
	Nd__In bool  HasChar (BasicStringView<T> str, const T ch) __NE___
	{
		const char*	end = str.data() + str.size();
		return FindChar( str.data(), end, ch ) < end;
	}

/*
=================================================
	HasSubString
----
	returns 'true' if 'str' has substring 'substr',
	comparison is case sensitive.
=================================================
*/
namespace _hidden_ {
	template <typename T>
	bool  HasSubStringImpl (BasicStringView<T> str, BasicStringView<T> substr) __NE___
	{
		auto*	end = str.data() + str.size();
		return FindStringImpl<T>( str, substr, 0u ) < end;
	}
}
	Nd__In bool  HasSubString (StringView str, StringView substr)		__NE___	{ return Base::_hidden_::HasSubStringImpl( str, substr ); }
	Nd__In bool  HasSubString (U8StringView str, U8StringView substr)	__NE___	{ return Base::_hidden_::HasSubStringImpl( str, substr ); }
	Nd__In bool  HasSubString (U32StringView str, U32StringView substr)	__NE___	{ return Base::_hidden_::HasSubStringImpl( str, substr ); }

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
namespace _hidden_ {
	template <typename T>
	bool  StartsWithImpl (BasicStringView<T> str, BasicStringView<T> substr) __NE___
	{
		if ( str.length() < substr.length() )
			return false;

		return std::memcmp( str.data(), substr.data(), substr.length() ) == 0;
	}
}
	Nd__In bool  StartsWith (StringView str, StringView substr)			__NE___	{ return Base::_hidden_::StartsWithImpl( str, substr ); }
	Nd__In bool  StartsWith (U8StringView str, U8StringView substr)		__NE___	{ return Base::_hidden_::StartsWithImpl( str, substr ); }
	Nd__In bool  StartsWith (U32StringView str, U32StringView substr)	__NE___	{ return Base::_hidden_::StartsWithImpl( str, substr ); }

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
namespace _hidden_ {
	template <typename T>
	bool  EndsWithImpl (BasicStringView<T> str, BasicStringView<T> substr) __NE___
	{
		if ( str.length() < substr.length() )
			return false;

		return std::memcmp( str.data()+str.size() - substr.length(), substr.data(), substr.length() ) == 0;
	}
}
	Nd__In bool  EndsWith (StringView str, StringView substr)		__NE___	{ return Base::_hidden_::EndsWithImpl( str, substr ); }
	Nd__In bool  EndsWith (U8StringView str, U8StringView substr)	__NE___	{ return Base::_hidden_::EndsWithImpl( str, substr ); }
	Nd__In bool  EndsWith (U32StringView str, U32StringView substr)	__NE___	{ return Base::_hidden_::EndsWithImpl( str, substr ); }

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
namespace _hidden_ {
	template <typename T>
	uint  FindAndReplaceCharImpl (INOUT MutableArrayView<T> str, const T oldSymb, const T newSymb) __NE___
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

	template <typename T>
	uint  FindAndReplaceCharImpl (INOUT BasicString<T> &str, const T oldSymb, const T newSymb) __NE___
	{
		return FindAndReplaceCharImpl( MutableArrayView<T>{ str.data(), str.size() }, oldSymb, newSymb );
	}

	template <typename T>
	uint  FindAndReplaceStringImpl (INOUT BasicString<T> &str, BasicStringView<T> oldStr, BasicStringView<T> newStr) __Th___
	{
		String::size_type	pos		= 0;
		uint				count	= 0;

		while ( (pos = BasicStringView<T>{str}.find( oldStr, pos )) != StringView::npos )
		{
			str.replace( pos, oldStr.length(), newStr.data() );  // throw
			pos += newStr.length();
			++count;
		}
		return count;
	}
}
	inline uint  FindAndReplace (INOUT MutableArrayView<char> str, const char oldSymb, const char newSymb)				__NE___	{ return Base::_hidden_::FindAndReplaceCharImpl( INOUT str, oldSymb, newSymb ); }
	inline uint  FindAndReplace (INOUT String &str, const char oldSymb, const char newSymb)								__NE___	{ return Base::_hidden_::FindAndReplaceCharImpl( INOUT str, oldSymb, newSymb ); }
	inline uint  FindAndReplace (INOUT String &str, StringView oldStr, StringView newStr)								__Th___	{ return Base::_hidden_::FindAndReplaceStringImpl( INOUT str, oldStr, newStr ); }

	inline uint  FindAndReplace (INOUT MutableArrayView<CharUtf8> str, const CharUtf8 oldSymb, const CharUtf8 newSymb)	__NE___	{ return Base::_hidden_::FindAndReplaceCharImpl( INOUT str, oldSymb, newSymb ); }
	inline uint  FindAndReplace (INOUT U8String &str, const CharUtf8 oldSymb, const CharUtf8 newSymb)					__NE___	{ return Base::_hidden_::FindAndReplaceCharImpl( INOUT str, oldSymb, newSymb ); }
	inline uint  FindAndReplace (INOUT U8String &str, U8StringView oldStr, U8StringView newStr)							__Th___	{ return Base::_hidden_::FindAndReplaceStringImpl( INOUT str, oldStr, newStr ); }
//-----------------------------------------------------------------------------



/*
=================================================
	WCharToAnsi
----
	Converts wide string to ansi string (7bit), unsupported symbols replaced by 'defaultChar'.
	Returns 'true' if 'src' is already ansi string and 'false' otherwise.
	deprecated, use 'ConvertString()'
=================================================
*/
	template <typename T>
	__Cx__ bool  WCharToAnsi (OUT CharAnsi* dst, const T* src, usize len, const CharAnsi defaultChar = CharAnsi('?')) __NE___
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
----
	Converts utf8 string to ansi string (7bit), unsupported symbols replaced by 'defaultChar'.
	Returns 'true' if 'src' is already ansi string and 'false' otherwise.
	deprecated, use 'ConvertString()'
=================================================
*/
#ifdef AE_ENABLE_UTF8PROC
	____In bool  Utf8ToAnsi (OUT CharAnsi* dst, const CharUtf8* src, INOUT usize &len, const CharAnsi defaultChar = CharAnsi('?')) __NE___
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
	IsAnsiString
----
	compatible with wchar, utf8, ...
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
	template <typename StringType>
	NdCx__ bool  StringLessThan (const StringType& lhs, const StringType& rhs) __NE___
	{
		return std::lexicographical_compare( lhs.begin(), lhs.end(), rhs.begin(), rhs.end() );
	}

/*
=================================================
	ConvertString (WString)
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
	template <typename T>
	Nd__In bool  ConvertString (OUT WString &dst, BasicStringView<T> src) __NE___
	{
		//StaticAssert( not IsSame< T, wchar_t >);
		//StaticAssert( not IsSame< T, CharUtf16 >);
		StaticAssert( sizeof(wchar_t) == sizeof(CharUtf16) );

		if constexpr( IsSame< T, CharAnsi >	or
					  IsSame< T, wchar_t >	or
					  IsSame< T, CharUtf16 >)
		{
			NOTHROW_ERR( dst.assign( src.begin(), src.end() ));
			return true;
		}

		// Utf8 -> WChar
		if constexpr( IsSame< T, CharUtf8 >)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			usize j = 0;
			for (usize i = 0; i < src.size();)
			{
				if_unlikely( j+2 >= dst.size() )
				{
					NOTHROW_ERR( dst.resize( dst.size() + 128 ));
				}

				auto [c32, w] = Base::_hidden_::Utf8Decode_v2( src.data() + i, src.size()-i );
				ASSERT( w > 0 and w <= 4 );
				i += Max( w, 1u );

				uint	cnt = Utf16Encode( c32, 2, OUT Cast<CharUtf16>(dst.data() + j) );
				ASSERT( cnt > 0 );
				j += cnt;
			}
			dst.resize( j );
			return true;
		}

		// Utf32 -> WChar
		if constexpr( IsSame< T, CharUtf32 >)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			usize j = 0;
			for (CharUtf32 c : src)
			{
				if_unlikely( j+2 >= dst.size() )
				{
					NOTHROW_ERR( dst.resize( dst.size() + 128 ));
				}

				uint	cnt = Utf16Encode( c, 2, OUT Cast<CharUtf16>(dst.data() + j) );
				ASSERT( cnt > 0 );
				j += cnt;
			}

			dst.resize( j );
			return true;
		}
	}
#endif

/*
=================================================
	ConvertString (U16String)
=================================================
*/
	template <typename T>
	Nd__In bool  ConvertString (OUT U16String &dst, BasicStringView<T> src) __NE___
	{
	  #ifdef AE_PLATFORM_WINDOWS
		//StaticAssert( not IsSame< T, wchar_t >);
	  #endif
		//StaticAssert( not IsSame< T, CharUtf16 >);

		if constexpr( IsSame< T, CharAnsi >	or
					  IsSame< T, CharUtf16 >
					#ifdef AE_PLATFORM_WINDOWS
					  or IsSame< T, wchar_t >
					#endif
					)
		{
			NOTHROW_ERR( dst.assign( src.begin(), src.end() ));
			return true;
		}

		// Utf8 -> Utf16
		if constexpr( IsSame< T, CharUtf8 >)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			usize j = 0;
			for (usize i = 0; i < src.size();)
			{
				if_unlikely( j+2 >= dst.size() )
				{
					NOTHROW_ERR( dst.resize( dst.size() + 128 ));
				}

				auto [c32, w] = Base::_hidden_::Utf8Decode_v2( src.data() + i, src.size()-i );
				ASSERT( w > 0 and w <= 4 );
				i += Max( w, 1u );

				uint	cnt = Utf16Encode( c32, 2, OUT dst.data() + j );
				ASSERT( cnt > 0 );
				j += cnt;
			}
			dst.resize( j );
			return true;
		}

		// Utf32 -> Utf16
		if constexpr( IsSame< T, CharUtf32 >)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			usize j = 0;
			for (CharUtf32 c : src)
			{
				if_unlikely( j+2 >= dst.size() )
				{
					NOTHROW_ERR( dst.resize( dst.size() + 128 ));
				}

				uint	cnt = Utf16Encode( c, 2, OUT Cast<CharUtf16>(dst.data() + j) );
				ASSERT( cnt > 0 );
				j += cnt;
			}

			dst.resize( j );
			return true;
		}
	}

/*
=================================================
	ConvertString (U8String)
=================================================
*/
	template <typename T>
	Nd__In bool  ConvertString (OUT U8String &dst, BasicStringView<T> src) __NE___
	{
		//StaticAssert( not IsSame< T, CharAnsi >);
		//StaticAssert( not IsSame< T, CharUtf8 >);

		if constexpr( IsSame< T, CharAnsi >	or
					  IsSame< T, CharUtf8 >)
		{
			NOTHROW_ERR( dst.assign( src.begin(), src.end() ));
			return true;
		}

		// Utf16 -> Utf8
		if constexpr( IsSame< T, CharUtf16 >
					#ifdef AE_PLATFORM_WINDOWS
					  or IsSame< T, wchar_t >
					#endif
					)
		{
			NOTHROW_ERR( dst.resize( src.size()*2 ));

			usize j = 0;
			for (usize i = 0; i < src.size();)
			{
				if_unlikely( j+4 >= dst.size() )
				{
					NOTHROW_ERR( dst.resize( dst.size() + 128 ));
				}

				auto [c32, w] = Utf16Decode( Cast<CharUtf16>(src.data() + i), src.size()-i );
				ASSERT( w > 0 and w <= 2 );
				i += Max( w, 1u );

				uint	cnt = Base::_hidden_::Utf8Encode_v2( c32, 4, OUT dst.data() + j );
				ASSERT( cnt > 0 );
				j += cnt;
			}
			dst.resize( j );
			return true;
		}

		// Utf32 -> Utf8
		if constexpr( IsSame< T, CharUtf32 >)
		{
			NOTHROW_ERR( dst.resize( src.size()*3 ));

			usize j = 0;
			for (CharUtf32 c : src)
			{
				if_unlikely( j+4 >= dst.size() )
				{
					NOTHROW_ERR( dst.resize( dst.size() + 128 ));
				}

				uint	cnt = Base::_hidden_::Utf8Encode_v2( c, 4, OUT dst.data() + j );
				ASSERT( cnt > 0 );
				j += cnt;
			}
			dst.resize( j );
			return true;
		}
	}

/*
=================================================
	ConvertString (U32String)
=================================================
*/
	template <typename T>
	Nd__In bool  ConvertString (OUT U32String &dst, BasicStringView<T> src) __NE___
	{
		//StaticAssert( not IsSame< T, CharUtf32 >);

		if constexpr( IsSame< T, CharUtf32 > or
					  IsSame< T, CharAnsi >)
		{
			NOTHROW_ERR( dst.assign( src.begin(), src.end() ));
			return true;
		}

		// Utf16 -> Utf32
		if constexpr( IsSame< T, CharUtf16 >
					#ifdef AE_PLATFORM_WINDOWS
					  or IsSame< T, wchar_t >
					#endif
					)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			usize j = 0;
			for (usize i = 0; i < src.size(); ++j)
			{
				auto [c, w] = Utf16Decode( src.data() + i, src.size() - i );
				ASSERT( w > 0 and w <= 2 );

				i += w;
				dst[j] = c;
			}
			dst.resize( j );
			return true;
		}

		// Utf8 -> Utf32
		if constexpr( IsSame< T, CharUtf8 >)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			usize j = 0;
			for (usize i = 0; i < src.size(); ++j)
			{
				auto [c, w] = Base::_hidden_::Utf8Decode_v2( src.data() + i, src.size() - i );
				ASSERT( w > 0 and w <= 4 );

				i += w;
				dst[j] = c;
			}
			dst.resize( j );
			return true;
		}
	}

/*
=================================================
	ConvertString (AnsiString)
=================================================
*/
	template <typename T>
	Nd__In bool  ConvertString (OUT String &dst, BasicStringView<T> src, const CharAnsi defaultChar = '?') __NE___
	{
		StaticAssert( not IsSame< T, CharAnsi >);

		if constexpr( IsSame< T, CharAnsi >)
		{
			NOTHROW_ERR( dst.assign( src.begin(), src.end() ));
			return true;
		}

		// Utf8 -> Ansi
		if constexpr( IsSame< T, CharUtf8 >)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			usize j = 0;
			for (usize i = 0; i < src.size(); ++j)
			{
				auto [c, w] = Base::_hidden_::Utf8Decode_v2( src.data() + i, src.size() - i );
				ASSERT( w > 0 and w <= 4 );

				i += w;
				dst[j] = c < 0x7F ? CharAnsi(c) : defaultChar;
			}
			dst.resize( j );
			return true;
		}

		// Utf16 -> Ansi
		if constexpr( IsSame< T, CharUtf16 >
					#ifdef AE_PLATFORM_WINDOWS
					  or IsSame< T, wchar_t >
					#endif
					)
		{
			NOTHROW_ERR( dst.resize( src.size() ));

			auto*	ptr = Cast<CharUtf16>( src.data() );

			usize j = 0;
			for (usize i = 0; i < src.size(); ++j)
			{
				auto [c, w] = Utf16Decode( ptr + i, src.size() - i );

				i += w;
				dst[j] = c < 0x7F ? CharAnsi(c) : defaultChar;
			}
			dst.resize( j );
			return true;
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	FromChars
----
	leading whitespace is not ignored
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

		explicit FromCharsResult (ssize inProcessed, bool isOK) __NE___ :
			processed{ uint(inProcessed) },
			ok{ isOK and inProcessed > 0 }
		{}

		ND_ explicit operator bool ()	C_NE___	{ return ok; }
	};

	template <typename T>
	  requires( IsInteger<T> )
	ND_ FromCharsResult  FromChars (OUT T &result, StringView str) __NE___
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
	template <typename T>
	  requires( IsFloatPoint<T> )
	ND_ FromCharsResult  FromChars (OUT T &result, StringView str) __NE___
	{
		auto	err	= std::from_chars( str.data(), str.data() + str.size(), OUT result, std::chars_format::general );
		return	FromCharsResult{ err, str.data() };
	}
#else
	template <typename T>
	  requires( IsFloatPoint<T> )
	ND_ FromCharsResult  FromChars (OUT T &result, NtStringView str) __NE___
	{
		if constexpr( IsSame< T, float >)
		{
			char* end = null;
			result = std::strtof( str.c_str(), OUT &end );
			return FromCharsResult{ end - str.c_str(), true };
		}
		else
		if constexpr( IsSame< T, double >)
		{
			char* end = null;
			result = std::strtod( str.c_str(), OUT &end );
			return FromCharsResult{ end - str.c_str(), true };
		}
	}
#endif

	template <typename T>
	  requires( IsInteger<T> )
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

	Nd__In ulong  StringToULong (StringView str, int base = 10) __NE___
	{
		ulong	val = 0;
		auto	ok	= FromChars( OUT val, str, base );	ASSERT( ok );	Unused( ok );
		return val;
	}

	Nd__In slong  StringToSLong (StringView str, int base = 10) __NE___
	{
		slong	val = 0;
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
#else
	Nd__In float  StringToFloat (NtStringView str) __NE___
	{
		float	val = 0.0f;
		auto	ok	= FromChars( OUT val, str );	ASSERT( ok );	Unused( ok );
		return val;
	}

	Nd__In double  StringToDouble (NtStringView str) __NE___
	{
		double	val = 0.0;
		auto	ok	= FromChars( OUT val, str );	ASSERT( ok );	Unused( ok );
		return val;
	}
#endif

} // AE::Base
