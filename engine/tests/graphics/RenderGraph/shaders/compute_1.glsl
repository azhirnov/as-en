
//layout(set=0, binding=0, rgba8) writeonly uniform image2D  un_OutImage;

void main ()
{
	vec4 color = vec4(float(gl_LocalInvocationID.x) / float(gl_WorkGroupSize.x),
					  float(gl_LocalInvocationID.y) / float(gl_WorkGroupSize.y),
					  1.0, 0.0);

	imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), color );
}
