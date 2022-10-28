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

		PROFILE_ONLY(
			RGBA8u					_dbgColor;
			String					_dbgName;
			RC<IGraphicsProfiler>	_profiler;
		)


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
		ND_ ArrayView<VkViewport>		GetViewports ()			const	{ return _viewports; }
		ND_ ArrayView<VkRect2D>			GetScissors ()			const	{ return _scissors; }
		
		#ifdef AE_DBG_OR_DEV_OR_PROF
			ND_ Ptr<IGraphicsProfiler>	GetProfiler ()			const	{ return _profiler.get(); }
			ND_ StringView				DbgName ()				const	{ return _dbgName; }
			ND_ RGBA8u					DbgColor ()				const	{ return _dbgColor; }
		#else
			ND_ StringView				DbgName ()				const	{ return Default; }
			ND_ RGBA8u					DbgColor ()				const	{ return HtmlColor::Lime; }
		#endif

		
	// render task scheduler api
	private:
		explicit VDrawCommandBatch (uint indexInPool);

		bool  _Create (const VPrimaryCmdBufState &primaryState, ArrayView<VkViewport> viewports, ArrayView<VkRect2D> scissors,
					   StringView dbgName, RGBA8u dbgColor);
		void  _ReleaseObject () override;
	};



	//
	// Vulkan Draw Task interface
	//
	
	class VDrawTask : public Threading::IAsyncTask
	{
	// variables
	private:
		RC<VDrawCommandBatch>	_batch;
		uint					_drawIndex	= UMax;

		PROFILE_ONLY(
			const String		_dbgName;
			const RGBA8u		_dbgColor;
		)
			

	// methods
	public:
		VDrawTask (RC<VDrawCommandBatch> batch, StringView dbgName, RGBA8u dbgColor = HtmlColor::Lime) :
			IAsyncTask{ EThread::Renderer },
			_batch{ RVRef(batch) },
			_drawIndex{ _GetPool().Acquire() }
			PROFILE_ONLY(, _dbgName{ dbgName }, _dbgColor{ dbgColor })
		{
			Unused( dbgName, dbgColor );
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
		
		#ifdef AE_DBG_OR_DEV_OR_PROF
			ND_ String		DbgFullName ()	const;
			ND_ StringView  DbgName ()		const override final	{ return _dbgName; }
			ND_ RGBA8u		DbgColor ()		const					{ return _dbgColor; }
		#else
			ND_ String		DbgFullName ()	const					{ return Default; }
			ND_ StringView  DbgName ()		const override final	{ return Default; }
			ND_ RGBA8u		DbgColor ()		const					{ return HtmlColor::Lime; }
		#endif
			
	protected:
		void  OnFailure ();

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf);

	private:
		ND_ VDrawCommandBatch::CmdBufPool&  _GetPool ()		{ return _batch->_cmdPool; }
	};
//-----------------------------------------------------------------------------
	

	
/*
=================================================
	constructor
=================================================
*/
	inline VDrawCommandBatch::VDrawCommandBatch (uint indexInPool) :
		_indexInPool{ CheckCast<ubyte>( indexInPool )}
	{}

/*
=================================================
	Add
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  VDrawCommandBatch::Add (const Tuple<Ctor...>&	ctorArgs,
									   const Tuple<Deps...>&	deps,
									   StringView				dbgName)
	{
		//ASSERT( not IsSubmitted() );

		auto	task = ctorArgs.Apply([this, dbgName] (auto&& ...args) { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbgName ); });

		if_likely( task->GetDrawOrderIndex() != UMax and Threading::Scheduler().Run( task, deps ))
			return task;
		else
			return null;
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	OnCancel
=================================================
*/
	inline void  VDrawTask::OnCancel ()
	{
		_GetPool().Complete( INOUT _drawIndex );
	}
	
/*
=================================================
	OnFailure
=================================================
*/
	inline void  VDrawTask::OnFailure ()
	{
		_GetPool().Complete( INOUT _drawIndex );
		IAsyncTask::OnFailure();
	}
	
/*
=================================================
	Execute
=================================================
*/
	template <typename CmdBufType>
	void  VDrawTask::Execute (CmdBufType &cmdbuf)
	{
		_GetPool().Add( INOUT _drawIndex, cmdbuf.EndCommandBuffer() );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
