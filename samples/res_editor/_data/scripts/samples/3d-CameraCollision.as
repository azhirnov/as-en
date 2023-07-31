// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor>
#   include <aestyle.glsl.h>
#   define CALC_COLLISION
#   define TRACE_RAYS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>       rt      = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT" );
        RC<FPSCamera>   camera  = FPSCamera();
        RC<Buffer>      cam_pos = Buffer();

        cam_pos.Float(  "actualPos",    float3() );
        cam_pos.Float(  "prevPos",      float3() );

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 100.f );
            camera.FovY( 50.f );

            const float s = 0.8f;
            camera.ForwardBackwardScale( s*2.f, s );
            camera.UpDownScale( s*6.f, s );
            camera.SideMovementScale( s );

            camera.Dimension( rt.Dimension() );
        }

        // render loop
        {
            RC<ComputePass>     collision   = ComputePass( "", "CALC_COLLISION", EPassFlags::Enable_ShaderTrace );
            collision.ArgIn(    camera );
            collision.ArgInOut( "un_CollisionData", cam_pos );
            collision.LocalSize( 1 );
            collision.DispatchGroups( 1 );

            RC<ComputePass>     draw        = ComputePass( "", "TRACE_RAYS", EPassFlags::Enable_ShaderTrace );
            draw.ArgIn(     camera );
            draw.ArgOut(    "un_OutImage",      rt );
            draw.ArgInOut(  "un_CollisionData", cam_pos );
            draw.LocalSize( 8, 8 );
            draw.DispatchThreads( rt.Dimension() );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
    #include "SDF.glsl"
    #include "DistAndMat.glsl"

    const float GroundY         = 1.f;
    const float CameraHeight    = 0.75f;
    const float CameraRadius    = 0.125f;
    const float Gravity         = 1.f;

    const int   MTR_Ground      = 0;
    const int   MTR_Column      = 1;
    const int   MTR_Sky         = 2;

    ND_ DistAndMat  SDFGround (const float3 pos)
    {
        DistAndMat  dm = DM_Create();
        dm.mtrIndex = MTR_Ground;
        dm.dist     = SDF_Plane( pos, float3(0.0, -GroundY, 0.0), 0.5 );
        return dm;
    }

    ND_ DistAndMat  SDFColumns2 (const float3 pos)
    {
        DistAndMat  dm = DM_Create();
        dm.mtrIndex = MTR_Column;
        dm.dist     = SDF_Cylinder( pos, float2(0.2, 1.0) );
        return dm;
    }

    ND_ DistAndMat  SDFColumns (float3 pos)
    {
        pos = SDF_Move( pos, float3(-0.5f, 0.f, -0.5f));
        return SDF_Repetition( pos, 2.f, float3(100.f, 1.f, 100.f), SDFColumns2 );
    }

    ND_ DistAndMat  SDFScene (const float3 pos)
    {
        DistAndMat  dm = DM_Create( 99.0, MTR_Sky );
        dm = DM_Min( dm, SDFColumns( pos ));
        dm = DM_Min( dm, SDFGround( pos ));
        return dm;
    }

    GEN_SDF_NORMAL_FN( SDFNormal, SDFScene, .dist )

#endif
//-----------------------------------------------------------------------------
#ifdef CALC_COLLISION

    void  Main ()
    {
        const float3    delta       = un_PerPass.camera.pos - un_CollisionData.prevPos;
        const float3    prev_pos    = un_CollisionData.actualPos;
              float3    pos         = prev_pos + delta;
        const float     ground      = GroundY - CameraHeight;
        const float     dist        = SDFScene( pos ).dist;

        if ( dist < CameraRadius )
        {
            const float3    left    = prev_pos + LeftVectorXZ( delta );
            const float3    right   = prev_pos + RightVectorXZ( delta );
            const float     l_dist  = SDFScene( left ).dist;
            const float     r_dist  = SDFScene( right ).dist;

            pos = prev_pos;

            if ( l_dist > r_dist ){
                if ( l_dist >= CameraRadius )   pos = left;
            }else{
                if ( r_dist >= CameraRadius )   pos = right;
            }
        }

        pos.y += Gravity * un_PerPass.timeDelta;

        if ( pos.y > ground )
            pos.y = ground;

        un_CollisionData.actualPos  = pos;
        un_CollisionData.prevPos    = un_PerPass.camera.pos;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef TRACE_RAYS
    #include "GlobalIndex.glsl"
    #include "Ray.glsl"
    #include "Fog.glsl"

    void  Main ()
    {
        Ray         ray         = Ray_From( un_PerPass.camera.invViewProj, un_CollisionData.actualPos, 0.1, GetGlobalCoordUNorm().xy );

        const int   max_iter    = 256;
        const float min_dist    = 0.00625;
        const float max_dist    = 100.0;
        int         mtr_index   = -1;

        // ray marching
        for (int i = 0; i < max_iter; ++i)
        {
            DistAndMat  dm = SDFScene( ray.pos );

            mtr_index = dm.mtrIndex;

            Ray_Move( INOUT ray, dm.dist );

            if ( Abs(dm.dist) < min_dist )
                break;

            if ( ray.t > max_dist )
            {
                mtr_index = -1;
                break;
            }
        }

        const float3    light_dir   = Normalize( float3( 0.f, -1.0f, -0.7f ));
        const float3    normal      = SDFNormal( ray.pos );
        const float     light       = Saturate( Dot( normal, light_dir )) + 0.25f;
        const float     fog         = FogFactorExp( ray.t / max_dist, 16.0f );
        const float4    fog_color   = float4( 0.4f, 0.4f, 0.5f, 1.f );

        float4  color;
        switch ( mtr_index )
        {
            case MTR_Ground :   color = float4( 0.2f, 0.2f, 0.2f, 1.f ) * light;    color = Lerp( color, fog_color, fog );  break;
            case MTR_Column :   color = float4( 1.0f, 0.0f, 0.0f, 1.f ) * light;    color = Lerp( color, fog_color, fog );  break;
            case MTR_Sky :
            default :           color = float4( 0.0f, 1.0f, 1.0f, 1.f );            break;
        }

        gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
    }

#endif
//-----------------------------------------------------------------------------

