// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Use quads to fill area.
	Use cached SDF inside quad to build exact shape.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define RAYMARCH
#	define SDF_TO_QUADS
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt1			= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			rt2			= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>			sdf_data	= Image( EPixelFormat::R16F,		uint2(128) );		// brick map
		RC<Buffer>			quads		= Buffer();
		RC<Scene>			scene		= Scene();
		const uint2			local_size	= uint2( 16 );
		RC<DynamicUInt>		grid_pot	= DynamicUInt();
		RC<DynamicUInt2>	grid_size	= grid_pot.Exp2().XX();
		RC<DynamicUInt>		quad_count	= grid_size.Area();
		RC<DynamicUInt>		sdf_qd_pot	= DynamicUInt();	// 1: 4 SDF texels per voxel
		RC<DynamicUInt>		sdf_per_qd	= sdf_qd_pot.Exp2();
		RC<DynamicUInt>		sdf_dim		= DynamicUInt( sdf_data.Dimension2().x );

		quads.ArrayLayout(
			"SdfQuad",
			"	uint	packedUV;",		// ~0u - empty, ~0u-1 - opaque
			"uint	count;",			// atomic
			quad_count
		);

		// create geometry
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount		= 4;
			cmd.InstanceCount( quad_count );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( grid_pot,	"GridSizePOT",		3,		7,		5 );
		Slider( sdf_qd_pot,	"TexelsPerQuadPOT",	1,		4,		1 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "RAYMARCH" );
			pass.Output(	"out_Color",	rt1 );
			pass.Constant(	"iScreenDim",	rt1.Dimension() );
		}{
			ClearBuffer( quads, 0 );

			RC<ComputePass>		pass = ComputePass( "", "SDF_TO_QUADS" );
			pass.ArgInOut(	"un_Quads",				quads );
			pass.ArgInOut(	"un_SDFCache",			sdf_data );
			pass.Constant(	"iScreenDim",			rt1.Dimension() );
			pass.Constant(	"iCacheDim",			sdf_data.Dimension2() );
			pass.Constant(	"iTexelsPerQuadPOT",	sdf_qd_pot );
			pass.Constant(	"iGridSize",			grid_size );
			pass.LocalSize( local_size );
			pass.DispatchThreads( grid_size );
			//pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw quads" );
			pass.AddPipeline( "*-draw.ppln" );
			pass.Output(	"out_Color",	rt2,		RGBA32f(0.f) );
			pass.ArgIn(		"un_Quads",		quads );
			pass.ArgIn(		"un_SDFCache",	sdf_data,	Sampler_LinearClamp );
			pass.Constant(	"iPixStep",		sdf_per_qd );
			pass.Constant(	"iInvDim",		float2(1.0) / float2(sdf_data.Dimension2()) );
			pass.Slider(	"iColorMode",	0,	4,	0 );
			pass.Constant(	"iGridSize",	grid_size );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",	rt );
			pass.ArgIn(		"un_RayMarch",	rt1,		Sampler_NearestClamp );
			pass.ArgIn(		"un_CachedSDF",	rt2,		Sampler_NearestClamp );
			pass.ArgIn(		"un_SDFCache",	sdf_data,	Sampler_LinearClamp );
			pass.ArgIn(		"un_Quads",		quads );
			pass.Slider(	"iBlend",		0.0,	1.0,	0.5 );
			pass.Slider(	"iDbgView",		0,		1,		0 );
			pass.Constant(	"iTexelsPerQuadPOT",	sdf_qd_pot );
			pass.Constant(	"iCacheDim",			sdf_data.Dimension2() );
		}

		DbgExport( quads, "quads-.h" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(RAYMARCH) or defined(SDF_TO_QUADS)
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	float  SDFScene (float2 pos)
	{
		pos.x *= float(iScreenDim.x) / float(iScreenDim.y);

		float	d = SDF2_Circle( pos, 0.5 );

		return d;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RAYMARCH

	void  Main ()
	{
		float2	pos = GetGlobalCoordSNorm().xy;
		float	d	= SDFScene( pos );

		out_Color	= d < 0.0 ? float4(0.6, 0.6, 0.0, 1.0) : float4(0.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SDF_TO_QUADS
	#include "Atomic.glsl"
	#include "Hash.glsl"

	void  StoreSDF (float2 pos, int2 posInCache)
	{
		float	d = SDFScene( ToSNorm( pos ));
		gl.image.Store( un_SDFCache, posInCache, float4(d) );
	}


	void  Main ()
	{
		const int2	coord		= GetGlobalCoord().xy;

		if ( AnyGreaterEqual( coord, iGridSize ))
			return;

		const float2	inv_size	= 1.0 / float2(iGridSize);
		const int		idx			= GetGlobalIndex();

		// fast path
		{
			float	d		= SDFScene( ToSNorm( (float2(coord) + 0.5) * inv_size )) * 0.4;
			float	max_d	= MaxOf( inv_size );

			if ( d > max_d )
			{
				un_Quads.elements[ idx ].packedUV = ~0u;		// empty
				return;
			}

			if ( d < -max_d )
			{
				un_Quads.elements[ idx ].packedUV = ~0u - 1;	// opque
				return;
			}
		}

		uint	pos;
		SubgroupAtomicInc( OUT pos, INOUT un_Quads.count );

		const uint2	cache_dim	= iCacheDim >> iTexelsPerQuadPOT;

		if ( pos >= cache_dim.x * cache_dim.y )
		{
			un_Quads.elements[ idx ].packedUV = ~0u - 1;		// opque	// TODO: special value for overflow?
			return;
		}

		const uint2	pos_in_cache = IndexToVec2( pos, cache_dim ) << iTexelsPerQuadPOT;

		un_Quads.elements[ idx ].packedUV = (pos_in_cache.x | (pos_in_cache.y << 16));

		const uint	texels_per_quad = 1u << iTexelsPerQuadPOT;
		const float	inv_tex			= 1.0 / float(texels_per_quad - 1);

		for (uint y = 0; y < texels_per_quad; ++y)
		for (uint x = 0; x < texels_per_quad; ++x)
		{
			float2	pos = (float2(coord) + float2(x,y) * inv_tex) * inv_size;
			StoreSDF( pos, int2(pos_in_cache) + int2(x,y) );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Math.glsl"
	#include "SDF.glsl"

	void  Main ()
	{
		float2	uv		= gl.FragCoord.xy * un_PerPass.invResolution;
		float4	col0	= gl.texture.Sample( un_RayMarch, uv );
		float4	col1	= gl.texture.Sample( un_CachedSDF, uv );

		out_Color = Lerp( col0, col1, iBlend );

		switch ( iDbgView )
		{
			case 1 :
			{
				float	sdf			= gl.texture.Sample( un_SDFCache, uv ).r;
				uint2	cache_dim	= iCacheDim >> iTexelsPerQuadPOT;
				uint2	pos_in_cache = IndexToVec2( un_Quads.count, cache_dim );
				float	max_uv_y	= float(pos_in_cache.y) / float(cache_dim.y);

				out_Color.rgb = uv.y < max_uv_y ? SDF_Isolines( sdf * 10.0 ) : float3(0.0);
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
