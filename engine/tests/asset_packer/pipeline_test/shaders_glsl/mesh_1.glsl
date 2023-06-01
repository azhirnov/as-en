#include "common.glsl"

const vec2	g_Positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

const vec3	g_Colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

void Main ()
{
	const uint I = gl_LocalInvocationID.x;

	gl_MeshVerticesEXT[I].gl_Position	= vec4( g_Positions[I], 0.0, 1.0 );
	Out[I].texcoord						= g_Positions[I];
	Out[I].color						= vec4( g_Colors[I], 1.0 );
	
	if ( I == 0 )
	{
		gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0,1,2);
		SetMeshOutputsEXT( 3, 1 );
	}
}
