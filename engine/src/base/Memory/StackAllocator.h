// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/BitMath.h"
#include "base/Math/Bytes.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Containers/FixedArray.h"
#include "base/CompileTime/Math.h"

namespace AE::Base
{

	//
	// Stack Allocator
	//

	template <typename AllocatorType, uint MaxBlocks>
	struct StackAllocator< AllocatorType, MaxBlocks, false > final
	{
	// types
	public:
		using Self			= StackAllocator< AllocatorType, MaxBlocks, false >;
		using Allocator_t	= AllocatorType;

		enum class Bookmark : usize {};
		
		static constexpr bool	IsThreadSafe = false;

		struct AutoReleaseBookmark
		{
			friend struct StackAllocator;
		private:
			Self &			_ref;
			const Bookmark	_bm;

			AutoReleaseBookmark (Self &ref) : _ref{ref}, _bm{ref.Push()} {}
		public:
			~AutoReleaseBookmark ()	{ _ref.Pop( _bm ); }
		};

	private:
		struct Block
		{
			void *		ptr			= null;
			Bytes		size;		// used memory size
			Bytes		capacity;	// size of block
		};

		using Blocks_t			= FixedArray< Block, MaxBlocks >;
		using BookmarkStack_t	= FixedArray< Bookmark, 32 >;

		static constexpr uint	_PtrOffset		= CT_CeilIntLog2< MaxBlocks >;
		static constexpr usize	_BlockIndexMask	= (1u << _PtrOffset) - 1;
		static constexpr Bytes	_PtrAlign		= SizeOf<void *>;


	// variables
	private:
		Blocks_t			_blocks;
		BookmarkStack_t		_bookmarks;
		Bytes				_blockSize	= SmallAllocationSize;
		
		NO_UNIQUE_ADDRESS
		 Allocator_t		_alloc;


	// methods
	public:
		StackAllocator () {}
		StackAllocator (Self &&other);
		StackAllocator (const Self &) = delete;
		explicit StackAllocator (const Allocator_t &alloc) : _alloc{alloc} {}

		~StackAllocator ()		{ Release(); }
		
		Self&  operator = (const Self &) = delete;
		Self&  operator = (Self &&rhs);


		void SetBlockSize (Bytes size)
		{
			_blockSize = size;
		}

		template <typename T>
		ND_ AE_ALLOCATOR T*  Allocate (usize count = 1)
		{
			return Cast<T>( Allocate( SizeOf<T> * count, AlignOf<T> ));
		}

		ND_ AE_ALLOCATOR void*  Allocate (const Bytes size, const Bytes align);
		
		void  Deallocate (void* ptr, Bytes size, Bytes align)
		{
			Unused( ptr, size, align );
		}

			bool		Commit (Bookmark bm, Bytes size);

		ND_ auto		PushAuto ()				{ return AutoReleaseBookmark{ *this }; }

		ND_ Bookmark	Push ();
			void		Pop (Bookmark bm);

		void  Discard ();
		void  Release ();

	private:
		ND_ static Pair<usize, usize>  _UnpackBookmark (Bookmark bm);
	};

	
	
/*
=================================================
	constructor
=================================================
*/
	template <typename A, uint B>
	StackAllocator<A,B,false>::StackAllocator (Self &&other) :
		_blocks{ RVRef(other._blocks) },
		_bookmarks{ RVRef(other._bookmarks) },
		_blockSize{ other._blockSize },
		_alloc{ RVRef(other._alloc) }
	{}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename A, uint B>
	StackAllocator<A,B,false>&  StackAllocator<A,B,false>::operator = (Self && rhs)
	{
		Release();
		_blocks		= RVRef(rhs._blocks);
		_bookmarks	= RVRef(rhs._bookmarks);
		_blockSize	= rhs._blockSize;
		_alloc		= RVRef(rhs._alloc);
		return *this;
	}
	
/*
=================================================
	Allocate
=================================================
*/
	template <typename A, uint B>
	void*  StackAllocator<A,B,false>::Allocate (const Bytes size, const Bytes align)
	{
		usize	idx	= _blocks.size() ? 0u : UMax;

		if ( _bookmarks.size() )
		{
			usize	off = 0;
			std::tie( idx, off ) = _UnpackBookmark( _bookmarks.back() );

			ASSERT( idx >= _blocks.size() or _blocks[idx].size >= off );
		}

		for (; idx < _blocks.size(); ++idx)
		{
			auto&	block	= _blocks[idx];
			Bytes	offset	= AlignUp( usize(block.ptr) + block.size, align ) - usize(block.ptr);
				
			if_likely( size <= (block.capacity - offset) )
			{
				block.size = offset + size;
				return block.ptr + offset;
			}
		}
			
		if_unlikely( _blocks.size() == _blocks.capacity() )
		{
			//DBG_WARNING( "overflow" );
			return null;
		}

		Bytes	block_size	= _blockSize * (1 + _blocks.size()/2);
				block_size	= size*2 < block_size ? block_size : block_size*2;
		void*	ptr			= _alloc.Allocate( block_size, _PtrAlign );

		if_unlikely( ptr == null )
		{
			DBG_WARNING( "failed to allocate memory" );
			return null;
		}

		auto&	block		= _blocks.emplace_back(Block{ ptr, 0_b, block_size });
		Bytes	offset		= AlignUp( usize(block.ptr) + block.size, align ) - usize(block.ptr);

		DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));
			
		block.size = offset + size;
		return block.ptr + offset;
	}
	
/*
=================================================
	Push
=================================================
*/
	template <typename A, uint B>
	typename StackAllocator<A,B,false>::Bookmark  StackAllocator<A,B,false>::Push ()
	{
		Bookmark	bm = Bookmark(0);

		if_likely( _blocks.size() )
		{
			bm = Bookmark( (_blocks.size()-1) | (usize(_blocks.back().size) << _PtrOffset) );
		}

		if_unlikely( _bookmarks.size() == _bookmarks.capacity() )
		{
			DBG_WARNING( "overflow" );
			return Bookmark(~0ull);
		}

		_bookmarks.push_back( bm );
		return bm;
	}
	
/*
=================================================
	Pop
=================================================
*/
	template <typename A, uint B>
	void  StackAllocator<A,B,false>::Pop (Bookmark bm)
	{
		for (usize i = 0; i < _bookmarks.size(); ++i)
		{
			if ( _bookmarks[i] == bm )
			{
				auto[idx, off] = _UnpackBookmark( bm );

				for (; idx < _blocks.size(); ++idx)
				{
					auto& block = _blocks[idx];
					block.size = Bytes{off};

					DEBUG_ONLY( DbgInitMem( block.ptr + block.size, block.capacity - block.size ));
					off = 0;
				}

				_bookmarks.resize( i );
				return;
			}
		}
	}
	
/*
=================================================
	Commit
=================================================
*/
	template <typename A, uint B>
	bool  StackAllocator<A,B,false>::Commit (Bookmark bm, Bytes size)
	{
		if_likely( not _bookmarks.empty() and _bookmarks.back() == bm )
		{
			auto[idx, off]	= _UnpackBookmark( bm );
			auto&	block	= _blocks.back();

			block.size = Bytes{off} + size;
			
			DEBUG_ONLY( DbgInitMem( block.ptr + block.size, block.capacity - block.size ));
			return true;
		}
		return false;
	}

/*
=================================================
	Discard
=================================================
*/
	template <typename A, uint B>
	void  StackAllocator<A,B,false>::Discard ()
	{
		for (auto& block : _blocks)
		{
			block.size = 0_b;
			DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));
		}
		_bookmarks.clear();
	}
	
/*
=================================================
	Release
=================================================
*/
	template <typename A, uint B>
	void  StackAllocator<A,B,false>::Release ()
	{
		for (auto& block : _blocks) {
			_alloc.Deallocate( block.ptr, block.capacity, _PtrAlign );
		}
		_blocks.clear();
		_bookmarks.clear();
	}
	
/*
=================================================
	_UnpackBookmark
=================================================
*/
	template <typename A, uint B>
	Pair<usize, usize>  StackAllocator<A,B,false>::_UnpackBookmark (Bookmark bm)
	{
		usize	u	= BitCast<usize>( bm );
		usize	idx	= u & _BlockIndexMask;
		usize	off	= u >> _PtrOffset;
		return { idx, off };
	}

} // AE::Base
