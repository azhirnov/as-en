// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "base/Containers/InPlace.h"
# include "graphics/Vulkan/VCommon.h"
# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"

namespace AE::Graphics
{

	//
	// Vulkan Pipeline Pack
	//

	class VPipelinePack
	{
	// types
	public:
		struct ShaderModuleRef
		{
			VkShaderStageFlagBits									stage			= Zero;
			VkShaderModule											module			= Default;
			StringView												entry			= "main";
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			
			ND_ bool  IsValid () const	{ return module != Default; }
		};

		using Allocator_t = Threading::LfLinearAllocator< usize(SmallAllocationSize * 16) >;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
