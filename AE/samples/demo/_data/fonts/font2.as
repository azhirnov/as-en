// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>

void ASmain ()
{
    RC<RasterFont>  font = RasterFont();

    font.Load( "Roboto-Regular.ttf" );
    font.AddCharset_Ascii();
    font.AddCharset_Rus();

    font.RasterMode( ERasterFontMode::MC_SDF );
    font.Format( EPixelFormat::RGBA8_UNorm );

    font.GlyphSize( 32 );       // pix
    font.SDFGlyphBorder( 1 );   // pix, increase for bold style
    font.GlyphPadding( 4 );     // pix
    font.SDFPixelRange( 4.0 );  // for AA

//  font.GlyphSize( 64 );       // pix
//  font.SDFGlyphBorder( 8 );   // pix, increase for bold style
//  font.GlyphPadding( 2 );     // pix
//  font.SDFPixelRange( 1.0 );  // for AA

    font.Store( "canvas2d.sdf_font" );
}
