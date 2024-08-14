// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Format:
		header
		image data:
			mipmap0
				slice0 / layer0 / face0
				slice1 / layer1 / face1
				...
			mipmap1
			...
*/

#pragma once

#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/ImageMemView.h"

namespace AE::ResLoader {
	class IntermImage;
}
namespace AE::AssetPacker
{
	using namespace AE::Base;
	using namespace AE::Graphics;


	//
	// Image Packer
	//

	class ImagePacker final
	{
	// types
	public:
		static constexpr ushort		Version			= 1;
		static constexpr uint		Magic			= "gr.Image"_Hash;

		struct Header
		{
			packed_ushort3	dimension;
			ushort			arrayLayers	= 0;
			ushort			mipmaps		= 0;
			EImage			viewType	= Default;
			EPixelFormat	format		= Default;
			ushort			flags		= 0;		// 0
			ubyte			rowAlignPOT	= 0;		// POTValue

			// TODO: viewFormats, usage, options

			Header ()							__NE___ = default;

			ND_ ImageDesc		ToDesc ()		C_NE___;
			ND_ ImageViewDesc	ToViewDesc ()	C_NE___;
		};
		StaticAssert( sizeof(Header) == 16 );
		StaticAssert( alignof(Header) == 2 );


		struct FileHeader
		{
			uint			magic		= Magic;
			ushort			version		= Version;
			Header			hdr;

			FileHeader ()							__NE___ = default;
			explicit FileHeader (const Header &h)	__NE___ : hdr{h} {};
		};
		StaticAssert( sizeof(FileHeader) == 24 );


	// variables
	private:
		FileHeader		_header;


	// methods
	public:
		ImagePacker ()								__NE___ {}
		explicit ImagePacker (const Header &h)		__NE___ : _header{h} {}
	};


/*
=================================================
	Header::ToDesc
=================================================
*/
	inline ImageDesc  ImagePacker::Header::ToDesc () C_NE___
	{
		EImageOpt	options = Default;
		switch ( viewType ) {
			case EImage::Cube :
			case EImage::CubeArray :	options |= EImageOpt::CubeCompatible;	break;
		}
		return ImageDesc{}
			.SetDimension( uint3{dimension} )
			.SetArrayLayers( arrayLayers )
			.SetMaxMipmaps( mipmaps )
			.SetType( viewType )
			.SetFormat( format )
			.SetOptions( options );
	}

/*
=================================================
	Header::ToViewDesc
=================================================
*/
	inline ImageViewDesc  ImagePacker::Header::ToViewDesc () C_NE___
	{
		return ImageViewDesc{ viewType };
	}


} // AE::AssetPacker

namespace AE::Base
{
	template <> struct TTriviallySerializable< AE::AssetPacker::ImagePacker::Header >		: CT_True {};
	template <> struct TTriviallySerializable< AE::AssetPacker::ImagePacker::FileHeader >	: CT_True {};
}
