
#include "common.glsl"

void Main ()
{
    imageAtomicAdd( un_Image, ivec2(gl_GlobalInvocationID.xy), uint(gl_LocalInvocationID.x) );
}
