// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<Image>		sdf_font_32		= Image( EImageType::FImage2D, "res/font/sdf-32.dds" );
		RC<Image>		mc_sdf_font_32	= Image( EImageType::FImage2D, "res/font/mc-sdf-32.dds" );
		RC<Image>		mc_sdf_font_64	= Image( EImageType::FImage2D, "res/font/mc-sdf-64.dds" );
		RC<FPVCamera>	camera			= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::None );
			pass.Set( camera );
			pass.ArgIn( "un_SdfFont_32",		sdf_font_32,		Sampler_LinearRepeat );
			pass.ArgIn( "un_McSdfFont_32",		mc_sdf_font_32,		Sampler_LinearRepeat );
			pass.ArgIn( "un_McSdfFont_64",		mc_sdf_font_64,		Sampler_LinearRepeat );
			pass.Output( "out_Color",			rt );

			pass.Slider( "iSdfTex",		0,		2 );
			pass.Slider( "iScale",		0.1,	8.0,	1.0 );
			pass.Slider( "iThick",		0.0,	5.0,	1.0 );

			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Ray.glsl"
	#include "Intersectors.glsl"

	// Result must be >= 1, AA will work if >= 2
	float  ScreenPxRange (gl::CombinedTex2D<float> msdfTex, float2 uv, float pxRange)
	{
		float2	unit_range		= float2(pxRange) / float2(gl.texture.GetSize( msdfTex, 0 ));
		float2	src_tex_size	= float2(1.0) / gl.fwidth( uv );
		return Max( 0.5 * Dot( unit_range, src_tex_size ), 1.0 );
	}


	float3  SdfFont (gl::CombinedTex2D<float> sdfTex, const float2 uv, float sdfScale, float sdfBias)
	{
		float2	size	= float2(gl.texture.GetSize( sdfTex, 0 ).xy);
		float2	dx		= Abs( gl.dFdxFine( uv ));
		float2	dy		= Abs( gl.dFdyFine( uv ));
		float2	md		= Max( dx, dy );		// minimal distance for 1px

		float	sd		= gl.texture.SampleLod( sdfTex, uv, 0.0 ).r;
				sd		= FusedMulAdd( sd, sdfScale, sdfBias );

		float2	t		= iThick * md * size;

		sd = MinOf( 1.0 - SmoothStep( float2(sd), -t, t ));

		sd *= (1.0 - SmoothStep( Length( md ), 0.006, 0.02 ));

		return float3(1.0, 0.3, 0.3) * sd;
	}


	float3  McSdfFont (gl::CombinedTex2D<float> msdfTex, const float2 uv, float sdfScale, float sdfBias)
	{
		float2	size	= float2(gl.texture.GetSize( msdfTex, 0 ).xy);
		float2	dx		= Abs( gl.dFdxFine( uv ));
		float2	dy		= Abs( gl.dFdyFine( uv ));
		float2	md		= Max( dx, dy );		// minimal distance for 1px

		float3	msd		= gl.texture.SampleLod( msdfTex, uv, 0.0 ).rgb;
		float	sd		= MCSDF_Median( msd );
				sd		= FusedMulAdd( sd, sdfScale, sdfBias );

		float2	t		= iThick * md * size;

		sd = MinOf( 1.0 - SmoothStep( float2(sd), -t, t ));

		sd *= (1.0 - SmoothStep( Length( md ), 0.006, 0.02 ));

		return float3(0.0, 1.0, 0.0) * sd;
	}


	void Main ()
	{
		Ray	ray = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, gl.FragCoord.xy / un_PerPass.resolution.xy );

		out_Color = float4(1.0);

		float	t;
		bool	isec = Plane_Ray_Intersect( ray, float3(0.0, 0.0, 2.0), float3(0.0, 0.0, 1.0), OUT t );

		Ray_SetLength( ray, t );

		float2	uv	= ray.pos.xy * iScale;
				uv.x += TriangleWave( un_PerPass.time * 0.1 );

		switch ( iSdfTex )
		{
			case 0 :	out_Color.rgb = SdfFont(	un_SdfFont_32,		uv, -20.1597252, 17.5723000 );	break;
			case 1 :	out_Color.rgb = McSdfFont(	un_McSdfFont_32,	uv, -41.1041679, 21.1272316 );	break;
			case 2 :	out_Color.rgb = McSdfFont(	un_McSdfFont_64,	uv, -40.4665451, 20.5937500 );	break;
		}
		if ( ! isec )
			out_Color = float4(0.0);
	}

#endif
//-----------------------------------------------------------------------------
