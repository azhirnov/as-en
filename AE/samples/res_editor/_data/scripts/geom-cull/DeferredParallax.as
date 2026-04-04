// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw cube with parallax mapping using deferred texturing.

	Tangent frame packing from [Siggraph2015: GPU-Driven Rendering Pipelines](https://advances.realtimerendering.com/s2015/aaltonenhaar_siggraph2015_combined_final_footer_220dpi.pdf).
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>			gbuf_uv		= Image( EPixelFormat::RGBA16F, SurfaceSize() );	gbuf_uv.Name( "GBuf-UV" );
		RC<Image>			gbuf_quat	= Image( EPixelFormat::R32U, SurfaceSize() );		gbuf_quat.Name( "GBuf-PackedQuat" );	// TBN as quaternion
		RC<Image>			gbuf_norm	= Image( EPixelFormat::RGBA32F, SurfaceSize() );	gbuf_norm.Name( "GBuf-RefNorm" );		// full TBN
		RC<Image>			gbuf_tan	= Image( EPixelFormat::RGBA32F, SurfaceSize() );	gbuf_tan.Name( "GBuf-RefTangent" );		// full TBN
		RC<Image>			ds			= Image( EPixelFormat::Depth32F, SurfaceSize() );
		RC<Scene>			scene		= Scene();
		RC<OrbitalCamera>	camera		= OrbitalCamera();

		RC<Image>		color_map	= Image( EImageType::Float_2D, "res/tex/rocks_color_rgba.ktx" );
		RC<Image>		norm_h_map	= Image( EImageType::Float_2D, "res/tex/rocks_normal_height_rgba.ktx" );
						norm_h_map.SetSwizzle( "ARGB" );	// R - height, GBA - normal

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );
			camera.Offset( 3.f );
			camera.OffsetScale( 10.0f );

			scene.Set( camera );
		}

		// create cube
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position | EAttribute::Texcoord2D | EAttribute::TBN | EAttribute::TBN_Quat );
			mesh.AddCube();

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.Draw( cmd );
			geometry.ArgIn(	"un_Geometry",	geom_data );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "defer tex" );
			pass.AddPipeline( "samples/DeferredParallax.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredParallax.as)
			pass.Output( "out_GBufUV",		gbuf_uv,	RGBA32f(-1.0) );
			pass.Output( "out_GBufQuat",	gbuf_quat,	RGBA32u(~0) );
			pass.Output( "out_GBufNorm",	gbuf_norm,	RGBA32f(0.0) );
			pass.Output( "out_GBufTan",		gbuf_tan,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil( 1.f, 0 ));
			pass.Constant( "iTexDim",		color_map.Dimension() );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",			rt );
			pass.ArgIn(  "un_GBufUV",			gbuf_uv,	Sampler_NearestClamp );
			pass.ArgIn(  "un_GBufQuat",			gbuf_quat,	Sampler_NearestClamp );
			pass.ArgIn(  "un_GBufNorm",			gbuf_norm,	Sampler_NearestClamp );
			pass.ArgIn(  "un_GBufTan",			gbuf_tan,	Sampler_NearestClamp );
			pass.ArgIn(	 "un_ColorMap",			color_map,	Sampler_LinearMipmapRepeat );
			pass.ArgIn(	 "un_HeightNormalMap",	norm_h_map,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iTBN_Cmp",			0,		2 );	// packed quat, matrix, compare both
			pass.Slider( "iCmpScale",			1,		8,		3 );
			pass.Slider( "iMode",				0,		3,		3 );
			pass.Slider( "iShowNormals",		0,		1 );
			pass.Slider( "iHeightScale",		0.f,	0.1f,	0.1f );
			pass.Constant( "iLightDir",			Normalize(float3( 0.f, -1.f, 0.f )) );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Matrix.glsl"
	#include "GBuffer.glsl"
	#include "ReliefMapping.glsl"


	#define ExtractNormal( _normalMap_, _uv_ )		(ToSNorm(gl.texture.SampleLod( _normalMap_, _uv_, texLod ).yzw) * float3(1.f, -1.f, 1.f))

	float3  ReliefMapping (const float2 uv, const float texLod, const float3 viewDir,
						   const float3x3 TBN, const Quat qTBN, const bool useTBNMat,
						   const float3 lightDir)
	{
		float3			tan_view_dir;
		float2			distorted_uv	= uv;
		const int		num_layers		= 64;

		if ( useTBNMat )
			tan_view_dir = Normalize( MatTranspose(TBN) * viewDir );
		else
		{
			tan_view_dir = Normalize( QMul( qTBN, viewDir ));
			tan_view_dir.y *= -1.0;  // fix for left-handed TBN
		}

		switch ( iMode )
		{
			case 0 :	break; // normal mapping
			case 1 :	distorted_uv = ParallaxMapping( un_HeightNormalMap, uv, tan_view_dir, iHeightScale, -0.02f );				break;
			case 2 :	distorted_uv = SteepParallaxMapping( un_HeightNormalMap, uv, tan_view_dir, num_layers, iHeightScale );		break;
			case 3:		distorted_uv = ParallaxOcclusionMapping( un_HeightNormalMap, uv, tan_view_dir, num_layers, iHeightScale );	break;
		}

		float3			normal			= ExtractNormal( un_HeightNormalMap, distorted_uv );
		const float3	tex_color		= gl.texture.SampleLod( un_ColorMap, distorted_uv, texLod ).rgb;
		const bool		left_src		= gl.FragCoord.x < un_PerPass.resolution.x*0.5;

		if ( useTBNMat )
			normal = Normalize( TBN * normal );
		else
		{
			normal = Normalize( QMul( qTBN, normal ));
			normal.y *= -1.0;  // fix for left-handed TBN
		}

		const float3	halfway_dir		= Normalize( lightDir + viewDir );
		const float3	ambient			= 0.2 * tex_color;
		const float3	diffuse			= Max( Dot(lightDir, normal), 0.f ) * tex_color;
		const float3	specular		= float3(0.2) * Pow( Max( Dot(normal, halfway_dir), 0.f ), 32.f );
		const float		shadow			= 1.0;
		const float3	color			= ambient + (diffuse * shadow) + (specular * shadow);

		return Lerp( color, (left_src ? normal : -normal), float(iShowNormals) );
	}


	void  Main ()
	{
		const float3	uv_lod = gl.texture.Fetch( un_GBufUV, int2(gl.FragCoord.xy), 0 ).rgb;

		if ( AnyLess( uv_lod.xy, float2(0.0) ))
		{
			// sky
			out_Color = float4( 0.3, 0.5, 1.0, 1.0 );
			return;
		}

		const float3	view_dir = ViewDir( un_PerPass.camera.invViewProj, gl.FragCoord.xy * un_PerPass.invResolution );
		float3x3		ref_tbn	 = float3x3( gl.texture.Fetch( un_GBufTan, int2(gl.FragCoord.xy), 0 ).rgb,
											 float3(0.0),
											 gl.texture.Fetch( un_GBufNorm, int2(gl.FragCoord.xy), 0 ).rgb );
						ref_tbn[1] = Normalize( Cross( ref_tbn[0], ref_tbn[2] ));

		const Quat		quat_tbn = DecodeQuat32( gl.texture.Fetch( un_GBufQuat, int2(gl.FragCoord.xy), 0 ).r );

		float3			ref_color = ReliefMapping( uv_lod.xy, uv_lod.z, view_dir, ref_tbn, quat_tbn, true, iLightDir );
		float3			color	  = ReliefMapping( uv_lod.xy, uv_lod.z, view_dir, ref_tbn, quat_tbn, false, iLightDir );

		switch ( iTBN_Cmp )
		{
			case 0 :	out_Color.rgb = color;		break;
			case 1 :	out_Color.rgb = ref_color;  break;
			case 2 :	out_Color.rgb = Abs( ref_color - color ) * Exp10( float(iCmpScale) );  break;
		}

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
