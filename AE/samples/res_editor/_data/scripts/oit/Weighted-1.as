// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Approximate OIT
	based on https://github.com/nvpro-samples/vk_order_independent_transparency (Apache-2.0 license)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT
	#include "samples/GenColoredSpheres.as"

	void ASmain ()
	{
		// initialize
		RC<Image>			rt1_color		= Image( EPixelFormat::RGBA16F,		SurfaceSize() );	rt1_color.Name( "RT-Color" );
		RC<Image>			rt1_weights		= Image( EPixelFormat::R16F,		SurfaceSize() );	rt1_weights.Name( "RT-Weights" );
		RC<Image>			rt2				= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );	rt2.Name( "RT-Output" );
		RC<Image>			ds				= Image( EPixelFormat::Depth32F,	SurfaceSize() );	ds.Name( "RT-Depth" );

		RC<FPVCamera>		camera			= FPVCamera();
		RC<Scene>			scene			= Scene();
		RC<Buffer>			drawtasks		= Buffer();

		RC<DynamicUInt>		subpasses		= DynamicUInt();
		RC<DynamicFloat>	weight_scale	= DynamicFloat();
		RC<DynamicFloat>	alpha_scale		= DynamicFloat();
		RC<DynamicFloat>	pos_scale		= DynamicFloat();
		RC<DynamicUInt>		dbg_view		= DynamicUInt();
		RC<DynamicInt>		dbg_scale		= DynamicInt();

		uint				instance_count	= 0;

		Slider( subpasses,		"UseSubpass",	0,		1 );
		Slider( weight_scale,	"WeightScale",	0.0,	0.99,	0.7 );
		Slider( alpha_scale,	"AlphaScale",	0.0,	2.0,	1.0 );
		Slider( pos_scale,		"PosScale",		1.0,	10.0,	1.0 );
		Slider( dbg_view,		"DbgView",		0,		2,		0 );
		Slider( dbg_scale,		"DbgScale",		-10,	10,		0 );

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 150.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -3.f));
			scene.Set( camera );
		}

		// setup draw tasks
		{
			array<float2x4>	draw_tasks = GenColoredSpheresDrawTasks();
			drawtasks.FloatArray( "tasks", draw_tasks );
			drawtasks.LayoutName( "DrawTask" );
			instance_count = draw_tasks.size();
		}

		// create sphere
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphere( 3 );

			RC<Buffer>	sphere = mesh.ToBuffer();
			sphere.LayoutName( "GeometrySBlock" );

			geometry.ArgIn(	"un_Geometry",	sphere );
			geometry.ArgIn( "un_DrawTasks",	drawtasks );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= mesh.IndexCount();
			cmd.instanceCount	= instance_count;
			cmd.layer			= ERenderLayer::Translucent;
			cmd.IndexBuffer( sphere, "indices" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount	= 4;
			cmd.layer		= ERenderLayer::PostProcess;
			geometry.Draw( cmd );
			scene.Add( geometry );
		}


		// render loop //

		// without subpass
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "main pass" );
			pass.AddPipeline( "samples/OIT-Weighted1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/OIT-Weighted1.as)
			pass.Output(	"out_Color",		rt1_color,		RGBA32f(0.f) );
			pass.Output(	"out_Weights",		rt1_weights,	RGBA32f(1.f) );
			pass.Output(						ds,				DepthStencil( 1.f, 0 ));
			pass.Constant(	"iWeightScale",		weight_scale );
			pass.Constant(	"iAlphaScale",		alpha_scale );
			pass.Constant(	"iPosScale",		pos_scale );
			pass.EnableIfEqual( subpasses, 0 );
			pass.Layer( ERenderLayer::Translucent );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Output(	"out_Color",		rt2 );
			pass.ArgIn(		"un_TexColor",		rt1_color,   Sampler_NearestClamp );
			pass.ArgIn(		"un_TexWeights",	rt1_weights, Sampler_NearestClamp );
			pass.Constant(	"iDbgView",			dbg_view );
			pass.Constant(	"iDbgScale",		dbg_scale );
			pass.EnableIfEqual( subpasses, 0 );
		}

		// with subpass
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "subpass-1" );
			{
				pass.AddPipeline( "samples/OIT-Weighted1-pass1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/OIT-Weighted1-pass1.as)
				pass.Output( "out_Color",			rt1_color,		RGBA32f(0.f) );
				pass.Output( "out_Weights",			rt1_weights,	RGBA32f(1.f) );
				pass.Output(						ds,				DepthStencil( 1.f, 0 ));
				pass.Constant( "iWeightScale",		weight_scale );
				pass.Constant( "iAlphaScale",		alpha_scale );
				pass.Constant( "iPosScale",			pos_scale );
				pass.Layer( ERenderLayer::Translucent );
			}
			pass.NextSubpass( "subpass-2" );
			{
				pass.AddPipeline( "samples/OIT-Weighted1-pass2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/OIT-Weighted1-pass2.as)
				pass.Output(	"out_Color2",		rt2 );
				pass.Input(		"un_TexColor",		rt1_color,		"out_Color" );
				pass.Input(		"un_TexWeights",	rt1_weights,	"out_Weights" );
				pass.Constant(	"iDbgView",			dbg_view );
				pass.Constant(	"iDbgScale",		dbg_scale );
				pass.Layer( ERenderLayer::PostProcess );
			}
			pass.EnableIfEqual( subpasses, 1 );
		}

		Present( rt2 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void  Main ()
	{
		const int2		coord	= int2(gl.FragCoord.xy);
		const float4	accum	= gl.texture.Fetch( un_TexColor, coord, 0 );
		const float		reveal	= gl.texture.Fetch( un_TexWeights, coord, 0 ).r;

		out_Color = float4( accum.rgb / Max(accum.a, 1.0e-5), 1.0 );
		out_Color.rgb *= 1.0 - reveal;

		float	scale = Exp2( float(iDbgScale) );

		switch ( iDbgView )
		{
			case 1 :	out_Color = accum * scale;					break;
			case 2 :	out_Color = float4(1.0 - reveal) * scale;	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
