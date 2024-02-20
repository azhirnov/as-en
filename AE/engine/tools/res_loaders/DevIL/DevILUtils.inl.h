// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"
#include "IL/il.h"
#include "IL/ilu.h"

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{

    //
    // DevIL Utils
    //

    struct DevILUtils
    {
        RecursiveMutex              guard;

        decltype(&ilActiveFace)     ilActiveFace        = null;
        decltype(&ilActiveImage)    ilActiveImage       = null;
        decltype(&ilActiveLayer)    ilActiveLayer       = null;
        decltype(&ilActiveMipmap)   ilActiveMipmap      = null;
        decltype(&ilConvertImage)   ilConvertImage      = null;
        decltype(&ilDetermineType)  ilDetermineType     = null;
        decltype(&ilDetermineTypeL) ilDetermineTypeL    = null;
        decltype(&ilEnable)         ilEnable            = null;
        decltype(&ilGetData)        ilGetData           = null;
        decltype(&ilGetDXTCData)    ilGetDXTCData       = null;
        decltype(&ilGetInteger)     ilGetInteger        = null;
        decltype(&ilInit)           ilInit              = null;
        decltype(&ilLoadL)          ilLoadL             = null;
        decltype(&ilSetInteger)     ilSetInteger        = null;
        decltype(&ilGenImage)       ilGenImage          = null;
        decltype(&ilTexImage)       ilTexImage          = null;
        decltype(&ilBindImage)      ilBindImage         = null;
        decltype(&ilSaveL)          ilSaveL             = null;
        decltype(&ilDeleteImage)    ilDeleteImage       = null;

        decltype(&iluFlipImage)     iluFlipImage        = null;
        decltype(&iluGetInteger)    iluGetInteger       = null;
        decltype(&iluInit)          iluInit             = null;

        ND_ static DevILUtils&  Instance ();

        ND_ bool  IsInitialized ()          { EXLOCK( guard );  return bool(_ilLib) and bool(_iluLib); }

    private:
        Library     _ilLib;
        Library     _iluLib;

        DevILUtils ()                       { _Init(); }

        bool  _Init ();
    };


/*
=================================================
    ILEnumCast
=================================================
*/
    ND_ inline ILenum  ILEnumCast (EImageFormat fileFormat)
    {
        switch_enum( fileFormat )
        {
            case EImageFormat::BMP :        return IL_BMP;
            case EImageFormat::JPG :        return IL_JPG;
            case EImageFormat::PCX :        return IL_PCX;
            case EImageFormat::PNG :        return IL_PNG;
            case EImageFormat::TGA :        return IL_TGA;
            case EImageFormat::TIF :        return IL_TIF;
            case EImageFormat::PSD :        return IL_PSD;
            case EImageFormat::RadianceHDR: return IL_HDR;
            case EImageFormat::OpenEXR :    return IL_EXR;
            case EImageFormat::KTX :        return IL_KTX;

            case EImageFormat::DDS :        RETURN_ERR( "use DDSLoader/DDSSaver instead", IL_DDS );
            case EImageFormat::AEImg :
            case EImageFormat::Unknown :
            case EImageFormat::_Count :     return IL_TYPE_UNKNOWN;
        }
        switch_end
        RETURN_ERR( "unsupported image file format", IL_TYPE_UNKNOWN );
    }


} // AE::ResLoader
