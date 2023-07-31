// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>               rt          = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );            rt.Name( "RT-Color" );
        RC<Image>               ds          = Image( EPixelFormat::Depth32F, SurfaceSize() );               ds.Name( "RT-Depth" );
        RC<FPVCamera>           camera      = FPVCamera();
        RC<Buffer>              cube        = Buffer();
        RC<UnifiedGeometry>     geometry    = UnifiedGeometry();
        RC<Scene>               scene       = Scene();

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 100.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s, s );
            camera.UpDownScale( s, s );
            camera.SideMovementScale( s );
        }

        // create cube
        {
            array<float3>   positions;
            array<float3>   normals;
            array<uint>     indices;
            GetCube( OUT positions, OUT normals, OUT indices );

            cube.FloatArray( "positions",   positions );
            cube.FloatArray( "normals",     normals );
            cube.UIntArray(  "indices",     indices );
            cube.Layout( "fsbarycentric.CubeUBlock" );

            UnifiedGeometry_Draw    cmd;
            cmd.vertexCount = indices.size();
            geometry.Draw( cmd );
            geometry.Buffer( "un_Cube", cube );
        }

        scene.Input( camera );
        scene.Input( geometry, float3(0.f, 0.f, 4.f) );

        // render loop
        {
            RC<SceneGraphicsPass>   draw = scene.AddGraphicsPass( "main pass" );
            draw.AddPipeline( "Draw/FSBarycentric-Cube.as" );   // file:///<path>/samples/res_editor/_data/pipelines/Draw/FSBarycentric-Cube.as
            draw.Output( "out_Color", rt, RGBA32f( 0.3, 0.5, 1.0, 1.0 ));
            draw.Output( ds, DepthStencil(1.f, 0) );
        }
        Present( rt );
    }

#endif
