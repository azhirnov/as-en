// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	https://developer.nvidia.com/blog/optimizing-compute-shaders-for-l2-locality-using-thread-group-id-swizzling/
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
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

		RC<Image>			image		= Image( EPixelFormat::RGBA32F, dim );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		gen_tex		= DynamicUInt();
		RC<DynamicUInt>		tile_size	= DynamicUInt();
		const uint			max_mode	= 4;

		Slider( tex_dim,	"TexDim",		1,	8,	2 );
		Slider( mode,		"Mode",			0,	max_mode );
		Slider( count,		"Repeat",		1,	32	);
		Slider( gen_tex,	"GenTex",		0,	1,  1 );
		Slider( tile_size,	"TileSize",		3,	6,  4 );

		Label( dim.XY(),	"Dimension" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "GEN_NOISE" );
			pass.ArgOut( "un_Image",	image );
			pass.LocalSize( local );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( gen_tex, 1 );
		}

		for (uint i = 0; i <= max_mode; ++i)
		{
			RC<ComputePass>		pass = ComputePass( "", "MODE="+i+";DIMX="+local.x+";DIMY="+local.y );
			pass.ArgInOut( "un_Image",		image );
			pass.Constant( "iTileSizePOT",	tile_size );
			pass.LocalSize( local );
			pass.DispatchThreads( dim );
			pass.EnableIfEqual( mode, i );
			pass.Repeat( count );
		}

		//Present( image );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MODE
	#include "InvocationID.glsl"
	#include "CodeTemplates.glsl"

	float4  Update (float4 val)
	{
		return val * val + 0.001;
	}


	void  Main ()
	{
		#if MODE == 0
			// implementation defined order
			const int2	pos  = GetGlobalCoord().xy;

		#elif MODE == 1
			// row major
			uint2	upos = gl.WorkGroupID.xy * uint2(DIMX, DIMY);
			upos.x += gl.LocalInvocationID.x;
			upos.y += gl.LocalInvocationID.y;
			const int2	pos = int2(upos);

		#elif MODE == 2
			// column major
			uint2	upos = gl.WorkGroupID.xy * uint2(DIMX, DIMY);
			upos.y += gl.LocalInvocationID.x;
			upos.x += gl.LocalInvocationID.y;
			const int2	pos = int2(upos);

		#elif MODE == 3 or MODE == 4
			const uint	tile_pot	= iTileSizePOT;
			const uint	tile_mask	= (1u << tile_pot) - 1;
			const uint	gi			= gl.WorkGroupID.x + gl.WorkGroupID.y * gl.NumWorkGroups.x;
			uint2	up;
			up.x = (gi & tile_mask) + (((gi >> tile_pot) / gl.NumWorkGroups.y) << tile_pot);
			up.y = (gi >> tile_pot) % gl.NumWorkGroups.y;

			#if MODE == 3
				// row major
				up = up * uint2(DIMX, DIMY) + gl.LocalInvocationID.xy;
			#else
				// column major
				up = up * uint2(DIMX, DIMY) + gl.LocalInvocationID.yx;
			#endif
			const int2	pos = int2(up);

		#else
		#	error unsupported MODE
		#endif

		float4	data = gl.image.Load( un_Image, pos );
		data = Update( data );
		gl.image.Store( un_Image, pos, data );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "InvocationID.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	uv  = float2(GetGlobalCoord().xy);
		float4	col = Rainbow( Hash_Uniform( uv, 0.111 ));

		gl.image.Store( un_Image, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
