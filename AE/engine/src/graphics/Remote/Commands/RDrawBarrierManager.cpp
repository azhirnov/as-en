// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RDrawBarrierManager.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	constructor
=================================================
*/
	RDrawBarrierManager::RDrawBarrierManager (Ptr<RDrawCommandBatch> batch, uint drawIdx) __NE___ :
		_primaryState{ batch->GetPrimaryCtxState() },
		_batch{ batch },  _drawIndex{ drawIdx },
		_resMngr{ GraphicsScheduler().GetResourceManager() }
	{
		GCTX_CHECK( _batch->GetQueueType() == GetQueueType() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( _primaryState.useSecondaryCmdbuf );
	}

	RDrawBarrierManager::RDrawBarrierManager (const RPrimaryCmdBufState &primaryState) __NE___ :
		_primaryState{ primaryState },
		_resMngr{ GraphicsScheduler().GetResourceManager() }
	{
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( not _primaryState.useSecondaryCmdbuf );
	}

/*
=================================================
	GetBarriers
=================================================
*/
	RDependencyInfo  RDrawBarrierManager::GetBarriers () __NE___
	{
		RDependencyInfo		result;
		if_unlikely( _cmdCount > 0 )
		{
			_ser.reset();
			auto	data = _memStream->GetData();

			result.data		= data.data();
			result.size		= ArraySizeOf(data);
			result.count	= _cmdCount;
		}
		return result;
	}

/*
=================================================
	ClearBarriers
=================================================
*/
	void  RDrawBarrierManager::ClearBarriers () __NE___
	{
		_memStream	= MakeRC<ArrayWStream>();
		_cmdCount	= 0;
		_ser.reset( new Serializing::Serializer{ _memStream });
		_ser->factory = &GraphicsScheduler().GetDevice().GetCommandsFactory();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_REMOTE_GRAPHICS
