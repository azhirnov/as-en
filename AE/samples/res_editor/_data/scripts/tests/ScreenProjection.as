// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
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
        RC<Image>       rt      = Image( EPixelFormat::RGBA16F, SurfaceSize() );
        RC<FPVCamera>   camera  = FPVCamera();

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 100.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s, s );
            camera.UpDownScale( s, s );
            camera.SideMovementScale( s );
        }

        // render loop
        {
            RC<Postprocess>     pass = Postprocess();
            pass.Set(    camera );
            pass.Output( "out_Color", rt );
            pass.Slider( "iProj",           0,  5 );
            pass.Slider( "iDbgView",        0,  1 );
            pass.Slider( "iInversionError", 0,  1 );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "GlobalIndex.glsl"
    #include "Ray.glsl"
    #include "Waves.glsl"

    void Main ()
    {
        Ray             ray;
        const float2    uv              = GetGlobalCoordUNorm().xy;
        float2          uv2             = uv;
        const float     ipd             = 64.0e-3f; // meters
        const float     z_near          = 0.1f;
        const float     dist_to_eye     = 0.5f;     // meters
        const float2    screen_size     = un_PerPass.resolution.xy * un_PerPass.pixToMm * 0.001f;   // meters
        const float     curve_radius    = 1.8f; // meters

        switch ( iProj )
        {
            // screen ViewProj matrix to ray:
            case 0 :    ray = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, z_near, uv );         break;

            // VR video:
            case 1 :    ray = Ray_PlaneToVR180( ipd, un_PerPass.camera.pos, z_near, uv );
                        uv2 = Inverted_PlaneToVR180( ray.dir, uv.x < 0.5 ? 0 : 1 );                                 break;

            case 2 :    ray = Ray_PlaneToVR360( ipd, un_PerPass.camera.pos, z_near, uv );
                        uv2 = Inverted_PlaneToVR360( ray.dir, uv.y < 0.5 ? 0 : 1 );                                 break;

            // 360 video
            case 3 :    ray = Ray_PlaneTo360( un_PerPass.camera.pos, z_near, uv );
                        uv2 = Inverted_PlaneTo360( ray.dir );                                                       break;

            // flat screen
            case 4 :    ray = Ray_FromFlatScreen( un_PerPass.camera.pos, dist_to_eye, screen_size, z_near, ToSNorm(uv) ); break;

            // curved screen
            case 5 :    ray = Ray_FromCurvedScreen( un_PerPass.camera.pos, dist_to_eye, curve_radius, screen_size, z_near, ToSNorm(uv) ); break;
        }

        out_Color = float4( ray.dir, 1.0 );

        if ( iDbgView == 0 )
            out_Color.rgb *= TriangleWave( ray.dir.z*100.0 );

        if ( iInversionError == 1 )
            out_Color.rgb = float3( Abs( uv - uv2 ) * 1000.f, 0.f );

        out_Color.a = ray.dir.z;
    }

#endif
//-----------------------------------------------------------------------------
