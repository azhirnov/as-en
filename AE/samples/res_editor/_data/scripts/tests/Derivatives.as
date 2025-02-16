// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Compare dFdxFine and dFdxCoarse.
	Emulate dFd* in compute shader.
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
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/4 );		rt.Name( "RT" );
		RC<DynamicUInt>	cs		= DynamicUInt();
		RC<DynamicUInt>	mode	= DynamicUInt();

		Slider( cs,		"CS",	0,	1 );
		Slider( mode,	"Mode",	0,	2 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Constant( "iMode",		mode );
			pass.EnableIfEqual( cs, 0 );
		}{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_Image",	rt );
			pass.Constant( "iMode",		mode );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.EnableIfEqual( cs, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float	h		= DHash12( GetGlobalCoord().xy );
		float	dx0_h	= gl.dFdxFine( h );
		float	dy0_h	= gl.dFdyFine( h );
		float	dx1_h	= gl.dFdxCoarse( h );
		float	dy1_h	= gl.dFdyCoarse( h );
		float	dx2_h	= QuadGroup_dFdxFine( h );
		float	dy2_h	= QuadGroup_dFdyFine( h );

		out_Color = float4(1.0);

		switch ( iMode )
		{
			case 0 :
			{
				out_Color.rg = ToUNorm( float2( dx0_h, dy0_h ));
				break;
			}
			case 1 :
			{
				out_Color.rg = ToUNorm( float2( dx1_h, dy1_h ));
				break;
			}
			case 2 :
			{
				out_Color.rg = ToUNorm( float2( dx2_h, dy2_h ));
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		int2	pos		= GetGlobalCoordQuadCorrected().xy;

		float4	col		= float4(1.0);
		float	h		= DHash12( pos.xy );

		float	dx1_h	= QuadGroup_dFdxCoarse( h );
		float	dy1_h	= QuadGroup_dFdyCoarse( h );
		float	dx2_h	= QuadGroup_dFdxFine( h );
		float	dy2_h	= QuadGroup_dFdyFine( h );

		switch ( iMode )
		{
			case 0 :
			{
				col.rg = ToUNorm( float2( dx2_h, dy2_h ));
				break;
			}
			case 1 :
			{
				col.rg = ToUNorm( float2( dx1_h, dy1_h ));
				break;
			}
			case 2 :
			{
				col.rg = ToUNorm( float2( dx2_h, dy2_h ));
				break;
			}
		}

		gl.image.Store( un_Image, pos, col );
	}

#endif
//-----------------------------------------------------------------------------
