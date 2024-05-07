// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Graphics.h"
#include "pch/Platform.h"
#include "pch/VFS.h"
#include "graphics_test/GraphicsTest.h"

#ifdef AE_ENABLE_VULKAN
# include "VulkanSyncLog.h"
#endif
#include "RenderGraph/cpp/types.h"

#include "../shared/UnitTest_Shared.h"

using namespace AE::Graphics;
