// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Math/Byte.h"
#include "base/Math/Vec.h"

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

		// Contains at least 1 unit in specified position.
		NdCx__ bool		Contains (T pos)						C_NE___	{ return (pos >= begin) and (pos < end); }

		// Return 'true' if position is in range, even if range is empty.
		NdCx__ bool		ContainsOrEqual (T pos)					C_NE___	{ return pos == begin or Contains( pos ); }

		// Other range contains at least 1 shared unit.
		NdCx__ bool		Intersects (const Self &other)			C_NE___	{ return IsIntersects( begin, end, other.begin, other.end ); }

		NdCx__ static Self  Max ()								__NE___	{ return Self{ MinValue<T>(), MaxValue<T>() }; }

		NdCx__ static Self  From (const Vec<T,2> &v)			__NE___	{ return Self{ v.x, v.y }; }
		NdCx__ static Self  FromOffsetSize (T offset, T size)	__NE___	{ return Self{ offset, offset + size }; }
	};


	template <typename T>
	Range (T, T) -> Range<T>;


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
