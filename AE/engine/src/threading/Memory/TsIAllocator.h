// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Common.h"

namespace AE::Threading
{

	//
	// Thread-safe Allocator implementation
	//

	class TsIAllocator final : public IAllocatorTS
	{
	// variables
	private:
		Mutex				_guard;
		RC<IAllocator>		_impl;


	// methods
	public:
		explicit TsIAllocator (RC<IAllocator> alloc)					__NE___;
		~TsIAllocator ()												__NE_OV;

	// IAllocator //
		void*	Allocate (Bytes size)									__NE_OV;
		void	Deallocate (void* ptr, Bytes size)						__NE_OV;
		void	Deallocate (void* ptr)									__NE_OV;

		void*	Allocate (const SizeAndAlign sizeAndAlign)				__NE_OV;
		void	Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)	__NE_OV;
	};



	inline TsIAllocator::TsIAllocator (RC<IAllocator> alloc) __NE___ :
		_impl{ RVRef(alloc) }
	{
		ASSERT( _impl );

		// don't use 'TsIAllocator' on top of 'IAllocatorTS'
		ASSERT( CastNotAllowed<IAllocatorTS>(_impl.get()) );
	}

	inline TsIAllocator::~TsIAllocator () __NE___
	{
		EXLOCK( _guard );
		_impl = null;
	}

	inline void*  TsIAllocator::Allocate (Bytes size) __NE___
	{
		EXLOCK( _guard );
		return _impl->Allocate( size );
	}

	inline void  TsIAllocator::Deallocate (void* ptr, Bytes size) __NE___
	{
		EXLOCK( _guard );
		return _impl->Deallocate( ptr, size );
	}

	inline void  TsIAllocator::Deallocate (void* ptr) __NE___
	{
		EXLOCK( _guard );
		return _impl->Deallocate( ptr );
	}

	inline void*  TsIAllocator::Allocate (const SizeAndAlign sizeAndAlign) __NE___
	{
		EXLOCK( _guard );
		return _impl->Allocate( sizeAndAlign );
	}

	inline void  TsIAllocator::Deallocate (void* ptr, const SizeAndAlign sizeAndAlign) __NE___
	{
		EXLOCK( _guard );
		return _impl->Deallocate( ptr, sizeAndAlign );
	}


} // AE::Threading
