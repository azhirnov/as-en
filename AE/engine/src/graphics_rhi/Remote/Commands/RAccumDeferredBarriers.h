// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/CommandBuffer.h"
# include "graphics_rhi/Remote/Commands/RBarrierManager.h"

// implementation
# include "graphics_rhi/Private/AccumDeferredBarriers.h"

#endif // AE_ENABLE_REMOTE_GRAPHICS
