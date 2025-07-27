// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Resources/ResourceCache.h"
#include "graphics/Resources/ResourceUploadManager.h"

namespace AE::Graphics
{

	//
	// Loadable Image
	//

	class LoadableImage final : public CachedResource
	{
	// types
	public:
		// should be used in background thread
		struct Loader {
			// single file
			ND_ static RC<LoadableImage>  Load (VFS::FileName::Ref, GfxMemAllocatorPtr alloc, ResourceUploadManager &) __NE___;
			ND_ static RC<LoadableImage>  Load (RC<Threading::AsyncRDataSource> ds, GfxMemAllocatorPtr alloc, ResourceUploadManager &) __NE___;

			// meta data + image data
			ND_ static RC<LoadableImage>  Load (Serializing::Deserializer &des, GfxMemAllocatorPtr alloc,
												ResourceCache &, ResourceUploadManager &, CachedResourceName::Ref selfName = Default) __NE___;

			// meta data + image data, ready when completely upload to GPU
			ND_ static Promise<RC<LoadableImage>>  LoadAsync (Serializing::Deserializer &des, GfxMemAllocatorPtr alloc,
															  ResourceCache &, ResourceUploadManager &, CachedResourceName::Ref selfName = Default) __NE___;
		private:
			ND_ static AsyncCoro  _OnUploadComplete (RC<LoadableImage>) __NE___;
		};

		using UploadResult = ResourceUploadManager::AtomicUploadResult;


	// variables
	private:
		Strong<ImageID>		_imageId;
		Strong<ImageViewID>	_viewId;

		UploadResult		_uploadResult;


	// methods
	public:
		LoadableImage ()												__NE___	{}
		~LoadableImage ()												__NE___;

		ND_ Strong<ImageViewID>		CloneImageView ()					C_NE___;

		ND_ ImageDesc				GetImageDesc ()						C_NE___;
		ND_ ImageViewDesc			GetViewDesc ()						C_NE___;

		ND_ ImageID					ImageId ()							C_NE___	{ return _imageId; }
		ND_ ImageViewID				ViewId ()							C_NE___	{ return _viewId; }

		ND_ auto					OnUploadComplete ()					__NE___	{ return _uploadResult.load(); }
		ND_ auto					OnCompleteOrFailed ()				__NE___	{ return ResourceUploadManager::WeakUploadResult{OnUploadComplete()}; }

		ND_ Promise<RC<LoadableImage>>  GetWhenUploadComplete ()		__NE___;
	};


} // AE::Graphics
