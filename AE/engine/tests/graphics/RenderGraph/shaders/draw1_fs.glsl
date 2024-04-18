// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

layout(location=0) in  vec3  v_Color;
layout(location=0) out vec4  out_Color;

void dbg_EnableTraceRecording (bool b) {}

void Main ()
{
	dbg_EnableTraceRecording( int(gl_FragCoord.x) == 400 && int(gl_FragCoord.y) == 300 );

	out_Color.rgb = v_Color.rgb;
	out_Color.a   = fract(v_Color.r + v_Color.g + v_Color.b + 0.5f);
}
