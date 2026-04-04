// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*

*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_PARAMS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	RC<Image>			rt;
	RC<Image>			ds;
	RC<FPVCamera>		camera;
	RC<Buffer>			params_buf;

	RC<DynamicUInt>		mode;


	void  DrawCone ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create cone
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCone( 32 );

			RC<Buffer>	cone = mesh.ToBuffer();
			cone.LayoutName( "GeometrySBlock" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	cone );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( cone, "indices" );
			cmd.PipelineHint( "cone" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw cone" );
			draw.AddPipeline( "tests/Cone.as" );
			draw.Output( "out_Color",	rt, RGBA32f(0.0) );
			draw.Output(				ds, DepthStencil( 1.f, 0 ));
			draw.ArgIn(	"un_Params",	params_buf );
		}
	}


	void  DrawSphere ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create sphere
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphere( 3 );

			RC<Buffer>	sphere = mesh.ToBuffer();
			sphere.LayoutName( "GeometrySBlock" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	sphere );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( sphere, "indices" );
			cmd.PipelineHint( "box" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw sphere" );
			draw.AddPipeline( "tests/Cone.as" );
			draw.Output( "out_Color",	rt );
			draw.Output(				ds );
			draw.ArgIn(	"un_Params",	params_buf );
			draw.EnableIfEqual( mode, 1 );
		}
	}


	void  DrawBox ()
	{
		RC<Scene>		scene = Scene();
		scene.Set( camera );

		// create cone
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCube();

			RC<Buffer>	cube = mesh.ToBuffer();
			cube.LayoutName( "GeometrySBlock" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	cube );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( cube, "indices" );
			cmd.PipelineHint( "box" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw box" );
			draw.AddPipeline( "tests/Cone.as" );
			draw.Output( "out_Color",	rt );
			draw.Output(				ds );
			draw.ArgIn(	"un_Params",	params_buf );
			draw.EnableIfEqual( mode, 2 );
		}
	}


	void ASmain ()
	{
		// initialize
		RC<DynamicDim>	dim = SurfaceSize();

		@rt				= Image( EPixelFormat::RGBA8_UNorm, dim );	rt.Name( "RT-Color" );
		@ds				= Image( EPixelFormat::Depth32F, dim );		ds.Name( "RT-Depth" );
		@params_buf		= Buffer();
		@mode			= DynamicUInt();

		params_buf.UseLayout(
			"Params",
			"	float3	coneOrigin;"
			"	float	coneHeight;"
			"	float3	coneDir;"
			"	float	coneAngle;"
			"	float3	boxCenter;"
			"	float3	boxHalfSize;"
			"	float	boxAlpha;"
		);

		// setup camera
		{
			@camera = FPVCamera();

			camera.ClipPlanes( 0.5f, 200.f );
			camera.FovY( 60.f );

			const float	s = 4.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( dim );
		}

		Slider( mode,	"Mode",		0,	2 );		// Bounding shape: 1 - Sphere, 2 - AABB

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT_PARAMS" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Params",	params_buf );
			pass.Slider(	"iPos",			float3(-1.0),	float3(1.0),	float3(0.0, 0.0, -0.8) );
			pass.Slider(	"iRotation",	float2(-1.0),	float2(1.0),	float2(0.0) );
			pass.Slider(	"iAngle",		0.01,			1.0,			0.3 );
			pass.Slider(	"iHeight",		0.01,			1.0,			0.4 );
			pass.Slider(	"iAlpha",		0.1,			1.0,			0.4 );
			pass.Constant(	"iMode",		mode );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			DrawCone();
			DrawSphere();
			DrawBox();
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT_PARAMS
	#include "Matrix.glsl"
	#include "Geometry3D.glsl"
	#include "Quaternion.glsl"

	void  Main ()
	{
		Quat	q	= QRotationX( float_Pi );
		q = QMul( q, QRotationZ( float_Pi * iRotation.x ));
		q = QMul( q, QRotationX( float_Pi * iRotation.y * 0.5 ));
		q = QNormalize( q );

		float	view_z	= Lerp( un_PerPass.camera.clipPlanes.x, un_PerPass.camera.clipPlanes.y, ToUNorm(iPos.z) );
		float	proj_z	= FastProjectZ( un_PerPass.camera.proj, view_z );
		float	max_h	= view_z * Tan( MaxOf( un_PerPass.camera.fov ) * 0.5 );

		un_Params.coneOrigin	= UnProjectNDC( MatInverse(un_PerPass.camera.proj), float3(iPos.xy, proj_z) );
		un_Params.coneDir		= -QMul( q, float3(0.0, 1.0, 0.0) );
		un_Params.coneAngle		= iAngle * float_Pi;
		un_Params.coneHeight	= max_h * iHeight;
		un_Params.boxAlpha		= iAlpha;

		Cone	cone = Cone_Create( un_Params.coneOrigin, un_Params.coneDir, un_Params.coneAngle, un_Params.coneHeight );

		switch ( iMode )
		{
			case 1 :
			{
				Sphere	sphere = Cone_ToBoundingSphere( cone );

				un_Params.boxCenter		= sphere.center;
				un_Params.boxHalfSize	= float3( sphere.radius );
				break;
			}

			case 2 :
			{
				AABB	box = Cone_ToBoundingBox( cone );

				un_Params.boxCenter		= AABB_Center( box );
				un_Params.boxHalfSize	= AABB_HalfSize( box );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
