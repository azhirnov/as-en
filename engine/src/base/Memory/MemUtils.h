// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/Math.h"

namespace AE::Base
{
	static constexpr Bytes	SmallAllocationSize		{4 << 10};		// 4 Kb
	static constexpr Bytes	LargeAllocationSize		{4 << 20};		// 4 Mb - large page size in WIndows
	static constexpr Bytes	DefaultAllocationSize	= SmallAllocationSize;


/*
=================================================
	AddressOf
=================================================
*/
	template <typename T>
	ND_ forceinline decltype(auto)  AddressOf (T &value)
	{
		return std::addressof( value );
	}
	
	template <typename T>
	ND_ forceinline decltype(auto)  VAddressOf (T &value)
	{
		return Cast<void>( std::addressof( value ));
	}

/*
=================================================
	AddressDistance
=================================================
*/
	template <typename LT, typename RT>
	ND_ forceinline Bytes  AddressDistance (LT &lhs, RT &rhs)
	{
		return Bytes{ usize(AddressOf(lhs)) - usize(AddressOf(rhs)) };
	}
	
/*
=================================================
	DbgInitMem
=================================================
*/
# ifdef AE_DEBUG
	forceinline void  DbgInitMem (OUT void* ptr, Bytes size)
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
		std::memset( OUT ptr, 0xCD, usize{size} );
	}

	template <typename T>
	forceinline void  DbgInitMem (OUT T& value)
	{
		DbgInitMem( OUT &value, SizeOf<T> );
	}
#endif
	
/*
=================================================
	DbgFreeMem
=================================================
*/
# ifdef AE_DEBUG
	forceinline void  DbgFreeMem (OUT void* ptr, Bytes size)
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
		std::memset( OUT ptr, 0xFE, usize{size} );
	}

	template <typename T>
	forceinline void  DbgFreeMem (OUT T& value)
	{
		DbgFreeMem( OUT &value, SizeOf<T> );
	}
#endif
	
/*
=================================================
	PlacementNew
=================================================
*/
	template <typename T, typename ...Types>
	forceinline T *  PlacementNew (OUT void *ptr, Types&&... args)
	{
		ASSERT( ptr != null );
		CheckPointerCast<T>( ptr );
		return ( new(ptr) T{ FwdArg<Types>(args)... });
	}
	
/*
=================================================
	PlacementDelete
=================================================
*/
	template <typename T>
	forceinline void  PlacementDelete (INOUT T &val)
	{
		val.~T();

		DEBUG_ONLY( DbgFreeMem( val ));
	}
	
/*
=================================================
	Reconstruct
=================================================
*/
	template <typename T, typename ...Args>
	forceinline void  Reconstruct (INOUT T &value, Args&& ...args)
	{
		CheckPointerCast<T>( &value );

		value.~T();
		DEBUG_ONLY( DbgFreeMem( value ));

		new(&value) T{ FwdArg<Args>(args)... };
	}

/*
=================================================
	MemCopy
----
	memory must not intersects
=================================================
*/
	template <typename T1, typename T2>
	forceinline void  MemCopy (OUT T1 &dst, const T2 &src)
	{
		STATIC_ASSERT( sizeof(dst) >= sizeof(src) );
		STATIC_ASSERT( IsMemCopyAvailable<T1> );
		STATIC_ASSERT( IsMemCopyAvailable<T2> );
		STATIC_ASSERT( not IsConst<T1> );
		ASSERT( static_cast<const void *>(&dst) != static_cast<const void *>(&src) );

		std::memcpy( OUT &dst, &src, sizeof(src) );
	}

	forceinline void  MemCopy (OUT void *dst, const void *src, Bytes size)
	{
		if_likely( size > 0 )
		{
			ASSERT( (dst != null) and (src != null) );
			ASSERT( not IsIntersects<const void *>( dst, dst + size, src, src + size ));

			std::memcpy( OUT dst, src, usize(size) );
		}
	}

	forceinline void  MemCopy (OUT void *dst, Bytes dstSize, const void *src, Bytes srcSize)
	{
		if_likely( srcSize > 0 )
		{
			ASSERT( srcSize <= dstSize );
			ASSERT( (dst != null) and (src != null) );
			ASSERT( not IsIntersects<const void *>( dst, dst + dstSize, src, src + srcSize ));

			std::memcpy( OUT dst, src, usize(std::min(srcSize, dstSize)) );
		}
	}

/*
=================================================
	MemMove
----
	memory may intersects
=================================================
*/
	forceinline void  MemMove (OUT void *dst, const void *src, Bytes size)
	{
		if_likely( size > 0 )
		{
			ASSERT( (dst != null) and (src != null) );

			std::memmove( OUT dst, src, usize(size) );
		}
	}

	forceinline void  MemMove (OUT void *dst, Bytes dstSize, const void *src, Bytes srcSize)
	{
		if_likely( srcSize > 0 )
		{
			ASSERT( srcSize <= dstSize );
			ASSERT( (dst != null) and (src != null) );

			std::memmove( OUT dst, src, usize(std::min(srcSize, dstSize)) );
		}
	}

/*
=================================================
	ZeroMem
=================================================
*/
	template <typename T>
	forceinline void  ZeroMem (T& value)
	{
		STATIC_ASSERT( IsZeroMemAvailable<T> );
		STATIC_ASSERT( not IsPointer<T> );

		std::memset( OUT &value, 0, sizeof(value) );
	}

	forceinline void  ZeroMem (void* ptr, Bytes size)
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );

		std::memset( OUT ptr, 0, usize{size} );
	}
	
/*
=================================================
	CopyCString
=================================================
*/
	template <typename T, usize S>
	forceinline void  CopyCString (OUT T (&dst)[S], const T* src)
	{
		#ifdef AE_COMPILER_MSVC
			strcpy_s( OUT dst, src );
		#else
			strcpy( OUT dst, src );
		#endif
	}

}	// AE::Base
