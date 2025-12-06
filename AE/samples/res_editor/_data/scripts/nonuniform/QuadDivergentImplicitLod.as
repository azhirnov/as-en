// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TODO: flickering on Mac
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define RAYTRACE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 1000.f );
			camera.FovY( 70.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		const uint2			img_dim		= uint2(128);
		const uint			img_count	= 16;
		array<RC<Image>>	images;
		RC<Image>			img_arr		= Image( EPixelFormat::RGBA8_UNorm, img_dim, ImageLayer(img_count), MipmapLevel(~0) );

		{
			for (uint i = 0; i < img_count; ++i){
				images.push_back( Image( EPixelFormat::RGBA8_UNorm, img_dim, MipmapLevel(~0) ));
			}

			RC<Collection>	args = Collection();

			args.Add( "array",	images );
			args.Add( "layers",	img_arr );

			RunScript( "NonUniform-GenImages.as", ScriptFlags::RunOnce, args );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/callable/NonUniform-GenImages.as)
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "RAYTRACE" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_Textures",		images,		Sampler_LinearMipmapRepeat );
			pass.Slider( "iUVScale",		0.5,				4.0,			2.0 );
			pass.Slider( "iHScaleBias",		float2(1.0, 0.0),	float2(10.0),	float2(1.0, 0.0) );
			pass.Slider( "iMode",			0,					3 );
			pass.Slider( "iCmp",			0,					2,				2 );	// 0 - implicit derivatives, 1 - explicit derivatives, 2 - difference
			pass.Slider( "iCmpScale",		0,					10,				10 );
			pass.Constant( "iMipCount",		float(img_arr.MipmapCount()) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RAYTRACE
	#include "Ray.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "TexSampling.glsl"
	#include "Intersectors.glsl"

	float4  LodToColor (float lod)
	{
		float4	c0	= Rainbow( Floor(lod) / iMipCount );
		float4	c1	= Rainbow( Ceil(lod) / iMipCount );
		return Lerp( c0, c1, Fract(lod) );
	}

	#ifdef AE_nonuniform_qualifier
	#	define NONUNIFORM(x)	gl::Nonuniform(x)
	#else
	#	define NONUNIFORM(x)	(x)
	#endif


	void Main ()
	{
		float2	uv = gl.FragCoord.xy * un_PerPass.invResolution;

		bool	isec;
		{
			Ray	ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, 1.0, uv );

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

			Ray_SetLength( ray, t );

			uv = ray.pos.xz * iUVScale;
		}

		uint	tex_id	= uint(DHash12( (gl.FragCoord.xy + iHScaleBias.y) * iHScaleBias.x ) * float(un_Textures.length()));

		// may have undefined derivatives if 'quadDivergentImplicitLod = false'
		float2	lod1	= gl.texture.QueryLod( un_Textures[NONUNIFORM(tex_id)], uv );
		float4	col1	= gl.texture.Sample( un_Textures[NONUNIFORM(tex_id)], uv );

		float2	lod3	= SwQueryLod( un_Textures[NONUNIFORM(tex_id)], uv, 0.0 );

		float2	dx		= gl.dFdx( uv );
		float2	dy		= gl.dFdy( uv );

		// reference
		float2	lod2	= gl.texture.QueryLod( un_Textures[0], uv );
		float4	col2	= gl.texture.SampleGrad( un_Textures[NONUNIFORM(tex_id)], uv, dx, dy );

		switch ( iMode )
		{
			case 0 :	break;
			case 1 :	col1 = LodToColor(lod1.x);	col2 = LodToColor(lod2.x);  break;
			case 2 :	col1 = LodToColor(lod1.y);	col2 = LodToColor(lod2.y);  break;
			case 3 :	col1 = LodToColor(lod3.y);	col2 = LodToColor(lod2.y);  break;
		}

		float	scale = Exp10( iCmpScale );
		switch ( iCmp )
		{
			case 0 :	out_Color = col1;	break;
			case 1 :	out_Color = col2;	break;
			case 2 :	out_Color = (col1 - col2) * scale; break;
		}

		if ( ! isec )
			out_Color = float4(0.25);
	}

#endif
//-----------------------------------------------------------------------------
