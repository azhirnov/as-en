// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

//layout(set=0, binding=0, rgba8) writeonly uniform image2D  un_OutImage;
//layout(set=0, binding=1) uniform texture2D un_Textures [1000];
//uniform sampler un_Sampler;

void Main ()
{
	uint	tex_id	= (gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * 1000) % pc.texCount;
	vec2	uv		= vec2(gl_GlobalInvocationID.xy) * 0.01;
	vec4	color	= texture( sampler2D( un_Textures[nonuniformEXT(tex_id)], un_Sampler ), uv );

	imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), color );
}
