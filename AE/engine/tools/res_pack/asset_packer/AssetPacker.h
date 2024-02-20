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

    enum class EPathParamsFlags : uint
    {
        Unknown                     = 0,        // auto-detect
        File                        = 1 << 0,
        Folder                      = 1 << 1,
        RecursiveFolder             = 1 << 2,
        _Last,
        All                         = ((_Last - 1) << 1) - 1,
    };

    struct PathParams
    {
        const CharType *    path        = null;
        usize               priority    : 16;
        usize               flags       : 8;    // EPathParamsFlags

        PathParams () : priority{0}, flags{0} {}

        PathParams (const CharType* inPath, usize inPriority = 0, EPathParamsFlags inFlags = EPathParamsFlags::Unknown) :
            path{ inPath },
            priority{ inPriority },
            flags{ usize(inFlags) }
        {}
    };


    struct AssetInfo
    {
        const PathParams *      inFiles                 = null;     // [inFileCount]    // files or folders
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
