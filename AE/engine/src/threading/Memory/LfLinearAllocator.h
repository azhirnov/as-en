// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/DataRaceCheck.h"
#endif

namespace AE::Threading
{
	using namespace AE::Math;

	//
	// Lock-Free Linear Allocator
	//

	template <usize BlockSize_v		= usize(SmallAllocationSize),	// in bytes
			  usize MemAlign		= AE_CACHE_LINE,				// in bytes
			  usize MaxBlocks_v		= 16,
			  typename AllocatorType = UntypedAllocator
			 >
	class LfLinearAllocator final : public IAllocatorTS
	{
		StaticAssert( BlockSize_v > 0 and IsMultipleOf( BlockSize_v, MemAlign ));
		StaticAssert( MaxBlocks_v > 0 and MaxBlocks_v < 64 );
		StaticAssert( MemAlign > 0 and IsPowerOfTwo( MemAlign ));

	// types
	public:
		using Self			= LfLinearAllocator< BlockSize_v, MemAlign, MaxBlocks_v, AllocatorType >;
		using Index_t		= uint;
		using Allocator_t	= AllocatorType;

	private:
		struct alignas(AE_CACHE_LINE) MemBlock
		{
			Atomic< void *>		mem		{null};
			Atomic< usize >		size	{0};			// <= BlockSize()
		};
		using MemBlocks_t = StaticArray< MemBlock, MaxBlocks_v >;

		static constexpr Bytes	_Capacity {BlockSize_v};
		static constexpr Bytes	_MemAlign {MemAlign};	// TODO: min align as cache line size?


	// variables
	private:
		MemBlocks_t			_blocks	= {};

		Mutex				_allocGuard;

		NO_UNIQUE_ADDRESS
		 Allocator_t		_allocator;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		explicit LfLinearAllocator (const Allocator_t &alloc = Allocator_t{}) __NE___;
		~LfLinearAllocator ()												__NE___	{ Release(); }


		// must be externally synchronized
			void	Release ()												__NE___;

		ND_ Bytes	CurrentSize ()											C_NE___;

		ND_ static constexpr Bytes  BlockSize ()							__NE___	{ return _Capacity; }
		ND_ static constexpr Bytes  MaxSize ()								__NE___	{ return MaxBlocks_v * _Capacity; }
		ND_ static constexpr usize	MaxBlocks ()							__NE___	{ return MaxBlocks_v; }


		// IAllocator //
		ND_ void*	Allocate (const SizeAndAlign sizeAndAlign)				__NE_OV;
			using	IAllocator::Allocate;

		// only for debugging
			void	Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)	__NE_OV	{ Deallocate( ptr, sizeAndAlign.size ); }
			void	Deallocate (void* ptr, const Bytes size)				__NE_OV;
			void	Deallocate (void* ptr)									__NE_OV	{ Deallocate( ptr, 1_b ); }

			void	Discard ()												__NE_OV;
	};


} // AE::Threading

#include "threading/Memory/LfLinearAllocator.inl.h"
