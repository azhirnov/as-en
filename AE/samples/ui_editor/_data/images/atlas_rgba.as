// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <asset_packer.as>

void AddAtlasRGBA (RC<MetaData> meta, RC<SharedImage> sharedImage)
{
	RC<ImageAtlas>	atlas = ImageAtlas();

	atlas.Padding( 2 );
	atlas.Add( "Blue", 	"images/blue.png" );
	atlas.Add( "Green",	"images/green.png" );
	atlas.Add( "Red", 	"images/red.png" );
	atlas.Add( "RRR", 	"images/red.png" );
	atlas.Add( "B1", 	"images/blue.png",	RectU(0,0,8,8) );
	atlas.Add( "R2", 	"images/red.png",	RectU(8,8,32,32) );

	atlas.PutMeta( meta, "ui-atlas-rgba" );
	atlas.PutData( sharedImage );
}
