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
	#include "Ray.glsl"
	#include "Normal.glsl"
	#include "InvocationID.glsl"

	float  SDF2 (float2 pos)
	{
		return SDF2_Star5( pos, 0.6, 0.4 );
	}

	float  SDF (float3 pos, uint idx, float2 uv)
	{
		pos = SDF_Move( pos, float3(0.0, 0.0, 4.0) );
		pos = SDF_Rotate( pos, QRotationX( ToRad(30.0) ));
		pos = SDF_Rotate( pos, QRotationY( ToRad(30.0) ));

		float	d0 = SDF_Box( pos, float3(0.3, 0.7, 1.1) );
		float	d1 = SDF_Sphere( pos, 1.0 );
		float	d  = SDF_OpUnite( d0, d1 );

		switch ( idx )
		{
			case 0 :	return SDF_OpRoundedShape( d, 0.2 );
			case 1 :	return SDF_OpAnnularShape( d, 0.2 );
			case 2 :	return SDF_OpExtrusion( pos.z, SDF2( pos.xy ), 1.0 );
			case 3 :	return SDF_OpRevolution( pos, SDF2, 0.2 );
		}
		return 1.0;
	}


	void  Main ()
	{
		const float2	scale	= float2(3.0,2.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const uint		idx		= uint(uv.x * scale.x) + uint(uv.y * scale.y) * uint(scale.x);
		const float2	uv2		= ToSNorm( Fract( uv * scale ));
		const float		min_d	= 0.001;

		Ray		ray = Ray_Perspective( float3(0.0), ToRad(45), un_PerPass.resolution.x/un_PerPass.resolution.y, 0.1, uv2 );
		float	md = float_max;

		for (uint i = 0; i < 64; ++i)
		{
			float	d = SDF( ray.pos, idx, uv2 );

			md = Min( md, d );
			Ray_Move( INOUT ray, d );

			if ( Abs(d) < min_d )
				break;
		}

		const float3	normal = ComputeNormalInWS_dxdy( ray.pos ) * float3(1.0, 1.0, -1.0);

		if ( md < min_d )
			out_Color.rgb = normal;
		else
			out_Color.rgb = SDF_Isolines( md * 20.0 );

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
