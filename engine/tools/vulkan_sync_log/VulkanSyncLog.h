// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Vulkan/VulkanLoader.h"

namespace AE::Graphics
{

	//
	// Vulkan Synchronization Logger
	//

	class VulkanSyncLog
	{
	public:
		void  Initialize (INOUT VulkanDeviceFnTable &, FlatHashMap<VkQueue, String> queueNames);
		void  Deinitialize (INOUT VulkanDeviceFnTable &);

		void  Enable ();
		void  Disable ();

		void  GetLog (OUT String &) const;
	};

} // AE::Graphics
