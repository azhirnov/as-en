// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/BufferDesc.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Ray Tracing Partitioned Scene
	//

	struct VRTPartitionedScene : Noninstanceable
	{
		ND_ static RTASBuildSizes	GetBuildSizes (const ResourceManager &, const RTPartitionedSceneInfo &)		__NE___;

		ND_ static bool				ConvertBuildInfo (const ResourceManager &, const RTPartitionedSceneBuild &,
													  OUT VkBuildPartitionedAccelerationStructureInfoNV &)			__NE___;

		ND_ static bool				IsSupported (const ResourceManager &, const RTPartitionedSceneInfo &)			__NE___;

	private:
		ND_ static bool  _Convert (const ResourceManager &, const RTPartitionedSceneInfo &desc,
								   OUT VkPartitionedAccelerationStructureInstancesInputNV &inputInfo)				__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
