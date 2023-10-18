// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ImagePacker.h"
#include "graphics/Private/EnumUtils.h"

#ifdef AE_BUILD_ASSET_PACKER
# include "res_loaders/Intermediate/IntermImage.h"
#endif

namespace AE::AssetPacker
{
namespace {
#   include "ImagePackerUtils.cpp.h"
}

/*
=================================================
    GetOffset
=================================================
*/
    void  ImagePacker::GetOffset (ImageLayer layer, MipmapLevel mipmap, OUT uint3 &imageDim, OUT Bytes &offset,
                                  OUT Bytes &size, OUT Bytes &rowSize, OUT Bytes &sliceSize) C_NE___
    {
        ImagePacker_GetOffset( _header.hdr, layer, mipmap, uint3{0},
                               OUT imageDim, OUT offset, OUT size, OUT rowSize, OUT sliceSize );
    }

/*
=================================================
    MaxSliceSize
=================================================
*/
    Bytes  ImagePacker::MaxSliceSize () C_NE___
    {
        return ImagePacker_MaxSliceSize( _header.hdr );
    }

/*
=================================================
    ReadHeader
=================================================
*/
    bool  ImagePacker::ReadHeader (RStream &stream) __NE___
    {
        return ImagePacker_ReadHeader( stream, OUT _header );
    }

/*
=================================================
    SaveHeader
=================================================
*/
    bool  ImagePacker::SaveHeader (WStream &stream) C_NE___
    {
        return ImagePacker_SaveHeader( stream, _header );
    }

/*
=================================================
    SaveImage
=================================================
*/
    bool  ImagePacker::SaveImage (WStream &stream, const ResLoader::IntermImage &src) C_NE___
    {
        return ImagePacker_SaveImage( stream, _header.hdr, src );
    }

/*
=================================================
    IsValid
=================================================
*/
    bool  ImagePacker::IsValid () C_NE___
    {
        return ImagePacker_IsValid( _header.hdr );
    }

/*
=================================================
    Header::ToDesc
=================================================
*/
    ImageDesc  ImagePacker::Header::ToDesc () C_NE___
    {
        EImageOpt   options = Default;
        switch ( viewType ) {
            case EImage::Cube :
            case EImage::CubeArray :    options |= EImageOpt::CubeCompatible;   break;
        }
        return ImageDesc{}
            .SetDimension( uint3{dimension} )
            .SetArrayLayers( arrayLayers )
            .SetMaxMipmaps( mipmaps )
            .SetType( viewType )
            .SetFormat( format )
            .SetOptions( options );
    }

/*
=================================================
    Header::ToViewDesc
=================================================
*/
    ImageViewDesc  ImagePacker::Header::ToViewDesc () C_NE___
    {
        return ImageViewDesc{ viewType };
    }


} // AE::AssetPacker
