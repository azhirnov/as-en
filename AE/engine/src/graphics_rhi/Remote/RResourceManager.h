// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS

# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Private/ResourceBase.h"
# include "graphics_rhi/Remote/RDevice.h"
# include "graphics_rhi/Remote/RSwapchain.h"

# include "graphics_rhi/Remote/Resources/RBuffer.h"
# include "graphics_rhi/Remote/Resources/RBufferView.h"
# include "graphics_rhi/Remote/Resources/RDescriptorSet.h"
# include "graphics_rhi/Remote/Resources/RDescriptorSetLayout.h"
# include "graphics_rhi/Remote/Resources/RImage.h"
# include "graphics_rhi/Remote/Resources/RImageView.h"
# include "graphics_rhi/Remote/Resources/RPipelineLayout.h"
# include "graphics_rhi/Remote/Resources/RComputePipeline.h"
# include "graphics_rhi/Remote/Resources/RGraphicsPipeline.h"
# include "graphics_rhi/Remote/Resources/RRayTracingPipeline.h"
# include "graphics_rhi/Remote/Resources/RMeshPipeline.h"
# include "graphics_rhi/Remote/Resources/RTilePipeline.h"
# include "graphics_rhi/Remote/Resources/RRenderPass.h"
# include "graphics_rhi/Remote/Resources/RPipelinePack.h"
# include "graphics_rhi/Remote/Resources/RPipelineCache.h"
# include "graphics_rhi/Remote/Resources/RSampler.h"
# include "graphics_rhi/Remote/Resources/RRTGeometry.h"
# include "graphics_rhi/Remote/Resources/RRTScene.h"
# include "graphics_rhi/Remote/Resources/RRTMicromap.h"
# include "graphics_rhi/Remote/Resources/RMemoryObject.h"
# include "graphics_rhi/Remote/Resources/RRTShaderBindingTable.h"
# include "graphics_rhi/Remote/Resources/RIndirectExecutionSet.h"
# include "graphics_rhi/Remote/Resources/RIndirectCommandsLayout.h"

# include "graphics_rhi/Remote/Resources/RStagingBufferManager.h"
# include "graphics_rhi/Remote/Resources/RQueryManager.h"

# include "graphics_rhi/Remote/Video/RVideoBuffer.h"
# include "graphics_rhi/Remote/Video/RVideoImage.h"
# include "graphics_rhi/Remote/Video/RVideoSession.h"

// implementation
# include "graphics_rhi/Private/ResourceManager.h"

#endif // AE_ENABLE_REMOTE_GRAPHICS
