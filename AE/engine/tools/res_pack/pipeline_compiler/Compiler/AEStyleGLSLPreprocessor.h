// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Compiler/IShaderProprocessor.h"

namespace AE::PipelineCompiler
{

    //
    // AE-style GLSL Preprocessor
    //

    class AEStyleGLSLPreprocessor final : public IShaderProprocessor
    {
    // variables
    private:
        FlatHashMap< StringView, StringView >   _typeMap;


    // methods
    public:
        AEStyleGLSLPreprocessor ();

        // IShaderProprocessor //
        bool  Process (EShader shaderType, const PathAndLine &, usize headerLines, StringView inStr, OUT String &outStr) override;
    };


} // AE::PipelineCompiler
