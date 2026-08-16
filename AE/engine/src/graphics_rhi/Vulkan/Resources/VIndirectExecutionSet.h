// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/IndirectCommandBuffer.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Indirect Execution Set immutable data
	//

	class VIndirectExecutionSet final : private VulkanInstanceFn
	{
	// types
	public:
		struct InitialState
		{
			VkPipeline					pipeline		= Default;
			VkPipelineLayout			layout			= Default;
			PipelineLayoutID			layoutId;
			VkPipelineBindPoint			bindPoint		= VK_PIPELINE_BIND_POINT_MAX_ENUM;
			EPipelineDynamicState		dynamicState	= Default;
			EShaderStages				activeStages	= Default;
		};

		struct CreateInfo
		{
			ArrayView<VkPipeline>	pipelines;
			InitialState			initialState;
			StringView				dbgName;
		};


	// variables
	private:
		VkIndirectExecutionSetEXT		_handle			= Default;
		InitialState					_initialState;
		uint							_pipelineCount	= 0;

		GFX_DBG_ONLY( DebugName_t		_debugName;	)


	// methods
	public:
		VIndirectExecutionSet ()									__NE___	{}
		~VIndirectExecutionSet ()									__NE___;

		ND_ bool  Create (ResourceManager &, const CreateInfo &)	__NE___;
			void  Destroy (ResourceManager &)						__NE___;

		ND_ VkIndirectExecutionSetEXT		Handle ()				C_NE___	{ return _handle; }
		ND_ InitialState					GetInitialState ()		C_NE___	{ return _initialState; }
		ND_ uint							PipelineCount ()		C_NE___	{ return _pipelineCount; }

		GFX_DBG_ONLY( ND_ StringView		GetDebugName ()			C_NE___	{ return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
