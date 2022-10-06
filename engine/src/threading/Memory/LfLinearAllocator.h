// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "base/Math/BitMath.h"
# include "base/Math/Bytes.h"
# include "base/Math/Math.h"
# include "base/Memory/UntypedAllocator.h"

# include "threading/Primitives/Atomic.h"
# include "threading/Primitives/SyncEvent.h"
#endif

#ifdef AE_DEBUG
# include "base/Algorithms/StringUtils.h"
#endif

#include "base/Platforms/Platform.h"


namespace AE::Threading
{
	using namespace AE::Math;

	//
	// Lock-Free Linear Allocator
	//

	template <usize BlockSize_v		= usize(SmallAllocationSize),	// in bytes
			  usize MemAlign		= 8,
			  usize MaxBlocks		= 16,
			  typename AllocatorType = UntypedAlignedAllocator
			 >
	class LfLinearAllocator final
	{
		STATIC_ASSERT( BlockSize_v > 0 );
		STATIC_ASSERT( MaxBlocks > 0 and MaxBlocks < 64 );

	// types
	public:
		using Self			= LfLinearAllocator< BlockSize_v, MemAlign, MaxBlocks, AllocatorType >;
		using Index_t		= uint;
		using Allocator_t	= AllocatorType;
		
		static constexpr bool	IsThreadSafe = true;

	private:
		struct alignas(AE_CACHE_LINE) MemBlock
		{
			Atomic< void *>		mem		{null};
			Atomic< usize >		size	{0};
		};
		using MemBlocks_t = StaticArray< MemBlock, MaxBlocks >;
		
		static constexpr Bytes	_Capacity {BlockSize_v};
		static constexpr Bytes	_MemAlign {MemAlign};


	// variables
	private:
		MemBlocks_t			_blocks	= {};

		Mutex				_allocGuard;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;


	// methods
	public:
		explicit LfLinearAllocator (const Allocator_t &alloc = Allocator_t{});
		~LfLinearAllocator ()	{ Release(); }

		void  Release ();
		void  Discard ();
		
		template <typename T>
		ND_ AE_ALLOCATOR T*  Allocate (usize count = 1)
		{
			return Cast<T>( Allocate( SizeOf<T> * count, AlignOf<T> ));
		}

		ND_ AE_ALLOCATOR void*  Allocate (const Bytes size, const Bytes align);

		void  Deallocate (void *ptr, const Bytes size, const Bytes align);

		
		ND_ static constexpr Bytes  BlockSize ()	{ return _Capacity; }
		ND_ static constexpr Bytes  MaxSize ()		{ return MaxBlocks * _Capacity; }
	};


}	// AE::Threading
