// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/Vec.h"
#include "graphics/Public/Common.h"

namespace AE::Graphics
{
struct ImageUtils final : Noninstanceable
{
    using Pixels    = uint;
    using Pixels2   = uint2;


/*
=================================================
    BytesPerBlock
=================================================
*/
    ND_ static Bytes  BytesPerBlock (uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        return (Bytes{bitsPerBlock} * texelBlock.x * texelBlock.y) / 8;
    }

/*
=================================================
    RowSize
=================================================
*/
    ND_ static Bytes  RowSize (Pixels width, uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        ASSERT( width % texelBlock.x == 0 );

        return ((Bytes{width + texelBlock.x-1} / texelBlock.x) * bitsPerBlock) / 8;
    }

/*
=================================================
    RowLength
----
    vk: bufferRowLength
=================================================
*/
    ND_ static Pixels  RowLength (Bytes rowPitch, uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        return Pixels((rowPitch*8) / bitsPerBlock) * texelBlock.x;
    }

/*
=================================================
    SliceSize
=================================================
*/
    ND_ static Bytes  SliceSize (const Pixels2 &dim, uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        Bytes   row_size = RowSize( dim.x, bitsPerBlock, texelBlock );
        return SliceSize( dim.y, row_size, texelBlock );
    }

    ND_ static Bytes  SliceSize (uint height, Bytes rowSize, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        ASSERT( height % texelBlock.y == 0 );
        return (rowSize * height + texelBlock.y-1) / texelBlock.y;
    }

/*
=================================================
    ImageHeight
----
    vk: bufferImageHeight
=================================================
*/
    ND_ static Pixels  ImageHeight (Bytes slicePitch, Bytes rowPitch, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        return Pixels(slicePitch / rowPitch) * texelBlock.y;
    }

/*
=================================================
    ImageSize
=================================================
*/
    ND_ static Bytes  ImageSize (const uint3 &dim, uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        return SliceSize( Pixels2{dim.x, dim.y}, bitsPerBlock, texelBlock ) * dim.z;
    }

/*
=================================================
    TexelBlocks
=================================================
*/
    ND_ static uint2  TexelBlocks (const uint2 &dim, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        return (dim + (texelBlock - 1u)) / texelBlock;
    }

/*
=================================================
    NumberOfMipmaps
=================================================
*/
    ND_ static uint  NumberOfMipmaps (const uint3 &dim) __NE___
    {
        return IntLog2( Max( Max( dim.x, dim.y ), dim.z )) + 1;
    }

/*
=================================================
    MipmapSize
=================================================
*/
    ND_ static uint3  MipmapSize (const uint3 &dim, usize mipLevel, const uint2 &texelBlock) __NE___
    {
        return Max( Max( dim >> mipLevel, 1u ), uint3{texelBlock, 1} );
    }


}; // ImageUtils
}  // AE::Graphics
