// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "geometry_tools/SphericalCube/SphericalCubeMath.h"

namespace AE::GeometryTools
{

    //
    // Spherical Cube Generator
    //

    class SphericalCubeGen : public SphericalCubeProjection< TangentialSphericalCube, TextureProjection >
    {
    // types
    public:
        struct Vertex
        {
            packed_short4   position;
            packed_short4   texcoord;
            packed_short4   tangent;
            packed_short4   bitangent;
        };
        STATIC_ASSERT( sizeof(Vertex) == 32 );

        struct UnpackedVertex
        {
            float3      position;   // in normalized coords
            float3      texcoord;
            //float3    projection; // TODO: use it for distortion correction and for tessellation (instead of linear interpolation)
            float3      tangent;
            float3      bitangent;

            UnpackedVertex () = default;
            UnpackedVertex (const Vertex &v);
        };

        using Index = uint;     // TODO: ushort ?


    // variables
    protected:
        Array<Vertex>       _vertices;
        Array<Index>        _indices;

        uint                _minLod         = 0;
        uint                _maxLod         = 0;
        bool                _quads          = false;    // quads for tessellation


    // methods
    public:
        ND_ bool  Create (uint minLod, uint maxLod, bool quads)                             __NE___;

            bool  GetVertices (uint lod, uint face, OUT ArrayView<Vertex> &result)          C_NE___;
            bool  GetIndices (uint lod, uint face, OUT ArrayView<Index> &result)            C_NE___;

        ND_ static uint  CalcFaceVertCount (uint lod)                                       __NE___;
        ND_ static uint  CalcVertCount (uint lod)                                           __NE___;

        ND_ static uint  CalcFaceIndexCount (uint lod, bool useQuads)                       __NE___;
        ND_ static uint  CalcIndexCount (uint lod, bool useQuads)                           __NE___;

        ND_ static bool  RayCast (const float3 &center, float radius, const float3 &begin,
                                  const float3 &end, OUT float3 &outIntersection)           __NE___;

        ND_ ArrayView<Vertex>   GetVertices ()                                              C_NE___ { return _vertices; }
        ND_ ArrayView<Index>    GetIndices ()                                               C_NE___ { return _indices; }
        ND_ uint2               LODRange ()                                                 C_NE___ { return uint2{ _minLod, _maxLod }; }
        ND_ bool                IsQuads ()                                                  C_NE___ { return _quads; }
    };


} // AE::GeometryTools
