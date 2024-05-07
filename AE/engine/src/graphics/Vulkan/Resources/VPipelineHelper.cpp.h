// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_GLSL_TRACE
# include "ShaderTrace.h"
#else
# include "Packer/ShaderTraceDummy.h"
#endif

namespace AE::Graphics
{
namespace
{
	//
	// Autorelease Pipeline Cache
	//
	struct AutoreleasePplnCache
	{
	// variables
	private:
		VResourceManager *			_resMngr	= null;
		VPipelineCache const*		_cachePtr	= null;
		Strong<PipelineCacheID>		_cacheId;


	// methods
	public:
		AutoreleasePplnCache (VResourceManager &resMngr, PipelineCacheID cacheId)	__NE___	:
			_resMngr{&resMngr}
		{
			_cachePtr = _resMngr->GetResource( cacheId, True{"incRef"}, True{"quiet"} );
			if ( _cachePtr != null )
				_cacheId.Attach( cacheId );
		}

		~AutoreleasePplnCache ()													__NE___
		{
			_resMngr->ImmediatelyRelease( INOUT _cacheId );
		}

		ND_ explicit operator bool ()												C_NE___	{ return _cachePtr != null; }

		ND_ VPipelineCache const*  operator -> ()									__NE___	{ return _cachePtr; }
	};



	using SpecConstants_t	= PipelineCompiler::ShaderBytecode::OptSpecConst_t;
	using SpecConstValues_t	= BasePipelineDesc::SpecValues_t;

/*
=================================================
	AddSpecialization
=================================================
*/
	template <typename FN>
	ND_ bool  AddSpecialization (OUT VkSpecializationInfo const*	&outSpecInfo,
								 const SpecConstValues_t			&specData,
								 const SpecConstants_t				&specRef,
								 VTempLinearAllocator				&allocator,
								 const VkShaderStageFlagBits		 stage,
								 const FN &							 customSpec) __NE___
	{
		outSpecInfo = null;

		uint	custom_count = 0;
		customSpec( stage, null, null, OUT custom_count );

		const uint	max_count = uint(specRef.size()) + custom_count;

		if ( max_count == 0 )
			return true;

		auto*		entry_arr		= allocator.Allocate<VkSpecializationMapEntry>( max_count );
		auto*		data_arr		= allocator.Allocate<uint>( max_count );
		uint		entriy_count	= 0;
		uint		data_count		= 0;
		BitSet<64>	used;

		CHECK_ERR( entry_arr != null and data_arr != null );

		customSpec( stage, entry_arr, data_arr, OUT entriy_count );
		data_count = entriy_count;

		CHECK_ERR( specData.empty() == specRef.empty() );

		for (auto [name, data] : specData)
		{
			auto	iter = specRef.find( name );

			if_unlikely( iter == specRef.end() )
				continue;

			if_unlikely( used[iter->second] )
				continue;

			used[iter->second] = true;

			VkSpecializationMapEntry&	entry = entry_arr[entriy_count++];
			entry.constantID	= iter->second;
			entry.offset		= data_count * sizeof(*data_arr);
			entry.size			= sizeof(uint);

			data_arr[data_count++] = data;

			ASSERT( data_count < max_count );
		}

		if ( entriy_count and data_count )
		{
			auto*	spec = allocator.Allocate<VkSpecializationInfo>();
			CHECK_ERR( spec != null );

			spec->mapEntryCount	= entriy_count;
			spec->pMapEntries	= entry_arr;
			spec->dataSize		= data_count * sizeof(*data_arr);
			spec->pData			= data_arr;

			outSpecInfo = spec;
		}
		return true;
	}

/*
=================================================
	SetShaderStages
=================================================
*/
	template <typename FN>
	ND_	bool  SetShaderStages (OUT VkPipelineShaderStageCreateInfo const*	&outStages,
							   OUT uint										&outCount,
							   ArrayView<VPipelinePack::ShaderModuleRef>	 shaders,
							   const SpecConstValues_t						&specData,
							   VTempLinearAllocator							&allocator,
							   const FN &									 customSpec) __NE___
	{
		auto*	stages = allocator.Allocate<VkPipelineShaderStageCreateInfo>( shaders.size() );
		CHECK_ERR( stages != null );

		outStages	= stages;
		outCount	= 0;

		for (auto& sh : shaders)
		{
			CHECK_ERR( sh.IsValid() );

			VkPipelineShaderStageCreateInfo&	info = stages[ outCount++ ];
			info.sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			info.pNext		= null;
			info.flags		= 0;
			info.module		= sh.module;
			info.pName		= sh.Entry();
			info.stage		= sh.stage;

			info.pSpecializationInfo = null;
			CHECK_ERR( AddSpecialization( OUT info.pSpecializationInfo, specData, *sh.shaderConstants, allocator, sh.stage, customSpec ));
		}
		return true;
	}

	ND_ bool  SetShaderStages (OUT VkPipelineShaderStageCreateInfo const*	&outStages,
							   OUT uint										&outCount,
							   ArrayView<VPipelinePack::ShaderModuleRef>	 shaders,
							   const SpecConstValues_t						&specData,
							   VTempLinearAllocator							&allocator) __NE___
	{
		const auto	AddCustomSpec = [] (VkShaderStageFlagBits, VkSpecializationMapEntry*, uint*, OUT uint &count)
		{{
			count = 0;
		}};

		return SetShaderStages( OUT outStages, OUT outCount,
								shaders, specData, allocator, AddCustomSpec );
	}

/*
=================================================
	SetDynamicState
=================================================
*/
	ND_ bool  SetDynamicState (OUT VkPipelineDynamicStateCreateInfo	&outState,
							   EPipelineDynamicState				inState,
							   bool									isGraphics,
							   VTempLinearAllocator					&allocator) __NE___
	{
		outState		= {};
		outState.sType	= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		outState.flags	= 0;

		const usize	max_size = BitCount( uint(inState) ) + (isGraphics ? 2 : 0);
		if ( max_size == 0 )
			return true;

		auto*	states = allocator.Allocate<VkDynamicState>( max_size );
		CHECK_ERR( states != null );

		outState.dynamicStateCount	= isGraphics ? 2 : 0;
		outState.pDynamicStates		= states;

		if ( isGraphics )
		{
			*(states++) = VK_DYNAMIC_STATE_VIEWPORT;
			*(states++) = VK_DYNAMIC_STATE_SCISSOR;
		}

		for (auto t : BitfieldIterate( inState ))
		{
			*(states++) = VEnumCast( t );
			++outState.dynamicStateCount;
			ASSERT( outState.dynamicStateCount <= max_size );
		}
		return true;
	}

/*
=================================================
	SetMultisampleState
=================================================
*/
	void  SetMultisampleState (OUT VkPipelineMultisampleStateCreateInfo	&outState,
							   const RenderState::MultisampleState		&inState) __NE___
	{
		outState.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		outState.pNext					= null;
		outState.flags					= 0;
		outState.rasterizationSamples	= VEnumCast( inState.samples );
		outState.sampleShadingEnable	= inState.sampleShading;
		outState.minSampleShading		= inState.minSampleShading;
		outState.pSampleMask			= inState.samples.IsEnabled() ? &inState.sampleMask : null;
		outState.alphaToCoverageEnable	= inState.alphaToCoverage;
		outState.alphaToOneEnable		= inState.alphaToOne;
	}

/*
=================================================
	SetTessellationState
=================================================
*/
	void  SetTessellationState (OUT VkPipelineTessellationStateCreateInfo &outState,
								uint patchSize) __NE___
	{
		outState.sType				= VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		outState.pNext				= null;
		outState.flags				= 0;
		outState.patchControlPoints	= patchSize;
	}

/*
=================================================
	SetStencilOpState
=================================================
*/
	void  SetStencilOpState (OUT VkStencilOpState					&outState,
							 const RenderState::StencilFaceState	&inState) __NE___
	{
		outState.failOp			= VEnumCast( inState.failOp );
		outState.passOp			= VEnumCast( inState.passOp );
		outState.depthFailOp	= VEnumCast( inState.depthFailOp );
		outState.compareOp		= VEnumCast( inState.compareOp );
		outState.compareMask	= inState.compareMask;
		outState.writeMask		= inState.writeMask;
		outState.reference		= inState.reference;
	}

/*
=================================================
	SetDepthStencilState
=================================================
*/
	void  SetDepthStencilState (OUT VkPipelineDepthStencilStateCreateInfo	&outState,
								const RenderState::DepthBufferState			&depth,
								const RenderState::StencilBufferState		&stencil) __NE___
	{
		if ( depth.write )
			ASSERT_MSG( depth.test, "'depth.write' requires 'depth.test'" );

		outState.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		outState.pNext					= null;
		outState.flags					= 0;

		// depth
		outState.depthTestEnable		= depth.test;
		outState.depthWriteEnable		= depth.write;
		outState.depthCompareOp			= VEnumCast( depth.compareOp );
		outState.depthBoundsTestEnable	= depth.bounds;
		outState.minDepthBounds			= depth.minBounds;
		outState.maxDepthBounds			= depth.maxBounds;

		// stencil
		outState.stencilTestEnable		= stencil.enabled;
		SetStencilOpState( OUT outState.front, stencil.front );
		SetStencilOpState( OUT outState.back,  stencil.back  );
	}

/*
=================================================
	SetRasterizationState
=================================================
*/
	void  SetRasterizationState (OUT VkPipelineRasterizationStateCreateInfo	&outState,
								 const RenderState::RasterizationState		&inState) __NE___
	{
		outState.sType						= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		outState.pNext						= null;
		outState.flags						= 0;
		outState.polygonMode				= VEnumCast( inState.polygonMode );
		outState.lineWidth					= 1.f; //inState.lineWidth;
		outState.depthBiasConstantFactor	= inState.depthBiasConstFactor;
		outState.depthBiasClamp				= inState.depthBiasClamp;
		outState.depthBiasSlopeFactor		= inState.depthBiasSlopeFactor;
		outState.depthBiasEnable			= inState.depthBias;
		outState.depthClampEnable			= inState.depthClamp;
		outState.rasterizerDiscardEnable	= inState.rasterizerDiscard;
		outState.frontFace					= inState.frontFaceCCW ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
		outState.cullMode					= VEnumCast( inState.cullMode );
	}

/*
=================================================
	SetupPipelineInputAssemblyState
=================================================
*/
	void  SetupPipelineInputAssemblyState (OUT VkPipelineInputAssemblyStateCreateInfo	&outState,
										   const RenderState::InputAssemblyState		&inState) __NE___
	{
		outState.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		outState.pNext					= null;
		outState.flags					= 0;
		outState.topology				= VEnumCast( inState.topology );
		outState.primitiveRestartEnable	= inState.primitiveRestart;
	}

/*
=================================================
	SetVertexInputState
=================================================
*/
	ND_ bool  SetVertexInputState (OUT VkPipelineVertexInputStateCreateInfo			&outState,
								   ArrayView< GraphicsPipelineDesc::VertexBuffer >	 vertexBuffers,
								   ArrayView< GraphicsPipelineDesc::VertexInput >	 vertexInput,
								   VTempLinearAllocator								&allocator,
								   bool												 supportsVertexDivisor) __NE___
	{
		outState.sType	= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		outState.pNext	= null;
		outState.flags	= 0;

		// if pipeline has attributes then buffer binding must be defined too
		CHECK( vertexInput.empty() == vertexBuffers.empty() );

		if ( vertexInput.empty() or vertexBuffers.empty() )
			return true;

		uint	divisor_count = 0;
		for (auto& src : vertexBuffers) {
			divisor_count += uint(src.divisor > 1);
		}
		if ( not supportsVertexDivisor )
			CHECK_ERR( divisor_count == 0 );

		auto*	input_attrib	= allocator.Allocate<VkVertexInputAttributeDescription>( vertexInput.size() );
		auto*	buffer_binding	= allocator.Allocate<VkVertexInputBindingDescription>( vertexBuffers.size() );
		auto*	divisor_descs	= Cast<VkVertexInputBindingDivisorDescriptionEXT>(null);
		auto*	divisor_info	= Cast<VkPipelineVertexInputDivisorStateCreateInfoEXT>(null);

		CHECK_ERR( input_attrib   != null	and
				   buffer_binding != null	);

		outState.pVertexAttributeDescriptions		= input_attrib;
		outState.vertexAttributeDescriptionCount	= uint(vertexInput.size());
		outState.pVertexBindingDescriptions			= buffer_binding;
		outState.vertexBindingDescriptionCount		= uint(vertexBuffers.size());

		for (auto& src : vertexInput)
		{
			VkVertexInputAttributeDescription&	dst = *(input_attrib++);
			dst.binding		= src.bufferBinding;
			dst.format		= VEnumCast( src.type );
			dst.location	= src.index;
			dst.offset		= uint(src.offset);
		}

		for (auto& src : vertexBuffers)
		{
			VkVertexInputBindingDescription&	dst = *(buffer_binding++);
			dst.binding		= src.index;
			dst.inputRate	= VEnumCast( src.rate );
			dst.stride		= uint(src.stride);
		}

		// VK_EXT_vertex_attribute_divisor
		if ( divisor_count > 0 )
		{
			divisor_info	= allocator.Allocate<VkPipelineVertexInputDivisorStateCreateInfoEXT>( 1 );
			divisor_descs	= allocator.Allocate<VkVertexInputBindingDivisorDescriptionEXT>( divisor_count );
			CHECK_ERR( divisor_info  != null and
					   divisor_descs != null );

			divisor_info->sType						= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT;
			divisor_info->pNext						= null;
			divisor_info->vertexBindingDivisorCount	= divisor_count;
			divisor_info->pVertexBindingDivisors	= divisor_descs;

			for (auto& src : vertexBuffers)
			{
				if ( src.divisor > 1 )
				{
					ASSERT( src.rate == EVertexInputRate::Instance );
					VkVertexInputBindingDivisorDescriptionEXT&	dst = *(divisor_descs++);
					dst.binding	= src.index;
					dst.divisor	= src.divisor;
				}
			}

			outState.pNext = divisor_info;
		}

		return true;
	}

/*
=================================================
	SetViewportState
=================================================
*/
	void  SetViewportState (OUT VkPipelineViewportStateCreateInfo	&outState,
							const uint								viewportCount) __NE___
	{
		outState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		outState.pNext			= null;
		outState.flags			= 0;
		outState.viewportCount	= viewportCount;
		outState.scissorCount	= viewportCount;
	}

/*
=================================================
	SetColorBlendAttachmentState
=================================================
*/
	void  SetColorBlendAttachmentState (OUT VkPipelineColorBlendAttachmentState	&outState,
										const RenderState::ColorBuffer			&inState,
										const bool								logicOpEnabled) __NE___
	{
		outState.blendEnable			= logicOpEnabled ? false : inState.blend;
		outState.srcColorBlendFactor	= VEnumCast( inState.srcBlendFactor.color );
		outState.srcAlphaBlendFactor	= VEnumCast( inState.srcBlendFactor.alpha );
		outState.dstColorBlendFactor	= VEnumCast( inState.dstBlendFactor.color );
		outState.dstAlphaBlendFactor	= VEnumCast( inState.dstBlendFactor.alpha );
		outState.colorBlendOp			= VEnumCast( inState.blendOp.color );
		outState.alphaBlendOp			= VEnumCast( inState.blendOp.alpha );
		outState.colorWriteMask			= (inState.colorMask.r ? VK_COLOR_COMPONENT_R_BIT : 0) |
										  (inState.colorMask.g ? VK_COLOR_COMPONENT_G_BIT : 0) |
										  (inState.colorMask.b ? VK_COLOR_COMPONENT_B_BIT : 0) |
										  (inState.colorMask.a ? VK_COLOR_COMPONENT_A_BIT : 0);
	}

/*
=================================================
	SetColorBlendState
=================================================
*/
	ND_ bool  SetColorBlendState (OUT VkPipelineColorBlendStateCreateInfo	&outState,
								  const RenderState::ColorBuffersState		&inState,
								  const VRenderPass::SubpassInfo			&subpass,
								  VTempLinearAllocator						&allocator) __NE___
	{
		if ( subpass.colorAttachments.empty() )
			return true;

		const bool	logic_op_enabled	= ( inState.logicOp != ELogicOp::None );
		auto*		attachments			= allocator.Allocate<VkPipelineColorBlendAttachmentState>( subpass.colorAttachments.size() );
		CHECK_ERR( attachments != null );

		for (usize i = 0, cnt = Min(inState.buffers.size(), subpass.colorAttachments.size()); i < cnt; ++i)
		{
			SetColorBlendAttachmentState( OUT attachments[i], inState.buffers[i], logic_op_enabled );
		}

		outState.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		outState.pNext				= null;
		outState.flags				= 0;
		outState.attachmentCount	= uint(subpass.colorAttachments.size());
		outState.pAttachments		= outState.attachmentCount ? attachments : null;
		outState.logicOpEnable		= logic_op_enabled;
		outState.logicOp			= logic_op_enabled ? VEnumCast( inState.logicOp ) : VK_LOGIC_OP_CLEAR;

		outState.blendConstants[0] = inState.blendColor.r;
		outState.blendConstants[1] = inState.blendColor.g;
		outState.blendConstants[2] = inState.blendColor.b;
		outState.blendConstants[3] = inState.blendColor.a;

		return true;
	}

/*
=================================================
	CopyShaderTrace
=================================================
*/
	bool  CopyShaderTrace (ArrayView< VPipelinePack::ShaderModuleRef >	shaders,
						   IAllocator *									allocator,
						   OUT ArrayView<ShaderTracePtr>				&dbgTrace) __NE___
	{
		dbgTrace = Default;
		CHECK_ERR( allocator != null );

		uint	trace_count = 0;

		for (auto& sh : shaders) {
			trace_count += uint(sh.dbgTrace != null);
		}

		if ( trace_count == 0 )
			return true;

		auto*	ptr = allocator->Allocate<ShaderTracePtr>( trace_count );
		CHECK_ERR( ptr != null );
		dbgTrace = ArrayView<ShaderTracePtr>{ ptr, trace_count };

		for (auto& sh : shaders)
		{
			if ( sh.dbgTrace )
				*(ptr++) = sh.dbgTrace;
		}
		return true;
	}

/*
=================================================
	ConvertLogFormat
=================================================
*/
	ND_ auto  ConvertLogFormat (ShaderDebugger::ELogFormat fmt) __NE___
	{
		using ELogFormat = PipelineCompiler::ShaderTrace::ELogFormat;

		StaticAssert( uint(ELogFormat::_Count)		== 5	);
		StaticAssert( uint(ELogFormat::_Count)		== uint(ShaderDebugger::ELogFormat::_Count)	);
		StaticAssert( uint(ELogFormat::Unknown)		== uint(ShaderDebugger::ELogFormat::Unknown) );
		StaticAssert( uint(ELogFormat::Text)		== uint(ShaderDebugger::ELogFormat::Text) );
		StaticAssert( uint(ELogFormat::VS_Console)	== uint(ShaderDebugger::ELogFormat::VS_Console)	);
		StaticAssert( uint(ELogFormat::VS)			== uint(ShaderDebugger::ELogFormat::VS)	);
		StaticAssert( uint(ELogFormat::VSCode)		== uint(ShaderDebugger::ELogFormat::VSCode)	);

		return ELogFormat(fmt);
	}


// avoid crash in driver when used very large/complex shader
#if defined(AE_DEBUG) and defined(AE_COMPILER_MSVC)

/*
=================================================
	CreateComputePipelines
=================================================
*/
	ND_ AE_NOINLINE VkResult  CreateComputePipelines (const VDevice &device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo * pCreateInfos, const VkAllocationCallbacks * pAllocator, OUT VkPipeline * pPipelines)
	{
		__try {
			return device.vkCreateComputePipelines( device.GetVkDevice(), pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
		}
		__except ( (GetExceptionCode() == AE_SEH_STACK_OVERFLOW) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH )
		{
			_resetstkoflw();
			return VK_ERROR_UNKNOWN;
		}
	}

/*
=================================================
	CreateGraphicsPipelines
=================================================
*/
	ND_ AE_NOINLINE VkResult CreateGraphicsPipelines (const VDevice &device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo * pCreateInfos, const VkAllocationCallbacks * pAllocator, OUT VkPipeline * pPipelines)
	{
		__try {
			return device.vkCreateGraphicsPipelines( device.GetVkDevice(), pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
		}
		__except ( (GetExceptionCode() == AE_SEH_STACK_OVERFLOW) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH )
		{
			_resetstkoflw();
			return VK_ERROR_UNKNOWN;
		}
	}

/*
=================================================
	CreateRayTracingPipelines
=================================================
*/
	ND_ AE_NOINLINE VkResult CreateRayTracingPipelines (const VDevice &device, VkDeferredOperationKHR deferredOperation, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkRayTracingPipelineCreateInfoKHR * pCreateInfos, const VkAllocationCallbacks * pAllocator, OUT VkPipeline * pPipelines)
	{
		__try {
			return device.vkCreateRayTracingPipelinesKHR( device.GetVkDevice(), deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
		}
		__except ( (GetExceptionCode() == AE_SEH_STACK_OVERFLOW) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH )
		{
			_resetstkoflw();
			return VK_ERROR_UNKNOWN;
		}
	}

#else

	ND_ inline VkResult  CreateComputePipelines (const VDevice &device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo * pCreateInfos, const VkAllocationCallbacks * pAllocator, OUT VkPipeline * pPipelines) __NE___
	{
		return device.vkCreateComputePipelines( device.GetVkDevice(), pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
	}

	ND_ inline VkResult CreateGraphicsPipelines (const VDevice &device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo * pCreateInfos, const VkAllocationCallbacks * pAllocator, OUT VkPipeline * pPipelines) __NE___
	{
		return device.vkCreateGraphicsPipelines( device.GetVkDevice(), pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
	}

	ND_ inline VkResult CreateRayTracingPipelines (const VDevice &device, VkDeferredOperationKHR deferredOperation, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkRayTracingPipelineCreateInfoKHR * pCreateInfos, const VkAllocationCallbacks * pAllocator, OUT VkPipeline * pPipelines)
	{
		return device.vkCreateRayTracingPipelinesKHR( device.GetVkDevice(), deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
	}

#endif

} // namespace
} // AE::Graphics
