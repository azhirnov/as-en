// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Generate random BC1 image and decode it using software and hardware algorithms.

	https://fgiesen.wordpress.com/2021/10/04/gpu-bcn-decoding/
	https://github.com/castano/icbc
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
		const EPixelFormat	comp_fmt	= EPixelFormat::BC1_RGB8_UNorm;

		RC<Image>	comp_img	= Image( comp_fmt, dim );								comp_img.Name( "Compressed" );
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
			pass.ArgIn(  "un_BlockImage",	block_img,	Sampler_NearestClamp );
			pass.Slider( "iCmp",			0,		2,		2 );
			pass.Slider( "iCmpScale",		0,		10,		5 );
			pass.Slider( "iChannel",		-1,		3,		-1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_COMPRESSED
	#include "Hash.glsl"
	#include "InvocationID.glsl"
	#include "Compression/DecodeBC.glsl"


	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		int2	coord	= GetGlobalCoord().xy;
		uint2	block;

		float3	color0	= DHash32( uv.xy * (iSeed.x + 111.0) + iSeed.y + 11.11 );
		float3	color1	= DHash32( uv.yx * (iSeed.x + 222.0) + iSeed.y + 22.22 );

		block.x = EncodeRGB565( color0 ) | (EncodeRGB565( color1 ) << 16);
		block.y = MHash12( floatBitsToUint( uv * (iSeed.x + 33.33) + iSeed.y ));

		gl.image.Store( un_BlockImage, coord, uint4(block, 0, 0) );
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
			case 3 :	return float4(c.a);
		}
	}

	float4  Decode (int2 coord)
	{
		uint2	block	= gl.texture.Fetch( un_BlockImage, coord/4, 0 ).rg;
		float4	col		= float4(DecodeBC1( block, coord & 3 ));
		return	col;
	}

	void  Main ()
	{
		float2	dim		= float2(gl.texture.GetSize( un_CompImage, 0 ));
		float2	uv		= MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy, dim );

		int2	coord	= int2(uv * dim);
		float4	ref		= Swizzle( gl.texture.Fetch( un_CompImage, coord, 0 ));
		float4	col		= Swizzle( Decode( coord ));

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


