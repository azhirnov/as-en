// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RMeshPipeline.h"
# include "RPipelineHelper.cpp.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RMeshPipeline::~RMeshPipeline () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _pplnId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RMeshPipeline::Create (RResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _pplnId );

		_layoutId = resMngr.AcquireResource( ci.pack.Cast( ci.ref.pipelineLayoutId ));
		CHECK_ERR( _layoutId );

		_pplnId			= ci.ref.pplnId;
		_meshLocalSize	= ci.ref.meshLocalSize;
		_taskLocalSize	= ci.ref.taskLocalSize;
		_dynamicState	= ci.ref.dynamicState;
		_options		= ci.ref.options;
		_subpassIndex	= ci.ref.subpassIndex;
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
	void  RMeshPipeline::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _pplnId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_pplnId} ));

		resMngr.ImmediatelyRelease( _layoutId );

		_pplnId			= Default;
		_meshLocalSize	= Default;
		_taskLocalSize	= Default;
		_dynamicState	= Default;
		_options		= Default;
		_subpassIndex	= Default;
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  RMeshPipeline::ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
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
