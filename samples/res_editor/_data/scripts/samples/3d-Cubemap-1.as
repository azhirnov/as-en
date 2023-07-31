// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <res_editor>

void ASmain ()
{
    RC<Image>           rt              = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT-Color" );
    RC<Image>           ds              = Image( EPixelFormat::Depth32F, SurfaceSize() );       ds.Name( "RT-Depth" );

    RC<Image>           cubemap         = Image( EPixelFormat::RGBA8_UNorm, uint2(640), ImageLayer(6), MipmapLevel(~0) );   cubemap.Name( "Cubemap tex" );
    RC<Image>           cubemap_view    = cubemap.CreateView( EImage::Cube );

    RC<Scene>           scene           = Scene();
    RC<FPVCamera>       camera          = FPVCamera();
    RC<SphericalCube>   skybox          = SphericalCube();

    // setup camera
    {
        camera.ClipPlanes( 0.1f, 10.f );
        camera.FovY( 70.f );

        const float s = 0.f;
        camera.ForwardBackwardScale( s );
        camera.UpDownScale( s );
        camera.SideMovementScale( s );
    }

    // setup skybox
    {
        skybox.AddTexture( "un_CubeMap", cubemap_view, Sampler_LinearMipmapRepeat );
        skybox.DetailLevel( 0, 9 );
    }

    scene.Input( camera );
    scene.Input( skybox );

    // load cubemap
    {
        cubemap.LoadLayer( "res/cubemap/cm-2.png", 4, ImageLoadOpFlags::GenMipmaps );   // -X
        cubemap.LoadLayer( "res/cubemap/cm-3.png", 0, ImageLoadOpFlags::GenMipmaps );   // -Z
        cubemap.LoadLayer( "res/cubemap/cm-4.png", 5, ImageLoadOpFlags::GenMipmaps );   // +X
        cubemap.LoadLayer( "res/cubemap/cm-5.png", 1, ImageLoadOpFlags::GenMipmaps );   // +Z
        cubemap.LoadLayer( "res/cubemap/cm-1.png", 3, ImageLoadOpFlags::GenMipmaps );   // -Y   - down
        cubemap.LoadLayer( "res/cubemap/cm-0.png", 2, ImageLoadOpFlags::GenMipmaps );   // +Y   - up
    }

    // render loop
    {
        RC<SceneGraphicsPass>   draw = scene.AddGraphicsPass( "main pass" );
        draw.AddPipeline( "VertexInput.as" );           // file:///<path>/samples/res_editor/_data/pipelines/VertexInput.as
        draw.AddPipeline( "Cubemap/Cubemap_v1.as" );    // file:///<path>/samples/res_editor/_data/pipelines/Cubemap/Cubemap_v1.as
        draw.Output( "out_Color", rt, RGBA32f(0.0) );
        draw.Output( ds, DepthStencil(1.f, 0) );
    }
    Present( rt );
}
