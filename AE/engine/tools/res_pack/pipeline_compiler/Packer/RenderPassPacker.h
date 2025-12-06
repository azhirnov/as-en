#pragma once

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "res_pack/pipeline_compiler/Packer/RenderPassPack.h"

namespace AE::PipelineCompiler
{

	//
	// Render Pass Packer
	//

	class RenderPassPacker
	{
	// methods
	public:
		ND_ static bool  Serialize (Serializing::Serializer &ser) __NE___;
	};

} // AE::PipelineCompiler
#endif // AE_BUILD_PIPELINE_COMPILER
