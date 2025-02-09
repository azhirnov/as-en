// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef SH_VERT

	layout(location=0) out vec2  out_UV;

	void  Main ()
	{
		vec2	uv	= vec2( gl_VertexIndex>>1, gl_VertexIndex&1 ) * 2.f;
		gl_Position	= vec4( uv * 2.0 - 1.0, 0.0, 1.0 );
		out_UV		= uv;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	layout(location=0) in  vec2  in_UV;
	layout(location=0) out vec4  out_Color;

	void  Main ()
	{
		out_Color = texture( un_YcbcrTexture, in_UV );
	}

#endif
//-----------------------------------------------------------------------------
