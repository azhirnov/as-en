// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "res_pack/pipeline_compiler/Packer/PipelinePack.h"
#include "res_pack/pipeline_compiler/Packer/RenderPassPack.h"
#include "res_pack/pipeline_compiler/Compiler/IShaderPreprocessor.h"
#include "res_pack/pipeline_compiler/ScriptObjects/Common.h"
#include "res_pack/pipeline_compiler/Compiler/SpirvCompiler.h"

namespace slang {
	struct IGlobalSession;
	struct ISession;
	struct ShaderReflection;
}

namespace AE::PipelineCompiler
{

	//
	// SLang Compiler
	//

	class SLangCompiler final : public NothrowAllocatable
	{
	// types
	public:
		using ShaderReflection = SpirvCompiler::ShaderReflection;

		struct Input
		{
			EShader				shaderType		= Default;
			EShaderOpt			options			= Default;
			EShaderVersion		dstVersion		= Default;
			StringView			entry;
			StringView			header;
			StringView			source;
			PathAndLine			fileLoc;		// only for output parser
		};

		struct Output
		{
			SpirvBytecode_t		spirv;
			MetalBytecode_t		metal;
			String				source;
			String				log;
			ShaderReflection	reflection;
		};

		using slang_shutdown_t = void (*) ();


	// variables
	private:
		slang::IGlobalSession*		_globalSession	= null;
		slang::ISession*			_session		= null;

		Library						_lib;
		slang_shutdown_t			_shutdown		= null;

		Array<String>				_includeDirs;

		static constexpr bool		_quietWarnings		= true;


	// methods
	public:
		explicit SLangCompiler (ArrayView<Path> includeDirs, Path libPath = {})	__NE___;
		~SLangCompiler ()														__NE___;

		ND_ bool  IsInitialized ()												C_NE___	{ return _globalSession != null; }

		ND_ bool  Compile (const Input &in, OUT Output &out)					__NE___;

	private:
		ND_ bool  _Initialize (ArrayView<Path> includeDirs, Path libPath);
			void  _Deinitialize ();

		ND_ bool  _BeginSession (const Input &in);
			void  _EndSession ();

		ND_ bool  _CompileImpl (const Input &in, OUT Output &out)						__Th___;

		ND_ bool  _ParseReflection (slang::ShaderReflection &layout, OUT ShaderReflection &result) __Th___;
	};


} // AE::PipelineCompiler
