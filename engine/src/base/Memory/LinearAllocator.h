// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Math.h"
#include "base/Math/Bytes.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Containers/FixedArray.h"
#include "base/CompileTime/TemplateUtils.h"

namespace AE::Base
{

	//
	// Linear Allocator
	//

	template <typename AllocatorType = UntypedAlignedAllocator,
			  uint MaxBlocks = 16,
			  bool ThreadSafe = false
			 >
	struct LinearAllocator final
	{
		STATIC_ASSERT( not ThreadSafe );

	// types
	public:
		using Allocator_t	= AllocatorType;
		using Self			= LinearAllocator< AllocatorType, MaxBlocks, false >;
		
		static constexpr bool	IsThreadSafe = false;

	private:
		struct Block
		{
			void *		ptr			= null;
			Bytes		size;		// used memory size
			Bytes		capacity;	// size of block
		};
		using Blocks_t = FixedArray< Block, MaxBlocks >;
		
		static constexpr Bytes	_PtrAlign	= SizeOf<void *>;

		static constexpr Bytes	_Padding	= 0_b;


	// variables
	private:
		Blocks_t			_blocks;
		Bytes				_blockSize	= SmallAllocationSize;
		
		NO_UNIQUE_ADDRESS
		 Allocator_t		_alloc;


	// methods
	public:
		LinearAllocator () {}
		
		explicit LinearAllocator (const Allocator_t &alloc) : _alloc{alloc}
		{}
		
		explicit LinearAllocator (Bytes blockSize) : _blockSize{ blockSize }
		{}

		LinearAllocator (Self &&other) :
			_blocks{ RVRef(other._blocks) },
			_blockSize{ other._blockSize },
			_alloc{ RVRef(other._alloc) }
		{}

		LinearAllocator (const Self &) = delete;

		Self&  operator = (const Self &) = delete;


		Self&  operator = (Self &&rhs)
		{
			Release();
			_blocks		= RVRef(rhs._blocks);
			_blockSize	= rhs._blockSize;
			_alloc		= RVRef(rhs._alloc);
			return *this;
		}


		~LinearAllocator ()
		{
			Release();
		}


		void  SetBlockSize (Bytes size)
		{
			_blockSize = size;
		}


		ND_ AE_ALLOCATOR void*  Allocate (const Bytes size, const Bytes align)
		{
			ASSERT( size > 0 );

			for (auto& block : _blocks)
			{
				Bytes	offset	= AlignUp( Bytes{block.ptr} + (block.size != 0 ? _Padding + block.size : 0_b), align ) - Bytes{block.ptr};

				if_likely( offset + size <= block.capacity )
				{
					block.size = offset + size;
					return block.ptr + offset;
				}
			}

			if_unlikely( _blocks.size() == _blocks.capacity() )
			{
				DBG_WARNING( "overflow" );
				return null;
			}

			Bytes	block_size	= _blockSize * (1 + _blocks.size() / 2);
					block_size	= size*2 < block_size ? block_size : size*2;
			void*	ptr			= _alloc.Allocate( block_size, _PtrAlign );

			if_unlikely( ptr == null )
			{
				DBG_WARNING( "failed to allocate memory" );
				return null;
			}

			auto&	block	= _blocks.emplace_back(Block{ ptr, 0_b, block_size });
			Bytes	offset	= AlignUp( Bytes{block.ptr}, align ) - Bytes{block.ptr};

			DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));
			
			block.size = offset + size;
			return block.ptr + offset;
		}


		template <typename T>
		ND_ AE_ALLOCATOR T*  Allocate (usize count = 1)
		{
			return Cast<T>( Allocate( SizeOf<T> * count, AlignOf<T> ));
		}
		
		void  Deallocate (void *, Bytes)		{}
		void  Deallocate (void *, Bytes, Bytes)	{}

		void  Discard ()
		{
			for (auto& block : _blocks)
			{
				block.size = 0_b;
				DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));
			}
		}

		void  Release ()
		{
			for (auto& block : _blocks) {
				_alloc.Deallocate( block.ptr, block.capacity, _PtrAlign );
			}
			_blocks.clear();
		}

		ND_ Bytes  TotalSize () const
		{
			Bytes	size;
			for (auto& block : _blocks) {
				size += block.capacity;
			}
			return size;
		}

		ND_ ArrayView<Block>  GetBlocks () const
		{
			return _blocks;
		}
	};

}	// AE::Base
