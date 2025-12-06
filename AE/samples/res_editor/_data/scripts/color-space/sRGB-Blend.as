// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Difference between linear space blending and sRGB space blending.

	top		- sRGB space blending (incorrect)
	bottom	- linear space blending (gamma correct)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.ColorSelector( "iColorA",	RGBA32f(1.f, 0.f, 0.f, 1.f) );
			pass.ColorSelector( "iColorB",	RGBA32f(0.f, 1.f, 0.f, 1.f) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Blend.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	void Main ()
	{
		float	x			= GetGlobalCoordUNorm().x;
		bool	use_sRGB	= GetGlobalCoordSNorm().y > 0.f;

		float4	col_a;
		float4	col_b;

		if ( use_sRGB ){
			col_a = RemoveSRGBCurve( iColorA );	// in linear space
			col_b = RemoveSRGBCurve( iColorB );
		}else{
			col_a = iColorA;					// in gamma (sRGB) space
			col_b = iColorB;
		}

		col_a.a	*= 1.0 - LinearStep( x, 0.25, 0.75 );
		col_b.a *= LinearStep( x, 0.25, 0.75 );

		BlendParams	p;
		p.srcColor	= col_a;
		p.dstColor	= col_b;
		p.srcBlend	= EBlendFactor_SrcAlpha;
		p.dstBlend	= EBlendFactor_OneMinusSrcAlpha;
		p.blendOp	= EBlendOp_Add;

		out_Color = BlendFn( p );

		if ( use_sRGB )
			out_Color = ApplySRGBCurve( out_Color );	// to sRGB space

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
