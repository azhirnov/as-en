
#include "common.glsl"

void Main ()
{
    float4 fragColor = float4(float(gl_LocalInvocationID.x) / float(gl_WorkGroupSize.x),
                              float(gl_LocalInvocationID.y) / float(gl_WorkGroupSize.y),
                              1.0, 0.0);

    imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), fragColor );
}
