// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS

#include "geometry_tools/Cube/CubeRenderer.h"

namespace AE::GeometryTools
{

/*
=================================================
	Create
=================================================
*/
	bool  CubeRenderer::Create (IResourceManager &resMngr, ITransferContext &ctx, Bool cubeMap, GfxMemAllocatorPtr gfxAlloc) __NE___
	{
		Destroy( resMngr );

		CubeGen		gen;
		CHECK_ERR( gen.Create( cubeMap ));

		const auto	vertices	= gen.GetVertices();
		const auto	indices		= gen.GetIndices();

		CHECK( _indexCount == indices.size() );

		_vertexBuffer	= resMngr.CreateBuffer( BufferDesc{ ArraySizeOf(vertices), EBufferUsage::Vertex | EBufferUsage::TransferDst }, "Cube vertices", gfxAlloc );
		_indexBuffer	= resMngr.CreateBuffer( BufferDesc{ ArraySizeOf(indices),  EBufferUsage::Index  | EBufferUsage::TransferDst }, "Cube indices",  gfxAlloc );
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
	void  CubeRenderer::Destroy (IResourceManager &resMngr) __NE___
	{
		resMngr.ReleaseResource( INOUT _vertexBuffer );
		resMngr.ReleaseResource( INOUT _indexBuffer );
	}


} // AE::GeometryTools

#endif // AE_GEOMTOOLS_HAS_GRAPHICS
