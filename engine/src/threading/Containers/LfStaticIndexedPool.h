// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Warning:
		This container does not flush/invalidate cache, all atomic operations has relaxed memory order.
*/

#pragma once

#ifndef AE_LFAS_ENABLED
# include "base/CompileTime/Math.h"
# include "base/Memory/UntypedAllocator.h"
# include "threading/Primitives/Atomic.h"
#endif
#include "base/Platforms/ThreadUtils.h"

namespace AE::Threading
{

	//
	// Lock-free Static Indexed Pool
	//
	
	template <typename ValueType,
			  typename IndexType,
			  usize Count,
			  typename AllocatorType = UntypedAlignedAllocator
			 >
	struct LfStaticIndexedPool final
	{
		STATIC_ASSERT( Count > 0 and Count % 32 == 0 );
		STATIC_ASSERT( MaxValue<IndexType>() >= Count );
		STATIC_ASSERT( AllocatorType::IsThreadSafe );

	// types
	public:
		using Self			= LfStaticIndexedPool< ValueType, IndexType, Count, AllocatorType >;
		using Index_t		= IndexType;
		using Value_t		= ValueType;
		using Allocator_t	= AllocatorType;

	private:
		static constexpr usize	ChunkSize	= Count < 32*12 ? 32 : 64;
		static constexpr usize	ChunksCount	= Count / ChunkSize;
		
		STATIC_ASSERT( Count % ChunkSize == 0 );

		using Bitfield_t	= Conditional< (ChunkSize <= 32), uint, ulong >;
		
		struct alignas(AE_CACHE_LINE) Chunk
		{
			Atomic< Bitfield_t >				assigned	{0};	// 1 - assigned, 0 - unassigned
			StaticArray< Value_t, ChunkSize >	arr			{};
		};
		using ChunkArray_t	= StaticArray< Chunk, ChunksCount >;
		
		static constexpr uint	NumAttempts			= 4;
		static constexpr usize	ThreadToChunkMask	=	ChunksCount < 4 ?	0 :
														ChunksCount < 8 ?	ToBitMask<usize>( 2u ) :
														ChunksCount < 16 ?	ToBitMask<usize>( 3u ) :
																			ToBitMask<usize>( 4u );
		static constexpr usize	ChunkIdxStep		=	ChunksCount < 4 ?	1 :
														ChunksCount < 10 ?	3 :
																			5;
		STATIC_ASSERT( ThreadToChunkMask < ChunksCount );


	// variables
	private:
		ChunkArray_t *		_arr	= null;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;
		

	// methods
	public:
		LfStaticIndexedPool (const Self &) = delete;
		LfStaticIndexedPool (Self &&) = default;

		Self& operator = (const Self &) = delete;
		Self& operator = (Self &&) = default;

		explicit LfStaticIndexedPool (const Allocator_t &alloc = Allocator_t{});
		~LfStaticIndexedPool ()		{ Release(); }

			void  Release ();

			template <typename FN>
			void  UnassignAll (FN && fn);
		
		ND_ auto  Assign ()										{ Index_t idx;  return Assign( OUT idx ) ? idx : UMax; }
		ND_ bool  Assign (OUT Index_t &outIndex);
			void  Unassign (Index_t index);
		ND_ bool  IsAssigned (Index_t index) const;

		ND_ Value_t&		operator [] (Index_t index);
		ND_ Value_t const&	operator [] (Index_t index) const	{ return const_cast<Self*>(this)->operator[]( index ); }
		
		ND_ Value_t*		At (Index_t index);
		ND_ Value_t const*	At (Index_t index) const			{ return const_cast<Self*>(this)->At( index ); }

		ND_ usize	size ()		const;
		ND_ bool	empty ()	const				{ return size() == 0; }

		ND_ static constexpr usize  capacity ()		{ return Count; }
		ND_ static constexpr Bytes  DynamicSize ()	{ return SizeOf<ChunkArray_t>; }
	};


} // AE::Threading