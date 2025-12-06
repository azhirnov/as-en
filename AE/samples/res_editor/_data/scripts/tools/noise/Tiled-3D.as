// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define VIEW
#endif
#include "tools/TileableNoise.as"
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		tex_dim	= DynamicUInt();
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>			tex		= Image( EPixelFormat::R8_UNorm, tex_dim.Mul(32).Dimension3() );

		Slider( tex_dim,	"TexDim",	1,	16,		4 );

		// render loop
		{
			CreateRenderTileableNoisePass3D( tex );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Noise",	tex,	Sampler_LinearRepeat );
			pass.Slider( "iGrid",		0,				1,				1 );
			pass.Slider( "iColor",		0,				3,				1 );
			pass.Slider( "iScale",		1,				8,				4 );
			pass.Slider( "iLayer",		0.f,			1.f				);
			pass.Slider( "iRotation",	float3(-180.f),	float3(180.f),	float3(0.f) );
		}

		Present( rt );
		Export( tex, "tile-noise-3d-.aeimg" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float	size	= GetGlobalSize().x;
		float2	pos		= GetGlobalCoordSF().xy / size;
		float3	pos3	= float3(pos * iScale, Clamp( iLayer, 0.01, 0.99 ));

		pos3 = QMul( QRotationY(ToRad( iRotation.x )), pos3 );
		pos3 = QMul( QRotationX(ToRad( iRotation.y )), pos3 );
		pos3 = QMul( QRotationZ(ToRad( iRotation.z )), pos3 );

		float	n		= gl.texture.Sample( un_Noise, pos3 ).r;
		float3	border_col;

		switch ( iColor )
		{
			case 0 :
				out_Color	= float4( n );
				out_Color.a	= 1.0;
				border_col	= float3(1.0, 0.0, 0.0);
				break;

			case 1 :
				out_Color	= Rainbow( 1.0 - n );
				border_col	= float3(0.0);
				break;

			case 2 :
			case 3 :
				out_Color.rgb	= ComputeNormalInWS_quadSg( float3( pos, n ));
				out_Color.rg	*= (iColor == 3 ? -1.0 : 1.0);
				out_Color.a		= 1.0;
				border_col		= float3(1.0);
				break;
		}

		if ( iGrid == 1 )
		{
			float	a = AA_QuadGrid_dxdy( pos3, float2(1.0, 2.5) ).x;
			out_Color.rgb = Lerp( border_col, out_Color.rgb, a );
		}
	}

#endif
//-----------------------------------------------------------------------------
