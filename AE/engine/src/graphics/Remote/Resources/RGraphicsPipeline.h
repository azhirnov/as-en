// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RPipelinePack.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Graphics Pipeline
	//

	class RGraphicsPipeline
	{
	// types
	public:
		struct CreateInfo
		{
			RemoteGraphics::Msg::ResMngr_CreateGraphicsPipeline_Response&	ref;
			RPipelinePack const&											pack;
			bool															releaseRef = false;
		};

	private:
		using VBtoIndex_t	= FixedMap< VertexBufferName::Optimized_t, ubyte, GraphicsConfig::MaxVertexBuffers >;
		using VBArray_t		= ArrayView< VertexBufferName::Optimized_t >;


	// variables
	private:
		RmGraphicsPipelineID		_pplnId;

		EPipelineDynamicState		_dynamicState		= Default;
		EPipelineOpt				_options			= Default;
		EPrimitive					_topology			= Default;
		ubyte						_subpassIndex		= UMax;
		bool						_releaseRef			= false;

		VBtoIndex_t					_vertexBuffers;
		Strong<PipelineLayoutID>	_layoutId;

		Array<ShaderTracePtr>		_dbgTrace;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RGraphicsPipeline ()											__NE___	{}
		~RGraphicsPipeline ()											__NE___;

		ND_ bool  Create (RResourceManager &, const CreateInfo &ci)		__NE___;
			void  Destroy (RResourceManager &)							__NE___;

		ND_ uint  GetVertexBufferIndex (VertexBufferName::Ref name)		C_NE___;

		ND_ bool  ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat, OUT Array<String> &result)	C_NE___;

		ND_ RmGraphicsPipelineID	Handle ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pplnId; }
		ND_ PipelineLayoutID		LayoutId ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		ND_ EPrimitive				Topology ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _topology; }
		ND_ uint					RenderPassSubpassIndex ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassIndex; }
		ND_ VBArray_t				GetVertexBuffers ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _vertexBuffers.GetKeyArray(); }
		ND_ EPipelineOpt			Options ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _options; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()					C_NE___	{ return Default; })
	};

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
