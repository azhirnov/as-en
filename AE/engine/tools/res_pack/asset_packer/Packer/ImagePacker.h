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


		struct Header2
		{
			uint			magic		= Magic;
			ushort			version		= Version;
			Header			hdr;

			Header2 ()							__NE___ = default;
			explicit Header2 (const Header &h)	__NE___ : hdr{h} {};
		};
		StaticAssert( sizeof(Header2) == 24 );


	// variables
	private:
		Header2		_header;


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
	template <> struct TTriviallySerializable< AE::AssetPacker::ImagePacker::Header >	{ static constexpr bool  value = true; };
	template <> struct TTriviallySerializable< AE::AssetPacker::ImagePacker::Header2 >	{ static constexpr bool  value = true; };
}
