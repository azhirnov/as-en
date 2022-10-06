// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MCommandBatch.h"

namespace AE::Graphics
{

	//
	// Metal Draw Command Batch
	//

	class MDrawCommandBatch final : public EnableRC< MDrawCommandBatch >
	{
		friend class MDrawTask;
		friend class MRenderTaskScheduler;

	// types
	public:
		using CmdBufPool	= MCommandBatch::CmdBufPool;
		using Encoder_t		= MetalParallelRenderCommandEncoderRC;


	// variables
	private:
		// for draw tasks
		CmdBufPool				_cmdPool;

		Encoder_t				_encoder;

		const ubyte				_indexInPool;
		
		MPrimaryCmdBufState		_primaryState;

		DEBUG_ONLY( String		_dbgName; )


	// methods
	public:
		~MDrawCommandBatch () override {}

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (const Tuple<Ctor...>&	ctor	= Default,
						 const Tuple<Deps...>&	deps	= Default,
						 StringView				dbgName	= Default);

		//bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs);
		
		ND_ ECommandBufferType			GetCmdBufType ()		const	{ return ECommandBufferType::Secondary_RenderCommands; }
		ND_ EQueueType					GetQueueType ()			const	{ return EQueueType::Graphics; }
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return _primaryState; }
		ND_ NtStringView				DbgName ()				const;
		//ND_ ArrayView<VkViewport>		GetViewports ()			const	{ return _viewports; }
		//ND_ ArrayView<VkRect2D>		GetScissors ()			const	{ return _scissors; }

		
	// render task scheduler api
	private:
		explicit MDrawCommandBatch (uint indexInPool);

		bool  _Create (const MPrimaryCmdBufState &primaryState, StringView dbgName);
		void  _ReleaseObject () override;
	};



	//
	// Metal Draw Task interface
	//
	
	class MDrawTask : public Threading::IAsyncTask
	{
	// variables
	private:
		RC<MDrawCommandBatch>		_batch;
		uint						_drawIndex	= UMax;
		DEBUG_ONLY(
			const String			_dbgName;
		)
			

	// methods
	public:
		MDrawTask (RC<MDrawCommandBatch> batch, StringView dbgName) :
			IAsyncTask{ EThread::Renderer },
			_batch{ RVRef(batch) },
			_drawIndex{ _GetPool().Acquire() }
			DEBUG_ONLY(, _dbgName{ dbgName })
		{
			Unused( dbgName );
		}

		~MDrawTask ()
		{
			ASSERT( _drawIndex == UMax );
		}
		
		ND_ uint					GetDrawOrderIndex ()	const	{ return _drawIndex; }
		ND_ RC<MDrawCommandBatch>	GetDrawBatch ()			const	{ return _batch; }
		ND_ MDrawCommandBatch *		GetDrawBatchPtr ()		const	{ return _batch.get(); }


	// IAsyncTask
	public:
		void  OnCancel () override final;

		DEBUG_ONLY( NtStringView  DbgName () const override final { return _dbgName; })
			
	protected:
		void  OnFailure ();

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf);

	private:
		ND_ MDrawCommandBatch::CmdBufPool&  _GetPool ()		{ return _batch->_cmdPool; }
	};

} // AE::Graphics

#endif	// AE_ENABLE_METAL
