// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <asset_packer.as>

void ASmain ()
{
    {
        RC<Texture>     tex = Texture();
        tex.Load( "tex1.png" );
        tex.Format( EPixelFormat::RGBA8_UNorm );
        tex.Store( "canvas2d.tex1" );
    }

    {
        RC<Texture>     tex = Texture();

        tex.AllocCube( uint2(128, 128), EPixelFormat::RGBA8_UNorm );
        tex.AddLayer( "cubemap.png", RectU(128, 128) + uint2(  1, 130), ECubeFace::XNeg );
        tex.AddLayer( "cubemap.png", RectU(128, 128) + uint2(130, 259), ECubeFace::YNeg );
        tex.AddLayer( "cubemap.png", RectU(128, 128) + uint2(130, 130), ECubeFace::ZPos );
        tex.AddLayer( "cubemap.png", RectU(128, 128) + uint2(259, 130), ECubeFace::XPos );
        tex.AddLayer( "cubemap.png", RectU(128, 128) + uint2(130,   1), ECubeFace::YPos );
        tex.AddLayer( "cubemap.png", RectU(128, 128) + uint2(388, 130), ECubeFace::ZNeg );

        tex.Store( "camera3d.cube" );
    }
}
