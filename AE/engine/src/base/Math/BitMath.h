#pragma once

#include "base/Algorithms/Cast.h"

#ifdef __cpp_lib_int_pow2
# if __cpp_lib_int_pow2 >= 202002L
#	define AE_cpp_lib_int_pow2
# endif
#endif

template <typename T>
constexpr bool _enable_bitmask_operators (T);

namespace AE::Base
{

/*
=================================================
	ToNearUInt
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	ND_ AE_INTRINSIC constexpr ToUnsignedInteger<T>  ToNearUInt (const T value) __NE___
	{
		StaticAssert( sizeof(value) <= sizeof(ToUnsignedInteger<T>) );

		return static_cast< ToUnsignedInteger<T> >( value );
	}

/*
=================================================
	ToNearInt
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	ND_ AE_INTRINSIC constexpr ToSignedInteger<T>  ToNearInt (const T value) __NE___
	{
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
	template <typename T1, typename T2>
		requires(HasScalarBitOp<T1> and HasScalarBitOp<T2>)
	NdCx__ bool  AllBits (const T1 lhs, const T2 rhs) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSame<T1, T2> );
		//ASSERT( rhs != T2(0) );
		return ( ToNearUInt(lhs) & ToNearUInt(rhs) ) == ToNearUInt(rhs);
	}

	template <usize S>
	NdCx__ bool  AllBits (const BitSet<S> &lhs, const BitSet<S> &rhs) __NE___
	{
		if constexpr( S <= 32 )	return AllBits( lhs.to_ulong(), rhs.to_ulong );
		if constexpr( S <= 64 )	return AllBits( lhs.to_ullong(), rhs.to_ullong );
	}

/*
=================================================
	AllBits
=================================================
*/
	template <typename T1, typename T2, typename T3>
		requires(HasScalarBitOp<T1> and HasScalarBitOp<T2> and HasScalarBitOp<T3>)
	NdCx__ bool  AllBits (const T1 lhs, const T2 rhs, const T3 mask) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSame<T1, T2> );
		StaticAssert( not (IsEnum<T1> and IsEnum<T3>) or IsSame<T1, T3> );
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
	template <typename T1, typename T2>
		requires(HasScalarBitOp<T1> and HasScalarBitOp<T2>)
	NdCx__ bool  AnyBits (const T1 lhs, const T2 rhs) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSame<T1, T2> );
		//ASSERT( rhs != T2(0) );
		return !!( ToNearUInt(lhs) & ToNearUInt(rhs) );
	}

	template <usize S>
	NdCx__ bool  AnyBits (const BitSet<S> &lhs, const BitSet<S> &rhs) __NE___
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
	template <typename T1, typename T2>
		requires(HasScalarBitOp<T1> and HasScalarBitOp<T2>)
	NdCx__ bool  NoBits (const T1 lhs, const T2 rhs) __NE___
	{
		StaticAssert( not (IsEnum<T1> and IsEnum<T2>) or IsSame<T1, T2> );
		//ASSERT( rhs != T2(0) );
		return !( ToNearUInt(lhs) & ToNearUInt(rhs) );
	}

/*
=================================================
	ExtractBit
----
	extract lowest non-zero bit.
	returns zero if 'value' is zero.
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  ExtractBit (INOUT T& value) __NE___
	{
		using U = ToUnsignedInteger<T>;
		ASSERT( U(value) > 0 );

		const U	result = U(value) & ~(U(value) - U{1});
		value = T(U(value) & ~result);

		return T(result);
	}

	template <typename Dst, typename T> requires(HasScalarBitOp<T>)
	NdCx__ Dst  ExtractBit (INOUT T& value)
	{
		return static_cast<Dst>( ExtractBit( INOUT value ));
	}
	
/*
=================================================
	ExtractHighBit
----
	extract highest non-zero bit.
	returns zero if 'value' is zero.
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  ExtractHighBit (INOUT T& value) __NE___
	{
		using U = ToUnsignedInteger<T>;
		ASSERT( U(value) > 0 );

		const U	result = U(value) & ~(U(value) >> 1);
		value = T(U(value) & ~result);

		return T(result);
	}
	
	template <typename Dst, typename T> requires(HasScalarBitOp<T>)
	NdCx__ Dst  ExtractHighBit (INOUT T& value)
	{
		return static_cast<Dst>( ExtractHighBit( INOUT value ));
	}
	
/*
=================================================
	IsPowerOfTwo / IsPowerOf2 / IsSingleBitSet
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ bool  IsPowerOfTwo (const T x) __NE___
	{
		using U = ToUnsignedInteger<T>;

	  #ifdef AE_cpp_lib_int_pow2
		return std::has_single_bit( U(x) );
	  #else
		U	val = U( x );
		return (val != U{0}) and ((val & (val - U{1})) == U{0});
	  #endif
	}

	template <typename T>
	NdCx__ bool  IsSingleBitSet (const T x) __NE___
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
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ int  IntLog2 (const T x) __NE___
	{
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

	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ int  BitScanReverse (const T x) __NE___
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
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ uint  HighBit (const T x) __NE___
	{
		return uint(IntLog2( x ));
	}

	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ uint  HighZeroBit (const T x) __NE___
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
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ int  CeilIntLog2 (const T x) __NE___
	{
		int	i = IntLog2( x );
		return i >= 0 ? i + int(not IsPowerOfTwo( x )) : -1;
	}

/*
=================================================
	ExtractBitIndex
----
	extract index of lowest non-zero bit.
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ int  ExtractBitIndex (INOUT T& value) __NE___
	{
		return IntLog2( ExtractBit( INOUT value ));
	}

	template <typename Dst, typename T> requires(HasScalarBitOp<T>)
	NdCx__ Dst  ExtractBitIndex (INOUT T& value) __NE___
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
	template <typename T> requires(HasScalarBitOp<T>)
	ND_ int  BitScanForward (const T x) __NE___
	{
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
	CountRZero / LSB
----
	returns max_bit_count if x == 0
	find low non-zero bit.
	0b010000100
	        ^
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ uint  CountRZero (const T x) __NE___
	{
		return uint( std::countr_zero( ToUnsignedInteger<T>(x) ));
	}

/*
=================================================
	LowBit / LowZeroBit
----
	return 'UMax' if empty
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ uint  LowBit (const T x) __NE___
	{
		return BitScanForward( x );
	}

	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ uint  LowZeroBit (const T x) __NE___
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
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ uint  IntLog10 (const T x) __NE___
	{
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
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ usize  BitCount (const T x) __NE___
	{
		return usize( std::popcount( ToUnsignedInteger<T>(x) ));
	}

/*
=================================================
	CountLZero
----
	counts the number of consecutive 0 bits, starting from the most significant bit
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ usize  CountLZero (const T x) __NE___
	{
		return usize( std::countl_zero( ToUnsignedInteger<T>(x) ));
	}

/*
=================================================
	CountLOne
----
	counts the number of consecutive 1 bits, starting from the most significant bit
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ usize  CountLOne (const T x) __NE___
	{
		return usize( std::countl_one( ToUnsignedInteger<T>(x) ));
	}

/*
=================================================
	SafeLeftBitShift / SafeRightBitShift
----
	in specs:
	  For negative 'x' the behavior of << is undefined (until C++20).
	  In any case, if the value of the right operand is negative or is greater or equal
	  to the number of bits in the promoted left operand, the behavior is undefined.
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  SafeLeftBitShift (const T x, const usize shift) __NE___
	{
		ASSERT( x >= T(0) );
		return	shift >= CT_SizeofInBits(x) ?
					T(0) :
					T( ToNearUInt(x) << shift );
	}

	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  SafeRightBitShift (const T x, const usize shift) __NE___
	{
		ASSERT( x >= T(0) );
		return	shift >= CT_SizeofInBits(x) ?
					T(0) :
					T( ToNearUInt(x) >> shift );
	}

/*
=================================================
	BitRotateLeft
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  BitRotateLeft (const T x, const usize shift) __NE___
	{
		return T( std::rotl( ToNearUInt(x), int(shift) ));
	}

/*
=================================================
	BitRotateRight
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  BitRotateRight (const T x, const usize shift) __NE___
	{
		return T( std::rotr( ToNearUInt(x), int(shift) ));
	}

/*
=================================================
	ShuffleBitScan
----
	returns < 0 if x == 0
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	ND_ int  ShuffleBitScan (const T x, const usize shuffle) __NE___
	{
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
		requires(IsUnsignedInteger<R> and HasScalarBitOp<T>)
	NdCx__ R  ToBitMask (const T count) __NE___
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

	template <typename T> requires(IsUnsignedInteger<T>)
	NdCx__ T  ToBitMask (const usize firstBit, const usize count) __NE___
	{
		ASSERT( firstBit < CT_SizeOfInBits<T> );
		return SafeLeftBitShift( ToBitMask<T>( count ), firstBit );
	}

/*
=================================================
	HasBit
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ bool  HasBit (const T x, const usize index) __NE___
	{
		using U = ToUnsignedInteger<T>;
		return (static_cast<U>(x) & (U{1} << index)) != 0;
	}

	template <uint Index, typename T> requires(HasScalarBitOp<T>)
	NdCx__ bool  HasBit (const T x) __NE___
	{
		StaticAssert( Index < CT_SizeOfInBits<T> );
		return HasBit( x, Index );
	}

/*
=================================================
	SetBit
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  SetBit (const T x, const bool bit, const usize index) __NE___
	{
		using U = ToUnsignedInteger<T>;
		if ( bit )
			return static_cast<T>( static_cast<U>(x) | (U{1} << index) );
		else
			return static_cast<T>( static_cast<U>(x) & ~(U{1} << index) );
	}

	template <uint Index, typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  SetBit (const T x, const bool bit) __NE___
	{
		StaticAssert( Index < CT_SizeOfInBits<T> );
		SetBit( x, bit, Index );
	}

/*
=================================================
	ToBit
=================================================
*/
	template <typename T> requires(IsUnsignedInteger<T>)
	NdCx__ T  ToBit (const usize index) __NE___
	{
		return T{1} << index;
	}

	template <typename T> requires(IsUnsignedInteger<T>)
	NdCx__ T  ToBit (const bool bit, const usize index) __NE___
	{
		return T{bit} << index;
	}

/*
=================================================
	ReadBits
=================================================
*
	template <typename T> requires(IsUnsignedInteger<T>)
	NdCx__ T  ReadBits (const T bits, usize offset, usize bitCount) __NE___
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
	template <typename T> requires(HasScalarBitOp<T>)
	ND_ T  ByteSwap (const T x) __NE___
	{
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
	FloorPOT / CeilPOT / NearPOT
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  FloorPOT (const T x) __NE___
	{
		int	i = IntLog2( x );
		return i >= 0 ? (T{1} << i) : T{0};
	}

	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  CeilPOT (const T x) __NE___
	{
		int	i = IntLog2( x );
		return i >= 0 ? (T{1} << (i + int(not IsPowerOfTwo( x )))) : T{0};
	}
	
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ T  NearPOT (const T x) __NE___
	{
		int	i		= IntLog2( x );
		T	floor	= i >= 0 ? (T{1} << i) : T{0};
		T	ceil	= floor << 1;
		T	d0		= x - floor;
		T	d1		= ceil - x;
		return d0 < d1 ? floor : ceil;
	}

/*
=================================================
	IsOdd / IsEven
=================================================
*/
	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ bool  IsOdd (const T x) __NE___
	{
		return ( x & T(1) ) == T(1);
	}

	template <typename T> requires(HasScalarBitOp<T>)
	NdCx__ bool  IsEven (const T x) __NE___
	{
		return ( x & T(1) ) == T(0);
	}

/*
=================================================
	BitEqual
=================================================
*/
	enum class EnabledBitCount : uint {};

	template <typename T> requires(IsFloatPoint<T>)
	NdCx__ bool  BitEqual (const T lhs, const T rhs, const EnabledBitCount bitCount) __NE___
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

	template <typename T> requires(IsFloatPoint<T>)
	NdCx__ bool  BitEqual (const T lhs, const T rhs) __NE___
	{
		if constexpr( IsSame< T, float >)
			return BitEqual( lhs, rhs, EnabledBitCount(28) );
		else
		if constexpr( IsSame< T, double >)
			return BitEqual( lhs, rhs, EnabledBitCount(50) );
	}

/*
=================================================
	BitDiff
=================================================
*/
	template <typename T> requires(IsFloatPoint<T>)
	NdCx__ int  BitDiff (const T lhs, const T rhs) __NE___
	{
		using I = ToSignedInteger<T>;
		using U = ToUnsignedInteger<T>;

		I	a	= BitCast<I>( lhs );
		I	b	= BitCast<I>( rhs );
			a	= (a < I{0} ? MinValue<I>() - a : a);
			b	= (b < I{0} ? MinValue<I>() - b : b);
		U	dif	= U(std::abs( a - b ));

		return CeilIntLog2( dif );
	}

/*
=================================================
	bit operators for enum
=================================================
*/
namespace EnumBitOperators
{
	template <AllowEnumBitOps T>
	NdCx__ T	operator |  (T lhs, T rhs)			__NE___	{ return static_cast<T>( ToNearUInt(lhs) | ToNearUInt(rhs) ); }
	
	template <AllowEnumBitOps T>
	NdCx__ T	operator &  (T lhs, T rhs)			__NE___	{ return static_cast<T>( ToNearUInt(lhs) & ToNearUInt(rhs) ); }
	
	template <AllowEnumBitOps T>
	NdCx__ T	operator ^  (T lhs, T rhs)			__NE___	{ return static_cast<T>( ToNearUInt(lhs) ^ ToNearUInt(rhs) ); }

	template <AllowEnumBitOps T>
	__Cx__ T&	operator |= (INOUT T &lhs, T rhs)	__NE___	{ return lhs = static_cast<T>( ToNearUInt(lhs) | ToNearUInt(rhs) ); }
	
	template <AllowEnumBitOps T>
	__Cx__ T&	operator &= (INOUT T &lhs, T rhs)	__NE___	{ return lhs = static_cast<T>( ToNearUInt(lhs) & ToNearUInt(rhs) ); }
	
	template <AllowEnumBitOps T>
	__Cx__ T&	operator ^= (INOUT T &lhs, T rhs)	__NE___	{ return lhs = static_cast<T>( ToNearUInt(lhs) ^ ToNearUInt(rhs) ); }

	template <AllowEnumBitOps T>
	NdCx__ T	operator ~ (T lhs)					__NE___	{ return static_cast<T>( ~ToNearUInt(lhs)); }
	
	template <AllowEnumBitOps T>
	NdCx__ bool	operator ! (T lhs)					__NE___	{ return not ToNearUInt(lhs); }
}

#define ImportBitOperators \
	using AE::Base::EnumBitOperators::operator |; \
	using AE::Base::EnumBitOperators::operator &; \
	using AE::Base::EnumBitOperators::operator ^; \
	using AE::Base::EnumBitOperators::operator |=; \
	using AE::Base::EnumBitOperators::operator &=; \
	using AE::Base::EnumBitOperators::operator ^=; \
	using AE::Base::EnumBitOperators::operator ~; \
	using AE::Base::EnumBitOperators::operator !

	ImportBitOperators;
	
} // AE::Base
