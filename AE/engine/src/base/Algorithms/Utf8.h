// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TODO: use https://en.cppreference.com/w/cpp/locale/codecvt it x2 faster
*/

#pragma once

#include "base/Common.h"

namespace AE::Base
{
namespace _hidden_
{

/*
=================================================
	Utf8Decode (v2)
----
	1.5-2x faster than 'Utf8Decode_v1()'
=================================================
*/
	NdCxIF Tuple<CharUtf32, uint>  Utf8Decode_v2 (const CharUtf8 *str, const usize length) __NE___
	{
		const uint	u = uint(*str);

		if ( (u < 0b1000'0000) and (length >= 1) )
		{
			return Tuple{ CharUtf32( str[0] ), 1u };
		}

		if ( (u >= 0b1100'0000) and (u <= 0b1101'1111) and (length >= 2) )
		{
			return Tuple{ (CharUtf32( uint(str[0]) & 0b0001'1111 ) << 6) |
						   CharUtf32( uint(str[1]) & 0b0011'1111 ),
						  2u };
		}

		if ( (u >= 0b1100'0000) and (u <= 0b1110'1111) and (length >= 3) )
		{
			return Tuple{ (CharUtf32( uint(str[0]) & 0b0000'1111 ) << 12) |
						  (CharUtf32( uint(str[1]) & 0b0011'1111 ) <<  6) |
						   CharUtf32( uint(str[2]) & 0b0011'1111 ),
						  3u };
		}

		if ( (u >= 0b1110'0000) and (u <= 0b1111'0111) and (length >= 4) )
		{
			return Tuple{ (CharUtf32( uint(str[0]) & 0b0000'0111 ) << 18) |
						  (CharUtf32( uint(str[1]) & 0b0011'1111 ) << 12) |
						  (CharUtf32( uint(str[2]) & 0b0011'1111 ) <<  6) |
						   CharUtf32( uint(str[3]) & 0b0011'1111 ),
						   4u };
		}

		return Tuple{ UMax, 0u };
	}

/*
=================================================
	Utf8Encode (v2)
=================================================
*/
	NdCxIF uint  Utf8Encode_v2 (const CharUtf32 symb, const usize dstSize, OUT CharUtf8 *dst) __NE___
	{
		uint	u = uint{symb};

		if ( (u <= 0x7F) and (dstSize >= 1) )
		{
			*dst = CharUtf8(u);
			return 1;
		}

		if ( (u >= 0x80) and (u <= 0x7FF) and (dstSize >= 2) )
		{
			dst[0] = CharUtf8( ((u >> 6) & 0b0001'1111) | 0b1100'0000 );
			dst[1] = CharUtf8(  (u       & 0b0011'1111) | 0b1000'0000 );
			return 2;
		}

		if ( (u >= 0x800) and (u <= 0xFFFF) and (dstSize >= 3) )
		{
			dst[0] = CharUtf8( ((u >> 12) & 0b0000'1111) | 0b1110'0000 );
			dst[1] = CharUtf8( ((u >>  6) & 0b0011'1111) | 0b1000'0000 );
			dst[2] = CharUtf8(  (u        & 0b0011'1111) | 0b1000'0000 );
			return 3;
		}

		if ( (u >= 0x1'0000) and (u <= 0x10'FFFF) and (dstSize >= 4) )
		{
			dst[0] = CharUtf8( ((u >> 18) & 0b0000'0111) | 0b1111'0000 );
			dst[1] = CharUtf8( ((u >> 12) & 0b0011'1111) | 0b1000'0000 );
			dst[2] = CharUtf8( ((u >>  6) & 0b0011'1111) | 0b1000'0000 );
			dst[3] = CharUtf8(  (u        & 0b0011'1111) | 0b1000'0000 );
			return 4;
		}

		return 0;
	}

} // _hidden_

/*
=================================================
	Utf8CharWidth
=================================================
*/
	NdCxIF uint  Utf8CharWidth (const CharUtf8 *str, const usize length, const uint defaultWidth = 0) __NE___
	{
		const uint	u = uint(*str);

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
	Utf16CharWidth
=================================================
*/
	NdCxIF uint  Utf16CharWidth (const CharUtf16 *str, const usize length, const uint defaultWidth = 0) __NE___
	{
		const uint	u = uint(*str);

		if ( (u < 0xD800 or u >= 0xE000) and length >= 1 )
			return 1;

		if ( (u >= 0xD800 and u < 0xE000) and length >= 2 )
		{
			const uint	u2 = uint(str[1]);
			if ( u2 >= 0xDC00 and u2 <= 0xDFFF )
				return 2;
		}

		return defaultWidth;
	}

/*
=================================================
	Utf16Decode
=================================================
*/
	NdCxIF Tuple<CharUtf32, uint>  Utf16Decode (const CharUtf16 *str, const usize length) __NE___
	{
		const uint	u = uint(*str);

		if ( (u < 0xD800 or u >= 0xE000) and length >= 1 )
		{
			return Tuple{ CharUtf32( str[0] ), 1u };
		}

		if ( (u >= 0xD800 and u < 0xE000) and length >= 2 )
		{
			const uint	u2 = uint(str[1]);
			if ( u2 >= 0xDC00 and u2 <= 0xDFFF )
			{
				return Tuple{ CharUtf32( 0x1'0000 + (((u - 0xD800) << 10) | (u2 - 0xDC00)) ), 2u };
			}
		}

		return Tuple{ UMax, 0u };
	}

/*
=================================================
	Utf16Encode
=================================================
*/
	NdCxIF uint  Utf16Encode (const CharUtf32 symb, const usize dstSize, OUT CharUtf16 *dst) __NE___
	{
		if ( symb < 0xD800 and dstSize >= 1 )
		{
			dst[0] = CharUtf16(symb);
			return 1u;
		}

		if ( symb >= 0xE000 and symb <= 0xFFFF and dstSize >= 1 )
		{
			dst[0] = CharUtf16(symb);
			return 1u;
		}

		if ( symb >= 0x1'0000 and symb <= 0x20'FFFF and dstSize >= 2 )
		{
			CharUtf32	c = symb - 0x1'0000;

			dst[0] = CharUtf16( (c >> 10)   + 0xD800 );
			dst[1] = CharUtf16( (c & 0x3FF) + 0xDC00 );
			return 2u;
		}

		return 0;
	}

/*
=================================================
	IsUtf8BOM / SkipUtf8BOM
=================================================
*/
	Nd__IF bool  IsUtf8BOM (StringView str) __NE___
	{
		return	str.size() >= 3			and
				str[0] == char(0xEF)	and
				str[1] == char(0xBB)	and
				str[2] == char(0xBF);
	}

	____IF void  SkipUtf8BOM (INOUT StringView &str) __NE___
	{
		if ( IsUtf8BOM( str ))
			str = str.substr( 3 );
	}

} // AE::Base
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_UTF8PROC
# include "base/Algorithms/Cast.h"
# include "utf8proc.h"

# if UTF8PROC_VERSION_MAJOR != 2 or UTF8PROC_VERSION_MINOR != 11
#	pragma message( "required Utf8Proc 2.11" )
# endif

namespace AE::Base
{
namespace _hidden_
{
/*
=================================================
	Utf8Decode (v1)
=================================================
*/
	Nd__IF Tuple<CharUtf32, uint>  Utf8Decode_v1 (const CharUtf8 *str, const usize length) __NE___
	{
		StaticAssert( sizeof(utf8proc_uint8_t) == sizeof(*str) );

		utf8proc_int32_t	symb;
		utf8proc_ssize_t	res = utf8proc_iterate( Cast<utf8proc_uint8_t>(str), length, OUT &symb );

		return Tuple{ CharUtf32(symb), uint(res > 0 ? res : 0) };
	}

/*
=================================================
	Utf8Encode (v1)
=================================================
*/
	Nd__IF ssize  Utf8Encode_v1 (const CharUtf32 symb, const usize dstSize, OUT CharUtf8 *dst) __NE___
	{
		Unused( dstSize );
		StaticAssert( sizeof(utf8proc_uint8_t) == sizeof(*dst) );

		ssize	cnt = utf8proc_encode_char( symb, OUT Cast<utf8proc_uint8_t>(dst) );
		ASSERT( cnt > 0 and cnt <= 4 );

		return cnt;
	}

} // _hidden_
//-----------------------------------------------------------------------------



/*
=================================================
	Utf8Decode
=================================================
*/
	Nd__IF CharUtf32  Utf8Decode (const CharUtf8 *str, const usize length, INOUT usize &pos) __NE___
	{
		ASSERT( pos < length );
		auto [c, w] = Base::_hidden_::Utf8Decode_v1( str + pos, length - pos );
		pos += Max( w, 1u );
		ASSERT( c != UMax );
		return c;
	}

	Nd__IF CharUtf32  Utf8Decode (BasicStringView<CharUtf8> str, INOUT usize &pos) __NE___
	{
		return Utf8Decode( str.data(), str.length(), INOUT pos );
	}

/*
=================================================
	Utf8CharCount
=================================================
*/
	Nd__IF usize  Utf8CharCount (const CharUtf8 *str, const usize length) __NE___
	{
		usize	count = 0;
		for (usize pos = 0; pos < length; ++count)
		{
			// to avoid infinite loop always add at least 1
			pos += Utf8CharWidth( str, length, 1u );
		}
		return count;
	}

	Nd__IF usize  Utf8CharCount (BasicStringView<CharUtf8> str) __NE___
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
		ASSERT( pos+4 <= dstSize );

		auto	cnt = Base::_hidden_::Utf8Encode_v1( symb, dstSize - pos, OUT dst + pos );

		pos += (cnt > 0 ? cnt : 0);
		return cnt > 0;
	}

/*
=================================================
	Utf32IsValid
=================================================
*/
	Nd__IF bool  Utf32IsValid (CharUtf32 c) __NE___
	{
		return utf8proc_codepoint_valid( c );
	}

/*
=================================================
	Utf32ToUpper
=================================================
*/
	Nd__IF CharUtf32  Utf32ToUpper (CharUtf32 c) __NE___
	{
		return utf8proc_toupper( c );
	}

	Nd__IF bool  Utf32IsUpper (CharUtf32 c) __NE___
	{
		return utf8proc_isupper( c ) == 1;
	}

/*
=================================================
	Utf32ToLower
=================================================
*/
	Nd__IF CharUtf32  Utf32ToLower (CharUtf32 c) __NE___
	{
		return utf8proc_tolower( c );
	}

	Nd__IF bool  Utf32IsLower (CharUtf32 c) __NE___
	{
		return utf8proc_islower( c ) == 1;
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
	Nd__IF CharUtf32  Utf32ToTitle (CharUtf32 c) __NE___
	{
		return utf8proc_totitle( c );
	}

} // AE::Base

#endif // AE_ENABLE_UTF8PROC
