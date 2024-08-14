#include <asset_packer.as>

void ASmain ()
{
	{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::BC1_RGB8_A1_UNorm );
		tex.Store( "bc1" );
	}{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::BC2_RGBA8_UNorm );
		tex.Store( "bc2" );
	}{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::BC3_RGBA8_UNorm );
		tex.Store( "bc3" );
	}{
		RC<Texture>	tex = Texture();
		tex.Alloc( uint2(400, 224), EPixelFormat::R8_UNorm );
		tex.LoadChannel( "img.png", "R", "R" );
		tex.Format( EPixelFormat::BC4_R8_UNorm );
		tex.Store( "bc4" );
	}{
	/*	RC<Texture>	tex = Texture();
		tex.Alloc( uint2(400, 224), EPixelFormat::RG8_UNorm );
		tex.LoadChannel( "img.png", "RG", "RG" );
		tex.Format( EPixelFormat::BC5_RG8_UNorm );
		tex.Store( "bc5" ); */
	}{
		RC<Texture>	tex = Texture();
		tex.Alloc( uint2(128), EPixelFormat::RGBA16F );
		tex.Format( EPixelFormat::BC6H_RGB16UF );
		tex.Store( "bc6" );
	}{
	/*	RC<Texture>	tex = Texture();
		tex.Alloc( uint2(128), EPixelFormat::RGBA8_UNorm );
		tex.Format( EPixelFormat::BC7_RGBA8_UNorm );
		tex.Store( "bc7" ); */
	}
	//------------------------------------------
	{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::ETC2_RGBA8_UNorm );
		tex.Store( "etc2_a8" );
	}{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::ETC2_RGB8_A1_UNorm );
		tex.Store( "etc2_a1" );
	}
	//------------------------------------------
	/*{
		RC<Texture>	tex = Texture();
		tex.Alloc( uint2(400, 224), EPixelFormat::R8_UNorm );
		tex.LoadChannel( "img.png", "R", "R" );
		tex.Format( EPixelFormat::EAC_R11_UNorm );
		tex.Store( "eac_r" );
	}*/
	//------------------------------------------
	{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::ASTC_RGBA8_4x4 );
		tex.Store( "astc4x4" );
	}{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::ASTC_RGBA8_8x8 );
		tex.Store( "astc8x8" );
	}{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::ASTC_RGBA16F_4x4 );
		tex.Store( "astc4x4f" );
	}{
		RC<Texture>	tex = Texture();
		tex.Load( "img.png" );
		tex.Format( EPixelFormat::ASTC_RGBA16F_8x8 );
		tex.Store( "astc8x8f" );
	}
}
