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
			PipelineCompiler::SerializableMeshPipeline const&	templCI;
			MeshPipelineDesc const&								specCI;
			VPipelineLayoutID									layoutId;
			ArrayView< VPipelinePack::ShaderModuleRef >			shaders;
			PipelineCacheID										cacheId;
		};


	// variables
	private:
		VkPipeline					_handle			= Default;
		VkPipelineLayout			_layout			= Default;
		
		uint						_meshGroupSize			= 0;
		uint						_taskGroupSize			= 0;
		EPipelineDynamicState		_dynamicState			= Default;
		EPipelineOpt				_options				= Default;
		//bool						_earlyFragmentTests		= true;

		Strong<VPipelineLayoutID>	_layoutId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VMeshPipeline () {}
		~VMeshPipeline ();

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci);
			void  Destroy (VResourceManager &);

		ND_ VkPipeline				Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()			const	{ return VK_PIPELINE_BIND_POINT_GRAPHICS; }
		ND_ VPipelineLayoutID		LayoutID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		//ND_ bool					IsEarlyFragmentTests ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _earlyFragmentTests; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
