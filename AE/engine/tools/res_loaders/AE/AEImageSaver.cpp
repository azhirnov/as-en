// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/AE/AEImageSaver.h"
#include "res_loaders/Intermediate/IntermImage.h"
#include "res_pack/asset_packer/Packer/ImagePacker.h"

namespace AE::ResLoader
{
namespace {
#	define AE_BUILD_ASSET_PACKER
#	include "res_pack/asset_packer/Packer/ImagePacker.cpp.h"
#	undef AE_BUILD_ASSET_PACKER
}

/*
=================================================
	SaveImage
=================================================
*/
	bool  AEImageSaver::SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY) __NE___
	{
		CHECK( not flipY );

		if ( not (fileFormat == Default or fileFormat == EImageFormat::AEImg) )
			return false;

		CHECK_ERR( image.GetData().size() );
		CHECK_ERR( image.GetData()[0].size() );

		AssetPacker::ImagePacker::Header	header;
		header.dimension	= packed_ushort3{image.GetData()[0][0].dimension};
		header.arrayLayers	= ushort(image.GetData()[0].size());
		header.mipmaps		= ushort(image.GetData().size());
		header.viewType		= image.GetType();
		header.format		= image.GetData()[0][0].format;

		CHECK_ERR( ImagePacker_SaveHeader( stream, header ));
		CHECK_ERR( ImagePacker_SaveImage( stream, header, image ));
		return true;
	}


} // AE::ResLoader

