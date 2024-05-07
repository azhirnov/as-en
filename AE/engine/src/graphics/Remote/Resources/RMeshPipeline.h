// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RGraphicsPipeline.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Mesh Pipeline
	//

	class RMeshPipeline
	{
	// types
	public:
		struct CreateInfo
		{
			RemoteGraphics::Msg::ResMngr_CreateMeshPipeline_Response&	ref;
			RPipelinePack const&										pack;
			bool														releaseRef = false;
		};


	// variables
	private:
		RmMeshPipelineID			_pplnId;
		ushort3						_meshLocalSize;
		ushort3						_taskLocalSize;
		EPipelineDynamicState		_dynamicState		= Default;
		EPipelineOpt				_options			= Default;
		ubyte						_subpassIndex		= UMax;
		bool						_releaseRef			= false;
		Strong<PipelineLayoutID>	_layoutId;
		Array<ShaderTracePtr>		_dbgTrace;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RMeshPipeline ()											__NE___	{}
		~RMeshPipeline ()											__NE___;

		ND_ bool  Create (RResourceManager &, const CreateInfo &ci)	__NE___;
			void  Destroy (RResourceManager &)						__NE___;

		ND_ bool  ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat, OUT Array<String> &result)	C_NE___;

		ND_ RmMeshPipelineID		Handle ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pplnId; }
		ND_ PipelineLayoutID		LayoutId ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		ND_ uint					RenderPassSubpassIndex ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassIndex; }

		ND_ uint3					TaskLocalSize ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_taskLocalSize}; }
		ND_ uint3					MeshLocalSize ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_meshLocalSize}; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()				C_NE___	{ return Default; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
