#pragma once

#include "base/Algorithms/Cast.h"
#include <cstdlib>

#ifdef __has_include
# if __has_include(<bit>)
#	include <bit>
# endif
#endif

#ifdef AE_COMPILER_MSVC
# include <intrin.h>
# pragma intrinsic( _BitScanForward, _BitScanReverse )
# pragma intrinsic( __popcnt16, __popcnt )
# if AE_PLATFORM_BITS == 64
#	pragma intrinsic( _BitScanForward64, _BitScanReverse64 )
#	pragma intrinsic( __popcnt64 )
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
	ND_ forceinline constexpr ToUnsignedInteger<T>  ToNearUInt (const T &value)
	{
		STATIC_ASSERT( IsScalarOrEnum<T> );
		STATIC_ASSERT( sizeof(value) <= sizeof(ToUnsignedInteger<T>) );

		return static_cast< ToUnsignedInteger<T> >( value );
	}

/*
=================================================
	ToNearInt
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr ToSignedInteger<T>  ToNearInt (const T &value)
	{
		STATIC_ASSERT( IsScalarOrEnum<T> );
		STATIC_ASSERT( sizeof(value) <= sizeof(ToSignedInteger<T>) );

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
	ND_ forceinline constexpr bool  AllBits (const T1& lhs, const T2& rhs)
	{
		STATIC_ASSERT( IsScalarOrEnum< T1 > );
		STATIC_ASSERT( IsScalarOrEnum< T2 > );
		ASSERT( rhs != T2(0) );

		return ( ToNearUInt(lhs) & ToNearUInt(rhs) ) == ToNearUInt(rhs);
	}
	
/*
=================================================
	AllBits
=================================================
*/
	template <typename T1, typename T2, typename T3>
	ND_ forceinline constexpr bool  AllBits (const T1& lhs, const T2& rhs, const T3& mask)
	{
		STATIC_ASSERT( IsScalarOrEnum< T1 > );
		STATIC_ASSERT( IsScalarOrEnum< T2 > );
		STATIC_ASSERT( IsScalarOrEnum< T3 > );
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
	ND_ forceinline constexpr bool  AnyBits (const T1& lhs, const T2& rhs)
	{
		STATIC_ASSERT( IsScalarOrEnum< T1 > );
		STATIC_ASSERT( IsScalarOrEnum< T2 > );
		ASSERT( rhs != T2(0) );

		return !!( ToNearUInt(lhs) & ToNearUInt(rhs) );
	}
	
/*
=================================================
	ExtractBit
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr T  ExtractBit (INOUT T& value)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );
		
		using U = ToUnsignedInteger<T>;
		ASSERT( U(value) > 0 );

		const U	result = U(value) & ~(U(value) - U{1});
		value = T(U(value) & ~result);

		return T(result);
	}

	template <typename Dst, typename T>
	ND_ forceinline constexpr Dst  ExtractBit (INOUT T& value)
	{
		return static_cast<Dst>( ExtractBit( INOUT value ));
	}

/*
=================================================
	IsPowerOfTwo / IsSingleBit
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr bool  IsPowerOfTwo (const T &x)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );
		
		using U = ToUnsignedInteger<T>;

	#ifdef __cpp_lib_int_pow2
		return std::has_single_bit( U(x) );
	#else
		U	val = U( x );
		return (val != U{0}) & ((val & (val - U{1})) == U{0});
	#endif
	}
	
/*
=================================================
	IntLog2 / GetPowerOfTwo / BitScanReverse / MSB
----
	returns < 0 if x == 0
=================================================
*/
	template <typename T>
	ND_ forceinline int  IntLog2 (const T& x)
	{
		STATIC_ASSERT( IsInteger<T> or IsEnum<T> );

		constexpr int	INVALID_INDEX = MinValue<int>();

	#ifdef __cpp_lib_int_pow2
		auto	c = std::bit_width( ToNearUInt( x ));
		return c > 0 ? int(c) - 1 : INVALID_INDEX;

	#elif defined(AE_COMPILER_MSVC)
		unsigned long	index;
		
		if constexpr( sizeof(x) == 8 )
			return _BitScanReverse64( OUT &index, ulong(x) ) ? index : INVALID_INDEX;
		else
		if constexpr( sizeof(x) <= 4 )
			return _BitScanReverse( OUT &index, uint(x) ) ? index : INVALID_INDEX;
		
	#elif defined(AE_COMPILER_GCC) or defined(AE_COMPILER_CLANG)
		if constexpr( sizeof(x) == 8 )
			return ulong(x) > 0 ? (sizeof(x)*8)-1 - __builtin_clzll( ulong(x) ) : INVALID_INDEX;
		else
		if constexpr( sizeof(x) <= 4 )
			return uint(x) > 0 ? (sizeof(x)*8)-1 - __builtin_clz( uint(x) ) : INVALID_INDEX;

	#else
		#error add BitScanReverse implementation
	#endif
	}

	template <typename T>
	ND_ forceinline int  BitScanReverse (const T& x)
	{
		return IntLog2( x );
	}
	
/*
=================================================
	CeilIntLog2
----
	returns < 0 if x == 0
=================================================
*/
	template <typename T>
	ND_ forceinline int  CeilIntLog2 (const T& x)
	{
		int	i = IntLog2( x );
		return i >= 0 ? i + int(not IsPowerOfTwo( x )) : MinValue<int>();
	}

/*
=================================================
	ExtractBitLog2
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr int  ExtractBitLog2 (INOUT T& value)
	{
		return IntLog2( ExtractBit( INOUT value ));
	}
	
	template <typename Dst, typename T>
	ND_ forceinline constexpr Dst  ExtractBitLog2 (INOUT T& value)
	{
		return static_cast<Dst>( ExtractBitLog2( INOUT value ));
	}
	
/*
=================================================
	BitScanForward / LSB
----
	returns < 0 if x == 0
=================================================
*/
	template <typename T>
	ND_ forceinline int  BitScanForward (const T& x)
	{
		STATIC_ASSERT( IsInteger<T> or IsEnum<T> );

	#ifdef AE_COMPILER_MSVC
		constexpr int	INVALID_INDEX = MinValue<int>();
		unsigned long	index;
		
		if constexpr( sizeof(x) == 8 )
			return _BitScanForward64( OUT &index, ulong(x) ) ? index : INVALID_INDEX;
		else
		if constexpr( sizeof(x) <= 4 )
			return _BitScanForward( OUT &index, uint(x) ) ? index : INVALID_INDEX;
		
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
	BitCount
=================================================
*/
	template <typename T>
	ND_ forceinline usize  BitCount (const T& x)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );
		
	#ifdef __cpp_lib_bitops
		return usize( std::popcount( ToUnsignedInteger<T>(x) ));

	#elif defined(AE_COMPILER_MSVC)
		// requires CPUInfo::POPCNT
	  #if AE_PLATFORM_BITS == 64
		if constexpr( sizeof(x) == 8 )
			return usize( __popcnt64( ulong(x) ));
		else
	  #endif
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
	SafeLeftBitShift
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr T  SafeLeftBitShift (const T& x, usize shift)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );
		
		return T( ToNearUInt(x) << (shift & (sizeof(x)*8 - 1)) );
	}
	
/*
=================================================
	SafeRightBitShift
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr T  SafeRightBitShift (const T& x, usize shift)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );

		return T( ToNearUInt(x) >> (shift & (sizeof(x)*8 - 1)) );
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
		forceinline constexpr T _BitRotateLeft (T value, usize shift)
		{
			const usize	mask = (sizeof(value)*8 - 1);

			shift &= mask;
			return (value << shift) | (value >> ( ~(shift-1) & mask ));
		}
	}	// _hidden_
	
	template <typename T>
	ND_ forceinline constexpr T  BitRotateLeft (const T& x, usize shift)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );
		
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
		forceinline constexpr T _BitRotateRight (T value, usize shift)
		{
			const usize	mask = (sizeof(value)*8 - 1);

			shift &= mask;
			return (value >> shift) | (value << ( ~(shift-1) & mask ));
		}
	}	// _hidden_

	template <typename T>
	ND_ forceinline constexpr T  BitRotateRight (const T& x, usize shift)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );
		
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
	ToBitMask
=================================================
*/
	template <typename R, typename T>
	ND_ forceinline constexpr EnableIf< IsUnsignedInteger<R>, R >  ToBitMask (T count)
	{
		if constexpr( IsUnsignedInteger<T> )
		{
			return	count >= T(sizeof(R)*8)	? ~R{0} :
											  (R{1} << count) - 1;
		}
		else
		{
			return	count >= T(sizeof(R)*8)		? ~R{0} :
					count <  T{0}				?  R{0} :
												  (R{1} << count) - 1;
		}
	}
	
	template <typename T>
	ND_ forceinline constexpr EnableIf< IsUnsignedInteger<T>, T >  ToBitMask (usize firstBit, usize count)
	{
		ASSERT( firstBit < sizeof(T)*8 );
		return SafeLeftBitShift( ToBitMask<T>( count ), firstBit );
	}
	
/*
=================================================
	ByteSwap
----
	Little Endian <-> Big Endian
=================================================
*/
	template <typename T>
	ND_ forceinline T  ByteSwap (const T &x)
	{
		STATIC_ASSERT( IsEnum<T> or IsInteger<T> );

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
	ND_ forceinline T  FloorPOT (const T x)
	{
		int	i = IntLog2( x );
		return i >= 0 ? (T{1} << i) : T{0};
	}

	template <typename T>
	ND_ forceinline T  CeilPOT (const T x)
	{
		int	i = IntLog2( x );
		return i >= 0 ? (T{1} << (i + int(not IsPowerOfTwo( x )))) : T{0};
	}


}	// AE::Math
