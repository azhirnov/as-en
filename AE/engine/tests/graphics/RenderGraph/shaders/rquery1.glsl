
void Main ()
{
    const vec2 uv        = (vec2(gl_GlobalInvocationID.xy) + 0.5) / vec2(gl_WorkGroupSize.xy * gl_NumWorkGroups.xy);
    const vec3 origin    = vec3(uv.x, 1.0 - uv.y, -1.0f);
    const vec3 direction = vec3(0.0f, 0.0f, 1.0f);

    rayQueryEXT ray_query;
    rayQueryInitializeEXT( ray_query, un_RtScene, gl_RayFlagsNoneEXT,
                           /*cullMask*/0xFF, origin, /*Tmin*/0.0f, direction, /*Tmax*/10.0f );

    while ( rayQueryProceedEXT( ray_query ))
    {
        if ( rayQueryGetIntersectionTypeEXT( ray_query, false ) == gl_RayQueryCandidateIntersectionTriangleEXT )
            rayQueryConfirmIntersectionEXT( ray_query );
    }

    vec4    color;

    if ( rayQueryGetIntersectionTypeEXT( ray_query, true ) == gl_RayQueryCommittedIntersectionNoneEXT )
    {
        // miss shader
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // hit shader
        vec2    attribs      = rayQueryGetIntersectionBarycentricsEXT( ray_query, true );
        vec3    barycentrics = vec3( 1.0f - attribs.x - attribs.y, attribs.x, attribs.y );
        color = vec4(barycentrics, 1.0);
    }

    imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), color );
}
