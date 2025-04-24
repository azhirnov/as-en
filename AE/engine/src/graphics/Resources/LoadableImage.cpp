// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Resources/LoadableImage.h"
#include "graphics/Resources/ResourceUploadManager.h"

namespace AE::Graphics
{
	using namespace AE::AssetPacker;

namespace {
#	include "Packer/ImagePacker.cpp.h"
}

/*
=================================================
	destructor
=================================================
*/
	LoadableImage::~LoadableImage () __NE___
	{
		if ( _imageId )
			GraphicsScheduler().GetResourceManager().ReleaseResources( _imageId, _viewId );
	}

/*
=================================================
	GetWhenUploadComplete
=================================================
*/
	Promise<RC<LoadableImage>>  LoadableImage::GetWhenUploadComplete () __NE___
	{
		return MakePromiseFromValue( GetRC<LoadableImage>(),
									 Tuple{OnUploadComplete()},
									 "GetWhenUploadComplete",
									 ETaskQueue::Background );
	}

/*
=================================================
	CloneImageView / GetImageDesc / GetViewDesc
=================================================
*/
	Strong<ImageViewID>  LoadableImage::CloneImageView () C_NE___
	{
		ASSERT( _viewId );
		return GraphicsScheduler().GetResourceManager().AcquireResource( _viewId.Get() );
	}

	ImageDesc  LoadableImage::GetImageDesc () C_NE___
	{
		ASSERT( _imageId );
		return GraphicsScheduler().GetResourceManager().GetDescription( _imageId );
	}

	ImageViewDesc  LoadableImage::GetViewDesc () C_NE___
	{
		ASSERT( _viewId );
		return GraphicsScheduler().GetResourceManager().GetDescription( _viewId );
	}

/*
=================================================
	Loader::OnUploadCompleteTask
=================================================
*/
	class LoadableImage::Loader::OnUploadCompleteTask final : public Threading::IAsyncTask
	{
	private:
		RC<LoadableImage>	_image;

	public:
		OnUploadCompleteTask (RC<LoadableImage> img) __NE___ : IAsyncTask{ ETaskQueue::Background }, _image{RVRef(img)} {}

		void  Run () __Th_OV
		{
			auto	upload	= _image->_uploadResult.release();
			bool	ok		= upload ? upload->IsCompleted() : true;

			_image->_SetLoadingStatus( ok ? ELoadingStatus::Complete : ELoadingStatus::Failed );
			_image = null;
		}

		DEBUG_ONLY( void  OnCancel ()	__NE_OV { DBG_WARNING("should never happens"); })

		StringView  DbgName ()			C_NE_OV { return "on image loading complete"; }
	};

/*
=================================================
	Loader::Load
=================================================
*/
	RC<LoadableImage>  LoadableImage::Loader::Load (Serializing::Deserializer &des, GfxMemAllocatorPtr alloc, ResourceCache &resCache,
													ResourceUploadManager &uploadMngr, CachedResourceName::Ref selfName) __NE___
	{
		using namespace AE::Threading;

		ImagePacker::FileHeader		file_hdr;
		CHECK_ERR( ImagePacker_Deserialize( des, OUT file_hdr ));
		CHECK_ERR( file_hdr.fileName.IsDefined() );

		auto&	img_header	= file_hdr.imageHeader;
		auto	image		= MakeRC<LoadableImage>();
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();

		image->_imageId = res_mngr.CreateImage( img_header.ToDesc().SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), Default, RVRef(alloc) );
		CHECK_ERR( image->_imageId );

		image->_viewId = res_mngr.CreateImageView( ImageViewDesc{img_header.viewType}, image->_imageId );
		CHECK_ERR( image->_viewId );

		auto	upload = uploadMngr.CreateTask();
		CHECK_ERR( upload );

		image->_uploadResult.store( upload );
		image->_SetLoadingStatus( ELoadingStatus::Created );

		if ( selfName.IsDefined() )
		{
			if ( auto cached = resCache.InsertResource( selfName, image ))
				return cached;
		}

		RC<AsyncRDataSource>	file;
		CHECK_ERR( GetVFS().Open( OUT file, VFS::FileName{file_hdr.fileName} ));

		CHECK_ERR( uploadMngr.EnqueueImage( upload, image->_imageId, RVRef(file), 0_b,
											EUploadFlags::UsedWhileUploading, Default, EResourceState::FragmentShader | EResourceState::ShaderSample ));

		image->_SetLoadingStatus( ELoadingStatus::Uploading );

		Scheduler().Run<OnUploadCompleteTask>( Tuple{image}, Tuple{ResourceUploadManager::WeakUploadResult{upload}} );

		return image;
	}

/*
=================================================
	Loader::LoadAsync
=================================================
*/
	Promise<RC<LoadableImage>>  LoadableImage::Loader::LoadAsync (Serializing::Deserializer &des, GfxMemAllocatorPtr alloc,
																  ResourceCache &resCache, ResourceUploadManager &uploadMngr,
																  CachedResourceName::Ref selfName) __NE___
	{
		auto	image = Loader::Load( des, RVRef(alloc), resCache, uploadMngr, selfName );
		if ( image )
			return image->GetWhenUploadComplete();
		else
			return Default;
	}

/*
=================================================
	Loader::Load
=================================================
*/
	RC<LoadableImage>  LoadableImage::Loader::Load (RC<Threading::AsyncRDataSource> file, GfxMemAllocatorPtr alloc, ResourceUploadManager &uploadMngr) __NE___
	{
		CHECK_ERR( file );

		ImagePacker::FileHeader		file_hdr;
		{
			Threading::SyncRStreamOnAsyncDS		stream {file};
			CHECK_ERR( ImagePacker_ReadHeader( stream, OUT file_hdr ));
			CHECK_ERR( not file_hdr.fileName.IsDefined() );
		}

		auto&	img_header	= file_hdr.imageHeader;
		auto	image		= MakeRC<LoadableImage>();
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();

		image->_imageId = res_mngr.CreateImage( img_header.ToDesc().SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), Default, RVRef(alloc) );
		CHECK_ERR( image->_imageId );

		image->_viewId = res_mngr.CreateImageView( ImageViewDesc{img_header.viewType}, image->_imageId );
		CHECK_ERR( image->_viewId );

		auto	upload = uploadMngr.CreateTask();
		CHECK_ERR( upload );

		image->_uploadResult.store( upload );
		image->_SetLoadingStatus( ELoadingStatus::Created );

		CHECK_ERR( uploadMngr.EnqueueImage( upload, image->_imageId, RVRef(file), Sizeof(file_hdr),
											EUploadFlags::UsedWhileUploading, Default, EResourceState::FragmentShader | EResourceState::ShaderSample ));

		image->_SetLoadingStatus( ELoadingStatus::Uploading );

		Scheduler().Run<OnUploadCompleteTask>( Tuple{image}, Tuple{ResourceUploadManager::WeakUploadResult{upload}} );

		return image;
	}

	RC<LoadableImage>  LoadableImage::Loader::Load (VFS::FileName::Ref name, GfxMemAllocatorPtr alloc, ResourceUploadManager &uploadMngr) __NE___
	{
		RC<Threading::AsyncRDataSource>		file;
		CHECK_ERR( GetVFS().Open( OUT file, name ));
		return Loader::Load( RVRef(file), RVRef(alloc), uploadMngr );
	}


} // AE::Graphics
