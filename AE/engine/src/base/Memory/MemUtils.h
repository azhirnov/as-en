// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base
{
	static constexpr inline Bytes	DefaultAllocatorAlign	{__STDCPP_DEFAULT_NEW_ALIGNMENT__};
	static constexpr inline Bytes	SmallAllocationSize		{4 << 10};		// 4 KiB
	static constexpr inline Bytes	LargeAllocationSize		{4 << 20};		// 4 MiB - large page size in Windows
	static constexpr inline Bytes	DefaultAllocationSize	= SmallAllocationSize;

/*
=================================================
	AddressOf
=================================================
*/
	template <typename T>
	NdCx__ exact_t  AddressOf (T &value) __NE___
	{
		return std::addressof( value );
	}

	template <typename T>
	NdCx__ exact_t  VoidAddressOf (T &value) __NE___
	{
		return Cast<void>( std::addressof( value ));
	}

/*
=================================================
	AddressDistance
=================================================
*/
	template <typename LT, typename RT>
	NdCx__ Bytes  AddressDistance (LT &lhs, RT &rhs) __NE___
	{
		return Bytes{ usize(AddressOf(lhs)) - usize(AddressOf(rhs)) };
	}

/*
=================================================
	DbgInitMem
=================================================
*/
#ifdef AE_DEBUG
	__CxIn void  DbgInitMem (OUT void* ptr, Bytes size) __NE___
	{
		if_not_consteval()
		{
			ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
			std::memset( OUT ptr, 0xCD, usize{size} );
		}
	}

	template <typename T>
	__CxIn void  DbgInitMem (OUT T& value) __NE___
	{
		DbgInitMem( OUT std::addressof(value), SizeOf<T> );
	}
#endif

/*
=================================================
	DbgFreeMem
=================================================
*/
#ifdef AE_DEBUG
	__CxIn void  DbgFreeMem (OUT void* ptr, Bytes size) __NE___
	{
		if_not_consteval()
		{
			ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
			std::memset( OUT ptr, 0xFE, usize{size} );
		}
	}

	template <typename T>
	__CxIn void  DbgFreeMem (OUT T& value) __NE___
	{
		DbgFreeMem( OUT std::addressof(value), SizeOf<T> );
	}
#endif

/*
=================================================
	PlacementNew
----
	copy-ctor may throw
	default-ctor & move-ctor should not throw
=================================================
*/
	template <typename T, typename ...Args>
	T*  PlacementNew (OUT void* ptr, Args&&... args)  NoExcept(IsNothrowCtor< T, Args... >)
	{
		NonNull( ptr );
		CheckPointerCast<T>( ptr );

		return ( ::new(ptr) T{ FwdArg<Args>(args)... });
	}

	template <typename T, typename ...Args>
	__Cx__ T*  PlacementNew (OUT T* ptr, Args&&... args)  NoExcept(IsNothrowCtor< T, Args... >)
	{
		NonNull( ptr );
		CheckPointerCast<T>( ptr );

		return std::construct_at( OUT ptr, FwdArg<Args>(args)... );
	}

/*
=================================================
	PlacementDelete
=================================================
*/
	template <typename T>
	__Cx__ void  PlacementDelete (INOUT T &val) __NE___
	{
		StaticAssert( std::is_nothrow_destructible_v<T> );
		val.~T();

		DEBUG_ONLY( DbgFreeMem( val ));
	}

/*
=================================================
	Reconstruct
=================================================
*/
	template <typename T, typename ...Args>
	constexpr T*  Reconstruct (INOUT T &value, Args&& ...args) __NE___
	{
		CheckPointerCast<T>( &value );

		StaticAssert( std::is_nothrow_destructible_v<T> );
		StaticAssert( IsConstructible< T, Args... >);

		if constexpr( IsNothrowCtor< T, Args... >)
		{
			value.~T();
			DEBUG_ONLY( DbgFreeMem( value ));

			return std::construct_at( OUT &value, FwdArg<Args>(args)... );
		}
		else
		{
			if_consteval()
			{
				value.~T();
				DEBUG_ONLY( DbgFreeMem( value ));

				return std::construct_at( OUT &value, FwdArg<Args>(args)... );
			}
			else
			{
				TRY{
					value.~T();
					DEBUG_ONLY( DbgFreeMem( value ));

					return std::construct_at( OUT &value, FwdArg<Args>(args)... );
				}
				CATCH_ALL(
					DBG_WARNING( "exception in ctor!" );
					DEBUG_ONLY( DbgFreeMem( value ));
					return null;
				);
			}
		}
	}

/*
=================================================
	MemCopyChecks, MemMoveChecks, ZeroMemChecks
=================================================
*/
namespace _hidden_
{
	forceinline void  MemCopyChecks (const void* dst, const void* src, Bytes size, uint ptrAlign = 0, uint sizeAlign = 0)
	{
		// spec: "If the objects overlap, the behavior is undefined."
		ASSERT( not IsIntersects<const void *>( dst, dst + size, src, src + size ));

		if ( ptrAlign != 0 )
		{
			ASSERT( CheckPointerAlignment( dst, ptrAlign ));
			ASSERT( CheckPointerAlignment( src, ptrAlign ));
			ASSERT( IsMultipleOf( size, ptrAlign ));
		}

		if ( sizeAlign != 0 )
			ASSERT( IsMultipleOf( size, sizeAlign ));

		Unused( dst, src, size, ptrAlign );
	}

	forceinline void  MemMoveChecks (const void* dst, const void* src, uint align = 0)
	{
		// spec: "The objects may overlap: copying takes place as if the characters were copied to a temporary character array
		//        and then the characters were copied from the array to dest."

		if ( align != 0 )
		{
			ASSERT( CheckPointerAlignment( dst, align ));
			ASSERT( CheckPointerAlignment( src, align ));
		}

		Unused( dst, src, align );
	}

	forceinline void  ZeroMemChecks (const void* dst, Bytes size, uint ptrAlign = 0, uint sizeAlign = 0)
	{
		if ( ptrAlign != 0 )
		{
			ASSERT( CheckPointerAlignment( dst, ptrAlign ));
			ASSERT( IsMultipleOf( size, ptrAlign ));
		}

		if ( sizeAlign != 0 )
			ASSERT( IsMultipleOf( size, sizeAlign ));

		Unused( dst, size, ptrAlign );
	}
}

/*
=================================================
	MemCopy
----
	memory must not intersects
=================================================
*/
	template <typename T1, typename T2>
	void  MemCopy (OUT T1 &dst, const T2 &src) __NE___
	{
		StaticAssert( sizeof(dst) >= sizeof(src) );
		StaticAssert( IsMemCopyAvailable<T1> );
		StaticAssert( IsMemCopyAvailable<T2> );
		StaticAssert( not IsConst<T1> );
		ASSERT( VoidAddressOf(dst) != VoidAddressOf(src) );

		std::memcpy( OUT std::addressof(dst), std::addressof(src), sizeof(src) );
	}

	inline void  MemCopy (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( dst, src, size );

		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		if_likely( dst != null and src != null and size != 0 )
		{
			std::memcpy( OUT dst, src, usize(size) );
		}
	}

	inline void  MemCopy (OUT void* dst, Bytes dstSize, const void* src, const Bytes srcSize) __NE___
	{
		ASSERT( srcSize <= dstSize );

		MemCopy( OUT dst, src, std::min( srcSize, dstSize ));
	}

	template <typename T>
	void  MemCopy (OUT T* dst, const T* src, const usize count) __NE___
	{
		StaticAssert( IsMemCopyAvailable<T> );
		Base::_hidden_::MemCopyChecks( dst, src, SizeOf<T>*count );

		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		if_likely( dst != null and src != null and count != 0 )
		{
			std::memcpy( OUT dst, src, sizeof(T)*count );
		}
	}

/*
=================================================
	MemMove
----
	memory may intersects
=================================================
*/
	inline void  MemMove (OUT void* dst, const void* src, Bytes size) __NE___
	{
		Base::_hidden_::MemMoveChecks( dst, src );

		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		if_likely( dst != null and src != null and size != 0 )
		{
			std::memmove( OUT dst, src, usize(size) );
		}
	}

	inline void  MemMove (OUT void* dst, Bytes dstSize, const void* src, Bytes srcSize) __NE___
	{
		ASSERT( srcSize <= dstSize );

		MemMove( OUT dst, src, std::min( srcSize, dstSize ));
	}

	template <typename T>
	void  MemMove (OUT T* dst, const T* src, const usize count) __NE___
	{
		StaticAssert( IsMemCopyAvailable<T> );
		Base::_hidden_::MemMoveChecks( dst, src );

		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		if_likely( dst != null and src != null and count != 0 )
		{
			std::memmove( OUT dst, src, sizeof(T)*count );
		}
	}

/*
=================================================
	ZeroMem
=================================================
*/
	template <typename T>
	__Cx__ void  UnsafeZeroMem (OUT T& value) __NE___
	{
	#ifdef AE_COMPILER_CLANG
		# pragma clang diagnostic push
		# pragma clang diagnostic ignored "-Wnontrivial-memcall"
		std::memset( OUT &value, 0, sizeof(value) );
		# pragma clang diagnostic pop
	#else
		std::memset( OUT &value, 0, sizeof(value) );
	#endif
	}

	template <typename T>
	__Cx__ void  ZeroMem (OUT T& value) __NE___
	{
		StaticAssert( IsZeroMemAvailable<T> );
		StaticAssert( not IsPointer<T> );

		UnsafeZeroMem( OUT value );
	}

	inline void  ZeroMem (OUT void* ptr, Bytes size) __NE___
	{
		if_likely( ptr != null and size > 0 )
		{
			std::memset( OUT ptr, 0, usize{size} );
		}
	}

	template <typename T>
	void  ZeroMem (OUT T* ptr, usize count) __NE___
	{
		StaticAssert( not IsVoid<T> );
		StaticAssert( IsZeroMemAvailable<T> );

		if_likely( ptr != null and count > 0 )
		{
			std::memset( OUT ptr, 0, sizeof(T) * count );
		}
	}

/*
=================================================
	SecureZeroMem
=================================================
*/
	template <typename T>
	void  SecureZeroMem (OUT T& value) __NE___
	{
		StaticAssert( IsZeroMemAvailable<T> );
		StaticAssert( not IsPointer<T> );

		SecureZeroMem( OUT &value, SizeOf<T> );
	}

	void  SecureZeroMem (OUT void* ptr, Bytes size) __NE___;	// native

	namespace _hidden_ {
		void  SecureZeroMemFallback (OUT void* ptr, Bytes size) __NE___;	// software
	}

/*
=================================================
	CopyCString
=================================================
*/
	template <usize S1, usize S2>
	void  CopyCString (OUT char (&dst)[S1], const char (&src)[S2]) __NE___
	{
		StaticAssert( S1 >= S2 );
	  #ifdef AE_COMPILER_MSVC
		Unused( ::strcpy_s( OUT dst, src ));
	  #else
		::strcpy( OUT dst, src );
	  #endif
	}

/*
=================================================
	MemEqual
=================================================
*/
	Nd__In bool  MemEqual (const void* lhs, const void* rhs, Bytes size) __NE___
	{
		NonNull( lhs );
		NonNull( rhs );
		return std::memcmp( lhs, rhs, usize(size) ) == 0;
	}

	template <typename T>
	ND_ bool  MemEqual (const T &lhs, const T &rhs) __NE___
	{
		return std::memcmp( &lhs, &rhs, sizeof(T) ) == 0;
	}

/*
=================================================
	MemLess
=================================================
*/
	Nd__In bool  MemLess (const void* lhs, const void* rhs, Bytes size) __NE___
	{
		NonNull( lhs );
		NonNull( rhs );
		return std::memcmp( lhs, rhs, usize(size) ) < 0;
	}

	template <typename T>
	ND_ bool  MemLess (const T &lhs, const T &rhs) __NE___
	{
		return std::memcmp( &lhs, &rhs, sizeof(T) ) < 0;
	}

/*
=================================================
	MemMismatch
----
	returns pointers to first mismatched byte
=================================================
*/
	Nd__In Pair<const void*, const void*>  MemMismatch (const void* lhs, const void* rhs, Bytes size) __NE___
	{
		NonNull( lhs );
		NonNull( rhs );

		// TODO: SIMD optimization
		const ubyte*	p0	= Cast<ubyte>(lhs);
		const ubyte*	p1	= Cast<ubyte>(rhs);
		const ubyte*	end	= p0 + size;

		for (; p0 < end; ++p0, ++p1)
		{
			if ( *p0 != *p1 )
				break;
		}
		return {p0, p1};
	}

} // AE::Base
