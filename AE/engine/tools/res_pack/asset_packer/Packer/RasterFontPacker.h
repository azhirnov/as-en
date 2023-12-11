// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "ImagePacker.h"

namespace AE::AssetPacker
{

    //
    // Raster Font Packer
    //

    class RasterFontPacker final : public Serializing::ISerializable
    {
    // types
    public:

        //         .==========.  <---------- top
        //         ||        ||
        //         ||        ||
        //  *----- ||--------||  <--*------- baseline (y = 0)
        //  ^      ||        ||     ^
        //  |      '=========='  <--|------- bottom
        //  |      ^          ^     |
        //  |      |          |     |
        //  |   left         right  |
        //  |<--------------------->|
        //           advance
        //

        struct Glyph
        {
        // variables
            Rectangle<ushort>   texcoord;           // unorm
            RectF               offset;             // pixels
            float               advance     = 0.0f; // glyph with including empty space

        // methods
            Glyph ()                __NE___ {}

            ND_ bool  HasImage ()   C_NE___ { return true; }    // TODO
        };


        struct SDFConfig
        {
            float           scale       = 0.f;  // value in texture (snorm/unorm) to distance in pixels
            float           bias        = 0.f;  // convert unorm value in texture to snorm
            float           pixRange2D  = 0.f;  // == SDFPixelRange(), screenPxRange = heightInPx * pixRange2D
            packed_float2   pixRange3D;         // TODO
        };


        union GlyphKey
        {
        // variables
        private:
            struct Packed {
                ulong       symbol;
                ulong       height;
            }           _packed;
            ulong       _value;

        // methods
        public:
            GlyphKey ()                                         __NE___ : _value{0} {}
            GlyphKey (CharUtf32 symbol, uint height)            __NE___ : _packed{symbol, height} {}

            ND_ CharUtf32   Symbol ()                           C_NE___ { return CharUtf32(_packed.symbol); }
            ND_ uint        HeightInPix ()                      C_NE___ { return uint(_packed.height); }

            ND_ bool        operator == (const GlyphKey &rhs)   C_NE___ { return _value == rhs._value; }
            ND_ HashVal     CalcHash ()                         C_NE___ { return HashOf(_value); }
        };


        using GlyphMap_t    = FlatHashMap< GlyphKey, Glyph, DefaultHasher_CalcHash<GlyphKey> >;
        using SizeArr_t     = FixedArray< ubyte, 16 >;      // size in pixels which is supported

        static constexpr ushort     Version     = 2;
        static constexpr uint       Magic       = "gr.RFnt"_Hash;

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
        SDFConfig           sdfConfig;

        GlyphMap_t          glyphMap;
        SizeArr_t           fontHeight;


    // methods
    public:
        RasterFontPacker ()                                                         __NE___ {}
        explicit RasterFontPacker (const ImagePacker::Header &h)                    __NE___ : _header{h} {}

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
    template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::Header2 >     { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::SDFConfig >   { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::GlyphKey >    { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::Glyph >       { static constexpr bool  value = true; };
}
