// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	How textureGather works.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_TEX
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		RC<Image>	lowres	= Image( EPixelFormat::RG16U, uint2(128) );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "INIT_TEX" );
			pass.Output( "out_Color",	lowres );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Texture",	lowres,		Sampler_NearestClamp );
			pass.Slider( "iCenter",		float2(0.0),	float2(1.0),	float2(0.5) );
			pass.Slider( "iSize",		0,				1 );							// 2x2, 4x4
			pass.Slider( "iMode",		0,				2,				2 );			// sample, fetch, gather
			pass.Slider( "iDUVScale",	1.0,			16.0,			9.25 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT_TEX

	void  Main ()
	{
		out_Color = uint4( gl.FragCoord.xy, 0, 0 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  CoordToColor (uint4 r, uint4 g, uint2 coord, float2 scaleBias, inout float4 color)
	{
		uint	i = 0;
		for (; i < 4; ++i)
		{
			if ( r[i] == coord.x and g[i] == coord.y )
				break;
		}

		if ( i < 4 )
			color = Rainbow2( (float(i) + scaleBias.y) / scaleBias.x );
	}


	void  TexSample (const uint2 coord, const float2 mpos_uv, float2 duv)
	{
		duv *= iDUVScale;

		int2	min, max;
		switch ( iSize )
		{
			case 0 :	min = int2(-1);		max = int2(1);	break;
			case 1 :	min = int2(-2);		max = int2(1);	break;
		}

		int		count	= (max.x - min.x);
		float	scale	= float(count * count - 1);

		for (int y = min.y; y < max.y; y += 2)
		for (int x = min.x; x < max.x; x += 2)
		{
			uint2	rg0	= gl.texture.Sample( un_Texture, mpos_uv + float2(x,  y+1) * duv ).rg;
			uint2	rg1	= gl.texture.Sample( un_Texture, mpos_uv + float2(x+1,y+1) * duv ).rg;
			uint2	rg2	= gl.texture.Sample( un_Texture, mpos_uv + float2(x+1,y)   * duv ).rg;
			uint2	rg3	= gl.texture.Sample( un_Texture, mpos_uv + float2(x,  y)   * duv ).rg;

			uint4	r	= uint4( rg0.r, rg1.r, rg2.r, rg3.r );
			uint4	g	= uint4( rg0.g, rg1.g, rg2.g, rg3.g );
			int		x2	= (x - min.x) / 2;
			int		y2	= (y - min.y) / 2;
			float	bias = float(x2 + y2 * (count-1)) * 2.0;

			CoordToColor( r, g, coord, float2(scale, bias), INOUT out_Color );
		}
	}


	void  TexFetch (const uint2 coord, const int2 mpos_coord)
	{
		int2	min, max;
		switch ( iSize )
		{
			case 0 :	min = int2(-1);		max = int2(1);	break;
			case 1 :	min = int2(-2);		max = int2(1);	break;
		}

		int		count	= (max.x - min.x);
		float	scale	= float(count * count - 1);

		for (int y = min.y; y < max.y; y += 2)
		for (int x = min.x; x < max.x; x += 2)
		{
			uint2	rg0	= gl.texture.Fetch( un_Texture, mpos_coord + int2(x,  y+1), 0 ).rg;
			uint2	rg1	= gl.texture.Fetch( un_Texture, mpos_coord + int2(x+1,y+1), 0 ).rg;
			uint2	rg2	= gl.texture.Fetch( un_Texture, mpos_coord + int2(x+1,y),   0 ).rg;
			uint2	rg3	= gl.texture.Fetch( un_Texture, mpos_coord + int2(x,  y),   0 ).rg;

			uint4	r	= uint4( rg0.r, rg1.r, rg2.r, rg3.r );
			uint4	g	= uint4( rg0.g, rg1.g, rg2.g, rg3.g );
			int		x2	= (x - min.x) / 2;
			int		y2	= (y - min.y) / 2;
			float	bias = float(x2 + y2 * (count-1)) * 2.0;

			CoordToColor( r, g, coord, float2(scale, bias), INOUT out_Color );
		}
	}


	void  TexGather (const uint2 coord, const float2 mpos_uv)
	{
		switch ( iSize )
		{
			case 0 :
			{
				uint4	r = gl.texture.Gather( un_Texture, mpos_uv, 0 );
				uint4	g = gl.texture.Gather( un_Texture, mpos_uv, 1 );
				CoordToColor( r, g, coord, float2(3.0, 0.0), INOUT out_Color );
				break;
			}

			case 1 :
			{
				float	scale = float(2*2*2);
				for (int y = 0; y < 2; ++y)
				for (int x = 0; x < 2; ++x)
				{
					uint4	r = gl.texture.GatherOffset( un_Texture, mpos_uv, int2(x,y)*2-1, 0 );
					uint4	g = gl.texture.GatherOffset( un_Texture, mpos_uv, int2(x,y)*2-1, 1 );
					float	bias = float((x + y*2) * 2);
					CoordToColor( r, g, coord, float2(scale, bias), INOUT out_Color );
				}
				break;
			}
		}
	}


	void  Main ()
	{
		float2	uv	= ToUNorm( GetGlobalCoordSNormCorrected2() );
		float2	duv	= gl.dFdx( uv ) + gl.dFdy( uv );

		if ( IsNotUNorm( uv ))
		{
			out_Color = float4(0.0);
			return;
		}

		float2	tex_dim		= float2(gl.texture.GetSize( un_Texture, 0 ));
		uint2	coord		= uint2(uv * tex_dim + 0.5);
		float2	mpos_uv		= iCenter;

		if ( un_PerPass.mouse.z > 0.0 )
			mpos_uv = ToUNorm( MapPixCoordToSNormCorrected2( un_PerPass.mouse.xy * un_PerPass.resolution.xy, un_PerPass.resolution.xy ));

		int2	mpos_coord	= int2(mpos_uv * tex_dim + 0.5);

		// background
		{
			uint	mask	= 0x7;
			uint2	texel	= gl.texture.Fetch( un_Texture, int2(coord), 0 ).rg & mask;
			uint	idx		= texel.x + texel.y * (mask+1);
			float	grey	= float(idx) / float(Square( mask+1 ) - 1);

			out_Color = float4(Remap( float2(0.0, 1.0), float2(0.1, 0.7), grey ));
		}

		// highlight
		switch ( iMode )
		{
			case 0 :	TexSample( coord, mpos_uv, duv );	break;
			case 1 :	TexFetch( coord, mpos_coord );		break;
			case 2 :	TexGather( coord, mpos_uv );		break;
		}
	}

#endif
//-----------------------------------------------------------------------------
