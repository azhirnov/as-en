// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Metal/Commands/MCommandBuffer.h"
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics::_hidden_
{
    class MAccumBarriersForTask;
}

// implementation
# include "graphics/Private/CommandBatch.h"

#endif // AE_ENABLE_METAL
