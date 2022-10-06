// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "base/CompileTime/Math.h"
# include "base/Math/BitMath.h"
# include "base/Memory/UntypedAllocator.h"
# include "threading/Primitives/Atomic.h"
# include "threading/Primitives/SpinLock.h"
#endif

namespace AE::Threading
{

	//
	// Lock-Free Indexed Pool v3
	//

	template <typename ValueType,
			  typename IndexType,
			  usize ChunkSize_v = 32*32,
			  usize MaxChunks_v = 8,
			  typename AllocatorType = UntypedAlignedAllocator
			 >
	struct LfIndexedPool3 final
	{
		STATIC_ASSERT( ChunkSize_v > 0 );
		STATIC_ASSERT( ChunkSize_v <= 64*64 );
		STATIC_ASSERT( ((ChunkSize_v % 32) == 0) or ((ChunkSize_v % 64) == 0) );
		STATIC_ASSERT( IsPowerOfTwo( ChunkSize_v ));	// must be power of 2 to increase performance
		STATIC_ASSERT( IsPowerOfTwo( MaxChunks_v ));	// must be power of 2 to increase performance
		STATIC_ASSERT( MaxChunks_v > 0 );
		STATIC_ASSERT( MaxValue<IndexType>() >= (ChunkSize_v * MaxChunks_v) );
		STATIC_ASSERT( AllocatorType::IsThreadSafe );
		
	// types
	public:
		using Self				= LfIndexedPool3< ValueType, IndexType, ChunkSize_v, MaxChunks_v, AllocatorType >;
		using Index_t			= IndexType;
		using Value_t			= ValueType;
		using Allocator_t		= AllocatorType;

	private:
		static constexpr usize	ChunkSize		= ChunkSize_v;
		static constexpr usize	MaxChunks		= MaxChunks_v;
		static constexpr usize	LowLvlCount		= ChunkSize <= 32*32 ? 32 : 64;
		static constexpr usize	HighLvlCount	= ChunkSize / LowLvlCount;
		STATIC_ASSERT( LowLvlCount * HighLvlCount == ChunkSize );

		using LowLvlBits_t	= Conditional< LowLvlCount < 64, uint, ulong >;
		using HighLvlBits_t	= Conditional< HighLvlCount < 64, uint, ulong >;

		struct alignas(AE_CACHE_LINE) LowLevelChunk
		{
			Atomic< LowLvlBits_t >					assigned	{0};	// 1 - assigned, 0 - unassigned
			StaticArray< Value_t, LowLvlCount >		values		{};
		};

		using LowLvlChunkArray_t	= StaticArray< LowLevelChunk, HighLvlCount >;
		using LowLvlChunkArrayPtr_t	= Atomic< LowLvlChunkArray_t *>;
		
		static constexpr HighLvlBits_t	MaxHighLevel		= ToBitMask<HighLvlBits_t>( HighLvlCount );
		static constexpr HighLvlBits_t	InitialHighLevel	= ~MaxHighLevel;

		struct alignas(AE_CACHE_LINE) HighLevelChunk
		{
			Atomic< HighLvlBits_t >		available	{InitialHighLevel};	// 1 - chunk is full, 0 - chunk has some empty elements
			SpinLockRelaxed				guard;							// only for 'available' modification
			LowLvlChunkArrayPtr_t		chunksPtr	{null};
		};
		using HighLvlArray_t	= StaticArray< HighLevelChunk, MaxChunks >;


	// variables
	private:
		HighLvlArray_t *	_highLvl		= null;
		Atomic<uint>		_highChunkCount	{0};
		
		Mutex				_allocGuard;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;


	// methods
	public:
		LfIndexedPool3 (const Self &) = delete;
		LfIndexedPool3 (Self &&) = delete;

		Self& operator = (const Self &) = delete;
		Self& operator = (Self &&) = delete;
		

		explicit LfIndexedPool3 (const Allocator_t &alloc = Allocator_t{});

		~LfIndexedPool3 ()		{ Release(); }
		

		void  Release (bool checkForAssigned = true);
		
		template <typename FN>
		void  UnassignAll (FN &&visitor);
		
		ND_ auto  Assign ()										{ Index_t idx;  return Assign( OUT idx ) ? idx : UMax; }
		ND_ bool  Assign (OUT Index_t &outIndex);
			bool  Unassign (Index_t index);
		ND_ bool  IsAssigned (Index_t index);

		ND_ Value_t&		operator [] (Index_t index);
		ND_ Value_t const&	operator [] (Index_t index) const	{ return const_cast<Self *>(this)->operator[]( index ); }
		
		ND_ Value_t *		At (Index_t index);
		ND_ Value_t const*	At (Index_t index) const	{ return const_cast<Self *>(this)->At( index ); }


		ND_ static constexpr Bytes  MaxDynamicSize ()	{ return SizeOf<HighLvlArray_t> * SizeOf<LowLvlChunkArray_t> * MaxChunks; }
		ND_ static constexpr usize  capacity ()			{ return ChunkSize * MaxChunks; }

		ND_ Bytes  DynamicSize () const;

	private:
		ND_ static bool  _AssignInChunk (HighLevelChunk& highChunk, OUT Index_t &outIndex);
	};


}	// AE::Threading
