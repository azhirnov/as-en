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
			  typename AllocatorType = UntypedAllocator
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
		explicit LfLinearAllocator (const Allocator_t &alloc = Allocator_t{}) __NE___;
		~LfLinearAllocator ()												__NE___	{ Release(); }

		void  Release ()													__NE___;
		void  Discard ()													__NE___;
		
		template <typename T>
		ND_ AE_ALLOCATOR T*  Allocate (usize count = 1)						__NE___
		{
			return Cast<T>( Allocate( SizeAndAlignOf<T> * count ));
		}

		ND_ AE_ALLOCATOR void*  Allocate (const SizeAndAlign sizeAndAlign)	__NE___;

		void  Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)		__NE___;
		void  Deallocate (void *ptr, const Bytes size)						__NE___	{ Deallocate( ptr, size, 1_b ); }
		void  Deallocate (void *ptr)										__NE___	{ Unused( ptr ); }

		
		ND_ static constexpr Bytes  BlockSize ()							__NE___	{ return _Capacity; }
		ND_ static constexpr Bytes  MaxSize ()								__NE___	{ return MaxBlocks * _Capacity; }
	};


	
/*
=================================================
	constructor
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	LfLinearAllocator<BS,MA,MB,A>::LfLinearAllocator (const Allocator_t &alloc) __NE___ :
		_allocator{ alloc }
	{
		// make visible changes in '_blocks'
		ThreadFence( EMemoryOrder::Release );
	}
	
/*
=================================================
	Release
----
	must be externally synchronized
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	void  LfLinearAllocator<BS,MA,MB,A>::Release () __NE___
	{
		EXLOCK( _allocGuard );

		Bytes	allocated;
		Bytes	used;
		
		for (auto& block : _blocks)
		{
			if ( void* ptr = block.mem.exchange( null ))
			{
				_allocator.Deallocate( ptr, SizeAndAlign{ BlockSize(), _MemAlign });

				allocated	+= BlockSize();
				used		+= block.size.load();
			}
		}

		//AE_LOG_DBG( "Linear allocator usage: "s << ToString(used) << " / " << ToString(allocated) << ",  "
		//			<< ToString( double(usize(used)) / double(usize(allocated)) * 100.0, 1 ) << " %" );
	}
	
/*
=================================================
	Discard
----
	must be externally synchronized
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	void  LfLinearAllocator<BS,MA,MB,A>::Discard () __NE___
	{
		for (auto& block : _blocks)
		{
			block.size.store( 0 );
		}
	}

/*
=================================================
	Allocate
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	void*  LfLinearAllocator<BS,MA,MB,A>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
	{
		for (auto block_it = _blocks.begin(); block_it != _blocks.end();)
		{
			void*	ptr = block_it->mem.load();

			if_likely( ptr != null )
			{
				// find available space
				for (usize offset = block_it->size.load();;)
				{
					Bytes	aligned_off = AlignUp( usize(ptr) + Bytes{offset}, sizeAndAlign.align ) - usize(ptr);
						
					if_unlikely( aligned_off + sizeAndAlign.size > BlockSize() )
						break; // current block is too small

					if_likely( block_it->size.CAS( INOUT offset, usize(aligned_off + sizeAndAlign.size) ))
						return ptr + aligned_off;
					
					ThreadUtils::Pause();
				}
				++block_it;
			}
			else
			{
				// only one thread will allocate new block
				EXLOCK( _allocGuard );

				ptr = block_it->mem.load();
				if ( ptr != null )
					continue;	// was allocated in another thread

				// allocate
				ptr = _allocator.Allocate( SizeAndAlign{ BlockSize(), _MemAlign });
				CHECK_ERR( ptr != null );

				void*	prev = block_it->mem.exchange( ptr );
				CHECK( prev == null );
			}

			ThreadUtils::Pause();
		}
		return null;
	}
	
/*
=================================================
	Deallocate
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	void  LfLinearAllocator<BS,MA,MB,A>::Deallocate (void *ptr, const SizeAndAlign sizeAndAlign) __NE___
	{
	#ifdef AE_DBG_OR_DEV
		for (auto& block : _blocks)
		{
			if ( void* mem = block.mem.load() )
			{
				if ( IsIntersects( ptr, ptr + sizeAndAlign.size, mem, mem + BlockSize() ))
				{
					CHECK( ptr + sizeAndAlign.size <= mem + Bytes{block.size.load()} );
					return;
				}
			}
		}
		AE_LOG_DEV( "'ptr' is not belong to this allocator" );
	#else
		Unused( ptr, sizeAndAlign );
	#endif
	}

} // AE::Threading
