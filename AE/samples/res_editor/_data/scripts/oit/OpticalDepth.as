// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Approximate OIT
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
		RC<Image>				rt1_color		= Image( EPixelFormat::R11G11B10F,	SurfaceSize() );	rt1_color.Name( "RT-Color" );
		RC<Image>				rt1_weights		= Image( EPixelFormat::R16F,		SurfaceSize() );	rt1_weights.Name( "RT-OpticalDepth" );
		RC<Image>				rt2				= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );	rt2.Name( "RT-Output" );
		RC<Image>				ds				= Image( EPixelFormat::Depth32F,	SurfaceSize() );	ds.Name( "RT-Depth" );

		RC<FPVCamera>			camera			= FPVCamera();
		RC<Scene>				scene			= Scene();
		RC<Buffer>				drawtasks		= Buffer();
		uint					instance_count	= 0;

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 150.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -3.f));
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
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			RC<Mesh>				mesh		= Mesh();

			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphere( 3 );

			RC<Buffer>	sphere = mesh.ToBuffer();
			sphere.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= mesh.IndexCount();
			cmd.instanceCount	= instance_count;
			cmd.IndexBuffer( sphere, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	sphere );
			geometry.ArgIn( "un_DrawTasks",	drawtasks );

			scene.Add( geometry );
		}

		scene.Set( camera );

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/OIT-OpticalDepth.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/OIT-OpticalDepth.as)
			draw.Output( "out_Color",		rt1_color,		RGBA32f(0.f) );
			draw.Output( "out_Weights",		rt1_weights,	RGBA32f(0.f) );
			draw.Output(					ds,				DepthStencil( 1.f, 0 ));
			draw.Slider( "iWeightScale",	0.0,	0.99,	0.7 );
			draw.Slider( "iAlphaScale",		0.0,	2.0,	1.0 );
			draw.Slider( "iPosScale",		1.0,	10.0,	1.0 );
		}
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",		rt2 );
			pass.ArgIn( "un_TexColor",		rt1_color,   Sampler_NearestClamp );
			pass.ArgIn( "un_TexWeights",	rt1_weights, Sampler_NearestClamp );
			pass.Slider( "iDbgView",		0,		2,		0 );
			pass.Slider( "iDbgScale",		-10,	10,		0 );
		}

		Present( rt2 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void  Main ()
	{
		const int2		uv		= int2(gl.FragCoord.xy);
		const float3	accum	= gl.texture.Fetch( un_TexColor, uv, 0 ).rgb;
		const float		tau		= gl.texture.Fetch( un_TexWeights, uv, 0 ).r;

		float	alpha				= 1.0 - Exp(-tau);
		float3	color				= accum / Max( tau, 1.0e-4 );
		float3	transparent_premul	= color * alpha;
		out_Color.rgb = transparent_premul;	// + opaqueColor * (1.0 - alpha);

		float	scale = Exp2( float(iDbgScale) );

		switch ( iDbgView )
		{
			case 1 :	out_Color.rgb = accum * scale;			break;
			case 2 :	out_Color.rgb = float3(alpha) * scale;	break;
		}
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
