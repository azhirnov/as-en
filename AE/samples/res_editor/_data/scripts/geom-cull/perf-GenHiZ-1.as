// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	How to calculate mip levels for HiZ if source image is not power-of-2.

	1. Convert to lowest power-of-2 image.
	2. Calculate mipmaps.

	Performance:
	 * downsampling of power-of-2 image is faster, because GPU optimized to load 2x2 blocks of texels.
	 * can be used min/max reduction sampler which is a bit faster.

	NV RTX2080:	+40% compared to 'GenHiZ-2' on POT images, same performance on non-POT.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_DEPTH
#	define MIPMAP_0
#	define GEN_MIPMAP
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
	  #if 0
		// debugging
		RC<DynamicUInt2>	tex_dim		= DynamicUInt2();
		RC<DynamicDim>		dim			= tex_dim.Dimension();

		Slider( tex_dim,	"Dimension",	uint2(3),	uint2(64),	uint2(54, 57) );

	  #else
		// performance test
		RC<DynamicUInt>		dim_scale	= DynamicUInt();
		RC<DynamicUInt2>	tex_dim		= DynamicUInt2( uint2(1920, 1080)/2 ).Mul( dim_scale.Exp2().XX() );
		RC<DynamicDim>		dim			= tex_dim.Dimension();

		Slider( dim_scale,	"DimensionScale",	0, 3, 1 );	// 1K, 2K, 4K, 8K
		Label(  tex_dim,	"Dimension" );
	  #endif

		RC<DynamicUInt>		first_mip	= DynamicUInt();
		RC<DynamicUInt2>	tex_dim2	= tex_dim.Div(uint2(2)).NearPOT().DivCeil( first_mip.Exp2().XX() );
		RC<DynamicDim>		dim2		= tex_dim2.Dimension();

		EPixelFormat		fmt			= EPixelFormat::R32F;	// R16_UNorm or R32F
		RC<Image>			high_mip	= Image( fmt, dim );								high_mip.Name( "High mip" );
		RC<Image>			mipmaps		= Image( fmt, dim2, MipmapLevel(~0) );				mipmaps.Name( "Mipmapped" );
		RC<Image>			rt			= Image( EPixelFormat::RGBA16F, SurfaceSize() );	rt.Name( "RT" );
		RC<DynamicUInt>		repeat		= DynamicUInt();
		RC<DynamicUInt>		raster		= DynamicUInt( 1 );
		RC<DynamicUInt>		reduction	= DynamicUInt();
		RC<DynamicUInt>		mode		= raster.Add( reduction.Mul(2) );
		const bool			gfx_only	= false;	// some devices disable compression if have 'Storage' usage

		Label( tex_dim2,	"MipDim" );
		Slider( repeat,		"Repeat",		1,	30 );
		Slider( first_mip,	"FirstMip",		0,	10 );

		if ( not gfx_only )
			Slider( raster,		"Gfx",				0,	1 );

		if ( GetFeatureSet().hasSamplerFilterMinmax() )
			Slider( reduction,	"UseReduction",		0,	1 );

		// render loop //

		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_DEPTH" );
			pass.Output( "out_Color",	high_mip );
			pass.Slider( "iHash",		0.0,	3.0,	0.0 );
		}

		// non-POT to POT image
		if ( not gfx_only )
		{
			RC<ComputePass>		pass = ComputePass( "", "MIPMAP_0" );
			pass.ArgOut( "un_OutImage",		mipmaps );
			pass.ArgIn(  "un_HighMip",		high_mip,	Sampler_NearestClamp );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( dim2 );
			pass.Repeat( repeat );
			pass.EnableIfEqual( raster, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "MIPMAP_0" );
			pass.Output( "out_Color",		mipmaps );
			pass.ArgIn(  "un_HighMip",		high_mip,	Sampler_NearestClamp );
			pass.Repeat( repeat );
			pass.EnableIfEqual( raster, 1 );
		}

		// generate mipmap chain
		if ( not gfx_only )
		{
			RC<ComputeMip>		pass = ComputeMip( "", "GEN_MIPMAP" );
			pass.Variable( "un_InImage",	"un_OutImage",	mipmaps,	Sampler_NearestClamp );
			pass.Repeat( repeat );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<RasterMip>		pass = RasterMip( "", "GEN_MIPMAP" );
			pass.Variable( "un_InImage",	"out_Color",	mipmaps,	Sampler_NearestClamp );
			pass.Repeat( repeat );
			pass.EnableIfEqual( mode, 1 );
		}

		// mipmap chain
		if ( GetFeatureSet().hasSamplerFilterMinmax() )
		{
			if ( not gfx_only )
			{
				RC<ComputeMip>		pass = ComputeMip( "", "GEN_MIPMAP; USE_REDUCTION" );
				pass.Variable( "un_InImage",	"un_OutImage",	mipmaps,	Sampler_MinLinearClamp );
				pass.Repeat( repeat );
				pass.EnableIfEqual( mode, 2 );
			}{
				RC<RasterMip>		pass = RasterMip( "", "GEN_MIPMAP; USE_REDUCTION" );
				pass.Variable( "un_InImage",	"out_Color",	mipmaps,	Sampler_MinLinearClamp );
				pass.Repeat( repeat );
				pass.EnableIfEqual( mode, 3 );
			}
		}

		{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_HighMip",		high_mip,		Sampler_NearestClamp );
			pass.ArgIn(  "un_Mipmaps",		mipmaps,		Sampler_NearestClamp );
			pass.Slider( "iBeginEnd",		float2(0.0),	float2(1.0),	float2(0.0, 1.0) );
			pass.Slider( "iMip",			-1,				5,				0 );
			pass.Slider( "iGrid",			0,				1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_DEPTH
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv = GetGlobalCoordUNormCorrected().xy + iHash;
		out_Color.r = Saturate( -0.1 +
								DHash12( uv * 141.2 ) * 0.4 +
								DHash12( Floor(uv * 40.0) * 28.33 ) * 0.4 +
								DHash12( Floor(uv * 10.0) * 55.77 ) * 0.4 +
								DHash12( Floor(uv * 1.0) * 55.77 ) * 0.4 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MIPMAP_0
	#include "InvocationID.glsl"

	// downsample 5x5 to 4x4
	//
	// |0|1|2|3|4|
	//  |0|1|2|3|

	void  Main ()
	{
		int2	dim		= gl.texture.GetSize( un_HighMip, 0 );
		float2	fdim	= float2(dim);
		float2	inv_res	= GetGlobalSizeRcp().xy;

		float2	uv0		= (GetGlobalCoordUF().xy * inv_res) * fdim;
		float2	uv1		= ((GetGlobalCoordUF().xy + 1.0) * inv_res) * fdim;

		float	d		= float_max;

		int2	c0		= Max( int2(uv0), int2(0) );
		int2	c1		= Min( int2(uv1), dim-1 );

		for (int y = c0.y; y <= c1.y; ++y)
		for (int x = c0.x; x <= c1.x; ++x)
		{
			d = Min( d, gl.texture.Fetch( un_HighMip, int2(x,y), 0 ).r);
		}

		#ifdef SH_FRAG
			out_Color = float4(d);
		#else
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(d) );
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MIPMAP
	#include "InvocationID.glsl"

	void  Main ()
	{
		float4	c;

		#ifdef USE_REDUCTION
		{
			float2	uv = (float2(GetGlobalCoord().xy) + 0.5) * iInvResolution;
			c = gl.texture.SampleLod( un_InImage, uv, 0.0 );
		}
		#else
		{
			int2	p = GetGlobalCoord().xy * 2;
			c = gl.texture.Fetch( un_InImage, p, 0 );
			c = Min( c, gl.texture.Fetch( un_InImage, p + int2(1,0), 0 ));
			c = Min( c, gl.texture.Fetch( un_InImage, p + int2(0,1), 0 ));
			c = Min( c, gl.texture.Fetch( un_InImage, p + int2(1,1), 0 ));
		}
		#endif

		#ifdef SH_FRAG
			out_Color = c;
		#else
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, c );
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "InvocationID.glsl"

	void  Main ()
	{
		int		max_mip	= gl.texture.QueryLevels( un_Mipmaps );
		if ( iMip >= max_mip )
		{
			out_Color = float4(1.0, 0.0, 1.0, 0.0);
			return;
		}

		float2	uv		= GetGlobalCoordUNorm().xy;
		float2	uv2		= GetGlobalCoordUNorm( int3(1) ).xy;
		float	d1		= gl.texture.SampleLod( un_HighMip, uv, 0.0 ).r;
		float2	dim1	= gl.texture.GetSize( un_HighMip, 0 );

		out_Color.a = 1.0;

		if ( iMip < 0 )
		{
			out_Color.rgb = float3( LinearStep( d1, iBeginEnd.x, iBeginEnd.y ));
			return;
		}

		float	d0		= gl.texture.SampleLod( un_Mipmaps, uv, iMip ).r;
		float2	dim0	= gl.texture.GetSize( un_Mipmaps, iMip );

		out_Color.rgb = float3( LinearStep( d0, iBeginEnd.x, iBeginEnd.y ));

		// incorrect data in mip-level
		if ( d0 > d1 )
		{
			out_Color.r = 1.0;
			out_Color.g = -out_Color.g;
			out_Color.b = -LinearStep( d1, iBeginEnd.x, iBeginEnd.y );
		}

		if ( iGrid == 1 )
		{
			// high-level mip pixel border
			if ( NotAllEqual( int2(uv * dim1), int2(uv2 * dim1) ))
				out_Color = float4(0.0, 0.0, 1.0, 1.0);

			// mipmap pixel border
			if ( NotAllEqual( int2(uv * dim0), int2(uv2 * dim0) ))
				out_Color = float4(0.0, 1.0, 0.0, 1.0);
		}
	}

#endif
//-----------------------------------------------------------------------------
