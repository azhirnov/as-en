// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	There are 2 concepts:
	 - allocator as static object which doesn't add 8-byte overhead for pointer to allocator.
	 - allocator as RC<IAllocator>.
*/

#pragma once

#include "base/Utils/Helpers.h"
#include "base/Math/Vec.h"

namespace AE::Base
{
	class UntypedAllocator;

	template <usize BaseAlign>
	class UntypedAllocatorBaseAlign;

	template <typename T>
	class StdAllocator;

	template <typename AllocatorType = UntypedAllocator, uint MaxBlocks = 16, bool ThreadSafe = false>
	class LinearAllocator;

	template <typename AllocatorType = UntypedAllocator, uint MaxBlocks = 16, bool ThreadSafe = false>
	class StackAllocator;


	template <typename AllocatorType>
	class AllocatorRef;

	template <typename T, typename AllocatorType>
	class StdAllocatorRef;


	class IAllocator;
	using IAllocatorRef		= AllocatorRef< IAllocator >;


} // AE::Base
