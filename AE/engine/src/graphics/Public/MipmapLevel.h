// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Mipmap Level
	//

	struct MipmapLevel
	{
	// variables
	private:
		ushort		_value	= 0;


	// methods
	public:
		constexpr MipmapLevel ()									__NE___	{}

		template <typename T, ENABLEIF( IsUnsignedInteger<T> )>
		explicit constexpr MipmapLevel (T value)					__NE___	: _value( CheckCast<ushort>(value)) {}

		ND_ constexpr uint	Get ()									C_NE___	{ return _value; }

		ND_ constexpr static MipmapLevel	Max ()					__NE___	{ return MipmapLevel{ uint(MaxValue<decltype(_value)>()) }; }

		ND_ constexpr bool	operator == (const MipmapLevel &rhs)	C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool	operator != (const MipmapLevel &rhs)	C_NE___	{ return _value != rhs._value; }
		ND_ constexpr bool	operator >  (const MipmapLevel &rhs)	C_NE___	{ return _value >  rhs._value; }
		ND_ constexpr bool	operator <  (const MipmapLevel &rhs)	C_NE___	{ return _value <  rhs._value; }
		ND_ constexpr bool	operator >= (const MipmapLevel &rhs)	C_NE___	{ return _value >= rhs._value; }
		ND_ constexpr bool	operator <= (const MipmapLevel &rhs)	C_NE___	{ return _value <= rhs._value; }

		ND_ MipmapLevel		operator + (MipmapLevel rhs)			C_NE___	{ return MipmapLevel{ uint{_value} + rhs._value }; }
			MipmapLevel&	operator ++ ()							__NE___	{ ++_value;  return *this; }
	};


	ND_ inline constexpr MipmapLevel operator "" _mipmap (unsigned long long value) __NE___	{ return MipmapLevel( uint(value) ); }


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
