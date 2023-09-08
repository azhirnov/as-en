// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Packer/PipelinePack.h"

namespace AE::PipelineCompiler
{

    //
    // Shader Proprocessor interface
    //

    class IShaderProprocessor
    {
    public:
        virtual ~IShaderProprocessor () {}

        virtual bool  Process (EShader shaderType, const PathAndLine &, usize headerLines, StringView inStr, OUT String &outStr) = 0;
    };


} // AE::PipelineCompiler
