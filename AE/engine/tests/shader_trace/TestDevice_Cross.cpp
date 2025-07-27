// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_SPIRV_CROSS
# include "spirv_cross/spirv_cross.hpp"
# include "spirv_cross/spirv_glsl.hpp"

# include "TestDevice.h"

namespace AE::PipelineCompiler
{
	
	void  TestDevice::_Decompile (const Array<uint> &spirvData)
	{
		spirv_cross::CompilerGLSL			compiler {spirvData.data(), spirvData.size()};
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

		String	glsl_src = compiler.compile();	// throw
		AE_LOGI( glsl_src );
	}

} // AE::PipelineCompiler
#endif // AE_ENABLE_SPIRV_CROSS
