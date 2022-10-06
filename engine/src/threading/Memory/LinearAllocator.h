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
	struct LinearAllocator< AllocatorType, MaxBlocks, true > final
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
		Threading::Mutex	_guard;
		BaseAlloc_t			_base;


	// methods
	public:
		LinearAllocator () {}
		
		explicit LinearAllocator (const Allocator_t &alloc) : _base{ alloc }
		{}
		
		LinearAllocator (Self &&other)
		{
			std::scoped_lock	lock{ _guard, other._guard };
			_base = RVRef(other._base);
		}

		LinearAllocator (const Self &) = delete;

		Self& operator = (const Self &) = delete;


		Self& operator = (Self &&rhs)
		{
			std::scoped_lock	lock{ _guard, other._guard };
			_base = RVRef(other._base);
			return *this;
		}


		~LinearAllocator ()
		{
			Release();
		}


		void  SetBlockSize (Bytes size)
		{
			EXLOCK( _guard );
			return _base.SetBlockSize( size );
		}


		ND_ AE_ALLOCATOR void*  Alloc (const Bytes size, const Bytes align)
		{
			EXLOCK( _guard );
			return _base.Alloc( size, align );
		}


		template <typename T>
		ND_ AE_ALLOCATOR T*  Alloc (usize count = 1)
		{
			EXLOCK( _guard );
			return _base.Alloc<T>( count );
		}


		void  Discard ()
		{
			EXLOCK( _guard );
			return _base.Discard();
		}

		void  Release ()
		{
			EXLOCK( _guard );
			return _base.Release();
		}
	};


}	// AE::Base
