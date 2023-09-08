// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    hint:
        upload      - write (on host)   - from host to device
        readback    - read  (on host)   - from device to host

    Upload staging buffer must use coherent memory and doesn't require FlushMemoryRanges.
    Readback staging buffer can use cached non-coherent memory.
    Vertex buffer use unified memory and doesn't require FlushMemoryRanges.
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VDevice.h"

// implementation
# include "graphics/Private/StagingBufferManager.h"

#endif // AE_ENABLE_VULKAN
