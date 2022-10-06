// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	This lock-free container has some limitations:
	- write access by same index must be externally synchronized.
	- read access is safe after flush & invalidation (synchronization).
	- Release() must be externally synchronized with all other read and write accesses.
	- Unassign() must be externally synchronized with all other read and write accesses to the 'index'.
	- custom allocator must be thread safe (use mutex or lock-free algorithms).

	This lock-free container designed for large number of elements.
*/

#pragma once

#ifndef AE_LFAS_ENABLED
# include "base/Containers/FixedArray.h"
# include "base/CompileTime/Math.h"
# include "base/Math/BitMath.h"
# include "base/Memory/UntypedAllocator.h"
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/Atomic.h"
#endif

namespace AE::Threading
{
	
	//
	// Lock-Free Indexed Pool
	//

	template <typename ValueType,
			  typename IndexType,
			  usize ChunkSize_v = 256,
			  usize MaxChunks_v = 16,
			  typename AllocatorType = UntypedAlignedAllocator
			 >
	struct LfIndexedPool2 final
	{
		STATIC_ASSERT( ChunkSize_v > 0 );
		STATIC_ASSERT( ((ChunkSize_v % 32) == 0) or ((ChunkSize_v % 64) == 0) );
		STATIC_ASSERT( ChunkSize_v <= 64*64 );
		STATIC_ASSERT( MaxChunks_v > 0 and MaxChunks_v <= 64 );
		STATIC_ASSERT( IsPowerOfTwo( ChunkSize_v ));	// must be power of 2 to increase performance
		STATIC_ASSERT( AllocatorType::IsThreadSafe );
		STATIC_ASSERT( MaxValue<IndexType>() >= (ChunkSize_v * MaxChunks_v) );

	// types
	public:
		using Self				= LfIndexedPool2< ValueType, IndexType, ChunkSize_v, MaxChunks_v, AllocatorType >;
		using Index_t			= IndexType;
		using Value_t			= ValueType;
		using Allocator_t		= AllocatorType;

	private:
		static constexpr usize	ChunkSize		= ChunkSize_v;
		static constexpr usize	MaxChunks		= MaxChunks_v;
		static constexpr usize	LowLevel_Count	= (ChunkSize <= 32 ? 32 : 64);
		static constexpr usize	HiLevel_Count	= Max( 1u, (ChunkSize + LowLevel_Count - 1) / LowLevel_Count );
		static constexpr uint	TopWaitCount	= 4;
		static constexpr uint	HighWaitCount	= 8;

		using LowLevelBits_t	= Conditional< (LowLevel_Count <= 32), uint, ulong >;
		using LowLevels_t		= StaticArray< Atomic< LowLevelBits_t >, HiLevel_Count >;
		using HiLevelBits_t		= Conditional< (HiLevel_Count <= 32), uint, ulong >;

		struct alignas(AE_CACHE_LINE) ChunkInfo
		{
			SpinLockRelaxed				guard;		// only for 'hiLevel' modification
			Atomic< HiLevelBits_t >		hiLevel;	// 0 - is unassigned bit, 1 - assigned bit
			LowLevels_t					lowLevel;	// 0 - is unassigned bit, 1 - assigned bit
			LowLevels_t					created;	// 0 - uninitialized bit, 1 - constructor has been called
		};

		using ChunkInfos_t		= StaticArray< ChunkInfo, MaxChunks >;
		using ValueChunk_t		= StaticArray< Value_t, ChunkSize >;
		using ChunkData_t		= StaticArray< Atomic< ValueChunk_t *>, MaxChunks >;
		using TopLevelBits_t	= Conditional< (MaxChunks <= 32), uint, ulong >;
		
		static constexpr HiLevelBits_t	MaxHighLevel		= ToBitMask<HiLevelBits_t>( HiLevel_Count );
		static constexpr HiLevelBits_t	InitialHighLevel	= ~MaxHighLevel;

		static constexpr TopLevelBits_t	MaxTopLevel			= ToBitMask<TopLevelBits_t>( MaxChunks );
		static constexpr TopLevelBits_t	InitialTopLevel		= ~MaxTopLevel;


	// variables
	private:
		alignas(AE_CACHE_LINE) SpinLockRelaxed	_topLevelGuard;		// only for '_topLevel' modification
		Atomic< TopLevelBits_t >				_topLevel;			// 0 - is unassigned bit, 1 - assigned bit

		ChunkInfos_t		_chunkInfo;
		ChunkData_t			_chunkData;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;


	// methods
	public:
		LfIndexedPool2 (const Self &) = delete;
		LfIndexedPool2 (Self &&) = delete;

		Self& operator = (const Self &) = delete;
		Self& operator = (Self &&) = delete;
		

		explicit LfIndexedPool2 (const Allocator_t &alloc = Default);
		~LfIndexedPool2 ()												{ Release(); }
	
		template <typename FN>
		void Release (FN &&dtor, bool checkForAssigned);
		void Release (bool checkForAssigned = true)						{ return Release( [](Value_t& value) { value.~Value_t(); }, checkForAssigned ); }
		
		template <typename FN>
		ND_ bool  Assign (OUT Index_t &outIndex, FN &&ctor);
		ND_ bool  Assign (OUT Index_t &outIndex)						{ return Assign( OUT outIndex, [](Value_t* ptr, Index_t) { PlacementNew<Value_t>( ptr ); }); }
		
		template <typename FN>
		ND_ bool  AssignAt (Index_t index, OUT Value_t* &outValue, FN &&ctor);
		ND_ bool  AssignAt (Index_t index, OUT Value_t* &outValue)		{ return AssignAt( index, OUT outValue, [](Value_t* ptr, Index_t) { PlacementNew<Value_t>( ptr ); }); }

			bool  Unassign (Index_t index);
	
		ND_ bool  IsAssigned (Index_t index);

		ND_ Value_t&  operator [] (Index_t index);
		
		ND_ static constexpr usize  capacity ()							{ return ChunkSize * MaxChunks; }
		
		ND_ static constexpr Bytes  MaxSize ()							{ return (MaxChunks * SizeOf<ValueChunk_t>) + sizeof(Self); }


	private:
		template <typename FN>
		bool  _AssignInChunk (OUT Index_t &outIndex, int chunkIndex, const FN &ctor);

		template <typename FN>
		bool  _AssignInLowLevel (OUT Index_t &outIndex, int chunkIndex, int hiLevelIndex, ValueChunk_t &data, const FN &ctor);

		void  _UpdateHiLevel (int chunkIndex, int hiLevelIndex);
	};


}	// AE::Threading