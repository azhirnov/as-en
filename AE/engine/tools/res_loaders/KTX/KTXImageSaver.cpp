// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_KTX
# include "KTXUtils.cpp.h"
# include "res_loaders/KTX/KTXImageSaver.h"

namespace AE::ResLoader
{
namespace
{
	using namespace AE::Graphics;

/*
=================================================
	CompressAstc
=================================================
*
	ND_ static bool  CompressAstc (ktxTexture2* ktx_tex)
	{
		ktxAstcParams	params = {};
		params.structSize		= sizeof(params);
		params.verbose			= true;
		params.threadCount		= 4;										// TODO
		params.blockDimension	= KTX_PACK_ASTC_BLOCK_DIMENSION_4x4;		// TODO
		params.mode				= KTX_PACK_ASTC_ENCODER_MODE_LDR;			// TODO
		params.qualityLevel		= KTX_PACK_ASTC_QUALITY_LEVEL_EXHAUSTIVE;	// TODO
		params.normalMap		= false;									// TODO
		params.perceptual		= false;									// TODO

		CHECK_ERR( ktxTexture2_CompressAstcEx( ktx_tex, &params ) == KTX_SUCCESS );
		return true;
	}

/*
=================================================
	CompressBasis
=================================================
*
	ND_ static bool  CompressBasis (ktxTexture2* ktx_tex)
	{
		ktxBasisParams	params = {};
		// TODO

		CHECK_ERR( ktxTexture2_CompressBasisEx( ktx_tex, &params ) == KTX_SUCCESS );

		// ktxTexture2_TranscodeBasis ?
		return true;
	}
*/

} // namespace


/*
=================================================
	SaveImage
=================================================
*/
	bool  KTXImageSaver::SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY) __NE___
	{
		CHECK( not flipY );

		if ( not (fileFormat == Default or fileFormat == EImageFormat::KTX) )
			return false;

		Unused( &CreateKtxRStream, &VkFormatToEPixelFormat, &GLFormatToEPixelFormat );

		Unique< ktxTexture2, Function< void (ktxTexture2 *) >>		ktx_tex;
		{
			const EPixelFormat		format	= image.PixelFormat();
			const uint3				dim		= image.Dimension();
			ktxTextureCreateInfo	ci		= {};

			ci.glInternalformat	= EPixelFormatToGLFormat( format );
			ci.vkFormat			= EPixelFormatToVkFormat( format );
			ci.baseWidth		= dim.x;
			ci.baseHeight		= dim.y;
			ci.baseDepth		= dim.z;
			ci.numLevels		= image.MipLevels();
			ci.numLayers		= image.ArrayLayers();
			ci.numFaces			= 1;
			ci.generateMipmaps	= false;

			switch_enum( image.GetType() )
			{
				case EImage_1D :		ci.numDimensions = 1;	ci.isArray = false;		break;
				case EImage_2D :		ci.numDimensions = 2;	ci.isArray = false;		break;
				case EImage_3D :		ci.numDimensions = 3;	ci.isArray = false;		break;
				case EImage_1DArray :	ci.numDimensions = 1;	ci.isArray = true;		break;
				case EImage_2DArray :	ci.numDimensions = 2;	ci.isArray = true;		break;
				case EImage_Cube :		ci.numDimensions = 2;	ci.isArray = false;		ci.numFaces = 6;	break;
				case EImage_CubeArray :	ci.numDimensions = 2;	ci.isArray = true;		ci.numFaces = 6;	break;
				case EImage::Unknown :
				case EImage::_Count :
				default :				RETURN_ERR( "unsupported image type" );
			}
			switch_end

			ktxTexture2*	temp_tex2 = null;
			CHECK_ERR( ktxTexture2_Create( &ci, KTX_TEXTURE_CREATE_ALLOC_STORAGE, OUT &temp_tex2 ) == KTX_SUCCESS );

			ktx_tex = { temp_tex2, [](ktxTexture2 *tex){ ktxTexture_Destroy(ktxTexture(tex)); }};
		}

		// SetPrimaries()

		const uint	array_layers	= image.ArrayLayers();
		const uint	mipmaps			= image.MipLevels();

		for (uint layer = 0; layer < array_layers; ++layer)
		{
			for (uint mm = 0; mm < mipmaps; ++mm)
			{
				auto*	level = image.GetLevel( MipmapLevel{mm}, ImageLayer{layer} );
				CHECK_ERR( level != null );

				CHECK_ERR( ktxTexture_SetImageFromMemory( ktxTexture(ktx_tex.get()), mm, layer, 0,
														  Cast<ubyte>(level->PixelData()), usize{level->DataSize()} ) == KTX_SUCCESS );
			}
		}

		ktxStream	ktx_stream = CreateKtxWStream( stream );

		CHECK_ERR( ktxTexture_WriteToStream( ktxTexture(ktx_tex.get()), &ktx_stream ) == KTX_SUCCESS );
		return true;
	}


} // AE::ResLoader

#endif // AE_ENABLE_KTX
