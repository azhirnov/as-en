
#include "common.glsl"

layout(triangles) out;
layout(max_vertices=32, max_primitives=64) out;
/*
out uint3  gl_PrimitiveTriangleIndicesEXT [max_primitives];

out gl_MeshPerVertexEXT {
	vec4	gl_Position;
} gl_MeshVertices[]; // [max_vertices]

out MeshOutput {
	layout(location = 0) vec2	texcoord;
	layout(location = 1) vec4	color;
} Out[]; // [max_vertices]
*/
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
