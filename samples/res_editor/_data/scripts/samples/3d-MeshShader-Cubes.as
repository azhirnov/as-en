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
        drawtasks.Float4Array( "tasks", draw_tasks );
        drawtasks.Layout( "mesh.DrawTask" );
        task_count = draw_tasks.size();
    }

    // create cube
    {
        array<float3>   positions = {
            float3(-1.0f, -1.0f, -1.0f),    float3(-1.0f,  1.0f, -1.0f),    float3( 1.0f,  1.0f, -1.0f),    float3( 1.0f, -1.0f, -1.0f),    // -Z
            float3(-1.0f, -1.0f,  1.0f),    float3( 1.0f, -1.0f,  1.0f),    float3( 1.0f,  1.0f,  1.0f),    float3(-1.0f,  1.0f,  1.0f),    // +Z
            float3(-1.0f, -1.0f, -1.0f),    float3( 1.0f, -1.0f, -1.0f),    float3( 1.0f, -1.0f,  1.0f),    float3(-1.0f, -1.0f,  1.0f),    // -Y
            float3( 1.0f, -1.0f, -1.0f),    float3( 1.0f,  1.0f, -1.0f),    float3( 1.0f,  1.0f,  1.0f),    float3( 1.0f, -1.0f,  1.0f),    // +X
            float3( 1.0f,  1.0f, -1.0f),    float3(-1.0f,  1.0f, -1.0f),    float3(-1.0f,  1.0f,  1.0f),    float3( 1.0f,  1.0f,  1.0f),    // +Y
            float3(-1.0f,  1.0f, -1.0f),    float3(-1.0f, -1.0f, -1.0f),    float3(-1.0f, -1.0f,  1.0f),    float3(-1.0f,  1.0f,  1.0f)     // -X
        };
        array<float3>   normals = {
            float3( 0.0f,  0.0f, -1.0f),    float3( 0.0f,  0.0f, -1.0f),    float3( 0.0f,  0.0f, -1.0f),    float3( 0.0f,  0.0f, -1.0f),    // -Z
            float3( 0.0f,  0.0f,  1.0f),    float3( 0.0f,  0.0f,  1.0f),    float3( 0.0f,  0.0f,  1.0f),    float3( 0.0f,  0.0f,  1.0f),    // +Z
            float3( 0.0f, -1.0f,  0.0f),    float3( 0.0f, -1.0f,  0.0f),    float3( 0.0f, -1.0f,  0.0f),    float3( 0.0f, -1.0f,  0.0f),    // -Y
            float3( 1.0f,  0.0f,  0.0f),    float3( 1.0f,  0.0f,  0.0f),    float3( 1.0f,  0.0f,  0.0f),    float3( 1.0f,  0.0f,  0.0f),    // +X
            float3( 0.0f,  1.0f,  0.0f),    float3( 0.0f,  1.0f,  0.0f),    float3( 0.0f,  1.0f,  0.0f),    float3( 0.0f,  1.0f,  0.0f),    // +Y
            float3(-1.0f,  0.0f,  0.0f),    float3(-1.0f,  0.0f,  0.0f),    float3(-1.0f,  0.0f,  0.0f),    float3(-1.0f,  0.0f,  0.0f)     // -X
        };
        array<uint3>    indices = {
            uint3( 0,  1,  2),   uint3( 2,  3,  0),     //  0-3
            uint3( 4,  5,  6),   uint3( 6,  7,  4),     //  4-7
            uint3( 8,  9, 10),   uint3(10, 11,  8),     //  8-11
            uint3(12, 13, 14),   uint3(14, 15, 12),     // 12-15
            uint3(16, 17, 18),   uint3(18, 19, 16),     // 16-19
            uint3(20, 21, 22),   uint3(22, 23, 20)      // 20-23
        };

        cube.Float3Array(   "positions",    positions );
        cube.Float3Array(   "normals",      normals );
        cube.UInt3Array(    "indices",      indices );
        cube.Float(         "sphereRadius", 2.0f * Sqrt(3.f) * 0.5f );  // cube_size * sqrt(3)/2
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
        RC<SceneGraphicsPass>   pass0 = scene.AddGraphicsPass( "pass name" );
        pass0.AddPipeline( "Mesh/Cube_2.as" );  // file:///<path>/samples/res_editor/_data/pipelines/Mesh/Cube_2.as
        pass0.Output( "out_Color", rt, RGBA32f(0.f) );
        pass0.Output( ds, DepthStencil(1.f, 0) );

        Present( rt );
    }
}
