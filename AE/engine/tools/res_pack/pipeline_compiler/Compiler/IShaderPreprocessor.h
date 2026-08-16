// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "res_pack/pipeline_compiler/Packer/PipelinePack.h"

namespace AE::PipelineCompiler
{

	//
	// Shader Preprocessor interface
	//

	class IShaderPreprocessor
	{
	public:
		virtual ~IShaderPreprocessor () {}

		virtual bool  Process (EShader shaderType, const PathAndLine &, usize headerLines, StringView inStr, OUT String &outStr) = 0;
	};


} // AE::PipelineCompiler
