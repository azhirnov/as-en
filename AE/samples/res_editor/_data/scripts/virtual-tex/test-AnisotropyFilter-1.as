// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
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
		const uint			dim		= 32;
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>			tex		= Image( EPixelFormat::R16F, uint2(dim), MipmapLevel(~0) );
		array<RC<Image>>	mips;	tex.GetMipmapsView( OUT mips );

		// render loop
		{
			// select texel
			RC<ComputePass>		pass = ComputePass( "", "GEN_IMAGE" );
			pass.ArgOut( "un_OutImages",	mips );
			pass.Slider( "iXY",				int2(0),	int2(dim),	int2(dim/2) );
			pass.Slider( "iMip",			-1,			tex.MipmapCount()-1 );
			pass.Slider( "iDiameter",		0,			dim );
			pass.Slider( "iBorder",			0,			dim );
			pass.Slider( "iBrightValue",	0.5,		100.0,		1.5 );
			pass.LocalSize( dim, dim );
			pass.DispatchGroups( 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( "out_Color",		rt,		RGBA32f(0.0) );
			pass.ArgIn(  "un_Bilinear",		tex,	Sampler_NearestMirrorRepeat );
			pass.ArgIn(  "un_Trilinear",	tex,	Sampler_LinearMipmapMirrorRepeat );
			pass.ArgIn(  "un_Anisotropy2",	tex,	Sampler_Anisotropy2MirrorRepeat );
			pass.ArgIn(  "un_Anisotropy4",	tex,	Sampler_Anisotropy4MirrorRepeat );
			pass.ArgIn(  "un_Anisotropy8",	tex,	Sampler_Anisotropy8MirrorRepeat );
			pass.ArgIn(  "un_Anisotropy16",	tex,	Sampler_Anisotropy16MirrorRepeat );
			pass.Slider( "iFilter",			0,			5 );
			pass.Slider( "iDUVScale",		int2(0),	int2(12),	int2(0) );	// LOD depends on screen resolution
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_IMAGE
	#include "InvocationID.glsl"

	void  Main ()
	{
		const int2	coord		= GetGlobalCoord().xy;
		const int3	selected	= int3( iXY >> iMip, iMip );

		for (int level = 0; level < un_OutImages.length(); ++level)
		{
			int2	dim	= GetGlobalSize().xy >> level;

			if ( AllLess( coord, dim ))
			{
				float4	color = float4(0.2);

				// dot
				if ( iDiameter == 0 and iBorder == 0 and AllEqual( selected, int3(coord, level) ))
					color = float4(iBrightValue);

				// circle in all mips
				if ( iDiameter > 0 )
				{
					float2	c = Abs( float2(coord - dim/2) + 0.55 );
					float	r = float(iDiameter >> level) * 0.5;

					if ( c.x > r or c.y > r )
						color = float4(iBrightValue);
				}

				//
				if ( iBorder > 0 )
				{
					if ( (coord.x < iBorder or coord.x > dim.x - iBorder - 1) or
						 (coord.y < iBorder or coord.y > dim.y - iBorder - 1))
						color = float4(iBrightValue);
				}

				gl.image.Store( un_OutImages[level], coord, color );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Color.glsl"
	#include "TexSampling.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv		= ToUNorm( GetGlobalCoordSNormCorrected2() );
		float2	uv2		= ToUNorm( GetGlobalCoordSNormCorrected2( int2(1) ));

		float2	scale	= Exp2( float2(iDUVScale) );
		float2	dx		= gl.dFdx( uv ) * scale.x;
		float2	dy		= gl.dFdy( uv ) * scale.y;

		float2	dim		= float2(gl.texture.GetSize( un_Bilinear, 0 ));
		float	mips	= float(gl.texture.QueryLevels( un_Bilinear )) - 1.0;

		float	lod		= Max( 0.0, SwQueryLod( dim, float4(dx, dy) ).x );
		float2	mip_dim	= dim * Exp2( -lod );

		if ( IsNotUNorm( uv ))
		{
			out_Color = Rainbow2( lod / mips );
			return;
		}

		float	grid	= AnyNotEqual( int2(uv * mip_dim), int2(uv2 * mip_dim) ) ? 0.0 : 1.0;

		float	v = 0.0;
		switch ( iFilter )
		{
			case 0 :	v = gl.texture.SampleGrad( un_Bilinear, uv, dx, dy ).r; break;
			case 1 :	v = gl.texture.SampleGrad( un_Trilinear, uv, dx, dy ).r; break;
			case 2 :	v = gl.texture.SampleGrad( un_Anisotropy2, uv, dx, dy ).r; break;
			case 3 :	v = gl.texture.SampleGrad( un_Anisotropy4, uv, dx, dy ).r; break;
			case 4 :	v = gl.texture.SampleGrad( un_Anisotropy8, uv, dx, dy ).r; break;
			case 5 :	v = gl.texture.SampleGrad( un_Anisotropy16, uv, dx, dy ).r; break;
		}
		out_Color = float2(v * grid, 1.0).rrrg;
	}

#endif
//-----------------------------------------------------------------------------
