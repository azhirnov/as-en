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
			MPipelinePack::Allocator_t *							allocator		= null;
		};

	private:
		using VBtoIndex_t = FixedMap< VertexBufferName::Optimized_t, MBufferIndex, GraphicsConfig::MaxVertexBuffers >;


	// variables
	private:
		MetalRenderPipelineRC		_pipeline;
		MetalDepthStencilStateRC	_dsState;
		MDynamicRenderState			_renderState;
		
		EPipelineDynamicState		_dynamicState		= Default;
		ubyte						_rasterOrderGroup	= UMax;

		VBtoIndex_t					_vertexBuffers;
		
		Strong<MPipelineLayoutID>	_layoutId;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MGraphicsPipeline ()											__NE___	{}
		~MGraphicsPipeline ()											__NE___;

		ND_ bool  Create (MResourceManager &, const CreateInfo &ci)		__NE___;
			void  Destroy (MResourceManager &)							__NE___;
		
		ND_ uint  GetVertexBufferIndex (const VertexBufferName &name)	C_NE___;

		ND_ MetalRenderPipeline			Handle ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pipeline; }
		ND_ MetalDepthStencilState		DepthStencilState ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dsState; }
		ND_ MDynamicRenderState const&	GetRenderState ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _renderState; }
		//ND_ auto						GetRenderState ()				C_NE___	{ return DRC_WRAP( _renderState, _drCheck ); }
		ND_ MPipelineLayoutID			LayoutID ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState		DynamicState ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		ND_ EPrimitive					Topology ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _renderState.topology; }
		ND_ uint						RasterOrderGroup ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _rasterOrderGroup; }
		
		DEBUG_ONLY( ND_ StringView		GetDebugName ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_METAL
