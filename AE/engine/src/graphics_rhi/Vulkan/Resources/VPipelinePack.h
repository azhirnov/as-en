// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/ShaderDebugger.h"

# include "res_pack/pipeline_compiler/PipelineCompilerImpl.h"

# include "graphics_rhi/Vulkan/VCommon.h"
# include "graphics_rhi/Vulkan/Resources/VSampler.h"
# include "graphics_rhi/Vulkan/Resources/VRenderPass.h"

namespace AE::Graphics
{
	class VComputePipeline;
	class VGraphicsPipeline;
	class VMeshPipeline;
	class VTilePipeline;
	class VRayTracingPipeline;
	class VDescriptorSetLayout;
	class VPipelineLayout;
	class VRTShaderBindingTable;

	using ShaderTracePtr = Ptr< const PipelineCompiler::ShaderTrace >;

} // AE::Graphics

// implementation
# include "graphics_rhi/Private/PipelinePack.h"

#endif // AE_ENABLE_VULKAN
