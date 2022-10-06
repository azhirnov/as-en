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
		};


	// variables
	private:
		MetalRenderPipelineRC		_handle;
		
		EPipelineOpt				_options			= Default;
		EPrimitive					_topology			= Default;
		
		Strong<MPipelineLayoutID>	_layoutId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MMeshPipeline () {}
		~MMeshPipeline ();

		ND_ bool  Create (MResourceManager &, const CreateInfo &ci);
			void  Destroy (MResourceManager &);

		ND_ MetalRenderPipeline		Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ EPrimitive				Topology ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _topology; }
		ND_ MPipelineLayoutID		LayoutID ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_METAL
