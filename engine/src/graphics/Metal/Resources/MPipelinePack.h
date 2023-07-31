// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/ResourceManager.h"

# include "PipelineCompilerImpl.h"

# include "graphics/Metal/MCommon.h"
# include "graphics/Metal/Resources/MSampler.h"
//# include "graphics/Metal/Resources/MRenderPass.h"

namespace AE::Graphics
{
    class MComputePipeline;
    class MGraphicsPipeline;
    class MMeshPipeline;
    class MTilePipeline;
    class MRayTracingPipeline;
    class MPipelineLayout;
    class MRTShaderBindingTable;

} // AE::Graphics

// implementation
# include "graphics/Private/PipelinePack.h"

#endif // AE_ENABLE_METAL
