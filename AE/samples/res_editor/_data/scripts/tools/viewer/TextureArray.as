// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	tex	= Image( EImageType::Float_2DArray, "TODO" );

		// render loop
		{
			RC<Postprocess>		pass	= Postprocess();
			pass.Output( "out_Color",	rt );
			pass.ArgIn(	 "un_Texture",	tex,	Sampler_LinearMipmapClamp );
			pass.Slider( "iColScale",	-5,		5,		0 );
			pass.Slider( "iXScale",		0.25,	4.0,	1.0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void  Main ()
	{
		float	layers			= float(gl.texture.GetSize( un_Texture, 0 ).z);
		float	layers_per_row	= Ceil( Sqrt( layers ) * iXScale );
		float	layers_per_col	= Ceil( layers / layers_per_row );
		float2	uv				= gl.FragCoord.xy * un_PerPass.invResolution;
		float2	coord			= uv * float2(layers_per_row, layers_per_col);
		float	scale			= Exp2( float(iColScale) );

		uv	= Fract( coord );
		coord = Floor( coord );

		float	layer	= coord.x + coord.y * layers_per_row;

		out_Color = gl.texture.Sample( un_Texture, float3(uv, layer) ) * scale;
	}

#endif
//-----------------------------------------------------------------------------
