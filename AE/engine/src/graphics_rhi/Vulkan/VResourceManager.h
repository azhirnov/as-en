// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Private/ResourceBase.h"
# include "graphics_rhi/Vulkan/VDevice.h"
# include "graphics_rhi/Vulkan/VSwapchain.h"

# include "graphics_rhi/Vulkan/Resources/VBuffer.h"
# include "graphics_rhi/Vulkan/Resources/VBufferView.h"
# include "graphics_rhi/Vulkan/Resources/VDescriptorSet.h"
# include "graphics_rhi/Vulkan/Resources/VDescriptorSetLayout.h"
# include "graphics_rhi/Vulkan/Resources/VFramebuffer.h"
# include "graphics_rhi/Vulkan/Resources/VImage.h"
# include "graphics_rhi/Vulkan/Resources/VImageView.h"
# include "graphics_rhi/Vulkan/Resources/VPipelineLayout.h"
# include "graphics_rhi/Vulkan/Resources/VComputePipeline.h"
# include "graphics_rhi/Vulkan/Resources/VGraphicsPipeline.h"
# include "graphics_rhi/Vulkan/Resources/VRayTracingPipeline.h"
# include "graphics_rhi/Vulkan/Resources/VMeshPipeline.h"
# include "graphics_rhi/Vulkan/Resources/VTilePipeline.h"
# include "graphics_rhi/Vulkan/Resources/VPipelinePack.h"
# include "graphics_rhi/Vulkan/Resources/VPipelineCache.h"
# include "graphics_rhi/Vulkan/Resources/VRenderPass.h"
# include "graphics_rhi/Vulkan/Resources/VSampler.h"
# include "graphics_rhi/Vulkan/Resources/VRTGeometry.h"
# include "graphics_rhi/Vulkan/Resources/VRTScene.h"
# include "graphics_rhi/Vulkan/Resources/VRTCluster.h"
# include "graphics_rhi/Vulkan/Resources/VRTPartitionedScene.h"
# include "graphics_rhi/Vulkan/Resources/VMemoryObject.h"
# include "graphics_rhi/Vulkan/Resources/VRTShaderBindingTable.h"

# include "graphics_rhi/Vulkan/Resources/VStagingBufferManager.h"
# include "graphics_rhi/Vulkan/Resources/VQueryManager.h"

# include "graphics_rhi/Vulkan/Video/VVideoBuffer.h"
# include "graphics_rhi/Vulkan/Video/VVideoImage.h"
# include "graphics_rhi/Vulkan/Video/VVideoSession.h"

// implementation
# include "graphics_rhi/Private/ResourceManager.h"

#endif // AE_ENABLE_VULKAN
