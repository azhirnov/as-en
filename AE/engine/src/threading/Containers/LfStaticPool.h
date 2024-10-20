// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	This class uses lock-free algorithm to put and extract values without any order (but not really in random order).
	You can use 'Put', 'Extract' methods without any syncs.
	'Release' method must be synchronized with 'Put' and 'Extract' methods.
*/

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Common.h"
# include "threading/Primitives/DataRaceCheck.h"
#endif

namespace AE::Threading
{

	//
	// Lock-free Static Pool
	//

	template <typename ValueType,
			  usize Count,
			  typename AllocatorType = UntypedAllocator
			 >
	class LfStaticPool final : public Noncopyable
	{
		StaticAssert( Count > 0 and IsMultipleOf( Count, 32 ));

	// types
	public:
		using Self			= LfStaticPool< ValueType, Count, AllocatorType >;
		using Index_t		= uint;
		using Value_t		= ValueType;
		using Allocator_t	= AllocatorType;

	private:
		static constexpr usize	ChunkSize	= Count < 32*12 ? 32 : 64;
		static constexpr usize	ChunksCount	= Count / ChunkSize;

		StaticAssert( IsMultipleOf( Count, ChunkSize ));

		using Bitfield_t	= Conditional< (ChunkSize <= 32), uint, ulong >;

		struct alignas(AE_CACHE_LINE) Chunk
		{
			Atomic<Bitfield_t>	assigned	{0};	// 1 - assigned,  0 - unassigned	- indicates that object is locked for creation
			Atomic<Bitfield_t>	available	{0};	// 1 - available, 0 - unavailable	- indicates that object is available for reading
			union {
				Value_t			values  [ ChunkSize ];
				ubyte			arrData [ ChunkSize * sizeof(Value_t) ];	// to avoid ctor for 'Value_t'
			};

			Chunk ()	__NE___ { DEBUG_ONLY( DbgInitMem( values, Sizeof(values) )); }
			~Chunk ()	__NE___ {}
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
		StaticAssert( ThreadToChunkMask < ChunksCount );


	// variables
	private:
		ChunkArray_t*		_arr		= null;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		explicit LfStaticPool (const Allocator_t &alloc = Allocator_t{}) __NE___;
		~LfStaticPool ()							__NE___	{ Release(); }

		ND_ static constexpr usize  Capacity ()		__NE___	{ return Count; }
		ND_ static constexpr Bytes  DynamicSize ()	__NE___	{ return SizeOf<ChunkArray_t>; }

			void  Release ()						__NE___	{ return Release( [](Value_t &value) __NE___ { value.~Value_t(); }); }

			template <typename FN>
			void  Release (FN &&fn)					__NE___;

		template <typename T>
		ND_ bool  Put (T &&value)					__NE___;

			bool  Extract (OUT Value_t &outValue)	__NE___;
	};

} // AE::Threading

#include "threading/Containers/LfStaticPool.inl.h"
