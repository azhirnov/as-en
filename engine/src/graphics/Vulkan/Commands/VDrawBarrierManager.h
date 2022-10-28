// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VDrawCommandBatch.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Draw Command Context Barrier Manager
	//

	class VDrawBarrierManager
	{
	// types
	private:
		using ImageMemoryBarriers_t	= FixedArray< VkImageMemoryBarrier2, GraphicsConfig::MaxAttachments/2 >;


	// variables
	private:
		const VPrimaryCmdBufState	_primaryState;
		Ptr<VDrawCommandBatch>		_batch;			// can be null

		VResourceManager &			_resMngr;

		VkDependencyInfo			_barrier		= {};
		ImageMemoryBarriers_t		_imageBarriers;


	// methods
	public:
		explicit VDrawBarrierManager (Ptr<VDrawCommandBatch> batch);
		explicit VDrawBarrierManager (const VPrimaryCmdBufState &primaryState);

		template <typename ID>
		ND_ auto*					Get (ID id)						{ return GetResourceManager().GetResource( id ); }
		
		template <typename ID>
		ND_ bool					IsAlive (ID id)			const	{ return GetResourceManager().IsAlive( id ); }

		ND_ VDevice const&			GetDevice ()			const	{ return _resMngr.GetDevice(); }
		ND_ VResourceManager&		GetResourceManager ()	const	{ return _resMngr; }
		ND_ FrameUID				GetFrameId ()			const	{ return _primaryState.frameId; }
		ND_ EQueueType				GetQueueType ()			const	{ return EQueueType::Graphics; }
		ND_ auto const&				GetPrimaryCtxState ()	const	{ return _primaryState; }

		ND_ bool					IsSecondary ()			const	{ return _batch != null; }
		ND_ VDrawCommandBatch *		GetBatchPtr ()			const	{ return _batch.get(); }

		ND_ const VkDependencyInfo*	GetBarriers ();
		ND_ bool					NoPendingBarriers () const	{ return _imageBarriers.empty(); }
		ND_ bool					HasPendingBarriers () const	{ return not NoPendingBarriers(); }

		void  ClearBarriers ();
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState);

	private:
		void  _Init ();
	};



/*
=================================================
	GetBarriers
=================================================
*/
	forceinline const VkDependencyInfo*  VDrawBarrierManager::GetBarriers ()
	{
		if_unlikely( HasPendingBarriers() )
		{
			_barrier.imageMemoryBarrierCount	= uint(_imageBarriers.size());
			_barrier.pImageMemoryBarriers		= _imageBarriers.data();

			return &_barrier;
		}
		else
			return null;
	}
	
/*
=================================================
	ClearBarriers
=================================================
*/
	forceinline void  VDrawBarrierManager::ClearBarriers ()
	{
		_imageBarriers.clear();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
