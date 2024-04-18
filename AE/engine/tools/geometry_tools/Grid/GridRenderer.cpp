// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS

#include "geometry_tools/Grid/GridRenderer.h"

namespace AE::GeometryTools
{

/*
=================================================
	destructor
=================================================
*/
	GridRenderer::~GridRenderer () __NE___
	{
		CHECK( not _vertexBuffer );
		CHECK( not _indexBuffer );
	}

/*
=================================================
	Create
=================================================
*/
	bool  GridRenderer::Create (IResourceManager &resMngr, ITransferContext &ctx, GfxMemAllocatorPtr gfxAlloc, uint vertsPerEdge, uint patchSize) __NE___
	{
		Destroy( resMngr );

		GridGen		gen;
		CHECK_ERR( gen.Create( vertsPerEdge, patchSize ));

		const auto	vertices	= gen.GetVertices();
		const auto	indices		= gen.GetIndices();

		_indexCount		= uint(indices.size());
		_vertexBuffer	= resMngr.CreateBuffer( BufferDesc{ ArraySizeOf(vertices), EBufferUsage::Vertex | EBufferUsage::TransferDst }, "Grid vertices", gfxAlloc );
		_indexBuffer	= resMngr.CreateBuffer( BufferDesc{ ArraySizeOf(indices),  EBufferUsage::Index  | EBufferUsage::TransferDst }, "Grid indices",  gfxAlloc );
		CHECK_ERR( _vertexBuffer and _indexBuffer );

		CHECK_ERR( ctx.UploadBuffer( _vertexBuffer, 0_b, vertices ));
		CHECK_ERR( ctx.UploadBuffer( _indexBuffer,  0_b, indices  ));

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  GridRenderer::Destroy (IResourceManager &resMngr) __NE___
	{
		resMngr.ReleaseResource( INOUT _vertexBuffer );
		resMngr.ReleaseResource( INOUT _indexBuffer );
	}


} // AE::GeometryTools

#endif // AE_GEOMTOOLS_HAS_GRAPHICS
