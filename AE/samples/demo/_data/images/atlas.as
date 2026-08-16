// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	images searched in path specified by '-d' console argument
*/
#include <asset_packer.as>

void ASmain ()
{
	RC<ImageAtlas>	atlas = ImageAtlas();

	atlas.Padding( 2 );
	atlas.Add( "Blue", 	"images/blue.png" );
	atlas.Add( "Green",	"images/green.png" );
	atlas.Add( "Red", 	"images/red.png" );
	atlas.Add( "RRR", 	"images/red.png" );
	atlas.Add( "B1", 	"images/blue.png",	RectU(0,0,8,8) );
	atlas.Add( "R2", 	"images/red.png",	RectU(8,8,32,32) );

	atlas.Store( "canvas2d.atlas" );
}
