// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compare conservative rasterization with emulated techniques.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define COMPARE
#	define DOWNSCALE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		scale_pot	= DynamicUInt();
		RC<DynamicUInt>		scale		= scale_pot.Exp2();
		RC<Image>			low_res		= Image( EPixelFormat::R8_UNorm, SurfaceSize().XY().Div( scale.XX() ).Dimension() );
		const int			upscale		= 8;
		RC<Image>			low_res_up	= Image( EPixelFormat::R8_UNorm, low_res.Dimension().Mul(upscale) );
		RC<Image>			high_res1	= Image( EPixelFormat::R8_UNorm, SurfaceSize() );
		RC<Image>			high_res2	= Image( EPixelFormat::R8_UNorm, SurfaceSize() );
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>			scene		= Scene();
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicFloat2>	tri_p0		= DynamicFloat2();
		RC<DynamicFloat2>	tri_p1		= DynamicFloat2();
		RC<DynamicFloat2>	tri_p2		= DynamicFloat2();
		RC<DynamicFloat>	extrude		= DynamicFloat();

		// create geometry
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 3;
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( mode,		"Mode",			0,				2 );	// extruded, conservative, downscale
		Slider( scale_pot,	"ResScale",		3,				7,				6 );
		Slider( tri_p0,		"P0",			float2(-1.0),	float2(1.0),	float2(-0.17, -0.5) );
		Slider( tri_p1,		"P1",			float2(-1.0),	float2(1.0),	float2(-0.5,   0.5) );
		Slider( tri_p2,		"P2",			float2(-1.0),	float2(1.0),	float2( 0.5,   0.7) );
		Slider( extrude,	"ExtrudePx",	0.0,			4.0,			3.0 );

		// render loop //
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "low res 1" );
			pass.AddPipeline( "tests/ConservativeRaster-v1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ConservativeRaster-v1.as)
			pass.Output(	"out_Color",	low_res,	RGBA32f(0.f) );
			pass.Constant(	"iP0",			tri_p0 );
			pass.Constant(	"iP1",			tri_p1 );
			pass.Constant(	"iP2",			tri_p2 );
			pass.Constant(	"iExtrudePx",	extrude );
			pass.EnableIfEqual( mode, 0 );
		}

		if ( GetFeatureSet().hasConservativeRasterization() )
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "low res 2" );
			pass.AddPipeline( "tests/ConservativeRaster-v2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ConservativeRaster-v2.as)
			pass.Output(	"out_Color",	low_res,	RGBA32f(0.f) );
			pass.Constant(	"iP0",			tri_p0 );
			pass.Constant(	"iP1",			tri_p1 );
			pass.Constant(	"iP2",			tri_p2 );
			pass.EnableIfEqual( mode, 1 );
		}
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "low res 2" );
			pass.AddPipeline( "tests/ConservativeRaster-v1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ConservativeRaster-v1.as)
			pass.Output(	"out_Color",	low_res_up,	RGBA32f(0.f) );
			pass.Constant(	"iP0",			tri_p0 );
			pass.Constant(	"iP1",			tri_p1 );
			pass.Constant(	"iP2",			tri_p2 );
			pass.EnableIfEqual( mode, 2 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "DOWNSCALE" );
			pass.Output(	"out_Color",	low_res,	RGBA32f(0.f) );
			pass.ArgIn(		"un_HighRes",	low_res_up,	Sampler_MaxLinearClamp );
			pass.Constant(	"iRadius",		upscale );
			pass.EnableIfEqual( mode, 2 );
		}

		// reference
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "high res" );
			pass.AddPipeline( "tests/ConservativeRaster-v1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ConservativeRaster-v1.as)
			pass.Output(	"out_Color",	high_res1,	RGBA32f(0.f) );
			pass.Constant(	"iP0",			tri_p0 );
			pass.Constant(	"iP1",			tri_p1 );
			pass.Constant(	"iP2",			tri_p2 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "high res" );
			pass.AddPipeline( "tests/ConservativeRaster-v1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ConservativeRaster-v1.as)
			pass.Output(	"out_Color",	high_res2,	RGBA32f(0.f) );
			pass.Constant(	"iP0",			tri_p0 );
			pass.Constant(	"iP1",			tri_p1 );
			pass.Constant(	"iP2",			tri_p2 );
			pass.Constant(	"iExtrudePx",	extrude.Mul( scale.ToFloat() ) );
		}

		// compare
		{
			RC<Postprocess>		pass = Postprocess( "", "COMPARE" );
			pass.Output(	"out_Color",	rt,			RGBA32f(0.f) );
			pass.ArgIn(		"un_LowRes",	low_res,	Sampler_NearestClamp );
			pass.ArgIn(		"un_HighRes1",	high_res1,	Sampler_NearestClamp );
			pass.ArgIn(		"un_HighRes2",	high_res2,	Sampler_NearestClamp );
			pass.Slider(	"iDrawOrder",	0,			1,		1 );
			pass.Constant(	"iMode",		mode );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef COMPARE

	void  Main ()
	{
		float2	uv = gl.FragCoord.xy * un_PerPass.invResolution;

		float	covered1 = gl.texture.Sample( un_LowRes, uv ).r;	// extruded or conservative
		float	covered2 = gl.texture.Sample( un_HighRes1, uv ).r;
		float	covered3 = gl.texture.Sample( un_HighRes2, uv ).r;	// high res extruded

		if ( iMode != 0 )
			covered3 = 0.0;

		out_Color = float4(0.0, 0.0, 0.0, 1.0);

		switch ( iDrawOrder )
		{
			case 0 :
			{
				if ( covered3 > 0.5 )	out_Color.rgb = float3(0.0, 0.0, 1.0);
				if ( covered1 > 0.5 )	out_Color.rgb = float3(1.0, 0.0, 0.0);
				if ( covered2 > 0.5 )	out_Color.rgb = float3(0.0, 1.0, 0.0);
				break;
			}
			case 1 :
			{
				if ( covered3 > 0.5 )	out_Color.rgb = float3(0.0, 0.0, 1.0);
				if ( covered2 > 0.5 )	out_Color.rgb = float3(0.0, 1.0, 0.0);
				if ( covered1 > 0.5 )	out_Color.rgb = float3(1.0, 0.0, 0.0);
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DOWNSCALE
	#include "Math.glsl"

	void  Main ()
	{
		float2	uv = gl.FragCoord.xy * un_PerPass.invResolution;

		const int2	coord	= int2(gl.FragCoord.xy);
		float		max_val	= 0.0;

		for (int y = 0; y < iRadius; ++y)
		for (int x = 0; x < iRadius; ++x)
		{
			max_val = Max( max_val, gl.texture.Fetch( un_HighRes, coord * iRadius + int2(x,y), 0 ).r );
		}

		out_Color.r = max_val;
	}

#endif
//-----------------------------------------------------------------------------
