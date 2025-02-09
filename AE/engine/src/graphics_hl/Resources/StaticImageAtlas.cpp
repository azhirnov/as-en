// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/LoadableImage.h"
#include "graphics_hl/Resources/StaticImageAtlas.h"
#include "graphics_hl/Resources/ResourceUploadManager.h"

namespace AE::Graphics
{
	using namespace AE::Serializing;
	using namespace AE::AssetPacker;

namespace {
#	include "Packer/ImagePacker.cpp.h"
#	include "Packer/ImageAtlasPacker.cpp.h"
}

/*
=================================================
	destructor
=================================================
*/
	StaticImageAtlas::~StaticImageAtlas () __NE___
	{
		if ( _viewId )
			GraphicsScheduler().GetResourceManager().DelayedReleaseResources( _viewId );
	}

/*
=================================================
	Get
=================================================
*/
	bool  StaticImageAtlas::Get (ImageInAtlasName::Ref name, OUT RectI &region) C_NE___
	{
		auto	it = _nameToIdx.find( name );
		if_likely( it != _nameToIdx.end() )
		{
			region = RectI{ _imageRects[ it->second ]};
			return true;
		}
		return false;
	}

	bool  StaticImageAtlas::Get (ImageInAtlasName::Ref name, OUT RectF &region) C_NE___
	{
		auto	it = _nameToIdx.find( name );
		if_likely( it != _nameToIdx.end() )
		{
			region = RectF{ _imageRects[ it->second ]} * _invImgSize;
			return true;
		}
		return false;
	}

	bool  StaticImageAtlas::Get (ImageInAtlasName::Ref name, OUT RectUNorm &region) C_NE___
	{
		auto	it = _nameToIdx.find( name );
		if_likely( it != _nameToIdx.end() )
		{
			region = FloatToUNormShort( RectF{ _imageRects[ it->second ]} * _invImgSize );
			return true;
		}
		return false;
	}

/*
=================================================
	GetImageDesc / GetViewDesc
=================================================
*/
	ImageDesc  StaticImageAtlas::GetImageDesc () C_NE___
	{
		ASSERT( _imageId );
		return GraphicsScheduler().GetResourceManager().GetDescription( _imageId );
	}

	ImageViewDesc  StaticImageAtlas::GetViewDesc () C_NE___
	{
		ASSERT( _viewId );
		return GraphicsScheduler().GetResourceManager().GetDescription( _viewId );
	}

/*
=================================================
	Loader::OnUploadCompleteTask
=================================================
*/
	class StaticImageAtlas::Loader::OnUploadCompleteTask final : public Threading::IAsyncTask
	{
	private:
		RC<StaticImageAtlas>					_atlas;
		ResourceUploadManager::UploadResult		_upload;

	public:
		OnUploadCompleteTask (RC<StaticImageAtlas> atlas, ResourceUploadManager::UploadResult upload) __NE___ :
			IAsyncTask{ ETaskQueue::Background }, _atlas{RVRef(atlas)}, _upload{RVRef(upload)} {}

		void  Run () __Th_OV
		{
			bool	ok = _upload ? _upload->IsCompleted() : true;
			_atlas->_SetLoadingStatus( ok ? ELoadingStatus::Complete : ELoadingStatus::Failed );

			_atlas  = null;
			_upload = null;
		}

		DEBUG_ONLY( void  OnCancel ()	__NE_OV { DBG_WARNING("should never happens"); })

		StringView  DbgName ()			C_NE_OV { return "on image loading complete"; }
	};

/*
=================================================
	Loader::_Load
=================================================
*/
	bool  StaticImageAtlas::Loader::_Load (Serializing::Deserializer &des, ResourceCache &resCache, CachedResourceName::Ref selfName,
											OUT RC<StaticImageAtlas> &atlas, OUT ResourceUploadManager::UploadResult* outUploadTask) __NE___
	{
		ImageAtlasPacker	unpacker;
		CHECK_ERR( ImageAtlasPacker_Deserialize( OUT unpacker, des ));

		atlas = MakeRC<StaticImageAtlas>();

		auto	img_res = resCache.GetResource<LoadableImage>( CachedResourceName{unpacker.ImageResourceName()} );
		CHECK_ERR_MSG( img_res, "StaticImageAtlas image must be in resource cache!" );

		auto	upload = img_res->OnUploadComplete();
		if ( outUploadTask != null )
			*outUploadTask = upload;

		auto	status = img_res->LoadingStatus();
		CHECK_ERR( status >= ELoadingStatus::Created );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		atlas->_viewId = img_res->CloneImageView();
		CHECK_ERR( atlas->_viewId );

		atlas->_imageId = img_res->ImageId();
		atlas->_SetLoadingStatus( status );

		atlas->_invImgSize	 = 1.0f / float2{res_mngr.GetDescription( atlas->_imageId ).dimension};
		atlas->_nameToIdx	 = RVRef(unpacker.map);
		atlas->_imageRects	 = RVRef(unpacker.rects);

		if ( selfName.IsDefined() )
		{
			if ( auto cached = resCache.InsertResource( selfName, atlas ))
			{
				ASSERT( cached->ImageId() == atlas->ImageId() );
				atlas = RVRef(cached);
				return true;
			}
		}

		// update status after uploading
		if ( upload )
		{
			Scheduler().Run<OnUploadCompleteTask>( Tuple{atlas, upload}, Tuple{ResourceUploadManager::WeakUploadResult{upload}} );
		}
		else
		{
			status = img_res->LoadingStatus();
			CHECK_ERR( status == ELoadingStatus::Complete );
			atlas->_SetLoadingStatus( status );
		}
		return true;
	}

/*
=================================================
	Loader::Load
=================================================
*/
	RC<StaticImageAtlas>  StaticImageAtlas::Loader::Load (Serializing::Deserializer &des, ResourceCache &resCache, CachedResourceName::Ref selfName) __NE___
	{
		RC<StaticImageAtlas>	atlas;
		Unused( _Load( des, resCache, selfName, OUT atlas, null ));
		return atlas;
	}

/*
=================================================
	Loader::LoadAsync
=================================================
*/
	Promise<RC<StaticImageAtlas>>  StaticImageAtlas::Loader::LoadAsync (Serializing::Deserializer &des, ResourceCache &resCache, CachedResourceName::Ref selfName) __NE___
	{
		RC<StaticImageAtlas>				atlas;
		ResourceUploadManager::UploadResult	upload;

		if ( _Load( des, resCache, selfName, OUT atlas, OUT &upload ))
		{
			return MakePromiseFromValue( RVRef(atlas),
										 Tuple{RVRef(upload)},
										 "StaticImageAtlas.LoadAsync",
										 ETaskQueue::Background );
		}else
			return Default;
	}

/*
=================================================
	Loader::Load
=================================================
*/
	RC<StaticImageAtlas>  StaticImageAtlas::Loader::Load (RC<Threading::AsyncRDataSource> file, GfxMemAllocatorPtr alloc, ResourceUploadManager &uploadMngr) __NE___
	{
		CHECK_ERR( file );

		ImageAtlasPacker	unpacker;
		Bytes				file_offset;
		{
			auto	stream = MakeRC<BufferedRStream>( MakeRC<Threading::SyncRStreamOnAsyncDS>( file ));
			{
				Serializing::Deserializer	des {stream};
				CHECK_ERR( ImageAtlasPacker_Deserialize( OUT unpacker, des ));
				CHECK_ERR( AllBits( unpacker._header.flags, ImageAtlasPacker::EFileFlags::HasImage ));
			}
			file_offset = stream->Position();
		}

		auto&	img_header	= *unpacker.ImageHeader();
		auto	atlas		= MakeRC<StaticImageAtlas>();
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();

		GAutorelease<ImageID>	image_id = res_mngr.CreateImage( img_header.ToDesc().SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), Default, RVRef(alloc) );
		CHECK_ERR( image_id );

		atlas->_viewId = res_mngr.CreateImageView( ImageViewDesc{img_header.viewType}, image_id );
		CHECK_ERR( atlas->_viewId );

		auto	upload = uploadMngr.CreateTask();
		CHECK_ERR( upload );

		atlas->_SetLoadingStatus( ELoadingStatus::Created );

		CHECK_ERR( uploadMngr.EnqueueImage( upload, image_id, RVRef(file), file_offset,
											EUploadFlags::UsedWhileUploading, Default, EResourceState::FragmentShader | EResourceState::ShaderSample ));

		atlas->_SetLoadingStatus( ELoadingStatus::Uploading );

		atlas->_invImgSize	 = 1.0f / float2{img_header.dimension};
		atlas->_nameToIdx	 = RVRef(unpacker.map);
		atlas->_imageRects	 = RVRef(unpacker.rects);

		Scheduler().Run<OnUploadCompleteTask>( Tuple{atlas, upload}, Tuple{ResourceUploadManager::WeakUploadResult{upload}} );
		return atlas;
	}

	RC<StaticImageAtlas>  StaticImageAtlas::Loader::Load (VFS::FileName::Ref name, GfxMemAllocatorPtr alloc, ResourceUploadManager &uploadMngr) __NE___
	{
		RC<Threading::AsyncRDataSource>		file;
		CHECK_ERR( GetVFS().Open( OUT file, name ));
		return Loader::Load( RVRef(file), RVRef(alloc), uploadMngr );
	}


} // AE::Graphics
