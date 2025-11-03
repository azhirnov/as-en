// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Math/Vec.h"

namespace AE::Base
{
	template <uint UID>
	struct TVersion2;


	//
	// 3 component Version
	//
	template <uint UID>
	struct TVersion3
	{
	// types
		using Self	= TVersion3<UID>;


	// variables
		ushort		major	= 0;
		ushort		minor	= 0;
		uint		patch	= 0;


	// methods
		__Cx__ TVersion3 ()												__NE___ {}
		__Cx__ TVersion3 (uint maj, uint min, uint patch = 0)			__NE___ : major{CheckCast{maj}}, minor{CheckCast{min}}, patch{patch} {}
		__Cx__ explicit TVersion3 (const TVersion2<UID> &, uint path = 0)__NE___;

		__Cx__ TVersion3 (const Self &)									__NE___ = default;
		__Cx__ TVersion3 (Self &&)										__NE___ = default;
		__Cx__ Self&	operator =  (const Self &)						__NE___ = default;
		__Cx__ Self&	operator =  (Self &&)							__NE___ = default;

		NdCx__ bool  operator == (const Self &rhs)						C_NE___;
		NdCx__ bool  operator >  (const Self &rhs)						C_NE___;
		NdCx__ bool  operator >= (const Self &rhs)						C_NE___;

		NdCx__ bool  operator != (const Self &rhs)						C_NE___	{ return not (*this == rhs); }
		NdCx__ bool  operator <  (const Self &rhs)						C_NE___	{ return not (*this >= rhs); }
		NdCx__ bool  operator <= (const Self &rhs)						C_NE___	{ return not (*this >  rhs); }

		template <uint UID2>
		NdCx__ TVersion3<UID2>	Cast ()									C_NE___	{ return {major, minor, patch}; }

		template <typename T>
		NdCx__ auto				Cast ()									C_NE___	{ return TVersion3< T::GetUID() >{ major, minor, patch }; }

		NdCx__ static uint		GetUID ()								__NE___	{ return UID; }
		NdCx__ static Self		Max ()									__NE___	{ return {0xFFFFu, 0xFFFFu, ~0u}; }
		NdCx__ static Self		Min ()									__NE___	{ return {0, 0, 0}; }
	};



	//
	// 2 component Version
	//
	template <uint UID>
	struct TVersion2
	{
	// types
		using Self	= TVersion2<UID>;


	// variables
		ushort		major	= 0;
		ushort		minor	= 0;


	// methods
		__Cx__ TVersion2 ()									__NE___ {}
		__Cx__ TVersion2 (uint maj, uint min)				__NE___ : major{CheckCast{maj}}, minor{CheckCast{min}} {}
		__Cx__ explicit TVersion2 (const TVersion3<UID> &v)	__NE___	: major{v.major}, minor{v.minor} {}

		__Cx__ TVersion2 (const Self &)						__NE___ = default;
		__Cx__ TVersion2 (Self &&)							__NE___ = default;

		__Cx__ Self&	operator =  (const Self &)			__NE___ = default;
		__Cx__ Self&	operator =  (Self &&)				__NE___ = default;

		NdCx__ bool  operator == (const Self &rhs)			C_NE___;
		NdCx__ bool  operator >  (const Self &rhs)			C_NE___;
		NdCx__ bool  operator >= (const Self &rhs)			C_NE___;

		NdCx__ bool  operator != (const Self &rhs)			C_NE___	{ return not (*this == rhs); }
		NdCx__ bool  operator <  (const Self &rhs)			C_NE___	{ return not (*this >= rhs); }
		NdCx__ bool  operator <= (const Self &rhs)			C_NE___	{ return not (*this >  rhs); }

		template <uint UID2>
		NdCx__ TVersion2<UID2>	Cast ()						C_NE___	{ return {major, minor}; }

		template <typename T>
		NdCx__ auto				Cast ()						C_NE___	{ return TVersion2< T::GetUID() >{ major, minor }; }

		NdCx__ static uint		GetUID ()					__NE___	{ return UID; }
		NdCx__ static Self		Max ()						__NE___	{ return {0xFFFFu, 0xFFFFu}; }
		NdCx__ static Self		Min ()						__NE___	{ return {}; }

		NdCx__ uint				ToUInt ()					C_NE___	{ return (uint{major} << 16) | uint{minor}; }
		NdCx__ static Self		FromUInt (uint val)			__NE___	{ return Self{ val >> 16, val & 0xFFFF }; }

		NdCx__ uint				To100 ()					C_NE___	{ ASSERT( minor < 10 );  return (uint(major) * 100) + (uint(minor) * 10); }
		NdCx__ static Self		From100 (uint val)			__NE___	{ return Self{ val / 100, (val / 10) % 10 }; }

		NdCx__ uint				To10 ()						C_NE___	{ ASSERT( minor < 10 );  return (uint(major) * 10) + uint(minor); }
		NdCx__ static Self		From10 (uint val)			__NE___	{ return Self{ val / 10, val % 10 }; }

		NdCx__ uint				ToHex ()					C_NE___	{ ASSERT( minor < 0xF );  return (uint(major) << 4) | (uint(minor) & 0xF); }
		NdCx__ static Self		FromHex (uint val)			__NE___	{ return Self{ val >> 4, val & 0xF }; }

		NdCx__ float			ToFloat ()					C_NE___	{ ASSERT( minor < 100 );  return float(major) + (float(minor) * 0.01f); }
		NdCx__ static Self		FromFloat (float val)		__NE___	{ uint f = uint(Fract(val) * 100.f);  return Self{ uint(Floor(val)), f }; }
	};


	using Version2 = TVersion2<0>;
	using Version3 = TVersion3<0>;


	template <uint UID>
	__Cx__ TVersion3<UID>::TVersion3 (const TVersion2<UID> &v, uint patch) __NE___ :
		major{v.major}, minor{v.minor}, patch{patch}
	{}

	template <uint UID>
	__Cx__ bool  TVersion2<UID>::operator == (const TVersion2<UID> &rhs) C_NE___
	{
		return	major == rhs.major and
				minor == rhs.minor;
	}

	template <uint UID>
	__Cx__ bool  TVersion2<UID>::operator >  (const TVersion2<UID> &rhs) C_NE___
	{
		return	major != rhs.major ? major > rhs.major :
									 minor > rhs.minor;
	}

	template <uint UID>
	__Cx__ bool  TVersion2<UID>::operator >= (const TVersion2<UID> &rhs) C_NE___
	{
		return not (rhs > *this);
	}


	template <uint UID>
	__Cx__ bool  TVersion3<UID>::operator == (const TVersion3<UID> &rhs) C_NE___
	{
		return	major == rhs.major and
				minor == rhs.minor and
				patch == rhs.patch;
	}

	template <uint UID>
	__Cx__ bool  TVersion3<UID>::operator >  (const TVersion3<UID> &rhs) C_NE___
	{
		return	major != rhs.major	? major > rhs.major :
				minor != rhs.minor	? minor > rhs.minor :
									  patch > rhs.patch;
	}

	template <uint UID>
	__Cx__ bool  TVersion3<UID>::operator >= (const TVersion3<UID> &rhs) C_NE___
	{
		return not (rhs > *this);
	}


	template <uint UID>	NdCx__ bool  operator == (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs == TVersion2<UID>{rhs}; }
	template <uint UID>	NdCx__ bool  operator == (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} == rhs; }

	template <uint UID>	NdCx__ bool  operator != (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs != TVersion2<UID>{rhs}; }
	template <uint UID>	NdCx__ bool  operator != (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} != rhs; }

	template <uint UID>	NdCx__ bool  operator >  (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs > TVersion2<UID>{rhs}; }
	template <uint UID>	NdCx__ bool  operator >  (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} > rhs; }

	template <uint UID>	NdCx__ bool  operator <  (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs < TVersion2<UID>{rhs}; }
	template <uint UID>	NdCx__ bool  operator <  (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} < rhs; }

	template <uint UID>	NdCx__ bool  operator >= (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs >= TVersion2<UID>{rhs}; }
	template <uint UID>	NdCx__ bool  operator >= (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} >= rhs; }

	template <uint UID>	NdCx__ bool  operator <= (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs <= TVersion2<UID>{rhs}; }
	template <uint UID>	NdCx__ bool  operator <= (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} <= rhs; }

} // AE::Base


template <AE::uint UID>
struct std::hash< AE::Base::TVersion2<UID> >
{
	ND_ size_t  operator () (const AE::Base::TVersion2<UID> &value) C_NE___
	{
		using namespace AE::Base;
		HashVal	h;
		h << HashOf( value.major );
		h << HashOf( value.minor );
		return size_t{h};
	}
};

template <AE::uint UID>
struct std::hash< AE::Base::TVersion3<UID> >
{
	ND_ size_t  operator () (const AE::Base::TVersion3<UID> &value) C_NE___
	{
		using namespace AE::Base;
		HashVal	h;
		h << HashOf( value.major );
		h << HashOf( value.minor );
		h << HashOf( value.patch );
		return size_t{h};
	}
};
