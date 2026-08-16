// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <asset_packer.as>

void AddRasterFont (RC<MetaData> meta, RC<SharedImage> sharedImage)
{
	RC<RasterFont>	font = RasterFont();

	font.Load( "fonts/Roboto-Regular.ttf" );
	font.AddCharset_Ascii();
	font.AddCharset_Rus();

	font.RasterMode( ERasterFontMode::Raster );
	font.Format( EPixelFormat::R8_UNorm );

	font.GlyphSize( 60 );	// pix
	font.GlyphPadding( 2 );	// pix

	font.PutMeta( meta, "ui-font" );
	font.PutData( sharedImage );
}
