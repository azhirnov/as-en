// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RPipelinePack.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Ray Tracing Pipeline
	//

	class RRayTracingPipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			RemoteGraphics::Msg::ResMngr_CreateRayTracingPipeline_Response&		ref;	// allow to move data
			RPipelinePack const&												pack;
			bool																releaseRef = false;
		};
	private:
		using NameToHandle_t	= FlatHashMap< RayTracingGroupName::Optimized_t, uint >;	// name to index in '_groupHandles'


	// variables
	private:
		RmRayTracingPipelineID		_pplnId;
		EPipelineDynamicState		_dynamicState		= Default;
		EPipelineOpt				_options			= Default;
		Bytes16u					_shaderGroupHandleSize;
		bool						_releaseRef			= false;
		NameToHandle_t				_nameToHandle;
		Array< ulong >				_groupHandles;
		Strong<PipelineLayoutID>	_layoutId;
		Array<ShaderTracePtr>		_dbgTrace;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RRayTracingPipeline ()												__NE___	{}
		~RRayTracingPipeline ()												__NE___;

		ND_ bool  Create (RResourceManager &, const CreateInfo &ci)			__NE___;
			void  Destroy (RResourceManager &)								__NE___;

		ND_ bool  ParseShaderTrace (const void*					ptr,
									Bytes						maxSize,
									ShaderDebugger::ELogFormat	logFmt,
									OUT Array<String>			&result)	C_NE___;

		ND_ Bytes  GetShaderGroupStackSize (const RDevice			 &dev,
											ArrayView<RayTracingGroupName>,
											ERTShaderGroup			 type)	C_NE___;

			bool  CopyHandle (const RDevice				&,
							  RayTracingGroupName::Ref	,
							  OUT void*					dst,
							  Bytes						dstSize)			C_NE___;

			bool  CopyHandle (const RDevice				&,
							  uint						index,
							  OUT void*					dst,
							  Bytes						dstSize)			C_NE___;

		ND_ RmRayTracingPipelineID	Handle ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pplnId; }
		ND_ PipelineLayoutID		LayoutId ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()						C_NE___	{ return Default; })
	};

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
