// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_pack/asset_packer/Packer/ImagePacker.h"

namespace AE::AssetPacker
{

	//
	// Image Atlas Packer
	//

	class ImageAtlasPacker
	{
	// types
	public:
		using Rect_t		= Rectangle<ushort>;
		using ImageMap_t	= FlatHashMap< Graphics::ImageInAtlasName::Optimized_t, uint >;	// index in 'ImageRects_t'
		using ImageRects_t	= Array< Rect_t >;

		static constexpr ushort		Version		= 2;
		static constexpr uint		Magic		= "gr.StAtlas"_Hash;
		static constexpr auto		SerID		= Serializing::SerializedID::Optimized_t{"StaticImageAtlas"};

		enum class EFileFlags : ushort
		{
			Unknown			= 0,
			HasImage		= 1 << 0,	// image data in current file
			SeparateData	= 1 << 1,	// load image data from another file
			HasResName		= 1 << 2,	// get image from cache
			_BITOPS_
		};

		struct FileHeader
		{
			uint			magic		= Magic;
			ushort			version		= Version;
			EFileFlags		flags		= Default;
		};
		StaticAssert( sizeof(FileHeader) == 8 );


	// variables
	public:
		FileHeader						_header;
		ImagePacker::Header				_imageHeader;		// HasImage or SeparateData
		CachedResourceName::Optimized_t	_imageResName;		// HasResName
		VFS::FileName::Optimized_t		_imageFileName;		// SeparateData

		ImageMap_t						map;
		ImageRects_t					rects;


	// methods
	public:
		ND_ auto*	ImageHeader ()			C_NE___;
		ND_ auto	ImageResourceName ()	C_NE___	{ return _imageResName; }
		ND_ auto	ImageFileName ()		C_NE___	{ return _imageFileName; }
	};


	Nd__In auto*  ImageAtlasPacker::ImageHeader () C_NE___
	{
		return AnyBits( _header.flags, EFileFlags::HasImage | EFileFlags::SeparateData ) ? &_imageHeader : null;
	}

} // AE::AssetPacker

namespace AE::Base
{
	template <> struct TTriviallySerializable< AE::AssetPacker::ImageAtlasPacker::FileHeader >	: CT_True {};
}
