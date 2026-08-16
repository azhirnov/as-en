// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

//layout(descriptor_heap, rgba8) writeonly uniform image2D	un_OutImage [];
//layout(descriptor_heap)		 uniform texture2D			un_Textures [];
//layout(descriptor_heap)		 uniform sampler			un_Samplers [];

void Main ()
{
	uint	tex_id	= (gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * 1000) % pc.Textures_count;
	vec2	uv		= vec2(gl_GlobalInvocationID.xy) * 0.01;
	vec4	color	= texture( sampler2D( un_Textures[pc.Textures_first + tex_id], un_Samplers[0] ), uv );

	imageStore( un_OutImage[pc.OutImage_first], ivec2(gl_GlobalInvocationID.xy), color );
}
