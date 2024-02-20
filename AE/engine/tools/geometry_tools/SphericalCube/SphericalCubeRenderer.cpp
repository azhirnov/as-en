// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS
# include "geometry_tools/SphericalCube/SphericalCubeRenderer.h"

namespace AE::GeometryTools
{

/*
=================================================
    Create
=================================================
*/
    bool  SphericalCubeRenderer::Create (IResourceManager &resMngr, ITransferContext &ctx,
                                         const uint minLod, const uint maxLod, const Bool quads, const Bool cubeMap,
                                         const EBufferUsage usage, GfxMemAllocatorPtr gfxAlloc) __NE___
    {
        Destroy( resMngr );

        SphericalCubeGen    gen;
        CHECK_ERR( gen.Create( minLod, maxLod, quads, cubeMap ));

        _minLod = minLod;
        _maxLod = maxLod;
        _quads  = quads;

        const auto  vertices    = gen.GetVertices();
        const auto  indices     = gen.GetIndices();

        // create resources
        _vertexBuffer   = resMngr.CreateBuffer( BufferDesc{ ArraySizeOf(vertices), EBufferUsage::Vertex | EBufferUsage::TransferDst | usage },
                                                "SphericalCube.Vertices", gfxAlloc );
        _indexBuffer    = resMngr.CreateBuffer( BufferDesc{ ArraySizeOf(indices), EBufferUsage::Index | EBufferUsage::TransferDst | usage },
                                                "SphericalCube.Indices", gfxAlloc );
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
    void  SphericalCubeRenderer::Destroy (IResourceManager &resMngr) __NE___
    {
        resMngr.ReleaseResource( _vertexBuffer );
        resMngr.ReleaseResource( _indexBuffer );
    }

/*
=================================================
    GetVertexBuffer
=================================================
*/
    bool  SphericalCubeRenderer::GetVertexBuffer (const uint lod, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint2 &vertCountPerFace) C_NE___
    {
        CHECK_ERR( lod >= _minLod and lod <= _maxLod );

        range.begin = 0_b;
        for (uint i = _minLod; i < lod; ++i) {
            range.begin += SizeOf<Vertex> * CalcVertCount( i );
        }

        vertCountPerFace = CalcFaceVertCount2( lod );

        const Bytes vers_size = SizeOf<Vertex> * vertCountPerFace.x * vertCountPerFace.y * FaceCount;

        range.end   = range.begin + vers_size;
        id          = _vertexBuffer;

        return true;
    }

/*
=================================================
    GetVertexBuffer
=================================================
*/
    bool  SphericalCubeRenderer::GetVertexBuffer (const uint lod, const uint face, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint2 &vertCountPerFace) C_NE___
    {
        CHECK_ERR( lod >= _minLod and lod <= _maxLod );
        CHECK_ERR( face < FaceCount );

        range.begin = 0_b;
        for (uint i = _minLod; i < lod; ++i) {
            range.begin += SizeOf<Vertex> * CalcVertCount( i );
        }

        vertCountPerFace = CalcFaceVertCount2( lod );

        const Bytes face_size = SizeOf<Vertex> * vertCountPerFace.x * vertCountPerFace.y;

        range.begin += face_size * face;
        range.end   = range.begin + face_size;
        id          = _vertexBuffer;

        return true;
    }

/*
=================================================
    GetIndexBuffer
=================================================
*/
    bool  SphericalCubeRenderer::GetIndexBuffer (const uint lod, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint &indexCountPerMesh) C_NE___
    {
        CHECK_ERR( lod >= _minLod and lod <= _maxLod );

        range.begin = 0_b;
        for (uint i = _minLod; i < lod; ++i) {
            range.begin += SizeOf<Index> * CalcIndexCount( i, Bool{_quads} );
        }

        indexCountPerMesh = CalcIndexCount( lod, Bool{_quads} );

        range.end   = range.begin + SizeOf<Index> * indexCountPerMesh;
        id          = _indexBuffer;

        return true;
    }

/*
=================================================
    GetIndexBuffer
=================================================
*/
    bool  SphericalCubeRenderer::GetIndexBuffer (const uint lod, const uint face, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint &indexCountPerFace) C_NE___
    {
        CHECK_ERR( lod >= _minLod and lod <= _maxLod );
        CHECK_ERR( face < FaceCount );

        range.begin = 0_b;
        for (uint i = _minLod; i < lod; ++i) {
            range.begin += SizeOf<Index> * CalcIndexCount( i, Bool{_quads} );
        }

        indexCountPerFace = CalcFaceIndexCount( lod, Bool{_quads} );

        const Bytes face_size = SizeOf<Index> * indexCountPerFace;

        range.begin += face_size * face;
        range.end   = range.begin + face_size;
        id          = _indexBuffer;

        return true;
    }


} // AE::GeometryTools

#endif // AE_GEOMTOOLS_HAS_GRAPHICS
