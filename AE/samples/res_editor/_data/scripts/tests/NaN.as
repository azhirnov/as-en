// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Test result of passing NaN to some math functions.
*/
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
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
	#include "GlobalIndex.glsl"

	void  Main ()
	{
		float4		col		= float4(0.0);
		float		nan;
		const bool	right	= (GetGroupCoordUNorm().x > 0.5);

		switch ( iNaNType )
		{
			case 0 :	nan = float_qnan;		break;
			case 1 :	nan = float_snan;		break;
			case 2 :	nan = float_inf;		break;
			case 3 :	nan = float_max * 2.f;	break;
			case 4 :	nan = float_max;		break;
		}

		switch ( int(GetGroupCoordUNorm().y * 7.0) )
		{
			// specs: Which operand is the result is undefined if one of the operands is a NaN.
			case 0 :	col = float4( right ? Min( nan, 0.5f ) : Min( 1.0f, nan ));					break;

			// specs: Which operand is the result is undefined if one of the operands is a NaN.
			case 1 :	col = float4( right ? Max( nan, 0.5f ) : Max( 1.0f, nan ));					break;

			case 2 :	col = float4( Saturate( nan ));												break;
			case 3 :	col = float4( right ? Clamp( 0.5f, nan, 1.f ) : Clamp( 0.5f, 1.f, nan ));	break;
			case 4 :	col = float4( bool(nan) ? 0.5f : 1.f );										break;
			case 5 :	col = float4( right ? Sign(nan) : SignOrZero(nan) );						break;
			case 6 :	col = float4( SmoothStep( nan, 0.f, 1.f ));									break;
		}

		if ( Any(IsNaN( col )) or Any(IsInfinity( col )) )
			col = float4(1.0, 0.0, 0.0, 1.0);
		else
			col *= float4(0.0, 0.75, 0.0, 1.0);

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
