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
            packed_short4   texcoord;   // 2D or CubeMap
            packed_short4   normal;
            packed_short4   tangent;
            packed_short4   bitangent;
        };
        StaticAssert( sizeof(Vertex) == 40 );

        struct UnpackedVertex
        {
            packed_float3   position;
            packed_float3   normal;
            packed_float3   texcoord;
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

        // if 'cubeMap' is true then xyz texcoords will be generated for cubemap texture.
        // if 'cubeMap' is false then xy texcoords will be generated for 2D texture.
        ND_ bool  Create (Bool cubeMap)         __NE___;

        ND_ ArrayView<Vertex>   GetVertices ()  C_NE___ { return _vertices; }
        ND_ ArrayView<Index>    GetIndices ()   C_NE___ { return _indices; }
    };


} // AE::GeometryTools
