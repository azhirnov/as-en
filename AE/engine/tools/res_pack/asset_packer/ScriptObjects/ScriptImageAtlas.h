// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_pack/asset_packer/ScriptObjects/ScriptSharedImage.h"
#include "res_pack/asset_packer/Packer/ImagePacker.h"

namespace AE::AssetPacker
{
	using AE::Graphics::ImageInAtlasName;


	//
	// Script Image Atlas
	//

	class ScriptImageAtlas final : public EnableScriptRC
	{
		friend class ScriptSharedImage;

	// types
	private:
		struct ImageRegion
		{
			RectU	region;
			uint	imageIdx	= UMax;		// index in '_imageFiles'

			ND_ bool  operator == (const ImageRegion &rhs) const {
				return All( region == rhs.region ) and imageIdx == rhs.imageIdx;
			}
		};

		struct ImageRegionHash {
			ND_ usize  operator () (const ImageRegion &x) const {
				return usize(HashOf(x.region) + HashOf(x.imageIdx));
			}
		};

		struct ImageInfo
		{
			Path							path;
			Unique<ResLoader::IntermImage>	data;

			ImageInfo () {}
			explicit ImageInfo (Path path) : path{RVRef(path)} {}
		};

		using ImageRegionMap_t	= FlatHashMap< ImageRegion, uint, ImageRegionHash >;	// index in '_imageRegions'
		using ImageMap_t		= FlatHashMap< String, uint >;							// index in '_imageRegions'
		using UniqueImages_t	= FlatHashMap< Path, uint, PathHasher >;				// index in '_imageFiles'
		using ImageFiles_t		= Array< ImageInfo >;
		using ImageRegionArr_t	= Array< ImageRegion >;
		using ImageAtlasInfo	= ObjectStorage::ImageAtlasInfo;

		enum class EState : uint
		{
			Recording,
			Immutable,
			Arranged,
			StoreData,
			Stored,
		};


	// variables
	private:
		ImageMap_t				_map;
		UniqueImages_t			_uniqueImages;
		ImageFiles_t			_imageFiles;
		ImageRegionMap_t		_imageRegMap;
		ImageRegionArr_t		_imageRegions;

		int						_paddingPix		= 1;
		EPixelFormat			_dstFormat		= EPixelFormat::RGBA8_UNorm;
		EPixelFormat			_intermFormat	= EPixelFormat::RGBA8_UNorm;
		//bool					_premultipliedAlpha;	// TODO

		RC<ImageAtlasInfo>		_info;
		String					_sharedImageMeta;
		String					_imageFileName;
		ImagePacker::Header		_imageHeader;

		mutable EState			_state			= EState::Recording;


	// methods
	public:
		ScriptImageAtlas ();
		~ScriptImageAtlas ();

		void  Add (const String &imageName, const String &filename)							__Th___;
		void  Add2 (const String &imageName, const String &filename, const RectU &region)	__Th___;

		void  Store (const String &nameInArchive)											__Th___;
		void  StoreData (const String &nameInArchive)										__Th___;

		void  PutMeta (const ScriptResourceMetaPtr &, const String &name)					__Th___;
		void  PutData (const ScriptSharedImagePtr &image)									__Th___;

		void  SetPadding (uint pix)															__Th___;
		void  SetFormat (EPixelFormat fmt)													__Th___;

		// used by 'ScriptResourceMeta'
		ND_ bool  _StoreMeta (RC<WStream>, const String &metaArchive = Default)				C_NE___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;

	private:
		ND_ bool  _ToTexture (OUT ScriptTexture &, const String &name)						__NE___;
			void  _LoadImages ()															__Th___;

		ND_ bool  _CopyPixels (INOUT ResLoader::IntermImage &, ArrayView<ScriptSharedImage::Result>) __NE___;
	};


} // AE::AssetPacker
