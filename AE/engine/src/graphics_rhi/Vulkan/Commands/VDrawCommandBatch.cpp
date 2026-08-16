// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VDrawCommandBatch.h"
# include "graphics_rhi/Vulkan/VRenderTaskScheduler.h"
# include "graphics_rhi/Private/DrawCommandBatch.cpp.h"

namespace AE::Graphics
{

/*
=================================================
	GetCmdBuffers
----
	same as 'EndAllSecondary()' for Metal
=================================================
*/
	bool  DrawCommandBatch::GetCmdBuffers (OUT uint &count, INOUT StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___
	{
		EStatus	old_status = _status.Set( EStatus::Submitted );
		CHECK_ERR( AnyEqual( old_status, EStatus::Recording, EStatus::Submitted, EStatus::Pending ));

		_cmdPool.Lock();
		CHECK_ERR( _cmdPool.IsReady() );

		auto&	rts	= GraphicsScheduler();
		CHECK_ERR( _cmdPool.CommitIndirectBuffers( rts.GetCommandPoolManager(), GetQueueType(), GetCmdBufType(), &GetPrimaryCtxState() ));

		_cmdPool.GetCommands( OUT cmdbufs.data(), OUT count, uint(cmdbufs.size()) );
		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
