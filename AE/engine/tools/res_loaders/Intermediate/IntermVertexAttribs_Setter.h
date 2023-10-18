// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Intermediate/IntermVertexAttribs.h"

namespace AE::ResLoader
{

    //
    // Intermediate Vertex Attributes Setter
    //

    class IntermVertexAttribs::Setter
    {
    // variables
    private:
        IntermVertexAttribs &   _self;


    // methods
    public:
        explicit Setter (IntermVertexAttribs &self) : _self{self} {}

        Setter&  AddBuffer (Bytes stride, EVertexInputRate rate = EVertexInputRate::Vertex);
        Setter&  AddBuffer (const Name_t &bufferId, Bytes stride, EVertexInputRate rate = EVertexInputRate::Vertex);

        // add vertex to last buffer
        Setter&  AddVertex (const Name_t &id, EVertexType type, Bytes offset);
    };


/*
=================================================
    AddBuffer
=================================================
*/
    inline IntermVertexAttribs::Setter&  IntermVertexAttribs::Setter::AddBuffer (Bytes stride, EVertexInputRate rate)
    {
        return AddBuffer( Name_t{}, stride, rate );
    }

    inline IntermVertexAttribs::Setter&  IntermVertexAttribs::Setter::AddBuffer (const Name_t &bufferId, Bytes stride, EVertexInputRate rate)
    {
        ASSERT( not _self._bindings.contains( bufferId ));

        auto&   dst = _self._bindings( bufferId );
        dst.index   = BufferIndex(_self._bindings.size()-1);
        dst.stride  = stride;
        dst.rate    = rate;

        return *this;
    }

/*
=================================================
    AddVertex
=================================================
*/
    inline IntermVertexAttribs::Setter&  IntermVertexAttribs::Setter::AddVertex (const Name_t &id, EVertexType type, Bytes offset)
    {
        ASSERT( not _self._vertices.contains( id ));

        auto&   dst = _self._vertices( id );
        dst.type            = type;
        dst.index           = uint(_self._vertices.size()-1);
        dst.offset          = offset;
        dst.bufferBinding   = BufferIndex(_self._bindings.size()-1);

        return *this;
    }


} // AE::ResLoader
