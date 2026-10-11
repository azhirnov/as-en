// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compare techniques for light pass in deferred shading:
	 * light volume
	 * light AABB / billboard

	related:
	* [ClusterForward](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/ClusterForward.as)
	* [TiledDeferred](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/TiledDeferred.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define RESOLVE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	hdr_fmt			= EPixelFormat::RGBA16F;
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( hdr_fmt, dim );						rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( EPixelFormat::RGB10_A2_UNorm, dim );	rt_col.Name( "Albedo" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, dim );	rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );		ds.Name( "Depth" );
		RC<Image>			rt_light		= Image( hdr_fmt, dim );						rt_light.Name( "LightBuf" );
		RC<Image>			rt_ovd			= Image( EPixelFormat::R16F, dim );				rt_ovd.Name( "Overdraw" );
		RC<Buffer>			obj_buf			= Buffer();										obj_buf.Name( "Buildings buf" );
		RC<Buffer>			lights_buf		= Buffer();										lights_buf.Name( "Lights buf" );
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_lights	= Scene();
		RC<Scene>			scene_defer_lights1 = Scene();
		RC<Scene>			scene_defer_lights2 = Scene();

		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt>		light_count		= DynamicUInt();
		RC<DynamicUInt>		mode			= DynamicUInt();
		RC<DynamicUInt>		view_mode		= DynamicUInt();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"
			"	float3	scale;"
			"	uint	color;",
			obj_count );

		lights_buf.ArrayLayout(
			"LightObject",
			"	float3	position;"
			"	float	angle;"
			"	float3	dir;"
			"	float	height;"
			"	float2	atten;"		// linear, quadratic
			"	uint	color;"
			"	float	brightness;",
			light_count );

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 1.0f );
			camera.FovY( 60.f );
			camera.ReverseZ( true );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
			scene_lights.Set( camera );
			scene_defer_lights1.Set( camera );
			scene_defer_lights2.Set( camera );
		}

		// create scene with buildings
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCube();

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( obj_count );
			cmd.PipelineHint( "opaque.GEqual" );	// reverseZ
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// create scene with lights
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_LightObjs",	lights_buf );
			{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 4;
				cmd.InstanceCount( light_count );
				cmd.PipelineHint( "light_bulb" );
				geometry.Draw( cmd );
			}{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 5;
				cmd.InstanceCount( light_count );
				cmd.PipelineHint( "light_cone" );
				geometry.Draw( cmd );
			}
			scene_lights.Add( geometry );
		}

		// create scene with light cones for deferred shading
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCone( 12 );

			RC<Buffer>		geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_LightObjs",	lights_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( light_count );
			geometry.Draw( cmd );

			scene_defer_lights1.Add( geometry );
		}

		// create scene with light billboards for deferred shading
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_LightObjs",	lights_buf );

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 4;
			cmd.InstanceCount( light_count );
			geometry.Draw( cmd );

			scene_defer_lights2.Add( geometry );
		}

		Slider( obj_count,		"ObjCount",		100,	400,		100 );
		Slider( light_count,	"LightCount",	100,	1024,		200 );
		Slider( mode,			"Mode",			0,		1 );				// light volumes, light billboards
		Slider( view_mode,		"View",			0,		3,			0 );	// 0 - combined, 1 - color, 2 - light, 3 - overdraw

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",	obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			RC<ComputePass>			pass = ComputePass( "", "PUT_LIGHTS" );
			pass.ArgInOut(	"un_Lights",	lights_buf );
			pass.Slider(	"iMaxHeight",	1.0,	20.0,	20.0 );
			pass.LocalSize( 64 );
			pass.DispatchThreads( light_count );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/StreetLights/Opaque.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/Opaque.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(0.0, 0) );		// write
		}

		// classic deferred
		{
			// not handled case when camera is inside light volume
			RC<SceneGraphicsPass>	pass = scene_defer_lights1.AddGraphicsPass( "light volumes" );
			pass.AddPipeline( "samples/DeferredShading-Volume.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredShading-Volume.as)
			pass.Output( "out_Color",		rt_light,	RGBA32f(0.0) );
			pass.Output(					ds );									// read-only
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn( "un_Depth",			ds,			Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene_defer_lights2.AddGraphicsPass( "light quads" );
			pass.AddPipeline( "samples/DeferredShading-Billboard.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredShading-Billboard.as)
			pass.Output( "out_Color",		rt_light,	RGBA32f(0.0) );
			pass.Output(					ds );									// read-only
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn( "un_Depth",			ds,			Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 1 );
		}

		// overdraw
		{
			// not handled case when camera is inside light volume
			RC<SceneGraphicsPass>	pass = scene_defer_lights1.AddGraphicsPass( "overdraw" );
			pass.AddPipeline( "samples/DeferredShading-Volume.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredShading-Volume.as)
			pass.Output( "out_Overdraw",	rt_ovd,		RGBA32f(0.0) );
			pass.Output(					ds );									// read-only
			pass.Constant( "OVERDRAW",		1 );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene_defer_lights2.AddGraphicsPass( "overdraw" );
			pass.AddPipeline( "samples/DeferredShading-Billboard.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredShading-Billboard.as)
			pass.Output( "out_Overdraw",	rt_ovd,		RGBA32f(0.0) );
			pass.Output(					ds );									// read-only
			pass.Constant( "OVERDRAW",		1 );
			pass.EnableIfEqual( mode, 1 );
		}

		{
			RC<Postprocess>			pass = Postprocess( "", "RESOLVE" );
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn(		"un_LightBuf",		rt_light,	Sampler_NearestClamp );
			pass.ArgIn(		"un_Overdraw",		rt_ovd,		Sampler_NearestClamp );
			pass.Slider(	"iLightScale",		1.0,	100.0,	10.0 );
			pass.Constant(	"iView",			view_mode );
			pass.Constant(	"iMaxOverdraw",		light_count );
		}{
			RC<SceneGraphicsPass>	pass = scene_lights.AddGraphicsPass( "translucent" );
			pass.AddPipeline( "samples/StreetLights/LightBulb.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightBulb.as)
			pass.AddPipeline( "samples/StreetLights/LightCone.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/LightCone.as)
			pass.Output( "out_Color",		rt );
			pass.OutputLS(					ds,			EAttachmentLoadOp::Load, EAttachmentStoreOp::None );
			pass.EnableIfLess( view_mode, 3 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(PUT_OBJECTS) or defined(PUT_LIGHTS)

	#include "samples/StreetLights.as"

#endif
//-----------------------------------------------------------------------------
#ifdef RESOLVE
	#include "Color.glsl"
	#include "ColorSpace.glsl"
	#include "ToneMapping.glsl"

	void Main ()
	{
		float3	light	= gl.texture.Fetch( un_LightBuf, int2(gl.FragCoord.xy), 0 ).rgb * iLightScale;
		float3	albedo	= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float3	ambient	= RemoveSRGBCurve( float3(54.0, 61.0, 75.0) / 255.0 );

		switch ( iView )
		{
			case 0 :
				out_Color = ApplySRGBCurve( float4( albedo * (light + ambient), 1.0 ));
				break;

			case 1 :
				out_Color = float4( albedo, 1.0 );
				break;

			case 2 :
				out_Color = float4( light, 1.0 );
				if ( Any( IsNaN( light )))
					out_Color = float4(1.0, 0.0, 1.0, 1.0);
				break;

			case 3 :
			{
				float	cnt = gl.texture.Fetch( un_Overdraw, int2(gl.FragCoord.xy), 0 ).r;
				out_Color = Rainbow2( 1.0 - Saturate( cnt * iLightScale / iMaxOverdraw ));
				out_Color.a = cnt;
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
