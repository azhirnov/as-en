// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#   define AE_RAY_QUERY
#   include <aestyle.glsl.h>
    gl::AccelerationStructure   un_RtScene;
    gl::Image2D<float>          un_OutImage;
#endif
//-----------------------------------------------------------------------------

void Main ()
{
    const float2 uv         = (float2(gl.GlobalInvocationID.xy) + 0.5f) / float2(gl.WorkGroupSize.xy * gl.NumWorkGroups.xy);
    const float3 origin     = float3(uv.x, 1.0f - uv.y, -1.0f);
    const float3 direction  = float3(0.0f, 0.0f, 1.0f);

    gl::RayQuery ray_query;
    gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::None,
                           /*cullMask*/0xFF, origin, /*Tmin*/0.0f, direction, /*Tmax*/10.0f );

    while ( gl.rayQuery.Proceed( ray_query ))
    {
        if ( gl::RayQueryCandidateIntersection(gl.rayQuery.GetIntersectionType( ray_query, false )) == gl::RayQueryCandidateIntersection::Triangle )
            gl.rayQuery.ConfirmIntersection( ray_query );
    }

    float4  color;

    if ( gl::RayQueryCommittedIntersection(gl.rayQuery.GetIntersectionType( ray_query, true )) == gl::RayQueryCommittedIntersection::None )
    {
        // miss shader
        color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        // hit shader
        float2  attribs      = gl.rayQuery.GetIntersectionBarycentrics( ray_query, true );
        float3  barycentrics = float3( 1.0f - attribs.x - attribs.y, attribs.x, attribs.y );
        color = float4(barycentrics, 1.0f);
    }

    gl.image.Store( un_OutImage, int2(gl.GlobalInvocationID.xy), color );
}
