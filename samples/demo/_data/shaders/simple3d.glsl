// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------

#ifdef SH_VERT
	void Main ()
	{
		gl.Position		= drawUB.mvp * float4(in_Position.xyz, 1.0);
		
		#ifdef CUBE
		Out.texcoord	= in_Texcoord.xyz;
		Out.normal		= in_Normal.xyz;
		#endif

		#ifdef SPHERICAL_CUBE
		Out.texcoord	= in_Texcoord.xyz;
		Out.normal		= in_Position.xyz;
		#endif
	}
#endif
//-----------------------------------------------------------------------------

#ifdef SH_FRAG
	#include "Math.glsl"

	void Main ()
	{
	#if 0
		out_Color = float4(In.texcoord, 1.0);
	#else
		out_Color = gl.texture.Sample( gl::CombinedTexCube<float>( un_ColorTexture, un_ColorTexture_sampler ), In.texcoord );
	#endif
	}
#endif
