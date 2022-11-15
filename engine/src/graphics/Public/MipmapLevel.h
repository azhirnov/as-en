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
		constexpr MipmapLevel () {}

		template <typename T, typename = EnableIf<IsUnsignedInteger<T>> >
		explicit constexpr MipmapLevel (T value)					__NE___	: _value( CheckCast<ushort>(value)) {}

		ND_ constexpr uint	Get ()									C_NE___	{ return _value; }

		ND_ constexpr static MipmapLevel	Max ()					__NE___	{ return MipmapLevel{ uint(MaxValue<decltype(_value)>()) }; }
		
		ND_ constexpr bool	operator == (const MipmapLevel &rhs)	C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool	operator != (const MipmapLevel &rhs)	C_NE___	{ return _value != rhs._value; }
		ND_ constexpr bool	operator >  (const MipmapLevel &rhs)	C_NE___	{ return _value >  rhs._value; }
		ND_ constexpr bool	operator <  (const MipmapLevel &rhs)	C_NE___	{ return _value <  rhs._value; }
		ND_ constexpr bool	operator >= (const MipmapLevel &rhs)	C_NE___	{ return _value >= rhs._value; }
		ND_ constexpr bool	operator <= (const MipmapLevel &rhs)	C_NE___	{ return _value <= rhs._value; }
	};

	
	ND_ inline constexpr MipmapLevel operator "" _mipmap (unsigned long long value) __NE___	{ return MipmapLevel( uint(value) ); }


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::MipmapLevel >		{ static constexpr bool  value = true; };
	template <> struct TZeroMemAvailable< AE::Graphics::MipmapLevel >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::MipmapLevel >	{ static constexpr bool  value = true; };

} // AE::Base


namespace std
{
	template <>
	struct hash< AE::Graphics::MipmapLevel >
	{
		ND_ size_t  operator () (const AE::Graphics::MipmapLevel &value) C_NE___
		{
			return size_t(AE::Base::HashOf( value.Get() ));
		}
	};

} // std
