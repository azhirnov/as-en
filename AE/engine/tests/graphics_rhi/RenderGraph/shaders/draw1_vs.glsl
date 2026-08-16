// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

layout(location=0) out vec3  v_Color;

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

void dbg_EnableTraceRecording (bool b) {}

void Main ()
{
	dbg_EnableTraceRecording( gl_VertexIndex == 1 or gl_VertexIndex == 2 );

	gl_Position	= vec4( g_Positions[gl_VertexIndex], 0.0, 1.0 );
	v_Color		= g_Colors[gl_VertexIndex];

	#ifdef PER_INSTANCE_OFFSET
		gl_Position.x += (float(gl_InstanceIndex) - 1.5) * 0.5;		// [1..2] --> [-0.25, +0.25]
	#endif
}
