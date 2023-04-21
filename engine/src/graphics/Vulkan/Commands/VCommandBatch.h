// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics::_hidden_
{
	class VAccumBarriersForTask;
}

// implementation
# include "graphics/Private/CommandBatch.h"

#endif // AE_ENABLE_VULKAN
