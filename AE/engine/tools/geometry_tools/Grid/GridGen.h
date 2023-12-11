// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "geometry_tools/GeometryTools.pch.h"

namespace AE::GeometryTools
{
    using namespace AE::Base;


    //
    // Grid Generator
    //

    class GridGen final
    {
    // types
    public:
        struct Vertex
        {
            float2  uv;     // TODO: unorm
        };
        StaticAssert( sizeof(Vertex) == 8 );

        using Index = ushort;


    // variables
    private:
        Array<Vertex>   _vertices;
        Array<Index>    _indices;


    // methods
    public:
        ND_ bool  Create (uint vertsPerEdge, uint patchSize = 3)    __NE___;

        ND_ ArrayView<Vertex>   GetVertices ()                      C_NE___ { return _vertices; }
        ND_ ArrayView<Index>    GetIndices ()                       C_NE___ { return _indices; }
    };


} // AE::GeometryTools
