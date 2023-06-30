// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "threading/Containers/LfIndexedPool3.h"
# include "threading/Containers/LfStaticIndexedPool.h"

# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Private/ResourceBase.h"
# include "graphics/Metal/MDevice.h"
# include "graphics/Metal/MSwapchain.h"

# include "graphics/Metal/Resources/MBuffer.h"
# include "graphics/Metal/Resources/MBufferView.h"
# include "graphics/Metal/Resources/MImage.h"
# include "graphics/Metal/Resources/MImageView.h"
# include "graphics/Metal/Resources/MSampler.h"
# include "graphics/Metal/Resources/MMemoryObject.h"
# include "graphics/Metal/Resources/MComputePipeline.h"
# include "graphics/Metal/Resources/MGraphicsPipeline.h"
# include "graphics/Metal/Resources/MTilePipeline.h"
# include "graphics/Metal/Resources/MMeshPipeline.h"
# include "graphics/Metal/Resources/MRayTracingPipeline.h"
# include "graphics/Metal/Resources/MPipelinePack.h"
# include "graphics/Metal/Resources/MDescriptorSet.h"
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"
# include "graphics/Metal/Resources/MPipelineLayout.h"
# include "graphics/Metal/Resources/MRenderPass.h"
# include "graphics/Metal/Resources/MRTGeometry.h"
# include "graphics/Metal/Resources/MRTScene.h"
# include "graphics/Metal/Resources/MPipelineCache.h"
# include "graphics/Metal/Resources/MRTShaderBindingTable.h"

# include "graphics/Metal/Resources/MStagingBufferManager.h"
# include "graphics/Metal/Resources/MQueryManager.h"

# include "graphics/Metal/Video/MVideoBuffer.h"
# include "graphics/Metal/Video/MVideoImage.h"
# include "graphics/Metal/Video/MVideoSession.h"

# include "graphics/Metal/Descriptors/MDefaultDescriptorAllocator.h"

// implementation   
# include "graphics/Private/ResourceManager.h"

#endif // AE_ENABLE_METAL
