// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_pack/pipeline_compiler/Compiler/SpirvCompiler.h"

#ifdef AE_ENABLE_SPIRV_CROSS

# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wdouble-promotion"
# endif

# include "spirv_cross/spirv_cross.hpp"
# include "spirv_cross/spirv_glsl.hpp"

# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
# endif

namespace AE::PipelineCompiler
{

String  SpirvCompiler::_SpirvToGLSL (const SpirvBytecode_t &spirv)
{
	try{
		spirv_cross::CompilerGLSL			compiler {spirv.data(), spirv.size()};
		spirv_cross::CompilerGLSL::Options	opt = {};

		opt.version						= 460;
		opt.es							= false;
		opt.vulkan_semantics			= true;
		opt.separate_shader_objects		= true;
		opt.enable_420pack_extension	= true;

		opt.vertex.fixup_clipspace		= false;
		opt.vertex.flip_vert_y			= false;
		opt.vertex.support_nonzero_base_instance = true;

		opt.fragment.default_float_precision	= spirv_cross::CompilerGLSL::Options::Precision::Highp;
		opt.fragment.default_int_precision		= spirv_cross::CompilerGLSL::Options::Precision::Highp;

		compiler.set_common_options( opt );

		return compiler.compile();	// throw
	}
	catch (...)
	{
		return {};
	}
}

} // AE::PipelineCompiler
#endif // AE_ENABLE_SPIRV_CROSS
