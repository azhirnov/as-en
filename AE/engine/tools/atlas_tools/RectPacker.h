// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "AtlasTools.pch.h"

namespace AE::AtlasTools
{
    using namespace AE::Base;


    //
    // Rect Packer
    //

    class RectPacker
    {
    // types
    public:
        struct Region
        {
            uint    id          = UMax;
            uint    reserved    = 0;

            // input
            uint2   size;

            // output
            uint2   pos;
        };


    // variables
    private:
        Array< Region >     _rects;
        ulong               _maxArea    = 0;
        uint2               _maxSize    {0};
        uint2               _targetSize {0};


    // methods
    public:
        RectPacker () {}

            void  Add (const uint2 &size);
            void  Add (const uint2 &size, uint id);

        ND_ bool  Pack ();
            void  Reset ();

        ND_ uint2  TargetSize ()    const   { return _targetSize; }
        ND_ float  PackingRate ()   const;

        ND_ ArrayView<Region>  GetResult () const { return _rects; }
    };


/*
=================================================
    Add
=================================================
*/
    inline void  RectPacker::Add (const uint2 &size)
    {
        return Add( size, uint(_rects.size()) );
    }

    inline void  RectPacker::Add (const uint2 &size, uint id)
    {
        auto&   dst = _rects.emplace_back();
        dst.id      = id;
        dst.size    = size;

        _maxArea    += Area( size );
        _maxSize    = Max( _maxSize, size );
    }

/*
=================================================
    PackingRate
=================================================
*/
    inline float  RectPacker::PackingRate () const
    {
        return float(_maxArea) / float(_targetSize.x * _targetSize.y);
    }


} // AE::AtlasTools
