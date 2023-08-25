// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>

void ASmain ()
{
    RC<RasterFont>  font = RasterFont();

    font.Load( "Roboto-Regular.ttf" );
    font.AddCharset_Ascii();
    font.AddCharset_Rus();

    font.RasterMode( ERasterFontMode::Raster );
    font.Format( EPixelFormat::R8_UNorm );

    font.GlyphSize( 60 );   // pix
    font.Padding( 2 );      // pix

    font.Store( "canvas2d.font" );
}
