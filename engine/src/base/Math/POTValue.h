// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/CompileTime/Math.h"
#include "base/Math/Vec.h"

namespace AE::Math
{

	//
	// Power of 2 value
	//

	template <typename T>
	struct TPowerOf2Value
	{
		STATIC_ASSERT( IsUnsignedInteger<T> );

	// types
	public:
		using Self = TPowerOf2Value<T>;


	// variables
	private:
		T	_value	= 0;


	// methods
	public:
		constexpr TPowerOf2Value ()						__NE___	{}
		constexpr TPowerOf2Value (const Self &other)	__NE___	: _value{other._value} {}
		explicit constexpr TPowerOf2Value (T val)		__NE___	: _value{val} {}
		
		ND_ explicit constexpr operator sbyte ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<sbyte > - 1);  return static_cast<sbyte >(1u   << _value); }
		ND_ explicit constexpr operator sshort ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<sshort> - 1);  return static_cast<sshort>(1u   << _value); }
		ND_ explicit constexpr operator int ()			C_NE___	{ ASSERT(_value < CT_SizeOfInBits<int   > - 1);  return static_cast<int   >(1u   << _value); }
		ND_ explicit constexpr operator slong ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<slong > - 1);  return static_cast<slong >(1ull << _value); }

		ND_ explicit constexpr operator ubyte ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<ubyte >);  return static_cast<ubyte >(1u   << _value); }
		ND_ explicit constexpr operator ushort ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<ushort>);  return static_cast<ushort>(1u   << _value); }
		ND_ explicit constexpr operator uint ()			C_NE___	{ ASSERT(_value < CT_SizeOfInBits<uint  >);  return static_cast<uint  >(1u   << _value); }
		ND_ explicit constexpr operator ulong ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<ulong >);  return static_cast<ulong >(1ull << _value); }

	#ifdef AE_PLATFORM_APPLE
		ND_ explicit constexpr operator ssize ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<ssize>);  return static_cast<ssize>(ssize{1} << _value); }
		ND_ explicit constexpr operator usize ()		C_NE___	{ ASSERT(_value < CT_SizeOfInBits<usize>);  return static_cast<usize>(usize{1} << _value); }
	#endif
		
		template <typename IT>
		ND_ static constexpr Self  From (IT value)		__NE___	{ return Self{ T( IntLog2( value ))}; }

		ND_ constexpr Self		operator *  (Self rhs)	C_NE___	{ return Self{ _value + rhs._value }; }
		ND_ constexpr Self		operator /  (Self rhs)	C_NE___	{ ASSERT( _value >= rhs._value );  return Self{ _value - rhs._value }; }
		ND_ constexpr ulong		operator +  (Self rhs)	C_NE___	{ return ulong{*this} + ulong{rhs}; }
		ND_ constexpr Self&		operator << (uint rhs)			{ _value += T(rhs);  return *this; }

		ND_ constexpr bool		operator == (Self rhs)	C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool		operator != (Self rhs)	C_NE___	{ return _value != rhs._value; }
		ND_ constexpr bool		operator >  (Self rhs)	C_NE___	{ return _value >  rhs._value; }
		ND_ constexpr bool		operator <  (Self rhs)	C_NE___	{ return _value <  rhs._value; }
		ND_ constexpr bool		operator >= (Self rhs)	C_NE___	{ return _value >= rhs._value; }
		ND_ constexpr bool		operator <= (Self rhs)	C_NE___	{ return _value <= rhs._value; }

		ND_ constexpr Self		Max (Self rhs)			C_NE___	{ return Self{ Math::Max( _value, rhs._value )}; }
		ND_ constexpr Self		Min (Self rhs)			C_NE___	{ return Self{ Math::Min( _value, rhs._value )}; }

		ND_ constexpr T			Get ()					C_NE___	{ return _value; }

		template <typename IT>
		ND_ constexpr IT		BitMask ()				C_NE___	{ return _value < CT_SizeOfInBits<IT> ? (IT{1} << _value) - 1 : ~IT{0}; }
		
		template <typename IT>
		ND_ friend constexpr IT  operator * (const Self lhs, const IT rhs) __NE___
		{
			STATIC_ASSERT( IsInteger<IT> );
			ASSERT( lhs._value < CT_SizeOfInBits<IT> );
			return rhs << lhs._value;
		}

		template <typename IT>
		ND_ friend constexpr IT  operator * (const IT lhs, const Self rhs) __NE___
		{
			STATIC_ASSERT( IsInteger<IT> );
			ASSERT( rhs._value < CT_SizeOfInBits<IT> );
			return lhs << rhs._value;
		}
		
		template <typename IT>
		ND_ friend constexpr IT  operator / (const Self lhs, const IT rhs) __NE___
		{
			STATIC_ASSERT( IsInteger<IT> );
			ASSERT( lhs._value < CT_SizeOfInBits<IT> );
			return rhs >> lhs._value;
		}
		
		template <typename IT>
		ND_ friend constexpr IT  operator / (const IT lhs, const Self rhs) __NE___
		{
			STATIC_ASSERT( IsInteger<IT> );
			ASSERT( rhs._value < CT_SizeOfInBits<IT> );
			return lhs >> rhs._value;
		}
	};
	
	using POTValue = TPowerOf2Value< uint >;

	ND_ inline constexpr POTValue operator "" _pot (unsigned long long value) __NE___	{ return POTValue{ uint(value) }; }


} // AE::Math
