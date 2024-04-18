// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"
#include "graphics/Public/IDs.h"
#include "graphics/Public/ResourceEnums.h"
#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::AssetPacker
{
	using AE::Graphics::ImageInAtlasName;
	using AE::Graphics::EPixelFormat;


	//
	// Script Image Atlas
	//

	class ScriptImageAtlas final : public EnableScriptRC
	{
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


	// variables
	private:
		ImageMap_t			_map;
		UniqueImages_t		_uniqueImages;
		ImageFiles_t		_imageFiles;
		ImageRegionMap_t	_imageRegMap;
		ImageRegionArr_t	_imageRegions;

		int					_paddingPix		= 1;
		EPixelFormat		_dstFormat		= EPixelFormat::RGBA8_UNorm;
		EPixelFormat		_intermFormat	= EPixelFormat::RGBA8_UNorm;
		//bool				_premultipliedAlpha;	// TODO

		RC<ImageAtlasInfo>	_info;


	// methods
	public:
		ScriptImageAtlas ();
		~ScriptImageAtlas ();

		void  Add (const String &imageName, const String &filename)							__Th___;
		void  Add2 (const String &imageName, const String &filename, const RectU &region)	__Th___;

		void  Store (const String &nameInArchive)											__Th___;

		void  SetPadding (uint pix)															__Th___;
		void  SetFormat (EPixelFormat fmt)													__Th___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;

	private:
		ND_ bool  _Pack (const String &nameInArchive, RC<WStream> stream);
			void  _LoadImages ()															__Th___;
	};

	using ScriptImageAtlasPtr = ScriptRC< ScriptImageAtlas >;


} // AE::AssetPacker
