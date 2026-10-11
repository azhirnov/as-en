// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Basic implementation of virtual textures.
	Limitations:
	* only bilinear filtering
	* no tile borders
	* only single 8K mega texture
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
		const uint		tile_dim		= 256;
		const uint2		phys_dim		= indir_dim * tile_dim;

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
		}

		// render loop
		{
			// copy 1 tile per frame
			RC<ComputePass>		pass = ComputePass( "", "INIT_PHYS_TEX" );
			pass.ArgIn(		"un_MegaTex",			mega_tex,			Sampler_NearestRepeat );
			pass.ArgInOut(	"un_IndirectionMap",	indirection_map );
			pass.ArgInOut(	"un_PhysicalTex",		physical_tex );
			pass.Constant(	"iTileDim",				tile_dim );
			pass.Constant(	"iCellPerSide",			int(cell_per_side) );
			pass.LocalSize( 16, 16 );
			pass.DispatchThreads( tile_dim, tile_dim );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_2D" );
			pass.Output(	"out_Color",			rt );
			pass.ArgIn(		"un_MegaTex",			mega_tex,			Sampler_NearestClamp );
			pass.ArgIn(		"un_IndirectionMap",	indirection_map,	Sampler_NearestClamp );
			pass.ArgIn(		"un_PhysicalTex",		physical_tex,		Sampler_LinearRepeat );
			pass.Slider(	"iDbgView",				0,		3 );
			pass.Constant(	"iTileDim",				tile_dim );
			pass.Constant(	"iPhysicalDim",			phys_dim );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_3D" );
			pass.Set( camera );
			pass.Output(	"out_Color",			rt );
			pass.ArgIn(		"un_IndirectionMap",	indirection_map,	Sampler_NearestClamp );
			pass.ArgIn(		"un_PhysicalTex",		physical_tex,		Sampler_LinearRepeat );
			pass.Slider(	"iDbgView",				0,		2,		1 );
			pass.Constant(	"iTileDim",				tile_dim );
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
		int2	dst_tile_count	= gl.image.GetSize( un_IndirectionMap );
		int2	indirect_pos	= IndexToVec2( int(un_PerPass.frame), dst_tile_count );								// in tiles
				indirect_pos.y	%= dst_tile_count.y;

		int		lod				= indirect_pos.y;
		int2	cell_id			= int2( indirect_pos.x % iCellPerSide, indirect_pos.x / iCellPerSide ) - iCellPerSide/2;

		int2	tex_dim			= gl.texture.GetSize( un_MegaTex, lod );
		int2	tile_src_pos	= tex_dim/2 + cell_id * iTileDim + GetGlobalCoord().xy;								// in pixels

		float2	src_uv			= float2(tile_src_pos) / float2(tex_dim);
		float4	texel			= gl.texture.SampleLod( un_MegaTex, src_uv, float(lod) );

		// add shuffle
		int2	tile_dst_id		= int2( dst_tile_count.x-1 - indirect_pos.x,
										(indirect_pos.y + 3) % dst_tile_count.y );									// in tiles
	//	int2	tile_dst_id		= indirect_pos;		// without shuffle, for debugging

		int2	tile_dst_pos	= tile_dst_id * iTileDim + GetGlobalCoord().xy;										// in pixels

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
		uint2	phys_tile	= gl.texture.Fetch( un_IndirectionMap, int2(tile_coord), 0 ).rg * iTileDim;
		float2	tex_uv		= (float2(phys_tile) + tile_uv) / float2(iPhysicalDim);

		out_Color = gl.texture.SampleLod( un_PhysicalTex, tex_uv, 0.0 );

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
