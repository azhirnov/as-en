// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

	//
	// Vulkan Compute Pipeline
	//

	class VComputePipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			PipelineCompiler::SerializableComputePipeline const&	templCI;
			ComputePipelineDesc const&								specCI;
			VPipelineLayoutID										layoutId;
			VPipelinePack::ShaderModuleRef							shader;
			PipelineCacheID											cacheId;
		};


	// variables
	private:
		VkPipeline					_handle				= Default;
		VkPipelineLayout			_layout				= Default;
		
		uint3						_localGroupSize;
		EPipelineOpt				_options			= Default;

		Strong<VPipelineLayoutID>	_layoutId;
		
		DEBUG_ONLY(	DebugName_t			_debugName;	)
		DRC_ONLY(	RWDataRaceCheck		_drCheck;	)


	// methods
	public:
		VComputePipeline () {}
		~VComputePipeline ();

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci);
			void  Destroy (VResourceManager &);

		ND_ VkPipeline				Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()			const	{ return VK_PIPELINE_BIND_POINT_COMPUTE; }
		ND_ VPipelineLayoutID		LayoutID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ uint3 const&			LocalSize ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _localGroupSize; }
		ND_ EPipelineDynamicState	DynamicState ()			const	{ return Default; }
		ND_ bool					AllowDispatchBase ()	const	{ DRC_SHAREDLOCK( _drCheck );  return AllBits( _options, EPipelineOpt::CS_DispatchBase ); }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
