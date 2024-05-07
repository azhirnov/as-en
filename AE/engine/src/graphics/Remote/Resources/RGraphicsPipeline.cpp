// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RGraphicsPipeline.h"
# include "RPipelineHelper.cpp.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RGraphicsPipeline::~RGraphicsPipeline () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _pplnId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RGraphicsPipeline::Create (RResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _pplnId );

		_layoutId = resMngr.AcquireResource( ci.pack.Cast( ci.ref.pipelineLayoutId ));
		CHECK_ERR( _layoutId );

		_pplnId			= ci.ref.pplnId;
		_dynamicState	= ci.ref.dynamicState;
		_options		= ci.ref.options;
		_topology		= ci.ref.topology;
		_subpassIndex	= ci.ref.subpassIndex;
		_vertexBuffers	= ci.ref.vertexBuffers;
		_releaseRef		= ci.releaseRef;
		_dbgTrace		= RVRef(ci.ref.shaderTrace);

		CHECK_ERR( _pplnId );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RGraphicsPipeline::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _pplnId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_pplnId} ));

		resMngr.ImmediatelyRelease( _layoutId );

		_pplnId			= Default;
		_dynamicState	= Default;
		_options		= Default;
		_topology		= Default;
		_subpassIndex	= UMax;
		_vertexBuffers.clear();
		_dbgTrace.clear();
	}

/*
=================================================
	GetVertexBufferIndex
=================================================
*/
	uint  RGraphicsPipeline::GetVertexBufferIndex (VertexBufferName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _vertexBuffers.find( name );
		return it != _vertexBuffers.end() ? it->second : UMax;
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  RGraphicsPipeline::ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
	{
		result.clear();
		DRC_SHAREDLOCK( _drCheck );

		for (auto& trace : _dbgTrace)
		{
			Array<String>	temp;
			CHECK_ERR( trace->ParseShaderTrace( ptr, maxSize, ConvertLogFormat(format), OUT temp ));
			result.insert( result.end(), temp.begin(), temp.end() );
		}
		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
