// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/Resources/MPipelinePack.h"

namespace AE::Graphics
{

	//
	// Metal Compute Pipeline
	//

	class MComputePipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			PipelineCompiler::SerializableComputePipeline const&	templCI;
			ComputePipelineDesc const&								specCI;
			MPipelineLayoutID										layoutId;
			MPipelinePack::ShaderModuleRef							shader;
			PipelineCacheID											cacheId;
		};


	// variables
	private:
		MetalComputePipelineRC		_handle;
		
		uint3						_localGroupSize;
		EPipelineOpt				_options			= Default;
		
		Strong<MPipelineLayoutID>	_layoutId;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MComputePipeline () {}
		~MComputePipeline ();

		ND_ bool  Create (MResourceManager &, const CreateInfo &ci);
			void  Destroy (MResourceManager &);
			
		ND_ usize	MaxTotalThreadsPerThreadgroup ()	const;
		ND_ usize	ThreadExecutionWidth ()				const;
		ND_ Bytes	StaticThreadgroupMemoryLength ()	const;
		ND_ Bytes	ImageblockMemoryLength (const uint2& imageBlockDim) const;

		ND_ MetalComputePipeline	Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ MPipelineLayoutID		LayoutID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ uint3 const&			LocalSize ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _localGroupSize; }
		ND_ EPipelineDynamicState	DynamicState ()			const	{ return Default; }
		ND_ bool					AllowDispatchBase ()	const	{ return false; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_METAL
