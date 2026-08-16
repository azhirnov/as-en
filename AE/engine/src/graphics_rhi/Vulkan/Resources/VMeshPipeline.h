// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Wrapper for mesh pipeline (VK_EXT_mesh_shader).
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VGraphicsPipeline.h"

namespace AE::Graphics
{

	//
	// Vulkan Mesh Pipeline
	//

	class VMeshPipeline
	{
	// types
	public:
		struct CreateInfo
		{
			VPipelinePack const&								pplnPack;
			PipelineCompiler::SerializableMeshPipeline const&	templCI;
			MeshPipelineDesc const&								specCI;
			PipelineLayoutID									layoutId;
			ArrayView< VPipelinePack::ShaderModuleRef >			shaders;
			PipelineCacheID										cacheId;
			IAllocator *										allocator		= null;
		};


	// variables
	private:
		VkPipeline					_handle					= Default;
		VkPipelineLayout			_layout					= Default;

		WGLocalSize_t				_meshLocalSize;
		WGLocalSize_t				_taskLocalSize;
		EPipelineDynamicState		_dynamicState			= Default;
		EPipelineOpt				_options				= Default;
		ubyte						_subpassIndex			= UMax;
		EShaderStages				_activeStages			= Default;

		Strong<PipelineLayoutID>	_layoutId;

		ArrayView<ShaderTracePtr>	_dbgTrace;				// allocated by pipeline pack linear allocator

		GFX_DBG_ONLY( DebugName_t	_debugName;	)


	// methods
	public:
		VMeshPipeline ()													__NE___	{}
		~VMeshPipeline ()													__NE___;

		ND_ bool  Create (ResourceManager &, const CreateInfo &ci)			__NE___;
			void  Destroy (ResourceManager &)								__NE___;

		ND_ bool  ParseShaderTrace (const void*					ptr,
									Bytes						maxSize,
									ShaderDebugger::ELogFormat	logFmt,
									OUT Array<String>			&result)	C_NE___;

		ND_ bool  ParseShaderAsserts (const void*					ptr,
									  Bytes							maxSize,
									  ShaderDebugger::ELogFormat	logFmt,
									  OUT Array<String>				&result) C_NE___;

		ND_ VkPipeline				Handle ()								C_NE___	{ return _handle; }
		ND_ VkPipelineLayout		Layout ()								C_NE___	{ return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()							C_NE___	{ return VK_PIPELINE_BIND_POINT_GRAPHICS; }
		ND_ PipelineLayoutID		LayoutId ()								C_NE___	{ return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()							C_NE___	{ return _dynamicState; }
		ND_ uint					RenderPassSubpassIndex ()				C_NE___	{ return _subpassIndex; }
		ND_ EPipelineOpt			Options ()								C_NE___	{ return _options; }
		ND_ EShaderStages			GetActiveStages ()						C_NE___	{ return _activeStages; }

		ND_ uint3					TaskLocalSize ()						C_NE___	{ return uint3{_taskLocalSize}; }
		ND_ uint3					MeshLocalSize ()						C_NE___	{ return uint3{_meshLocalSize}; }

		ND_ ArrayView<ShaderTracePtr> GetShaderTrace ()						C_NE___	{ return _dbgTrace; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()						C_NE___	{ return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
