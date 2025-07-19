// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	for screenshot test
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
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Easing.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		float2		uv		= GetGlobalCoordUNorm().xy;
		float		y		= float_nan;
		const float	x		= uv.x;
		const float	y_max	= 48.0;
		const int	i		= int(uv.y * y_max);

		switch ( i )
		{
			case 0 :	y = QuadraticEaseIn( x );  break;
			case 1 :	y = QuadraticEaseOut( x );  break;
			case 2 :	y = QuadraticEaseInOut( x );  break;

			case 3 :	y = CubicEaseIn( x );  break;
			case 4 :	y = CubicEaseOut( x );  break;
			case 5 :	y = CubicEaseInOut( x );  break;

			case 6 :	y = QuarticEaseIn( x );  break;
			case 7 :	y = QuarticEaseOut( x );  break;
			case 8 :	y = QuarticEaseInOut( x );  break;

			case 9 :	y = QuinticEaseIn( x );  break;
			case 10 :	y = QuinticEaseOut( x );  break;
			case 11 :	y = QuinticEaseInOut( x );  break;

			case 12 :	y = SineEaseIn( x );  break;
			case 13 :	y = SineEaseOut( x );  break;
			case 14 :	y = SineEaseInOut( x );  break;

			case 15 :	y = CircularEaseIn( x );  break;
			case 16 :	y = CircularEaseOut( x );  break;
			case 17 :	y = CircularEaseInOut( x );  break;

			case 18 :	y = ExponentialEaseIn( x );  break;
			case 19 :	y = ExponentialEaseOut( x );  break;
			case 20 :	y = ExponentialEaseInOut( x );  break;

			case 21 :	y = ElasticEaseIn( x );  break;
			case 22 :	y = ElasticEaseOut( x );  break;
			case 23 :	y = ElasticEaseInOut( x );  break;

			case 24 :	y = HermiteEaseIn( x );  break;
			case 25 :	y = HermiteEaseOut( x );  break;
			case 26 :	y = HermiteEaseInOut( x );  break;

			case 27 :	y = LogarithmicEaseIn( x );  break;
			case 28 :	y = LogarithmicEaseOut( x );  break;
		//	case 29 :	y = LogarithmicEaseInOut( x );  break;

			case 30 :	y = Logarithmic2EaseIn( x );  break;
			case 31 :	y = Logarithmic2EaseOut( x );  break;
		//	case 32 :	y = Logarithmic2EaseInOut( x );  break;

			case 33 :	y = ReciprocalEaseIn( x );  break;
			case 34 :	y = ReciprocalEaseOut( x );  break;
		//	case 35 :	y = ReciprocalEaseInOut( x );  break;

			case 36 :	y = ExponentialE_EaseIn( x );  break;
			case 37 :	y = ExponentialE_EaseOut( x );  break;
		//	case 38 :	y = ExponentialE_EaseInOut( x );  break;

			case 39 :	y = ReciprocalSquaredEaseIn( x );  break;
			case 40 :	y = ReciprocalSquaredEaseOut( x );  break;
		//	case 41 :	y = ReciprocalSquaredEaseInOut( x );  break;

			case 42 :	y = SquareRootEaseIn( x );  break;
			case 43 :	y = SquareRootEaseOut( x );  break;
		//	case 44 :	y = SquareRootEaseInOut( x );  break;

			case 45 :	y = CubicRootEaseIn( x );  break;
			case 46 :	y = CubicRootEaseOut( x );  break;
			case 47 :	y = 0.0;  break; //y = CubicRootEaseInOut( x );  break;
		}

		if ( i != int(GetGlobalCoordUNorm(int3(1)).y * y_max) )
		{
			out_Color = float4(0.0);
			return;
		}

		if ( IsNaN( y ) or IsInfinity( y ))
			out_Color = float4(0.2);
		else
			out_Color = RainbowWrap( y );
	}

#endif
//-----------------------------------------------------------------------------
