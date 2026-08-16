// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Generate random EAC image and decode it using software and hardware algorithms.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_COMPRESSED
#	define COMPARE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const uint2			dim			= uint2(32);
		const EPixelFormat	block_fmt	= EPixelFormat::RG32U;	// 64 bit
		const EPixelFormat	comp_fmt	= EPixelFormat::EAC_R11_UNorm;
		const EPixelFormat	comp2_fmt	= EPixelFormat::EAC_R11_SNorm;

		RC<Image>	comp_img	= Image( comp_fmt, dim );								comp_img.Name( "Compressed" );
		RC<Image>	comp2_img	= comp_img.CreateView( EImage_2D, comp2_fmt );			comp2_img.Name( "Compressed-2" );
		RC<Image>	block_img	= comp_img.CreateView( EImage_2D, block_fmt );			block_img.Name( "BlockView" );
		RC<Image>	rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );

		Assert( block_img.Dimension2() == dim/4 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_COMPRESSED" );
			pass.ArgOut( "un_BlockImage",	block_img );
			pass.Slider( "iSeed",			float2(0.0),	float2(1.0) );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( block_img.Dimension2() );
		}{
			RC<Postprocess>		pass = Postprocess( "", "COMPARE" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_CompImage",	comp_img,	Sampler_NearestClamp );
			pass.ArgIn(  "un_Comp2Image",	comp2_img,	Sampler_NearestClamp );
			pass.ArgIn(  "un_BlockImage",	block_img,	Sampler_NearestClamp );
			pass.Slider( "iCmp",			0,		5,		4 );
			pass.Slider( "iCmpScale",		0,		14,		10 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_COMPRESSED
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		int2	coord	= GetGlobalCoord().xy;
		uint2	block;

		block.x = MHash12( floatBitsToUint( uv.xy * (iSeed.x + 111.0) + iSeed.y + 11.11 ));
		block.y = MHash12( floatBitsToUint( uv.yx * (iSeed.x + 222.0) + iSeed.y + 22.22 ));

		gl.image.Store( un_BlockImage, coord, uint4(block, 0, 0) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef COMPARE
	#include "InvocationID.glsl"
	#include "Compression/DecodeETC.glsl"

	void  Main ()
	{
		float2	dim		= float2(gl.texture.GetSize( un_CompImage, 0 ));
		float2	uv		= MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy, dim );

		int2	coord	= int2(uv * dim);
		float4	ref1	= gl.texture.Fetch( un_CompImage,  coord, 0 );
		float4	ref2	= gl.texture.Fetch( un_Comp2Image, coord, 0 );	ref2.r = ToUNorm( ref2.r );
		uint2	block	= gl.texture.Fetch( un_BlockImage, coord/4, 0 ).rg;
		float4	col1	= DecodeEAC_Ru( block, coord & 3 );
		float4	col2	= DecodeEAC_Rs( block, coord & 3 );				col2.r = ToUNorm( col2.r );

		if ( IsNotUNorm( uv ))
		{
			out_Color = float4(0.0);
			return;
		}

		float	scale = Exp2( float(iCmpScale) );

		switch ( iCmp )
		{
			case 0 :	out_Color = ref1;	break;
			case 1 :	out_Color = col1;	break;

			case 2 :	out_Color = ref2;	break;
			case 3 :	out_Color = col2;	break;

			case 4 :
				out_Color = Abs( ref1 - col1 ) * scale;
				if ( AnyGreater( out_Color.rgb, 1.0 ))
					out_Color = float4(1.0);
				break;

			case 5 :
				out_Color = Abs( ref2 - col2 ) * scale;
				if ( AnyGreater( out_Color.rgb, 1.0 ))
					out_Color = float4(1.0);
				break;
		}
	}

#endif
//-----------------------------------------------------------------------------


