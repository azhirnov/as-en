// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "res_pack/pipeline_compiler/Packer/SamplerPack.h"

namespace AE::PipelineCompiler
{

	//
	// Sampler Packer
	//

	class SamplerPacker
	{
	// methods
	public:
		static bool  Serialize (Serializing::Serializer &ser) __NE___;
	};

} // AE::PipelineCompiler
#endif // AE_BUILD_PIPELINE_COMPILER
