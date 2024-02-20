// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ImageLayer.h"

namespace AE::Graphics
{
struct ImageUtils final : Noninstanceable
{
    using Pixels    = uint;
    using Pixels2   = uint2;


/*
=================================================
    RowSize
=================================================
*/
    ND_ static Bytes  RowSize (const Pixels width, const uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        ASSERT( IsMultipleOf( width, texelBlock.x ));

        return ((Bytes{width + texelBlock.x-1} / texelBlock.x) * bitsPerBlock) / 8;
    }

/*
=================================================
    RowLength
----
    Vulkan: bufferRowLength
=================================================
*/
    ND_ static Pixels  RowLength (const Bytes rowPitch, const uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        return Pixels((rowPitch*8) / bitsPerBlock) * texelBlock.x;
    }

/*
=================================================
    SliceSize
----
    for 2D slice
=================================================
*/
    ND_ static Bytes  SliceSize (const Pixels2 &dim, const uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        Bytes   row_size = RowSize( dim.x, bitsPerBlock, texelBlock );
        return SliceSize( dim.y, row_size, texelBlock );
    }

    ND_ static Bytes  SliceSize (uint height, Bytes rowSize, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        ASSERT( IsMultipleOf( height, texelBlock.y ));
        return (rowSize * height + texelBlock.y-1) / texelBlock.y;
    }

/*
=================================================
    ImageHeight
----
    Vulkan: bufferImageHeight
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
----
    for 3D slice
=================================================
*/
    ND_ static Bytes  ImageSize (const uint3 &dim, uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        return SliceSize( Pixels2{dim.x, dim.y}, bitsPerBlock, texelBlock ) * dim.z;
    }

    ND_ static Bytes  ImageSize (const uint3 &dim, ImageLayer layer, uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        return ImageSize( dim, bitsPerBlock, texelBlock ) * layer.Get();
    }

/*
=================================================
    ImageOffset
=================================================
*/
    ND_ static Bytes  ImageOffset (const uint3 &offset, const Bytes rowSize, const Bytes sliceSize, const uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( texelBlock > 0u ));
        ASSERT( All( IsMultipleOf( uint2{offset}, texelBlock )));

        Bytes   data_offset;
        data_offset += RowSize( offset.x, bitsPerBlock, texelBlock );
        data_offset += SliceSize( offset.y, rowSize, texelBlock );
        data_offset += sliceSize * offset.z;
        return data_offset;
    }

    ND_ static Bytes  ImageOffset (const uint3 &offset, const uint3 &dim, uint bitsPerBlock, const uint2 &texelBlock) __NE___
    {
        ASSERT( All( offset < dim ));
        Bytes   row_size    = RowSize( dim.x, bitsPerBlock, texelBlock );
        Bytes   slice_size  = SliceSize( dim.y, row_size, texelBlock );
        return ImageOffset( offset, row_size, slice_size, bitsPerBlock, texelBlock );
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
    MipmapDimension
=================================================
*/
    ND_ static uint3  MipmapDimension (const uint3 &dim, const usize mipLevel, const uint2 &texelBlock) __NE___
    {
        return Max( dim >> mipLevel, uint3{texelBlock, 1} );
    }


}; // ImageUtils
}  // AE::Graphics
