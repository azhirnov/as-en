// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Memory/MemUtils.h"
#include "base/Utils/Helpers.h"

namespace AE::Base
{

struct CopyPolicy final : public Noninstanceable
{
	// interface:
	//	Create		- create default elements
	//	Destroy		- destroy elements
	//	Copy		- copy elements from one memblock to another memblock, using copy constructor
	//	Move		- move elements from one memblock to another memblock, using move constructor without destructor on source element
	//	Replace		- replace elements inside memory block, using move constructor and destructor on source element	- replace to the left
	//	ReplaceRev	- replace elements inside memory block with reverse order										- replace to the right
	//
	//	in Replace & ReplaceRev :
	//		inSingleMemBlock is used when source and destination memory is aliased

private:
	enum _Flags : uint
	{
		NonTrivialCtor		= 1u << 0,
		NonTrivialDtor		= 1u << 1,
		NonTrivialCopyCtor	= 1u << 2,
		NonTrivialMoveCtor	= 1u << 3,
	};

	template <typename T, uint Flags>
	struct _DefaultImpl : public Noninstanceable
	{
		static void  Create (OUT T* ptr, const usize count) __NE___
		{
			ASSERT( (count == 0) or ((ptr != null) == (count != 0)) );

			if constexpr( !!(Flags & NonTrivialCtor) )
			{
				for (usize i = 0; i < count; ++i)
					PlacementNew<T>( OUT ptr + i );
			}
			else
			{
				StaticAssert( IsZeroMemAvailable<T> );
				ZeroMem( OUT ptr, SizeOf<T> * count );
			}
		}

		static void  Destroy (INOUT T* ptr, const usize count) __NE___
		{
			ASSERT( (count == 0) or ((ptr != null) == (count != 0)) );

			if constexpr( !!(Flags & NonTrivialDtor) )
			{
				for (usize i = 0; i < count; ++i)
					ptr[i].~T();
			}
			else
			{
				StaticAssert( IsTriviallyDestructible<T> );
			}
			DEBUG_ONLY( DbgInitMem( OUT ptr, SizeOf<T> * count ));
		}

		static void  Copy (OUT T* dst, const T * const src, const usize count)  NoExcept(IsNothrowCopyCtor<T>)
		{
			ASSERT( (count == 0) or ((dst != null) == (count != 0)) );
			ASSERT( (count == 0) or ((src != null) == (count != 0)) );

			if constexpr( !!(Flags & NonTrivialCopyCtor) )
			{
				for (usize i = 0; i < count; ++i)
					PlacementNew<T>( OUT dst+i, src[i] );	// throw
			}
			else
			{
				StaticAssert( IsMemCopyAvailable<T> );
				MemCopy( OUT dst, src, SizeOf<T> * count );
			}
		}

		static void  Move (OUT T* dst, INOUT T* src, const usize count) __NE___
		{
			ASSERT( (count == 0) or ((dst != null) == (count != 0)) );
			ASSERT( (count == 0) or ((src != null) == (count != 0)) );

			if constexpr( !!(Flags & NonTrivialMoveCtor) )
			{
				for (usize i = 0; i < count; ++i)
					PlacementNew<T>( OUT dst+i, RVRef( src[i] ));
			}
			else
			{
				StaticAssert( IsMemCopyAvailable<T> );
				MemMove( OUT dst, src, SizeOf<T> * count );
			}
		}

		static void  Replace (OUT T* dst, INOUT T* src, const usize count, Bool inSingleMemBlock = False{}) __NE___
		{
			ASSERT( (count == 0) or ((dst != null) == (count != 0)) );
			ASSERT( (count == 0) or ((src != null) == (count != 0)) );
			ASSERT( src != dst );
			Unused( inSingleMemBlock );

			if constexpr( !!(Flags & NonTrivialMoveCtor) )
			{
				for (usize i = 0; i < count; ++i)
				{
					PlacementNew<T>( OUT dst+i, RVRef( src[i] ));

					if constexpr( !!(Flags & NonTrivialDtor) )
					{
						src[i].~T();
					}else
						StaticAssert( IsTriviallyDestructible<T> );
				}
			}
			else
			{
				StaticAssert( IsMemCopyAvailable<T> );
				MemMove( OUT dst, src, SizeOf<T> * count );
			}

			// clear old values after replace
			DEBUG_ONLY(
			if ( inSingleMemBlock )
			{
				for (T* t = src; t < src + count; ++t)
					if ( t < dst or t >= dst + count )
						DbgInitMem( t );
			})
		}

		static void  ReplaceRev (OUT T* dst, INOUT T* src, const usize count, Bool inSingleMemBlock = False{}) __NE___
		{
			ASSERT( (count == 0) or ((dst != null) == (count != 0)) );
			ASSERT( (count == 0) or ((src != null) == (count != 0)) );
			ASSERT( src != dst );
			Unused( inSingleMemBlock );

			if constexpr( !!(Flags & NonTrivialMoveCtor) )
			{
				for (usize i = count-1; i < count; --i)
				{
					PlacementNew<T>( OUT dst+i, RVRef( src[i] ));

					if constexpr( !!(Flags & NonTrivialDtor) )
					{
						StaticAssert( std::is_nothrow_destructible_v<T> );
						src[i].~T();
					}else
						StaticAssert( IsTriviallyDestructible<T> );
				}
			}
			else
			{
				StaticAssert( IsMemCopyAvailable<T> );
				MemMove( OUT dst, src, SizeOf<T> * count );
			}

			// clear old values after replace
			DEBUG_ONLY(
			if ( inSingleMemBlock )
			{
				for (T* t = src; t < src + count; ++t)
					if ( t < dst or t >= dst + count )
						DbgInitMem( t );
			})
		}
	};

	template <typename T>
	struct _AutoDetect
	{
		static constexpr uint	Flags = (IsMemCopyAvailable<T>		? 0 : (NonTrivialCopyCtor | NonTrivialMoveCtor))	|
										(IsTriviallyDestructible<T>	? 0 : NonTrivialDtor)								|
										(IsZeroMemAvailable<T>		? 0 : NonTrivialCtor);
		using type = _DefaultImpl< T, Flags >;
	};


public:

	//
	// Copy Policy with Copy Ctor and Move Ctor
	//

	template <typename T>
	struct CopyAndMoveCtor final :
		_DefaultImpl< T, NonTrivialCtor | NonTrivialDtor | NonTrivialCopyCtor | NonTrivialMoveCtor >
	{};


	//
	// Copy Policy with MemCopy & MemMove without Ctors (low overhead)
	//

	template <typename T>
	struct MemCopyWithoutCtor final :
		_DefaultImpl< T, 0 >
	{};


	template <typename T>
	using AutoDetect = typename _AutoDetect<T>::type;
};

} // AE::Base
