// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	More Physically Based version
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define DRAW_SKY
#	define LIGHT_MASK
#	define RADIAL_BLUR
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		EPixelFormat		format			= EPixelFormat::RGBA16F;		// R11G11B10F or RGBA16F
		RC<Image>			scene_color		= Image( format, SurfaceSize() );						scene_color.Name( "RT-Color" );
		RC<Image>			scene_depth		= Image( EPixelFormat::Depth32F, SurfaceSize() );		scene_depth.Name( "RT-Depth" );
		RC<Image>			rt				= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Image>			light_mask		= Image( format, SurfaceSize()/4 );						light_mask.Name( "Light mask" );
		RC<Image>			blur1			= Image( format, light_mask.Dimension() );
		RC<Image>			blur2			= Image( format, light_mask.Dimension() );
		RC<Image>			tex				= Image( EImageType::Float_2DArray, "res/models/Plants/texturearray_plants_rgba.ktx" );
		RC<Image>			tex2			= Image( EImageType::Float_2D, "shadertoy/Organic_2.jpg", ImageLoadOpFlags::GenMipmaps );
		RC<Scene>			scene			= Scene();
		RC<DynamicFloat3>	center			= DynamicFloat3();
		RC<DynamicFloat>	blur			= DynamicFloat();
		RC<DynamicFloat>	halo			= DynamicFloat();
		RC<DynamicUInt>		sample_cnt		= DynamicUInt();
		const uint			tree_grid		= 32;

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 50.f );
			camera.FovY( 60.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, -0.2f, 0.0f));

			scene.Set( camera );
		}

		// create geometry
		{
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Buffer>				geom_data	= Buffer();

			geom_data.LoadCSV( "res/models/Plants/Plant11.csv", "Vertex" );
			geometry.ArgIn(	"un_Geometry",	geom_data );
			geometry.ArgIn( "un_Texture",	tex,	Sampler_LinearMipmapClamp );
			geometry.ArgIn( "un_GroundTex",	tex2,	Sampler_LinearMipmapRepeat );

			{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount		= geom_data.ConstArraySize();
				cmd.instanceCount	= tree_grid * tree_grid;
				cmd.PipelineHint( "Tree" );
				geometry.Draw( cmd );
			}{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount	= 4;
				cmd.PipelineHint( "Ground" );
				geometry.Draw( cmd );
			}

			scene.Add( geometry );
		}

		Slider( center,		"Center",		float3(-0.1,-0.1, 0.0),	float3(1.1, 1.1, 1.0),	float3(0.5, 0.2, 0.999) );
		Slider( sample_cnt,	"SampleCount",	2,				64,				18 );
		Slider( blur,		"Blur",			0.0,			1.0,			0.6 );
		Slider( halo,		"HaloRadius",	0.01,			0.3,			0.075 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "DRAW_SKY" );
			pass.Output(	"out_Color",		scene_color,	RGBA32f(0.0) );
			pass.Slider(	"iBright",			0.2,	4.0,	0.8 );
			pass.Constant(	"iHaloRadius",		halo );
			pass.Constant(	"iCenter",			center );
			pass.ColorSelector( "iLightCol",	RGBA32f(1.0, 0.8, 0.0, 1.0) );
			pass.ColorSelector( "iSkyCol",		RGBA32f(0.4, 0.5, 0.6, 1.0) );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "samples/LightShafts-draw.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/LightShafts-draw.as)
			pass.Output(	"out_Color",		scene_color );
			pass.Output(						scene_depth,	DepthStencil(1.f, 0) );
			pass.Constant(	"iGridSize",		tree_grid );
		}{
			// sky can be rendered in separate RT, then read here, then combine and apply to main RT
			RC<Postprocess>		pass = Postprocess( "", "LIGHT_MASK" );
			pass.Output(	"out_Color",		light_mask );
			pass.ArgIn(		"un_SceneColor",	scene_color,	Sampler_LinearClamp );
			pass.ArgIn(		"un_SceneDepth",	scene_depth,	Sampler_LinearClamp );
			pass.Slider(	"iMask",			-1,		2,		-1 );		// cull: -1-all, 0-depth, 1-luminance, 2-dist to ligth
			pass.Constant(	"iHaloRadius",		halo );
			pass.Constant(	"iCenter",			center );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RADIAL_BLUR" );
			pass.Output(	"out_Color",		blur1 );
			pass.ArgIn(		"un_SrcImage",		light_mask,		Sampler_LinearClamp );
			pass.Constant(	"iSampleCount",		sample_cnt );
			pass.Constant(	"iBlur",			blur );
			pass.Constant(	"iCenter",			center );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RADIAL_BLUR" );
			pass.Output(	"out_Color",		blur2 );
			pass.ArgIn(		"un_SrcImage",		blur1,			Sampler_LinearClamp );
			pass.Constant(	"iSampleCount",		sample_cnt );
			pass.Constant(	"iBlur",			blur );
			pass.Constant(	"iCenter",			center );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_Blur1",			blur1,			Sampler_LinearClamp );
			pass.ArgIn(		"un_Blur2",			blur2,			Sampler_LinearClamp );
			pass.ArgIn(		"un_LightMask",		light_mask,		Sampler_NearestClamp );
			pass.ArgIn(		"un_SceneColor",	scene_color,	Sampler_NearestClamp );
			pass.ArgIn(		"un_SceneDepth",	scene_depth,	Sampler_NearestClamp );
			pass.Slider(	"iStrength",		0.0,	2.0,	1.0 );
			pass.Slider(	"iDbgView",			-1,		3,		-1 );
			pass.Constant(	"iCenter",			center );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DRAW_SKY
	#include "ColorSpace.glsl"
	#include "ToneMapping.glsl"

	void  Main ()
	{
		float2	uv			= gl.FragCoord.xy * un_PerPass.invResolution;

		float3	light_col	= RemoveSRGBCurve( iLightCol.rgb );
		float3	sky_col		= RemoveSRGBCurve( iSkyCol.rgb );

		float2	center		= un_PerPass.mouse.z > 0.0 ? un_PerPass.mouse.xy : iCenter.xy;
		float2	ratio		= float2(1.0, un_PerPass.resolution.y * un_PerPass.invResolution.x);  // aspect ratio correction
		float	x			= Distance( uv * ratio, center * ratio ) / iHaloRadius;

		// params for [Rcp graph](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tools/2d/Graph3.as)
		{
			const float2	iA	= float2( 0.0070f, -0.0350f * 0.01 );
			const float3	iBB	= float3( 0.9630f, 0.0000f, 0.2040f );

			float	a = x * (x + iA.x) - iA.y;
			float	b = x * (iBB.x * x + iBB.y) + iBB.z;
			x = Saturate( 1.0 - a / b );
		}

		float3	col = light_col * x * iBright + sky_col;

		out_Color.rgb = ApplySRGBCurve( ToneMap_LinearHDR( col ));
	}

#endif
//-----------------------------------------------------------------------------
#ifdef LIGHT_MASK
	#include "Math.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	inv_src_dim	= un_PerPass.invResolution / 4.0;
		float2	uv			= gl.FragCoord.xy * un_PerPass.invResolution - inv_src_dim * 0.5;
		float2	duv			= inv_src_dim;
		float	min_d		= iCenter.z;

		float2	center		= un_PerPass.mouse.z > 0.0 ? un_PerPass.mouse.xy : iCenter.xy;
		float2	ratio		= float2(1.0, un_PerPass.resolution.y * un_PerPass.invResolution.x);	// aspect ratio correction
		float	atten		= 1.0 - (Distance( uv * ratio, center * ratio ) / (iHaloRadius * 1.5));	// 1.0 in center, 0.0 on edge

		// 4x downsample using linear filter for smooth transition
		//
		// 0,  1,  2,  3 - X axis
		// -------------
		// 0   1   5   6
		//   x       x    - coords: 0.5/src_dim, 2.5/src_dim
		// 2   3   7   8
		//      uv        - 1.5/src_dim
		// ...

		float	d0	= GreaterF( gl.texture.Sample( un_SceneDepth, uv + float2(-duv.x,  duv.y) ).r, min_d );
		float	d1	= GreaterF( gl.texture.Sample( un_SceneDepth, uv + float2(-duv.x, -duv.y) ).r, min_d );
		float	d2	= GreaterF( gl.texture.Sample( un_SceneDepth, uv + float2( duv.x, -duv.y) ).r, min_d );
		float	d3	= GreaterF( gl.texture.Sample( un_SceneDepth, uv + float2( duv.x,  duv.y) ).r, min_d );
		float	d	= (d0 + d1 + d2 + d3) / 4.0;

		float3	col = gl.texture.Sample( un_SceneColor, uv ).rgb;

		if ( iMask == -1 or iMask == 0 )
			col *= d;

		if ( iMask == -1 or iMask == 1 )
			col *= GreaterF( RGBtoLuminance( col ), 0.5 );

		if ( iMask == -1 or iMask == 2 )
			col *= Saturate( atten );

		out_Color.rgb = col;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RADIAL_BLUR
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	center	= un_PerPass.mouse.z > 0.0 ? un_PerPass.mouse.xy : iCenter.xy;
		float2	uv		= GetGlobalCoordUNorm().xy;
		float2	duv		= (center - uv) * iBlur / iSampleCount;
		float3	col		= float3(0.0);

		for (uint i = 0; i < iSampleCount; ++i)
		{
			col += gl.texture.Sample( un_SrcImage, uv ).rgb;
			uv  += duv;
		}
		out_Color.rgb = col / iSampleCount;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "ColorSpace.glsl"
	#include "ToneMapping.glsl"
	#include "InvocationID.glsl"


	float3  LightShafts (float3 sceneColor, float2 uv)
	{
		float3	blur	= gl.texture.Sample( un_Blur2, uv ).rgb * iStrength;
		float3	col		= ToneMap_LinearHDR( blur + sceneColor );
		return	col;
	}


	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		float4	scene	= gl.texture.Sample( un_SceneColor, uv );

		out_Color = RemoveSRGBCurve( scene );
		out_Color.rgb = LightShafts( out_Color.rgb, uv );
		out_Color = ApplySRGBCurve( out_Color );

		switch ( iDbgView )
		{
			case 0 :	out_Color.rgb = scene.rgb;										break;
			case 1 :	out_Color.rgb = gl.texture.Sample( un_LightMask, uv ).rgb;		break;
			case 2 :	out_Color.rgb = gl.texture.Sample( un_Blur1, uv ).rgb;			break;
			case 3 :	out_Color.rgb = gl.texture.Sample( un_Blur2, uv ).rgb;			break;
		}
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
