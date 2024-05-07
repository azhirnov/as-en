// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RComputePipeline.h"
# include "RPipelineHelper.cpp.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RComputePipeline::~RComputePipeline () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _pplnId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RComputePipeline::Create (RResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _pplnId );

		_layoutId = resMngr.AcquireResource( ci.pack.Cast( ci.ref.pipelineLayoutId ));
		CHECK_ERR( _layoutId );

		_pplnId		= ci.ref.pplnId;
		_localSize	= ci.ref.localSize;
		_options	= ci.ref.options;
		_releaseRef	= ci.releaseRef;
		_dbgTrace	= RVRef(ci.ref.shaderTrace);

		CHECK_ERR( _pplnId );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RComputePipeline::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _pplnId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_pplnId} ));

		resMngr.ImmediatelyRelease( _layoutId );

		_pplnId		= Default;
		_localSize	= Default;
		_options	= Default;
		_dbgTrace	= null;
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  RComputePipeline::ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		return _dbgTrace and _dbgTrace->ParseShaderTrace( ptr, maxSize, ConvertLogFormat(format), OUT result );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
