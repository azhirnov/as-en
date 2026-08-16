// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/Common.h"

namespace AE::Graphics
{

	//
	// Image Array Layer
	//

	struct ImageLayer
	{
	// variables
	private:
		LayerCount_t	_value	= 0;


	// methods
	public:
		__Cx__ ImageLayer ()									__NE___	{}
		__Cx__ ImageLayer (Zero_t)								__NE___	{}
		__Cx__ ImageLayer (Default_t)							__NE___ {}
		__Cx__ ImageLayer (const ImageLayer &)					__NE___ = default;

		template <typename T> requires( IsUnsignedInteger<T> )
		__Cx__ explicit ImageLayer (T value)					__NE___	: _value{ CheckCast{value}} {}

		NdCx__ uint	 Get ()										C_NE___	{ return _value; }

		__Cx__ ImageLayer&  operator = (Zero_t)					__NE___	{ _value = 0;		return *this; }
		__Cx__ ImageLayer&  operator = (Default_t)				__NE___	{ _value = 0;		return *this; }
		__Cx__ ImageLayer&  operator = (UMax_t)					__NE___	{ _value = UMax;	return *this; }
		__Cx__ ImageLayer&  operator = (const ImageLayer &)		__NE___	= default;

		NdCx__ bool	 operator == (const ImageLayer &rhs)		C_NE___	{ return _value == rhs._value; }
		NdCx__ bool	 operator != (const ImageLayer &rhs)		C_NE___	{ return _value != rhs._value; }
		NdCx__ bool	 operator >  (const ImageLayer &rhs)		C_NE___	{ return _value >  rhs._value; }
		NdCx__ bool	 operator <  (const ImageLayer &rhs)		C_NE___	{ return _value <  rhs._value; }
		NdCx__ bool	 operator >= (const ImageLayer &rhs)		C_NE___	{ return _value >= rhs._value; }
		NdCx__ bool	 operator <= (const ImageLayer &rhs)		C_NE___	{ return _value <= rhs._value; }

		ND_ ImageLayer		operator + (const ImageLayer &rhs)	C_NE___	{ return ImageLayer{ Get() + rhs.Get() }; }
			ImageLayer&		operator ++ ()						__NE___	{ ++_value;  return *this; }
			ImageLayer		operator ++ (int)					__NE___	{ ImageLayer prev = *this;  ++_value;  return prev; }

			ImageLayer&		operator *= (uint rhs)				__NE___	{ _value = CheckCast{_value * rhs};  return *this; }

		NdCx__ explicit operator LayerCount_t ()				C_NE___	{ return _value; }
	};


	NdCxIn ImageLayer operator ""_layer (unsigned long long value)	__NE___	{ return ImageLayer{ value }; }


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::ImageLayer >		: CT_True {};
	template <> struct TZeroMemAvailable< AE::Graphics::ImageLayer >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::ImageLayer >	: CT_True {};

} // AE::Base


template <>
struct std::hash< AE::Graphics::ImageLayer >
{
	ND_ size_t  operator () (const AE::Graphics::ImageLayer &value) C_NE___
	{
		return size_t(AE::Base::HashOf( value.Get() ));
	}
};
