// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS

#include "SphericalCubeGen.h"
#include "graphics/Public/CommandBuffer.h"
#include "graphics/Public/ResourceManager.h"

namespace AE::GeometryTools
{
	using namespace AE::Graphics;


	//
	// Spherical Cube Renderer
	//

	class SphericalCubeRenderer final : public SphericalCubeGen
	{
	// variables
	private:
		Strong<BufferID>	_vertexBuffer;
		Strong<BufferID>	_indexBuffer;


	// methods
	public:
		SphericalCubeRenderer () {}
		~SphericalCubeRenderer ();

		ND_ bool  Create (IResourceManager &, ITransferContext &ctx, uint minLod, uint maxLod, bool quads);
			void  Destroy (IResourceManager &);
		
		ND_ bool  IsCreated () const	{ return _vertexBuffer and _indexBuffer; }
		
			template <typename DrawCtx>
			bool  Draw (DrawCtx &ctx, uint lod) const;

		ND_ bool  GetVertexBuffer (uint lod, uint face, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint2 &vertCount) const;
		ND_ bool  GetIndexBuffer (uint lod, uint face, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint &indexCount) const;
	};

	
	template <typename DrawCtx>
	bool  SphericalCubeRenderer::Draw (DrawCtx &ctx, uint lod) const
	{
		CHECK_ERR( lod >= _minLod and lod <= _maxLod );
		lod = Clamp( lod, _minLod, _maxLod );

		BufferID		vb, ib;
		Range<Bytes>	vb_range, ib_range;
		uint2			vert_cnt;
		uint			idx_cnt;

		CHECK_ERR( GetVertexBuffer( lod, 0, OUT vb, OUT vb_range, OUT vert_cnt ));
		CHECK_ERR( GetIndexBuffer( lod, 0, OUT ib, OUT ib_range, OUT idx_cnt ));
		
		ctx.BindVertexBuffer( 0, vb, vb_range.Offset() );
		ctx.BindIndexBuffer( ib, ib_range.Offset(), EIndex::UInt );
		
		DrawIndexedCmd	cmd;
		cmd.indexCount		= idx_cnt * 6;
		cmd.instanceCount	= 1;
		cmd.firstIndex		= 0;
		cmd.vertexOffset	= 0;
		cmd.firstInstance	= 0;

		ctx.DrawIndexed( cmd );
		return true;
	}

} // AE::GeometryTools

#endif // AE_GEOMTOOLS_HAS_GRAPHICS
