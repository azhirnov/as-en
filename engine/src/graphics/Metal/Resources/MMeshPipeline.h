// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/Resources/MPipelinePack.h"

namespace AE::Graphics
{

	//
	// Metal Mesh Pipeline
	//

	class MMeshPipeline
	{
	// types
	public:
		struct CreateInfo
		{
			PipelineCompiler::SerializableMeshPipeline const&	templCI;
			MeshPipelineDesc const&								specCI;
			MPipelineLayoutID									layoutId;
			ArrayView< MPipelinePack::ShaderModuleRef >			shaders;
			PipelineCacheID										cacheId;
			MPipelinePack::Allocator_t *							allocator		= null;
		};


	// variables
	private:
		MetalRenderPipelineRC		_pipeline;
		MetalDepthStencilStateRC	_dsState;
		MDynamicRenderState			_renderState;

		ushort3						_meshLocalSize;
		ushort3						_taskLocalSize;
		
		EPipelineDynamicState		_dynamicState		= Default;
		
		Strong<MPipelineLayoutID>	_layoutId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MMeshPipeline () {}
		~MMeshPipeline ();

		ND_ bool  Create (MResourceManager &, const CreateInfo &ci);
			void  Destroy (MResourceManager &);
			
		ND_ MetalRenderPipeline			Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _pipeline; }
		ND_ MetalDepthStencilState		DepthStencilState ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _dsState; }
		ND_ MDynamicRenderState const&	GetRenderState ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _renderState; }
		ND_ MPipelineLayoutID			LayoutID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState		DynamicState ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }

		ND_ uint3						TaskLocalSize ()		const	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_taskLocalSize}; }
		ND_ uint3						MeshLocalSize ()		const	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_meshLocalSize}; }
		
		DEBUG_ONLY(  ND_ StringView		GetDebugName ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_METAL
