// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorRef.h"
#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

	//
	// GlobalLinearAllocatorRef
	//

	struct GlobalLinearAllocatorRef final : AllocatorRef2< MemoryManagerImpl::GlobalLinearAllocator_t >
	{
		GlobalLinearAllocatorRef () :
			AllocatorRef2{ MemoryManager().GetGlobalLinearAllocator() }
		{}
	};
	
	//
	// GlobalLinearStdAllocatorRef
	//

	template <typename T>
	struct GlobalLinearStdAllocatorRef final : StdAllocatorRef< T, MemoryManagerImpl::GlobalLinearAllocator_t* >
	{
		using Base_t = StdAllocatorRef< T, MemoryManagerImpl::GlobalLinearAllocator_t* >;
		
		GlobalLinearStdAllocatorRef () :
			Base_t{ &MemoryManager().GetGlobalLinearAllocator() }
		{}

		GlobalLinearStdAllocatorRef (MemoryManagerImpl::GlobalLinearAllocator_t* alloc) :
			Base_t{ alloc }
		{}
	};


#	define AE_GLOBALLY_ALLOC \
	void*  operator new (std::size_t size) \
	{ \
		return AE::MemoryManager().GetGlobalLinearAllocator().Allocate( SizeAndAlign{ Bytes{size}, Bytes{__STDCPP_DEFAULT_NEW_ALIGNMENT__} }); \
	} \
	void*  operator new (std::size_t size, std::align_val_t align) \
	{ \
		return AE::MemoryManager().GetGlobalLinearAllocator().Allocate( SizeAndAlign{ Bytes{size}, Bytes{align} }); \
	} \
	void operator delete (void *) \
	{} \
	void operator delete (void *, std::align_val_t) \
	{} \
	void operator delete (void *, std::size_t) \
	{} \
	void operator delete (void *, std::size_t, std::align_val_t) \
	{}


} // AE::Threading
