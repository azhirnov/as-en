// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorRef.h"
#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

	//
	// GlobalLinearAllocatorRef
	//

	struct GlobalLinearAllocatorRef final : AlignedAllocatorRef< MemoryManager::GlobalLinearAllocator_t >
	{
		GlobalLinearAllocatorRef () :
			AlignedAllocatorRef{ MemoryManagerInstance().GetGlobalLinearAllocator() }
		{}
	};
	
	//
	// GlobalLinearStdAllocatorRef
	//

	template <typename T>
	struct GlobalLinearStdAllocatorRef final : StdAllocatorRef< T, MemoryManager::GlobalLinearAllocator_t* >
	{
		using Base_t = StdAllocatorRef< T, MemoryManager::GlobalLinearAllocator_t* >;
		
		GlobalLinearStdAllocatorRef () :
			Base_t{ &MemoryManagerInstance().GetGlobalLinearAllocator() }
		{}

		GlobalLinearStdAllocatorRef (MemoryManager::GlobalLinearAllocator_t* alloc) :
			Base_t{ alloc }
		{}
	};


#	define AE_GLOBALLY_ALLOC \
	void*  operator new (std::size_t size) \
	{ \
		return AE::Threading::MemoryManagerInstance().GetGlobalLinearAllocator().Allocate( Bytes{size}, Bytes{__STDCPP_DEFAULT_NEW_ALIGNMENT__} ); \
	} \
	void*  operator new (std::size_t size, std::align_val_t align) \
	{ \
		return AE::Threading::MemoryManagerInstance().GetGlobalLinearAllocator().Allocate( Bytes{size}, Bytes{align} ); \
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
