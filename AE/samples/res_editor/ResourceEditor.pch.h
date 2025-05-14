// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "PipelineCompiler.pch.h"

#include "pch/Platform.h"
#include "pch/Graphics.h"
#include "pch/Profiler.h"
#include "pch/Video.h"

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi_shared/GraphicsLib.h"
#endif

#include "res_loaders/AllImages/AllImageLoaders.h"
#include "res_loaders/AllImages/AllImageSavers.h"
#include "res_loaders/Intermediate/IntermImage.h"

#include "platform/Remote/RemoteInputActions.h"

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# define RmG_UI_ON_HOST		1
#else
# define RmG_UI_ON_HOST		0
#endif

// disable clear screen and present if not used
#define RE_PROFILING	1
