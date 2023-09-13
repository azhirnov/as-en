// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        RC<Image>       rt      = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT-Color" );
        RC<Image>       ds      = Image( EPixelFormat::Depth32F, SurfaceSize() );       ds.Name( "RT-Depth" );

        RC<Scene>       scene   = Scene();
        RC<FPVCamera>   camera  = FPVCamera();
        RC<Model>       model   = Model( "res/models/Sponza/Sponza.gltf" );

        // setup model
        {
            float4x4    m = float4x4().RotateZ( ToRad(180.f) ) *
                            float4x4().RotateY( ToRad(180.f) ) *
                            float4x4().Translate( float3( 0.f, -1.f, 0.f )) *
                            float4x4().Scaled( 100.f );
            model.InitialTransform( m );
        }

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 100.f );
            camera.FovY( 60.f );

            const float s = 1.0f;
            camera.ForwardBackwardScale( s*2.0f, s );
            camera.UpDownScale( s );
            camera.SideMovementScale( s );
        }

        scene.Set( camera );
        scene.Add( model, float3(0.0, 0.0, 2.0) );

        // render loop
        {
            RC<SceneGraphicsPass>   draw_pass = scene.AddGraphicsPass( "draw" );
            draw_pass.AddPipeline( "ModelShared.as" );  // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/ModelShared.as)
            draw_pass.AddPipelines( "Model" );          // [folder](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/Model/)
            draw_pass.Output( "out_Color", rt, RGBA32f(0.0f, 1.f, 1.f, 1.f) );
            draw_pass.Output( ds, DepthStencil(1.f, 0) );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
