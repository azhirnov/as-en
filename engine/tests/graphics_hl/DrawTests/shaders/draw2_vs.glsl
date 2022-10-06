
layout(location=0) out vec4  v_Color;

void main ()
{
	gl_Position	= vec4( in_Position, 0.0, 1.0 );
	v_Color		= in_Color;
}
