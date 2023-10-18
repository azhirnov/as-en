// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ImageAtlasPacker.h"

namespace AE::AssetPacker
{

/*
=================================================
    IsValid
=================================================
*/
    bool  ImageAtlasPacker::IsValid () C_NE___
    {
        CHECK_ERR( ImagePacker{_header.hdr}.IsValid() );

        CHECK_ERR( not map.empty() );
        CHECK_ERR( not rects.empty() );

        for (auto& [name, idx] : map)
        {
            CHECK_ERR( name.IsDefined() );
            CHECK_ERR( idx < rects.size() );
        }

        return true;
    }

/*
=================================================
    Serialize / Deserialize
=================================================
*/
    bool  ImageAtlasPacker::Serialize (Serializing::Serializer &ser) C_NE___
    {
        ASSERT( IsValid() );
        return ser( _header, map, rects );
    }

    bool  ImageAtlasPacker::Deserialize (Serializing::Deserializer &des) __NE___
    {
        bool    res = des( OUT _header );
        res &= (_header.magic == Magic);
        res &= (_header.version == Version);

        if_unlikely( not res )
            return false;

        res = des( OUT map, OUT rects );
        ASSERT( IsValid() );

        return res;
    }

/*
=================================================
    SaveImage
=================================================
*/
    bool  ImageAtlasPacker::SaveImage (WStream &stream, const ResLoader::IntermImage &src) C_NE___
    {
    #ifdef AE_BUILD_ASSET_PACKER
        return ImagePacker{_header.hdr}.SaveImage( stream, src );
    #else
        Unused( stream, src );
        return false;
    #endif
    }


} // AE::AssetPacker
