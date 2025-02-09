// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>

void AddMSdfFont (RC<MetaData> meta, RC<SharedImage> sharedImage)
{
	RC<RasterFont>	font = RasterFont();

	font.Load( "fonts/Roboto-Regular.ttf" );
	font.AddCharset_Ascii();
	font.AddCharset_Rus();

	font.RasterMode( ERasterFontMode::MC_SDF );
	font.Format( EPixelFormat::RGBA8_UNorm );

	font.GlyphSize( 32 );		// pix
	font.SDFGlyphBorder( 1 );	// pix,	increase for bold style
	font.GlyphPadding( 4 );		// pix

//	font.GlyphSize( 64 );		// pix
//	font.SDFGlyphBorder( 8 );	// pix,	increase for bold style
//	font.GlyphPadding( 2 );		// pix

	font.PutMeta( meta, "ui-msdf-font" );
	font.PutData( sharedImage );
}
