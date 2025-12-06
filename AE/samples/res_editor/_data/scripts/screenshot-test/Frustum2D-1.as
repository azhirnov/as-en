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
		RC<Image>		rt		= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<FPVCamera>	camera	= FPVCamera();

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iScale",		0,	8,	2 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		float4		col		= float4(1.0);
		const float	x_max	= 4.0;
		const float	y_max	= 2.0;
		const int2	pos		= int2(GetGlobalCoordUNorm().xy * float2(x_max,y_max));
		const float	scale	= Exp10( float(iScale) );

		float	t  = 0.7345;
		float2	p0 = float2(-1.0,  1.0) * Abs(float2(Sin( t ), Cos( t )));
		float2	p1 = float2( 1.0,  1.0) * Abs(float2(Cos( t ), Sin( t )));	t += 0.835;
		float2	p2 = float2(-1.0, -1.0) * Abs(float2(Sin( t ), Cos( t )));	t += 0.543;
		float2	p3 = float2( 1.0, -1.0) * Abs(float2(Cos( t ), Sin( t )));

		Frustum2d	ref_fr	= Frustum2d_FromCornerPoints( p0, p1, p2, p3 );

		float2		ref_corners[4];
		Frustum2d_ToCornerPoints( ref_fr, OUT ref_corners );

		Frustum2d	test_fr	= Frustum2d_FromCornerPoints( ref_corners );

		float2		test_corners[4];
		Frustum2d_ToCornerPoints( test_fr, OUT test_corners );

		if ( pos.y == 0 )
		{
			int	i = pos.x;
			col.rgb = Abs( ref_fr.planes[i] - test_fr.planes[i] ) * scale;
		}
		else
		{
			int	i = pos.x;
			col.rg = Abs( ref_corners[i] - test_corners[i] ) * scale / Max( Abs(ref_corners[i]), 0.001 );
			col.b  = 0.0;
		}

		if ( Any(IsNaN( col.rgb )) or Any(IsInfinity( col.rgb )))
		{
			out_Color = float4(0.2);
			return;
		}

		if ( AllNotEqual( pos, int2(GetGlobalCoordUNorm().xy * float2(x_max,y_max)) ))
		{
			out_Color = float4(0.0);
			return;
		}

		col.a = 1.0;
		out_Color = col;
	}

#endif
//-----------------------------------------------------------------------------
