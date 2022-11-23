// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"

namespace AE::Math
{

	//
	// Large Bool (Bool32)
	//

	struct Bool32
	{
	private:
		uint	_value	= 0;

	public:
		constexpr Bool32 ()				__NE___	{}
		constexpr Bool32 (bool val)		__NE___	: _value{uint(val)} {}

		ND_ constexpr operator bool ()	C_NE___	{ return _value != 0; }
	};

} // AE::Math
