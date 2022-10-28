// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/Resources/VPipelinePack.h"

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
			PipelineCompiler::SerializableTilePipeline const&	templCI;
			TilePipelineDesc const&								specCI;
			VPipelineLayoutID									layoutId;
			VPipelinePack::ShaderModuleRef						shader;
			PipelineCacheID										cacheId;
		};


	// variables
	private:
		VkPipeline					_handle				= Default;
		VkPipelineLayout			_layout				= Default;
		
		ushort2						_localSize;
		EPipelineOpt				_options			= Default;

		Strong<VPipelineLayoutID>	_layoutId;
		
		ShaderTracePtr				_dbgTrace;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VTilePipeline () {}
		~VTilePipeline ();

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci);
			void  Destroy (VResourceManager &);
			
		ND_ bool  ParseShaderTrace (const void *ptr, Bytes maxSize, OUT Array<String> &result) const;

		ND_ VkPipeline				Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()			const	{ return VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI; }
		ND_ VPipelineLayoutID		LayoutID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ uint2					LocalSize ()			const	{ DRC_SHAREDLOCK( _drCheck );  return uint2{_localSize}; }
		ND_ EPipelineDynamicState	DynamicState ()			const	{ return Default; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
