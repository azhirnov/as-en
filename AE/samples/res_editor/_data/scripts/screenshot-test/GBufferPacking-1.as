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
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iScale",		0,	10,	7 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"
	#include "GBuffer.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		float4		col		= float4(0.0);
		const float	y_max	= 7.0;
		const float	scale	= Exp10( float(iScale) );

		float2		uv		= GetGlobalCoordUNorm().xy;
		const float	y		= (uv.y * y_max);
		const int	i		= int(y);
					uv.y	= (uv.y - y/y_max) * y_max;
		float3		dir		= Ray_PlaneToSphereMap360( float3(0.0), 0.1, uv ).dir;

		switch ( i )
		{
			case 0 : {
				float2	enc		= CryTeck_EncodeNormal( dir );
				float3	dir2	= CryTeck_DecodeNormal( enc );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 1 : {
				float2	enc		= Stalker_EncodeNormal( dir );
				float3	dir2	= Stalker_DecodeNormal( enc );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 2 : {
				float2	enc		= Octahedron_EncodeNormal( dir );
				float3	dir2	= Octahedron_DecodeNormal( enc );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 3 : {
				float3	enc		= SigOctahedron_EncodeNormal( dir );
				float3	dir2	= SigOctahedron_DecodeNormal( enc );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 4 : {
				float2	enc		= Stereo_EncodeNormal( dir );
				float3	dir2	= Stereo_DecodeNormal( enc );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 5 : {
				float2	enc		= Spheremap_EncodeNormal( dir );
				float3	dir2	= Spheremap_DecodeNormal( enc );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
			case 6 : {
				float2	enc		= Spherical_EncodeNormal( dir );
				float3	dir2	= Spherical_DecodeNormal( enc );
				col.rgb = Abs( dir - dir2 ) * scale;
				break;
			}
		}

		if ( i != int(GetGlobalCoordUNorm(int3(1)).y * y_max) )
		{
			out_Color = float4(0.0);
			return;
		}

		if ( Any(IsNaN( col.rgb )) or Any(IsInfinity( col.rgb )))
		{
			out_Color = float4(0.2);
			return;
		}

		col.a = 1.0;
		out_Color = col;
	}

#endif
//-----------------------------------------------------------------------------
