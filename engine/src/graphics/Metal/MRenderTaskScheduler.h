// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "threading/Containers/LfIndexedPool2.h"

# include "graphics/Metal/Commands/MCommandBuffer.h"
# include "graphics/Metal/Commands/MCommandBatch.h"
# include "graphics/Metal/Commands/MDrawCommandBatch.h"
# include "graphics/Metal/Commands/MImageOpHelper.h"
# include "graphics/Metal/MResourceManager.h"

// implementation
# include "graphics/Private/RenderTaskScheduler.h"

#endif // AE_ENABLE_METAL
