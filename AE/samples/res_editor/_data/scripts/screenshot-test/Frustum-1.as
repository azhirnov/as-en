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

		// setup camera
		{
			camera.ClipPlanes( 0.01f, 10.f );
			camera.FovY( 50.f );

			const float	s = 0.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

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
	#include "Frustum.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		float4		col;
		const float	y_max	= 6.0 + 8.0;
		const int	i		= int(GetGlobalCoordUNorm().y * y_max);
		const float	scale	= Exp10( float(iScale) );

		Frustum		ref_fr	= Frustum_FromMatrix( un_PerPass.camera.viewProj );

		float3		ref_corners[8];
		Frustum_ToCornerPoints( ref_fr, OUT ref_corners );

		Frustum		test_fr	= Frustum_FromCornerPoints( ref_corners );

		float3		test_corners[8];
		Frustum_ToCornerPoints( test_fr, OUT test_corners );

		if ( i < 6 )
		{
			col = Abs( ref_fr.planes[i] - test_fr.planes[i] ) * scale;
		}
		else
		{
			const uint	j = i - 6;

			col.rgb = Abs( ref_corners[j] - test_corners[j] ) * scale / Max( Abs(ref_corners[j]), 0.001 );
			col.a   = 1.0;
		}

		if ( Any(IsNaN( col )) or Any(IsInfinity( col )))
		{
			out_Color = float4(0.2);
			return;
		}

		if ( i != int(GetGlobalCoordUNorm(int3(1)).y * y_max) )
		{
			out_Color = float4(0.0);
			return;
		}

		out_Color = col;
	}

#endif
//-----------------------------------------------------------------------------
