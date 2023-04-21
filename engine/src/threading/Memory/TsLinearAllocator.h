// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/LinearAllocator.h"
#include "threading/Common.h"

namespace AE::Base
{

	//
	// Linear Allocator
	//

	template <typename AllocatorType, uint MaxBlocks>
	class LinearAllocator< AllocatorType, MaxBlocks, true > final : public MovableOnly
	{
	// types
	private:
		using BaseAlloc_t	= LinearAllocator< AllocatorType, MaxBlocks, false >;
	public:
		using Allocator_t	= AllocatorType;
		using Self			= LinearAllocator< AllocatorType, MaxBlocks, true >;
		
		static constexpr bool	IsThreadSafe = true;


	// variables
	private:
		Mutex				_guard;
		BaseAlloc_t			_base;


	// methods
	public:
		LinearAllocator ()										__NE___ {}
		explicit LinearAllocator (const Allocator_t &alloc)		__NE___ : _base{ alloc } {}
		explicit LinearAllocator (Bytes blockSize)				__NE___ : _base{ blockSize } {}

		~LinearAllocator ()										__NE___	{ Release(); }
		

		LinearAllocator (Self &&other)							__NE___
		{
			EXLOCK( _guard, other._guard );
			_base = RVRef(other._base);
		}
		
		Self& operator = (Self &&rhs)							__NE___
		{
			EXLOCK( _guard, rhs._guard );
			_base = RVRef(rhs._base);
			return *this;
		}


		void  SetBlockSize (Bytes size)							__NE___
		{
			EXLOCK( _guard );
			return _base.SetBlockSize( size );
		}


		ND_ void*  Allocate (const SizeAndAlign sizeAndAlign)	__NE___
		{
			EXLOCK( _guard );
			return _base.Allocate( sizeAndAlign );
		}


		template <typename T>
		ND_ T*  Allocate (usize count = 1)						__NE___
		{
			EXLOCK( _guard );
			return _base.Allocate<T>( count );
		}
		
		void  Deallocate (void *, Bytes)						__NE___ {}
		void  Deallocate (void *, const SizeAndAlign)			__NE___ {}


		void  Discard ()										__NE___
		{
			EXLOCK( _guard );
			return _base.Discard();
		}

		void  Release ()										__NE___
		{
			EXLOCK( _guard );
			return _base.Release();
		}
	};


} // AE::Base
