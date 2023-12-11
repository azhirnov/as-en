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
        using Rect_t        = Rectangle<ushort>;
        using ImageMap_t    = FlatHashMap< Graphics::ImageInAtlasName::Optimized_t, uint >; // index in 'ImageRects_t'
        using ImageRects_t  = Array< Rect_t >;

        static constexpr ushort     Version     = 1;
        static constexpr uint       Magic       = "gr.StAtlas"_Hash;

        struct Header2
        {
            uint                magic       = Magic;
            ushort              version     = Version;
            ImagePacker::Header hdr;

            Header2 ()                                      __NE___ = default;
            explicit Header2 (const ImagePacker::Header &h) __NE___ : hdr{h} {}
        };
        StaticAssert( sizeof(Header2) == 24 );


    // variables
    private:
        Header2             _header;
    public:
        ImageMap_t          map;
        ImageRects_t        rects;


    // methods
    public:
        ImageAtlasPacker ()                                                         __NE___ {}
        explicit ImageAtlasPacker (const ImagePacker::Header &h)                    __NE___ : _header{h} {}

        ND_ bool  IsValid ()                                                        C_NE___;

            bool  SaveImage (WStream &stream, const ResLoader::IntermImage &src)    C_NE___;

        ND_ ImagePacker::Header const&  Header ()                                   C_NE___ { return _header.hdr; }


        // ISerializable
            bool  Serialize (Serializing::Serializer &)                             C_NE_OV;
            bool  Deserialize (Serializing::Deserializer &)                         __NE_OV;
    };


} // AE::AssetPacker

namespace AE::Base
{
    template <> struct TTriviallySerializable< AE::AssetPacker::ImageAtlasPacker::Header2 > { static constexpr bool  value = true; };
}
