
#include "common.glsl"

//layout(location=0) in  float2  in_Texcoord;

layout(location=0) out float4  out_Color;

void Main ()
{
	out_Color = texture( sampler2D(un_ColorTexture, un_ColorTexture_sampler), In.texcoord ) * In.color;
}
