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
		Threading::Mutex	_guard;
		BaseAlloc_t			_base;


	// methods
	public:
		LinearAllocator ()									__NE___ {}
		explicit LinearAllocator (const Allocator_t &alloc)	__NE___ : _base{ alloc } {}

		~LinearAllocator ()									__NE___	{ Release(); }
		

		LinearAllocator (Self &&other)						__NE___
		{
			EXLOCK( _guard, other._guard );
			_base = RVRef(other._base);
		}
		
		Self& operator = (Self &&rhs)						__NE___
		{
			EXLOCK( _guard, other._guard );
			_base = RVRef(other._base);
			return *this;
		}


		void  SetBlockSize (Bytes size)						__NE___
		{
			EXLOCK( _guard );
			return _base.SetBlockSize( size );
		}


		ND_ void*  Alloc (const SizeAndAlign sizeAndAlign)	__NE___
		{
			EXLOCK( _guard );
			return _base.Alloc( sizeAndAlign );
		}


		template <typename T>
		ND_ T*  Alloc (usize count = 1)						__NE___
		{
			EXLOCK( _guard );
			return _base.Alloc<T>( count );
		}


		void  Discard ()									__NE___
		{
			EXLOCK( _guard );
			return _base.Discard();
		}

		void  Release ()									__NE___
		{
			EXLOCK( _guard );
			return _base.Release();
		}
	};


} // AE::Base
