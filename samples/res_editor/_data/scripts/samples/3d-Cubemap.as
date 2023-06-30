// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <res_editor>

void ASmain ()
{
    RC<Image>           rt              = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT-Color" );
    RC<Image>           ds              = Image( EPixelFormat::Depth32F, SurfaceSize() );       ds.Name( "RT-Depth" );

    RC<Image>           cubemap         = Image( EPixelFormat::RGBA8_UNorm, uint2(640), ImageLayer(6), MipmapLevel(~0) );   cubemap.Name( "Cubemap tex" );
    RC<Image>           cubemap_view    = cubemap.CreateView( EImage::Cube );

    RC<Scene>           scene   = Scene();
    RC<FPVCamera>       camera  = FPVCamera();
    RC<SphericalCube>   skybox  = SphericalCube();

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

    // generate cubemap
    #if 1
    {
        RC<Collection>  args = Collection();
        args.Add( "dst", cubemap_view );

        RunScript( "GenCubemap.as", ScriptFlags::RunOnce, args );   // file:///<path>/samples/res_editor/_data/scripts2/GenCubemap.as
    }
    #else
    {
        cubemap.LoadLayer( "res_editor/cubemap/cm-2.png", 4, ImageLoadOpFlags::GenMipmaps );    // -X
        cubemap.LoadLayer( "res_editor/cubemap/cm-3.png", 0, ImageLoadOpFlags::GenMipmaps );    // -Z
        cubemap.LoadLayer( "res_editor/cubemap/cm-4.png", 5, ImageLoadOpFlags::GenMipmaps );    // +X
        cubemap.LoadLayer( "res_editor/cubemap/cm-5.png", 1, ImageLoadOpFlags::GenMipmaps );    // +Z
        cubemap.LoadLayer( "res_editor/cubemap/cm-1.png", 3, ImageLoadOpFlags::GenMipmaps );    // -Y   - down
        cubemap.LoadLayer( "res_editor/cubemap/cm-0.png", 2, ImageLoadOpFlags::GenMipmaps );    // +Y   - up
    }
    #endif

    // render loop
    {
        RC<SceneGraphicsPass>   draw_pass = scene.AddGraphicsPass( "main pass" );
        draw_pass.AddPipeline( "VertexInput.as" );          // file:///<path>/samples/res_editor/_data/pipelines/VertexInput.as
        draw_pass.AddPipeline( "Cubemap/Cubemap_v1.as" );   // file:///<path>/samples/res_editor/_data/pipelines/Cubemap/Cubemap_v1.as
        draw_pass.Output( "out_Color", rt, RGBA32f(0.0) );
        draw_pass.Output( ds, DepthStencil(1.f, 0) );
    }
    Present( rt );
}
