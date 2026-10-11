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
		const int		block_size	= 6;	// 4x4, 6x6, 8x8
		int				tex_scale	= 1;

		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		RC<Buffer>		buf0	= Buffer( "res/utils/astc_lut/LUTRemainingBitsToEndpointQuantizer.bin" );
		RC<Buffer>		buf1	= Buffer( "res/utils/astc_lut/LUTEndpointUnquantize.bin" );
		RC<Buffer>		buf2	= Buffer( "res/utils/astc_lut/LUTWeightQuantizer.bin" );
		RC<Buffer>		buf3	= Buffer( "res/utils/astc_lut/LUTWeightUnquantize.bin" );
		RC<Buffer>		buf4	= Buffer( "res/utils/astc_lut/LUTTritQuintDecode.bin" );

		RC<BufferView>	view0	= buf0.CreateView( EPixelFormat::RGBA16U );
		RC<BufferView>	view1	= buf1.CreateView( EPixelFormat::R8U );
		RC<BufferView>	view2	= buf2.CreateView( EPixelFormat::RGBA8U );
		RC<BufferView>	view3	= buf3.CreateView( EPixelFormat::R8U );
		RC<BufferView>	view4	= buf4.CreateView( EPixelFormat::R16U );
		RC<Image>		tex5;

		array<RC<Image>>	origin_images;

		array<RC<Image>>	comp_images;
		if ( block_size == 4 )
		{
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/bright_desert_sand/Bright_Desert_Sand_sjzkfega_2K_BaseColor.jpg" ));
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/dry_fallen_leaves/Dry_Fallen_Leaves_vesobghbw_2K_BaseColor.jpg" ));
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/littered_grassy_grround/Littered_Grassy_Ground_vlqvdja_2K_BaseColor.jpg" ));

			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc4x4/Bright_Desert_Sand_sjzkfega_2K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc4x4/Dry_Fallen_Leaves_vesobghbw_2K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc4x4/Littered_Grassy_Ground_vlqvdja_2K_BaseColor.aeimg",	ImageLoadOpFlags::BlockTexelView ));
			@tex5 = Image( EImageType::UInt_2D, "res/utils/astc_lut/LUTPartitionTable_128x128_4x4.aeimg" );
		}
		if ( block_size == 6 )
		{
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex4-6k/Bright_Desert_Sand_sjzkfega_6K_BaseColor.jpg" ));
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex4-6k/Dry_Fallen_Leaves_vesobghbw_6K_BaseColor.jpg" ));
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex4-6k/Littered_Grassy_Ground_vlqvdja_6K_BaseColor.jpg" ));

			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc6x6/Bright_Desert_Sand_sjzkfega_3K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc6x6/Dry_Fallen_Leaves_vesobghbw_3K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc6x6/Littered_Grassy_Ground_vlqvdja_3K_BaseColor.aeimg",	ImageLoadOpFlags::BlockTexelView ));
			@tex5 = Image( EImageType::UInt_2D, "res/utils/astc_lut/LUTPartitionTable_192x192_6x6.aeimg" );
			tex_scale = 2;	// 6K to 3K
		}
		if ( block_size == 8 )
		{
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/bright_desert_sand/Bright_Desert_Sand_sjzkfega_2K_BaseColor.jpg" ));
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/dry_fallen_leaves/Dry_Fallen_Leaves_vesobghbw_2K_BaseColor.jpg" ));
			origin_images.push_back( Image( EImageType::Float_2D, "res/terrain-tex/littered_grassy_grround/Littered_Grassy_Ground_vlqvdja_2K_BaseColor.jpg" ));

			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc8x8/Bright_Desert_Sand_sjzkfega_2K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc8x8/Dry_Fallen_Leaves_vesobghbw_2K_BaseColor.aeimg",		ImageLoadOpFlags::BlockTexelView ));
			comp_images.push_back( Image( EImageType::UInt_2D, "res/terrain-tex4-astc8x8/Littered_Grassy_Ground_vlqvdja_2K_BaseColor.aeimg",	ImageLoadOpFlags::BlockTexelView ));
			@tex5 = Image( EImageType::UInt_2D, "res/utils/astc_lut/LUTPartitionTable_256x192_8x6.aeimg" );
		}

		Assert( comp_images.size() == origin_images.size() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output(	"out_Color",		rt );
			pass.ArgIn(		"un_CompImages",	comp_images,	Sampler_NearestClamp );
			pass.ArgIn(		"un_OriginImages",	origin_images,	Sampler_NearestClamp );

			pass.ArgIn(		"LUTRemainingBitsToEndpointQuant",	view0 );
			pass.ArgIn(		"LUTEndpointUnquantize",			view1 );
			pass.ArgIn(		"LUTWeightQuantizer",				view2 );
			pass.ArgIn(		"LUTWeightUnquantize",				view3 );
			pass.ArgIn(		"LUTTritQuintDecode",				view4 );
			pass.ArgIn(		"LUTPartitionTable",				tex5,	Sampler_NearestClamp );

			pass.Slider(	"iCmp",				0,		2,		1 );
			pass.Slider(	"iCmpScale",		0,		14,		1 );
			pass.Slider(	"iImageIdx",		0,		comp_images.size()-1 );
			pass.Slider(	"iUVScale",			1.0,	4.0,	1.0 );
			pass.Constant(	"iBlockSize",		block_size );
			pass.Constant(	"iTexScale",		tex_scale );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "InvocationID.glsl"
	#include "Compression/DecodeASTC.glsl"

	void  Main ()
	{
		const int2	block_size = int2(iBlockSize, iBlockSize);	// ASTC supports custom block size

		float2	dim		= float2(gl.texture.GetSize( un_OriginImages[iImageIdx], 0 ));
		float2	uv		= MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy, dim, 1.0 / iUVScale );
		int2	coord	= int2(uv * dim);
		int2	coord2	= coord / iTexScale;

		float4	ref		= gl.texture.Fetch( un_OriginImages[iImageIdx], coord, 0 );
		uint4	block	= gl.texture.Fetch( un_CompImages[iImageIdx], coord2 / block_size, 0 );
		float4	col;

		if ( IsNotUNorm( uv ))
		{
			out_Color = float4(0.0);
			return;
		}

		if ( !DecodeASTC( block, uint2(block_size), coord2 & 3, OUT col ))
			col = float4(1.0, 0.0, 0.0, 1.0);

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
