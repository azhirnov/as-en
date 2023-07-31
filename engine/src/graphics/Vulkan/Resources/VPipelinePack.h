// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/ShaderDebugger.h"

# include "PipelineCompilerImpl.h"

# include "graphics/Vulkan/VCommon.h"
# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"

namespace AE::Graphics
{
    class VComputePipeline;
    class VGraphicsPipeline;
    class VMeshPipeline;
    class VTilePipeline;
    class VRayTracingPipeline;
    class VPipelineLayout;
    class VRTShaderBindingTable;

    using ShaderTracePtr = Ptr< const PipelineCompiler::ShaderTrace >;


} // AE::Graphics

// implementation
# include "graphics/Private/PipelinePack.h"

#endif // AE_ENABLE_VULKAN
