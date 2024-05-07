// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RRayTracingPipeline.h"
# include "RPipelineHelper.cpp.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RRayTracingPipeline::~RRayTracingPipeline () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _pplnId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RRayTracingPipeline::Create (RResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _pplnId );

		_layoutId = resMngr.AcquireResource( ci.pack.Cast( ci.ref.pipelineLayoutId ));
		CHECK_ERR( _layoutId );

		_pplnId					= ci.ref.pplnId;
		_dynamicState			= ci.ref.dynamicState;
		_options				= ci.ref.options;
		_shaderGroupHandleSize	= ci.ref.shaderGroupHandleSize;
		_nameToHandle			= RVRef(ci.ref.nameToHandle);
		_groupHandles			= RVRef(ci.ref.groupHandles);
		_releaseRef				= ci.releaseRef;
		_dbgTrace				= RVRef(ci.ref.shaderTrace);

		CHECK_ERR( _pplnId );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RRayTracingPipeline::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _pplnId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_pplnId} ));

		resMngr.ImmediatelyRelease( _layoutId );

		_pplnId					= Default;
		_dynamicState			= Default;
		_options				= Default;
		_shaderGroupHandleSize	= 0_b;
		_nameToHandle.clear();
		_groupHandles.clear();
		_dbgTrace.clear();
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  RRayTracingPipeline::ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
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

/*
=================================================
	CopyHandle
=================================================
*/
	bool  RRayTracingPipeline::CopyHandle (const RDevice &, RayTracingGroupName::Ref name, OUT void* dst, Bytes dstSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		const Bytes		handle_size	{_shaderGroupHandleSize};
		CHECK_ERR( dstSize <= handle_size );

		auto	it = _nameToHandle.find( name );
		if ( it != _nameToHandle.end() )
		{
			const Bytes		offset = it->second * handle_size;
			GRES_CHECK( offset + handle_size <= ArraySizeOf(_groupHandles) );

			MemCopy( OUT dst, _groupHandles.data() + offset, handle_size );
			return true;
		}
		else
		{
			// zero handle makes shader inactive
			ZeroMem( OUT dst, handle_size );
			return false;
		}
	}

/*
=================================================
	CopyHandle
=================================================
*/
	bool  RRayTracingPipeline::CopyHandle (const RDevice &, uint index, OUT void* dst, Bytes dstSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		const Bytes		handle_size	{_shaderGroupHandleSize};
		const Bytes		offset		= index * handle_size;

		CHECK_ERR( dstSize <= handle_size );

		if ( offset + handle_size <= ArraySizeOf(_groupHandles) )
		{
			MemCopy( OUT dst, _groupHandles.data() + offset, handle_size );
			return true;
		}
		else
		{
			// zero handle makes shader inactive
			ZeroMem( OUT dst, handle_size );
			return false;
		}
	}

/*
=================================================
	GetShaderGroupStackSize
=================================================
*/
	Bytes  RRayTracingPipeline::GetShaderGroupStackSize (const RDevice &dev, ArrayView<RayTracingGroupName> names, ERTShaderGroup type) C_NE___
	{
		if ( names.empty() )
			return 0_b;

		DRC_SHAREDLOCK( _drCheck );

		Msg::ResMngr_GetShaderGroupStackSize				msg;
		RC<Msg::ResMngr_GetShaderGroupStackSize_Response>	res;

		msg.pplnId	= _pplnId;
		msg.names	= names;
		msg.type	= type;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		return res->size;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
