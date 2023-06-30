// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#   include <aestyle.glsl.h>
    gl::AccelerationStructure   un_RtScene;
    gl::Image2D<float>          un_OutImage;
#endif
//-----------------------------------------------------------------------------

#define PAYLOAD_LOC 0
//-----------------------------------------------------------------------------

#ifdef SH_RAY_GEN
    layout(location=PAYLOAD_LOC) gl::RayPayload float4  payload;

    void Main ()
    {
        const float2    uv          = (float2(gl.LaunchID.xy) + 0.5) / float2(gl.LaunchSize.xy);
        const float3    origin      = float3(uv.x, 1.0f - uv.y, -1.0f);
        const float3    direction   = float3(0.0f, 0.0f, 1.0f);
        const uint      inst_offset = HitGroupStride + 1;

        payload = float4(0.0);

        // hitShader = RTSceneBuild::Instance::instanceSBTOffset + geometryIndex * sbtRecordStride + sbtRecordOffset

        gl.TraceRay( /*topLevel*/un_RtScene, /*rayFlags*/gl::RayFlags::None, /*cullMask*/0xFF,
                     /*sbtRecordOffset*/inst_offset, /*sbtRecordStride*/HitGroupStride, /*missIndex*/0,
                     /*origin*/origin, /*Tmin*/0.0f, /*direction*/direction, /*Tmax*/10.0f,
                     /*payload*/0 );

        gl.image.Store( un_OutImage, int2(gl.LaunchID), payload );
    }
#endif
//-----------------------------------------------------------------------------

#ifdef SH_RAY_MISS
    layout(location=PAYLOAD_LOC) gl::RayPayloadIn float4  payload;

    void Main ()
    {
        payload = float4(0.0, 0.1, 0.1, 1.0);
    }
#endif
//-----------------------------------------------------------------------------

#ifdef SH_RAY_CHIT
    layout(location=PAYLOAD_LOC) gl::RayPayloadIn float4  payload;

    gl::HitAttribute float2  hitAttribs;

    void Main ()
    {
        float3  barycentricCoords = float3(hitAttribs.x, hitAttribs.y, 1.0f - hitAttribs.x - hitAttribs.y);
        payload = float4(barycentricCoords, 1.0);
    }
#endif
