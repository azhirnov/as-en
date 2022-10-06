// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/Resources/MPipelinePack.h"

namespace AE::Graphics
{

	//
	// Metal Tile Pipeline
	//

	class MTilePipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			PipelineCompiler::SerializableTilePipeline const&	templCI;
			TilePipelineDesc const&								specCI;
			MPipelineLayoutID									layoutId;
			MPipelinePack::ShaderModuleRef						shader;
			PipelineCacheID										cacheId;
		};


	// variables
	private:
		MetalRenderPipelineRC		_handle;
		
		uint2						_localGroupSize;
		EPipelineOpt				_options			= Default;
		
		Strong<MPipelineLayoutID>	_layoutId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MTilePipeline () {}
		~MTilePipeline ();

		ND_ bool  Create (MResourceManager &, const CreateInfo &ci);
			void  Destroy (MResourceManager &);

		ND_ MetalRenderPipeline		Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ uint2 const&			LocalSize ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _localGroupSize; }
		ND_ MPipelineLayoutID		LayoutID ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_METAL
