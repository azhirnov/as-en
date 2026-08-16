// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics_rhi/Vulkan/Commands/VCommandBatch.h"
# include "graphics_rhi/Vulkan/Commands/VDrawCommandBatch.h"

// implementation
# include "graphics_rhi/Private/RenderTaskScheduler.h"

#endif // AE_ENABLE_VULKAN
