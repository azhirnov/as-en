// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Basic implementation of virtual textures.
	Software trilinear/anisotropy filtering between mip levels.
	Still used HW filtering inside single mip level.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_PHYS_TEX
#	define VIEW_2D
#	define VIEW_3D
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<FPVCamera>	camera			= FPVCamera();

		const uint		cell_per_side	= 4;
		const uint		max_lod			= 10;
		const uint2		indir_dim		= uint2( cell_per_side * cell_per_side, max_lod );

		const uint		border			= 4;	// 4x anisotropy
		const uint		phys_tile_dim	= 256;	// including border
		const uint		tile_dim		= phys_tile_dim - border*2;
		const uint2		phys_dim		= indir_dim * phys_tile_dim;

		RC<Image>		physical_tex	= Image( EPixelFormat::RGBA8_UNorm, uint2(phys_dim) );
		RC<Image>		indirection_map	= Image( EPixelFormat::RG8U, uint2(indir_dim) );
		RC<Image>		mega_tex		= Image( EImageType::Float_2D, "res/terrain-tex4-8k/grassy_soil_xbklfix_8k/Grassy_Soil_xbklfix_8K_BaseColor.jpg", ImageLoadOpFlags::GenMipmaps );

		RC<DynamicUInt>	mode	= DynamicUInt();

		Slider( mode,	"3D",		0,	1,	1 );	// 2d, 3d

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position(float3( 0.0, 0.7, 0.0 ));
		}

		// render loop
		{
			// copy 1 tile per frame
			RC<ComputePass>		pass = ComputePass( "", "INIT_PHYS_TEX" );
			pass.ArgIn(		"un_MegaTex",			mega_tex,			Sampler_NearestRepeat );
			pass.ArgInOut(	"un_IndirectionMap",	indirection_map );
			pass.ArgInOut(	"un_PhysicalTex",		physical_tex );
			pass.Constant(	"iTileDim",				tile_dim );
			pass.Constant(	"iTileBorder",			border );
			pass.Constant(	"iCellPerSide",			int(cell_per_side) );
			pass.LocalSize( 16, 16 );
			pass.DispatchThreads( phys_tile_dim, phys_tile_dim );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_2D" );
			pass.Output(	"out_Color",			rt );
			pass.ArgIn(		"un_MegaTex",			mega_tex,			Sampler_NearestClamp );
			pass.ArgIn(		"un_IndirectionMap",	indirection_map,	Sampler_NearestClamp );
			pass.ArgIn(		"un_PhysicalTex",		physical_tex,		Sampler_LinearMipmapRepeat );
			pass.Slider(	"iDbgView",				0,		3 );
			pass.Constant(	"iTileBorder",			border );
			pass.Constant(	"iTileDim",				tile_dim );
			pass.Constant(	"iPhysTileDim",			phys_tile_dim );
			pass.Constant(	"iPhysicalDim",			phys_dim );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_3D" );
			pass.Set( camera );
			pass.Output(	"out_Color",			rt );
			pass.ArgIn(		"un_MegaTex",			mega_tex,			Sampler_Anisotropy4Repeat );
			pass.ArgIn(		"un_IndirectionMap",	indirection_map,	Sampler_NearestClamp );
			pass.ArgIn(		"un_PhysicalTexBi",		physical_tex,		Sampler_LinearRepeat );
			pass.ArgIn(		"un_PhysicalTexTri",	physical_tex,		Sampler_LinearMipmapRepeat );
			pass.ArgIn(		"un_PhysicalTexAni2",	physical_tex,		Sampler_Anisotropy2Repeat );
			pass.ArgIn(		"un_PhysicalTexAni4",	physical_tex,		Sampler_Anisotropy4Repeat );
			pass.Slider(	"iDbgView",				0,		4,		1 );
			pass.Slider(	"iFilterLeft",			0,		4,		0 );
			pass.Slider(	"iFilterRight",			0,		4,		4 );
			pass.Slider(	"iTexLodBias",			-2.0,	2.0,	0.37 );
			pass.Slider(	"iCmpScale",			0,		10,		5 );
			pass.Constant(	"iTileBorder",			border );
			pass.Constant(	"iTileDim",				tile_dim );
			pass.Constant(	"iPhysTileDim",			phys_tile_dim );
			pass.Constant(	"iPhysicalDim",			phys_dim );
			pass.Constant(	"iMaxLOD",				max_lod );
			pass.Constant(	"iCellPerSide",			cell_per_side );
			pass.EnableIfEqual( mode, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT_PHYS_TEX
	#include "Math.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		int2	px_coord		= GetGlobalCoord().xy;
		int2	dst_tile_count	= gl.image.GetSize( un_IndirectionMap );
		int2	indirect_pos	= IndexToVec2( int(un_PerPass.frame), dst_tile_count );								// in tiles
				indirect_pos.y	%= dst_tile_count.y;

		int		lod				= indirect_pos.y;
		int2	cell_id			= int2( indirect_pos.x % iCellPerSide, indirect_pos.x / iCellPerSide ) - iCellPerSide/2;

		int2	tex_dim			= gl.texture.GetSize( un_MegaTex, lod );
		int2	tile_src_pos	= tex_dim/2 + cell_id * iTileDim + px_coord - iTileBorder;							// in pixels

		float2	src_uv			= (float2(tile_src_pos) + 0.5) / float2(tex_dim);
		float4	texel			= gl.texture.SampleLod( un_MegaTex, src_uv, float(lod) );

		int2	tile_dst_id		= int2( dst_tile_count.x-1 - indirect_pos.x,
										(indirect_pos.y + 3) % dst_tile_count.y );									// in tiles

		int		phys_tile_dim	= iTileDim + iTileBorder*2;
		int2	tile_dst_pos	= tile_dst_id * phys_tile_dim + px_coord;											// in pixels

		gl.image.Store( un_PhysicalTex, tile_dst_pos, texel );

		if ( OncePerDispatch() )
		{
			gl.image.Store( un_IndirectionMap, indirect_pos, uint4(tile_dst_id,0,0) );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_2D
	#include "InvocationID.glsl"

	void  Main ()
	{
		const float2	uv = GetGlobalCoordUNorm().xy;

		switch ( iDbgView )
		{
			case 0 :
			{
				out_Color = gl.texture.SampleLod( un_PhysicalTex, uv, 0.0 );
				return;
			}
			case 1 :
			{
				float2	phys_coord	= float2(gl.texture.SampleLod( un_IndirectionMap, uv, 0.0 ).rg);
				float2	subpix		= Fract( uv * float2(gl.texture.GetSize( un_IndirectionMap, 0 )) );
				float2	uv2			= (phys_coord + subpix) * float(iTileDim) / float2(iPhysicalDim);
				out_Color = gl.texture.SampleLod( un_PhysicalTex, uv2, 0.0 );
				out_Color.a = phys_coord.x * 10.0 + phys_coord.y;
				return;
			}
			case 2 :
			{
				float2	phys_coord	= float2(gl.texture.SampleLod( un_IndirectionMap, uv, 0.0 ).rg);
				out_Color = float4(phys_coord, 0.0, 1.0);
				return;
			}
			case 3 :
			{
				out_Color = gl.texture.SampleLod( un_MegaTex, uv, 0.0 );
				return;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_3D
	#include "Color.glsl"
	#include "TexSampling.glsl"
	#include "Intersectors.glsl"
	#include "InvocationID.glsl"

	const float	c_GroundY = 1.0;


	float  ClipmapLOD (float2 posXZ)
	{
		float2	rel		= Abs( posXZ /*- un_PerPass.camera.pos.xz*/ );
		float   dist	= Max( rel.x, rel.y );
		float	height	= Abs( un_PerPass.camera.pos.y - c_GroundY );
				dist	= Max( dist, height * 0.5 );
		float   lod		= Log2( Max( dist, 1.0 ));
		float   level	= Floor( lod );
		return	level;
	}


	struct ColorSamples
	{
		float4	bi;
		float4	tri;
		float4	ani2;
		float4	ani4;

		float	lod0;
		float	lod1;
		float	fracPart;
	};

	ColorSamples  TexSample (const float2 pos, const float clipmapLOD, const float refLOD, const float2 virtualDim)
	{
		float2	duv_scale	= float2(iTileDim) * Exp2( iTexLodBias ) / virtualDim;
		float2	dx			= gl.dFdx( pos ) * duv_scale;
		float2	dy			= gl.dFdy( pos ) * duv_scale;
	//	float	tex_lod		= Max( 0.0, SwQueryLod( virtualDim, float4(dx,dy) ).y );					// for trilinear filtering
		float	tex_lod		= Max( 0.0, SwQueryLodAnisotropic( virtualDim, float4(dx,dy), 4.0 ).y );	// for anisotropy filtering
	//	float	tex_lod		= Max( 0.0, refLOD );

		float	lod0		= Clamp( Floor( tex_lod ), clipmapLOD, float(iMaxLOD) - 1.0 );
		float	lod1		= Clamp( Ceil( tex_lod ), clipmapLOD, float(iMaxLOD) - 1.0 );

		float2	cell_id_f0	= pos * Exp2( -lod0 );
		float2	cell_id_f1	= pos * Exp2( -lod1 );

		float2	tile_uv0	= Fract( cell_id_f0 ) * float(iTileDim);
		float2	tile_uv1	= Fract( cell_id_f1 ) * float(iTileDim);

		int2	cell_id0	= Clamp( int2(Floor( cell_id_f0 )) + iCellPerSide/2, 0, iCellPerSide-1 );
		int2	cell_id1	= Clamp( int2(Floor( cell_id_f1 )) + iCellPerSide/2, 0, iCellPerSide-1 );

		int2	tile_coord0	= int2( cell_id0.x + cell_id0.y * iCellPerSide, int(lod0) );
		int2	tile_coord1	= int2( cell_id1.x + cell_id1.y * iCellPerSide, int(lod1) );

		uint2	phys_tile0	= gl.texture.Fetch( un_IndirectionMap, int2(tile_coord0), 0 ).rg * iPhysTileDim + iTileBorder;
		uint2	phys_tile1	= gl.texture.Fetch( un_IndirectionMap, int2(tile_coord1), 0 ).rg * iPhysTileDim + iTileBorder;
		float2	tex_uv0		= (float2(phys_tile0) + tile_uv0) / float2(iPhysicalDim);
		float2	tex_uv1		= (float2(phys_tile1) + tile_uv1) / float2(iPhysicalDim);

		float2	dx0			= dx * Exp2( -lod0 );
		float2	dy0			= dy * Exp2( -lod0 );
		float2	dx1			= dx * Exp2( -lod1 );
		float2	dy1			= dy * Exp2( -lod1 );

		float4	tri_0		= gl.texture.SampleLod( un_PhysicalTexTri, tex_uv0, lod0 );
		float4	ani2_0		= gl.texture.SampleGrad( un_PhysicalTexAni2, tex_uv0, dx0, dy0 );
		float4	ani4_0		= gl.texture.SampleGrad( un_PhysicalTexAni4, tex_uv0, dx0, dy0 );

		float4	tri_1		= gl.texture.SampleLod( un_PhysicalTexTri, tex_uv1, lod1 );
		float4	ani2_1		= gl.texture.SampleGrad( un_PhysicalTexAni2, tex_uv1, dx1, dy1 );
		float4	ani4_1		= gl.texture.SampleGrad( un_PhysicalTexAni4, tex_uv1, dx1, dy1 );

		ColorSamples	col;
	//	col.fracPart = Saturate( tex_lod - lod0 );
		col.fracPart = (lod0 == lod1) ? 0.0 : Saturate((tex_lod - lod0) / (lod1 - lod0));
		col.lod0	 = lod0;
		col.lod1	 = lod1;

		col.tri  = Lerp( tri_0,  tri_1,  col.fracPart );
		col.ani2 = Lerp( ani2_0, ani2_1, col.fracPart );
		col.ani4 = Lerp( ani4_0, ani4_1, col.fracPart );
		col.bi	 = gl.texture.SampleLod( un_PhysicalTexBi, tex_uv0, 0.0 );
		return col;
	}


	void  Main ()
	{
		const float		max_lod = float(iMaxLOD);

		float2	pos;
		bool	isec;
		{
			Ray	ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, c_GroundY, 0.0), float3(0.0, 1.0, 0.0), OUT t );

			Ray_SetLength( ray, t );

			pos = ray.pos.xz;
		}

		float	lod			= Clamp( ClipmapLOD( pos ), 0.0, max_lod - 1.0 );
		float2	cell_id_f	= pos * Exp2( -lod );

		float2	cell_uv		= ToSNorm( Fract( cell_id_f ));
		float	grid		= 1.0 - SmoothStep( MaxOf(Abs( cell_uv )), 0.99, 1.0 );

		float2	vt_dim		= float2(gl.texture.GetSize( un_MegaTex, 0 ));
		float2	ref_uv		= pos * float2(iTileDim) / vt_dim - 0.5;
		float2	ref_lod		= gl.texture.QueryLod( un_MegaTex, ref_uv );

		float4			col_ref	= gl.texture.Sample( un_MegaTex, ref_uv );
		ColorSamples	col		= TexSample( pos, lod, ref_lod.y, vt_dim );

		float	x			= GetGlobalCoordUNorm().x;
		int		filter_type	= x < 0.5 ? iFilterLeft : iFilterRight;
		switch ( filter_type )
		{
			case 0 :	out_Color = col_ref; break;
			case 1 :	out_Color = col.bi; break;
			case 2 :	out_Color = col.tri; break;
			case 3 :	out_Color = col.ani2; break;
			case 4 :	out_Color = col.ani4; break;
		}

		if ( iFilterLeft != iFilterRight )
			out_Color.rgb *= SmoothStep( Abs(x - 0.5), 0.0, un_PerPass.invResolution.x * 1.5 );  // split screen

		switch ( iDbgView )
		{
			case 1 :	// texture + grid
				out_Color.rgb *= grid;
				break;

			case 2 :	// lods
				out_Color = Rainbow2( Saturate( lod / max_lod )) * grid;
				out_Color.a = lod;
				break;

			case 3 :	// lods
			{
				float	lod0	= filter_type == 0 ? Floor( ref_lod.y ) : col.lod0;
				float	lod1	= filter_type == 0 ? Ceil( ref_lod.y ) : col.lod1;
				float	lod_f	= filter_type == 0 ? Fract( ref_lod.y ) : col.fracPart;

				float4	col0 = Rainbow2( Saturate( lod0 / max_lod ));
				float4	col1 = Rainbow2( Saturate( lod1 / max_lod ));

				out_Color = Lerp( col0, col1, lod_f ) * grid;
				out_Color.a = lod0 + lod_f;
				break;
			}
			case 4 :	// compare
			{
				const float	col_scale = Exp2( iCmpScale );
				switch ( iFilterRight ) {
					case 0 :	out_Color = float4(0.0); break;
					case 1 :	out_Color = Abs( col.bi - col_ref ) * col_scale; break;
					case 2 :	out_Color = Abs( col.tri - col_ref ) * col_scale; break;
					case 3 :	out_Color = Abs( col.ani2 - col_ref ) * col_scale; break;
					case 4 :	out_Color = Abs( col.ani4 - col_ref ) * col_scale; break;
				}
				break;
			}
		}

		if ( !isec )
		{
			// sky
			out_Color = float4(0.6, 0.6, 1.0, 1.0);
		}
	}

#endif
//-----------------------------------------------------------------------------
