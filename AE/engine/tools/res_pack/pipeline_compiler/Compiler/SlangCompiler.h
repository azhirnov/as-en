// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

# include "Packer/PipelinePack.h"
# include "Packer/RenderPassPack.h"
# include "Compiler/IShaderPreprocessor.h"

namespace slang {
	struct IGlobalSession;
	struct ISession;
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
		struct Input
		{
		//	EShader				shaderType		= Default;
		//	Version2			spirvVersion;
		//	EShaderOpt			options			= Default;
			StringView			entry;
			StringView			header;
			StringView			source;
			PathAndLine			fileLoc;		// only for output parser
		};

		struct Output
		{
			SpirvBytecode_t		spirv;
			MetalBytecode_t		metal;
			String				log;
		};
		
		using slang_shutdown_t = void (*) ();


	// variables
	private:
		slang::IGlobalSession*		_globalSession	= null;
		slang::ISession*			_session		= null;

		Library						_lib;
		slang_shutdown_t			_shutdown		= null;


	// methods
	public:
		explicit SLangCompiler (ArrayView<Path> includeDirs)		__NE___;
		~SLangCompiler ();

		ND_ bool  IsInitialized ()									C_NE___	{ return _session != null; }

		ND_ bool  Compile (const Input &in, OUT Output &out);

	private:
		ND_ bool  _Initialize (ArrayView<Path> includeDirs);
			void  _Deinitialize ();
	};


} // AE::PipelineCompiler
