// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Math/Math.h"

namespace AE::Base
{
	template <ulong UID>
	struct TVersion2;


	//
	// 3 component Version
	//
	template <ulong UID>
	struct TVersion3
	{
	// types
		using Self	= TVersion3<UID>;
		
	// variables
		ushort	major	= 0;
		ushort	minor	= 0;
		uint	patch	= 0;
		
	// methods
		constexpr TVersion3 ()												__NE___ {}
		constexpr TVersion3 (uint maj, uint min, uint patch = 0)			__NE___ : major{CheckCast<ushort>(maj)}, minor{CheckCast<ushort>(min)}, patch{patch} {}
		constexpr TVersion3 (const Self &v)									__NE___ = default;
		constexpr explicit TVersion3 (const TVersion2<UID> &, uint path = 0)__NE___;

		ND_ constexpr bool  operator == (const Self &rhs)					C_NE___;
		ND_ constexpr bool  operator >  (const Self &rhs)					C_NE___;
		ND_ constexpr bool  operator >= (const Self &rhs)					C_NE___;

		ND_ constexpr bool  operator != (const Self &rhs)					C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool  operator <  (const Self &rhs)					C_NE___	{ return not (*this >= rhs); }
		ND_ constexpr bool  operator <= (const Self &rhs)					C_NE___	{ return not (*this >  rhs); }

		template <ulong UID2>
		ND_ constexpr TVersion3<UID2>	Cast ()								C_NE___	{ return {major, minor, patch}; }
		
		template <typename T>
		ND_ constexpr auto				Cast ()								C_NE___	{ return TVersion3< T::GetUID() >{ major, minor, patch }; }

		ND_ constexpr static ulong		GetUID ()							__NE___	{ return UID; }
		ND_ constexpr static Self		Max ()								__NE___	{ return {0xFFFFu, 0xFFFFu, ~0u}; }
		ND_ constexpr static Self		Min ()								__NE___	{ return {0, 0, 0}; }
	};
	


	//
	// 2 component Version
	//
	template <ulong UID>
	struct TVersion2
	{
	// types
		using Self	= TVersion2<UID>;

	// variables
		ushort	major	= 0;
		ushort	minor	= 0;

	// methods
		constexpr TVersion2 ()									__NE___ {}
		constexpr TVersion2 (uint maj, uint min)				__NE___ : major{CheckCast<ushort>(maj)}, minor{CheckCast<ushort>(min)} {}
		constexpr TVersion2 (const Self &v)						__NE___ = default;
		constexpr explicit TVersion2 (const TVersion3<UID> &v)	__NE___	: major{v.major}, minor{v.minor} {}

		ND_ constexpr bool  operator == (const Self &rhs)		C_NE___;
		ND_ constexpr bool  operator >  (const Self &rhs)		C_NE___;
		ND_ constexpr bool  operator >= (const Self &rhs)		C_NE___;
		
		ND_ constexpr bool  operator != (const Self &rhs)		C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool  operator <  (const Self &rhs)		C_NE___	{ return not (*this >= rhs); }
		ND_ constexpr bool  operator <= (const Self &rhs)		C_NE___	{ return not (*this >  rhs); }
		
		template <ulong UID2>
		ND_ constexpr TVersion2<UID2>	Cast ()					C_NE___	{ return {major, minor}; }

		template <typename T>
		ND_ constexpr auto				Cast ()					C_NE___	{ return TVersion2< T::GetUID() >{ major, minor }; }
		
		ND_ constexpr static ulong		GetUID ()				__NE___	{ return UID; }
		ND_ constexpr static Self		Max ()					__NE___	{ return {0xFFFFu, 0xFFFFu}; }
		ND_ constexpr static Self		Min ()					__NE___	{ return {}; }

		ND_ constexpr uint				To100 ()				C_NE___	{ ASSERT( minor < 10 );  return (uint(major) * 100) + (uint(minor) * 10); }
		ND_ constexpr static Self		From100 (uint val)		__NE___	{ return Self{ val / 100, (val / 10) % 10 }; }
		
		ND_ constexpr uint				To10 ()					C_NE___	{ ASSERT( minor < 10 );  return (uint(major) * 10) + uint(minor); }
		ND_ constexpr static Self		From10 (uint val)		__NE___	{ return Self{ val / 10, val % 10 }; }

		ND_ constexpr uint				ToHex ()				C_NE___	{ ASSERT( minor < 0xF );  return (uint(major) << 4) | (uint(minor) & 0xF); }
		ND_ constexpr static Self		FromHex (uint val)		__NE___	{ return Self{ val >> 4, val & 0xF }; }

		ND_ constexpr float				ToFloat ()				C_NE___	{ ASSERT( minor < 100 );  return float(major) + (float(minor) * 0.01f); }
		ND_ constexpr static Self		FromFloat (float val)	__NE___	{ uint f = uint(Fract(val) * 100.f);  return Self{ uint(Floor(val)), f }; }
	};
		

	using Version2 = TVersion2<0>;
	using Version3 = TVersion3<0>;

	
	template <ulong UID>
	constexpr TVersion3<UID>::TVersion3 (const TVersion2<UID> &v, uint path) __NE___ :
		major{v.major}, minor{v.minor}, patch{patch}
	{}

	template <ulong UID>
	inline constexpr bool  TVersion2<UID>::operator == (const TVersion2<UID> &rhs) C_NE___
	{
		return	major == rhs.major and
				minor == rhs.minor;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion2<UID>::operator >  (const TVersion2<UID> &rhs) C_NE___
	{
		return	major != rhs.major ? major > rhs.major :
									 minor > rhs.minor;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion2<UID>::operator >= (const TVersion2<UID> &rhs) C_NE___
	{
		return not (rhs > *this);
	}
	
	
	template <ulong UID>
	inline constexpr bool  TVersion3<UID>::operator == (const TVersion3<UID> &rhs) C_NE___
	{
		return	major == rhs.major and
				minor == rhs.minor and
				patch == rhs.patch;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion3<UID>::operator >  (const TVersion3<UID> &rhs) C_NE___
	{
		return	major != rhs.major	? major > rhs.major :
				minor != rhs.minor	? minor > rhs.minor :
									  patch > rhs.patch;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion3<UID>::operator >= (const TVersion3<UID> &rhs) C_NE___
	{
		return not (rhs > *this);
	}
	
	
	template <ulong UID>	ND_ constexpr bool  operator == (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs == TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator == (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} == rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator != (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs != TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator != (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} != rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator >  (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs > TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator >  (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} > rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator <  (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs < TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator <  (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} < rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator >= (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs >= TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator >= (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} >= rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator <= (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs) __NE___	{ return lhs <= TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator <= (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs) __NE___	{ return TVersion2<UID>{lhs} <= rhs; }

} // AE::Base


namespace std
{
	
	template <AE::ulong UID>
	struct hash< AE::Base::TVersion2<UID> >
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
	
	template <AE::ulong UID>
	struct hash< AE::Base::TVersion3<UID> >
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

} // std
