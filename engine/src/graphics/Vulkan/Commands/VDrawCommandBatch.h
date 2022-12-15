// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VCommandBatch.h"

namespace AE::Graphics
{

	//
	// Vulkan Draw Command Batch
	//

	class VDrawCommandBatch final : public EnableRC< VDrawCommandBatch >
	{
		#include "graphics/Private/DrawCommandBatchDecl.h"

	// methods
	public:
		bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	include "graphics/Private/DrawTask.h"
//-----------------------------------------------------------------------------

#endif // AE_ENABLE_VULKAN
