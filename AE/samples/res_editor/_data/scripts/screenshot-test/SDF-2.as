// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color", rt );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const float2	scale	= float2(3.0,2.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const uint		idx		= uint(uv.x * scale.x) + uint(uv.y * scale.y) * uint(scale.x);
		const float2	pos		= ToSNorm( Fract( uv * scale ));
		const float2	duv2	= scale / float2(GetGlobalSize().xy);

		float	md = float_max;

		switch ( idx )
		{
			case 0 :	md = SDF2_Triangle( pos, float2(0.0, 0.7), float2(0.8, 0.3), float2(-0.7, -0.7) );				break;
			case 1 :	md = SDF2_Pentagon( pos, 0.4 );																	break;
			case 2 :	md = SDF2_Hexagon( pos, 0.4 );																	break;
			case 3 :	md = SDF2_Octagon( pos, 0.4 );																	break;
			case 4 :	md = SDF2_QuadraticBezier( pos, float2(-0.7,-0.7), float2(0.0, 0.8), float2(0.7, -0.2) ) - 0.1;	break;
			case 5 :	md = SDF2_Trapezeoid( pos, 0.2, 0.4, 0.5 );														break;
		}

		out_Color.rgb = SDF_Isolines( md * 10.0 );
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
