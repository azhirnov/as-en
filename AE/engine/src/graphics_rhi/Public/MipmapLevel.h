// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/Common.h"

namespace AE::Graphics
{

	//
	// Mipmap Level
	//

	struct MipmapLevel
	{
	// variables
	private:
		MipmapCount_t		_value	= 0;


	// methods
	public:
		__Cx__ MipmapLevel ()									__NE___	{}
		__Cx__ MipmapLevel (Zero_t)								__NE___	{}
		__Cx__ MipmapLevel (UMax_t)								__NE___ : _value{UMax} {}
		__Cx__ MipmapLevel (Default_t)							__NE___ {}
		__Cx__ MipmapLevel (const MipmapLevel &)				__NE___ = default;

		template <typename T> requires( IsInteger<T> )
		__Cx__ explicit MipmapLevel (T value)					__NE___	: _value( CheckCast<MipmapCount_t>(value)) {}

		NdCx__ uint	 Get ()										C_NE___	{ return _value; }

		NdCx__ static MipmapLevel	Max ()						__NE___	{ return MipmapLevel{ uint(MaxValue<decltype(_value)>()) }; }

		__Cx__ MipmapLevel&  operator = (Zero_t)				__NE___	{ _value = 0;		return *this; }
		__Cx__ MipmapLevel&  operator = (Default_t)				__NE___	{ _value = 0;		return *this; }
		__Cx__ MipmapLevel&  operator = (UMax_t)				__NE___	{ _value = UMax;	return *this; }
		__Cx__ MipmapLevel&  operator = (const MipmapLevel &)	__NE___	= default;

		NdCx__ bool	 operator == (const MipmapLevel &rhs)		C_NE___	{ return _value == rhs._value; }
		NdCx__ bool	 operator != (const MipmapLevel &rhs)		C_NE___	{ return _value != rhs._value; }
		NdCx__ bool	 operator >  (const MipmapLevel &rhs)		C_NE___	{ return _value >  rhs._value; }
		NdCx__ bool	 operator <  (const MipmapLevel &rhs)		C_NE___	{ return _value <  rhs._value; }
		NdCx__ bool	 operator >= (const MipmapLevel &rhs)		C_NE___	{ return _value >= rhs._value; }
		NdCx__ bool	 operator <= (const MipmapLevel &rhs)		C_NE___	{ return _value <= rhs._value; }

		ND_ MipmapLevel		operator + (MipmapLevel rhs)		C_NE___	{ return MipmapLevel{ uint{_value} + rhs._value }; }

			MipmapLevel&	operator ++ ()						__NE___	{ ++_value;  return *this; }
			MipmapLevel&	operator -- ()						__NE___	{ --_value;  return *this; }

			MipmapLevel		operator ++ (int)					__NE___	{ MipmapLevel prev = *this;  ++_value;  return prev; }
			MipmapLevel		operator -- (int)					__NE___	{ MipmapLevel prev = *this;  --_value;  return prev; }

		NdCx__ explicit operator MipmapCount_t ()				C_NE___	{ return _value; }
	};


	NdCxIn MipmapLevel operator ""_mipmap (unsigned long long value) __NE___	{ return MipmapLevel( value ); }


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::MipmapLevel >		: CT_True {};
	template <> struct TZeroMemAvailable< AE::Graphics::MipmapLevel >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::MipmapLevel >	: CT_True {};

} // AE::Base


template <>
struct std::hash< AE::Graphics::MipmapLevel >
{
	ND_ size_t  operator () (const AE::Graphics::MipmapLevel &value) C_NE___
	{
		return size_t(AE::Base::HashOf( value.Get() ));
	}
};
