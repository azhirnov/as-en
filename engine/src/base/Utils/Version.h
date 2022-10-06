// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Algorithms/Hash.h"

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
		ushort	major	= 0;
		ushort	minor	= 0;
		uint	patch	= 0;

		constexpr TVersion3 () {}
		constexpr TVersion3 (uint maj, uint min, uint patch = 0) : major{CheckCast<ushort>(maj)}, minor{CheckCast<ushort>(min)}, patch{patch} {}
		explicit constexpr TVersion3 (const TVersion2<UID> &v, uint path = 0) : major{v.major}, minor{v.minor}, patch{patch} {}

		ND_ constexpr bool  operator == (const TVersion3<UID> &rhs) const;
		ND_ constexpr bool  operator >  (const TVersion3<UID> &rhs) const;
		ND_ constexpr bool  operator >= (const TVersion3<UID> &rhs) const;

		ND_ constexpr bool  operator != (const TVersion3<UID> &rhs) const		{ return not (*this == rhs); }
		ND_ constexpr bool  operator <  (const TVersion3<UID> &rhs) const		{ return not (*this >= rhs); }
		ND_ constexpr bool  operator <= (const TVersion3<UID> &rhs) const		{ return not (*this >  rhs); }

		template <ulong UID2>
		ND_ constexpr TVersion3<UID2>		Cast ()	const	{ return {major, minor, patch}; }
		
		template <typename T>
		ND_ constexpr auto					Cast ()	const	{ return TVersion3<T::GetUID()>{ major, minor, patch }; }

		ND_ constexpr static ulong			GetUID ()		{ return UID; }
		ND_ constexpr static TVersion3<UID>	Max ()			{ return {0xFFFFu, 0xFFFFu, ~0u}; }
		ND_ constexpr static TVersion3<UID>	Min ()			{ return {}; }
	};
	


	//
	// 2 component Version
	//
	template <ulong UID>
	struct TVersion2
	{
		ushort	major	= 0;
		ushort	minor	= 0;

		constexpr TVersion2 () {}
		constexpr TVersion2 (uint maj, uint min) : major{CheckCast<ushort>(maj)}, minor{CheckCast<ushort>(min)} {}
		explicit constexpr TVersion2 (const TVersion3<UID> &v) : major{v.major}, minor{v.minor} {}

		ND_ constexpr bool  operator == (const TVersion2<UID> &rhs) const;
		ND_ constexpr bool  operator >  (const TVersion2<UID> &rhs) const;
		ND_ constexpr bool  operator >= (const TVersion2<UID> &rhs) const;
		
		ND_ constexpr bool  operator != (const TVersion2<UID> &rhs) const		{ return not (*this == rhs); }
		ND_ constexpr bool  operator <  (const TVersion2<UID> &rhs) const		{ return not (*this >= rhs); }
		ND_ constexpr bool  operator <= (const TVersion2<UID> &rhs) const		{ return not (*this >  rhs); }
		
		template <ulong UID2>
		ND_ constexpr TVersion2<UID2>		Cast ()	const	{ return {major, minor}; }

		template <typename T>
		ND_ constexpr auto					Cast ()	const	{ return TVersion2<T::GetUID()>{ major, minor }; }
		
		ND_ constexpr static ulong			GetUID ()		{ return UID; }
		ND_ constexpr static TVersion2<UID>	Max ()			{ return {0xFFFFu, 0xFFFFu}; }
		ND_ constexpr static TVersion2<UID>	Min ()			{ return {}; }
	};
		

	using Version2 = TVersion2<0>;
	using Version3 = TVersion2<0>;

	
	template <ulong UID>
	inline constexpr bool  TVersion2<UID>::operator == (const TVersion2<UID> &rhs) const
	{
		return	major == rhs.major and
				minor == rhs.minor;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion2<UID>::operator >  (const TVersion2<UID> &rhs) const
	{
		return	major != rhs.major ? major > rhs.major :
									 minor > rhs.minor;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion2<UID>::operator >= (const TVersion2<UID> &rhs) const
	{
		return not (rhs > *this);
	}
	
	
	template <ulong UID>
	inline constexpr bool  TVersion3<UID>::operator == (const TVersion3<UID> &rhs) const
	{
		return	major == rhs.major and
				minor == rhs.minor and
				patch == rhs.patch;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion3<UID>::operator >  (const TVersion3<UID> &rhs) const
	{
		return	major != rhs.major	? major > rhs.major :
				minor != rhs.minor	? minor > rhs.minor :
									  patch > rhs.patch;
	}
	
	template <ulong UID>
	inline constexpr bool  TVersion3<UID>::operator >= (const TVersion3<UID> &rhs) const
	{
		return not (rhs > *this);
	}
	
	
	template <ulong UID>	ND_ constexpr bool  operator == (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs)	{ return lhs == TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator == (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs)	{ return TVersion2<UID>{lhs} == rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator != (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs)	{ return lhs != TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator != (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs)	{ return TVersion2<UID>{lhs} != rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator >  (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs)	{ return lhs > TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator >  (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs)	{ return TVersion2<UID>{lhs} > rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator <  (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs)	{ return lhs < TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator <  (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs)	{ return TVersion2<UID>{lhs} < rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator >= (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs)	{ return lhs >= TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator >= (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs)	{ return TVersion2<UID>{lhs} >= rhs; }
	
	template <ulong UID>	ND_ constexpr bool  operator <= (const TVersion2<UID> &lhs, const TVersion3<UID> &rhs)	{ return lhs <= TVersion2<UID>{rhs}; }
	template <ulong UID>	ND_ constexpr bool  operator <= (const TVersion3<UID> &lhs, const TVersion2<UID> &rhs)	{ return TVersion2<UID>{lhs} <= rhs; }

}	// AE::Base


namespace std
{
	
	template <AE::ulong UID>
	struct hash< AE::Base::TVersion2<UID> >
	{
		ND_ size_t  operator () (const AE::Base::TVersion2<UID> &value) const
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
		ND_ size_t  operator () (const AE::Base::TVersion3<UID> &value) const
		{
			using namespace AE::Base;
			HashVal	h;
			h << HashOf( value.major );
			h << HashOf( value.minor );
			h << HashOf( value.patch );
			return size_t{h};
		}
	};

}	// std
