// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VGraphicsPipeline.h"

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
			VPipelinePack::Allocator_t *						allocator		= null;
		};


	// variables
	private:
		VkPipeline					_handle					= Default;
		VkPipelineLayout			_layout					= Default;
		
		ushort3						_meshLocalSize;
		ushort3						_taskLocalSize;
		EPipelineDynamicState		_dynamicState			= Default;
		EPipelineOpt				_options				= Default;
		//bool						_earlyFragmentTests		= true;
		ubyte						_subpassIndex			= UMax;

		Strong<PipelineLayoutID>	_layoutId;
		
		ArrayView<ShaderTracePtr>	_dbgTrace;				// allocated by pipeline pack linear allocator

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VMeshPipeline ()											__NE___	{}
		~VMeshPipeline ()											__NE___;

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci)	__NE___;
			void  Destroy (VResourceManager &)						__NE___;
			
		ND_ bool  ParseShaderTrace (const void *ptr, Bytes maxSize, ShaderDebugger::ELogFormat, OUT Array<String> &result) C_NE___;

		ND_ VkPipeline				Handle ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()					C_NE___	{ return VK_PIPELINE_BIND_POINT_GRAPHICS; }
		ND_ PipelineLayoutID		LayoutID ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		//ND_ bool					IsEarlyFragmentTests ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _earlyFragmentTests; }
		ND_ uint					RenderPassSubpassIndex ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassIndex; }
		
		ND_ uint3					TaskLocalSize ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_taskLocalSize}; }
		ND_ uint3					MeshLocalSize ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_meshLocalSize}; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
