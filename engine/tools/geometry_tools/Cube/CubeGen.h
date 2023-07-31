// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "geometry_tools/GeometryTools.pch.h"

namespace AE::GeometryTools
{
    using namespace AE::Base;


    //
    // Cube Generator
    //

    class CubeGen final
    {
    // types
    public:
        struct Vertex
        {
            packed_short4   position;
            packed_short4   texcoord;   // for CubeMap, use xy for 2D
            packed_short4   normal;
            packed_short4   tangent;
            packed_short4   bitangent;
        };
        STATIC_ASSERT( sizeof(Vertex) == 40 );

        struct UnpackedVertex
        {
            packed_float3   position;
            packed_float3   normal;
            packed_float3   texcoord;   // for CubeMap, use xy for 2D
            packed_float3   tangent;
            packed_float3   bitangent;

            UnpackedVertex () = default;
            UnpackedVertex (const Vertex &);
        };

        using Index = ushort;


    // variables
    private:
        Array<Vertex>       _vertices;
        Array<Index>        _indices;


    // methods
    public:
        ND_ bool  Create ()                     __NE___;

        ND_ ArrayView<Vertex>   GetVertices ()  C_NE___ { return _vertices; }
        ND_ ArrayView<Index>    GetIndices ()   C_NE___ { return _indices; }
    };


} // AE::GeometryTools
