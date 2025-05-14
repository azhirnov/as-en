// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Check render target lossless compression.
	Possible names:
		* Delta Color Compression (DCC)
		* ARM FrameBuffer Compression (AFBC)

	RT compression will decrease memory traffic which minimize memory bandwidth and memory access stalls.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_SOLID
#	define GEN_GRADIENT
#	define GEN_NOISE
#	define READ
#	define READ_CS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	fmt			= EPixelFormat::RGBA8_UNorm;
		RC<DynamicUInt>		tex_dim		= DynamicUInt();
		RC<DynamicDim>		dim			= tex_dim.Mul( 1024 ).Dimension2();
		const uint2			local_size	= uint2(8,16);
		RC<DynamicDim>		dim2		= dim.Mul( 2 );
		RC<Image>			rt2			= Image( fmt, dim2 );
		RC<Image>			rt3;//		= Image( fmt, dim2 );
		RC<Image>			rt4;//		= Image( fmt, dim2 );
		RC<Image>			rt			= Image( fmt, dim );
		RC<DynamicUInt>		gen_color	= DynamicUInt();
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		count		= DynamicUInt();
		uint				max_mode	= 0;

		// with DCC
		if ( @rt2 != null )
		{
			//rt2.AddOption( EImageOpt::LossyRTCompression );

			{
				RC<Postprocess>		pass = Postprocess( "", "GEN_SOLID" );
				pass.Output( "out_Color",	rt2 );
				pass.EnableIfEqual( gen_color, 1 );
			}{
				RC<Postprocess>		pass = Postprocess( "", "GEN_GRADIENT" );
				pass.Output( "out_Color",	rt2 );
				pass.EnableIfEqual( gen_color, 2 );
			}
			for (uint i = 0; i < 5; ++i)
			{
				RC<Postprocess>		pass = Postprocess( "", "SIZE=" + (4-i) + ";GEN_NOISE" );
				pass.Output( "out_Color",	rt2 );
				pass.EnableIfEqual( gen_color, 3+i );
			}
			for (uint i = 0; i < 3; ++i)
			{
				RC<ComputePass>		pass = ComputePass( "", "READ;MODE=" + i );
				pass.ArgIn(  "un_RT",		rt2,	(i == 2 ? Sampler_LinearClamp : Sampler_NearestClamp) );
				pass.ArgOut( "un_OutImage",	rt	);
				pass.LocalSize( local_size );
				pass.DispatchThreads( rt.Dimension() );
				pass.EnableIfEqual( mode, ++max_mode );
				pass.Repeat( count );
			}
		}

		// without DCC
		if ( @rt3 != null )
		{
			{
				RC<ComputePass>		pass = ComputePass( "", "GEN_SOLID" );
				pass.ArgOut( "un_OutImage",	rt3 );
				pass.LocalSize( local_size );
				pass.DispatchThreads( rt3.Dimension() );
				pass.EnableIfEqual( gen_color, 1 );
			}{
				RC<ComputePass>		pass = ComputePass( "", "GEN_GRADIENT" );
				pass.ArgOut( "un_OutImage",	rt3 );
				pass.LocalSize( local_size );
				pass.DispatchThreads( rt3.Dimension() );
				pass.EnableIfEqual( gen_color, 2 );
			}
			for (uint i = 0; i < 5; ++i)
			{
				RC<ComputePass>		pass = ComputePass( "", "SIZE=" + (4-i) + ";GEN_NOISE" );
				pass.ArgOut( "un_OutImage",	rt3 );
				pass.LocalSize( local_size );
				pass.DispatchThreads( rt3.Dimension() );
				pass.EnableIfEqual( gen_color, 3+i );
			}
			for (uint i = 0; i < 3; ++i)
			{
				RC<ComputePass>		pass = ComputePass( "", "READ_CS;MODE="+i );
				if ( i == 0 )	pass.ArgIn(  "un_RT",	rt3,	Sampler_LinearClamp );
				if ( i == 1 )	pass.ArgIn(  "un_RT",	rt3,	Sampler_NearestClamp );
				if ( i == 2 )	pass.ArgIn(  "un_RT",	rt3 );
				pass.ArgOut( "un_OutImage",	rt );
				pass.LocalSize( local_size );
				pass.DispatchThreads( rt.Dimension() );
				pass.EnableIfEqual( mode, ++max_mode );
				pass.Repeat( count );
			}
		}

		// with DCC
		if ( @rt4 != null )
		{
			{
				RC<Postprocess>		pass = Postprocess( "", "GEN_SOLID" );
				pass.Output( "out_Color",	rt4 );
				pass.EnableIfEqual( gen_color, 1 );
			}{
				RC<Postprocess>		pass = Postprocess( "", "GEN_GRADIENT" );
				pass.Output( "out_Color",	rt4 );
				pass.EnableIfEqual( gen_color, 2 );
			}
			for (uint i = 0; i < 5; ++i)
			{
				RC<Postprocess>		pass = Postprocess( "", "SIZE=" + (4-i) + ";GEN_NOISE" );
				pass.Output( "out_Color",	rt4 );
				pass.EnableIfEqual( gen_color, 3+i );
			}

			for (uint i = 0; i < 3; ++i)
			{
				RC<ComputePass>		pass = ComputePass( "", "READ_CS;MODE="+i );
				if ( i == 0 )	pass.ArgIn(  "un_RT",	rt4,	Sampler_LinearClamp );
				if ( i == 1 )	pass.ArgIn(  "un_RT",	rt4,	Sampler_NearestClamp );
				if ( i == 2 )	pass.ArgIn(  "un_RT",	rt4 );
				pass.ArgOut( "un_OutImage",	rt );
				pass.LocalSize( local_size );
				pass.DispatchThreads( rt.Dimension() );
				pass.EnableIfEqual( mode, ++max_mode );
				pass.Repeat( count );
			}
		}

		Slider( tex_dim,	"TexDim",	1,	8,	2 );	// NV: max 3 for RGBA32, max 5 for RGBA16, max 7 for RGBA8
		Slider( gen_color,	"Pattern",	0,	7,	1 );
		Slider( mode,		"Mode",		0,	3,	max_mode );
		Slider( count,		"Repeat",	1,	16 );

		Label( dim.XY(),	"Dimension" );

		//Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(SH_FRAG) or defined(SH_COMPUTE)
	#include "InvocationID.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"
#endif
//-----------------------------------------------------------------------------
#ifdef GEN_SOLID

	void Main ()
	{
		float4	col = float4(0.0);

		#ifdef SH_COMPUTE
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
		#else
			out_Color = col;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_GRADIENT

	void Main ()
	{
		float2	uv = GetGlobalCoordUNorm().xy;

		float3	col = float3(uv, 1.0 - uv.x * uv.y);

		#ifdef SH_COMPUTE
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(col, 1.0) );
		#else
			out_Color = float4(col, 1.0);
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE

	void Main ()
	{
		float2	uv = float2(GetGlobalCoord().xy >> SIZE);

		float4	col = DHash42( uv );

		#ifdef SH_COMPUTE
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
		#else
			out_Color = col;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef READ

	void Main ()
	{
		const int	dim		= 2;
		float4		col		= float4(0.0);

	#if MODE == 0
		const int2	coord	= GetGlobalCoord().xy * dim;

		[[unroll]]  for (int y = 0; y < dim; ++y)
		[[unroll]]  for (int x = 0; x < dim; ++x)
			col += gl.texture.Fetch( un_RT, coord + int2(x,y), 0 );

		col /= float(dim * dim);

	#elif MODE == 1

		const float2	step	= 1.0 / float2(GetGlobalSize().xy * dim);
		const float2	coord	= GetGlobalCoord().xy * dim;

		[[unroll]]  for (int y = 0; y < dim; ++y)
		[[unroll]]  for (int x = 0; x < dim; ++x)
			col += gl.texture.Sample( un_RT, (coord + float2(x,y)) * step );

		col /= float(dim * dim);
	#else

		col = gl.texture.Sample( un_RT, GetGlobalCoordUNorm().xy );
	#endif

		if ( AllLess( col, float4(-1.e+20) ))
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef READ_CS

	void Main ()
	{
		float4		col		= float4(0.0);
		const int	dim		= 2;
		const int2	coord	= GetGlobalCoord().xy * dim;

	#if MODE == 0

		col = gl.texture.Sample( un_RT, GetGlobalCoordUNorm().xy );

	#elif MODE == 1

		[[unroll]]  for (int y = 0; y < dim; ++y)
		[[unroll]]  for (int x = 0; x < dim; ++x)
			col += gl.texture.Fetch( un_RT, coord + int2(x,y), 0 );

		col /= float(dim * dim);
	#else

		[[unroll]] for (int y = 0; y < dim; ++y)
		[[unroll]] for (int x = 0; x < dim; ++x)
			col += gl.image.Load( un_RT, coord + int2(x,y) );

		col /= float(dim * dim);
	#endif

		if ( AllLess( col, float4(-1.e+20) ))
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
