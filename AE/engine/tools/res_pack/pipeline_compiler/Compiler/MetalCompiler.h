// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Packer/PipelinePack.h"
#include "Compiler/SpirvCompiler.h"

namespace AE::PipelineCompiler
{

	//
	// Metal Compiler
	//

	struct MetalCompiler : NothrowAllocatable
	{
	// types
	public:
		struct SpirvToMslConfig
		{
			EShader			shaderType		= Default;
			EShaderVersion	version			= Default;
			bool			useArgBuffer	= false;
		};

		struct Input
		{
			ECompilationTarget	target				= Default;
			EShaderOpt			options				= Default;
			EShaderVersion		version				= Default;
			EShader				shaderType			= Default;
			StringView			source;
			usize				headerLines			= 0;
			bool				enablePreprocessing	= false;
			PathAndLine			fileLoc;			// only for output parser
		};

		using ShaderReflection = SpirvCompiler::ShaderReflection;


	// variables
	private:
		Array< String >				_directories;

		Unique<IShaderPreprocessor>	_preprocessor;


	// methods
	public:
		explicit MetalCompiler (ArrayView<Path> includeDirs)	__NE___;
		~MetalCompiler ()										__NE___	{}

			void  SetPreprocessor (IShaderPreprocessor* ptr)	__NE___	{ _preprocessor.reset( ptr ); }

		ND_ bool  SpirvToMsl (const SpirvToMslConfig &cfg, SpirvBytecode_t spirv, INOUT ShaderReflection &reflection, OUT String &src) const;
		ND_ bool  Compile (const Input &in, OUT MetalBytecode_t &bytecode, OUT String &log) const;

		// not a full reflection!
		// only limited number of features
		ND_ bool  BuildReflection (const Input &in, INOUT ShaderReflection &reflection, OUT String &log) const;

	private:
		ND_ bool  _ParseOutput (const Input &in, StringView output, StringView tempShaderName, OUT String &log) const;

		ND_ bool  _BuildReflectionForMeshShader (StringView src, INOUT ShaderReflection &reflection, INOUT String &log) const;
	};


} // AE::PipelineCompiler
