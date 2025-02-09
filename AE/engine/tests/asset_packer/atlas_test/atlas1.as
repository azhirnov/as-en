#include <asset_packer.as>

void ASmain ()
{
	RC<ImageAtlas>	atlas = ImageAtlas();

	atlas.Padding( 2 );
	atlas.Add( "Blue", 	"blue.png" );
	atlas.Add( "Green",	"green.png" );
	atlas.Add( "Red", 	"red.png" );
	atlas.Add( "RRR", 	"red.png" );
	atlas.Add( "B1", 	"blue.png",	RectU(0,0,8,8) );
	atlas.Add( "R2", 	"red.png",	RectU(8,8,32,32) );

	atlas.Store( "atlas" );
}
