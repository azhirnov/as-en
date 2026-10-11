// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Render scene from 3 perspective cameras and apply projections (panini/spherical)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define SETUP_CAMERA
#	define REPROJECT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void AddScenePass (RC<Scene> &in scene0, RC<Scene> &in scene2, RC<DynamicUInt> &in mode, RC<Image> &in rt, RC<Image> &in ds, RC<Buffer> &in cbuf, uint cameraId)
	{
		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "scene opaque" );
			pass.AddPipeline( "samples/Model-Cubemap.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model-Cubemap.as)
			pass.Output( "out_Color",		rt, RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			pass.Output(					ds, DepthStencil(1.f, 0) );
			pass.ArgIn(  "un_CBuf",			cbuf );
			pass.Constant( "iUseCameraPos",	1 );
			pass.Constant( "iCameraIdx",	cameraId );
			pass.Layer( ERenderLayer::Opaque );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "grid" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color",		rt, RGBA32f(0.1, 0.1, 0.2, 1.0) );
			pass.Output(					ds, DepthStencil(1.f, 0) );
			pass.ArgIn(  "un_CBuf",			cbuf );
			pass.Constant( "iCameraIdx",	cameraId );
			pass.Layer( ERenderLayer::Opaque );
			pass.EnableIfEqual( mode, 1 );
		}
	}

	void ASmain ()
	{
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Scene>			scene0			= Scene();
		RC<Scene>			scene2			= Scene();
		RC<FPVCamera>		camera			= FPVCamera();
		RC<Buffer>			cbuf			= Buffer();
		RC<DynamicUInt>		mode			= DynamicUInt();

		RC<DynamicUInt2>	side_dim_scale	= DynamicUInt2();
		RC<DynamicUInt>		center_dim_scale= DynamicUInt();
		RC<DynamicDim>		side_dim		= side_dim_scale.Mul(uint2( 512 )).Dimension();
		RC<DynamicDim>		center_dim		= center_dim_scale.Mul( 512 ).Dimension2();

		RC<DynamicFloat>	side_fov		= DynamicFloat();
		RC<DynamicFloat>	center_fov		= DynamicFloat();

		RC<Image>			left_rt			= Image( EPixelFormat::RGBA8_UNorm, side_dim );
		RC<Image>			right_rt		= Image( EPixelFormat::RGBA8_UNorm, side_dim );
		RC<Image>			center_rt		= Image( EPixelFormat::RGBA8_UNorm, center_dim );

		RC<Image>			left_ds			= Image( EPixelFormat::Depth32F, side_dim );
		RC<Image>			right_ds		= Image( EPixelFormat::Depth32F, side_dim );
		RC<Image>			center_ds		= Image( EPixelFormat::Depth32F, center_dim );

		Slider( side_dim_scale,		"SideDim",			uint2(1),	uint2(4),	uint2(2) );
		Slider( center_dim_scale,	"CenterDim",		uint(1),	uint(8),	uint(2) );
		Slider( mode,				"Scene",			0,			1 );
		Slider( side_fov,			"SideCameraFov",	30.0,		90.0,		45.0 );
		Slider( center_fov,			"CenterCameraFov",	30.0,		90.0,		45.0 );

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );	// ignored

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s*2.0f, s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.RotationScale( 2.0, 1.0 );	// for wide angle

			camera.Dimension( rt.Dimension() );

			scene0.Set( camera );
		}

		cbuf.UseLayout( "Params",
			"float3x3	view [3];"
			"float4x4	proj [3];"
			"float4x4	cubemapViewProj [3];" );

		// setup model
		{
			RC<Model>	model = Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( Transform().Position( 0.f, 1.f, 0.f ).Rotation( 0.f, ToRad(90.f), ToRad(180.f) ).Scale( 100.f ));
			model.InstanceCount( 6 );

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene0.Add( model );
		}

		// scene
		{
			// create sphere
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddSphericalCube( 4 );

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometrySBlock" );

			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount		= mesh.IndexCount();
			cmd.instanceCount	= 6;
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.PipelineHint( "Grid" );

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	geom_data );

			scene2.Add( geometry );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "SETUP_CAMERA" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Params",		cbuf );
			pass.Constant(	"iSideCameraFov",	side_fov );
			pass.Constant(	"iCenterCameraFov",	center_fov );
			pass.Constant(	"iSideDim",			side_dim );
			pass.Constant(	"iCenterDim",		center_dim );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			AddScenePass( scene0, scene2, mode,  left_rt,   left_ds,   cbuf, 0 );
			AddScenePass( scene0, scene2, mode,  right_rt,  right_ds,  cbuf, 2 );
			AddScenePass( scene0, scene2, mode,  center_rt, center_ds, cbuf, 1 );
		}{
			array<RC<Image>>	images	= { left_rt, center_rt, right_rt };
			RC<Postprocess>		pass	= Postprocess( "", "REPROJECT" );
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_CameraRT",		images,	Sampler_LinearClamp );
			pass.ArgIn(		"un_Params",		cbuf );
			pass.Slider(	"iPaniniFovY",		10.0,	179.9,	114.0 );
			pass.Slider(	"iView",			0,		5,		4 );
			pass.Slider(	"iDensScale",		0.2,	4.0,	1.0 );
			pass.Constant(	"iSideDim",			side_dim );
			pass.Constant(	"iCenterDim",		center_dim );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SETUP_CAMERA
	#include "Matrix.glsl"

	void  Main ()
	{
		float	side_fov_y	= ToRad( iSideCameraFov );
		float	center_fov	= ToRad( iCenterCameraFov );
		float	side_aspect	= float(iSideDim.y) / float(iSideDim.x);

		float	side_fov_x	= 2.0 * ATan( Tan( side_fov_y * 0.5 ) * side_aspect );
		float	rot			= 0.5 * (center_fov + side_fov_x) - 1.0e-3;

		un_Params.view[0] = f3x3_RotateY( -rot );	// left
		un_Params.view[1] = f3x3_Identity();
		un_Params.view[2] = f3x3_RotateY(  rot );	// right

		un_Params.proj[0] = f4x4_Perspective( side_fov_y, side_aspect, un_PerPass.camera.clipPlanes );
		un_Params.proj[1] = f4x4_Perspective( center_fov, 1.0, un_PerPass.camera.clipPlanes );
		un_Params.proj[2] = un_Params.proj[0];

		un_Params.cubemapViewProj[0] = un_Params.proj[0] * float4x4(un_Params.view[0]) * un_PerPass.camera.view;
		un_Params.cubemapViewProj[1] = un_Params.proj[1] * float4x4(un_Params.view[1]) * un_PerPass.camera.view;
		un_Params.cubemapViewProj[2] = un_Params.proj[2] * float4x4(un_Params.view[2]) * un_PerPass.camera.view;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef REPROJECT
	#include "Ray.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"


	bool  ProjectDir (float3 dir, out int img_idx, out float2 uv)
	{
		float2	l_uv	= RayInverse_Perspective( un_Params.proj[0], un_Params.view[0] * dir );
		float2	c_uv	= RayInverse_Perspective( un_Params.proj[1], un_Params.view[1] * dir );
		float2	r_uv	= RayInverse_Perspective( un_Params.proj[2], un_Params.view[2] * dir );

		img_idx = -1;

		if ( IsUNorm( c_uv ))	{ uv = c_uv; img_idx = 1; }	else
		if ( IsUNorm( l_uv ))	{ uv = l_uv; img_idx = 0; }	else
		if ( IsUNorm( r_uv ))	{ uv = r_uv; img_idx = 2; }

		return img_idx >= 0;
	}

	float3  PaniniDir (float2 pix)
	{
		return Ray_PaniniProjection( ToRad(iPaniniFovY), float3(0.0), 0.0, pix, un_PerPass.resolution.xy ).dir;
	}

	float2  SourceDim (int img_idx)
	{
		return (img_idx == 1) ? float2(iCenterDim) : float2(iSideDim);
	}

	// texels moved per output pixel (aniso max)
	float  TexelDensity (int img_idx, float2 uv, int img_x, float2 uv_x, int img_y, float2 uv_y)
	{
		float2	ts	= SourceDim( img_idx );
		float2	dx	= (img_x == img_idx) ? (uv_x - uv) * ts : float2(0.0);
		float2	dy	= (img_y == img_idx) ? (uv_y - uv) * ts : float2(0.0);
		return Max( Length(dx), Length(dy) );
	}

	float3  DensityHeatmap (float d)
	{
		// log2: 0.25x -> red, 1x -> green, 4x -> blue
		float	t = Clamp( Log2( Max( d, 1.0e-4 )) / 2.0, -1.0, 1.0 );
		if ( t < 0.0 )
			return Lerp( float3(0.0, 1.0, 0.0), float3(1.0, 0.0, 0.0), -t );
		return Lerp( float3(0.0, 1.0, 0.0), float3(0.0, 0.4, 1.0), t );
	}

	float4  PaniniReproj ()
	{
		float2	pix	= gl.FragCoord.xy;
		int		idx;
		float2	uv;

		if ( ! ProjectDir( PaniniDir( pix ), OUT idx, OUT uv ))
			return float4(0.2);

		return gl.texture.Sample( un_CameraRT[ gl::Nonuniform( idx )], uv );
	}


	float4  PaniniTexelDensity ()
	{
		float2	pix = gl.FragCoord.xy;
		int		idx, idx_x, idx_y;
		float2	uv, uv_x, uv_y;

		if ( ! ProjectDir( PaniniDir( pix ), idx, uv ))
			return float4(0.2);

		ProjectDir( PaniniDir( pix + float2(1.0, 0.0) ), OUT idx_x, OUT uv_x );
		ProjectDir( PaniniDir( pix + float2(0.0, 1.0) ), OUT idx_y, OUT uv_y );

		float	dens = TexelDensity( idx, uv, idx_x, uv_x, idx_y, uv_y );
		return float4( DensityHeatmap( dens * iDensScale ), 1.0 );
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;

		switch ( iView )
		{
			case 0 :
				out_Color = gl.texture.Sample( un_CameraRT[0], uv );
				break;

			case 1 :
				out_Color = gl.texture.Sample( un_CameraRT[1], uv );
				break;

			case 2 :
				out_Color = gl.texture.Sample( un_CameraRT[2], uv );
				break;

			case 3 :
			{
				int	i = 2 - Clamp( int( uv.x * 3.0 ), 0, 2 );
				uv.x = Fract( uv.x * 3.0 );
				uv.y = ToUNorm( ToSNorm( uv.y ) * un_PerPass.resolution.y * 3.0 / un_PerPass.resolution.x );

				if ( IsUNorm( uv ))
					out_Color = gl.texture.Sample( un_CameraRT[ gl::Nonuniform( i )], uv );
				else
					out_Color = float4(0.2);
				break;
			}

			case 4 :
				out_Color = PaniniReproj();
				break;

			case 5 :
				out_Color = PaniniTexelDensity();
				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
