// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	LfLinearAllocator<BS,MA,MB,A>::LfLinearAllocator (const Allocator_t &alloc) __NE___ :
		_allocator{ alloc }
	{}

/*
=================================================
	CurrentSize
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	Bytes  LfLinearAllocator<BS,MA,MB,A>::CurrentSize () C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		Bytes	result;

		for (auto& block : _blocks)
		{
			result += Bytes{block.size.load()};
		}
		return result;
	}

/*
=================================================
	Release
----
	Must be externally synchronized
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	void  LfLinearAllocator<BS,MA,MB,A>::Release () __NE___
	{
		DRC_EXLOCK( _drCheck );
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
	Must be externally synchronized
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	void  LfLinearAllocator<BS,MA,MB,A>::Discard () __NE___
	{
		DRC_EXLOCK( _drCheck );

		for (auto& block : _blocks)
		{
			block.size.store( 0 );
		}

	#ifdef AE_DEBUG
		MemoryBarrier( EMemoryOrder::Acquire );
		for (auto& block : _blocks)
		{
			if ( auto* ptr = block.mem.load() )
				DbgInitMem( OUT ptr, BlockSize() );
		}
		MemoryBarrier( EMemoryOrder::Release );
	#endif
	}

/*
=================================================
	Allocate
=================================================
*/
	template <usize BS, usize MA, usize MB, typename A>
	void*  LfLinearAllocator<BS,MA,MB,A>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

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
	void  LfLinearAllocator<BS,MA,MB,A>::Deallocate (void* ptr, Bytes size) __NE___
	{
	#ifdef AE_DEBUG
		DRC_SHAREDLOCK( _drCheck );

		for (auto& block : _blocks)
		{
			if ( void* mem = block.mem.load() )
			{
				if ( IsIntersects( ptr, ptr + size, mem, mem + BlockSize() ))
				{
					CHECK( ptr + size <= mem + Bytes{block.size.load()} );
					return;
				}
			}
		}
		AE_LOG_DBG( "'ptr' is not belong to this allocator" );
	#else
		Unused( ptr, size );
	#endif
	}

} // AE::Threading
