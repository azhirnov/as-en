// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Page allocation test.
	* iRndAllocations=1 - randomly allocate pages each frame (if have free space).
	* iRndAllocations=0 - use right mouse button to allocate selected page in virtual texture.
	* iFreePages=0 - stop deallocating obsolete pages.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define ALLOC_PAGES
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const uint2		table_dim	= uint2( 16 );
		const uint		mip_count	= IntLog2( table_dim.x );
		const uint2		phys_pages	= uint2( 8 );

		// 'phys_tex' contains page ID and lod
		// 'page_table'
		RC<Image>		rt			= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Image>		phys_tex 	= Image( EPixelFormat::R32U, phys_pages );						phys_tex.Name( "Physical texture" );
		RC<Image>		page_table	= Image( EPixelFormat::R32U, uint3(table_dim, mip_count) );		page_table.Name( "Page talbe" );

		RC<Buffer>		params		= Buffer();
		RC<DynamicInt>	delay 		= DynamicInt();
		RC<DynamicInt>	mip			= DynamicInt();

		Slider( delay,	"DelayToFree",		1,	20,				5 );
		Slider( mip,	"Mip",				0,	mip_count-1,	0 );

		page_table.InitialValue( RGBA32u(0) );
		phys_tex.InitialValue( RGBA32u(0) );

		params.UseLayout(
			"ParamsBuffer",
			"	int2	allocPage;"
			"	uint	allocCounter;"
		);

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "ALLOC_PAGES" );
			pass.ArgInOut(	"un_Params",		params );
			pass.ArgInOut(	"un_PhysicalTex",	phys_tex );
			pass.ArgInOut(	"un_PageTable",		page_table );
			pass.Constant(	"iDelayToFree",		delay );
			pass.Constant(	"iMip",				mip );
			pass.Slider(	"iRndAllocations",	0,	1,	1 );
			pass.Slider(	"iFreePages",		0,	1,	1 );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( 	"out_Color",		rt );
			pass.ArgIn(		"un_PhysicalTex",	phys_tex,		Sampler_NearestClamp );
			pass.ArgIn(		"un_PageTable",		page_table,		Sampler_NearestClamp );
			pass.ArgInOut(	"un_Params",		params );
			pass.Constant(	"iMip",				mip );
			pass.Constant(	"iDelayToFree",		delay );
			pass.Slider(	"iViewMode",		0,		1,		0 );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef ALLOC_PAGES
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	const uint	c_Skip		= ~0u;
	const uint	c_Pending	= ~0u;
	const uint	c_AllocBit	= 1u << 31;


	uint  PackOwner (int2 pageId, int lod)
	{
		return	1u |
				((uint(pageId.x) & 0xFF) << 1) |
				((uint(pageId.y) & 0xFF) << 9) |
				((uint(lod) & 0x7F) << 17);
	}

	uint3  UnpackOwner (uint packed)
	{
		uint	page_x	= (packed >> 1) & 0xFF;
		uint	page_y	= (packed >> 9) & 0xFF;
		uint	lod		= (packed >> 17) & 0x7F;
		return	uint3( page_x, page_y, lod );
	}

	uint  PackPage (uint slot, uint time)
	{
		// 'slot' - linear page ID
		// 'time' - last usage time in seconds

		return c_AllocBit | (slot << 16) | time;
	}


	void  FreeUnusedPages (const int2 physDim)
	{
		if ( iFreePages == 0 )
			return;

		const uint	slot_count	= uint(physDim.x * physDim.y);
		const uint	s			= un_PerPass.frame % slot_count;
		int2		phys		= int2( int(s) % physDim.x, int(s) / physDim.x );
		uint		owner		= gl.image.Load( un_PhysicalTex, phys ).r;

		if ( owner == 0 )
			return;

		uint3	pp		= UnpackOwner( owner );
		uint	texel	= gl.image.Load( un_PageTable, int3(int2(pp.xy), int(pp.z)) ).r;

		if ( texel == c_Pending )
			return;

		float	dt = un_PerPass.time - float(texel & 0xFFFF);

		if ( dt <= float(iDelayToFree) )
			return;

		gl.image.Store( un_PageTable, int3(int2(pp.xy), int(pp.z)), uint4(0) );
		gl.image.Store( un_PhysicalTex, phys, uint4(0) );
	}


	void  AllocatePage (const int2 pageId, const int2 physDim)
	{
		const uint	slot_count	= uint(physDim.x * physDim.y);
		const uint	time		= uint(un_PerPass.time) & 0xFFFF;
		const int	lod			= iMip;

		// dedup: claim the page
		uint	old = gl.image.AtomicCompSwap( un_PageTable, int3(pageId, lod), 0u, c_Pending );
		if ( old != 0 )
			return;		// already loaded or claimed by another request

		// find a victim slot (FIFO ring, skip slots still in use)
		uint	slot = c_Skip;
		for (int a = 0; a < 16; ++a)
		{
			uint	s		= gl.AtomicAdd( INOUT un_Params.allocCounter, 1 ) % slot_count;
			int2	phys	= int2( int(s) % physDim.x, int(s) / physDim.x );
			uint	owner	= gl.image.Load( un_PhysicalTex, phys ).r;

			if ( owner != 0 )
			{
				if ( iFreePages == 0 )
					continue;

				uint3	pp		= UnpackOwner( owner );
				uint	used	= gl.image.Load( un_PageTable, int3(int2(pp.xy), int(pp.z)) ).r & 0xFFFF;
				float	dt		= un_PerPass.time - float(used);

				if ( dt < float(iDelayToFree) )
					continue;
			}

			slot = s;
			break;
		}

		if ( slot == c_Skip )
		{
			// couldn't allocate: release the claim, the page will be requested again next frame
			gl.image.Store( un_PageTable, int3(pageId, lod), uint4(0) );
			return;
		}

		// evict previous owner
		int2	phys	= int2( int(slot) % physDim.x, int(slot) / physDim.x );
		uint	prev	= gl.image.AtomicExchange( un_PhysicalTex, phys, PackOwner( pageId, lod ));
		if ( prev != 0 )
		{
			uint3	pp = UnpackOwner( prev );
			gl.image.Store( un_PageTable, int3(int2(pp.xy), int(pp.z)), uint4(0) );
		}

		gl.image.Store( un_PageTable, int3(pageId, lod), uint4(PackPage( slot, time )) );
	}


	void  Main ()
	{
		const int2	phys_dim 	= gl.image.GetSize( un_PhysicalTex );
		int2		page_id		= un_Params.allocPage;

		FreeUnusedPages( phys_dim );

		if ( iRndAllocations == 1 )
		{
			int2	table_dim	= Max( gl.image.GetSize( un_PageTable ).xy >> iMip, 1 );
			float2	rnd			= DHash21( float(un_PerPass.frame) + 11.1 );
			page_id				= Min( int2( rnd * float2(table_dim) ), table_dim - 1 );
		}

		if ( AnyLess( page_id, 0 ))
			return;

		un_Params.allocPage = int2(-1);

		AllocatePage( page_id, phys_dim );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Color.glsl"
	#include "InvocationID.glsl"

	float2  ScreenPosToUV (float2 scrPos)
	{
		float2	scale 	= float2(1.0, un_PerPass.resolution.x * un_PerPass.invResolution.y);
		float2	uv 		= (scrPos - un_PerPass.resolution.xy * 0.5) * (un_PerPass.invResolution * 2.0);

		uv.x  	= (Abs( uv.x  ) - 0.5) * 2.0;
		uv  	*= scale;
		return 	uv;
	}

	float  MouseScreenX ()
	{
		return un_PerPass.mouse.x * un_PerPass.resolution.x - un_PerPass.resolution.x * 0.5;
	}

	uint3  UnpackOwner (uint packed)
	{
		uint	page_x	= (packed >> 1) & 0xFF;
		uint	page_y	= (packed >> 9) & 0xFF;
		uint	lod		= (packed >> 17) & 0x7F;
		return	uint3( page_x, page_y, lod );
	}

	int2  SlotToPhys (uint slot)
	{
		int2	phys_dim = gl.texture.GetSize( un_PhysicalTex, 0 );
		return	int2( int(slot) % phys_dim.x, int(slot) / phys_dim.x );
	}

	uint  PhysToSlot (int2 coord)
	{
		int2	phys_dim = gl.texture.GetSize( un_PhysicalTex, 0 );
		return	uint( coord.x + coord.y * phys_dim.x );
	}

	float4  PhysSlotColor (uint slot)
	{
		int2	phys_dim	= gl.texture.GetSize( un_PhysicalTex, 0 );
		float	count		= float( phys_dim.x * phys_dim.y - 1 );
		return	Rainbow( float(slot) / Max( count, 1.0 ));
	}


	void  DrawPageTable (float2 uv, float2 uv2)
	{
		if ( IsNotSNorm( uv ))
			return;

		float2	dim		= float2(Max( gl.texture.GetSize( un_PageTable, 0 ).xy >> iMip, 1 ));
		int2	coord  	= int2(ToUNorm( uv  ) * dim);
		int2	coord2 	= int2(ToUNorm( uv2 ) * dim);
		uint4	texel 	= gl.texture.Fetch( un_PageTable, int3(coord, iMip), 0 );

		float	last_used_at_sec = float(texel.r & 0xFFFF);
		bool	allocated 		 = (texel.r >> 31) != 0;

		float	dt 		= un_PerPass.time - last_used_at_sec;

		out_Color = Rainbow2( dt / float(iDelayToFree) );

		if ( dt > float(iDelayToFree) )
			out_Color = float4(1.0, 0.0, 1.0, 1.0);

		if ( ! allocated )
			out_Color = float4(0.2);

		if ( iViewMode == 1 )
			out_Color = allocated ? PhysSlotColor( (texel.r >> 16) & 0x7FFF ) : float4(0.2);

		float2	mpos		= ScreenPosToUV( un_PerPass.mouse.xy * un_PerPass.resolution.xy );
		bool	alloc_page	= un_PerPass.mouse.z > 0.0;
		bool	m_vtable	= MouseScreenX() > 1.5;
		bool	m_phys		= MouseScreenX() < -1.5;
		int2	mipos		= int2(ToUNorm(mpos) * dim);

		// highlight page
		if ( m_vtable and AllEqual( coord, mipos ))
		{
			if ( alloc_page )
				un_Params.allocPage = coord;

			out_Color = float4(1.0);
		}

		// mouse on physical page, highlight same page on virtual texture
		if ( m_phys )
		{
			int2	p_mipos	= int2(ToUNorm(mpos) * float2(gl.texture.GetSize( un_PhysicalTex, 0 )));
			uint	owner	= gl.texture.Fetch( un_PhysicalTex, p_mipos, 0 ).r;

			if ( owner != 0 )
			{
				uint3	pp = UnpackOwner( owner );
				if ( int(pp.z) == iMip and AllEqual( coord, int2(pp.xy) ))
					out_Color = float4(1.0);
			}
		}

		out_Color.rgb *= 0.8;

		// page border
		if ( AnyNotEqual( coord, coord2 ))
			out_Color = float4(1.0);
	}


	void  DrawPhysicalTex (float2 uv, float2 uv2)
	{
		if ( IsNotSNorm( uv ))
			return;

		int2	coord  	= int2(ToUNorm( uv ) * float2(gl.texture.GetSize( un_PhysicalTex, 0 )));
		int2	coord2 	= int2(ToUNorm( uv2 ) * float2(gl.texture.GetSize( un_PhysicalTex, 0 )));
		uint4	texel 	= gl.texture.Fetch( un_PhysicalTex, coord, 0 );

		bool	allocated = (texel.r != 0);

		out_Color = (allocated ? float4(0.0, 1.0, 0.0, 1.0) : float4(1.0, 0.0, 0.0, 1.0)) * 0.8;

		if ( iViewMode == 1 )
			out_Color = allocated ? PhysSlotColor( PhysToSlot( coord )) * 0.8 : float4(0.2);

		float2	mpos		= ScreenPosToUV( un_PerPass.mouse.xy * un_PerPass.resolution.xy );
		bool	m_vtable	= MouseScreenX() > 1.5;
		bool	m_phys		= MouseScreenX() < -1.5;
		int2	mipos		= int2(ToUNorm(mpos) * float2(gl.texture.GetSize( un_PhysicalTex, 0 )));

		// highlight page
		if ( m_phys and AllEqual( coord, mipos ))
			out_Color = float4(1.0);

		// mouse on virtual texture, highlight same page on physical texture
		if ( m_vtable )
		{
			int2	vdim	= Max( gl.texture.GetSize( un_PageTable, 0 ).xy >> iMip, 1 );
			int2	v_mipos	= int2(ToUNorm(mpos) * float2(vdim));
			uint	vtexel	= gl.texture.Fetch( un_PageTable, int3(v_mipos, iMip), 0 ).r;

			if ( (vtexel >> 31) != 0 )
			{
				int2	phys = SlotToPhys( (vtexel >> 16) & 0x7FFF );
				if ( AllEqual( coord, phys ))
					out_Color = float4(1.0);
			}
		}

		out_Color.rgb *= 0.8;

		// page border
		if ( AnyNotEqual( coord, coord2 ))
			out_Color = float4(1.0);
	}


	void  Main ()
	{
		float	x = gl.FragCoord.x - un_PerPass.resolution.x * 0.5;

		out_Color = float4(0.0);

		if ( Abs(x) < 1.5 )
			out_Color = float4(1.0);
		else
		{
			float2	uv 		= ScreenPosToUV( gl.FragCoord.xy );
			float2	uv2		= ScreenPosToUV( gl.FragCoord.xy + 1.0 );

			if ( x > 0.0 )
				DrawPageTable( uv, uv2 );
			else
				DrawPhysicalTex( uv, uv2 );
		}
	}

#endif
//-----------------------------------------------------------------------------
