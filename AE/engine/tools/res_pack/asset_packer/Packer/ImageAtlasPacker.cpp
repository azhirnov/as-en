// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ImageAtlasPacker.h"

namespace AE::AssetPacker
{

/*
=================================================
    IsValid
=================================================
*/
    bool  ImageAtlasPacker::IsValid () const
    {
        ImagePacker     img_packer;
        img_packer.header = header;
        CHECK_ERR( img_packer.IsValid() );

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
        return ser( Magic, version, header, map, rects );
    }

    bool  ImageAtlasPacker::Deserialize (Serializing::Deserializer &des) __NE___
    {
        bool    res = des( OUT magic, OUT version );
        if ( not res or (magic != Magic) or (version != Version) )
            return false;

        res = des( OUT header, OUT map, OUT rects );
        ASSERT( IsValid() );
        return res;
    }

/*
=================================================
    ReadImage
=================================================
*/
    bool  ImageAtlasPacker::ReadImage (RStream &stream, INOUT ImageData &result) const
    {
        ImagePacker     img_packer;
        img_packer.header = header;
        return img_packer.ReadImage( stream, INOUT result );
    }

/*
=================================================
    SaveImage
=================================================
*/
    bool  ImageAtlasPacker::SaveImage (WStream &stream, const ImageMemView &src)
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


} // AE::AssetPacker
