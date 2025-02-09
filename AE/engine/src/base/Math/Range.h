// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Byte.h"

namespace AE::Base
{

	//
	// Range
	//

	template <typename T>
	struct Range
	{
		//StaticAssert( IsScalar<T> or IsBytes<T> );

	// types
	public:
		using Value_t	= T;
		using Self		= Range<T>;


	// variables
	public:
		T	begin, end;


	// methods
	public:
		__Cx__ Range ()											__NE___ : begin{}, end{}
		{
		  #ifdef AE_COMPILETIME_OFFSETOF
			// check if supported cast Rectangle to array
			StaticAssert( offsetof(Self, begin) + sizeof(T) == offsetof(Self, end) );
		  #endif
		}

		template <typename BT, typename ET>
		__Cx__ Range (BT begin, ET end)							__NE___ :
			begin{begin}, end{end}
		{}

		NdCx__ T		Offset ()								C_NE___	{ return begin; }
		NdCx__ T		Size ()									C_NE___	{ return end - begin; }
		NdCx__ T		Middle ()								C_NE___	{ return Average( begin, end ); }

		NdCx__ bool		IsEmpty ()								C_NE___	{ return begin == end; }
		NdCx__ bool		IsInvalid ()							C_NE___	{ return end < begin; }
		NdCx__ bool		IsValid ()								C_NE___	{ return not IsInvalid(); }

		NdCx__ bool		Contains (T value)						C_NE___	{ return (value >= begin) and (value < end); }

		NdCx__ static Self  Max ()								__NE___	{ return Self{ MinValue<T>(), MaxValue<T>() }; }

		NdCx__ static Self  From (const Vec<T,2> &v)			__NE___	{ return Self{ v.x, v.y }; }
		NdCx__ static Self  FromOffsetSize (T offset, T size)	__NE___	{ return Self{ offset, offset + size }; }
	};


	using RangeI	= Range<int>;
	using RangeU	= Range<uint>;
	using RangeF	= Range<float>;
	using RangeD	= Range<double>;
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< Range<T> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< Range<T> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< Range<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};
	template <typename T>	struct TUnwrap< Range<T> >					: TUnwrap<T> {};

} // AE::Base
