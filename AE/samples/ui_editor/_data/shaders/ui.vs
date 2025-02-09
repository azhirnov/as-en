// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	void Main ()
	{
		gl.Position	= float4( FusedMulAdd( in_Position, globalUB.posScale, globalUB.posBias ), 0.0, 1.0 );
		Out.uv		= in_UV;
		Out.color	= in_Color;
	}

#endif
//-----------------------------------------------------------------------------
