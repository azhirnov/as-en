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
	#include "InvocationID.glsl"


	void  Main ()
	{
		float2		uv		= GetGlobalCoordUNorm().xy;
		float		y		= float_nan;
		const float	x		= uv.x;
		const float	y_max	= 42.0;
		const int	i		= int(uv.y * y_max);

		switch ( i )
		{
			case 0 :	y = Saturate( x * 4.0 - 2.0 );  break;
			case 1 :	y = Cbrt( x );  break;
			case 2 :	y = ToUNorm( x );  break;
			case 3 :	y = ToSNorm( x );  break;
			case 4 :	y = Square( x );  break;
			case 5 :	y = Select( x < 0.5, x+1.0, -x );  break;
			case 6 :	y = SelectF( x, 0.5, x+1.0, -x );  break;
			case 7 :	y = BranchLess( x < 0.5, x+1.0, -x );  break;
			case 8 :	y = MinAbs( x, -(x+1.0) );  break;
			case 9 :	y = MaxAbs( x, -(x+1.0) );  break;

			case 10 :	y = GreaterF( x, 0.5 );  break;
			case 11 :	y = LessF( x, 0.5 );  break;
			case 12 :	y = Sign( x - 0.5 );  break;
			case 13 :	y = LinearStep( x, 0.2, 0.8 );  break;
			case 14 :	y = BumpStep( x, 0.2, 0.8 );  break;
			case 15 :	y = SmoothBumpStep( x, 0.2, 0.8 );  break;
			case 16 :	y = TriangleWave( x );  break;
			case 17 :	y = Steps( x, 8.0 ).x;  break;
			case 18 :	y = Steps( x, 8.0 ).y;  break;
			case 19 :	y = FloorToBase( x*10.0, 2.0 );  break;

			case 20 :	y = RoundToBase( x*10.0, 3.0 );  break;
			case 21 :	y = FloorToPOT( x*128.0 );  break;
			case 22 :	y = RoundToPOT( x*128.0 );  break;
			case 23 :	y = Clamp( x, 0.3, 0.7 );  break;
			case 24 :	y = ClampOut( x, 0.3, 0.7 );  break;
			case 25 :	y = Wrap( x, 0.3, 0.7 );  break;
			case 26 :	y = MirroredWrap( x, 0.3, 0.7 );  break;
			case 27 :	y = BaryLerp( 0.0, 1.0, 2.0, float2(x) );  break;
			case 28 :	y = BaryLerp( 0.0, 1.0, 2.0, float3(x) );  break;
			case 29 :	y = BiLerp( 0.0, 1.0, 2.0, 3.0, float2(x) );  break;

			case 30 :	y = BiCubic( 0.0, 1.0, 2.0, 3.0, float2(x) );  break;
			case 31 :	y = RemapDst( x, float2(0.2, 0.4) );  break;
			case 32 :	y = RemapSrc( float2(0.2, 0.4), x );  break;
			case 33 :	y = Remap( float2(0.2, 0.7), float2(0.1, 1.4), x );  break;
			case 34 :	y = RemapClamp( float2(0.2, 0.7), float2(0.1, 1.4), x );  break;
			case 35 :	y = RemapWrap( float2(0.2, 0.7), float2(0.1, 1.4), x );  break;
			case 36 :	y = RemapWrapMirror( float2(0.2, 0.7), float2(0.1, 1.4), x );  break;
			case 37 :	y = IsUNorm( x*2.0 - 0.5 ) ? 1.0 : float_nan;  break;
			case 38 :	y = IsSNorm( x*4.0 - 1.0 ) ? 1.0 : float_nan;  break;
			case 39 :	y = FpEqual( x, x, float_epsilon ) ? 1.0 : float_nan;  break;

			case 40 :	y = FpEqual( x, x+0.1, float_epsilon ) ? float_nan : 1.0;  break;
			case 41 :	{ float a = x; Swap( a, y );  break; }
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
