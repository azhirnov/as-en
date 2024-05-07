// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/ResourceValidation.h"
# include "graphics/Remote/Resources/RImageView.h"
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
	RImageView::~RImageView () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _viewId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RImageView::Create (RResourceManager &resMngr, const ImageViewDesc &desc, ImageID imageId, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _viewId );

		const RImage*	image = resMngr.GetResource( imageId, True{"incRef"} );
		CHECK_ERR( image != null );
		_imageId = Strong<ImageID>{imageId};

		const auto&	img_desc = image->Description();

		_desc = desc;
		_desc.Validate( img_desc );
		GRES_CHECK( ImageView_IsSupported( resMngr, img_desc, _desc ));

		auto&	dev = resMngr.GetDevice();

		Msg::ResMngr_CreateImageView				msg;
		RC<Msg::ResMngr_CreateImageView_Response>	res;

		msg.imageId		= image->Handle();
		msg.desc		= _desc;
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
	bool  RImageView::Create (RResourceManager &resMngr, const RemoteImageViewDesc &desc, ImageID imageId, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _viewId );

		const RImage*	image = resMngr.GetResource( imageId, True{"incRef"} );
		CHECK_ERR( image != null );
		_imageId = Strong<ImageID>{imageId};

		const auto&	img_desc = image->Description();

		_desc = desc.desc;
		_desc.Validate( img_desc );
		GRES_CHECK( ImageView_IsSupported( resMngr, img_desc, _desc ));

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
	void  RImageView::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _viewId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_viewId} ));

		resMngr.ImmediatelyRelease( INOUT _imageId );

		_imageId	= Default;
		_viewId		= Default;
		_desc		= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
