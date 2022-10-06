// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Log/Log.h"
#include "base/CompileTime/TypeTraits.h"

namespace AE::Base
{

	//
	// Hash Value
	//

	template <typename T>
	struct THashVal final
	{
	// variables
	private:
		T	_value	= 0;

	// methods
	public:
		constexpr THashVal () {}
		explicit constexpr THashVal (T val) : _value{val} {}

		ND_ constexpr bool	operator == (const THashVal &rhs)	const	{ return _value == rhs._value; }
		ND_ constexpr bool	operator != (const THashVal &rhs)	const	{ return not (*this == rhs); }
		ND_ constexpr bool	operator >  (const THashVal &rhs)	const	{ return _value > rhs._value; }
		ND_ constexpr bool  operator <  (const THashVal &rhs)	const	{ return _value < rhs._value; }

		constexpr THashVal&  operator << (const THashVal &rhs)
		{
			const T	mask	= (sizeof(_value)*8 - 1);
			T		val		= rhs._value;
			T		shift	= 8;

			shift  &= mask;
			_value ^= (val << shift) | (val >> ( ~(shift-1) & mask ));

			return *this;
		}

		ND_ constexpr const THashVal<T>  operator + (const THashVal<T> &rhs) const
		{
			return THashVal<T>(*this) << rhs;
		}

		ND_ explicit constexpr operator T () const	{ return _value; }
	};

	using HashVal	= THashVal<usize>;
	using HashVal32	= THashVal<uint>;


	//
	// Hasher
	//

	template <typename T>
	struct DefaultHasher_CalcHash
	{
		ND_ usize  operator () (const T &key) const {
			return usize(key.CalcHash());
		}
	};
	
	template <typename T>
	struct DefaultHasher_GetHash
	{
		ND_ usize  operator () (const T &key) const {
			return usize(key.GetHash());
		}
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	HashOf
=================================================
*/
	template <typename T>
	ND_ forceinline EnableIf<not IsFloatPoint<T>, HashVal>  HashOf (const T &value)
	{
		return HashVal( std::hash<T>()( value ));
	}

/*
=================================================
	HashOf (float)
=================================================
*/
	ND_ forceinline HashVal  HashOf (const float &value, uint ignoreMantissaBits = (23-10))
	{
		ASSERT( ignoreMantissaBits < 23 );
		uint	dst;
		std::memcpy( OUT &dst, &value, sizeof(dst) );
		dst &= ~((1 << ignoreMantissaBits)-1);
		return HashVal( std::hash<uint>()( dst ));
	}

/*
=================================================
	HashOf (double)
=================================================
*/
	ND_ forceinline HashVal  HashOf (const double &value, uint ignoreMantissaBits = (52-10))
	{
		ASSERT( ignoreMantissaBits < 52 );
		ulong	dst;
		std::memcpy( OUT &dst, &value, sizeof(dst) );
		dst &= ~((1 << ignoreMantissaBits)-1);
		return HashVal( std::hash<ulong>()( dst ));
	}

/*
=================================================
	HashOf (buffer)
----
	use private api to calculate hash of buffer
=================================================
*/
	ND_ forceinline HashVal  HashOf (const void *ptr, usize sizeInBytes)
	{
		ASSERT( ptr and sizeInBytes );

		# if defined(AE_HAS_HASHFN_HashArrayRepresentation)
			return HashVal{std::_Hash_array_representation( static_cast<const unsigned char*>(ptr), sizeInBytes )};

		#elif defined(AE_HAS_HASHFN_Murmur2OrCityhash)
			return HashVal{std::__murmur2_or_cityhash<usize>()( ptr, sizeInBytes )};

		#elif defined(AE_HAS_HASHFN_HashBytes)
			return HashVal{std::_Hash_bytes( ptr, sizeInBytes, 0 )};

		#else
			AE_COMPILATION_MESSAGE( "used fallback hash function" )
			const ubyte*	buf		= static_cast<const ubyte*>(ptr);
			HashVal			result;
			for (usize i = 0; i < sizeInBytes; ++i) {
				result << HashVal{buf[i]};
			}
			return result;
		#endif
	}

}	// AE::Base


namespace std
{
	template <>
	struct hash< AE::Base::HashVal >
	{
		ND_ size_t  operator () (const AE::Base::HashVal &value) const
		{
			return size_t(value);
		}
	};

	template <typename First, typename Second>
	struct hash< std::pair<First, Second> >
	{
		ND_ size_t  operator () (const std::pair<First, Second> &value) const
		{
			return size_t(AE::Base::HashOf( value.first ) + AE::Base::HashOf( value.second ));
		}
	};

}	// std
