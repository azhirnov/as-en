// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	This lock-free container has some limitations:
	 - write access by the same index must be externally synchronized.
	 - read access is safe after flush & invalidation (synchronization).
	 - Release() must be externally synchronized with all other read and write accesses.
	 - Unassign() / UnassignAll() must be externally synchronized with all other read and write accesses to the same 'index'.

	Warning:
	 - This container does not flush/invalidate cache, all atomic operations has relaxed memory order.
	 - This lock-free container designed for large number of elements.

	Pool allocates memory by blocks with 'ChunkSize' elements.

	Elements lifetime:
	 - Elements are created when memory block is allocated.
	 - Assign() doesn't create element, just return index in the pool.
	 - Unassign() doesn't destroy element.
	 - Elements are destroyed only in Release() or in pool destructor.
	 - User can recreate any element using 'Reconstruct()' function.

	Type 'AllocatorType' :
		UntypedAllocator	- static allocator
		AllocatorRef		- as adaptor for RC<IAllocatorTS>
*/

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/DataRaceCheck.h"
#endif

namespace AE::Threading
{

	//
	// Lock-Free Indexed Pool v3
	//

	template <typename ValueType,
			  typename IndexType,
			  usize ChunkSize_v,
			  usize MaxChunks_v,
			  typename AllocatorType = UntypedAllocator
			 >
	class LfIndexedPool final : public Noncopyable
	{
		StaticAssert( ChunkSize_v > 0 );
		StaticAssert( ChunkSize_v <= 64*64 );
		StaticAssert( IsMultipleOf( ChunkSize_v, 32 ) or IsMultipleOf( ChunkSize_v, 64 ));
		StaticAssert( IsPowerOfTwo( ChunkSize_v ));	// must be power of 2 to increase performance
		StaticAssert( IsPowerOfTwo( MaxChunks_v ));	// must be power of 2 to increase performance
		StaticAssert( MaxChunks_v > 0 );
		StaticAssert( MaxValue<IndexType>() >= (ChunkSize_v * MaxChunks_v) );

	// types
	public:
		using Self				= LfIndexedPool< ValueType, IndexType, ChunkSize_v, MaxChunks_v, AllocatorType >;
		using Index_t			= IndexType;
		using Value_t			= ValueType;
		using Allocator_t		= AllocatorType;

	private:
		static constexpr usize	ChunkSize		= ChunkSize_v;
		static constexpr usize	MaxChunks		= MaxChunks_v;
		static constexpr usize	LowLvlCount		= ChunkSize <= 32*32 ? 32 : 64;
		static constexpr usize	HighLvlCount	= ChunkSize / LowLvlCount;
		StaticAssert( LowLvlCount * HighLvlCount == ChunkSize );

		using LowLvlBits_t	= Conditional< (LowLvlCount  < 64), uint, ulong >;
		using HighLvlBits_t	= Conditional< (HighLvlCount < 64), uint, ulong >;

		struct alignas(AE_CACHE_LINE) LowLevelChunk
		{
			Atomic< LowLvlBits_t >					assigned	{0};	// 1 - assigned, 0 - unassigned
			StaticArray< Value_t, LowLvlCount >		values		{};

			LowLevelChunk ()	__NE___	{}
		};

		using LowLvlChunkArray_t	= StaticArray< LowLevelChunk, HighLvlCount >;
		using LowLvlChunkArrayPtr_t	= Atomic< LowLvlChunkArray_t *>;

		static constexpr HighLvlBits_t	MaxHighLevel		= ToBitMask<HighLvlBits_t>( HighLvlCount );
		static constexpr HighLvlBits_t	InitialHighLevel	= ~MaxHighLevel;

		struct alignas(AE_CACHE_LINE) HighLevelChunk
		{
			Atomic< HighLvlBits_t >		available		{InitialHighLevel};		// 1 - chunk is full, 0 - chunk has some empty elements
			SpinLockRelaxed				availableGuard;							// only for 'available' modification
			LowLvlChunkArrayPtr_t		chunksPtr		{null};

			HighLevelChunk ()	__NE___	{}
		};
		using HighLvlArray_t	= StaticArray< HighLevelChunk, MaxChunks >;


	// variables
	private:
		HighLvlArray_t *	_highLvl		= null;
		Atomic<uint>		_highChunkCount	{0};

		Mutex				_allocGuard;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		explicit LfIndexedPool (const Allocator_t &alloc = Allocator_t{}) __NE___;
		~LfIndexedPool ()										__NE___	{ Release( True{"check for assigned"} ); }

			void			Release (Bool checkForAssigned)		__NE___;

			template <typename FN>
			void			UnassignAll (FN &&visitor)			__NE___;

			template <typename FN>
			void			ForEachAssigned (FN &&fn)			C_NE___;

		ND_ auto			Assign ()							__NE___	{ Index_t idx;  Unused( Assign( OUT idx ));  return idx; }
		ND_ bool			Assign (OUT Index_t &outIndex)		__NE___;
			bool			Unassign (Index_t index)			__NE___;
		ND_ bool			IsAssigned (Index_t index)			__NE___;

		ND_ Index_t			IndexOf (const void* ptr)			C_NE___;
			bool			Unassign (const void* ptr)			__NE___;

		ND_ Value_t&		operator [] (Index_t index)			__NE___;
		ND_ Value_t const&	operator [] (Index_t index)			C_NE___	{ return const_cast<Self *>(this)->operator[]( index ); }

		ND_ Value_t *		At (Index_t index)					__NE___;
		ND_ Value_t const*	At (Index_t index)					C_NE___	{ return const_cast<Self *>(this)->At( index ); }


		ND_ static constexpr Bytes  MaxDynamicSize ()			__NE___	{ return SizeOf<HighLvlArray_t> * SizeOf<LowLvlChunkArray_t> * MaxChunks; }
		ND_ static constexpr usize  capacity ()					__NE___	{ return ChunkSize * MaxChunks; }

		ND_ Bytes			DynamicSize ()						C_NE___;

	private:
		ND_ static bool  _AssignInChunk (HighLevelChunk& highChunk, OUT Index_t &outIndex) __NE___;
	};

} // AE::Threading

#include "threading/Containers/LfIndexedPool.inl.h"
