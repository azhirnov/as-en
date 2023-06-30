// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer>

void ASmain ()
{
    RC<RasterFont>  font = RasterFont();

    font.Load( "Roboto-Regular.ttf" );
    font.AddCharset_Ascii();
    font.AddCharset_Rus();

    font.RasterMode( ERasterFontMode::MC_SDF );
    font.Format( EPixelFormat::RGBA8_SNorm );

    font.GlyphSize( 32 );       // pix
    font.SDFPixelRange( 1 );    // pix, increse for bold style
    font.Padding( 4 );          // pix

//  font.GlyphSize( 64 );       // pix
//  font.SDFPixelRange( 8 );    // pix, increse for bold style
//  font.Padding( 2 );          // pix

    font.Store( "canvas2d.sdf_font" );
}
