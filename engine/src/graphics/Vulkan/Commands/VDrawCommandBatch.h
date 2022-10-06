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
		friend class VDrawTask;
		friend class VRenderTaskScheduler;

	// types
	public:
		using CmdBufPool	= VCommandBatch::CmdBufPool;
		using Viewports_t	= FixedArray< VkViewport, GraphicsConfig::MaxViewports >;
		using Scissors_t	= FixedArray< VkRect2D,   GraphicsConfig::MaxViewports >;


	// variables
	private:
		// for draw tasks
		CmdBufPool				_cmdPool;

		const ubyte				_indexInPool;
		
		VPrimaryCmdBufState		_primaryState;

		Viewports_t				_viewports;
		Scissors_t				_scissors;

		DEBUG_ONLY( String		_dbgName; )


	// methods
	public:
		~VDrawCommandBatch () override {}

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (const Tuple<Ctor...>&	ctor	= Default,
						 const Tuple<Deps...>&	deps	= Default,
						 StringView				dbgName	= Default);

		bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs);
		
		ND_ ECommandBufferType			GetCmdBufType ()		const	{ return ECommandBufferType::Secondary_RenderCommands; }
		ND_ EQueueType					GetQueueType ()			const	{ return EQueueType::Graphics; }
		ND_ VPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return _primaryState; }
		ND_ NtStringView				DbgName ()				const;
		ND_ ArrayView<VkViewport>		GetViewports ()			const	{ return _viewports; }
		ND_ ArrayView<VkRect2D>			GetScissors ()			const	{ return _scissors; }

		
	// render task scheduler api
	private:
		explicit VDrawCommandBatch (uint indexInPool);

		bool  _Create (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports, ArrayView<VkRect2D> scissors, StringView dbgName);
		void  _ReleaseObject () override;
	};



	//
	// Vulkan Draw Task interface
	//
	
	class VDrawTask : public Threading::IAsyncTask
	{
	// variables
	private:
		RC<VDrawCommandBatch>		_batch;
		uint						_drawIndex	= UMax;
		DEBUG_ONLY(
			const String			_dbgName;
		)
			

	// methods
	public:
		VDrawTask (RC<VDrawCommandBatch> batch, StringView dbgName) :
			IAsyncTask{ EThread::Renderer },
			_batch{ RVRef(batch) },
			_drawIndex{ _GetPool().Acquire() }
			DEBUG_ONLY(, _dbgName{ dbgName })
		{
			Unused( dbgName );
		}

		~VDrawTask ()
		{
			ASSERT( _drawIndex == UMax );
		}
		
		ND_ uint					GetDrawOrderIndex ()	const	{ return _drawIndex; }
		ND_ RC<VDrawCommandBatch>	GetDrawBatch ()			const	{ return _batch; }
		ND_ VDrawCommandBatch *		GetDrawBatchPtr ()		const	{ return _batch.get(); }


	// IAsyncTask
	public:
		void  OnCancel () override final;

		DEBUG_ONLY( NtStringView  DbgName () const override final { return _dbgName; })
			
	protected:
		void  OnFailure ();

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf);

	private:
		ND_ VDrawCommandBatch::CmdBufPool&  _GetPool ()		{ return _batch->_cmdPool; }
	};


} // AE::Graphics

#endif	// AE_ENABLE_VULKAN
