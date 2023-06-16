
layout(location=0) out vec3  v_Color;

void Main ()
{
	gl_Position	= vec4( pc.PositionColorArr[gl_VertexIndex].xy, 0.0, 1.0 );
	v_Color		= unpackUnorm4x8( floatBitsToUint( pc.PositionColorArr[gl_VertexIndex].z )).xyz;
}
