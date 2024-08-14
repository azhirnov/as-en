// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "PipelineCompiler.pch.h"

#include "pch/Platform.h"
#include "pch/GraphicsHL.h"
#include "pch/Profiler.h"
#include "pch/Video.h"

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_lib/GraphicsLib.h"
#endif

#include "res_loaders/AllImages/AllImageLoaders.h"
#include "res_loaders/AllImages/AllImageSavers.h"
#include "res_loaders/Intermediate/IntermImage.h"

#include "platform/Remote/RemoteInputActions.h"

#ifdef AE_ENABLE_VULKAN
# define PIPELINE_STATISTICS	0	// 0 or 1
#else
# define PIPELINE_STATISTICS	0	// not supported
#endif

#define OPTIMIZE_SHADER		1
