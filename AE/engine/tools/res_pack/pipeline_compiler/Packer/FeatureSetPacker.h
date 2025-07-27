// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "res_pack/pipeline_compiler/Packer/FeatureSetPack.h"

namespace AE::PipelineCompiler
{

	//
	// Feature Set Packer
	//

	class FeatureSetPacker
	{
	// methods
	public:
		static bool  Serialize (Serializing::Serializer &ser) __NE___;
	};


} // AE::PipelineCompiler
#endif // AE_BUILD_PIPELINE_COMPILER
