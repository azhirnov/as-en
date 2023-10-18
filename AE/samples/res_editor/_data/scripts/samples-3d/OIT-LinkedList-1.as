// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Exact OIT
    Per pixel linked list with front faces.
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT
    #include "samples/GenColoredSpheres.as"

    void ASmain ()
    {
        // initialize
        RC<Image>       rt              = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT-Color" );
        RC<Image>       abuf            = Image( EPixelFormat::R32U, SurfaceSize() );           abuf.Name( "ABuffer" );
        RC<Image>       ds              = Image( EPixelFormat::Depth32F, SurfaceSize() );       ds.Name( "RT-Depth" );
        RC<Buffer>      storage         = Buffer();
        RC<Buffer>      count_buf       = Buffer();

        RC<Scene>       scene           = Scene();
        RC<Buffer>      drawtasks       = Buffer();
        uint            instance_count  = 0;

        // setup camera
        {
            RC<FPVCamera>   camera = FPVCamera();

            camera.ClipPlanes( 0.1f, 100.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s );
            camera.UpDownScale( s );
            camera.SideMovementScale( s );

            camera.Position( float3(0.f, 0.f, -3.f));

            scene.Set( camera );
        }

        // setup draw tasks
        {
            array<float2x4> draw_tasks = GenColoredSpheresDrawTasks();
            drawtasks.FloatArray( "tasks", draw_tasks );
            drawtasks.LayoutName( "DrawTask" );
            instance_count = draw_tasks.size();
        }

        {
            storage.ArrayLayout(
                "IntrsPoint",
                "   float   depth;" +
                "   uint    objId;" +
                "   uint    next;",
                64 << 20 );

            count_buf.Uint( "counter", 0 );
            count_buf.LayoutName( "CountSBlock" );
        }

        // create sphere
        {
            RC<Buffer>              sphere      = Buffer();
            RC<UnifiedGeometry>     geometry    = UnifiedGeometry();

            array<float3>   positions;
            array<uint>     indices;
            GetSphere( 7, OUT positions, OUT indices );

            sphere.FloatArray( "positions", positions );
            sphere.UIntArray(  "indices",   indices );
            sphere.LayoutName( "GeometrySBlock" );

            UnifiedGeometry_DrawIndexed cmd;
            cmd.indexCount      = indices.size();
            cmd.instanceCount   = instance_count;
            cmd.IndexBuffer( sphere, "indices" );

            geometry.Draw( cmd );
            geometry.ArgIn(     "un_Geometry",  sphere );
            geometry.ArgIn(     "un_DrawTasks", drawtasks );
            geometry.ArgInOut(  "un_ABuffer",   abuf );
            geometry.ArgInOut(  "un_Storage",   storage );
            geometry.ArgInOut(  "un_Count",     count_buf );

            scene.Add( geometry );
        }

        // render loop
        {
            ClearImage( abuf, RGBA32u(~0) );
            ClearBuffer( count_buf, 0 );

            RC<SceneGraphicsPass>   draw = scene.AddGraphicsPass( "main pass" );
            draw.AddPipeline( "samples/OIT-LinkedList1.as" );   // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/OIT-LinkedList1.as)
            draw.Output( ds, DepthStencil( 1.f, 0 ));
        }
        {
            RC<Postprocess>     pass = Postprocess( EPostprocess::None, EPassFlags::Enable_ShaderTrace );
            pass.ArgIn( "un_ABuffer",   abuf );
            pass.ArgIn( "un_Storage",   storage );
            pass.ArgIn( "un_DrawTasks", drawtasks );
            pass.Output( "out_Color",   rt,     RGBA32f(0.f) );
        }

        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Sort.glsl"
    #include "Blend.glsl"

    ND_ uint  UnpackInstanceID (uint data)      { return data & 0xFFFF; }

    #define FRONT_TO_BACK   0

    #if FRONT_TO_BACK
    # define SortCmp( i, j )    points[i].depth > points[j].depth
    #else
    # define SortCmp( i, j )    points[i].depth < points[j].depth   // back to front
    #endif

    #define SortSwap( i, j )            \
        IntrsPoint  tmp = points[i];    \
        points[i]   = points[j];        \
        points[j]   = tmp;


    float4  Shading (uint objId)
    {
        return un_DrawTasks.tasks[ gl::Nonuniform(objId) ][1];
    }


    void  Main ()
    {
        uint        st_pos      = gl.image.Load( un_ABuffer, int2(gl.FragCoord.xy) ).r;
        uint        ip_count    = 0;
        IntrsPoint  points [32];

        // read all points
        for (; st_pos < un_Storage.elements.length() and ip_count < points.length(); ++ip_count)
        {
            points[ip_count] = un_Storage.elements[st_pos];
            st_pos = points[ip_count].next;
        }

        if ( ip_count == 0 )
        {
            out_Color = float4(ip_count);
            return;
        }

        BubbleSort( ip_count, SortCmp, SortSwap );

      #if FRONT_TO_BACK
        float4  color   = float4(0.0, 0.0, 0.0, 1.0);
      #else
        float4  color   = float4(0.0);
      #endif

        [[unroll]] for (uint i = 0; i < points.length(); ++i)
        {
            if ( i >= ip_count )
                break;

            float4  src = Shading( UnpackInstanceID( points[i].objId ));

            SeparateBlendParams     p;
          #if FRONT_TO_BACK
            p.srcColor      = src * src.a;  // from shader
            p.dstColor      = color;        // from render target
            p.srcBlendRGB   = EBlendFactor_DstAlpha;
            p.srcBlendA     = EBlendFactor_One;
            p.dstBlendRGB   = EBlendFactor_One;
            p.dstBlendA     = EBlendFactor_SrcAlpha;
            p.blendOpRGB    = EBlendOp_Add;
            p.blendOpA      = EBlendOp_Add;
          #else
            p.srcColor      = src;      // from shader
            p.dstColor      = color;    // from render target
            p.srcBlendRGB   = EBlendFactor_SrcAlpha;
            p.srcBlendA     = EBlendFactor_One;
            p.dstBlendRGB   = EBlendFactor_OneMinusSrcAlpha;
            p.dstBlendA     = EBlendFactor_OneMinusSrcAlpha;
            p.blendOpRGB    = EBlendOp_Add;
            p.blendOpA      = EBlendOp_Add;
          #endif

            color = BlendFn( p );
        }

        out_Color = color;
        out_Color.a = 1.0;
    }

#endif
//-----------------------------------------------------------------------------

