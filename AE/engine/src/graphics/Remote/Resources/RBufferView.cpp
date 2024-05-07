// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/ResourceValidation.h"
# include "graphics/Remote/Resources/RBufferView.h"
# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/REnumCast.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RBufferView::~RBufferView () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _viewId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RBufferView::Create (RResourceManager &resMngr, const BufferViewDesc &desc, BufferID bufferId, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _viewId );

		const RBuffer*	buffer = resMngr.GetResource( bufferId, True{"incRef"} );
		CHECK_ERR( buffer != null );
		_bufferId = Strong<BufferID>{bufferId};

		_desc = desc;
		_desc.Validate( buffer->Description() );
		GRES_CHECK( BufferView_IsSupported( resMngr, buffer->Description(), _desc ));

		auto&	dev = resMngr.GetDevice();

		Msg::ResMngr_CreateBufferView				msg;
		RC<Msg::ResMngr_CreateBufferView_Response>	res;

		msg.desc		= _desc;
		msg.bufferId	= buffer->Handle();
		msg.dbgName		= dbgName;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		_viewId		= res->viewId;
		_desc		= res->desc;
		_releaseRef	= true;
		CHECK_ERR( _viewId );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  RBufferView::Create (RResourceManager &resMngr, const RemoteBufferViewDesc &desc, BufferID bufferId, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _viewId );

		const RBuffer*	buffer = resMngr.GetResource( bufferId, True{"incRef"} );
		CHECK_ERR( buffer != null );
		_bufferId = Strong<BufferID>{bufferId};

		_desc = desc.desc;
		_desc.Validate( buffer->Description() );
		GRES_CHECK( BufferView_IsSupported( resMngr, buffer->Description(), _desc ));

		_viewId		= desc.viewId;
		_releaseRef	= desc.releaseRef;
		CHECK_ERR( _viewId );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RBufferView::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _viewId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_viewId} ));

		resMngr.ImmediatelyRelease( INOUT _bufferId );

		auto	id	= _viewId;
		_bufferId	= Default;
		_viewId		= Default;
		_desc		= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
