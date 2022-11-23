// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Bytes.h"

namespace AE::Math
{

	//
	// Range
	//

	template <typename T>
	struct Range
	{
		STATIC_ASSERT( IsScalar<T> or IsBytes<T> );

	// types
	public:
		using Value_t	= T;
		using Self		= Range<T>;


	// variables
	public:
		T	begin, end;


	// methods
	public:
		constexpr Range () __NE___ : begin{}, end{}
		{
			// check if supported cast Rectangle to array
			STATIC_ASSERT( offsetof(Self, begin) + sizeof(T) == offsetof(Self, end) );
		}

		constexpr Range (T begin, T end) __NE___ :
			begin{begin}, end{end}
		{}

		ND_ constexpr T		Offset ()		C_NE___	{ return begin; }
		ND_ constexpr T		Size ()			C_NE___	{ return end - begin; }
		ND_ constexpr T		Middle ()		C_NE___	{ return CalcAverage( begin, end ); }

		ND_ constexpr bool	IsEmpty ()		C_NE___	{ return begin == end; }
		ND_ constexpr bool	IsInvalid ()	C_NE___	{ return end < begin; }
		ND_ constexpr bool	IsValid ()		C_NE___	{ return not IsInvalid(); }

		ND_ constexpr static Self  Max ()	__NE___							{ return Self{ MinValue<T>(), MaxValue<T>() }; }

		ND_ constexpr static Self  From (const Vec<T,2> &v)			__NE___	{ return Self{ v.x, v.y }; }
		ND_ constexpr static Self  FromOffsetSize (T offset, T size)__NE___	{ return Self{ offset, offset + size }; }
	};


	using RangeI	= Range<int>;
	using RangeU	= Range<uint>;
	using RangeF	= Range<float>;
	using RangeD	= Range<double>;

	


} // AE::Math
