// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <res_editor>

Random  _rnd;
float   Rnd () { return _rnd.Uniform( 0.f, 1.f ); }

void ASmain ()
{
    // initialize
    RC<Image>               rt          = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT-Color" );
    RC<Image>               ds          = Image( EPixelFormat::Depth32F, SurfaceSize() );       ds.Name( "RT-Depth" );

    RC<Scene>               scene       = Scene();
    RC<FPVCamera>           camera      = FPVCamera();
    RC<UnifiedGeometry>     geometry    = UnifiedGeometry();
    RC<Buffer>              drawtasks   = Buffer();
    RC<Buffer>              cube        = Buffer();
    uint                    task_count  = 1;

    // setup camera
    {
        camera.ClipPlanes( 0.1f, 20.f );
        camera.FovY( 70.f );

        const float s = 0.3f;
        camera.ForwardBackwardScale( s );
        camera.UpDownScale( s );
        camera.SideMovementScale( s );
    }

    // setup draw tasks
    {
        const int2      grid_dim (128, 128);
        array<float4>   draw_tasks;

        for (int y = 0; y < grid_dim.y; ++y)
        {
            for (int x = 0; x < grid_dim.x; ++x)
            {
                int     idx     = x + y * grid_dim.x;
                float   scale   = 0.1f;
                float4  task;
                task.x  = ((x - grid_dim.x / 2) * 4.0f + (Rnd() * 2.0f - 1.0f)) * scale;
                task.y  = ((y - grid_dim.y / 2) * 4.0f + (Rnd() * 2.0f - 1.0f)) * scale;
                task.z  = (Rnd() * 0.5f + 0.5f) * scale;    // 0.5 .. 1  -- scale
                task.w  = Rnd();                            // time offset
                draw_tasks.push_back( task );
            }
        }
        drawtasks.FloatArray( "tasks", draw_tasks );
        drawtasks.Layout( "mesh.DrawTask" );
        task_count = draw_tasks.size();
    }

    // create cube
    {
        array<float3>   positions;
        array<float3>   normals;
        array<uint3>    indices;
        GetCube( OUT positions, OUT normals, OUT indices );

        cube.FloatArray( "positions",       positions );
        cube.FloatArray( "normals",         normals );
        cube.UIntArray(  "indices",         indices );
        cube.Float(      "sphereRadius",    2.0f * Sqrt(3.f) * 0.5f );  // cube_size * sqrt(3)/2
        cube.Layout( "mesh.CubeUBlock" );
    }

    // how to draw geometry
    {
        UnifiedGeometry_DrawMeshTasks   cmd;
        cmd.taskCount.x = task_count / 32;
        geometry.Draw( cmd );
        geometry.Buffer( "un_Cube",         cube );
        geometry.Buffer( "un_DrawTasks",    drawtasks );
    }

    scene.Input( camera );
    scene.Input( geometry, float3(0.f, 2.f, 0.f) );

    // render loop
    {
        RC<SceneGraphicsPass>   draw_pass = scene.AddGraphicsPass( "draw cubes" );
        draw_pass.AddPipeline( "Draw/MeshShader-Cube-2.as" );   // file:///<path>/samples/res_editor/_data/pipelines/Draw/MeshShader-Cube-2.as
        draw_pass.Output( "out_Color", rt, RGBA32f(0.f) );
        draw_pass.Output( ds, DepthStencil(1.f, 0) );

        Present( rt );
    }
}
