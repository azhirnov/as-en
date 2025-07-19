// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Large Bool (Bool32)
	//

	struct Bool32
	{
	private:
		uint	_value	= 0;

	private:
		__Cx__ explicit Bool32 (uint val)				__NE___	: _value{val} {}
	public:
		__Cx__ Bool32 ()								__NE___	= default;
		__Cx__ Bool32 (bool val)						__NE___	: _value{uint(val)} {}

		__Cx__ Bool32 (Bool32 &&)						__NE___	= default;
		__Cx__ Bool32 (const Bool32 &)					__NE___	= default;

		__Cx__ Bool32&	operator = (Bool32 &&)			__NE___	= default;
		__Cx__ Bool32&	operator = (const Bool32 &)		__NE___	= default;
		__Cx__ Bool32&	operator = (bool rhs)			__NE___	{ _value = uint(rhs);  return *this; }

		NdCx__ Bool32	operator not ()					C_NE___	{ return Bool32{ ~_value }; }

		NdCx__ Bool32	operator | (Bool32 rhs)			C_NE___	{ return Bool32{ _value | rhs._value }; }
		NdCx__ Bool32	operator & (Bool32 rhs)			C_NE___	{ return Bool32{ _value & rhs._value }; }
		NdCx__ Bool32	operator ^ (Bool32 rhs)			C_NE___	{ return Bool32{ _value ^ rhs._value }; }

		NdCx__ Bool32	operator || (Bool32 rhs)		C_NE___	{ return Bool32{ _value || rhs._value }; }
		NdCx__ Bool32	operator && (Bool32 rhs)		C_NE___	{ return Bool32{ _value && rhs._value }; }

		__Cx__ Bool32&	operator |= (Bool32 rhs)		__NE___	{ _value |= rhs._value;  return *this; }
		__Cx__ Bool32&	operator &= (Bool32 rhs)		__NE___	{ _value &= rhs._value;  return *this; }
		__Cx__ Bool32&	operator ^= (Bool32 rhs)		__NE___	{ _value ^= rhs._value;  return *this; }

		NdCx__ operator bool ()							C_NE___	{ return _value != 0; }

		NdCx__ uint  ToBit ()							C_NE___	{ return _value & 1; }
	};


	template <>	struct TMemCopyAvailable< Bool32 >		: CT_True {};
	template <>	struct TZeroMemAvailable< Bool32 >		: CT_True {};
	template <>	struct TTriviallySerializable< Bool32 >	: CT_True {};
	
	template <typename T>
	static constexpr bool  IsBool = IsSame<T, bool> or IsSame<T, Bool32>;

} // AE::Base
