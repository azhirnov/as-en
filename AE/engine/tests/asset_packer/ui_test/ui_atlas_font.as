// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>

void ASmain ()
{
	RC<SharedImage>	rgba_image	= SharedImage();
	RC<SharedImage>	r_image		= SharedImage();
	RC<MetaData>	meta		= MetaData();

	r_image   .PutMeta( meta, "ui-shared-image-r" );
	rgba_image.PutMeta( meta, "ui-shared-image-rgba" );

	{
		RC<RasterFont>	font = RasterFont();

		font.Load( "../font_test/Roboto-Regular.ttf" );
		font.AddCharset_Ascii();
		font.AddCharset_Rus();

		font.RasterMode( ERasterFontMode::SDF );
		font.Format( EPixelFormat::R8_UNorm );

		font.GlyphSize( 32 );		// pix
		font.SDFGlyphBorder( 1 );	// pix,	increase for bold style
		font.GlyphPadding( 4 );		// pix

		font.PutMeta( meta, "ui_font" );
		font.PutData( r_image );
	}{
		RC<ImageAtlas>	atlas = ImageAtlas();

		atlas.Padding( 2 );
		atlas.Add( "Blue", 	"../atlas_test/blue.png" );
		atlas.Add( "Green",	"../atlas_test/green.png" );
		atlas.Add( "Red", 	"../atlas_test/red.png" );

		atlas.PutMeta( meta, "ui_atlas" );
		atlas.PutData( rgba_image );
	}

	r_image   .Store( "ui_image_r" );
	rgba_image.Store( "ui_image_rgba" );

	meta.Store( "ui_res_meta" );
}
