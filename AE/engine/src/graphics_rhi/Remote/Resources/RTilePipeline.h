// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Resources/RPipelinePack.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Tile Pipeline
	//

	class RTilePipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			RemoteGraphics::Msg::ResMngr_CreateTilePipeline_Response &	ref;
			RPipelinePack const&										pack;
			bool														releaseRef = false;
		};


	// variables
	private:
		RmTilePipelineID			_pplnId;
		WGLocalSize2_t				_localSize;
		EPipelineOpt				_options			= Default;
		ubyte						_subpassIndex		= UMax;
		bool						_releaseRef			= false;
		Strong<PipelineLayoutID>	_layoutId;
		ShaderTracePtr				_dbgTrace;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RTilePipeline ()												__NE___	{}
		~RTilePipeline ()												__NE___;

		ND_ bool  Create (ResourceManager &, const CreateInfo &ci)		__NE___;
			void  Destroy (ResourceManager &)							__NE___;

		ND_ bool  ParseShaderTrace (const void*					ptr,
									Bytes						maxSize,
									ShaderDebugger::ELogFormat	logFmt,
									OUT Array<String>			&result) C_NE___;

		ND_ bool  ParseShaderAsserts (const void*					ptr,
									  Bytes							maxSize,
									  ShaderDebugger::ELogFormat	logFmt,
									  OUT Array<String>				&result) C_NE___;

		ND_ RmTilePipelineID		Handle ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pplnId; }
		ND_ PipelineLayoutID		LayoutId ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ uint2					LocalSize ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint2{_localSize}; }
		ND_ EPipelineDynamicState	DynamicState ()						C_NE___	{ return Default; }
		ND_ uint					RenderPassSubpassIndex ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassIndex; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()					C_NE___	{ return Default; })
	};

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
