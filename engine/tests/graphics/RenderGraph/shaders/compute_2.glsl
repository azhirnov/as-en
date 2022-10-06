
void main ()
{
	ivec2	coord = ivec2(gl_GlobalInvocationID.xy);
	vec4	color = imageLoad( un_Image, coord );

	imageStore( un_Image, coord, 1.0f - color );
}
