// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <res_editor>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>           rt      = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT" );
        RC<FlightCamera>    camera  = FlightCamera();
        RC<Buffer>          cbuf    = Buffer();

        // setup camera
        {
            camera.ClipPlanes( 0.1f, 10.f );
            camera.FovY( 70.f );
            camera.RotationScale( 1.f, 1.f, 1.f );
            camera.Dimension( rt.Dimension() );
            camera.Position( float3( 0.f, 1.f, 0.f ));
        }

        {
            cbuf.Float( "actualPos",        float3() );
            cbuf.Float( "prevPos",          float3() );
            cbuf.Float( "velocity",         float3() );
            cbuf.Float( "airplaneWidth",    0.1f );
            cbuf.Float( "liftScale",        0.5f );
            cbuf.Int(   "crashed",          0 );
        }

        // render loop
        {
            RC<ComputePass>     logic = ComputePass( "" );
            logic.ArgInOut( "un_CBuf",      cbuf );
            logic.ArgIn(    camera );
            logic.LocalSize( 1 );
            logic.DispatchGroups( 1 );

            RC<Postprocess>     draw = Postprocess( EPostprocess::Shadertoy, EPassFlags::None );
            draw.Input( "un_CBuf",  cbuf );
            draw.Input( camera );
            draw.Output( rt );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
    #include "Skyline.glsl"

    ND_ float  RayTrace (const Ray ray, const float maxDepth, const int maxIter)
    {
        float2          distAndMat;
        float           t           = ray.t;
        float3          pos         = float3(0.0);
        const float     smallVal    = 0.000625;
        const float3    rayVec      = ray.dir;

        for (int i = 0; i < maxIter; i++)
        {
            marchCount+=1.0;
            pos = (ray.origin + rayVec * t);
            distAndMat = DistanceToObject(pos);

            float walk = distAndMat.x;
            float dx = -fract(pos.x);
            if (rayVec.x > 0.0) dx = fract(-pos.x);
            float dz = -fract(pos.z);
            if (rayVec.z > 0.0) dz = fract(-pos.z);
            float nearestVoxel = min(fract(dx/rayVec.x), fract(dz/rayVec.z))+voxelPad;
            nearestVoxel = max(voxelPad, nearestVoxel);
            walk = min(walk, nearestVoxel);

            t += walk;
            if ((t > maxDepth) || (abs(distAndMat.x) < smallVal))
                break;
        }
        return t;
    }

    ND_ float  LiftMinusGravity ()
    {
        float3  v0  = (un_PerPass.camera.view * float4(-1.f, 0.f, 0.f, 0.f)).xyz;   v0.y = 0.f;
        float3  v1  = (un_PerPass.camera.view * float4( 1.f, 0.f, 0.f, 0.f)).xyz;   v1.y = 0.f;
        float3  v2  = (un_PerPass.camera.view * float4( 0.f, 0.f, 1.f, 0.f)).xyz;   v2.y = 0.f;
        float   lift = Length(Cross( v2 - v0, v2 - v1 )) * 0.5f;            // area of triangle in XZ plane

        return Pow( Saturate( 1.f - lift ), 2.0 ) * un_PerPass.timeDelta * un_CBuf.liftScale;
    }

    ND_ bool  HasCollision (const float3 pos, const float radius)
    {
        const Ray   ray = Ray_From( un_PerPass.camera.invViewProj, pos, 0.05, float2(0.5) );
        const float d   = RayTrace( ray, 5.0, 50 );
        return d < radius;
    }

    void  Main ()
    {
        const float3    delta       = un_PerPass.camera.pos - un_CBuf.prevPos;
        const float3    prev_pos    = un_CBuf.actualPos;
              float3    pos         = prev_pos + delta;
        const float     ground      = 0.f;

        pos.y -= LiftMinusGravity();

        if ( pos.y < ground )
            pos.y = ground;

        if ( HasCollision( pos, un_CBuf.airplaneWidth ))
        {
            un_CBuf.crashed = 1;
        }

        un_CBuf.actualPos   = pos;
        un_CBuf.prevPos     = un_PerPass.camera.pos;

        // restart
        if ( un_PerPass.customKeys == 1.0 )
        {
            un_CBuf.crashed     = 0;
            un_CBuf.actualPos   = float3(0.f, 1.f, 0.f);
        }
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Skyline.glsl"

    void mainVR (out vec4 fragColor, in vec2 fragCoord, in vec3 fragRayOri, in vec3 fragRayDir)
    {
        if ( un_CBuf.crashed != 0 ) {
            fragColor = float4(1.0, 0.0, 0.0, 1.0);
            return;
        }
        Ray ray = Ray_Create( fragRayOri, fragRayDir, 0.05 );
        fragColor = Trace( ray, fragCoord );
    }

    void mainImage (out vec4 fragColor, in vec2 fragCoord)
    {
        if ( un_CBuf.crashed != 0 ) {
            fragColor = float4(1.0, 0.0, 0.0, 1.0);
            return;
        }
        Ray ray = Ray_From( un_PerPass.camera.invViewProj, un_CBuf.actualPos, 0.05, fragCoord / iResolution.xy );
        fragColor = Trace( ray, fragCoord );
    }

#endif
//-----------------------------------------------------------------------------
