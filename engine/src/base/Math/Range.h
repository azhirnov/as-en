// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Bytes.h"

namespace AE::Math
{

	//
	// Packed Integer
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
		constexpr Range () : begin{T{0}}, end{T{0}}
		{
			// check if supported cast Rectangle to array
			STATIC_ASSERT( offsetof(Self, begin) + sizeof(T) == offsetof(Self, end) );
		}

		constexpr Range (T begin, T end) :
			begin{begin}, end{end}
		{}

		ND_ constexpr T		Offset ()		const	{ return begin; }
		ND_ constexpr T		Size ()			const	{ return end - begin; }
		ND_ constexpr T		Middle ()		const	{ return CalcAverage( begin, end ); }

		ND_ constexpr bool	IsEmpty ()		const	{ return begin == end; }
		ND_ constexpr bool	IsInvalid ()	const	{ return end < begin; }
		ND_ constexpr bool	IsValid ()		const	{ return not IsInvalid(); }

		ND_ constexpr static Self  Max ()								{ return Self{ MinValue<T>(), MaxValue<T>() }; }

		ND_ constexpr static Self  From (const Vec<T,2> &v)				{ return Self{ v.x, v.y }; }
		ND_ constexpr static Self  FromOffsetSize (T offset, T size)	{ return Self{ offset, offset + size }; }
	};


	using RangeI	= Range<int>;
	using RangeU	= Range<uint>;
	using RangeF	= Range<float>;
	using RangeD	= Range<double>;

	


} // AE::Math
