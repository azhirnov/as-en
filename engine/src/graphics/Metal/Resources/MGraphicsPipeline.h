// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/Resources/MPipelinePack.h"

namespace AE::Graphics
{

	//
	// Metal Graphics Pipeline
	//

	class MGraphicsPipeline
	{
	// types
	public:
		struct CreateInfo
		{
			PipelineCompiler::SerializableGraphicsPipeline const&	templCI;
			GraphicsPipelineDesc const&								specCI;
			MPipelineLayoutID										layoutId;
			ArrayView< MPipelinePack::ShaderModuleRef >				shaders;
			PipelineCacheID											cacheId;
		};

	private:
		using VBtoIndex_t = FixedMap< VertexBufferName::Optimized_t, MBufferIndex, GraphicsConfig::MaxVertexBuffers >;


	// variables
	private:
		MetalRenderPipelineRC		_pipeline;
		MetalDepthStencilStateRC	_dsState;
		MDynamicRenderState			_renderState;
		
		EPipelineOpt				_options			= Default;
		EPrimitive					_topology			= Default;
		EPipelineDynamicState		_dynamicState		= Default;

		VBtoIndex_t					_vertexBuffers;
		
		Strong<MPipelineLayoutID>	_layoutId;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MGraphicsPipeline () {}
		~MGraphicsPipeline ();

		ND_ bool  Create (MResourceManager &, const CreateInfo &ci);
			void  Destroy (MResourceManager &);
		
		ND_ uint  GetVertexBufferIndex (const VertexBufferName &name) const;

		ND_ MetalRenderPipeline			Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _pipeline; }
		ND_ MetalDepthStencilState		DepthStencilState ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _dsState; }
		ND_ MDynamicRenderState const&	GetRenderState ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _renderState; }
		//ND_ auto						GetRenderState ()		const	{ return DRC_WRAP( _renderState, _drCheck ); }
		ND_ MPipelineLayoutID			LayoutID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState		DynamicState ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		ND_ EPrimitive					Topology ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _topology; }
		
		DEBUG_ONLY( ND_ StringView		GetDebugName ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_METAL
