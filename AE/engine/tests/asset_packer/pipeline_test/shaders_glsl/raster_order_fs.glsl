
#include "common.glsl"

void Main ()
{
	vec4	col = subpassLoad( in_Color );

	out_Color = texture( sampler2D(un_ColorTexture, un_ColorTexture_sampler), In.Texcoord ) + col;
}
