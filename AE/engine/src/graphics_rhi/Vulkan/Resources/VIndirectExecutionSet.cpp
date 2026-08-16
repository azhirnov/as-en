// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VIndirectExecutionSet.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VIndirectExecutionSet::~VIndirectExecutionSet () __NE___
	{
		ASSERT( _handle == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VIndirectExecutionSet::Create (ResourceManager &resMngr, const CreateInfo &desc) __NE___
	{
		CHECK_ERR( _handle == Default );
		CHECK_ERR( not desc.pipelines.empty() );

		auto&	dev = resMngr.GetDevice();

		// must be validates at resource compilation stage
		GRES_CHECK( desc.pipelines.size() <= dev.GetVProperties().deviceGeneratedCommandsProps.maxIndirectPipelineCount );

		VkIndirectExecutionSetPipelineInfoEXT	ppln_info = {};
		ppln_info.sType				= VK_STRUCTURE_TYPE_INDIRECT_EXECUTION_SET_PIPELINE_INFO_EXT;
		ppln_info.maxPipelineCount	= uint(desc.pipelines.size());
		ppln_info.initialPipeline	= desc.pipelines[0];

		VkIndirectExecutionSetCreateInfoEXT		info = {};
		info.sType				= VK_STRUCTURE_TYPE_INDIRECT_EXECUTION_SET_CREATE_INFO_EXT;
		info.type				= VK_INDIRECT_EXECUTION_SET_INFO_TYPE_PIPELINES_EXT;
		info.info.pPipelineInfo	= &ppln_info;

		VK_CHECK_ERR( dev.vkCreateIndirectExecutionSetEXT( dev.GetVkDevice(), &info, null, OUT &_handle ));

		dev.SetObjectName( _handle, desc.dbgName, VK_OBJECT_TYPE_INDIRECT_EXECUTION_SET_EXT );

		if ( desc.pipelines.size() > 1 )
		{
			Array<VkWriteIndirectExecutionSetPipelineEXT>	write_pipes;
			write_pipes.resize( desc.pipelines.size()-1 );

			for (usize i = 1; i < desc.pipelines.size(); ++i)
			{
				auto&	dst = write_pipes[i-1];
				dst.sType	= VK_STRUCTURE_TYPE_WRITE_INDIRECT_EXECUTION_SET_PIPELINE_EXT;
				dst.pNext	= null;
				dst.index	= uint(i);
				dst.pipeline = desc.pipelines[i];
			}

			dev.vkUpdateIndirectExecutionSetPipelineEXT( dev.GetVkDevice(), _handle, uint(write_pipes.size()), write_pipes.data() );
		}

		_initialState  = desc.initialState;
		_pipelineCount = uint(desc.pipelines.size());

		GFX_DBG_ONLY( _debugName = desc.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VIndirectExecutionSet::Destroy (ResourceManager &resMngr) __NE___
	{
		auto&	dev = resMngr.GetDevice();

		dev.vkDestroyIndirectExecutionSetEXT( dev.GetVkDevice(), _handle, null );

		_handle			= Default;
		_initialState	= Default;
		_pipelineCount	= 0;

		GFX_DBG_ONLY( _debugName.clear() );
	}

} // AE::Graphics
#endif // AE_ENABLE_VULKAN
