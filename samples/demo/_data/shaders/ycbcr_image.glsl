// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------

#ifdef SH_VERT
	layout(location=0) out float2  out_UV;

	void  Main ()
	{
		float2	uv = float2( gl.VertexIndex>>1, gl.VertexIndex&1 ) * 2.0;

		gl.Position	= float4( uv * 2.0 - 1.0, 0.0, 1.0 );
		out_UV		= uv;
	}
#endif
//-----------------------------------------------------------------------------

#ifdef SH_FRAG
	layout(location=0) in  float2  in_UV;
	layout(location=0) out float4  out_Color;

	void  Main ()
	{
		out_Color = gl.texture.Sample( un_YcbcrTexture, in_UV );
	}
#endif
