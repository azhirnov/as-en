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
		RC<Image>		rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iScale",		0,		10,		6 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		float4		col		= float4(0.0);
		const float	y_max	= 10.0;
		const float	scale	= Exp10( float(iScale) );

		float2		uv		= GetGlobalCoordUNorm().xy;
		const float	y		= (uv.y * y_max);
		const int	i		= int(y);
					uv.y	= Fract( uv.y * y_max );

		switch ( i )
		{
			case 0 :
			{
				float	fov_y	= ToRad(60.0);
				float	ratio	= 1.3;
				float3	dir		= Ray_Perspective( fov_y, ratio, uv );
				float2	uv2		= RayInverse_Perspective( fov_y, ratio, dir );
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 1 :
			{
				float2	fov		= ToRad(float2( 76.0, 64.0 ));
				float3	dir		= Ray_Perspective( fov, uv );
				float2	uv2		= RayInverse_Perspective( fov, dir );
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 2 :
			{
				float	ipd		= 0.1;
				uint	eye		= 0;
				float3	dir		= Ray_PlaneToVR180( ipd, float3(0.0), 0.1, uv, eye ).dir;
				float2	uv2		= RayInverse_PlaneToVR180( dir, eye );	uv2.x *= 2.0;
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 3 :
			{
				float	ipd		= 0.1;
				uint	eye		= 0;
				float3	dir		= Ray_PlaneToVR360( ipd, float3(0.0), 0.1, uv, eye ).dir;
				float2	uv2		= RayInverse_PlaneToVR360( dir, eye );	uv2.y *= 2.0;
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 4 :
			{
				float3	dir		= Ray_PlaneToSphereMap360( uv );
				float2	uv2		= RayInverse_PlaneToSphereMap360( dir );
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 5 :
			{
				float2	fov		= ToRad(float2( 87.0, 55.0 ));
				float3	dir		= Ray_PlaneToSphere( fov, uv );
				float2	uv2		= RayInverse_PlaneToSphere( Rcp(fov * 0.5), dir );
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 6 :
			{
				float	fov		= ToRad(180.0);
				float3	dir		= Ray_FishEye( fov, uv );
				float2	uv2		= RayInverse_FishEye( fov, dir );
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 7 :
			{
				float3	dir		= Ray_Paraboloid( uv );
				float2	uv2		= RayInverse_Paraboloid( dir );
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 8 :
			{
				float3	dir		= Ray_DualParaboloid( uv );
				float2	uv2		= RayInverse_DualParaboloid2D( dir );
				col.rg = Abs(uv2 - uv) * scale;
				break;
			}
			case 9 :
			{
				float	fov		= ToRad(170.0);
				float2	dim		= float2(1024.0);
				float2	sp		= uv * dim;
				float3	dir		= Ray_PaniniProjection( fov, float3(0.0), 0.1, sp, dim ).dir;
				float2	sp2		= RayInverse_Panini( fov, dim, dir );
				col.rg = Abs(sp2 - sp) / dim * scale;
				break;
			}
		}

		if ( i != int(GetGlobalCoordUNorm(int3(1)).y * y_max) )
		{
			out_Color = float4(1.0);
			return;
		}

		if ( Any(IsNaN( col.rgb )) or Any(IsInfinity( col.rgb )))
		{
			out_Color = float4(0.0, 0.0, 1.0, 1.0);
			return;
		}

		col.a = 1.0;
		out_Color = col;
	}

#endif
//-----------------------------------------------------------------------------
