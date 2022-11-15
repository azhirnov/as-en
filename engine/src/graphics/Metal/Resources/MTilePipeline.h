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
			MPipelinePack::Allocator_t *						allocator		= null;
		};


	// variables
	private:
		MetalRenderPipelineRC		_pipeline;
		
		ushort2						_localSize;
		
		Strong<MPipelineLayoutID>	_layoutId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MTilePipeline () {}
		~MTilePipeline ();

		ND_ bool  Create (MResourceManager &, const CreateInfo &ci);
			void  Destroy (MResourceManager &);

		ND_ MetalRenderPipeline		Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _pipeline; }
		ND_ MPipelineLayoutID		LayoutID ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ uint2					LocalSize ()		const	{ DRC_SHAREDLOCK( _drCheck );  return uint2{_localSize}; }
		ND_ EPipelineDynamicState	DynamicState ()		const	{ return Default; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_METAL
