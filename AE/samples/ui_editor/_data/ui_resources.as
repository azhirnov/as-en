// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "fonts/font1.as"
#include "fonts/font2.as"

#include "images/atlas_rgba.as"

void ASmain ()
{
	RC<SharedImage>	rgba_image	= SharedImage();
	RC<SharedImage>	r_image		= SharedImage();
	RC<MetaData>	meta		= MetaData();

	r_image   .PutMeta( meta, "ui-shared-image-r" );
	rgba_image.PutMeta( meta, "ui-shared-image-rgba" );

	r_image   .Format( EPixelFormat::R8_UNorm );
	rgba_image.Format( EPixelFormat::RGBA8_UNorm );

	// pack font into R image
	AddRasterFont( meta, r_image );

	// pack atlas and font into single RGBA image
	AddMSdfFont( meta, rgba_image );
	AddAtlasRGBA( meta, rgba_image );

	r_image   .Store( "ui_image_r" );
	rgba_image.Store( "ui_image_rgba" );

	meta.Store( "ui_res_meta" );
}
