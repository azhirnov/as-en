// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		array<RC<Image>>	comp_images;
		comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-etc2/Bright_Desert_Sand_sjzkfega_2K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
		comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-etc2/Dry_Fallen_Leaves_vesobghbw_2K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
		comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-etc2/Littered_Grassy_Ground_vlqvdja_2K_BaseColor.aeimg",	ImageLoadOpFlags::BlockTexelView ));

		array<RC<Image>>	origin_images;
		origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/bright_desert_sand/Bright_Desert_Sand_sjzkfega_2K_BaseColor.jpg" ));
		origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/dry_fallen_leaves/Dry_Fallen_Leaves_vesobghbw_2K_BaseColor.jpg" ));
		origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/littered_grassy_grround/Littered_Grassy_Ground_vlqvdja_2K_BaseColor.jpg" ));

		Assert( comp_images.size() == origin_images.size() );

		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		int			tex_scale	= 1;

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_CompImages",	comp_images,	Sampler_NearestClamp );
			pass.ArgIn(		"un_OriginImages",	origin_images,	Sampler_NearestClamp );
			pass.Slider(	"iCmp",				0,		2,		1 );
			pass.Slider(	"iCmpScale",		0,		14,		1 );
			pass.Slider(	"iImageIdx",		0,		comp_images.size()-1 );
			pass.Slider(	"iUVScale",			1.0,	4.0,	1.0 );
			pass.Constant(	"iTexScale",		tex_scale );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "InvocationID.glsl"
	#include "Compression/DecodeETC.glsl"

	void  Main ()
	{
		float2	dim		= float2(gl.texture.GetSize( un_OriginImages[iImageIdx], 0 ));
		float2	uv		= MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy, dim, 1.0 / iUVScale );
		int2	coord	= int2(uv * dim);
		int2	coord2	= coord / iTexScale;

		float4	ref		= gl.texture.Fetch( un_OriginImages[iImageIdx], coord, 0 );
		uint2	block	= gl.texture.Fetch( un_CompImages[iImageIdx], coord2 / 4, 0 ).rg;
		float4	col		= DecodeETC2( block, coord2 & 3 );

		if ( IsNotUNorm( uv ))
		{
			out_Color = float4(0.0);
			return;
		}

		float	scale = Exp2( float(iCmpScale) );

		switch ( iCmp )
		{
			case 0 :	out_Color = ref;	break;
			case 1 :	out_Color = col;	break;

			case 2 :
				out_Color = Abs( ref - col ) * scale;
				if ( AnyGreater( out_Color.rgb, 1.0 ))
					out_Color = float4(1.0);
				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
