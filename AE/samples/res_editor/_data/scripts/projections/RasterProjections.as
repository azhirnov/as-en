// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Example of rasterization compatible projections: paraboloid, dual paraboloid.
	Also see [RenderToCubemap](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/projections/RenderToCubemap.as).
	Other projections use one of this 3 projections (paraboloid, dual, cubemap) and then add per-pixel reprojection.

	Dual paraboloid commonly used for omni shadow maps and reflection probes.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT-Color" );
		const uint2			dim				= uint2(1024 * 2);
		RC<Image>			col				= Image( EPixelFormat::RGBA8_UNorm, dim, ImageLayer(2) );	col.Name( "RT-Layered" );
		RC<Image>			ds				= Image( EPixelFormat::Depth32F, dim, ImageLayer(2) );		ds.Name( "RT-Depth" );
		RC<Scene>			scene			= Scene();
		RC<FPVCamera>		camera			= FPVCamera();
		RC<DynamicUInt>		proj			= DynamicUInt();
		RC<DynamicFloat>	overlap			= DynamicFloat();
		RC<DynamicFloat>	tess_level		= DynamicFloat();
		const uint			dual_pass_proj	= 0;	// for dual paraboloid

		// setup camera
		{

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 1.0f;
			camera.ForwardBackwardScale( s*2.0f, s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( rt.Dimension() );

			scene.Set( camera );
		}

		// setup model
		{
			RC<Model>	model	= Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( Transform().Position( 0.f, 1.f, 0.f ).Rotation( 0.f, ToRad(90.f), ToRad(180.f) ).Scale( 100.f ));

			scene.Add( model );
		}

		Slider( proj,		"Proj",			0,		2 );				// paraboloid, dual paraboloid, dp v2
		Slider( overlap,	"Overlap",		0.0,	1.0,	0.4 );		// only for dual paraboloid
		Slider( tess_level,	"TessLevel",	1.0,	64.0,	10.0 );

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color",		col,		ImageLayer(0),	RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			pass.Output(					ds,			ImageLayer(0),	DepthStencil(1.0, 0) );
			pass.Constant( "iProjection",	proj );
			pass.Constant( "iPassIdx",		0 );
			pass.Constant( "iOverlap",		overlap );
			pass.Constant( "iTessLevel",	tess_level );
			pass.Layer( ERenderLayer::Opaque );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque-2" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output( "out_Color",		col,		ImageLayer(1),	RGBA32f(0.0f, 1.f, 1.f, 1.f) );
			pass.Output(					ds,			ImageLayer(1),	DepthStencil(1.0, 0) );
			pass.Constant( "iProjection",	proj );
			pass.Constant( "iPassIdx",		1 );
			pass.Constant( "iOverlap",		overlap );
			pass.Constant( "iTessLevel",	tess_level );
			pass.Layer( ERenderLayer::Opaque );
			pass.EnableIfGreater( proj, dual_pass_proj );
		}{
			RC<Postprocess>			pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_ColorTex",		col,	Sampler_LinearClamp );
			pass.ArgIn(  "un_DepthTex",		ds,		Sampler_LinearClamp );
			pass.Slider( "iView",			0,		2,		2 );		// color, depth, reproject
			pass.Constant( "iProjection",	proj );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Ray.glsl"


	float3  ProjectUV (float2 uv)
	{
		switch ( iProjection )
		{
			case 0 :	return float3(uv, 0.0);

			case 1 :
			case 2 :
			{
				float2	uv1 = uv;
				uv1.x = Fract( uv1.x * 2.0 );
				return float3( uv1, GreaterF( uv.x, 0.5 ));		// left - front
			}
		}
	}


	float3  Project3D (float2 uv)
	{
		Ray	ray = Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), un_PerPass.camera.clipPlanes.x, uv );

		switch ( iProjection )
		{
			case 0 :	return float3( RayInverse_Paraboloid( ray.dir ), 0.0 );
			case 1 :
			case 2 :	return RayInverse_DualParaboloid( ray.dir );
		}
	}


	void  Main ()
	{
		float2	uv	= gl.FragCoord.xy * un_PerPass.invResolution;
		float3	uvw = ProjectUV( uv );

		switch ( iView )
		{
			case 0 :	// color
				out_Color = gl.texture.Sample( un_ColorTex, uvw );  break;

			case 1 :	// depth
				out_Color = float4( gl.texture.Sample( un_DepthTex, uvw ).rrr, 1.0 );  break;

			case 2 :	// reproject
			{
				uvw = Project3D( uv );
				out_Color = gl.texture.Sample( un_ColorTex, uvw );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
