// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Simple ray marching (sphere tracing) with triplanar texturing.
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
        RC<Image>       rt      = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
        RC<FPVCamera>   camera  = FPVCamera();
        RC<Image>       tex1    = Image( EImageType::FImage2D, "shadertoy/Abstract_2.jpg" );
        RC<Image>       tex2    = Image( EImageType::FImage2D, "shadertoy/Wood.jpg" );

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 100.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s*2.f, s );
            camera.UpDownScale( s, s );
            camera.SideMovementScale( s );
        }

        // render loop
        {
            RC<Postprocess>     pass = Postprocess( EPostprocess::Shadertoy, EPassFlags::None );
            pass.ArgIn( camera );
            pass.ArgIn( "un_Object2Tex",    tex1,   Sampler_LinearMipmapRepeat );
            pass.ArgIn( "un_Object3Tex",    tex2,   Sampler_LinearMipmapRepeat );
            pass.Output( rt );
            pass.Slider( "iTexturing", 0, 1 );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Ray.glsl"
    #include "SDF.glsl"
    #include "Normal.glsl"
    #include "DistAndMtr.glsl"
    #include "TexSampling.glsl"

    const int   MTR_Ground      = 0;
    const int   MTR_Object1     = 1;
    const int   MTR_Object2     = 2;
    const int   MTR_Object3     = 3;
    const int   MTR_Sky         = -1;

    ND_ DistAndMtr  SDFScene (const float3 pos)
    {
        DistAndMtr  dm = DM_Create( 1.0e+10, MTR_Sky );
        dm = DM_Min( dm, SDF_Plane(     pos,                                    float3(0.0, -1.0, 0.0), 0.5 ), MTR_Ground );
        dm = DM_Min( dm, SDF_Cylinder(  SDF_Move(pos, float3(-1.5, 0.0, 2.0)),  float2(0.4, 1.5)            ), MTR_Object1 );
        dm = DM_Min( dm, SDF_Box(       SDF_Move(pos, float3( 1.5, 0.0, 2.0)),  float3(0.3)                 ), MTR_Object2 );
        dm = DM_Min( dm, SDF_Sphere(    SDF_Move(pos, float3( 0.0, 0.0, 2.0)),  0.3                         ), MTR_Object3 );
        return dm;
    }


    ND_ DistAndMtr  SphereTrace (Ray ray)
    {
        DistAndMtr  dm;
        const int   max_iter    = 256;
        const float min_dist    = 0.00625;
        const float max_dist    = 100.0;

        for (int i = 0; i < max_iter; ++i)
        {
            dm = SDFScene( ray.pos );

            Ray_Move( INOUT ray, dm.dist );

            if ( Abs(dm.dist) < min_dist )
                break;

            if ( ray.t > max_dist )
            {
                dm.mtrIndex = MTR_Sky;
                break;
            }
        }
        return DM_Create( ray.t, dm.mtrIndex );
    }


    void mainImage (out float4 fragColor, in float2 fragCoord)
    {
        // get ray from ViewProj matrix and pixel coordinate
        Ray         ray = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, gl.FragCoord.xy / iResolution.xy );

        DistAndMtr  dm = SphereTrace( ray );

        Ray_Move( INOUT ray, dm.dist );

        const float3    light_dir   = Normalize( float3( 0.f, -1.0f, -0.7f ));
        const float3    normal      = ScreenSpaceNormal_dxdy( ray.pos );
        const float3    uv_norm     = Abs(normal);
        const float     light       = Saturate( Dot( normal, light_dir )) + 0.25f;  // with ambient

        switch ( dm.mtrIndex )
        {
            case MTR_Ground :
            {
                float4  col1 = float4( 0.2, 0.2, 0.2, 1.0 );
                fragColor = col1 * light;
                break;
            }
            case MTR_Object1 :
            {
                float4  col1 = float4( 1.0, 0.0, 0.0, 1.0 );
                fragColor = col1 * light;
                break;
            }
            case MTR_Object2 :
            {
                float4  col1 = float4( 0.0, 1.0, 0.0, 1.0 );
                float4  col2 = TriplanarMapping( ray.pos, uv_norm, un_Object2Tex );
                fragColor = Lerp( col1, col2, float(iTexturing) ) * light;
                break;
            }
            case MTR_Object3 :
            {
                float4  col1 = float4( 0.0, 0.0, 1.0, 1.0 );
                float4  col2 = TriplanarMapping( ray.pos, uv_norm, un_Object3Tex );
                fragColor = Lerp( col1, col2, float(iTexturing) ) * light;
                break;
            }
            case MTR_Sky :
            default :
                fragColor = float4( 0.0, 1.0, 1.0, 1.0 );
                break;
        }
    }

#endif
//-----------------------------------------------------------------------------
