// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/Commands/VRenderTaskScheduler.h"

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
		Ptr<VDrawCommandBatch>		_batch;		// can be null

		VkDependencyInfo			_barrier		= {};
		ImageMemoryBarriers_t		_imageBarriers;


	// methods
	public:
		explicit VDrawBarrierManager (Ptr<VDrawCommandBatch> batch);
		explicit VDrawBarrierManager (const VPrimaryCmdBufState &primaryState);

		template <typename ID>
		ND_ auto*						Get (ID id)						{ return GetResourceManager().GetResource( id ); }

		ND_ VDevice const&				GetDevice ()			const	{ return RenderTaskScheduler().GetDevice(); }
		ND_ VResourceManager&			GetResourceManager ()	const	{ return RenderTaskScheduler().GetResourceManager(); }
		ND_ FrameUID					GetFrameId ()			const	{ return _primaryState.frameId; }
		ND_ EQueueType					GetQueueType ()			const	{ return EQueueType::Graphics; }
		ND_ VPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return _primaryState; }

		ND_ bool						IsSecondary ()			const	{ return _batch != null; }
		//ND_ VDrawCommandBatch &		GetBatch ()				const	{ return *_batch; }
		//ND_ RC<VDrawCommandBatch>		GetBatchRC ()			const	{ return _batch->GetRC<VDrawCommandBatch>(); }

		ND_ const VkDependencyInfo*		GetBarriers ();
		ND_ bool						NoPendingBarriers () const	{ return _imageBarriers.empty(); }
		ND_ bool						HasPendingBarriers () const	{ return not NoPendingBarriers(); }

		void  ClearBarriers ();
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState);

	private:
		void  _Init ();
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
