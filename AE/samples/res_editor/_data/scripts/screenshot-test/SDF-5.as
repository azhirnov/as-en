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


	float  SDF (float3 pos, uint idx)
	{
		pos = SDF_Move( pos, float3(0.0, 0.0, 4.0) );
		pos = SDF_Rotate( pos, QRotationX( ToRad(30.0) ));
		pos = SDF_Rotate( pos, QRotationY( ToRad(30.0) ));

		switch ( idx )
		{
			case 0 :	return SDF_Cylinder( pos, float2(0.4, 0.6) );
			case 1 :	return -SDF_InfiniteCone( pos, Normalize(float2( 0.5, 0.7 )) );
			case 2 :	return SDF_Plane( pos, Normalize(float3(0.2, -1.0, 0.0)), 0.1 );
			case 3 :	return SDF_HexagonalPrism( pos, float2(0.5, 0.7) );
			case 4 :	return SDF_TriangularPrism( pos, float2(0.5, 0.7) );
			case 5 :	return SDF_Capsule( pos, float3(-0.5), float3(0.5), 0.5 );
		}
	}


	void  Main ()
	{
		const float2	scale	= float2(3.0,2.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const uint		idx		= uint(uv.x * scale.x) + uint(uv.y * scale.y) * uint(scale.x);
		const float		min_d	= 0.001;

		Ray		ray = Ray_Perspective( float3(0.0), ToRad(45), un_PerPass.resolution.x/un_PerPass.resolution.y, 0.1, ToSNorm( Fract( uv * scale )) );
		float	md = float_max;

		for (uint i = 0; i < 64; ++i)
		{
			float	d = SDF( ray.pos, idx );

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
