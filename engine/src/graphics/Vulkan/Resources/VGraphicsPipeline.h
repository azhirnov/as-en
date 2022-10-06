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
			PipelineCompiler::SerializableGraphicsPipeline const&	templCI;
			GraphicsPipelineDesc const&								specCI;
			VPipelineLayoutID										layoutId;
			ArrayView< VPipelinePack::ShaderModuleRef >				shaders;
			PipelineCacheID											cacheId;
		};

	private:
		using VBtoIndex_t = FixedMap< VertexBufferName::Optimized_t, ubyte, GraphicsConfig::MaxVertexBuffers >;


	// variables
	private:
		VkPipeline					_handle			= Default;
		VkPipelineLayout			_layout			= Default;
		
		EPipelineDynamicState		_dynamicState			= Default;
		EPipelineOpt				_options				= Default;
		//uint						_patchControlPoints		= 0;
		//bool						_earlyFragmentTests		= true;
		EPrimitive					_topology				= Default;

		VBtoIndex_t					_vertexBuffers;

		Strong<VPipelineLayoutID>	_layoutId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VGraphicsPipeline () {}
		~VGraphicsPipeline ();

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci);
			void  Destroy (VResourceManager &);
		
		ND_ uint  GetVertexBufferIndex (const VertexBufferName &name) const;

		ND_ VkPipeline				Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()			const	{ return VK_PIPELINE_BIND_POINT_GRAPHICS; }
		ND_ VPipelineLayoutID		LayoutID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		ND_ EPrimitive				Topology ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _topology; }
		//ND_ bool					IsEarlyFragmentTests ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _earlyFragmentTests; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
