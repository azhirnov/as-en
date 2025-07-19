// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#	define SH_VERT
#	define SH_FRAG
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		gl.Position	= FullscreenTrianglePos();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Matrix.glsl"

	void Main ()
	{
		float	depth	= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;
				depth	= FastUnProjectZ( pc.proj, depth );
				depth	= (depth - pc.nearPlane) * pc.invDistance;
		out_Color		= float4(depth);
	}

#endif
//-----------------------------------------------------------------------------
