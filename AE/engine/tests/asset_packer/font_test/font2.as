// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>

void ASmain ()
{
	RC<RasterFont>	font = RasterFont();

	font.Load( "Roboto-Regular.ttf" );
	font.AddCharset_Ascii();
	font.AddCharset_Rus();

	font.RasterMode( ERasterFontMode::SDF );
	font.Format( EPixelFormat::R8_UNorm );

	font.GlyphSize( 32 );		// pix
	font.SDFGlyphBorder( 1 );	// pix,	increase for bold style
	font.GlyphPadding( 4 );		// pix

	font.Store( "sdf-font" );
}
