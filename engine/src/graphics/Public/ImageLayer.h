// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Image Array Layer
	//
	
	struct ImageLayer
	{
	// variables
	private:
		ushort		_value	= 0;


	// methods
	public:
		constexpr ImageLayer () {}
		explicit constexpr ImageLayer (uint value) : _value( CheckCast<ushort>(value)) {}

		ND_ constexpr uint	Get ()								const	{ return _value; }
		
		ND_ ImageLayer		operator + (const ImageLayer &rhs)	const	{ return ImageLayer{ Get() + rhs.Get() }; }

		ND_ constexpr bool	operator == (const ImageLayer &rhs) const	{ return _value == rhs._value; }
		ND_ constexpr bool	operator != (const ImageLayer &rhs) const	{ return _value != rhs._value; }
		ND_ constexpr bool	operator >  (const ImageLayer &rhs) const	{ return _value >  rhs._value; }
		ND_ constexpr bool	operator <  (const ImageLayer &rhs) const	{ return _value <  rhs._value; }
		ND_ constexpr bool	operator >= (const ImageLayer &rhs) const	{ return _value >= rhs._value; }
		ND_ constexpr bool	operator <= (const ImageLayer &rhs) const	{ return _value <= rhs._value; }

		ImageLayer&			operator *= (uint rhs)						{ _value = static_cast<ushort>(_value * rhs);  return *this; }
	};
	

	ND_ inline constexpr ImageLayer operator "" _layer (unsigned long long value)	{ return ImageLayer{ uint(value) }; }


}	// AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::ImageLayer >		{ static constexpr bool  value = true; };
	template <> struct TZeroMemAvailable< AE::Graphics::ImageLayer >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::ImageLayer >	{ static constexpr bool  value = true; };

}	// AE::Base

namespace std
{
	template <>
	struct hash< AE::Graphics::ImageLayer >
	{
		ND_ size_t  operator () (const AE::Graphics::ImageLayer &value) const
		{
			return size_t(AE::Base::HashOf( value.Get() ));
		}
	};

}	// std
