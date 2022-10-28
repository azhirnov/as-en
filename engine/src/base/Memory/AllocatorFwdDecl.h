// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{
	struct UntypedAllocator;
	struct UntypedAlignedAllocator;
	struct UntypedOnStackAllocator;	// may be unsafe!
	
	template <usize BaseAlign>
	struct UntypedAllocatorBaseAlign;

	template <typename T>
	struct StdAllocator;

	template <typename AllocatorType, uint MaxBlocks, bool ThreadSafe>
	struct LinearAllocator;

	template <typename AllocatorType, uint MaxBlocks, bool ThreadSafe>
	struct StackAllocator;

	struct MtAllocator;
	
	template <typename AllocatorType>
	struct AllocatorRef;
	
	template <typename AllocatorType>
	struct AlignedAllocatorRef;

	template <typename T, typename AllocatorType>
	struct StdAllocatorRef;


	class IAllocator;
	class IAlignedAllocator;
	
	using IAllocatorRef			= AllocatorRef< IAllocator >;
	using IAlignedAllocatorRef	= AlignedAllocatorRef< IAlignedAllocator >;


} // AE::Base
