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
			RC<ComputePass>		pass = ComputePass();

			pass.ArgOut( "un_OutImage", rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );

			pass.Slider( "iScale",	0,	8,	3 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "Geometry.glsl"
	#include "CubeMap.glsl"
	#include "Ray.glsl"


	void  Main ()
	{
		float4		col		= float4(0.0);
		const float	y_max	= 11.0;
		const float	y		= Floor( GetGroupCoordUNorm().y * y_max );
		const float	scale	= Exp10( float(iScale) );

		float2		uv		= GetGlobalCoordUNorm().xy;		uv.y = (uv.y - y/y_max) * y_max;
		float3		dir		= Ray_PlaneTo360( float3(0.0), 0.1, uv ).dir;

		switch ( int(y) )
		{
			case 0 : {
				float3	uv_f	= CM_IdentitySC_Inverse( dir );
				float3	dir2	= CM_IdentitySC_Forward( uv_f.xy, ECubeFace(uv_f.z) );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 1 : {																// high accuracy
				float3	uv_f	= CM_TangentialSC_Inverse( dir );
				float3	dir2	= CM_TangentialSC_Forward( uv_f.xy, ECubeFace(uv_f.z) );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 2 : {																// high accuracy
				float3	uv_f	= CM_EverittSC_Inverse( dir );
				float3	dir2	= CM_EverittSC_Forward( uv_f.xy, ECubeFace(uv_f.z) );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 3 : {
				float3	uv_f	= CM_5thPolySC_Inverse( dir );
				float3	dir2	= CM_5thPolySC_Forward( uv_f.xy, ECubeFace(uv_f.z) );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 4 : {
				float3	uv_f	= CM_COBE_SC_Inverse( dir );
				float3	dir2	= CM_COBE_SC_Forward( uv_f.xy, ECubeFace(uv_f.z) );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 5 : {																// high accuracy
				float3	uv_f	= CM_ArvoSC_Inverse( dir );
				float3	dir2	= CM_ArvoSC_Forward( uv_f.xy, ECubeFace(uv_f.z) );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 6 : {
				ECubeFace	face = CM_DirToFace( dir );
				float3		n	 = GetMajorAxis( dir );
				col.rgb = Abs( CM_GetNormal( face ) - n );
				break;
			}
			case 7 : {
				ECubeFace	face	= CM_DirToFace( dir );
				float3		n2		= CM_InverseRotation( face, dir );
				float3		n3		= CM_InverseRotation( dir ).xyz;
				col.rgb = Abs( n2 - n3 ) * scale;
				break;
			}
			case 8 : {
				ECubeFace	face	= CM_DirToFace( dir );
				float3		t1		= CM_GetTangent( face );
				float3		t2		= GetMajorAxis( CM_Tangent( dir, face ));
				col.rgb = Abs( t1 - t2 ) * scale;
				break;
			}
			case 9 : {
				ECubeFace	face	= CM_DirToFace( dir );
				float3		b1		= CM_GetBitangent( face );
				float3		b2		= GetMajorAxis( CM_Bitangent( dir, face ));
				col.rgb = Abs( b1 - b2 ) * scale;
				break;
			}
			case 10 : {
				ECubeFace	face	= CM_DirToFace( dir );
				float3		t		= CM_Tangent( dir, face );
				float3		b		= CM_Bitangent( dir, face );
				float3		n		= Normalize( Cross( b, t ));
				col.rgb = Abs( dir - n ) * scale;
				break;
			}
		}

		if ( Any(IsNaN( col.rgb )) or Any(IsInfinity( col.rgb )))
		{
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(0.2) );
			return;
		}

		col.a = 1.0;

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
