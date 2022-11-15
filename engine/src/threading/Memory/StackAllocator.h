// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/StackAllocator.h"
#include "threading/Common.h"

namespace AE::Base
{

	//
	// Thread-Safe Stack Allocator
	//
	
	template <typename AllocatorType, uint MaxBlocks>
	struct StackAllocator< AllocatorType, MaxBlocks, true > final
	{
	// types
	private:
		using BaseAllocator_t	= StackAllocator< AllocatorType, MaxBlocks, false >;
	public:
		using Self				= StackAllocator< AllocatorType, MaxBlocks, true >;
		using Allocator_t		= AllocatorType;
		using Bookmark			= typename BaseAllocator_t::Bookmark;
		
		static constexpr bool	IsThreadSafe = true;


	// variables
	private:
		Threading::Mutex	_guard;
		BaseAllocator_t		_base;


	// methods
	public:
		StackAllocator () __NE___ {}
		
		explicit StackAllocator (const Allocator_t &alloc) __NE___ : _base{ alloc }
		{}
		
		StackAllocator (Self &&other) __NE___
		{
			SAFE_EXLOCK( _guard, other._guard );
			_base = RVRef(other._base);
		}

		StackAllocator (const Self &) = delete;

		Self&  operator = (const Self &) = delete;


		Self&  operator = (Self &&rhs) __NE___
		{
			SAFE_EXLOCK( _guard, rhs._guard );
			_base = RVRef(rhs._base);
			return *this;
		}


		~StackAllocator () __NE___
		{
			Release();
		}


		void  SetBlockSize (Bytes size) __NE___
		{
			EXLOCK( _guard );
			return _base.SetBlockSize( size );
		}


		ND_ AE_ALLOCATOR void*  Allocate (const SizeAndAlign sizeAndAlign) __NE___
		{
			EXLOCK( _guard );
			return _base.Allocate( sizeAndAlign );
		}


		template <typename T>
		ND_ AE_ALLOCATOR T*  Allocate (usize count = 1) __NE___
		{
			EXLOCK( _guard );
			return _base.template Allocate<T>( count );
		}
		

		void  Deallocate (void* ptr, const SizeAndAlign sizeAndAlign) __NE___
		{
			Unused( ptr, sizeAndAlign );
		}


		ND_ Bookmark  Push () __NE___
		{
			EXLOCK( _guard );
			return _base.Push();
		}


		void  Pop (Bookmark bm) __NE___
		{
			EXLOCK( _guard );
			return _base.Pop( bm );
		}


		void  Discard () __NE___
		{
			EXLOCK( _guard );
			return _base.Discard();
		}

		void  Release () __NE___
		{
			EXLOCK( _guard );
			return _base.Release();
		}
	};

} // AE::Base
