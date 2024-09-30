// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Texture sampling.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


ND_ float4  TriplanarMapping (const float3 uvw, float3 dir, gl::CombinedTex2D<float> samp);

ND_ float4  TexFilter (gl::CombinedTex2D<float> tex, float2 uv);
//-----------------------------------------------------------------------------



float4  TriplanarMapping (const float3 uvw, float3 dir, gl::CombinedTex2D<float> samp)
{
	float4  a = gl.texture.Sample( samp, uvw.zy );
	float4  b = gl.texture.Sample( samp, uvw.xz );
	float4  c = gl.texture.Sample( samp, uvw.xy );
	dir = Abs(Normalize( dir ));
	return (a * dir.x) + (b * dir.y) + (c * dir.z);
}

/*
=================================================
	TexFilter
----
	from https://www.shadertoy.com/view/XsfGDn
	The MIT License
	Copyright © 2013 Inigo Quilez
=================================================
*/
#ifdef AE_LICENSE_MIT
	float4  TexFilter (gl::CombinedTex2D<float> tex, float2 uv)
	{
		float2	dim = float2(gl.texture.GetSize( tex, 0 ));
		uv = uv * dim + 0.5;

		float2	iuv = Floor( uv );
		float2	fuv = uv - iuv;

		uv = iuv + fuv*fuv * (3.0 - 2.0 * fuv);
		uv = (uv - 0.5) / dim;

		return gl.texture.Sample( tex, uv );
	}
#endif
