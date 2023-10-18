// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Compiler/IShaderPreprocessor.h"

namespace AE::PipelineCompiler
{

    //
    // AE-style GLSL Preprocessor
    //

    class AEStyleGLSLPreprocessor final : public IShaderPreprocessor
    {
    // variables
    private:
        FlatHashMap< StringView, StringView >   _typeMap;


    // methods
    public:
        AEStyleGLSLPreprocessor ();

        // IShaderPreprocessor //
        bool  Process (EShader shaderType, const PathAndLine &, usize headerLines, StringView inStr, OUT String &outStr) override;
    };


} // AE::PipelineCompiler
