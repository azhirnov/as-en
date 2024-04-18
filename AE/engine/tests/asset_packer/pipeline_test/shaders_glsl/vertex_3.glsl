
//layout(location=0) in  float3  in_Position;
//layout(location=1) in  float2  in_Texcoord;

//layout(location=0) out vec2  out_Texcoord;


void Main ()
{
	gl_Position  = vec4(in_Position, 1.0);
	Out.Texcoord = in_Texcoord;
}
