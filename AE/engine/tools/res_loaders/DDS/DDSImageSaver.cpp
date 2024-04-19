// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/DDS/DDSImageSaver.h"
#include "res_loaders/DDS/DDSUtils.cpp.h"

namespace AE::ResLoader
{
namespace
{

/*
=================================================
	SaveDX10Image
=================================================
*/
	ND_ static bool  SaveDX10Image (WStream &stream, const IntermImage &image)
	{
		CHECK_ERR( image.GetData().size() );
		CHECK_ERR( image.GetData()[0].size() );

		const uint			arr_layers	= uint(image.GetData()[0].size());
		const uint			mipmap_count= uint(image.GetData().size());

		const auto&			level		= image.GetData()[0][0];
		const uint3			dim			= level.dimension;

		DDS_HEADER			header		= {};
		DDS_HEADER_DXT10	header10	= {};

		header.dwMagic			= MakeFourCC('D','D','S',' ');
		header.dwSize			= sizeof(header) - sizeof(header.dwMagic);
		header.dwFlags			= DDS_FLAGS(DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT);
		header.ddspf.dwSize		= sizeof(header.ddspf);
		header.ddspf.dwFourCC	= MakeFourCC('D','X','1','0');
		header.ddspf.dwFlags	= DDPF_FOURCC;
		header.dwWidth			= dim.x;
		header.dwHeight			= dim.y;
		header.dwMipMapCount	= mipmap_count;

		header10.dxgiFormat		= PixelFormatToDDSFormat( level.format );
		header10.arraySize		= 1;

		switch ( image.GetType() )
		{
			case EImage_1D :
			{
				header10.resourceDimension = D3D11_RESOURCE_DIMENSION_TEXTURE1D;
				break;
			}

			case EImage_2D :
			{
				header10.resourceDimension = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
				break;
			}

			case EImage_1DArray :
			{
				header10.resourceDimension	= D3D11_RESOURCE_DIMENSION_TEXTURE1D;
				header10.arraySize			= arr_layers;
				break;
			}

			case EImage_2DArray :
			{
				header10.resourceDimension	= D3D11_RESOURCE_DIMENSION_TEXTURE2D;
				header10.arraySize			= arr_layers;
				break;
			}

			case EImage_Cube :
			{
				header10.resourceDimension	= D3D11_RESOURCE_DIMENSION_TEXTURE2D;
				header10.miscFlag			= D3D11_RESOURCE_MISC_TEXTURECUBE;
				break;
			}

			case EImage_CubeArray :
			{
				header10.resourceDimension	= D3D11_RESOURCE_DIMENSION_TEXTURE2D;
				header10.arraySize			= arr_layers / 6;
				header10.miscFlag			= D3D11_RESOURCE_MISC_TEXTURECUBE;
				break;
			}

			case EImage_3D :
			{
				header10.resourceDimension = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
				header.dwFlags	= DDS_FLAGS(header.dwFlags | DDSD_DEPTH);
				header.dwDepth	= dim.z;
				break;
			}
		}

		CHECK_ERR( stream.Write( header ));
		CHECK_ERR( stream.Write( header10 ));

		for (uint layer = 0; layer < arr_layers; ++layer)
		{
			for (uint mm = 0; mm < mipmap_count; ++mm)
			{
				CHECK_ERR( layer < image.GetData()[mm].size() );

				auto&	lvl = image.GetData()[mm][layer];

				CHECK_ERR( stream.Write( lvl.Pixels() ));
			}
		}

		return true;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	SaveImage
=================================================
*/
	bool  DDSImageSaver::SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY) __NE___
	{
		CHECK( not flipY );

		if ( not (fileFormat == Default or fileFormat == EImageFormat::DDS) )
			return false;

		switch ( image.GetType() )
		{
			case EImage_1D :
			case EImage_2D :
			case EImage_1DArray :
			case EImage_2DArray :
			case EImage_Cube :
			case EImage_CubeArray :
			case EImage_3D :
				return SaveDX10Image( stream, image );
		}
		RETURN_ERR( "unsupported image type" );
	}

} // AE::ResLoader
