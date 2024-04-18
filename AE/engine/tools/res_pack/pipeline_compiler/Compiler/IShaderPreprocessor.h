// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Packer/PipelinePack.h"

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
