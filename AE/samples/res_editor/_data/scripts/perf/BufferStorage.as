// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Same as [Storage](Storage.as) but with tunable data size per thread
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define MODE		0
#	define GEN_NOISE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		tex_dim		= DynamicUInt();
		RC<DynamicDim>		dim			= tex_dim.Mul( 1024 ).Dimension2();
		const uint2			local		= uint2(8);
		const uint2			local2		= uint2(16);

		RC<Buffer>			buf16		= Buffer();
		RC<Buffer>			buf32		= Buffer();
		RC<Buffer>			buf64		= Buffer();
		RC<Buffer>			buf128		= Buffer();

		RC<DynamicUInt>		buf16_size	= dim.Area();
		RC<DynamicUInt>		buf32_size	= buf16_size.Div( 2 );
		RC<DynamicUInt>		buf64_size	= buf16_size.Div( 4 );
		RC<DynamicUInt>		buf128_size	= buf16_size.Div( 8 );

		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		gen_tex		= DynamicUInt();

		buf16.ArrayLayout(
			"SBuf16",
			"float4		data;",
			buf16_size );

		buf32.ArrayLayout(
			"SBuf32",
			"float2x4	data;",
			buf32_size );
		
		buf64.ArrayLayout(
			"SBuf64",
			"float4x4	data;",
			buf64_size );
		
		buf128.ArrayLayout(
			"SBuf128",
			"float4		data[8];",
			buf128_size );
		
		Slider( tex_dim,	"TexDim",		1,	8,	2 );
		Slider( mode,		"Mode",			0,	11	);
		Slider( count,		"Repeat",		1,	32	);
		Slider( gen_tex,	"GenTex",		0,	1,  1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_NOISE" );
			pass.ArgOut( "un_Buffer16",		buf16 );
			pass.ArgOut( "un_Buffer32",		buf32 );
			pass.ArgOut( "un_Buffer64",		buf64 );
			pass.ArgOut( "un_Buffer128",	buf128 );
			pass.LocalSize( local );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( gen_tex, 1 );
		}
		//--------------------
		{
			RC<ComputePass>		pass = ComputePass( "", "MODE=0" );		// 16 byte
			pass.ArgInOut( "un_Buffer",	buf16 );
			pass.LocalSize( local.x*local.y );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( mode, 0 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=1" );		// 32 byte
			pass.ArgInOut( "un_Buffer",	buf32 );
			pass.LocalSize( local.x*local.y );
			pass.DispatchThreads( dim.Div(int2(1,2)) );
			pass.EnableIfEqual( mode, 1 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=2" );		// 64 byte
			pass.ArgInOut( "un_Buffer",	buf64 );
			pass.LocalSize( local.x*local.y );
			pass.DispatchThreads( dim.Div(int2(1,4)) );
			pass.EnableIfEqual( mode, 2 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=3" );		// 128 byte
			pass.ArgInOut( "un_Buffer",	buf128 );
			pass.LocalSize( local.x*local.y );
			pass.DispatchThreads( dim.Div(int2(1,8)) );
			pass.EnableIfEqual( mode, 3 );
			pass.Repeat( count );
		}
		//--------------------
		{
			RC<ComputePass>		pass = ComputePass( "", "MODE=0" );
			pass.ArgInOut( "un_Buffer",	buf16 );
			pass.LocalSize( local );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( mode, 4 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=1" );
			pass.ArgInOut( "un_Buffer",	buf32 );
			pass.LocalSize( local );
			pass.DispatchThreads( dim.Div(int2(1,2)) );
			pass.EnableIfEqual( mode, 5 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=2" );
			pass.ArgInOut( "un_Buffer",	buf64 );
			pass.LocalSize( local );
			pass.DispatchThreads( dim.Div(int2(1,4)) );
			pass.EnableIfEqual( mode, 6 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=3" );
			pass.ArgInOut( "un_Buffer",	buf128 );
			pass.LocalSize( local );
			pass.DispatchThreads( dim.Div(int2(1,8)) );
			pass.EnableIfEqual( mode, 7 );
			pass.Repeat( count );
		}
		//--------------------
		{
			RC<ComputePass>		pass = ComputePass( "", "MODE=0" );
			pass.ArgInOut( "un_Buffer",	buf16 );
			pass.LocalSize( local2 );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( mode, 8 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=1" );
			pass.ArgInOut( "un_Buffer",	buf32 );
			pass.LocalSize( local2 );
			pass.DispatchThreads( dim.Div(int2(1,2)) );
			pass.EnableIfEqual( mode, 9 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=2" );
			pass.ArgInOut( "un_Buffer",	buf64 );
			pass.LocalSize( local2 );
			pass.DispatchThreads( dim.Div(int2(1,4)) );
			pass.EnableIfEqual( mode, 10 );
			pass.Repeat( count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MODE=3" );
			pass.ArgInOut( "un_Buffer",	buf128 );
			pass.LocalSize( local2 );
			pass.DispatchThreads( dim.Div(int2(1,8)) );
			pass.EnableIfEqual( mode, 11 );
			pass.Repeat( count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MODE
	#include "GlobalIndex.glsl"
	#include "CodeTemplates.glsl"

	float4  Update (float4 val)
	{
		return val * val + 0.001;
	}

	void  Main ()
	{
		const uint	idx = GetGlobalIndex();

		#if MODE == 0
		{
			float4	data = un_Buffer.elements[ idx ].data;
			data = Update( data );
			un_Buffer.elements[ idx ].data = data;
		}
		#elif MODE == 1
		{
			float2x4	data = un_Buffer.elements[ idx ].data;
			data[0] = Update( data[0] );
			data[1] = Update( data[1] );
			un_Buffer.elements[ idx ].data = data;
		}
		#elif MODE == 2
		{
			float4x4	data = un_Buffer.elements[ idx ].data;
			data[0] = Update( data[0] );
			data[1] = Update( data[1] );
			data[2] = Update( data[2] );
			data[3] = Update( data[3] );
			un_Buffer.elements[ idx ].data = data;
		}
		#elif MODE == 3
		{
			float4	data[8] = un_Buffer.elements[ idx ].data;

			[[unroll]] for (uint i = 0; i < 8; ++i)
				data[i] = Update( data[i] );
			
			un_Buffer.elements[ idx ].data = data;
		}
		#else
		#	error unsupported MODE
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "GlobalIndex.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	uv		= float2(GetGlobalCoord().xy);
		float4	col0	= Rainbow( Hash_Uniform( uv, 0.11 ));
		float4	col1	= Rainbow( Hash_Uniform( uv, 0.22 ));
		float4	col2	= Rainbow( Hash_Uniform( uv, 0.33 ));
		float4	col3	= Rainbow( Hash_Uniform( uv, 0.44 ));
		uint	idx		= GetGlobalIndex();

		un_Buffer16.elements[ idx ].data = col0;
		un_Buffer32.elements[ idx ].data = float2x4( col0, col1 );
		un_Buffer64.elements[ idx ].data = float4x4( col0, col1, col2, col3 );
		
		un_Buffer128.elements[ idx ].data[0] = col0;
		un_Buffer128.elements[ idx ].data[1] = col1;
		un_Buffer128.elements[ idx ].data[2] = col2;
		un_Buffer128.elements[ idx ].data[3] = col3;
		un_Buffer128.elements[ idx ].data[4] = col0 + col1;
		un_Buffer128.elements[ idx ].data[5] = col1 + col2;
		un_Buffer128.elements[ idx ].data[6] = col2 + col3;
		un_Buffer128.elements[ idx ].data[7] = col0 + col2;
	}

#endif
//-----------------------------------------------------------------------------
