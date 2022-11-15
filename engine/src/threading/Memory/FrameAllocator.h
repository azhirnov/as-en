// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorRef.h"
#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

	//
	// GraphicsFrameAllocatorRef
	//

	struct GraphicsFrameAllocatorRef final : AllocatorRef2< MemoryManagerImpl::FrameAllocator_t >
	{
		GraphicsFrameAllocatorRef () :
			AllocatorRef2{ MemoryManager().GetGraphicsFrameAllocator().Get() }
		{}
	};


	//
	// FrameStdAllocatorRef
	//

	template <typename T, typename BaseAlloc>
	struct FrameStdAllocatorRef final : StdAllocatorRef< T, MemoryManagerImpl::FrameAllocator_t* >
	{
		using Base_t = StdAllocatorRef< T, MemoryManagerImpl::FrameAllocator_t* >;
		
		FrameStdAllocatorRef () :
			Base_t{ &BaseAlloc{}.GetAllocatorRef() }
		{}

		FrameStdAllocatorRef (MemoryManagerImpl::FrameAllocator_t* alloc) :
			Base_t{ alloc }
		{}
	};


} // AE::Threading
