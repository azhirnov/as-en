// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Test software anisotropy between mipmaps.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_IMAGE
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Buffer>			buf			= Buffer();
		const uint			dim			= 32;
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>			tex			= Image( EPixelFormat::RGBA16F, uint2(dim), MipmapLevel(2) );
		array<RC<Image>>	mipmaps;	tex.GetMipmapsView( OUT mipmaps );
		array<RC<Image>>	textures;

		textures.push_back( mipmaps[0] );	// mip 0
		textures.push_back( mipmaps[1] );	// mip 1
		textures.push_back( tex );			// 2 mips

		RC<DynamicUInt>		bright_border	= DynamicUInt();

		RC<DynamicInt2>		duv_scale		= DynamicInt2();
		RC<DynamicFloat2>	duv_scale2		= DynamicFloat2();
		RC<DynamicFloat2>	duv_scalef		= duv_scale.ToFloat().Add( duv_scale2 ).Exp2();
		RC<DynamicFloat>	nominator		= duv_scalef.X().Sub( duv_scalef.Y() ).Abs();
		RC<DynamicFloat>	denom			= duv_scalef.X().Add( duv_scalef.Y() );
		RC<DynamicFloat>	angle			= nominator.Div( denom ).ACos().ToDeg().Sub( 90.0 ).Abs();

		buf.UseLayout(
			"OutputBuffer",
			"	float2	level;"
		);

		Slider( bright_border,	"Border",		0,				dim/2,			2 );
		Slider( duv_scale,		"DUVScale",		int2(0),		int2(12),		int2(0) );	// LOD depends on screen resolution
		Slider( duv_scale2,		"dUVFract",		float2(0.0),	float2(1.0),	float2(0.0) );

		// render loop
		{
			// select texel
			RC<ComputePass>		pass = ComputePass( "", "GEN_IMAGE" );
			pass.ArgOut(	"un_OutImages",		mipmaps );
			pass.Constant(	"iBorder",			bright_border );
			pass.Slider(	"iBrightValue",		1.0,		5.0,		1.5 );
			pass.LocalSize( 16, 16 );
			pass.DispatchThreads( dim, dim );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",		rt,			RGBA32f(0.0) );
			pass.ArgIn(		"un_Bilinear",		textures,	Sampler_NearestMirrorRepeat );
			pass.ArgIn(		"un_Trilinear",		textures,	Sampler_LinearMipmapMirrorRepeat );
			pass.ArgIn(		"un_Anisotropy2",	textures,	Sampler_Anisotropy2MirrorRepeat );
			pass.ArgIn(		"un_Anisotropy4",	textures,	Sampler_Anisotropy4MirrorRepeat );
			pass.ArgIn(		"un_Anisotropy8",	textures,	Sampler_Anisotropy8MirrorRepeat );
			pass.ArgIn(		"un_Anisotropy16",	textures,	Sampler_Anisotropy16MirrorRepeat );
			pass.ArgOut(	"un_OutBuf",		buf );
			pass.Slider(	"iFilter",			0,			5,			5 );
			pass.Slider(	"iCmp",				0,			3,			0 );
			pass.Slider(	"iChannel",			-1,			2,			-1 );
			pass.Constant(	"iDUVScale",		duv_scalef );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}

		RC<DynamicFloat2>	level = DynamicFloat2();
		ReadBuffer( level, buf, "level" );

		Label( duv_scalef,	"dUV scale" );
		Label( level,		"Tex LOD level" );
		Label( angle,		"Grazing angle (deg)" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_IMAGE
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const int2	coord = GetGlobalCoord().xy;

		for (int level = 0; level < un_OutImages.length(); ++level)
		{
			int2	dim	= GetGlobalSize().xy >> level;

			if ( AllLess( coord, dim ))
			{
				float4	color = float4(0.2);

				if ( (coord.x < iBorder or coord.x > dim.x - iBorder - 1) or
					 (coord.y < iBorder or coord.y > dim.y - iBorder - 1))
				{
					color = float4(0.1);
					color[ level%3 ] = iBrightValue;
				}

				gl.image.Store( un_OutImages[level], coord, color );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "TexSampling.glsl"
	#include "InvocationID.glsl"


	float2x3  Sample (gl::CombinedTex2D<float> tex[3], float2 uv, float2 dx, float2 dy)
	{
		float3	hw = gl.texture.SampleGrad( tex[2], uv, dx, dy ).rgb;
		float3	sw;
		{
			float2	dim		= float2(gl.texture.GetSize( tex[0], 0 ));
			float2	lods	= SwQueryLod( dim, float4(dx, dy) );
			float	lod		= Saturate( lods.y );

			float3	a		= gl.texture.SampleGrad( tex[0], uv, dx, dy ).rgb;	// mip 0
			float3	b		= gl.texture.SampleGrad( tex[1], uv, dx, dy ).rgb;	// mip 1
					sw		= Lerp( a, b, lod );
		}
		return float2x3( hw, sw );
	}


	float3  Swizzle (float3 origin)
	{
		switch ( iChannel )
		{
			case 0 :	return origin.rrr;
			case 1 :	return origin.ggg;
			case 2 :	return origin.bbb;
		}
		return origin;
	}


	void  Main ()
	{
		float2	uv		= ToUNorm( GetGlobalCoordSNormCorrected2() );
		float2	uv2		= ToUNorm( GetGlobalCoordSNormCorrected2( int2(1) ));

		float2	dx		= gl.dFdx( uv ) * iDUVScale.x;
		float2	dy		= gl.dFdy( uv ) * iDUVScale.y;

		if ( IsNotUNorm( uv ))
		{
			out_Color = float4(0.0);
			return;
		}

		float2		dim		= float2(gl.texture.GetSize( un_Bilinear[0], 0 ));
		int2		coord	= int2(uv * dim);
		float		grid	= AnyNotEqual( coord, int2(uv2 * dim) ) ? 0.0 : 1.0;
		float2x3	v		= float2x3(0.0);

		switch ( iFilter )
		{
			case 0 :	v = Sample( un_Bilinear, uv, dx, dy ); break;
			case 1 :	v = Sample( un_Trilinear, uv, dx, dy ); break;
			case 2 :	v = Sample( un_Anisotropy2, uv, dx, dy ); break;
			case 3 :	v = Sample( un_Anisotropy4, uv, dx, dy ); break;
			case 4 :	v = Sample( un_Anisotropy8, uv, dx, dy ); break;
			case 5 :	v = Sample( un_Anisotropy16, uv, dx, dy ); break;
		}

		switch ( iCmp )
		{
			case 0 :	// reference
				out_Color.rgb = Swizzle( v[0] );
				if ( AllLess( Abs(out_Color.rgb - float3(0.2)), 0.001 ))
				{
					out_Color.rgb = float3(0.8);
					if ( iChannel != -1 )
						out_Color.rgb = float3(0.0, 0.8, 0.0);
				}
				else
				if ( AnyGreater( out_Color.rgb, 0.2 ) and iChannel != -1 )
					out_Color.rgb = float3(0.8, 0.0, 0.0);
				break;

			case 1 :	// per channel
				out_Color.rgb = Swizzle( v[1] );
				 break;

			case 2 :	// diff
				out_Color.rgb = float3(Abs( v[0] - v[1] ));
				 break;

			case 3 :	// red color if not match with HW filter (need mip2+ ?)
				out_Color.rgb = MaxOf( Abs( (v[0] - v[1]) / v[0] )) > 0.01 ? float3(1.0, 0.0, 0.0) : float3(0.2);
				break;
		}

		out_Color.rgb *= grid;
		out_Color.a = 1.0;

		if ( AllEqual( int2(gl.FragCoord.xy), int2(un_PerPass.resolution.xy)/2 ))
		{
			un_OutBuf.level = SwQueryLod( dim, float4(dx,dy) );
		}
	}

#endif
//-----------------------------------------------------------------------------
