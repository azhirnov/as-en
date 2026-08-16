// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		gl.Position		= float4( float2x2(drawUB.rotation0, drawUB.rotation1) * in_Position, 0.0, 1.0 );
		Out.uv_scale	= float3( in_UV_Scale.xy, in_UV_Scale.z * 4.0 );
		Out.color		= in_Color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"

	layout(location=0) out float4  out_Color;


	float  ApplyStyle (float2 uv, float sd, float2 texDim)
	{
		float3	thick = float3(-0.5, 0.0, 1.5);
		sd = AA_Font( uv, sd, thick, texDim ).x;
		return sd;
	}

	void Main ()
	{
		float3	msd		= gl.texture.Sample( un_Texture, In.uv_scale.xy ).rgb;
		float2	dim		= float2(gl.texture.GetSize( un_Texture, 0 ));

		float	sd		= MCSDF_Median( msd );
				sd		= FusedMulAdd( sd, drawUB.sdfScale, drawUB.sdfBias );	// front packed distance to correct SDF
				sd		= ApplyStyle( In.uv_scale.xy, sd, dim );

		out_Color = Lerp( drawUB.bgColor, In.color, sd );
	}

#endif
//-----------------------------------------------------------------------------
