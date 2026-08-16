// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <asset_packer.as>

void ASmain ()
{
	RC<SharedImage>	image	= SharedImage();
	RC<MetaData>	meta	= MetaData();

	image.PutMeta( meta, "font-image" );

	{
		RC<RasterFont>	font = RasterFont();

		font.Load( "Roboto-Regular.ttf" );
		font.AddCharset_Ascii();
		font.AddCharset_Rus();

		font.RasterMode( ERasterFontMode::Raster );
		font.Format( EPixelFormat::R8_UNorm );

		font.GlyphSize( 60 );	// pix
		font.GlyphPadding( 2 );	// pix

		font.PutMeta( meta, "raster-font" );
		font.PutData( image );
	}

	image.Store( "font_image" );
	meta.Store( "font_meta" );
}
