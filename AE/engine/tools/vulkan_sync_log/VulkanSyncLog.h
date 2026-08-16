// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Vulkan/VulkanLoader.h"

namespace AE::Graphics
{

	//
	// Vulkan Synchronization Logger
	//

	class VulkanSyncLog //: Noninstanceable
	{
	public:
		static void  Initialize (INOUT VulkanDeviceFnTable &, FlatHashMap<VkQueue, String> queueNames);
		static void  Deinitialize (INOUT VulkanDeviceFnTable &);

		static void  Enable ();
		static void  Disable ();

		static void  GetLog (OUT String &);
	};

} // AE::Graphics
