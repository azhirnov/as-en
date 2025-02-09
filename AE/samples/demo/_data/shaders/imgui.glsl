// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		float3x2 m	= float3x2( imguiUB.transform_c0, imguiUB.transform_c1, imguiUB.transform_c2 );
		gl.Position	= float4(m * float3(in_Position, 1.0), 0.0, 1.0 );
		Out.uv		= in_UV;
		Out.color	= in_Color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	layout(location=0) out float4  out_Color;

	void Main ()
	{
		out_Color = In.color * gl.texture.Sample( un_Texture, In.uv );
	}

#endif
//-----------------------------------------------------------------------------
