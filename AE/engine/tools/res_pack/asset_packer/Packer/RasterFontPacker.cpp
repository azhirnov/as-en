// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "RasterFontPacker.h"

namespace AE::AssetPacker
{

/*
=================================================
    SaveImage
=================================================
*/
    bool  RasterFontPacker::SaveImage (WStream &stream, const ResLoader::IntermImage &src) C_NE___
    {
    #ifdef AE_BUILD_ASSET_PACKER
        return ImagePacker{_header.hdr}.SaveImage( stream, src );
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
    bool  RasterFontPacker::IsValid () C_NE___
    {
        CHECK_ERR( ImagePacker{_header.hdr}.IsValid() );

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
    Serialize
=================================================
*/
    bool  RasterFontPacker::Serialize (Serializing::Serializer &ser) C_NE___
    {
        ASSERT( IsValid() );
        return ser( _header, sdfConfig, glyphMap, fontHeight );
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  RasterFontPacker::Deserialize (Serializing::Deserializer &des) __NE___
    {
        bool    res = des( OUT _header );
        res &= (_header.magic == Magic);
        res &= (_header.version == Version);

        if_unlikely( not res )
            return false;

        res = des( OUT sdfConfig, OUT glyphMap, OUT fontHeight );
        ASSERT( IsValid() );

        return res;
    }

} // AE::AssetPacker
