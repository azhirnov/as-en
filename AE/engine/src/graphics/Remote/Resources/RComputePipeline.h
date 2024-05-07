// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RPipelinePack.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Compute Pipeline
	//

	class RComputePipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			RemoteGraphics::Msg::ResMngr_CreateComputePipeline_Response&	ref;
			RPipelinePack const&											pack;
			bool															releaseRef = false;
		};


	// variables
	private:
		RmComputePipelineID			_pplnId;
		ushort3						_localSize;
		EPipelineOpt				_options		= Default;
		bool						_releaseRef		= false;
		Strong<PipelineLayoutID>	_layoutId;
		ShaderTracePtr				_dbgTrace;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RComputePipeline ()														__NE___	{}
		~RComputePipeline ()													__NE___;

		ND_ bool  Create (RResourceManager &, const CreateInfo &ci)				__NE___;
			void  Destroy (RResourceManager &)									__NE___;

		ND_ bool  ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat, OUT Array<String> &result)	C_NE___;

		ND_ RmComputePipelineID		Handle ()									C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pplnId; }
		ND_ PipelineLayoutID		LayoutId ()									C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ uint3       			LocalSize ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_localSize}; }
		ND_ EPipelineDynamicState	DynamicState ()								C_NE___	{ return Default; }
		ND_ bool					AllowDispatchBase ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return AllBits( _options, EPipelineOpt::CS_DispatchBase ); }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return Default; })
	};

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
