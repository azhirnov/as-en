// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

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
		constexpr THashVal ()									__NE___	{}
		explicit constexpr THashVal (T val)						__NE___ : _value{val} {}

		template <typename B>
		explicit constexpr THashVal (THashVal<B> h)				__NE___ : _value{static_cast<T>(B{h})} {}

		ND_ constexpr bool	operator == (const THashVal &rhs)	C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool	operator != (const THashVal &rhs)	C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool	operator >  (const THashVal &rhs)	C_NE___	{ return _value > rhs._value; }
		ND_ constexpr bool  operator <  (const THashVal &rhs)	C_NE___	{ return _value < rhs._value; }

		constexpr THashVal&  operator << (const THashVal &rhs)	__NE___	{ Append( rhs );  return *this; }
		constexpr THashVal&  operator += (const THashVal &rhs)	__NE___	{ Append( rhs );  return *this; }

		constexpr void  Append (const THashVal &rhs)			__NE___
		{
			const T	mask	= (sizeof(_value)*8 - 1);
			T		val		= rhs._value;
			T		shift	= 8;

			shift  &= mask;
			_value ^= (val << shift) | (val >> ( ~(shift-1) & mask ));
		}

		ND_ constexpr const THashVal<T>  operator + (const THashVal<T> &rhs) C_NE___
		{
			return THashVal<T>(*this) << rhs;
		}

		ND_ explicit constexpr operator T ()					C_NE___	{ return _value; }

		template <typename R>
		ND_ constexpr R  Cast ()								C_NE___
		{
			if constexpr( sizeof(R) >= sizeof(T) )
				return R(_value);
			else
			if constexpr( sizeof(R)*2 >= sizeof(T) )
			{
				constexpr usize	bits	= sizeof(T)*8 / 2;
				constexpr T		mask	= (T{1} << bits) - 1;

				return R( ((_value >> bits) ^ _value) & mask );
			}
			else
			if constexpr( sizeof(R)*4 >= sizeof(T) )
			{
				constexpr usize	bits	= sizeof(T)*8 / 4;
				constexpr T		mask	= (T{1} << bits) - 1;

				return R( ((_value >> bits*3) ^ (_value >> bits*2) ^ (_value >> bits) ^ _value) & mask );
			}
		}
	};

	using HashVal	= THashVal<usize>;
	using HashVal32	= THashVal<uint>;
	using HashVal64	= THashVal<ulong>;


	//
	// Hasher
	//

	template <typename T>
	struct DefaultHasher_CalcHash
	{
		ND_ usize  operator () (const T &key) C_NE___ {
			return usize(key.CalcHash());
		}
	};
	
	template <typename T>
	struct DefaultHasher_GetHash
	{
		ND_ usize  operator () (const T &key) C_NE___ {
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
	ND_ forceinline EnableIf<not IsFloatPoint<T>, HashVal>  HashOf (const T &value) __NE___
	{
		return HashVal( std::hash<T>()( value ));
	}

/*
=================================================
	HashOf (float)
=================================================
*/
	ND_ forceinline HashVal  HashOf (const float &value, uint ignoreMantissaBits = (23-10)) __NE___
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
	ND_ forceinline HashVal  HashOf (const double &value, uint ignoreMantissaBits = (52-10)) __NE___
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
	ND_ forceinline HashVal  HashOf (const void *ptr, usize sizeInBytes) __NE___
	{
		ASSERT( ptr != null and sizeInBytes );

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

} // AE::Base


namespace std
{
	template <>
	struct hash< AE::Base::HashVal >
	{
		ND_ size_t  operator () (const AE::Base::HashVal &value) C_NE___
		{
			return size_t(value);
		}
	};

	template <typename First, typename Second>
	struct hash< std::pair<First, Second> >
	{
		ND_ size_t  operator () (const std::pair<First, Second> &value) C_NE___
		{
			return size_t(AE::Base::HashOf( value.first ) + AE::Base::HashOf( value.second ));
		}
	};

} // std
