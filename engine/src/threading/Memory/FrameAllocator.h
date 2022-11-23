// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorRef.h"
#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

	//
	// GraphicsFrameAllocatorRef
	//

	class GraphicsFrameAllocatorRef final : public AllocatorRef2< MemoryManagerImpl::FrameAllocator_t >
	{
	public:
		GraphicsFrameAllocatorRef () __NE___ :
			AllocatorRef2{ MemoryManager().GetGraphicsFrameAllocator().Get() }
		{}
	};


	//
	// FrameStdAllocatorRef
	//

	template <typename T, typename BaseAlloc>
	class FrameStdAllocatorRef final : public StdAllocatorRef< T, MemoryManagerImpl::FrameAllocator_t* >
	{
		using Base_t = StdAllocatorRef< T, MemoryManagerImpl::FrameAllocator_t* >;
	
	public:
		FrameStdAllocatorRef () __NE___ :
			Base_t{ &BaseAlloc{}.GetAllocatorRef() }
		{}

		explicit FrameStdAllocatorRef (MemoryManagerImpl::FrameAllocator_t* alloc) __NE___:
			Base_t{ alloc }
		{}
	};


} // AE::Threading
