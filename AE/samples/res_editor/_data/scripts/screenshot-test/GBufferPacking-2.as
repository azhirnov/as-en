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
		RC<Image>	rt = Image( EPixelFormat::RGBA16F, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iScale",		0,	8,	3 );
			pass.Slider( "iView",		0,	2 );		// min, non-inverse, inverse quat
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"
	#include "GBuffer.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		const float	scale	= Exp10( float(iScale) );

		float2		uv		= GetGlobalCoordUNorm().xy;
		float3		dir		= Ray_PlaneToSphereMap360( float3(0.0), 0.1, uv ).dir;
		Quat		q		= QFromEuler( dir * float_Pi );

		uint		enc		= EncodeQuat32( q );
		Quat		q2		= DecodeQuat32( enc );

		float4		col0	= Abs( q.data - q2.data );
		float4		col1	= Abs( q.data + q2.data );	// quaternion may be inversed
		float4		col;

		switch ( iView )
		{
			case 0 :	col = Min( col0, col1 );  break;
			case 1 :	col = col0;  break;
			case 2 :	col = col1;  break;
		}

		if ( Any(IsNaN( col )) or Any(IsInfinity( col )))
		{
			out_Color = float4(0.2);
			return;
		}

		out_Color = col * scale;
	}

#endif
//-----------------------------------------------------------------------------
