// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	https://developer.apple.com/library/archive/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/Render-Ctx/Render-Ctx.html
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MCommandBatch.h"
# include "graphics/Metal/Commands/MCommandBuffer.h"

// implementation
# include "graphics/Private/DrawCommandBatch.h"

#endif // AE_ENABLE_METAL
