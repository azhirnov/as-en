// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	ref:
	https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
*/
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
		RC<Image>	rt = Image( EPixelFormat::RGBA16F, SurfaceSize() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output(	"out_Color",	rt );

			pass.Slider(	"iShape",		0,				4 );	// directional, omni, spot, rect area, disk area
			pass.Slider(	"iSRGB",		0,				1,				1 );
			pass.Slider(	"iDir",			-1.0,			1.0,			0.0  );
			pass.Slider(	"iDistScale",	0.1,			10.0,			1.0 );
			pass.Slider(	"iAtten",		float3(0.0),	float3(1.0),	float3(1.0, 0.8, 0.6) );
			pass.Slider(	"iParams",		float4(0.0),	float4(1.0),	float4(0.0) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "PBR.glsl"
	#include "ColorSpace.glsl"
	#include "InvocationID.glsl"

	struct LightParams
	{
		float3	dirToLight;
		float	distToLight;
		float3	lightDir;
	};


	float  DirectionalLight (const LightParams p)
	{
		float	nl		= Dot( p.dirToLight, p.lightDir );
		float	atten	= Attenuation( iAtten, p.distToLight );
		return	atten * nl;
	}

	float  DirectionalLight2 (const LightParams p)
	{
		float	umbra	= iParams.x;
		float	penumbra = Max( iParams.y, umbra + 0.1 );
		float	nl		= Dot( p.dirToLight, p.lightDir );
		float	atten	= Saturate( (nl - umbra) / (penumbra - umbra) );
		return	atten;
	}

	float  OmniLight (const LightParams p)
	{
		return Attenuation( iAtten, p.distToLight );
	}

	float  SpotLight (const LightParams p)
	{
		float	nl = Dot( p.dirToLight, p.lightDir );
		return SpotAttenuation( iAtten, nl, p.distToLight, iParams.xy );
	}

	float  RectLight (const LightParams p)
	{
		return RectAreaLightSolidAngle( p.dirToLight * p.distToLight, p.lightDir, Max( iParams.z, 0.01 ));
	}


	void  Main ()
	{
		const float2	center			= float2( 0.0, -0.5 );
		float2			uv				= GetGlobalCoordSNormCorrected();
		float			atten			= 0.0;
		LightParams		params;

		if ( iShape < 1 ){
			params.dirToLight	= float3( 0.0, ToSNorm(GreaterF( uv.y, center.y )), 0.0 );
			params.distToLight	= Abs( uv.y - center.y ) * iDistScale;
		}else{
			params.dirToLight	= float3( Normalize( uv - center ), 0.0 );
			params.distToLight	= Length( uv - center ) * iDistScale;
		}

		params.lightDir		= float3( SinCos( iDir * float_HalfPi ), 0.0 );

		switch ( iShape )
		{
			case 0 :	atten = DirectionalLight( params );		break;
			case 1 :	atten = DirectionalLight2( params );	break;
			case 2 :	atten = OmniLight( params );			break;
			case 3 :	atten = SpotLight( params );			break;
			case 4 :	atten = RectLight( params );			break;
		}

		out_Color = float4(atten);
		if ( iSRGB == 1 )
			out_Color = ApplySRGBCurve( out_Color );
	}

#endif
//-----------------------------------------------------------------------------
