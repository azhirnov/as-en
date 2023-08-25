// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Draw cylinder with parallax mapping.
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>               rt          = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT-Color" );
        RC<Image>               ds          = Image( EPixelFormat::Depth32F, SurfaceSize() );       ds.Name( "RT-Depth" );
        RC<Image>               color_map   = Image( EImageType::FImage2D, "res/tex/rocks_color_rgba.ktx" );
        RC<Image>               norm_h_map  = Image( EImageType::FImage2D, "res/tex/rocks_normal_height_rgba.ktx" );

        RC<FPVCamera>           camera      = FPVCamera();
        RC<Buffer>              geom_data   = Buffer();
        RC<UnifiedGeometry>     geometry    = UnifiedGeometry();
        RC<Scene>               scene       = Scene();

        norm_h_map.SetSwizzle( "ARGB" );    // R - height, GBA - normal

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
            array<float3>   tangents;
            array<float3>   bitangents;
            array<float2>   texcoords;
            array<uint>     indices;
            GetCylinder( 32, /*inner*/true, OUT positions, OUT normals, OUT tangents, OUT bitangents, OUT texcoords, OUT indices );

            const float2    uv_scale = float2(6.28f, 0.5f) * 2.f;
            for (uint i = 0; i < texcoords.size(); ++i) {
                texcoords[i] *= uv_scale;
            }

            geom_data.FloatArray(   "positions",    positions );
            geom_data.FloatArray(   "texcoords",    texcoords );
            geom_data.FloatArray(   "normals",      normals );
            geom_data.FloatArray(   "tangents",     tangents );
            geom_data.FloatArray(   "bitangents",   bitangents );
            geom_data.UIntArray(    "indices",      indices );
            geom_data.Float(        "lightDir",     Normalize(float3( 0.f, -1.f, 0.f )) );
            geom_data.Layout( "GeometrySBlock" );

            UnifiedGeometry_DrawIndexed cmd;
            cmd.indexCount = indices.size();
            cmd.IndexBuffer( geom_data, "indices" );

            geometry.Draw( cmd );
            geometry.ArgIn( "un_Geometry",          geom_data );
            geometry.ArgIn( "un_ColorMap",          color_map,  Sampler_LinearMipmapRepeat );
            geometry.ArgIn( "un_HeightNormalMap",   norm_h_map, Sampler_LinearMipmapRepeat );
        }

        scene.Input( camera );
        scene.Input( geometry );

        // render loop
        {
            RC<SceneGraphicsPass>   draw = scene.AddGraphicsPass( "main pass" );
            draw.AddPipeline( "Draw/samples-Parallax.as" );    // file:///<path>/AE/samples/res_editor/_data/pipelines/Draw/samples-Parallax.as
            draw.Output( "out_Color", rt, RGBA32f( 0.3, 0.5, 1.0, 1.0 ));
            draw.Output( ds, DepthStencil( 1.f, 0 ));
            draw.Slider( "iHeightScale",    0.f,    0.1f );
            draw.Slider( "iShowNormals",    0,      1 );
            draw.Slider( "iScreenSpaceTBN", 0,      1 );
            draw.Slider( "iCalcNormal",     0,      1 );
            draw.Slider( "iMode",           0,      3 );
        }
        {
            RC<SceneGraphicsPass>   dbg_draw = scene.AddGraphicsPass( "debug normals" );
            dbg_draw.AddPipeline( "Draw/samples-DebugTBN.as" );    // file:///<path>/AE/samples/res_editor/_data/pipelines/Draw/samples-DebugTBN.as
            dbg_draw.Output( "out_Color", rt );
            dbg_draw.Output( ds );
            dbg_draw.Slider( "DbgTBN", 0, 3 );
        }

        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
