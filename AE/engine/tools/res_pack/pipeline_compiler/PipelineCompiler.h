// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

#ifdef AE_BUILD_PIPELINE_COMPILER
#   define AE_PC_API    AE_DLL_EXPORT
#else
#   define AE_PC_API    AE_DLL_IMPORT
#endif


namespace AE::PipelineCompiler
{
    using AE::uint;
    using AE::usize;
    using AE::CharType;


    enum class EPathParamsFlags : uint
    {
        Unknown                     = 0,
        Recursive                   = 1 << 0,
        _Last,
        All                         = ((_Last - 1) << 1) - 1,
    };


    enum class EReflectionFlags : uint
    {
        Unknown                     = 0,
        RenderTechniques            = 1 << 0,
        RTechPass_Pipelines         = 1 << 1,
        RTech_ShaderBindingTable    = 1 << 2,
        _Last,
        All                         = ((_Last - 1) << 1) - 1,
    };
    AE_BIT_OPERATORS( EReflectionFlags );


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


    struct PipelinesInfo
    {
        // input pipelines
        const PathParams *      pipelineFolders         = null;     // [pipelineFolderCount]
        usize                   pipelineFolderCount     = 0;
        const PathParams *      inPipelines             = null;     // [inPipelineCount]
        usize                   inPipelineCount         = 0;

        // input shaders
        const CharType * const* shaderFolders           = null;     // [shaderFolderCount]
        usize                   shaderFolderCount       = 0;

        // shader include directories
        const CharType * const* shaderIncludeDirs       = null;     // [shaderIncludeDirCount]
        usize                   shaderIncludeDirCount   = 0;

        // pipeline include directories
        const CharType * const* pipelineIncludeDirs     = null;     // [pipelineIncludeDirCount]
        usize                   pipelineIncludeDirCount = 0;

        // output
        const CharType *        outputPackName          = null;
        EReflectionFlags        cppReflectionFlags      = EReflectionFlags::Unknown;
        const CharType *        outputCppStructsFile    = null;     // C++ reflection
        const CharType *        outputCppNamesFile      = null;     // C++ reflection
        const CharType *        outputScriptFile        = null;     // script reflection
        bool                    addNameMapping          = false;    // for debugging
    };


    extern "C" bool AE_PC_API CompilePipelines (const PipelinesInfo* info);


} // AE::PipelineCompiler
