// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/CommandBufferVulkan.h"
# include "graphics_rhi/Vulkan/Commands/VBarrierManager.h"

// implementation
# include "graphics_rhi/Private/AccumDeferredBarriers.h"

#endif // AE_ENABLE_VULKAN
