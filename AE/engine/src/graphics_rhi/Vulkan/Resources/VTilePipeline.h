// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Wrapper for tile pipeline (VK_HUAWEI_subpass_shading).

	TODO: VK_EXT_shader_tile_image
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

	//
	// Vulkan Tile Pipeline
	//

	class VTilePipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			VPipelinePack const&								pplnPack;
			PipelineCompiler::SerializableTilePipeline const&	templCI;
			TilePipelineDesc const&								specCI;
			PipelineLayoutID									layoutId;
			VPipelinePack::ShaderModuleRef						shader;
			PipelineCacheID										cacheId;
		};


	// variables
	private:
		VkPipeline					_handle				= Default;
		VkPipelineLayout			_layout				= Default;

		WGLocalSize2_t				_localSize;
		EPipelineOpt				_options			= Default;
		ubyte						_subpassIndex		= UMax;

		Strong<PipelineLayoutID>	_layoutId;

		ShaderTracePtr				_dbgTrace;

		GFX_DBG_ONLY( DebugName_t	_debugName;	)


	// methods
	public:
		VTilePipeline ()												__NE___	{}
		~VTilePipeline ()												__NE___;

		ND_ bool  Create (ResourceManager &, const CreateInfo &ci)		__NE___;
			void  Destroy (ResourceManager &)							__NE___;

		ND_ bool  ParseShaderTrace (const void*					ptr,
									Bytes						maxSize,
									ShaderDebugger::ELogFormat	logFmt,
									OUT Array<String>			&result) C_NE___;

		ND_ bool  ParseShaderAsserts (const void*					ptr,
									  Bytes							maxSize,
									  ShaderDebugger::ELogFormat	logFmt,
									  OUT Array<String>				&result) C_NE___;

		ND_ VkPipeline				Handle ()							C_NE___	{ return _handle; }
		ND_ VkPipelineLayout		Layout ()							C_NE___	{ return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()						C_NE___	{ return VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI; }
		ND_ PipelineLayoutID		LayoutId ()							C_NE___	{ return _layoutId; }
		ND_ uint2					LocalSize ()						C_NE___	{ return uint2{_localSize}; }
		ND_ EPipelineDynamicState	DynamicState ()						C_NE___	{ return Default; }
		ND_ uint					RenderPassSubpassIndex ()			C_NE___	{ return _subpassIndex; }
		ND_ EPipelineOpt			Options ()							C_NE___	{ return _options; }
		ND_ EShaderStages			GetActiveStages ()					C_NE___	{ return EShaderStages::Tile; }

		ND_ ShaderTracePtr			GetShaderTrace ()					C_NE___	{ return _dbgTrace; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()					C_NE___	{ return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
