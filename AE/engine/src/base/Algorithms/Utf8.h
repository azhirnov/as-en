// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TODO: use https://en.cppreference.com/w/cpp/locale/codecvt it x2 faster
*/

#pragma once

#ifdef AE_ENABLE_UTF8PROC

# include "base/Algorithms/Cast.h"
# include "utf8proc.h"

namespace AE::Base
{
namespace _hidden_
{
/*
=================================================
	Utf8Decode (v1)
=================================================
*/
	ND_ forceinline CharUtf32  Utf8Decode_v1 (const CharUtf8 *str, const usize length, INOUT usize &pos) __NE___
	{
		StaticAssert( sizeof(utf8proc_uint8_t) == sizeof(*str) );

		utf8proc_int32_t	symb;
		utf8proc_ssize_t	res = utf8proc_iterate( Cast<utf8proc_uint8_t>(str + pos), length - pos, OUT &symb );

		pos += (res > 0 ? res : 1);
		return symb;
	}

/*
=================================================
	Utf8Decode (v2)
----
	1.5-2x faster than 'Utf8Decode_v1()'
=================================================
*/
	ND_ forceinline constexpr CharUtf32  Utf8Decode_v2 (const CharUtf8 *str, const usize length, INOUT usize &pos) __NE___
	{
		str += pos;
		const uint	u = uint{*str};

		if ( (u < 0b1000'0000) and (length >= 1) )
		{
			pos += 1;
			return CharUtf32( str[0] );
		}

		if ( (u >= 0b1100'0000) and (u <= 0b1101'1111) and (length >= 2) )
		{
			pos += 2;
			return	(CharUtf32( str[0] & 0b0001'1111 ) << 6) |
					 CharUtf32( str[1] & 0b0011'1111 );
		}

		if ( (u >= 0b1100'0000) and (u <= 0b1110'1111) and (length >= 3) )
		{
			pos += 3;
			return	(CharUtf32( str[0] & 0b0000'1111 ) << 12) |
					(CharUtf32( str[1] & 0b0011'1111 ) <<  6) |
					 CharUtf32( str[2] & 0b0011'1111 );
		}

		if ( (u >= 0b1110'0000) and (u <= 0b1111'0111) and (length >= 4) )
		{
			pos += 4;
			return	(CharUtf32( str[0] & 0b0000'0111 ) << 18) |
					(CharUtf32( str[1] & 0b0011'1111 ) << 12) |
					(CharUtf32( str[2] & 0b0011'1111 ) <<  6) |
					 CharUtf32( str[3] & 0b0011'1111 );
		}

		return UMax;
	}

/*
=================================================
	Utf8CharWidth
=================================================
*/
	ND_ forceinline constexpr uint  Utf8CharWidth (const CharUtf8 *str, const usize length, const uint defaultWidth = 0) __NE___
	{
		const uint	u = uint{*str};

		if ( (u < 0b1000'0000) and (length >= 1) )
			return 1;

		if ( (u >= 0b1100'0000) and (u <= 0b1101'1111) and (length >= 2) )
			return 2;

		if ( (u >= 0b1100'0000) and (u <= 0b1110'1111) and (length >= 3) )
			return 3;

		if ( (u >= 0b1110'0000) and (u <= 0b1111'0111) and (length >= 4) )
			return 4;

		return defaultWidth;
	}

/*
=================================================
	Utf8Encode (v1)
=================================================
*/
	forceinline bool  Utf8Encode_V1 (const CharUtf32 symb, const usize dstSize, OUT CharUtf8 *dst, INOUT usize &pos) __NE___
	{
		Unused( dstSize );
		ASSERT( pos+4 <= dstSize );
		StaticAssert( sizeof(utf8proc_uint8_t) == sizeof(*dst) );

		ssize	cnt = utf8proc_encode_char( symb, Cast<utf8proc_uint8_t>(dst + pos) );
		ASSERT( cnt > 0 and cnt <= 4 );

		pos += cnt;
		return cnt > 0;
	}

/*
=================================================
	Utf8Encode (v2)
=================================================
*/
	forceinline bool  Utf8Encode_V2 (const CharUtf32 symb, const usize dstSize, OUT CharUtf8 *dst, INOUT usize &pos) __NE___
	{
		uint	u = uint{symb};

		if ( (u <= 0x7F) and (dstSize >= 1) )
		{
			*dst = CharUtf8(u);
			pos += 1;
			return true;
		}

		if ( (u >= 0x80) and (u <= 0x7FF) and (dstSize >= 2) )
		{
			dst[0] = CharUtf8( ((u >> 6) & 0b0001'1111) | 0b1100'0000 );
			dst[1] = CharUtf8(  (u       & 0b0011'1111) | 0b1000'0000 );
			pos += 2;
			return true;
		}

		if ( (u >= 0x800) and (u <= 0xFFFF) and (dstSize >= 3) )
		{
			dst[0] = CharUtf8( ((u >> 12) & 0b0000'1111) | 0b1110'0000 );
			dst[1] = CharUtf8( ((u >>  6) & 0b0011'1111) | 0b1000'0000 );
			dst[2] = CharUtf8(  (u        & 0b0011'1111) | 0b1000'0000 );
			pos += 3;
			return true;
		}

		if ( (u >= 0x1'0000) and (u <= 0x10'FFFF) and (dstSize >= 4) )
		{
			dst[0] = CharUtf8( ((u >> 18) & 0b0000'0111) | 0b1111'0000 );
			dst[1] = CharUtf8( ((u >> 12) & 0b0011'1111) | 0b1000'0000 );
			dst[2] = CharUtf8( ((u >>  6) & 0b0011'1111) | 0b1000'0000 );
			dst[3] = CharUtf8(  (u        & 0b0011'1111) | 0b1000'0000 );
			pos += 4;
			return true;
		}

		return false;
	}

} // _hidden_
//-----------------------------------------------------------------------------



/*
=================================================
	Utf8Decode
=================================================
*/
	ND_ forceinline CharUtf32  Utf8Decode (const CharUtf8 *str, const usize length, INOUT usize &pos) __NE___
	{
		ASSERT( pos < length );
		CharUtf32	c = Base::_hidden_::Utf8Decode_v1( str, length, INOUT pos );
		ASSERT( c != UMax );
		return c;
	}

	ND_ forceinline CharUtf32  Utf8Decode (BasicStringView<CharUtf8> str, INOUT usize &pos) __NE___
	{
		return Utf8Decode( str.data(), str.length(), INOUT pos );
	}

/*
=================================================
	Utf8CharCount
=================================================
*/
	ND_ forceinline usize  Utf8CharCount (const CharUtf8 *str, const usize length) __NE___
	{
		usize	count = 0;
		for (usize pos = 0; pos < length; ++count)
		{
			// to avoid infinite loop always add at least 1
			pos += Base::_hidden_::Utf8CharWidth( str, length, 1u );
		}
		return count;
	}

	ND_ forceinline usize  Utf8CharCount (BasicStringView<CharUtf8> str) __NE___
	{
		return Utf8CharCount( str.data(), str.length() );
	}

/*
=================================================
	Utf8Encode
=================================================
*/
	forceinline bool  Utf8Encode (const CharUtf32 symb, const usize dstSize, OUT CharUtf8 *dst, INOUT usize &pos) __NE___
	{
		return Base::_hidden_::Utf8Encode_V1( symb, dstSize, OUT dst, INOUT pos );
	}

/*
=================================================
	Utf32IsValid
=================================================
*/
	ND_ forceinline bool  Utf32IsValid (CharUtf32 c) __NE___
	{
		return utf8proc_codepoint_valid( c );
	}

/*
=================================================
	Utf32ToUpper
=================================================
*/
	ND_ forceinline CharUtf32  Utf32ToUpper (CharUtf32 c) __NE___
	{
		return utf8proc_toupper( c );
	}

	ND_ forceinline bool  Utf32IsUpper (CharUtf32 c) __NE___
	{
		return utf8proc_isupper( c ) == 1;
	}

/*
=================================================
	Utf32ToLower
=================================================
*/
	ND_ forceinline CharUtf32  Utf32ToLower (CharUtf32 c) __NE___
	{
		return utf8proc_tolower( c );
	}

/*
=================================================
	Utf32ToTitle
----
	When using title case, all words are capitalized, except for minor words
	(typically articles, short prepositions, and some conjunctions)
	that are not the first or last word of the title.
=================================================
*/
	ND_ forceinline CharUtf32  Utf32ToTitle (CharUtf32 c) __NE___
	{
		return utf8proc_totitle( c );
	}


} // AE::Base

#endif // AE_ENABLE_UTF8PROC
