// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Private/ResourceBase.h"
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/VSwapchain.h"

# include "graphics/Vulkan/Resources/VBuffer.h"
# include "graphics/Vulkan/Resources/VBufferView.h"
# include "graphics/Vulkan/Resources/VDescriptorSet.h"
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"
# include "graphics/Vulkan/Resources/VFramebuffer.h"
# include "graphics/Vulkan/Resources/VImage.h"
# include "graphics/Vulkan/Resources/VImageView.h"
# include "graphics/Vulkan/Resources/VPipelineLayout.h"
# include "graphics/Vulkan/Resources/VComputePipeline.h"
# include "graphics/Vulkan/Resources/VGraphicsPipeline.h"
# include "graphics/Vulkan/Resources/VRayTracingPipeline.h"
# include "graphics/Vulkan/Resources/VMeshPipeline.h"
# include "graphics/Vulkan/Resources/VTilePipeline.h"
# include "graphics/Vulkan/Resources/VPipelinePack.h"
# include "graphics/Vulkan/Resources/VPipelineCache.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"
# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/Resources/VRTScene.h"
# include "graphics/Vulkan/Resources/VMemoryObject.h"
# include "graphics/Vulkan/Resources/VRTShaderBindingTable.h"

# include "graphics/Vulkan/Resources/VStagingBufferManager.h"
# include "graphics/Vulkan/Resources/VQueryManager.h"

# include "threading/Containers/LfIndexedPool3.h"
# include "threading/Containers/LfStaticIndexedPool.h"

// implementation
# include "graphics/Private/ResourceManager.h"

#endif // AE_ENABLE_VULKAN
