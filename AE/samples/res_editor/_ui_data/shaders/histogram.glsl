// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#	define SH_VERT
#	define SH_FRAG
#	include <aestyle.glsl.h>
#	define COUNT	100u
	const struct {
		uint	maxRGB;
		uint	maxLuma;
		uint4	RGBLuma [COUNT];
	} un_Histogram {};
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	layout(location=0) out float4  v_Color;

	const float3  c_ChannelColor[4] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.5f, 0.5f, 0.5f}
	};

	void Main ()
	{
		const int		index	= (gl.VertexIndex / 2) % COUNT;
		const int		channel = ((gl.VertexIndex / 2) / COUNT) & 3;	// R, G, B, Luma
		const float		y		= float(un_Histogram.RGBLuma[index][channel]) / float(channel < 3 ? un_Histogram.maxRGB : un_Histogram.maxLuma);
		const float2	pos		= float2( float(index) / float(COUNT),
										  Saturate( 1.0f - ((gl.VertexIndex & 1) == 0 ? y : 0.0f) ));
		const float		u		= (gl.VertexIndex & 1) == 0 ? 1.0f : 1.0f - y;

		gl.Position = float4( ToSNorm(pos), 0.0f, 1.0f );
		v_Color		= float4( c_ChannelColor[channel], u );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	layout(location=0) in  float4  v_Color;
	layout(location=0) out float4  out_Color;

	void Main ()
	{
		out_Color = float4( v_Color.rgb, SmoothStep( v_Color.a, 0.95f, 0.999f ) * 0.75f + 0.25f );
	}

#endif
//-----------------------------------------------------------------------------
