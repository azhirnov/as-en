// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "RasterFontPacker.h"

namespace AE::AssetPacker
{

/*
=================================================
    SaveImage
=================================================
*/
    bool  RasterFontPacker::SaveImage (WStream &stream, const ImageMemView &src)
    {
    #ifdef AE_BUILD_ASSET_PACKER

        ImagePacker     img_packer;
        img_packer.header = header;

        return img_packer.SaveImage( stream, src );

    #else
        Unused( stream, src );
        return false;
    #endif
    }

/*
=================================================
    IsValid
=================================================
*/
    bool  RasterFontPacker::IsValid () const
    {
        ImagePacker     img_packer;
        img_packer.header = header;
        CHECK_ERR( img_packer.IsValid() );

        CHECK_ERR( not glyphMap.empty() );
        CHECK_ERR( not fontHeight.empty() );

        FlatHashSet< CharUtf32 >    unique_chars;
        for (auto& [key, glyph] : glyphMap)
        {
            unique_chars.insert( key.Symbol() );
        }

        for (auto h : fontHeight)
        {
            for (auto c : unique_chars)
            {
                CHECK_ERR( glyphMap.contains( GlyphKey{ c, h }));
            }
        }

        return true;
    }

/*
=================================================
    ReadImage
=================================================
*/
    bool  RasterFontPacker::ReadImage (RStream &stream, INOUT ImageData &result) const
    {
        ImagePacker     img_packer;
        img_packer.header = header;
        return img_packer.ReadImage( stream, INOUT result );
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  RasterFontPacker::Serialize (Serializing::Serializer &ser) C_NE___
    {
        ASSERT( IsValid() );
        return ser( Magic, version, header, sdfConfig, glyphMap, fontHeight );
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  RasterFontPacker::Deserialize (Serializing::Deserializer &des) __NE___
    {
        bool    res = des( OUT magic, OUT version );
        if ( not res or (magic != Magic) or (version != Version) )
            return false;

        res = des( OUT header, OUT sdfConfig, OUT glyphMap, OUT fontHeight );
        ASSERT( IsValid() );
        return res;
    }

} // AE::AssetPacker
