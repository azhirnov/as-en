// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Test result of passing NaN to some math functions.
*/
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
			RC<ComputePass>		pass = ComputePass();

			pass.Slider( "iNaNType",	0,	4 );
			pass.ArgOut( "un_OutImage", rt );

			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#define MODE	0

	#if MODE == 3
		precision mediump float;
	#endif

	#include "InvocationID.glsl"


	#if MODE == 0 or MODE == 3
	#	define ftype	float
	#	define f4type	float4
	#elif MODE == 1
	#	define ftype	half
	#	define f4type	half4
	#elif MODE == 2
	#	define ftype	double
	#	define f4type	double4
	#endif

	void  Main ()
	{
		float4		col		= float4(0.0);
		ftype		nan;
		const bool	left	= (GetGroupCoordUNorm().x < 0.5);
		const int	row0	= int(GetGlobalCoordUNorm().y * 8.0);
		const int	row1	= int(GetGlobalCoordUNorm().y * 8.0 + 0.02);

		switch ( iNaNType )
		{
		#if MODE == 0 or MODE == 3
			case 0 :	nan = float_qnan;		break;
			case 1 :	nan = float_snan;		break;
			case 2 :	nan = float_inf;		break;
			case 3 :	nan = float_max * 2.f;	break;
			case 4 :	nan = float_max;		break;

		#elif MODE == 1
			case 0 :	nan = half_nan;			break;
			case 1 :	nan = half_nan;			break;
			case 2 :	nan = half_inf;			break;
			case 3 :	nan = half_max * 2.hf;	break;
			case 4 :	nan = half_max;			break;

		#elif MODE == 2
			case 0 :	nan = double_qnan;		break;
			case 1 :	nan = double_snan;		break;
			case 2 :	nan = double_inf;		break;
			case 3 :	nan = double_max * 2.lf;break;
			case 4 :	nan = double_max;		break;
		#endif
		}

		switch ( row0 )
		{
			// specs: Which operand is the result is undefined if one of the operands is a NaN.
			case 0 :	col = float4( left ? Min( nan, ftype(0.5) ) : Min( ftype(1.0), nan ));									break;

			// specs: Which operand is the result is undefined if one of the operands is a NaN.
			case 1 :	col = float4( left ? Max( nan, ftype(0.5) ) : Max( ftype(1.0), nan ));									break;

			case 2 :	col = float4( Saturate( nan ));																			break;

		//	case 3 :	col = float4( left ? Clamp( ftype(0.5), nan, ftype(1.0) ) : Clamp( ftype(0.5), ftype(1.0), nan ));		break;
			case 3 :	col = float4( left ? IsNaN(nan) : IsInfinity(nan));														break;

			case 4 :	col = float4( left ? (bool(nan) ? ftype(1.0) : ftype(0.5)) : (nan != nan ? ftype(1.0) : ftype(0.0)) );	break;
			case 5 :	col = float4( left ? Step(ftype(0.0), nan)  : ToUNorm(SignOrZero(nan)) );								break;
			case 6 :	col = float4( left ? Step(ftype(0.0), -nan) : ToUNorm(SignOrZero(-nan)) );								break;
			case 7 :	col = float4( SmoothStep( nan, ftype(0.0), ftype(1.0) ));												break;
		}

		if ( Any(IsNaN( col )) )
			col = float4(1.0, 0.0, 0.0, 1.0);
		else
		if ( Any(IsInfinity( col )) )
			col = float4(1.0, 0.3, 0.2, 1.0);
		else
		if ( AnyNotEqual( col, col ))
			col = float4(0.9, 0.7, 0.2, 1.0);
		else
			col *= float4(0.0, 0.75, 0.0, 1.0);

		if ( row0 != row1 )
			col = float4(0.0, 0.0, 1.0, 1.0);

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
