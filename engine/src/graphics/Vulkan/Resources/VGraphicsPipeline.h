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
			VPipelineLayoutID										layoutId;
			ArrayView< VPipelinePack::ShaderModuleRef >				shaders;
			PipelineCacheID											cacheId;
			VPipelinePack::Allocator_t *							allocator	= null;
		};

	private:
		using VBtoIndex_t = FixedMap< VertexBufferName::Optimized_t, ubyte, GraphicsConfig::MaxVertexBuffers >;


	// variables
	private:
		VkPipeline					_handle					= Default;
		VkPipelineLayout			_layout					= Default;
		
		EPipelineDynamicState		_dynamicState			= Default;
		EPipelineOpt				_options				= Default;
		//uint						_patchControlPoints		= 0;
		//bool						_earlyFragmentTests		= true;
		EPrimitive					_topology				= Default;
		ubyte						_subpassIndex			= UMax;

		VBtoIndex_t					_vertexBuffers;

		Strong<VPipelineLayoutID>	_layoutId;
		
		ArrayView<ShaderTracePtr>	_dbgTrace;				// allocated by pipeline pack linear allocator

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VGraphicsPipeline ()											__NE___	{}
		~VGraphicsPipeline ()											__NE___;

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci)		__NE___;
			void  Destroy (VResourceManager &)							__NE___;
		
		ND_ uint  GetVertexBufferIndex (const VertexBufferName &name)	C_NE___;
		
		ND_ bool  ParseShaderTrace (const void *ptr, Bytes maxSize, OUT Array<String> &result) C_NE___;

		ND_ VkPipeline				Handle ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()						C_NE___	{ return VK_PIPELINE_BIND_POINT_GRAPHICS; }
		ND_ VPipelineLayoutID		LayoutID ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		ND_ EPrimitive				Topology ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _topology; }
		//ND_ bool					IsEarlyFragmentTests ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _earlyFragmentTests; }
		ND_ uint					RenderPassSubpassIndex ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassIndex; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
