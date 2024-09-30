// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/Common.h"

namespace AE::PipelineCompiler
{

	//
	// Script Config
	//

	class ScriptConfig
	{
	// methods
	public:
		ScriptConfig () {}
		~ScriptConfig ()									__Th___;

		void  SetTarget (ECompilationTarget value)			__Th___;

		void  SetShaderVersion (EShaderVersion value)		__Th___;
		void  SetSpirvToMslVersion (EShaderVersion value)	__Th___;
		void  SetShaderOptions (EShaderOpt value)			__Th___;
		void  SetDefaultLayout (EStructLayout value)		__Th___;
		void  SetPipelineOptions (EPipelineOpt value)		__Th___;
		void  SetPreprocessor (EShaderPreprocessor value)	__Th___;
		void  SetDefaultFeatureSet (const String &value)	__Th___;
		void  SetShaderDefines (const String &value)		__Th___;

		static void  Bind (const ScriptEnginePtr &se)		__Th___;
	};


} // AE::PipelineCompiler
