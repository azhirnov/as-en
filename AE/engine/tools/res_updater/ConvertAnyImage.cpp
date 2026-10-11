// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Common.h"
#include "res_loaders/Intermediate/IntermImage.h"
#include "res_loaders/AllImages/AllImageLoaders.h"
#include "res_loaders/AllImages/AllImageSavers.h"

#include "res_pack/asset_packer/Utils/GenMipmaps.h"

using namespace AE::ResLoader;

#ifdef AE_ENABLE_COMPRESSONATOR
# include "compressonator.h"

namespace {
#	include "res_pack/asset_packer/Utils/Compressonator.cpp.h"

/*
=================================================
	CompressBC_ETC2
=================================================
*/
	ND_ static bool  CompressBC_ETC2 (EPixelFormat dstFormat, IntermImage &srcImage, OUT IntermImage &dstImage)
	{
		CHECK_ERR( dstImage.Allocate( srcImage.GetType(), dstFormat, srcImage.Dimension(), ImageLayer{srcImage.ArrayLayers()}, MipmapLevel{srcImage.MipLevels()} ));

		Unused( Compressonator_GetBCLib() );

		const uint	thread_count	= Max( ThreadUtils::MaxThreadCount(), 3u ) - 2;
		const float	quality			= 1.f;

		auto&		src_img_data	= *srcImage.GetMutableData();
		auto&		dst_img_data	= *dstImage.GetMutableData();
		const usize	mip_count		= Min( src_img_data.size(), dst_img_data.size() );

		for (usize mip = 0; mip < mip_count; ++mip)
		{
			auto&	src_layers = src_img_data[mip];
			auto&	dst_layers = dst_img_data[mip];
			CHECK_ERR( src_layers.size() == dst_layers.size() );

			for (usize layer = 0; layer < src_layers.size(); ++layer)
			{
				AE_LOGI( "compress layer("s << ToString(layer) << ") mip(" << ToString(mip) << ")" );

				CHECK_ERR( Compressonator_Compress( srcImage.ToView( MipmapLevel{mip}, ImageLayer{layer} ),
													dstImage.ToView( MipmapLevel{mip}, ImageLayer{layer} ),
													thread_count, quality
												));
			}
		}
		return true;
	}

} // namespace
#endif // AE_ENABLE_COMPRESSONATOR
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_ASTC_ENCODER
# include "astcenc.h"

namespace {
#	include "res_pack/asset_packer/Utils/AstcEncoder.cpp.h"

/*
=================================================
	CompressASTC
=================================================
*/
	ND_ static bool  CompressASTC (EPixelFormat dstFormat, IntermImage &srcImage, OUT IntermImage &dstImage)
	{
		CHECK_ERR( dstImage.Allocate( srcImage.GetType(), dstFormat, srcImage.Dimension(), ImageLayer{srcImage.ArrayLayers()}, MipmapLevel{srcImage.MipLevels()} ));

		const uint	thread_count	= Max( ThreadUtils::MaxThreadCount(), 3u ) - 2;
		const float	quality			= 1.f;

		auto&		src_img_data	= *srcImage.GetMutableData();
		auto&		dst_img_data	= *dstImage.GetMutableData();
		const usize	mip_count		= Min( src_img_data.size(), dst_img_data.size() );

		for (usize mip = 0; mip < mip_count; ++mip)
		{
			auto&	src_layers = src_img_data[mip];
			auto&	dst_layers = dst_img_data[mip];
			CHECK_ERR( src_layers.size() == dst_layers.size() );

			for (usize layer = 0; layer < src_layers.size(); ++layer)
			{
				AE_LOGI( "compress layer("s << ToString(layer) << ") mip(" << ToString(mip) << ")" );

				CHECK_ERR( AstcEncode( srcImage.ToView( MipmapLevel{mip}, ImageLayer{layer} ),
									   dstImage.ToView( MipmapLevel{mip}, ImageLayer{layer} ),
									   thread_count, quality
									));
			}
		}

		Unused( &AstcDecode );
		return true;
	}

} // namespace
#endif // AE_ENABLE_ASTC_ENCODER
//-----------------------------------------------------------------------------


/*
=================================================
	ConvertAnyImage
=================================================
*/
extern bool  ConvertAnyImage (const Path &src, const Path &dst, const ConvertImageConfig &cfg)
{
	IntermImage   interm {src};

	// load
	{
		AE_LOGI( "load origin image '"s << ToString(src) << "'" );

		AllImageLoaders loader;
		CHECK_ERR( loader.LoadImage( OUT interm, {}, False{"don't flip Y"}, null, Default ));
	}

	// convert
	/*if ( cfg.intermFormat != Default and cfg.intermFormat != interm.PixelFormat() )
	{
		IntermImage     temp{ RVRef(interm) };

	}*/

	// mipmaps
	if ( cfg.genMips )
	{
		AE_LOGI( "gen mipmaps" );

		IntermImage     temp{ RVRef(interm) };
		CHECK_ERR( AssetPacker::MipMapGenerator::Gen( temp, OUT interm ));
	}

	// base mip
	if ( cfg.baseMip > 0 )
	{
		AE_LOGI( "remove high mips" );

		auto&	data = *interm.GetMutableData();

		CHECK_ERR( cfg.baseMip < data.size() );
		data.erase( data.begin(), data.begin() + cfg.baseMip );
	}

	// compress
	if ( cfg.compFormat != Default								and
		 not EPixelFormat_IsCompressed( interm.PixelFormat() ))
	{
		#ifdef AE_ENABLE_COMPRESSONATOR
		if ( EPixelFormat_IsETC( cfg.compFormat ) or EPixelFormat_IsBC( cfg.compFormat ))
		{
			AE_LOGI( "compress" );

			IntermImage     temp{ RVRef(interm) };
			CHECK_ERR( CompressBC_ETC2( cfg.compFormat, temp, OUT interm ));
		}
		#endif

		#ifdef AE_ENABLE_ASTC_ENCODER
		if ( EPixelFormat_IsASTC( cfg.compFormat ))
		{
			AE_LOGI( "compress" );

			IntermImage     temp{ RVRef(interm) };
			CHECK_ERR( CompressASTC( cfg.compFormat, temp, OUT interm ));
		}
		#endif
	}

	// save
	{
		AE_LOGI( "save" );

		AllImageSavers saver;
		CHECK_ERR( saver.SaveImage( dst, interm ));
	}
	return true;
}

/*
=================================================
	ConvertImagesInFolder
=================================================
*/
extern bool  ConvertImagesInFolder (const Path &srcFolder, const Path &dstFolder, StringView dstExt, const ConvertImageConfig &cfg)
{
	CHECK_ERR( FileSystem::IsDirectory( srcFolder ));

	FileSystem::CreateDirectories( dstFolder );

	for (auto entry : FileSystem::EnumRecursive( srcFolder ))
	{
		if ( entry.IsFile() )
			CHECK( ConvertAnyImage( entry.Get(), dstFolder / Path{entry.Get().filename()}.replace_extension(dstExt), cfg ));
	}
	return true;
}

#include "res_pack/asset_packer/Utils/GenMipmaps.cpp"
