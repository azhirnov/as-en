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

#include "graphics_rhi/Public/ResourceEnums.h"
#include "graphics_rhi/Public/ImageDesc.h"
#include "graphics_rhi/Public/ImageMemView.h"
#include "vfs/Common.h"

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
		static constexpr ushort		Version		= 2;
		static constexpr uint		Magic		= "gr.Image"_Hash;
		static constexpr auto		SerID		= Serializing::SerializedID::Optimized_t{"Image"};

		struct Header
		{
			ImageDim_t		dimension;
			LayerCount_t	arrayLayers	= 0;
			MipmapCount_t	mipmaps		= 0;
			EImage			viewType	= Default;
			EPixelFormat	format		= Default;
			ushort			flags		= 0;		// 0
			POTBytes		rowAlignPOT;

			// TODO: viewFormats, usage, options

			Header ()									__NE___ = default;
			explicit Header (const ImageDesc &desc,
							 EImage viewType = Default)	__NE___;

			ND_ ImageDesc		ToDesc ()				C_NE___;
			ND_ ImageViewDesc	ToViewDesc ()			C_NE___;
		};
		StaticAssert( sizeof(Header) == 16 );
		StaticAssert( alignof(Header) == 2 );


		struct FileHeader
		{
			uint							magic			= Magic;
			ushort							version			= Version;
			Header							imageHeader;
			// padding 2 bytes
			VFS::FileName::Optimized_t		fileName;

			FileHeader ()							__NE___ = default;
			explicit FileHeader (const Header &h)	__NE___ : imageHeader{h} {};
		};
		StaticAssert( sizeof(FileHeader) == 28 );


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

/*
=================================================
	Header ctor
=================================================
*/
	inline ImagePacker::Header::Header (const ImageDesc &desc, EImage viewType) __NE___ :
		dimension{ desc.dimension },
		arrayLayers{ LayerCount_t{desc.arrayLayers} },
		mipmaps{ MipmapCount_t{desc.mipLevels} },
		viewType{ viewType },
		format{ desc.format }
	{}


} // AE::AssetPacker

namespace AE::Base
{
	template <> struct TTriviallySerializable< AE::AssetPacker::ImagePacker::Header >		: CT_True {};
	template <> struct TTriviallySerializable< AE::AssetPacker::ImagePacker::FileHeader >	: CT_True {};
}
