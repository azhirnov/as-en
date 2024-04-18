// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Packed Bits
	//

	template <uint Bits, typename T>
	struct PackedBits
	{
		T	value;

		ND_ T*			operator -> ()		__NE___	{ return &value; }
		ND_ T const*	operator -> ()		C_NE___	{ return &value; }
	};


} // AE::Base