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
			pass.Slider( "iMode",		0,			1 );
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


	void  Main ()
	{
		float2	uv	= ToUNorm( GetGlobalCoordSNormCorrected2() );

		if ( IsNotUNorm( uv ))
		{
			out_Color = float4(0.0);
			return;
		}

		float2	tex_dim		= float2(gl.texture.GetSize( un_Texture, 0 ));
		uint2	coord		= uint2(uv * tex_dim + 0.5);
		float2	mpos_uv		= float2(0.5);

		if ( un_PerPass.mouse.z > 0.0 )
			mpos_uv = ToUNorm( MapPixCoordToSNormCorrected2( un_PerPass.mouse.xy * un_PerPass.resolution.xy, un_PerPass.resolution.xy ));

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

#endif
//-----------------------------------------------------------------------------
