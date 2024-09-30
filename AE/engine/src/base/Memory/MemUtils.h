// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base
{
	static constexpr inline Bytes	DefaultAllocatorAlign	{__STDCPP_DEFAULT_NEW_ALIGNMENT__};
	static constexpr inline Bytes	SmallAllocationSize		{4 << 10};		// 4 Kb
	static constexpr inline Bytes	LargeAllocationSize		{4 << 20};		// 4 Mb - large page size in WIndows
	static constexpr inline Bytes	DefaultAllocationSize	= SmallAllocationSize;

/*
=================================================
	AddressOf
=================================================
*/
	template <typename T>
	ND_ constexpr exact_t  AddressOf (T &value) __NE___
	{
		return std::addressof( value );
	}

	template <typename T>
	ND_ constexpr exact_t  VAddressOf (T &value) __NE___
	{
		return Cast<void>( std::addressof( value ));
	}

/*
=================================================
	AddressDistance
=================================================
*/
	template <typename LT, typename RT>
	ND_ constexpr Bytes  AddressDistance (LT &lhs, RT &rhs) __NE___
	{
		return Bytes{ usize(AddressOf(lhs)) - usize(AddressOf(rhs)) };
	}

/*
=================================================
	DbgInitMem
=================================================
*/
#ifdef AE_DEBUG
	inline void  DbgInitMem (OUT void* ptr, Bytes size) __NE___
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
		std::memset( OUT ptr, 0xCD, usize{size} );
	}

	template <typename T>
	void  DbgInitMem (OUT T& value) __NE___
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
	inline void  DbgFreeMem (OUT void* ptr, Bytes size) __NE___
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
		std::memset( OUT ptr, 0xFE, usize{size} );
	}

	template <typename T>
	void  DbgFreeMem (OUT T& value) __NE___
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

		return ( new(ptr) T{ FwdArg<Args>(args)... });
	}

	template <typename T, typename ...Args>
	constexpr T*  PlacementNew (OUT T* ptr, Args&&... args)  NoExcept(IsNothrowCtor< T, Args... >)
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
	constexpr void  PlacementDelete (INOUT T &val) __NE___
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
	constexpr void  Reconstruct (INOUT T &value, Args&& ...args) __NE___
	{
		CheckPointerCast<T>( &value );

		StaticAssert( std::is_nothrow_destructible_v<T> );
		StaticAssert( IsConstructible< T, Args... >);

		if constexpr( IsNothrowCtor< T, Args... > or IsConstEvaluated() )
		{
			value.~T();
			DEBUG_ONLY( DbgFreeMem( value ));

			std::construct_at( OUT &value, FwdArg<Args>(args)... );
		}
		else
		{
			TRY{
				value.~T();
				DEBUG_ONLY( DbgFreeMem( value ));

				std::construct_at( OUT &value, FwdArg<Args>(args)... );
			}
			CATCH_ALL(
				DBG_WARNING( "exception in ctor!" );
				DEBUG_ONLY( DbgFreeMem( value ));
			);
		}
	}

/*
=================================================
	MemCopy
----
	memory must not intersects,
	null pointers are not allowed
=================================================
*/
namespace _hidden_
{
	forceinline void  MemCopyChecks (const void* dst, const void* src, Bytes size, uint align = 0)
	{
		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		NonNull( dst );
		NonNull( src );

		// spec: "If the objects overlap, the behavior is undefined."
		ASSERT( not IsIntersects<const void *>( dst, dst + size, src, src + size ));

		if ( align != 0 )
		{
			ASSERT( CheckPointerAlignment( dst, align ));
			ASSERT( CheckPointerAlignment( src, align ));
		}

		Unused( dst, src, size, align );
	}
}

	template <typename T1, typename T2>
	void  MemCopy (OUT T1 &dst, const T2 &src) __NE___
	{
		StaticAssert( sizeof(dst) >= sizeof(src) );
		StaticAssert( IsMemCopyAvailable<T1> );
		StaticAssert( IsMemCopyAvailable<T2> );
		StaticAssert( not IsConst<T1> );
		ASSERT( VAddressOf(dst) != VAddressOf(src) );

		std::memcpy( OUT std::addressof(dst), std::addressof(src), sizeof(src) );
	}

	inline void  MemCopy (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( dst, src, size );
		std::memcpy( OUT dst, src, usize(size) );
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

		std::memcpy( OUT dst, src, sizeof(T)*count );
	}

/*
=================================================
	MemCopy_NullCheck
----
	memory must not intersects,
	null pointers are allowed
=================================================
*/
	forceinline void  MemCopy_NullCheck (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		// spec: "If the objects overlap, the behavior is undefined."
		ASSERT( not IsIntersects<const void *>( dst, dst + size, src, src + size ));

		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		if_likely( dst != null and src != null and size != 0 )
		{
			std::memcpy( OUT dst, src, usize(size) );
		}
	}

	template <typename T>
	void  MemCopy_NullCheck (OUT T* dst, const T* src, const usize count) __NE___
	{
		StaticAssert( IsMemCopyAvailable<T> );

		// spec: "If the objects overlap, the behavior is undefined."
		ASSERT( not IsIntersects<const void *>( dst, dst + count, src, src + count ));

		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		if_likely( dst != null and src != null and count != 0 )
		{
			std::memcpy( OUT dst, src, sizeof(T)*count );
		}
	}

/*
=================================================
	MemCopy16 / MemCopy32 / MemCopy64
----
	memory must not intersects
	memory must be aligned to 16/32/64 bytes
=================================================
*/
	inline void  MemCopy16 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( dst, src, size, 16 );
		std::memcpy( OUT AssumeAligned<16>(dst), AssumeAligned<16>(src), usize(size) );	// TODO: SSE/Neon
	}

	inline void  MemCopy32 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( dst, src, size, 32 );
		std::memcpy( OUT AssumeAligned<32>(dst), AssumeAligned<32>(src), usize(size) );	// TODO: SSE/Neon
	}

	inline void  MemCopy64 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( dst, src, size, 64 );
		std::memcpy( OUT AssumeAligned<64>(dst), AssumeAligned<64>(src), usize(size) );	// TODO: SSE/Neon
	}

/*
=================================================
	MemMove
----
	memory may intersects,
	null pointers are not allowed
=================================================
*/
namespace _hidden_
{
	forceinline void  MemMoveChecks (const void* dst, const void* src, uint align = 0)
	{
		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		NonNull( dst );
		NonNull( src );

		// spec: "The objects may overlap: copying takes place as if the characters were copied to a temporary character array
		//        and then the characters were copied from the array to dest."

		if ( align != 0 )
		{
			ASSERT( CheckPointerAlignment( dst, align ));
			ASSERT( CheckPointerAlignment( src, align ));
		}

		Unused( dst, src, align );
	}
}
	inline void  MemMove (OUT void* dst, const void* src, Bytes size) __NE___
	{
		Base::_hidden_::MemMoveChecks( dst, src );
		std::memmove( OUT dst, src, usize(size) );
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

		std::memmove( OUT dst, src, sizeof(T)*count );
	}

/*
=================================================
	MemMove_NullCheck
----
	memory may intersects,
	null pointers are allowed
=================================================
*/
	inline void  MemMove_NullCheck (OUT void* dst, const void* src, Bytes size) __NE___
	{
		// spec: "If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero."
		if_likely( dst != null and src != null and size != 0 )
		{
			std::memmove( OUT dst, src, usize(size) );
		}
	}

	template <typename T>
	void  MemMove_NullCheck (OUT T* dst, const T* src, const usize count) __NE___
	{
		StaticAssert( IsMemCopyAvailable<T> );

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
	constexpr void  UnsafeZeroMem (OUT T& value) __NE___
	{
		std::memset( OUT &value, 0, sizeof(value) );
	}

	template <typename T>
	constexpr void  ZeroMem (OUT T& value) __NE___
	{
		StaticAssert( IsZeroMemAvailable<T> );
		StaticAssert( not IsPointer<T> );

		UnsafeZeroMem( OUT value );
	}

	inline void  ZeroMem (OUT void* ptr, Bytes size) __NE___
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
		std::memset( OUT ptr, 0, usize{size} );
	}

	template <typename T>
	void  ZeroMem (OUT T* ptr, usize count) __NE___
	{
		StaticAssert( IsZeroMemAvailable<T> );
		ASSERT( (count == 0) or ((ptr != null) == (count != 0)) );

		std::memset( OUT ptr, 0, sizeof(T) * count );
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
	ND_ inline bool  MemEqual (const void* lhs, const void* rhs, Bytes size) __NE___
	{
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
	ND_ inline bool  MemLess (const void* lhs, const void* rhs, Bytes size) __NE___
	{
		return std::memcmp( lhs, rhs, usize(size) ) < 0;
	}

	template <typename T>
	ND_ bool  MemLess (const T &lhs, const T &rhs) __NE___
	{
		return std::memcmp( &lhs, &rhs, sizeof(T) ) < 0;
	}


} // AE::Base
