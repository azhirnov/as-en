// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

/*
=================================================
	CT_IntLog2
=================================================
*/
namespace _hidden_
{
	template <typename T, T X, uint Bit>
	struct _IntLog2 {
		static const int	value = int((X >> Bit) != 0) + _IntLog2<T, X, Bit-1 >::value;
	};

	template <typename T, T X>
	struct _IntLog2< T, X, 0 > {
		static const int	value = 0;
	};

}	// _hidden_

	template <auto X>
	static constexpr int	CT_IntLog2 = (X ? Base::_hidden_::_IntLog2< decltype(X), X, sizeof(X)*8-1 >::value : -1);
	
/*
=================================================
	CT_IsPowerOfTwo
=================================================
*/
namespace _hidden_
{
	template <ulong X>
	struct _IsPowerOfTwo {
		static constexpr bool	value = (X != 0) & ((X & (X - 1)) == 0);
	};
}	// _hidden_

	template <auto X>
	static constexpr bool	CT_IsPowerOfTwo = _hidden_::_IsPowerOfTwo< ulong(X) >::value;
	
/*
=================================================
	CT_CeilIntLog2
=================================================
*/
namespace _hidden_
{
	template <auto X>
	struct _CeilIntLog2 {
		static constexpr int	il2		= CT_IntLog2<X>;
		static constexpr int	value	= il2 >= 0 ? il2 + int(not CT_IsPowerOfTwo<X>) : -1;
	};
}	// _hidden_

	template <auto X>
	static constexpr int	CT_CeilIntLog2 = _hidden_::_CeilIntLog2<X>::value;

/*
=================================================
	CT_Pow
=================================================
*/
	template <auto Power, typename T>
	inline constexpr T  CT_Pow (const T &base)
	{
		STATIC_ASSERT( IsInteger<T> and IsInteger<decltype(Power)> and Power >= 0 );

		if constexpr( Power == 0 )
		{
			Unused( base );
			return 1;
		}
		else
			return CT_Pow<Power-1>( base ) * base;
	}
	
/*
=================================================
	CT_SizeOfInBits
=================================================
*/
	template <typename T>
	static constexpr usize		CT_SizeOfInBits = sizeof(T) * 8;


}	// AE::Base
