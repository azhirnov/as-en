// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

layout(location=0) out vec3  v_Color;

void Main ()
{
	gl_Position	= vec4( in_Position, 0.0, 1.0 );
	v_Color		= in_Color.rgb;

	#ifdef VIEWPORT_ARRAY
		gl_ViewportIndex = gl_InstanceIndex;
		if ( gl_ViewportIndex == 1 )
			v_Color = clamp( 1.0 - v_Color, 0.0, 1.0 );
	#endif
	#ifdef MULTI_VIEW
		if ( gl_ViewIndex == 1 )
		{
			gl_Position.xy += 0.1;
			v_Color = clamp( 1.0 - v_Color, 0.0, 1.0 );
		}
	#endif
}
