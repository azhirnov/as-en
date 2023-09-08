// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_GEOMTOOLS_HAS_GRAPHICS
# include "geometry_tools/SphericalCube/SphericalCubeGen.h"

namespace AE::GeometryTools
{
    using namespace AE::Graphics;


    //
    // Spherical Cube Renderer
    //

    class SphericalCubeRenderer final : public SphericalCubeProjection< TangentialSphericalCube, TextureProjection >
    {
    // types
    public:
        using Vertex    = SphericalCubeGen::Vertex;
        using Index     = SphericalCubeGen::Index;
    private:
        using Base_t    = SphericalCubeGen;

        static constexpr uint   FaceCount = 6;


    // variables
    private:
        Strong<BufferID>    _vertexBuffer;
        Strong<BufferID>    _indexBuffer;

        uint                _minLod         = 0;
        uint                _maxLod         = 0;
        bool                _quads          = false;


    // methods
    public:
        SphericalCubeRenderer ()                                                                                            __NE___ {}
        ~SphericalCubeRenderer ()                                                                                           __NE___ {}

        ND_ bool  Create (IResourceManager &, ITransferContext &ctx, uint minLod, uint maxLod,
                          Bool quads, Bool cubeMap, EBufferUsage usage, GfxMemAllocatorPtr gfxAlloc)                        __NE___;
            void  Destroy (IResourceManager &)                                                                              __NE___;

        ND_ bool        IsCreated ()                                                                                        C_NE___ { return _vertexBuffer and _indexBuffer; }

        ND_ uint2       LODRange ()                                                                                         C_NE___ { return uint2{ _minLod, _maxLod }; }
        ND_ bool        IsQuads ()                                                                                          C_NE___ { return _quads; }

        ND_ BufferID    VertexBufferId ()                                                                                   C_NE___ { return _vertexBuffer; }
        ND_ BufferID    IndexBufferId ()                                                                                    C_NE___ { return _indexBuffer; }

            template <typename DrawCtx>
            bool  Draw (DrawCtx &ctx, uint lod)                                                                             C_NE___;

            template <typename DrawCtx>
            bool  DrawMeshes (DrawCtx &ctx, uint lod)                                                                       C_NE___;

        ND_ bool  GetVertexBuffer (uint lod, uint face, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint2 &vertCount)    C_NE___;
        ND_ bool  GetIndexBuffer (uint lod, uint face, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint &indexCount)     C_NE___;

        ND_ bool  GetVertexBuffer (uint lod, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint2 &vertCount)               C_NE___;
        ND_ bool  GetIndexBuffer (uint lod, OUT BufferID &id, OUT Range<Bytes> &range, OUT uint &indexCount)                C_NE___;

        ND_ static uint2 CalcFaceVertCount2 (uint lod)                                                                      __NE___ { return Base_t::CalcFaceVertCount2( lod ); }
        ND_ static uint  CalcFaceVertCount (uint lod)                                                                       __NE___ { return Base_t::CalcFaceVertCount( lod ); }
        ND_ static uint  CalcVertCount (uint lod)                                                                           __NE___ { return Base_t::CalcVertCount( lod ); }

        ND_ static uint  CalcFaceIndexCount (uint lod, Bool useQuads)                                                       __NE___ { return Base_t::CalcFaceIndexCount( lod, useQuads ); }
        ND_ static uint  CalcIndexCount (uint lod, Bool useQuads)                                                           __NE___ { return Base_t::CalcIndexCount( lod, useQuads ); }

        ND_ static bool  RayCast (const float3 &center, float radius, const float3 &begin,
                                  const float3 &end, OUT float3 &outIntersection)                                           __NE___ { return Base_t::RayCast( center, radius, begin, end, OUT outIntersection ); }
    };


/*
=================================================
    Draw
=================================================
*/
    template <typename DrawCtx>
    bool  SphericalCubeRenderer::Draw (DrawCtx &ctx, uint lod) C_NE___
    {
        CHECK_ERR( lod >= _minLod and lod <= _maxLod );

        BufferID        vb, ib;
        Range<Bytes>    vb_range, ib_range;
        uint2           vert_cnt;
        uint            idx_cnt;

        CHECK_ERR( GetVertexBuffer( lod, OUT vb, OUT vb_range, OUT vert_cnt ));
        CHECK_ERR( GetIndexBuffer( lod, OUT ib, OUT ib_range, OUT idx_cnt ));

        ctx.BindVertexBuffer( 0, vb, vb_range.Offset() );
        ctx.BindIndexBuffer( ib, ib_range.Offset(), EIndex::UInt );
        STATIC_ASSERT( sizeof(Index) == sizeof(uint) );

        DrawIndexedCmd  cmd;
        cmd.indexCount      = idx_cnt;
        cmd.instanceCount   = 1;
        cmd.firstIndex      = 0;
        cmd.vertexOffset    = 0;
        cmd.firstInstance   = 0;

        ctx.DrawIndexed( cmd );
        return true;
    }

/*
=================================================
    DrawMeshes
=================================================
*/
    template <typename DrawCtx>
    bool  SphericalCubeRenderer::DrawMeshes (DrawCtx &ctx, uint lod) C_NE___
    {
        CHECK_ERR( lod >= _minLod and lod <= _maxLod );

    }


} // AE::GeometryTools

#endif // AE_GEOMTOOLS_HAS_GRAPHICS
