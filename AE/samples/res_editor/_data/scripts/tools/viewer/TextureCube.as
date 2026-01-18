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
		RC<Image>	tex	= Image( EImageType::Float_Cube, "res/tex/perlin-fbm7-cm.aeimg" );

		// render loop
		{
			RC<Postprocess>		pass	= Postprocess();
			pass.Output( "out_Color",	rt );
			pass.ArgIn(	 "un_Texture",	tex,	Sampler_LinearMipmapClamp );
			pass.Slider( "iColScale",	-5,		5,		0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"

	void  Main ()
	{
		float2	uv		= gl.FragCoord.xy * un_PerPass.invResolution;
		float3	dir		= Ray_PlaneTo360( float3(0.0), 0.1, uv ).dir;
		float	scale	= Exp2( float(iColScale) );

		out_Color = gl.texture.Sample( un_Texture, dir ) * scale;
	}

#endif
//-----------------------------------------------------------------------------
