// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/Helpers.h"
#include "base/Math/Math.h"
#include "base/Math/Bytes.h"

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

    class MtAllocator;

    template <typename AllocatorType>
    class AllocatorRef;

    template <typename T, typename AllocatorType>
    class StdAllocatorRef;


    class IAllocator;
    using IAllocatorRef     = AllocatorRef< IAllocator >;


} // AE::Base
