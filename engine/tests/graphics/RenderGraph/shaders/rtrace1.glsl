
#define PAYLOAD_LOC 0

void dbg_EnableTraceRecording (bool b) {}
//-----------------------------------------------------------------------------

#ifdef SH_RAY_GEN
layout(location=PAYLOAD_LOC) rayPayloadEXT vec4  payload;

void Main ()
{
    dbg_EnableTraceRecording( (gl_LaunchIDEXT.x == 370 && gl_LaunchIDEXT.y == 326) ||
                              (gl_LaunchIDEXT.x == 612 && gl_LaunchIDEXT.y == 124) );

    const vec2  uv          = vec2(gl_LaunchIDEXT.xy + 0.5) / vec2(gl_LaunchSizeEXT.xy);
    const vec3  origin      = vec3(uv.x, 1.0f - uv.y, -1.0f);
    const vec3  direction   = vec3(0.0f, 0.0f, 1.0f);
    const uint  ray_idx     = 1;    // must be < HitGroupStride

    payload = vec4(0.0);

    // hitShader = RTSceneBuild::Instance::instanceSBTOffset + geometryIndex * sbtRecordStride + sbtRecordOffset

    traceRayEXT( /*topLevel*/un_RtScene, /*rayFlags*/gl_RayFlagsNoneEXT, /*cullMask*/0xFF,
                 /*sbtRecordOffset*/ray_idx, /*sbtRecordStride*/HitGroupStride, /*missIndex*/1,
                 /*origin*/origin, /*Tmin*/0.0f, /*direction*/direction, /*Tmax*/10.0f,
                 /*payload*/0 );

    vec4    color = payload;
    imageStore( un_OutImage, ivec2(gl_LaunchIDEXT), color );
}
#endif
//-----------------------------------------------------------------------------

#ifdef SH_RAY_MISS
layout(location=PAYLOAD_LOC) rayPayloadInEXT vec4  payload;

void Main ()
{
    dbg_EnableTraceRecording( gl_LaunchIDEXT.x == 612 && gl_LaunchIDEXT.y == 124 );

    payload = vec4( 0.412f, 0.796f, 1.0f, 1.0f );
}
#endif
//-----------------------------------------------------------------------------

#ifdef SH_RAY_CHIT
layout(location=PAYLOAD_LOC) rayPayloadInEXT vec4  payload;

hitAttributeEXT vec2  hitAttribs;

void Main ()
{
    dbg_EnableTraceRecording( gl_LaunchIDEXT.x == 370 && gl_LaunchIDEXT.y == 326 );

    const vec3  barycentrics = vec3( 1.0f - hitAttribs.x - hitAttribs.y, hitAttribs.x, hitAttribs.y );
    payload = vec4(barycentrics, 1.0);
}
#endif
//-----------------------------------------------------------------------------
