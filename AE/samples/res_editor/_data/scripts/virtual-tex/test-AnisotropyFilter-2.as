// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Test minimal tile border for filtering.
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
		RC<Image>			tex			= Image( EPixelFormat::R16F, uint2(dim), MipmapLevel(2) );
		array<RC<Image>>	mipmaps;	tex.GetMipmapsView( OUT mipmaps );
		array<RC<Image>>	textures;

		textures.push_back( mipmaps[0] );	// only first mip
		textures.push_back( tex );			// 2 mips

		RC<DynamicUInt>		bright_border	= DynamicUInt();
		RC<DynamicUInt>		tile_border		= DynamicUInt();
		RC<DynamicUInt>		border_sum		= bright_border.Add( tile_border );

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

		Slider( bright_border,	"BrightBorder",		0,				dim/2,			2 );
		Slider( tile_border,	"TileBorder",		0,				dim/2,			4 );
		Slider( duv_scale,		"dUVScale",			int2(0),		int2(12),		int2(0) );	// LOD depends on screen resolution
		Slider( duv_scale2,		"dUVFract",			float2(0.0),	float2(1.0),	float2(0.0) );

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
			pass.Slider(	"iMaxMips",			1,			2,			2 );
			pass.Constant(	"iDUVScale",		duv_scalef );
			pass.Constant(	"iBorder",			border_sum );
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
					color = float4(iBrightValue);

				gl.image.Store( un_OutImages[level], coord, color );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "TexSampling.glsl"
	#include "InvocationID.glsl"

	bool  OutsideOfBorder (float2 uv, float2 dim)
	{
		int2	coord = int2(uv * dim);

		if ( (coord.x < iBorder or coord.x > int(dim.x) - iBorder - 1) or
			 (coord.y < iBorder or coord.y > int(dim.y) - iBorder - 1))
			return false;  // inside

		return true;  // outside
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

		int		tex_id	= iMaxMips - 1;
		float2	dim		= float2(gl.texture.GetSize( un_Bilinear[0], 0 ));
		int2	coord	= int2(uv * dim);
		float	grid	= AnyNotEqual( coord, int2(uv2 * dim) ) ? 0.0 : 1.0;

		float	v = 0.0;
		switch ( iFilter )
		{
			case 0 :	v = gl.texture.SampleGrad( un_Bilinear[tex_id], uv, dx, dy ).r; break;
			case 1 :	v = gl.texture.SampleGrad( un_Trilinear[tex_id], uv, dx, dy ).r; break;
			case 2 :	v = gl.texture.SampleGrad( un_Anisotropy2[tex_id], uv, dx, dy ).r; break;
			case 3 :	v = gl.texture.SampleGrad( un_Anisotropy4[tex_id], uv, dx, dy ).r; break;
			case 4 :	v = gl.texture.SampleGrad( un_Anisotropy8[tex_id], uv, dx, dy ).r; break;
			case 5 :	v = gl.texture.SampleGrad( un_Anisotropy16[tex_id], uv, dx, dy ).r; break;
		}
		out_Color.rgb = float3(v);

		if ( OutsideOfBorder( uv,  dim ))
		{
			out_Color.rgb = float3(0.0, v, 0.0);

			if ( v > 0.2 )
				out_Color.rgb = float3(1.0, 0.0, 0.0);
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
