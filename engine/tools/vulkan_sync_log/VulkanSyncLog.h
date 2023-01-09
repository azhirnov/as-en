// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Vulkan/VulkanLoader.h"

namespace AE::Graphics
{

	//
	// Vulkan Synchronization Logger
	//

	class VulkanSyncLog //: Noninstancable
	{
	public:
		static void  Initialize (INOUT VulkanDeviceFnTable &, FlatHashMap<VkQueue, String> queueNames);
		static void  Deinitialize (INOUT VulkanDeviceFnTable &);

		static void  Enable ();
		static void  Disable ();

		static void  GetLog (OUT String &);
	};

} // AE::Graphics
