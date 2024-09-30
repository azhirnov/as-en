// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/ResourceValidation.h"
# include "graphics/Remote/Resources/RImage.h"
# include "graphics/Remote/REnumCast.h"
# include "graphics/Private/EnumUtils.h"
# include "graphics/Remote/RResourceManager.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RImage::~RImage () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _imageId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RImage::Create (RResourceManager &resMngr, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _imageId );
		CHECK_ERR( All( desc.dimension > ImageDim_t{0} ));
		CHECK_ERR( desc.imageDim != Default );
		CHECK_ERR( desc.arrayLayers > 0_layer );
		CHECK_ERR( desc.mipLevels > 0_mipmap );
		CHECK_ERR( desc.format != Default );
		CHECK_ERR( desc.usage != Default );
		CHECK_ERR( EPixelFormat_PlaneCount( desc.format ) == 0 );	// use VideoImage instead

		_desc = desc;
		_desc.Validate();
		GRES_CHECK( Image_IsSupported( resMngr, _desc, True{"imageFormatList"} ));

		auto&	dev = resMngr.GetDevice();

		Msg::ResMngr_CreateImage				msg;
		RC<Msg::ResMngr_CreateImage_Response>	res;

		msg.desc		= _desc;
		msg.gfxAlloc	= dev.Cast( allocator );
		msg.dbgName		= dbgName;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		_imageId	= res->imageId;
		_memoryId	= res->memoryId;
		_desc		= res->desc;
		_releaseRef	= true;
		CHECK_ERR( _imageId );

		_allocator = RVRef(allocator);
		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  RImage::Create (RResourceManager &resMngr, const RemoteImageDesc &desc, GfxMemAllocatorPtr, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _imageId );

		_desc = desc.desc;
		_desc.Validate();
		GRES_CHECK( Image_IsSupported( resMngr, _desc, True{"imageFormatList"} ));

		_imageId	= desc.imageId;
		_releaseRef	= desc.releaseRef;
		CHECK_ERR( _imageId );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RImage::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _imageId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_imageId} ));

		_imageId	= Default;
		_memoryId	= Default;
		_desc		= Default;
		_allocator	= null;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetMemoryInfo
=================================================
*/
	bool  RImage::GetMemoryInfo (OUT RemoteMemoryObjInfo &info) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		info = Default;
		return true;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  RImage::IsSupported (const RResourceManager &resMngr, const ImageDesc &desc) __NE___
	{
		if ( not Image_IsSupported( resMngr, desc, True{"imageFormatList"} ))
			return false;

		Msg::ResMngr_IsSupported_ImageDesc		msg;
		RC<Msg::ResMngr_IsSupported_Response>	res;

		msg.desc = desc;

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->supported;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  RImage::IsSupported (const RResourceManager &resMngr, const ImageViewDesc &desc) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		if ( not ImageView_IsSupported( resMngr, _desc, desc ))
			return false;

		Msg::ResMngr_IsSupported_ImageViewDesc	msg;
		RC<Msg::ResMngr_IsSupported_Response>	res;

		msg.imageId	= _imageId;
		msg.desc	= desc;

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->supported;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
