// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/Resources/ResourceCache.h"
#include "graphics_hl/Resources/ResourceUploadManager.h"

#include "AssetPackerImpl.h"

namespace AE::Graphics
{
	class ITransferContext;
	class ResourceUploadManager;


	//
	// Static Image Atlas
	//

	class StaticImageAtlas final : public CachedResource
	{
	// types
	public:
		using RectUNorm		= Rectangle<ushort>;
		using Rect_t		= AssetPacker::ImageAtlasPacker::Rect_t;
		using ImageMap_t	= AssetPacker::ImageAtlasPacker::ImageMap_t;
		using ImageRects_t	= AssetPacker::ImageAtlasPacker::ImageRects_t;

		// should be used in background thread
		struct Loader {
			// single file
			ND_ static RC<StaticImageAtlas>  Load (VFS::FileName::Ref, GfxMemAllocatorPtr alloc, ResourceUploadManager &) __NE___;
			ND_ static RC<StaticImageAtlas>  Load (RC<Threading::AsyncRDataSource> ds, GfxMemAllocatorPtr alloc, ResourceUploadManager &) __NE___;

			// meta data + image data
			ND_ static RC<StaticImageAtlas>  Load (Serializing::Deserializer &des, ResourceCache &, CachedResourceName::Ref selfName = Default) __NE___;

			// meta data + image data, ready when completely upload to GPU
			ND_ static Promise<RC<StaticImageAtlas>>  LoadAsync (Serializing::Deserializer &des, ResourceCache &, CachedResourceName::Ref selfName = Default) __NE___;

		private:
			class OnUploadCompleteTask;
			ND_ static bool  _Load (Serializing::Deserializer &des, ResourceCache&, CachedResourceName::Ref selfName,
									OUT RC<StaticImageAtlas> &, OUT ResourceUploadManager::UploadResult *) __NE___;
		};


	// variables
	private:
		ImageID					_imageId;
		Strong<ImageViewID>		_viewId;

		float2					_invImgSize		{1};
		ImageMap_t				_nameToIdx;
		ImageRects_t			_imageRects;


	// methods
	public:
		StaticImageAtlas ()													__NE___	{}
		~StaticImageAtlas ()												__NE___;

			bool  Get (ImageInAtlasName::Ref name, OUT RectI &region)		C_NE___;
			bool  Get (ImageInAtlasName::Ref name, OUT RectF &region)		C_NE___;
			bool  Get (ImageInAtlasName::Ref name, OUT RectUNorm &region)	C_NE___;

		ND_ ImageDesc		GetImageDesc ()									C_NE___;
		ND_ ImageViewDesc	GetViewDesc ()									C_NE___;

		ND_ ImageID			ImageId ()										C_NE___	{ return _imageId; }
		ND_ ImageViewID		ViewId ()										C_NE___	{ return _viewId; }
	};


} // AE::Graphics
