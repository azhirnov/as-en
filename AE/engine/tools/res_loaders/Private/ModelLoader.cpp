// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{

/*
=================================================
    PathToModelFormat
=================================================
*/
    EModelFormat  PathToModelFormat (const Path &) __NE___
    {
        // TODO
        return Default;
    }

/*
=================================================
    ModelFormatToExt
=================================================
*/
    StringView  ModelFormatToExt (EModelFormat fmt) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( fmt )
        {
            case EModelFormat::GLTF :       return ".gltf";
            case EModelFormat::Unknown :    break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown model format" );
    }


} // AE::ResLoader
