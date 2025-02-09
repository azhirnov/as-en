// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_MESH

	const vec2	g_Positions[3] = vec2[](
		vec2(0.0, -0.5),
		vec2(0.5, 0.5),
		vec2(-0.5, 0.5)
	);

	const vec3	g_Colors[3] = vec3[](
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	);

	void dbg_EnableTraceRecording (bool b) {}

	void Main ()
	{
		const uint I = gl_LocalInvocationID.x;

		dbg_EnableTraceRecording( gl_GlobalInvocationID.x == 0 );

		gl_MeshVerticesEXT[I].gl_Position	= vec4( g_Positions[I], 0.0, 1.0 );
		Out[I].color						= vec4( g_Colors[I], 1.0 );

		if ( I == 0 )
		{
			gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0,1,2);
			SetMeshOutputsEXT( 3, 1 );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	layout(location=0) out vec4  out_Color;

	void Main () {
		out_Color = In.color;
	}

#endif
//-----------------------------------------------------------------------------
