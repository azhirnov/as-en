// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VDrawCommandBatch.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{
#	include "graphics/Private/DrawCommandBatch.cpp.h"

/*
=================================================
	GetCmdBuffers
----
	same as 'EndAllSecondary()' for Metal
=================================================
*/
	bool  DRAWCMDBATCH::GetCmdBuffers (OUT uint &count, INOUT StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___
	{
		EStatus	old_status = _status.exchange( EStatus::Submitted );
		CHECK_ERR( AnyEqual( old_status, EStatus::Recording, EStatus::Submitted, EStatus::Pending ));

		_cmdPool.Lock();
		CHECK_ERR( _cmdPool.IsReady() );

		auto&	rts	= GraphicsScheduler();
		CHECK_ERR( _cmdPool.CommitIndirectBuffers( rts.GetCommandPoolManager(), GetQueueType(), GetCmdBufType(), &GetPrimaryCtxState() ));

		_cmdPool.GetCommands( cmdbufs.data(), OUT count, uint(cmdbufs.size()) );
		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
