#ifdef __INTELLISENSE__
#	define SH_VERT
#	define SH_FRAG
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------

#ifdef SH_VERT
	layout(location=0) out float2  out_UV;

	void Main ()
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
		out_Color = float4(0.0, 0.0, 0.0, 1.0);
		out_Color.rgb += float3(100.0,  0.0,  0.0) * (in_UV.y > 0.0 && in_UV.y < 0.1 ? 1.0 : 0.0);
		out_Color.rgb += float3( 0.0, 100.0,  0.0) * (in_UV.y > 0.1 && in_UV.y < 0.2 ? 1.0 : 0.0);
		out_Color.rgb += float3( 0.0,  0.0, 100.0) * (in_UV.y > 0.2 && in_UV.y < 0.3 ? 1.0 : 0.0);
		/*
		out_Color.rgb += float3(1000.0,    0.0,    0.0) * (in_UV.y > 0.4 && in_UV.y < 0.5 ? 1.0 : 0.0);
		out_Color.rgb += float3(   0.0, 1000.0,    0.0) * (in_UV.y > 0.5 && in_UV.y < 0.6 ? 1.0 : 0.0);
		out_Color.rgb += float3(   0.0,    0.0, 1000.0) * (in_UV.y > 0.6 && in_UV.y < 0.7 ? 1.0 : 0.0);
		out_Color.rgb += float3(1000.0,    0.0, 1000.0) * (in_UV.y > 0.7 && in_UV.y < 0.8 ? 1.0 : 0.0);
		out_Color.rgb += float3(1000.0, 1000.0,    0.0) * (in_UV.y > 0.8 && in_UV.y < 0.9 ? 1.0 : 0.0);
		out_Color.rgb += float3(1000.0, 1000.0, 1000.0) * (in_UV.y > 0.9 && in_UV.y < 1.0 ? 1.0 : 0.0);
		*/
		//out_Color.rgb += float3(10000.0, 0.0, 0.0) * (in_UV.y > 0.9 && in_UV.y < 1.0 ? 1.0 : 0.0);
		out_Color.rgb *= in_UV.x;
	}
#endif