// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermMesh.h"

namespace AE::ResLoader
{
    using namespace AE::Graphics;

/*
=================================================
    constructor
=================================================
*/
    IntermMesh::IntermMesh (Array<ubyte> vertices, RC<IntermVertexAttribs> attribs,
                            Bytes vertStride, EPrimitive topology,
                            Array<ubyte> indices, EIndex indexType) __NE___ :
        _vertices{ RVRef(vertices) },   _attribs{ RVRef(attribs) },
        _vertexStride{ vertStride },    _topology{ topology },
        _indices{ RVRef(indices) },     _indexType{ indexType }
    {}

/*
=================================================
    CalcAABB
=================================================
*/
    void  IntermMesh::CalcAABB () __NE___
    {
        CHECK_ERRV( _attribs and _vertexStride > 0 and _vertices.size() );

        _boundingBox = AABB{};

        auto    positions = _attribs->GetData<float3>( VertexAttributeName::Position, _vertices.data(),
                                                       VertexCount(), _vertexStride );
        if ( positions.empty() )
            return;

        AABB    bbox{ positions[0] };

        for (size_t i = 1; i < positions.size(); ++i)
        {
            bbox.Add( positions[i] );
        }

        _boundingBox = bbox;
    }

/*
=================================================
    IndexStride
=================================================
*/
    Bytes  IntermMesh::IndexStride () C_NE___
    {
        return EIndex_SizeOf( _indexType );
    }


} // AE::ResLoader
