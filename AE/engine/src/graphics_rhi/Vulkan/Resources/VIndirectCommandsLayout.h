// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/IndirectCommandBuffer.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Indirect Commands Layout immutable data
	//

	class VIndirectCommandsLayout final : private VulkanInstanceFn
	{
	// variables
	private:
		VkIndirectCommandsLayoutEXT		_handle			= Default;
		EIndirectCommandsLayoutUsage	_usage			= Default;
		EShaderStages					_stages			= Default;

		GFX_DBG_ONLY( DebugName_t		_debugName;	)


	// methods
	public:
		VIndirectCommandsLayout ()								__NE___	{}
		~VIndirectCommandsLayout ()								__NE___;

		ND_ bool  Create (ResourceManager &, const IndirectCommandsLayoutDesc &, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)															__NE___;

		ND_ VkIndirectCommandsLayoutEXT		Handle ()			C_NE___	{ return _handle; }
		ND_ EIndirectCommandsLayoutUsage	Usage ()			C_NE___	{ return _usage; }
		ND_ EShaderStages					ShaderStages ()		C_NE___	{ return _stages; }

		GFX_DBG_ONLY( ND_ StringView		GetDebugName ()		C_NE___	{ return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
