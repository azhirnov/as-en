// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define VIEW_1D
#	define VIEW_2D
#	define VIEW_TURB_2D
#endif
#include "tools/TileableNoise.as"
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		tex_pot		= DynamicUInt();
		RC<DynamicDim>		tex_dim		= tex_pot.Mul(32).Dimension2();
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>			tex			= Image( EPixelFormat::R8_UNorm, tex_dim );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicFloat>	p_lac		= DynamicFloat();
		RC<DynamicFloat>	p_pers		= DynamicFloat();
		RC<DynamicFloat2>	p_vscale	= DynamicFloat2();
		RC<DynamicUInt>		p_oct		= DynamicUInt();
		RC<DynamicUInt>		p_filter	= DynamicUInt();

		// sliders
		{
			const array<float>	params = {
				1, 						// TexDim
				1, 						// 2D
				4, 						// Octaves
				0, 						// Filter
				1.2000f, 				// Lacunarity
				1.0000f, 				// Persistence
				1.5000f, 0.0000f, 		// VScale
			};
			int		i = 0;
			Slider( tex_pot,	"TexDim",		1,					16,					int(params[i]) );		++i;
			Slider( mode,		"View",			0,					2,					int(params[i]) );		++i;
			Slider( p_oct,		"Octaves",		1,					8,					int(params[i]) );		++i;
			Slider( p_filter,	"Filter",		0,					2,					int(params[i]) );		++i;
			Slider( p_lac,		"Lacunarity",	0.f,				4.f,				params[i] );			++i;
			Slider( p_pers,		"Persistence",	0.f,				4.f,				params[i] );			++i;
			Slider( p_vscale,	"VScale",		float2(0.1f, -2.f),	float2(4.f, 2.f),	float2( params[i], params[i+1] ));	i += 2;
		}

		// render loop
		{
			CreateRenderTileableNoisePass( tex );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_2D" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Noise",	tex,	Sampler_LinearRepeat );
			pass.Slider( "iGrid",		0,		1,		0 );
			pass.Slider( "iColor",		0,		3,		0 );
			pass.Slider( "iScale",		1,		8,		4 );
			pass.Constant( "iOctaves",		p_oct		);
			pass.Constant( "iFilter",		p_filter	);
			pass.Constant( "iLacunarity",	p_lac		);
			pass.Constant( "iPersistence",	p_pers		);
			pass.Constant( "iVScale",		p_vscale	);
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_1D" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Noise",	tex,	Sampler_LinearRepeat );
			pass.Slider( "iYOffset",	0.f,	1.f,	0.5f );
			pass.Constant( "iOctaves",		p_oct		);
			pass.Constant( "iFilter",		p_filter	);
			pass.Constant( "iLacunarity",	p_lac		);
			pass.Constant( "iPersistence",	p_pers		);
			pass.Constant( "iVScale",		p_vscale	);
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_TURB_2D" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Noise",	tex,	Sampler_LinearRepeat );
			pass.Constant( "iOctaves",		p_oct		);
			pass.Constant( "iFilter",		p_filter	);
			pass.Constant( "iLacunarity",	p_lac		);
			pass.Constant( "iPersistence",	p_pers		);
			pass.Constant( "iVScale",		p_vscale	);
			pass.EnableIfEqual( mode, 2 );
		}

		Present( rt );
		Export( tex, "tile-noise-.aeimg" );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(VIEW_2D) or defined(VIEW_1D) or defined(VIEW_TURB_2D)
	#include "Noise.glsl"

	float  NoiseTex (float2 uv)
	{
		const float2	dim = gl.texture.GetSize( un_Noise, 0 );
		const float2	f	= Fract( uv * dim + 0.5 + 1.0/512.0 );

		switch ( iFilter )
		{
			case 0 :
				return gl.texture.SampleLod( un_Noise, uv, 0.f ).r;

			case 1 : {
				float4	data = gl.texture.Gather( un_Noise, uv, 0 );
				return BiLerp( data[3], data[2], data[0], data[1], f );
			}

			case 2 : {
				float4	data = gl.texture.Gather( un_Noise, uv, 0 );
				return BiCubic( data[3], data[2], data[0], data[1], f );
			}
		}
	}

	float  NoiseTex (float2 pos, const NoiseParams params)	{ return NoiseTex( pos.xy ); }

	FBM_NOISE2D_Hash( NoiseTex )


	float  NoiseFBM (float2 pos, const FBMParams fbm)
	{
		float n = NoiseTexFBM( pos, fbm );
		return ToSNorm(n) * iVScale.x + iVScale.y;
	}


	float  NoiseFBM (float2 pos, const NoiseParams params, const FBMParams fbm)  { return NoiseFBM( pos, fbm ); }

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_2D
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	pos	= GetGlobalCoordSF().xy / float(GetGlobalSize().x);
		float2	uv	= pos * iScale;
		float	n	= ToUNorm( NoiseFBM( uv, CreateFBMParams( iLacunarity, iPersistence, iOctaves )));
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
			float	a = AA_QuadGrid_dxdy( uv, float2(1.0, 2.5) ).x;
			out_Color.rgb = Lerp( border_col, out_Color.rgb, a );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_1D
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	ND_ float2  Noise (const int dx)
	{
		float	x	= GetGlobalCoordUNorm( int3(dx) ).x;
		float2	uv	= Saturate( float2( x, iYOffset ));
		float	n	= ToUNorm( NoiseFBM( uv, CreateFBMParams( iLacunarity, iPersistence, iOctaves )));
		return float2( x, 1.0 - n );
	}

	void  Main ()
	{
		float2	pos	= GetGlobalCoordUNorm().xy;		pos.y = pos.y * 1.05 - 0.025;
		float2	p0	= Noise( 0 );
		float2	p1	= Noise( 1 );
		float	d	= SDF2_Line( pos, p0, p1 );
		float	n	= p0.y;

		out_Color = pos.y > 0.5 ? float4(0.15, 0.15, 0.2, 1.0) : float4(0.2, 0.15, 0.15, 1.0);

		out_Color.rgb *= AA_QuadGrid( ToSNorm(pos) * un_PerPass.resolution.xy, float2(1.0/200.0), 3.0 );

		if ( d < 0.001f )
			out_Color = Rainbow( n );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_TURB_2D
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	TURBULENCE2D_FBM_Hash( NoiseFBM )


	void  Main ()
	{
		float2	pos		= GetGlobalCoordSF().xy / float(GetGlobalSize().x);
		float2	dist	= Turbulence_NoiseFBM( pos, CreateFBMParams( iLacunarity, iPersistence, iOctaves ));
		float	a		= AA_QuadGrid_dxdy( dist, float2(1.0, 2.5) ).x;

		out_Color = float4(a);
	}

#endif
//-----------------------------------------------------------------------------
