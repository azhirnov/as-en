// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

	//
	// Vulkan Graphics Pipeline
	//

	class VGraphicsPipeline
	{
	// types
	public:
		struct CreateInfo
		{
			VPipelinePack const&									pplnPack;
			PipelineCompiler::SerializableGraphicsPipeline const&	templCI;
			GraphicsPipelineDesc const&								specCI;
			PipelineLayoutID										layoutId;
			ArrayView< VPipelinePack::ShaderModuleRef >				shaders;
			PipelineCacheID											cacheId;
			IAllocator *											allocator	= null;
		};

	private:
		using VBtoIndex_t	= FixedMap< VertexBufferName::Optimized_t, ubyte, GraphicsConfig::MaxVertexBuffers >;
		using VBArray_t		= ArrayView< VertexBufferName::Optimized_t >;


	// variables
	private:
		VkPipeline					_handle					= Default;
		VkPipelineLayout			_layout					= Default;

		EPipelineDynamicState		_dynamicState			= Default;
		EPipelineOpt				_options				= Default;
		EPrimitive					_topology				= Default;
		ubyte						_subpassIndex			= UMax;

		VBtoIndex_t					_vertexBuffers;

		Strong<PipelineLayoutID>	_layoutId;

		ArrayView<ShaderTracePtr>	_dbgTrace;				// allocated by pipeline pack linear allocator

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VGraphicsPipeline ()												__NE___	{}
		~VGraphicsPipeline ()												__NE___;

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci)			__NE___;
			void  Destroy (VResourceManager &)								__NE___;

		ND_ uint  GetVertexBufferIndex (VertexBufferName::Ref name)			C_NE___;

		ND_ bool  ParseShaderTrace (const void*					ptr,
									Bytes						maxSize,
									ShaderDebugger::ELogFormat	logFmt,
									OUT Array<String>			&result)	C_NE___;

		ND_ VkPipeline				Handle ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()							C_NE___	{ return VK_PIPELINE_BIND_POINT_GRAPHICS; }
		ND_ PipelineLayoutID		LayoutId ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		ND_ EPrimitive				Topology ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _topology; }
		ND_ uint					RenderPassSubpassIndex ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassIndex; }
		ND_ VBArray_t				GetVertexBuffers ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vertexBuffers.GetKeyArray(); }
		ND_ VBtoIndex_t const&		GetVertexBufferMap ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vertexBuffers; }
		ND_ EPipelineOpt			Options ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _options; }

		ND_ ArrayView<ShaderTracePtr> GetShaderTrace ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dbgTrace; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
