// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS

# include "graphics/Public/ResourceManager.h"
# include "graphics/Private/ResourceBase.h"
# include "graphics/Remote/RDevice.h"
# include "graphics/Remote/RSwapchain.h"

# include "graphics/Remote/Resources/RBuffer.h"
# include "graphics/Remote/Resources/RBufferView.h"
# include "graphics/Remote/Resources/RDescriptorSet.h"
# include "graphics/Remote/Resources/RDescriptorSetLayout.h"
# include "graphics/Remote/Resources/RImage.h"
# include "graphics/Remote/Resources/RImageView.h"
# include "graphics/Remote/Resources/RPipelineLayout.h"
# include "graphics/Remote/Resources/RComputePipeline.h"
# include "graphics/Remote/Resources/RGraphicsPipeline.h"
# include "graphics/Remote/Resources/RRayTracingPipeline.h"
# include "graphics/Remote/Resources/RMeshPipeline.h"
# include "graphics/Remote/Resources/RTilePipeline.h"
# include "graphics/Remote/Resources/RRenderPass.h"
# include "graphics/Remote/Resources/RPipelinePack.h"
# include "graphics/Remote/Resources/RPipelineCache.h"
# include "graphics/Remote/Resources/RSampler.h"
# include "graphics/Remote/Resources/RRTGeometry.h"
# include "graphics/Remote/Resources/RRTScene.h"
# include "graphics/Remote/Resources/RMemoryObject.h"
# include "graphics/Remote/Resources/RRTShaderBindingTable.h"

# include "graphics/Remote/Resources/RStagingBufferManager.h"
# include "graphics/Remote/Resources/RQueryManager.h"

# include "graphics/Remote/Video/RVideoBuffer.h"
# include "graphics/Remote/Video/RVideoImage.h"
# include "graphics/Remote/Video/RVideoSession.h"

// implementation
# include "graphics/Private/ResourceManager.h"

#endif // AE_ENABLE_REMOTE_GRAPHICS
