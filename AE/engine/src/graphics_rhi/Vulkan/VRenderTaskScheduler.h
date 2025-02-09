// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics_rhi/Vulkan/Commands/VCommandBatch.h"
# include "graphics_rhi/Vulkan/Commands/VDrawCommandBatch.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"

// implementation
# include "graphics_rhi/Private/RenderTaskScheduler.h"

#endif // AE_ENABLE_VULKAN
