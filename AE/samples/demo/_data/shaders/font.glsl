// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		gl.Position	= float4( in_Position, 0.0, 1.0 );
		Out.uv		= in_UV_Scale.xy;
		Out.color	= in_Color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	layout(location=0) out float4  out_Color;

	void Main ()
	{
		out_Color = In.color * gl.texture.Sample( un_Texture, In.uv ).r;
	}

#endif
//-----------------------------------------------------------------------------
