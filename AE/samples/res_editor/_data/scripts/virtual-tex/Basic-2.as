// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Basic implementation of virtual textures.
	Added tile borders and trilinear filtering.

	Limitations:
	* VT is not updated when camera moved
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
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<FPVCamera>	camera			= FPVCamera();

		const uint		cell_per_side	= 4;
		const uint		max_lod			= 10;
		const uint2		indir_dim		= uint2( cell_per_side * cell_per_side, max_lod );

		const uint		border			= 1;	// for correct mipmaps should be 3
		const uint		phys_tile_dim	= 256;	// including border
		const uint		tile_dim		= phys_tile_dim - border*2;
		const uint2		phys_dim		= indir_dim * phys_tile_dim;

		RC<Image>		physical_tex	= Image( EPixelFormat::RGBA8_UNorm, uint2(phys_dim), MipmapLevel(2) );
		RC<Image>		physical_tex1	= physical_tex.CreateView( EImage_2D, MipmapLevel(1), 1 );
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
		}

		// render loop
		{
			// copy 1 tile per frame
			RC<ComputePass>		pass = ComputePass( "", "INIT_PHYS_TEX" );
			pass.ArgIn(		"un_MegaTex",			mega_tex,			Sampler_NearestRepeat );
			pass.ArgInOut(	"un_IndirectionMap",	indirection_map );
			pass.ArgInOut(	"un_PhysicalTex0",		physical_tex );
			pass.ArgInOut(	"un_PhysicalTex1",		physical_tex1 );
			pass.Constant(	"iTileDim",				tile_dim );
			pass.Constant(	"iTileBorder",			border );
			pass.Constant(	"iCellPerSide",			int(cell_per_side) );
			pass.LocalSize( 16, 16, 1 );
			pass.DispatchThreads( phys_tile_dim, phys_tile_dim, 2 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_2D" );
			pass.Output(	"out_Color",			rt );
			pass.ArgIn(		"un_MegaTex",			mega_tex,			Sampler_NearestClamp );
			pass.ArgIn(		"un_IndirectionMap",	indirection_map,	Sampler_NearestClamp );
			pass.ArgIn(		"un_PhysicalTex",		physical_tex,		Sampler_LinearMipmapRepeat );
			pass.Slider(	"iDbgView",				0,		4 );
			pass.Constant(	"iTileBorder",			border );
			pass.Constant(	"iTileDim",				tile_dim );
			pass.Constant(	"iPhysTileDim",			phys_tile_dim );
			pass.Constant(	"iPhysicalDim",			phys_dim );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_3D" );
			pass.Set( camera );
			pass.Output(	"out_Color",			rt );
			pass.ArgIn(		"un_IndirectionMap",	indirection_map,	Sampler_NearestClamp );
			pass.ArgIn(		"un_PhysicalTexBi",		physical_tex,		Sampler_LinearRepeat );
			pass.ArgIn(		"un_PhysicalTexTri",	physical_tex,		Sampler_LinearMipmapRepeat );
			pass.Slider(	"iDbgView",				0,		2,		1 );
			pass.Slider(	"iTrilinearFilter",		0,		2,		1 );
			pass.Slider(	"iTexLodBias",			-2.0,	2.0,	0.0 );
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
		int		lod_offset		= GetGlobalCoord().z;

		int2	dst_tile_count	= gl.image.GetSize( un_IndirectionMap );
		int2	indirect_pos	= IndexToVec2( int(un_PerPass.frame), dst_tile_count );								// in tiles
				indirect_pos.y	%= dst_tile_count.y;

		int		lod				= indirect_pos.y + lod_offset;
		int2	cell_id			= int2( indirect_pos.x % iCellPerSide, indirect_pos.x / iCellPerSide ) - iCellPerSide/2;

		int2	tex_dim			= gl.texture.GetSize( un_MegaTex, lod );
		int2	tile_src_dim	= int2(iTileDim) >> lod_offset;
		int		src_border		= iTileBorder >> lod_offset;
		int2	tile_src_pos	= tex_dim/2 + cell_id * tile_src_dim + px_coord - src_border;						// in pixels

		float2	src_uv			= (float2(tile_src_pos) + 0.5) / float2(tex_dim);
		float4	texel			= gl.texture.SampleLod( un_MegaTex, src_uv, float(lod) );

		int2	tile_dst_id		= int2( dst_tile_count.x-1 - indirect_pos.x,
										(indirect_pos.y + 3) % dst_tile_count.y );									// in tiles

		int		phys_tile_dim	= (iTileDim + iTileBorder*2) >> lod_offset;
		int2	tile_dst_pos	= tile_dst_id * phys_tile_dim + px_coord;											// in pixels

		if ( lod_offset == 0 )
			gl.image.Store( un_PhysicalTex0, tile_dst_pos, texel );
		else
		if ( AllLess( px_coord, phys_tile_dim ))
			gl.image.Store( un_PhysicalTex1, tile_dst_pos, texel );

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
		const float2	uv	= GetGlobalCoordUNorm().xy;
		const float2	uv2	= GetGlobalCoordUNorm( int3(1) ).xy;

		switch ( iDbgView )
		{
			case 0 :
			{
				out_Color = gl.texture.SampleLod( un_PhysicalTex, uv, 0.0 );
				return;
			}
			case 1 :
			case 2 :
			{
				int		lod			= iDbgView - 1;

				float2	phys_coord	= float2(gl.texture.SampleLod( un_IndirectionMap, uv, 0.0 ).rg);
				float2	subpix		= Fract( uv * float2(gl.texture.GetSize( un_IndirectionMap, 0 )) );
				float2	phys_uv		= phys_coord * float(iPhysTileDim) + float(iTileBorder);
						phys_uv		+= subpix * float(iTileDim);
						phys_uv		/= float2(iPhysicalDim);

				float2	phys_coord2	= float2(gl.texture.SampleLod( un_IndirectionMap, uv2, 0.0 ).rg);
				float	grid		= AnyNotEqual( phys_coord, phys_coord2 ) ? 0.0 : 1.0;

				out_Color = gl.texture.SampleLod( un_PhysicalTex, phys_uv, float(lod) ) * grid;
				//out_Color.a = phys_coord.x * 10.0 + phys_coord.y;
				return;
			}
			case 3 :
			{
				float2	phys_coord	= float2(gl.texture.SampleLod( un_IndirectionMap, uv, 0.0 ).rg);
				out_Color = float4(phys_coord, 0.0, 1.0);
				return;
			}
			case 4 :
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
	#include "Intersectors.glsl"
	#include "InvocationID.glsl"

	const float	c_GroundY = 1.0;


	float  ClipmapLOD (float2 posXZ)
	{
		float2	rel		= Abs( posXZ /*- un_PerPass.camera.pos.xz*/ );
		float   dist	= Max( rel.x, rel.y );
		float	height	= Abs( un_PerPass.camera.pos.y - c_GroundY );
				dist	= Max( dist, height );
		float   lod		= Log2( Max( dist, 1.0 ));
		float   level	= Floor( lod );
		return	level;
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

		int2	cell_id		= Clamp( int2(Floor( cell_id_f )) + iCellPerSide/2, 0, iCellPerSide-1 );
		int2	tile_coord	= int2( cell_id.x + cell_id.y * iCellPerSide, int(lod) );
		float2	tile_uv		= Fract( cell_id_f ) * float(iTileDim);

		uint2	phys_tile	= gl.texture.Fetch( un_IndirectionMap, int2(tile_coord), 0 ).rg * iPhysTileDim + iTileBorder;
		float2	tex_uv		= (float2(phys_tile) + tile_uv) / float2(iPhysicalDim);

		float4	col_tri;
		{
			float	bias	= Exp2( iTexLodBias );
			float2	scale	= (Exp2( -lod ) * float(iTileDim) * bias) / float2(iPhysicalDim);
			float2	dx		= gl.dFdx( pos ) * scale;
			float2	dy		= gl.dFdy( pos ) * scale;
			col_tri = gl.texture.SampleGrad( un_PhysicalTexTri, tex_uv, dx, dy );
		}

		float4	col_bi	= gl.texture.SampleLod( un_PhysicalTexBi, tex_uv, 0.0 );

		switch ( iTrilinearFilter )
		{
			case 0 :	// only bilinear
				out_Color = col_bi;  break;

			case 1 :	// half screen for bilinear, another for trilinear
			{
				float	x = GetGlobalCoordUNorm().x;
				out_Color = x < 0.5 ? col_bi : col_tri;
				out_Color.rgb *= SmoothStep( Abs(x - 0.5), 0.0, un_PerPass.invResolution.x * 1.5 );  // split screen
				break;
			}
			case 2 :	// only trilinear
				out_Color = col_tri;  break;
		}

		switch ( iDbgView )
		{
			case 1 :	// texture + grid
				out_Color.rgb *= grid;
				break;

			case 2 :	// lods
				out_Color = Rainbow2( Saturate( lod / max_lod )) * grid;
				out_Color.a = lod;
				break;
		}

		if ( !isec )
		{
			// sky
			out_Color = float4(0.6, 0.6, 1.0, 1.0);
		}
	}

#endif
//-----------------------------------------------------------------------------
