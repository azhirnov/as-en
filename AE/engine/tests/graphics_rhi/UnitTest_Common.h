// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/GraphicsRHI.h"
#include "pch/Platform.h"
#include "pch/VFS.h"
#include "graphics_test/GraphicsTest.h"

#ifdef AE_ENABLE_VULKAN
# include "vulkan_sync_log/VulkanSyncLog.h"
#endif
#include "RenderGraph/cpp/types.h"

#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Graphics;
