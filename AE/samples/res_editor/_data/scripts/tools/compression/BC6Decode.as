// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Generate random BC6 image and decode it using software and hardware algorithms.
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
		const EPixelFormat	block_fmt	= EPixelFormat::RGBA32U;		// 128 bit
		const EPixelFormat	comp_fmt	= EPixelFormat::BC6H_RGB16F;	// signed
		const EPixelFormat	comp2_fmt	= EPixelFormat::BC6H_RGB16UF;	// unsigned

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
			pass.Slider( "iCmpScale",		0,		15,		10 );
			pass.Slider( "iChannel",		-1,		2,		-1 );
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
		uint4	block;

		block.x = MHash12( floatBitsToUint( uv.xy * (iSeed.x + 111.1) + iSeed.y + 11.11 ));
		block.y = MHash12( floatBitsToUint( uv.yx * (iSeed.x + 222.2) + iSeed.y + 22.22 ));
		block.z = MHash12( floatBitsToUint( uv.xy * (iSeed.x + 333.3) + iSeed.y + 33.33 ));
		block.w = MHash12( floatBitsToUint( uv.yx * (iSeed.x + 444.4) + iSeed.y + 44.44 ));

		gl.image.Store( un_BlockImage, coord, block );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef COMPARE
	#include "InvocationID.glsl"
	#include "Compression/DecodeBC.glsl"

	float4  Swizzle (float4 c)
	{
		switch ( iChannel )
		{
			case -1 :	return c;
			case 0 :	return float4(c.r);
			case 1 :	return float4(c.g);
			case 2 :	return float4(c.b);
		}
	}

	void  Main ()
	{
		float2	dim		= float2(gl.texture.GetSize( un_CompImage, 0 ));
		float2	uv		= MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy, dim );

		int2	coord	= int2(uv * dim);
		float4	ref1	= Swizzle( gl.texture.Fetch( un_CompImage, coord, 0 ));		// signed
		float4	ref2	= Swizzle( gl.texture.Fetch( un_Comp2Image, coord, 0 ));	// unsigned
		uint4	block	= gl.texture.Fetch( un_BlockImage, coord/4, 0 );
		float4	col1	= Swizzle( DecodeBC6_SF16( block, coord & 3 ));				// signed
		float4	col2	= Swizzle( DecodeBC6_UF16( block, coord & 3 ));				// unsigned

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


