// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VGraphicsPipeline.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Vulkan/Utils/NextChain.h"
# include "VPipelineHelper.cpp.h"

namespace AE::Graphics
{
/*
=================================================
	destructor
=================================================
*/
	VGraphicsPipeline::~VGraphicsPipeline () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( _handle == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VGraphicsPipeline::Create (ResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( not ci.shaders.empty() );
		CHECK_ERR( (ci.specCI.dynamicState & ~EPipelineDynamicState::GraphicsPipelineMask) == Zero );
		CHECK_ERR( not _handle and not _layout );
		CHECK_ERR( ci.specCI.renderStatePtr );

		const auto&	render_state = *ci.specCI.renderStatePtr;
		GRES_CHECK( ci.templCI.supportedTopology.contains( render_state.inputAssembly.topology ));
		GRES_CHECK( resMngr.GetFeatureSet().IsSupported( render_state ));

		auto*	ppln_layout = resMngr.GetResource( ci.layoutId, True{"incRef"} );
		CHECK_ERR( ppln_layout != null );

		_layout = ppln_layout->Handle();
		_layoutId.Attach( ci.layoutId );

		auto&					dev			= resMngr.GetDevice();
		const auto&				ext			= dev.GetVExtensions();
		AutoreleasePplnCache	cache_ptr	{ resMngr, ci.cacheId };
		VkPipelineCache			ppln_cache	= cache_ptr ? cache_ptr->Handle() : Default;
		RenderPassID			rp_id		= ci.pplnPack.GetRenderPass( CompatRenderPassName{ ci.specCI.renderPass });
		auto*					render_pass	= resMngr.GetResource( rp_id );
		CHECK_ERR( render_pass != null );

		VRenderPass::SubpassInfo const*		subpass		= null;
		uint								subpass_idx;
		{
			auto	iter = render_pass->SubpassMap().find( ci.specCI.subpass );
			CHECK_ERR( iter != render_pass->SubpassMap().end() );
			CHECK_ERR( usize{iter->second} < render_pass->Subpasses().size() );
			subpass_idx	= uint{iter->second};
			subpass		= &render_pass->Subpasses()[ subpass_idx ];
		}

		VkGraphicsPipelineCreateInfo				pipeline_info		= {};
		VkPipelineInputAssemblyStateCreateInfo		input_assembly_info	= {};
		VkPipelineColorBlendStateCreateInfo			blend_info			= {};
		VkPipelineDepthStencilStateCreateInfo		depth_stencil_info	= {};
		VkPipelineMultisampleStateCreateInfo		multisample_info	= {};
		VkPipelineRasterizationStateCreateInfo		rasterization_info	= {};
		VkPipelineTessellationStateCreateInfo		tessellation_info	= {};
		VkPipelineDynamicStateCreateInfo			dynamic_state_info	= {};
		VkPipelineVertexInputStateCreateInfo		vertex_input_info	= {};
		VkPipelineViewportStateCreateInfo			viewport_info		= {};
		VkPipelineRobustnessCreateInfoEXT			robustness_ci;
		VkPipelineCreateFlags2CreateInfoKHR			flags2_ci			= {};
		VNextChain									p_next				{pipeline_info};
		VTempLinearAllocator						allocator;

		const bool	vertex_divisor_supported = (resMngr.GetFeatureSet().vertexDivisor == FeatureSet::EFeature::RequireTrue);

		CHECK_ERR( SetShaderStages( OUT pipeline_info.pStages, OUT pipeline_info.stageCount, ci.shaders, ci.specCI.specialization, allocator ));
		CHECK_ERR( SetDynamicState( OUT dynamic_state_info, ci.specCI.dynamicState, true, allocator ));
		SetMultisamplingState( OUT multisample_info, render_state.multisample );
		SetTessellationState( OUT tessellation_info, ci.templCI.patchControlPoints );
		SetDepthStencilState( OUT depth_stencil_info, render_state.depth, render_state.stencil, render_state.rasterOrderAccess );
		CHECK_ERR( SetRasterizationState( OUT rasterization_info, render_state.rasterization, allocator ));
		SetupPipelineInputAssemblyState( OUT input_assembly_info, render_state.inputAssembly );
		CHECK_ERR( SetVertexInputState( OUT vertex_input_info, ci.specCI.vertexBuffers, ci.specCI.vertexInput, allocator, vertex_divisor_supported ));
		SetViewportState( OUT viewport_info, ci.specCI.viewportCount );
		CHECK_ERR( SetColorBlendState( OUT blend_info, render_state.color, *subpass, render_state.rasterOrderAccess, allocator ));

		flags2_ci.sType						= VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO;
		flags2_ci.flags						= VEnumCast( ci.specCI.options );

		pipeline_info.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.flags					= VkPipelineCreateFlags( flags2_ci.flags );
		pipeline_info.basePipelineIndex		= -1;
		pipeline_info.basePipelineHandle	= Default;
		pipeline_info.pColorBlendState		= &blend_info;
		pipeline_info.pDepthStencilState	= &depth_stencil_info;
		pipeline_info.pDynamicState			= &dynamic_state_info;
		pipeline_info.pInputAssemblyState	= &input_assembly_info;
		pipeline_info.pMultisampleState		= &multisample_info;
		pipeline_info.pRasterizationState	= &rasterization_info;
		pipeline_info.pTessellationState	= (ci.templCI.patchControlPoints > 0 ? &tessellation_info : null);
		pipeline_info.pVertexInputState		= &vertex_input_info;
		pipeline_info.layout				= _layout;
		pipeline_info.renderPass			= render_pass->Handle();
		pipeline_info.subpass				= subpass_idx;

		if ( ext.pipelineRobustness )
		{
			p_next.Add( robustness_ci );
			SetRobustness( OUT robustness_ci );
		}

		if ( ext.rayQueryMicromapARM and NoBits( ci.specCI.options, EPipelineOpt::OpacityMicromap ))
		{
			flags2_ci.flags |= VK_PIPELINE_CREATE_2_DISALLOW_OPACITY_MICROMAP_BIT_ARM;
		}

		if ( ext.maintenance5 ){
			p_next.Add( flags2_ci );
		}else{
			CHECK_ERR_MSG( flags2_ci.flags == pipeline_info.flags,
				"Some pipeline creation flags requires 'maintenance5' extension" );
		}

		if ( not rasterization_info.rasterizerDiscardEnable )
		{
			pipeline_info.pViewportState		= &viewport_info;
		}else{
			pipeline_info.pViewportState		= null;
			pipeline_info.pMultisampleState		= null;
			pipeline_info.pDepthStencilState	= null;
			pipeline_info.pColorBlendState		= null;
		}

		VK_CHECK_ERR( CreateGraphicsPipelines( dev, ppln_cache, 1, &pipeline_info, null, OUT &_handle ));

		dev.SetObjectName( _handle, ci.specCI.dbgName, VK_OBJECT_TYPE_PIPELINE );

		_dynamicState	= ci.specCI.dynamicState;
		_options		= ci.specCI.options;
		_topology		= render_state.inputAssembly.topology;
		_subpassIndex	= CheckCast{ subpass_idx };

		for (auto& vb : ci.specCI.vertexBuffers) {
			_vertexBuffers.emplace( vb.name, vb.index );
		}

		CopyShaderTrace( ci.shaders, ci.allocator, OUT _dbgTrace );

		GFX_DBG_ONLY( _debugName = ci.specCI.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VGraphicsPipeline::Destroy (ResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	dev = resMngr.GetDevice();

		if ( _handle != Default )
			dev.vkDestroyPipeline( dev.GetVkDevice(), _handle, null );

		resMngr.ImmediatelyRelease( INOUT _layoutId );

		_vertexBuffers.clear();

		_handle			= Default;
		_layout			= Default;
		_layoutId		= Default;
		_dynamicState	= Default;
		_options		= Default;
		_topology		= Default;
		_subpassIndex	= UMax;
		_dbgTrace		= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetVertexBufferIndex
=================================================
*/
	uint  VGraphicsPipeline::GetVertexBufferIndex (VertexBufferName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _vertexBuffers.find( name );
		return it != _vertexBuffers.end() ? it->second : UMax;
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  VGraphicsPipeline::ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
	{
		result.clear();
		DRC_SHAREDLOCK( _drCheck );

		for (auto& trace : _dbgTrace)
		{
			Array<String>	temp;
			CHECK_ERR( trace->ParseShaderTrace( ptr, maxSize, ConvertLogFormat(format), OUT temp ));
			result.insert( result.end(), temp.begin(), temp.end() );
		}
		return true;
	}

/*
=================================================
	ParseShaderAsserts
=================================================
*/
	bool  VGraphicsPipeline::ParseShaderAsserts (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
	{
		result.clear();
		DRC_SHAREDLOCK( _drCheck );

		for (auto& trace : _dbgTrace)
		{
			Array<String>	temp;
			CHECK_ERR( trace->ParseAsserts( ptr, maxSize, ConvertLogFormat(format), OUT temp ));
			result.insert( result.end(), temp.begin(), temp.end() );
		}
		return true;
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
