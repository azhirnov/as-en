// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Draw triangle using recursive ray tracing.
*/
#ifdef __INTELLISENSE__
#   include <res_editor.as>
#   define SH_RAY_GEN
#   include <aestyle.glsl.h>
#   define PRIMARY_MISS
#   define SHADOW_MISS
#   define PRIMARY_HIT
#   define SHADOW_HIT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // initialize
        RC<Image>   rt          = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );    rt.Name( "RT" );
        RC<RTScene> scene       = RTScene();
        RC<Buffer>  triangles   = Buffer();

        // create geometry
        {
            RC<RTGeometry>  geom        = RTGeometry();
            array<float2>   positions   = { float2(0.f, -0.5f), float2(0.5f, 0.5f), float2(-0.5f, 0.5f) };
            array<float3>   colors      = { float3(1.f, 0.f, 0.f), float3(0.f, 1.f, 0.f), float3(0.f, 0.f, 1.f) };
            array<uint>     indices     = { 0, 1, 2 };

            triangles.FloatArray(   "positions",    positions );
            triangles.FloatArray(   "colors",       colors );
            triangles.UIntArray(    "indices",      indices );

            geom.AddIndexedTriangles( triangles, triangles );

            scene.AddInstance( geom );
        }

        // render loop
        {
            RC<RayTracingPass>      pass = RayTracingPass( EPassFlags::Enable_ShaderTrace );
            pass.ArgOut( "un_OutImage",     rt );
            pass.ArgIn(  "un_RtScene",      scene );
            pass.ArgIn(  "un_Triangles",    triangles );
            pass.Dispatch( rt.Dimension() );

            // setup SBT
            pass.HitGroupStride( 1 );
            pass.RayGen( RTShader("") );
            pass.RayMiss( MissIndex(0), RTShader("") );
            pass.TriangleHit( RayIndex(0), InstanceIndex(0), RTShader("") );
        }
        Present( rt );
    }

#endif
//-----------------------------------------------------------------------------

#define RAY_INDEX   0

//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
    #include "GlobalIndex.glsl"
    #include "HWRayTracing.glsl"

    layout(location=RAY_INDEX) gl::RayPayload float4  payload;

    void Main ()
    {
        HWRay   hwray = HWRay_Create();
        hwray.rayFlags          = gl::RayFlags::None;
        hwray.sbtRecordOffset   = RAY_INDEX;
        hwray.sbtRecordStride   = HitGroupStride;
        hwray.missIndex         = RAY_INDEX;
        hwray.rayOrigin         = float3(GetGlobalCoordSNormCorrected() * 1.5f, -1.0f);
        hwray.rayDir            = float3(0.0f, 0.0f, 1.0f);
        hwray.tMin              = 0.0f;
        hwray.tMax              = 10.0f;

        payload = float4(0.0);

        // hitShader = RTSceneBuild::Instance::instanceSBTOffset + geometryIndex * sbtRecordStride + sbtRecordOffset

        HWTraceRay( un_RtScene, hwray, /*payload*/RAY_INDEX );

        gl.image.Store( un_OutImage, int2(gl.LaunchID), payload );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_MISS
    layout(location=RAY_INDEX) gl::RayPayloadIn float4  payload;

    void Main ()
    {
        payload = float4(0.0, 0.1, 0.1, 1.0);
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_CHIT
    #include "Math.glsl"

    layout(location=RAY_INDEX) gl::RayPayloadIn float4  payload;

    gl::HitAttribute float2  in_HitAttribs;

    ND_ float4  HitShader (const float2 barycentric, const uint primitiveId)
    {
        float3  c0      = un_Triangles.colors[ un_Triangles.indices[ primitiveId * 3 + 0 ]];
        float3  c1      = un_Triangles.colors[ un_Triangles.indices[ primitiveId * 3 + 1 ]];
        float3  c2      = un_Triangles.colors[ un_Triangles.indices[ primitiveId * 3 + 2 ]];
        float3  color   = BaryLerp( c0, c1, c2, barycentric );
        return float4(color, 1.f);
    }

    void Main ()
    {
        payload = HitShader( in_HitAttribs, gl.PrimitiveID );
    }

#endif
//-----------------------------------------------------------------------------
