#pragma once

#include "base/Algorithms/Cast.h"

#ifdef __cpp_lib_int_pow2
# if __cpp_lib_int_pow2 == 202002L
#	define AE_cpp_lib_int_pow2
# endif
#endif

namespace AE::Math
{

/*
=================================================
	ToNearUInt
=================================================
*/
	template <typename T>
	ND_ AE_INTRINSIC constexpr EnableIf<IsInteger<T> or IsEnum<T>, ToUnsignedInteger<T>>  ToNearUInt (const T value) __NE___
	{
		StaticAssert( IsScalarOrEnum<T> );
		StaticAssert( not IsFloatPoint<T> );
		StaticAssert( sizeof(value) <= sizeof(ToUnsignedInteger<T>) );

		return static_cast< ToUnsignedInteger<T> >( value );
	}

/*
=================================================
	ToNearInt
=================================================
*/
	template <typename T>
	ND_ AE_INTRINSIC constexpr EnableIf<IsInteger<T> or IsEnum<T>, ToSignedInteger<T>>  ToNearInt (const T value) __NE___
	{
		StaticAssert( IsScalarOrEnum<T> );
		StaticAssert( not IsFloatPoint<T> );
		StaticAssert( sizeof(value) <= sizeof(ToSignedInteger<T>) );

		return static_cast< ToSignedInteger<T> >( value );
	}

/*
=================================================
	AllBits
----
	returns 'true' if 'lhs' has ALL bits that presented in 'rhs'
=================================================
*/
	template <typename T1, typename T2,
			  ENABLEIF( IsScalarOrEnum< T1 > and IsScalarOrEnum< T2 >)
			 >
	ND_ constexpr bool  AllBits (const T1 lhs, const T2 rhs) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSameTypes<T1, T2> );
		//ASSERT( rhs != T2(0) );
		return ( ToNearUInt(lhs) & ToNearUInt(rhs) ) == ToNearUInt(rhs);
	}

	template <usize S>
	ND_ constexpr bool  AllBits (const BitSet<S> &lhs, const BitSet<S> &rhs) __NE___
	{
		if constexpr( S <= 32 )	return AllBits( lhs.to_ulong(), rhs.to_ulong );
		if constexpr( S <= 64 )	return AllBits( lhs.to_ullong(), rhs.to_ullong );
	}

/*
=================================================
	AllBits
=================================================
*/
	template <typename T1, typename T2, typename T3,
			  ENABLEIF( IsScalarOrEnum< T1 > and IsScalarOrEnum< T2 > and IsScalarOrEnum< T3 >)
			 >
	ND_ constexpr bool  AllBits (const T1 lhs, const T2 rhs, const T3 mask) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSameTypes<T1, T2> );
		StaticAssert( not (IsEnum<T1> and IsEnum<T3>) or IsSameTypes<T1, T3> );
		ASSERT( mask != T2(0) );

		return ( ToNearUInt(lhs) & ToNearUInt(mask) ) == ( ToNearUInt(rhs) & ToNearUInt(mask) );
	}

/*
=================================================
	AnyBits
----
	returns 'true' if 'lhs' has ANY bit that presented in 'rhs'
=================================================
*/
	template <typename T1, typename T2,
			  ENABLEIF( IsScalarOrEnum< T1 > and IsScalarOrEnum< T2 >)
			 >
	ND_ constexpr bool  AnyBits (const T1 lhs, const T2 rhs) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSameTypes<T1, T2> );
		//ASSERT( rhs != T2(0) );
		return !!( ToNearUInt(lhs) & ToNearUInt(rhs) );
	}

	template <usize S>
	ND_ constexpr bool  AnyBits (const BitSet<S> &lhs, const BitSet<S> &rhs) __NE___
	{
		if constexpr( S <= 32 )	return AnyBits( lhs.to_ulong(), rhs.to_ulong );
		if constexpr( S <= 64 )	return AnyBits( lhs.to_ullong(), rhs.to_ullong );
	}

/*
=================================================
	NoBits
----
	returns 'true' if 'lhs' and 'rhs' hasn't same bits.
	same as 'not AnyBits()'
=================================================
*/
	template <typename T1, typename T2,
			  ENABLEIF( IsScalarOrEnum< T1 > and IsScalarOrEnum< T2 >)
			 >
	ND_ constexpr bool  NoBits (const T1 lhs, const T2 rhs) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSameTypes<T1, T2> );
		//ASSERT( rhs != T2(0) );
		return !( ToNearUInt(lhs) & ToNearUInt(rhs) );
	}

/*
=================================================
	ExtractBit
----
	extract lowest non-zero bit.
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  ExtractBit (INOUT T& value) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

		using U = ToUnsignedInteger<T>;
		ASSERT( U(value) > 0 );

		const U	result = U(value) & ~(U(value) - U{1});
		value = T(U(value) & ~result);

		return T(result);
	}

	template <typename Dst, typename T>
	ND_ constexpr EnableIf<IsScalar<T>, Dst>  ExtractBit (INOUT T& value)
	{
		return static_cast<Dst>( ExtractBit( INOUT value ));
	}

/*
=================================================
	IsPowerOfTwo / IsSingleBitSet
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, bool>  IsPowerOfTwo (const T x) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

		using U = ToUnsignedInteger<T>;

	  #ifdef AE_cpp_lib_int_pow2
		return std::has_single_bit( U(x) );
	  #else
		U	val = U( x );
		return (val != U{0}) and ((val & (val - U{1})) == U{0});
	  #endif
	}

	template <typename T>
	ND_ constexpr bool  IsSingleBitSet (const T x) __NE___
	{
		return IsPowerOfTwo( x );
	}

/*
=================================================
	IntLog2 / GetPowerOfTwo / BitScanReverse / MSB
----
	returns < 0 if x == 0
	find high non-zero bit.
	0b010000100
	   ^
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, int>  IntLog2 (const T x) __NE___
	{
		StaticAssert( IsInteger<T> or IsEnum<T> );

		constexpr int	INVALID_INDEX = -1;

	  #ifdef AE_cpp_lib_int_pow2
		Unused( INVALID_INDEX );
		return int(std::bit_width( ToNearUInt( x ))) - 1;

	  #elif defined(AE_COMPILER_MSVC)
		unsigned long	index;

		if constexpr( sizeof(x) == 8 )
			return _BitScanReverse64( OUT &index, ulong(x) ) ? index : INVALID_INDEX;
		else
		if constexpr( sizeof(x) <= 4 )
			return _BitScanReverse( OUT &index, uint(x) ) ? index : INVALID_INDEX;

	  #elif defined(AE_COMPILER_GCC) or defined(AE_COMPILER_CLANG)
		if constexpr( sizeof(x) == 8 )
			return ulong(x) > 0 ? 63 - __builtin_clzll( ulong(x) ) : INVALID_INDEX;
		else
		if constexpr( sizeof(x) <= 4 )
			return uint(x) > 0 ? 31 - __builtin_clz( uint(x) ) : INVALID_INDEX;

	  #else
		#error add BitScanReverse implementation
	  #endif
	}

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, int>  BitScanReverse (const T x) __NE___
	{
		return IntLog2( x );
	}

/*
=================================================
	HighBit / HighZeroBit
----
	return 'UMax' if empty
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, uint>  HighBit (const T x) __NE___
	{
		return uint(IntLog2( x ));
	}

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, uint>  HighZeroBit (const T x) __NE___
	{
		return uint(HighBit( ~ToNearUInt( x )));
	}

/*
=================================================
	CeilIntLog2
----
	returns < 0 if x == 0
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, int>  CeilIntLog2 (const T x) __NE___
	{
		int	i = IntLog2( x );
		return i >= 0 ? i + int(not IsPowerOfTwo( x )) : MinValue<int>();
	}

/*
=================================================
	ExtractBitIndex
----
	extract index of lowest non-zero bit.
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, int>  ExtractBitIndex (INOUT T& value) __NE___
	{
		return IntLog2( ExtractBit( INOUT value ));
	}

	template <typename Dst, typename T>
	ND_ constexpr EnableIf<IsScalar<T>, Dst>  ExtractBitIndex (INOUT T& value) __NE___
	{
		return static_cast<Dst>( ExtractBitIndex( INOUT value ));
	}

/*
=================================================
	BitScanForward / LSB
----
	returns < 0 if x == 0
	find low non-zero bit.
	0b010000100
	        ^
=================================================
*/
	template <typename T>
	ND_ EnableIf<IsScalar<T>, int>  BitScanForward (const T x) __NE___
	{
		StaticAssert( IsInteger<T> or IsEnum<T> );

	#ifdef AE_COMPILER_MSVC
		constexpr int	INVALID_INDEX = -1;
		unsigned long	index;

		if constexpr( sizeof(x) == 8 )
			return _BitScanForward64( OUT &index, ulong(x) ) ? int(index) : INVALID_INDEX;
		else
		if constexpr( sizeof(x) <= 4 )
			return _BitScanForward( OUT &index, uint(x) ) ? int(index) : INVALID_INDEX;

	#elif defined(AE_COMPILER_GCC) or defined(AE_COMPILER_CLANG)
		if constexpr( sizeof(x) == 8 )
			return __builtin_ffsll( ulong(x) ) - 1;
		else
		if constexpr( sizeof(x) <= 4 )
			return __builtin_ffs( uint(x) ) - 1;

	#else
		#error add BitScanForward implementation
	#endif
	}

/*
=================================================
	LowBit / LowZeroBit
----
	return 'UMax' if empty
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, uint>  LowBit (const T x) __NE___
	{
		return BitScanForward( x );
	}

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, uint>  LowZeroBit (const T x) __NE___
	{
		return LowBit( ~ToNearUInt( x ));
	}

/*
=================================================
	IntLog10
----
	how many times X can be divided by 10.
	slow implementation!
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, uint>  IntLog10 (const T x) __NE___
	{
		StaticAssert( IsInteger<T> or IsEnum<T> );

		using U		= ToUnsignedInteger< T >;
		using NL	= std::numeric_limits< U >;
		StaticAssert( NL::is_specialized );

		const U		uval	= U(x);
		constexpr U	cnt		= U(NL::digits10);
		U			res		= 0;

		for (U i = 1, j = 10; i <= cnt; ++i, j *= 10)
		{
			res = (uval >= j) ? i : res;
		}
		return uint(res);
	}

/*
=================================================
	BitCount
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, usize>  BitCount (const T x) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

	  #ifdef __cpp_lib_bitops
		return usize( std::popcount( ToUnsignedInteger<T>(x) ));

	  #elif defined(AE_COMPILER_MSVC)
		// requires CPUInfo::POPCNT
	  # if AE_PLATFORM_BITS == 64
		if constexpr( sizeof(x) == 8 )
			return usize( __popcnt64( ulong(x) ));
		else
	  # endif
		if constexpr( sizeof(x) == 4 )
			return usize( __popcnt( uint(x) ));
		else
		if constexpr( sizeof(x) <= 2 )
			return usize( __popcnt16( ushort(x) ));

	  #else // clang & gcc
		if constexpr( sizeof(x) == 8 )
			return usize( std::bitset<64>{ ulong(x) }.count() );
		else
		if constexpr( sizeof(x) <= 4 )
			return usize( std::bitset<32>{ uint(x) }.count() );
	  #endif
	}

/*
=================================================
	CountLZero
----
	counts the number of consecutive 0 bits, starting from the most significant bit
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, usize>  CountLZero (const T x) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

	  #ifdef __cpp_lib_bitops
		return usize( std::countl_zero( ToUnsignedInteger<T>(x) ));
	  #else
		// TODO
	  #endif
	}

/*
=================================================
	CountLOne
----
	counts the number of consecutive 1 bits, starting from the most significant bit
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, usize>  CountLOne (const T x) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

	  #ifdef __cpp_lib_bitops
		return usize( std::countl_one( ToUnsignedInteger<T>(x) ));
	  #else
		// TODO
	  #endif
	}

/*
=================================================
	SafeLeftBitShift / SafeRightBitShift
----
	in specs:
	  For negative 'x' the behaviour of << is undefined (until C++20).
	  In any case, if the value of the right operand is negative or is greater or equal
	  to the number of bits in the promoted left operand, the behavior is undefined.
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  SafeLeftBitShift (const T x, const usize shift) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );
		ASSERT( x >= T(0) );

		return	shift >= CT_SizeofInBits(x) ?
					T(0) :
					T( ToNearUInt(x) << shift );
	}

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  SafeRightBitShift (const T x, const usize shift) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );
		ASSERT( x >= T(0) );

		return	shift >= CT_SizeofInBits(x) ?
					T(0) :
					T( ToNearUInt(x) >> shift );
	}

/*
=================================================
	BitRotateLeft
----
	from https://en.wikipedia.org/wiki/Circular_shift#Implementing_circular_shifts
=================================================
*/
	namespace _hidden_
	{
		template <typename T>
		constexpr T _BitRotateLeft (const T x, const usize shift) __NE___
		{
			constexpr usize	mask = (CT_SizeofInBits(x) - 1);
			return (x << (shift & mask)) | (x >> ( ~(shift-1) & mask ));
		}
	} // _hidden_

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  BitRotateLeft (const T x, const usize shift) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

	  #ifdef __cpp_lib_bitops
		return T( std::rotl( ToNearUInt(x), int(shift) ));

	  #elif defined(AE_COMPILER_MSVC)
		if constexpr( sizeof(x) > sizeof(uint) )
			return T( _rotl64( ulong(x), int(shift) ));
		else
			return T( _rotl( uint(x), int(shift) ));

	  #else // clang & gcc
		return T( Math::_hidden_::_BitRotateLeft( ToNearUInt(x), shift ));
	  #endif
	}

/*
=================================================
	BitRotateRight
----
	from https://en.wikipedia.org/wiki/Circular_shift#Implementing_circular_shifts
=================================================
*/
	namespace _hidden_
	{
		template <typename T>
		constexpr T _BitRotateRight (const T x, const usize shift) __NE___
		{
			constexpr usize	mask = (CT_SizeofInBits(x) - 1);
			return (x >> (shift & mask)) | (x << ( ~(shift-1) & mask ));
		}
	} // _hidden_

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  BitRotateRight (const T x, const usize shift) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

	  #ifdef __cpp_lib_bitops
		return T( std::rotr( ToNearUInt(x), int(shift) ));

	  #elif defined(AE_COMPILER_MSVC)
		if constexpr( sizeof(x) > sizeof(uint) )
			return T( _rotr64( ulong(x), int(shift) ));
		else
			return T( _rotr( uint(x), int(shift) ));

	  #else // clang & gcc
		return T( Math::_hidden_::_BitRotateRight( ToNearUInt(x), shift ));
	  #endif
	}

/*
=================================================
	ShuffleBitScan
----
	returns < 0 if x == 0
=================================================
*/
	template <typename T>
	ND_ EnableIf<IsScalar<T>, int>  ShuffleBitScan (const T x, const usize shuffle) __NE___
	{
		StaticAssert( IsInteger<T> or IsEnum<T> );
		ASSERT( shuffle >= 0 );

		int off = int(shuffle & (CT_SizeofInBits(x) - 1));
		int	idx = BitScanForward( BitRotateLeft( x, shuffle ));

		return	idx >= 0 ?
				(idx >= off ?
					idx - off :
					int(CT_SizeofInBits(x)) - (off - idx)) :
				-1;
	}

/*
=================================================
	ToBitMask
=================================================
*/
	template <typename R, typename T>
	ND_ constexpr EnableIf<IsUnsignedInteger<R>, R>  ToBitMask (const T count) __NE___
	{
		if constexpr( IsUnsignedInteger<T> )
		{
			return	count >= T(CT_SizeOfInBits<R>)	? ~R{0}
													: (R{1} << count) - 1;
		}
		else
		{
			return	count >= T(CT_SizeOfInBits<R>)	? ~R{0} :
					count <  T{0}					?  R{0} :
													  (R{1} << count) - 1;
		}
	}

	template <typename T>
	ND_ constexpr EnableIf<IsUnsignedInteger<T>, T>  ToBitMask (const usize firstBit, const usize count) __NE___
	{
		ASSERT( firstBit < CT_SizeOfInBits<T> );
		return SafeLeftBitShift( ToBitMask<T>( count ), firstBit );
	}

/*
=================================================
	HasBit
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsInteger<T> or IsEnum<T>, bool>  HasBit (const T x, const usize index) __NE___
	{
		using U = ToUnsignedInteger<T>;
		return (static_cast<U>(x) & (U{1} << index)) != 0;
	}

/*
=================================================
	SetBit
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsInteger<T> or IsEnum<T>, T>  SetBit (const T x, const bool bit, const usize index) __NE___
	{
		using U = ToUnsignedInteger<T>;
		if ( bit )
			return static_cast<T>( static_cast<U>(x) | (U{1} << index) );
		else
			return static_cast<T>( static_cast<U>(x) & ~(U{1} << index) );
	}

/*
=================================================
	ToBit
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsUnsignedInteger<T>, T>  ToBit (const usize index) __NE___
	{
		return T{1} << index;
	}

	template <typename T>
	ND_ constexpr EnableIf<IsUnsignedInteger<T>, T>  ToBit (const bool bit, const usize index) __NE___
	{
		return T{bit} << index;
	}

/*
=================================================
	ReadBits
=================================================
*
	template <typename T>
	ND_ constexpr EnableIf<IsUnsignedInteger<T>, T>  ReadBits (const T bits, usize offset, usize bitCount) __NE___
	{
		return (bits >> offset) & ((T{1} << bitCount) - 1);
	}

/*
=================================================
	ByteSwap
----
	Little Endian <-> Big Endian
=================================================
*/
	template <typename T>
	ND_ EnableIf<IsScalar<T>, T>  ByteSwap (const T x) __NE___
	{
		StaticAssert( IsEnum<T> or IsInteger<T> );

		if constexpr( sizeof(x) == 1 )
			return x;
		else

	  #ifdef __cpp_lib_byteswap
			return T( std::byteswap( ToNearUInt(x) ));

	  #elif defined(AE_COMPILER_MSVC)
		if constexpr( sizeof(x) == 2 )
			return T( _byteswap_ushort( ToNearUInt(x) ));
		else
		if constexpr( sizeof(x) == 4 )
			return T( _byteswap_ulong( ToNearUInt(x) ));
		else
		if constexpr( sizeof(x) == 8 )
			return T( _byteswap_uint64( ToNearUInt(x) ));

	  #elif defined(AE_COMPILER_GCC) or defined(AE_COMPILER_CLANG)
		if constexpr( sizeof(x) == 2 )
			return T(((ushort(x) & 0x00FF) << 8) | ((ushort(x) & 0xFF00) >> 8));
		else
		if constexpr( sizeof(x) == 4 )
			return T( __builtin_bswap32( ToNearUInt(x) ));
		else
		if constexpr( sizeof(x) == 8 )
			return T( __builtin_bswap64( ToNearUInt(x) ));
	  #else
		// will fail to compile
		return;
	  #endif
	}

/*
=================================================
	FloorPOT / CeilPOT
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  FloorPOT (const T x) __NE___
	{
		int	i = IntLog2( x );
		return i >= 0 ? (T{1} << i) : T{0};
	}

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  CeilPOT (const T x) __NE___
	{
		int	i = IntLog2( x );
		return i >= 0 ? (T{1} << (i + int(not IsPowerOfTwo( x )))) : T{0};
	}

/*
=================================================
	IsOdd / IsEven
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsInteger<T> or IsEnum<T>, bool>  IsOdd (const T x) __NE___
	{
		return ( x & T(1) ) == T(1);
	}

	template <typename T>
	ND_ constexpr EnableIf<IsInteger<T> or IsEnum<T>, bool>  IsEven (const T x) __NE___
	{
		return ( x & T(1) ) == T(0);
	}

/*
=================================================
	BitEqual
=================================================
*/
	enum class EnabledBitCount : uint {};

	template <typename T>
	ND_ constexpr EnableIf<IsFloatPoint<T>, bool>  BitEqual (const T lhs, const T rhs, const EnabledBitCount bitCount) __NE___
	{
		ASSERT( uint(bitCount) <= sizeof(T)*8 );

		using I = ToSignedInteger<T>;
		using U = ToUnsignedInteger<T>;

		I	a	= BitCast<I>( lhs );
		I	b	= BitCast<I>( rhs );
			a	= (a < I{0} ? MinValue<I>() - a : a);
			b	= (b < I{0} ? MinValue<I>() - b : b);
		U	dif	= U(std::abs( a - b ));
		U	ac	= U{1} << (sizeof(T)*8 - uint(bitCount));

		return dif < ac;
	}

	template <typename T>
	ND_ constexpr EnableIf<IsFloatPoint<T>, bool>  BitEqual (const T lhs, const T rhs) __NE___
	{
		if constexpr( IsSameTypes< T, float >)
			return BitEqual( lhs, rhs, EnabledBitCount(28) );
		else
		if constexpr( IsSameTypes< T, double >)
			return BitEqual( lhs, rhs, EnabledBitCount(50) );
	}


} // AE::Math
