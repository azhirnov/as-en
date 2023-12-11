// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

#ifdef AE_BUILD_ASSET_PACKER
#   define AE_AP_API    AE_DLL_EXPORT
#else
#   define AE_AP_API    AE_DLL_IMPORT
#endif


namespace AE::AssetPacker
{
    using AE::uint;
    using AE::usize;
    using AE::CharType;


    struct AssetInfo
    {
        const CharType * const* inFiles                 = null;     // [inFileCount]
        usize                   inFileCount             = 0;

        // to use #include in scripts from 'inFiles'
        const CharType * const* inIncludeFolders        = null;     // [inIncludeFolderCount]
        usize                   inIncludeFolderCount    = 0;

        const CharType *        tempFile                = null;
        const CharType *        outputArchive           = null;
        const CharType *        outputScriptFile        = null;
    };


    extern "C" bool AE_AP_API PackAssets (const AssetInfo* info);


} // AE::AssetPacker
