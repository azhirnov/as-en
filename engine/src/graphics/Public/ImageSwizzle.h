// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{
	
	//
	// Image Color Swizzle
	//

	struct ImageSwizzle
	{
	// variables
	private:
		ushort	_value	= 0x1234;	// 0 - unknown, 1 - R, 2 - G, 3 - B, 4 - A, 5 - O, 6 - 1, example: RGB0 - 0x1235


	// methods
	public:
		constexpr ImageSwizzle () __NE___ {}

		explicit ImageSwizzle (const uint4 &comp) __NE___ : _value{0}
		{
			ASSERT(All( comp < 7u ));

			_value |= (comp.x & 0xF) << 12;
			_value |= (comp.y & 0xF) << 8;
			_value |= (comp.z & 0xF) << 4;
			_value |= (comp.w & 0xF);
		}

		ND_ constexpr uint  Get ()		 C_NE___	{ return _value; }
		ND_ constexpr bool  IsDefault () C_NE___	{ return _value == ImageSwizzle().Get(); }

		ND_ uint4			ToVec ()	C_NE___
		{
			return uint4( (_value >> 12) & 0xF, (_value >> 8) & 0xF, (_value >> 4) & 0xF, _value & 0xF );
		}

		ND_ constexpr bool  operator == (const ImageSwizzle &rhs) C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool  operator >  (const ImageSwizzle &rhs) C_NE___	{ return _value >  rhs._value; }


		friend constexpr ImageSwizzle  operator "" _swizzle (const char *str, const usize len) __NE___;


	private:
		static constexpr uint  _CharToValue (char c) __NE___
		{
			return	c == 'r' or c == 'R'	? 1 :
					c == 'g' or c == 'G'	? 2 :
					c == 'b' or c == 'B'	? 3 :
					c == 'a' or c == 'A'	? 4 :
					c == '0'				? 5 :
					c == '1'				? 6 : 0;
		}
	};
	
	
/*
=================================================
	operator ""
=================================================
*/
	ND_ constexpr ImageSwizzle  operator "" _swizzle (const char *str, const usize len) __NE___
	{
		ASSERT( len > 0 and len <= 4 );

		ImageSwizzle	res;
		uint			sw = 0;

		for (usize i = 0; (i < len) and (i < 4); ++i)
		{
			const char	c = str[i];
			const uint	v = ImageSwizzle::_CharToValue( c );

			ASSERT( v != 0 );	// 'c' must be R, G, B, A, 0, 1
			sw |= (v << (3 - i) * 4);
		}
		res._value = CheckCast<ushort>(sw);
		return res;
	}

} // AE::Graphics


namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::ImageSwizzle >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::ImageSwizzle >	{ static constexpr bool  value = true; };

} // AE::Base


namespace std
{
	template <>
	struct hash< AE::Graphics::ImageSwizzle >
	{
		ND_ size_t  operator () (const AE::Graphics::ImageSwizzle &value) C_NE___
		{
			return size_t( value.Get() );
		}
	};

} // std
