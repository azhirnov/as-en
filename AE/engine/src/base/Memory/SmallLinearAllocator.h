// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Small version of LinearAllocator, should be used inside function for temporary allocations.
*/

#pragma once

#include "base/Memory/IAllocator.h"
#include "base/Containers/FixedArray.h"

namespace AE::Base
{

	//
	// Linear Allocator
	//

	template <uint  MaxBlocks			= 8,
			  usize DynamicBlockSize_v	= usize{SmallAllocationSize},
			  usize StaticSize_v		= usize{1_Kb},
			 typename AllocatorType		= UntypedAllocator
			 >
	class SmallLinearAllocator final : public IAllocator
	{
		StaticAssert( MaxBlocks > 0 );
		StaticAssert( DynamicBlockSize_v > 0 );
		StaticAssert( StaticSize_v > 0 );

	// types
	public:
		using Allocator_t	= AllocatorType;
		using Self			= SmallLinearAllocator< MaxBlocks, DynamicBlockSize_v, StaticSize_v, AllocatorType >;

	private:
		struct Block
		{
			void *		ptr			= null;
			Bytes32u	size;		// used memory size
			Bytes32u	capacity;	// size of block
		};
		using Blocks_t	= FixedArray< Block, MaxBlocks >;

		using Helper_t	= AllocatorHelper< EAllocatorType::Linear >;

		static constexpr Bytes	_Padding	= 0_b;


	// variables
	private:
		Blocks_t	_blocks;
		char		_memory [StaticSize_v];


	// methods
	public:
		SmallLinearAllocator ()										__NE___ {}
		~SmallLinearAllocator ()									__NE_OV	{ Release(); }

			void	Release ()										__NE___;

		ND_ Bytes				TotalSize ()						C_NE___;
		ND_ ArrayView<Block>	GetBlocks ()						C_NE___	{ return _blocks; }

		ND_ static constexpr Bytes	StaticSize ()					__NE___	{ return Bytes{StaticSize_v}; }
		ND_ static constexpr Bytes	DynamicBlockSize ()				__NE___	{ return Bytes{DynamicBlockSize_v}; }


		// IAllocator //
		ND_ void*	Allocate (const SizeAndAlign)					__NE_OV;
			using	IAllocator::Allocate;

			void	Deallocate (void* ptr)							__NE_OV { Deallocate( ptr, 1_b ); }
			void	Deallocate (void* ptr, Bytes size)				__NE_OV;
			void	Deallocate (void* ptr, const SizeAndAlign sa)	__NE_OV { Deallocate( ptr, sa.size ); }

			void	Discard ()										__NE_OV;
	};



/*
=================================================
	Allocate
=================================================
*/
	template <uint MB, usize DBS, usize S, typename A>
	void*  SmallLinearAllocator<MB,DBS,S,A>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
	{
		ASSERT( sizeAndAlign.size > 0 );
		ASSERT( sizeAndAlign.align > 0 );

		for (auto& block : _blocks)
		{
			const Bytes	offset	= AlignUp( Bytes{block.ptr} + (block.size != 0 ? _Padding + block.size : 0_b), sizeAndAlign.align ) - Bytes{block.ptr};

			if_likely( offset + sizeAndAlign.size <= block.capacity )
			{
				block.size = offset + sizeAndAlign.size;
				Helper_t::OnAllocate( block.ptr + offset, sizeAndAlign );
				return block.ptr + offset;
			}
		}

		if_unlikely( _blocks.size() == _blocks.capacity() )
			return null;

		Bytes	block_size;
		void*	ptr;

		if ( _blocks.empty() and sizeAndAlign.size <= StaticSize() )
		{
			ptr			= _memory;
			block_size	= StaticSize();
		}
		else
		{
			block_size	= DynamicBlockSize() * (1 + _blocks.size() / 2);
			block_size	= sizeAndAlign.size*2 < block_size ? block_size : sizeAndAlign.size*2;
			ptr			= Allocator_t::Allocate( SizeAndAlign{ block_size, DefaultAllocatorAlign });

			if_unlikely( ptr == null )
			{
				DBG_WARNING( "failed to allocate memory" );
				return null;
			}
		}

		auto&	block	= _blocks.emplace_back( Block{ ptr, 0_b, block_size });
		Bytes	offset	= AlignUp( Bytes{block.ptr}, sizeAndAlign.align ) - Bytes{block.ptr};

		DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));

		block.size = offset + sizeAndAlign.size;
		Helper_t::OnAllocate( block.ptr + offset, sizeAndAlign );
		return block.ptr + offset;
	}

/*
=================================================
	Deallocate
=================================================
*/
	template <uint MB, usize DBS, usize S, typename A>
	void  SmallLinearAllocator<MB,DBS,S,A>::Deallocate (void* ptr, Bytes size) __NE___
	{
	#ifdef AE_DEBUG
		for (auto& block : _blocks)
		{
			if ( IsIntersects( ptr, ptr + size, block.ptr, block.ptr + block.size ))
			{
				CHECK( ptr + size <= block.ptr + block.size );
				return;
			}
			AE_LOG_DBG( "'ptr' is not belong to this allocator" );
		}
	#else
		Unused( ptr, size );
	#endif
	}

/*
=================================================
	Discard
=================================================
*/
	template <uint MB, usize DBS, usize S, typename A>
	void  SmallLinearAllocator<MB,DBS,S,A>::Discard () __NE___
	{
		for (auto& block : _blocks)
		{
			DEBUG_ONLY( DbgInitMem( block.ptr, block.size ));
			block.size = 0_b;
		}
	}

/*
=================================================
	Release
=================================================
*/
	template <uint MB, usize DBS, usize S, typename A>
	void  SmallLinearAllocator<MB,DBS,S,A>::Release () __NE___
	{
		for (auto& block : _blocks)
		{
			if ( block.ptr == _memory )
				continue;

			Allocator_t::Deallocate( block.ptr, SizeAndAlign{ block.capacity, DefaultAllocatorAlign });
		}
		_blocks.clear();
	}

/*
=================================================
	TotalSize
=================================================
*/
	template <uint MB, usize DBS, usize S, typename A>
	Bytes  SmallLinearAllocator<MB,DBS,S,A>::TotalSize () C_NE___
	{
		Bytes	size;
		for (auto& block : _blocks) {
			size += block.capacity;
		}
		return size;
	}


} // AE::Base
