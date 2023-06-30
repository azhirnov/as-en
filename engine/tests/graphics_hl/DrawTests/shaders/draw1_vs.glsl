
layout(location=0) out vec4  v_Color;
layout(location=1) out vec2  v_UV;

void Main ()
{
    gl_Position = vec4( in_Position, 0.0, 1.0 );
    v_UV        = in_UV;
    v_Color     = in_Color;
}
