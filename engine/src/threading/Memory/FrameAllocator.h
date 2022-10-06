// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorRef.h"
#include "threading/Memory/MemoryManager.h"

namespace AE::Threading
{

	//
	// GraphicsFrameAllocatorRef
	//

	struct GraphicsFrameAllocatorRef final : AlignedAllocatorRef< MemoryManager::FrameAllocator_t >
	{
		GraphicsFrameAllocatorRef () :
			AlignedAllocatorRef{ MemoryManagerInstance().GetGraphicsFrameAllocator().Get() }
		{}
	};


	//
	// FrameStdAllocatorRef
	//

	template <typename T, typename BaseAlloc>
	struct FrameStdAllocatorRef final : StdAllocatorRef< T, MemoryManager::FrameAllocator_t* >
	{
		using Base_t = StdAllocatorRef< T, MemoryManager::FrameAllocator_t* >;
		
		FrameStdAllocatorRef () :
			Base_t{ &BaseAlloc{}.GetAllocatorRef() }
		{}

		FrameStdAllocatorRef (MemoryManager::FrameAllocator_t* alloc) :
			Base_t{ alloc }
		{}
	};


} // AE::Threading
