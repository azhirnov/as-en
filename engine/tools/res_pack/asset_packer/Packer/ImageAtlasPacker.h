// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "ImagePacker.h"

namespace AE::AssetPacker
{

    //
    // Image Atlas Packer
    //

    class ImageAtlasPacker final : public Serializing::ISerializable
    {
    // types
    public:
        using ImageData     = ImagePacker::ImageData;
        using Rect_t        = Rectangle<ushort>;
        using ImageMap_t    = FlatHashMap< Graphics::ImageInAtlasName::Optimized_t, uint >; // index in 'ImageRects_t'
        using ImageRects_t  = Array< Rect_t >;

        static constexpr ushort     Version     = 1;
        static constexpr uint       Magic       = uint("gr.StAtlas"_StringToID);


    // variables
    public:
        uint                    magic       = Magic;
        ushort                  version     = Version;
        ImagePacker::Header     header;

        ImageMap_t              map;
        ImageRects_t            rects;


    // methods
    public:

        #ifdef AE_BUILD_ASSET_PACKER
            bool  SaveImage (WStream &stream, const ImageMemView &src);
        #endif

        ND_ bool  IsValid () const;

            bool  ReadImage (RStream &stream, INOUT ImageData &) const;

        // ISerializable
        bool  Serialize (Serializing::Serializer &)     C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &) __NE_OV;
    };


} // AE::AssetPacker
