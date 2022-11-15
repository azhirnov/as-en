// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/Math.h"

namespace AE::Base
{
	static constexpr Bytes	SmallAllocationSize		{4 << 10};		// 4 Kb
	static constexpr Bytes	LargeAllocationSize		{4 << 20};		// 4 Mb - large page size in WIndows
	static constexpr Bytes	DefaultAllocationSize	= SmallAllocationSize;


	//
	// UniquePtr Placement Delete
	//
	template <typename T>
	struct UniquePtr_PlacementDelete
	{
		constexpr UniquePtr_PlacementDelete () = default;

		void  operator () (T* ptr) const
		{
			STATIC_ASSERT( 0 < sizeof(T), "can't delete an incomplete type" );
			ptr->~T();
			// without releasing memory
		}
	};

	template <typename T>
	using UniqueNoDel = std::unique_ptr< T, UniquePtr_PlacementDelete<T> >;
	
	template <typename T, typename ...Types>
	ND_ forceinline UniqueNoDel<T>  MakeUniqueNoDel (Types&&... args) __TH___
	{
		return UniqueNoDel<T>{ new T{ FwdArg<Types>( args )... }};
	}

/*
=================================================
	AddressOf
=================================================
*/
	template <typename T>
	ND_ forceinline constexpr decltype(auto)  AddressOf (T &value) __NE___
	{
		return std::addressof( value );
	}
	
	template <typename T>
	ND_ forceinline constexpr decltype(auto)  VAddressOf (T &value) __NE___
	{
		return Cast<void>( std::addressof( value ));
	}

/*
=================================================
	AddressDistance
=================================================
*/
	template <typename LT, typename RT>
	ND_ forceinline Bytes  AddressDistance (LT &lhs, RT &rhs) __NE___
	{
		return Bytes{ usize(AddressOf(lhs)) - usize(AddressOf(rhs)) };
	}
	
/*
=================================================
	DbgInitMem
=================================================
*/
# ifdef AE_DEBUG
	forceinline void  DbgInitMem (OUT void* ptr, Bytes size) __NE___
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
		std::memset( OUT ptr, 0xCD, usize{size} );
	}

	template <typename T>
	forceinline void  DbgInitMem (OUT T& value) __NE___
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
	forceinline void  DbgFreeMem (OUT void* ptr, Bytes size) __NE___
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );
		std::memset( OUT ptr, 0xFE, usize{size} );
	}

	template <typename T>
	forceinline void  DbgFreeMem (OUT T& value) __NE___
	{
		DbgFreeMem( OUT &value, SizeOf<T> );
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
	forceinline T *  PlacementNew (OUT void *ptr, Args&&... args) __TH___
	{
		ASSERT( ptr != null );
		CheckPointerCast<T>( ptr );
		return ( new(ptr) T{ FwdArg<Args>(args)... });	// throw
	}
	
/*
=================================================
	PlacementNewNE
	PlacementNew
=================================================
*
	template <typename T, typename ...Args>
	forceinline T *  PlacementNewNE (OUT void *ptr, Args&&... args) __NE___
	{
		if constexpr( sizeof...(Args) == 0 ) {
			STATIC_ASSERT( IsNothrowDefaultCtor< T >);
		}else{
			STATIC_ASSERT( IsNothrowCtor< T, Args... >);
			STATIC_ASSERT( IsNothrowCopyCtor< T >);
			STATIC_ASSERT( IsNothrowMoveCtor< T >);
		}
		ASSERT( ptr != null );
		CheckPointerCast<T>( ptr );
		return ( new(ptr) T{ FwdArg<Args>(args)... });	// nothrow
	}
	
/*
=================================================
	PlacementDelete
=================================================
*/
	template <typename T>
	forceinline void  PlacementDelete (INOUT T &val) __NE___
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
	forceinline void  Reconstruct (INOUT T &value, Args&& ...args) __TH___
	{
		CheckPointerCast<T>( &value );

		value.~T();
		DEBUG_ONLY( DbgFreeMem( value ));

		new(&value) T{ FwdArg<Args>(args)... };		// throw
	}

/*
=================================================
	MemCopy
----
	memory must not intersects
=================================================
*/
	template <typename T1, typename T2>
	forceinline void  MemCopy (OUT T1 &dst, const T2 &src) __NE___
	{
		STATIC_ASSERT( sizeof(dst) >= sizeof(src) );
		STATIC_ASSERT( IsMemCopyAvailable<T1> );
		STATIC_ASSERT( IsMemCopyAvailable<T2> );
		STATIC_ASSERT( not IsConst<T1> );
		ASSERT( static_cast<const void *>(&dst) != static_cast<const void *>(&src) );

		std::memcpy( OUT &dst, &src, sizeof(src) );
	}

	forceinline void  MemCopy (OUT void *dst, const void *src, Bytes size) __NE___
	{
		if_likely( size > 0 )
		{
			ASSERT( (dst != null) and (src != null) );
			ASSERT( not IsIntersects<const void *>( dst, dst + size, src, src + size ));

			std::memcpy( OUT dst, src, usize(size) );
		}
	}

	forceinline void  MemCopy (OUT void *dst, Bytes dstSize, const void *src, Bytes srcSize) __NE___
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
	forceinline void  MemMove (OUT void *dst, const void *src, Bytes size) __NE___
	{
		if_likely( size > 0 )
		{
			ASSERT( (dst != null) and (src != null) );

			std::memmove( OUT dst, src, usize(size) );
		}
	}

	forceinline void  MemMove (OUT void *dst, Bytes dstSize, const void *src, Bytes srcSize) __NE___
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
	forceinline void  ZeroMem (OUT T& value) __NE___
	{
		STATIC_ASSERT( IsZeroMemAvailable<T> );
		STATIC_ASSERT( not IsPointer<T> );

		std::memset( OUT &value, 0, sizeof(value) );
	}

	forceinline void  ZeroMem (OUT void* ptr, Bytes size) __NE___
	{
		ASSERT( (size == 0) or ((ptr != null) == (size != 0)) );

		std::memset( OUT ptr, 0, usize{size} );
	}
	
/*
=================================================
	SecureZeroMem
=================================================
*/
	template <typename T>
	forceinline void  SecureZeroMem (OUT T& value) __NE___
	{
		STATIC_ASSERT( IsZeroMemAvailable<T> );
		STATIC_ASSERT( not IsPointer<T> );

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
	template <typename T, usize S>
	forceinline void  CopyCString (OUT T (&dst)[S], const T* src) __NE___
	{
		#ifdef AE_COMPILER_MSVC
			::strcpy_s( OUT dst, src );
		#else
			::strcpy( OUT dst, src );
		#endif
	}
	
/*
=================================================
	MemEqual
=================================================
*/
	ND_ forceinline bool  MemEqual (const void* lhs, const void* rhs, Bytes size) __NE___
	{
		return std::memcmp( lhs, rhs, usize(size) ) == 0;
	}

	template <typename T>
	ND_ forceinline bool  MemEqual (const T &lhs, const T &rhs) __NE___
	{
		return MemEqual( &lhs, &rhs, SizeOf<T> );
	}


} // AE::Base
