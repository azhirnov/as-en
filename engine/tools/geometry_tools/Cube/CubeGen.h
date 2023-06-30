// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Math/Vec.h"

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
            packed_float3   position;
            packed_float3   normal;
            packed_float3   texcoord;   // for CubeMap, use xy for 2D
        };
        STATIC_ASSERT( sizeof(Vertex) == 36 );

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
