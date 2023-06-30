// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    file format:

        uint    hash                (InputActions_Name)
        uint    count

        uint    hash    [count]     (InputActionsAndroid_Name, InputActionsGLFW_Name, InputActionsWinAPI_Name, ...)
        uint    offset  [count]     offsets to data for deserialization, use 'SerializableInputActions'.
*/

#pragma once

#include "base/Common.h"

#ifdef AE_BUILD_INPUT_ACTIONS_BINDING
#   define AE_IA_API    AE_DLL_EXPORT
#else
#   define AE_IA_API    AE_DLL_IMPORT
#endif

namespace AE::InputActions
{
    using AE::usize;
    using AE::CharType;


    struct InputActionsInfo
    {
        // input
        const CharType* const*  inFiles             = null;     // [inFileCount]
        usize                   inFileCount         = 0;

        // output
        const CharType *        outputPackName      = null;
        const CharType *        outputScriptFile    = null;
    };

    extern "C" bool AE_IA_API ConvertInputActions (const InputActionsInfo *info);

} // AE::InputActions
