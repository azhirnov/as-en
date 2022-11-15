// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MCommandBatch.h"
# include "graphics/Metal/Commands/MCommandBuffer.h"

namespace AE::Graphics
{

	//
	// Metal Draw Command Batch
	//

	class MDrawCommandBatch final : public EnableRC< MDrawCommandBatch >
	{
		friend class DrawTask;
		friend class MRenderTaskScheduler;

	// types
	public:
		using CmdBufPool	= MCommandBatch::CmdBufPool;
		using Encoder_t		= MetalParallelRenderCommandEncoderRC;
		using Viewport_t	= RenderPassDesc::Viewport;
		using Viewports_t	= FixedArray< Viewport_t,	GraphicsConfig::MaxViewports >;
		using Scissors_t	= FixedArray< RectI,		GraphicsConfig::MaxViewports >;


	// variables
	private:
		// for draw tasks
		CmdBufPool				_cmdPool;

		Encoder_t				_encoder;

		const ubyte				_indexInPool;
		
		MPrimaryCmdBufState		_primaryState;
		
		Viewports_t				_viewports;
		Scissors_t				_scissors;

		PROFILE_ONLY(
			RGBA8u					_dbgColor;
			String					_dbgName;
			RC<IGraphicsProfiler>	_profiler;
		)


	// methods
	public:
		~MDrawCommandBatch () override {}

		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask	Add (const Tuple<Ctor...>&	ctor	= Default,
						 const Tuple<Deps...>&	deps	= Default,
						 StringView				dbgName	= Default);

	  #ifdef AE_HAS_COROUTINE
		template <typename PromiseT, typename ...Deps>
		AsyncTask	Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
						 const Tuple<Deps...>&						deps	= Default,
						 StringView									dbgName	= Default);
	  #endif
	  
		//bool  GetCmdBuffers (OUT uint &count, INOUT StaticArray< MetalCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs);
		
		ND_ ECommandBufferType			GetCmdBufType ()		const	{ return ECommandBufferType::Secondary_RenderCommands; }
		ND_ EQueueType					GetQueueType ()			const	{ return EQueueType::Graphics; }
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return _primaryState; }
		ND_ ArrayView<Viewport_t>		GetViewports ()			const	{ return _viewports; }
		ND_ ArrayView<RectI>			GetScissors ()			const	{ return _scissors; }

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
		explicit MDrawCommandBatch (uint indexInPool);

		bool  _Create (const MPrimaryCmdBufState &primaryState, ArrayView<Viewport_t> viewports,
					   ArrayView<RectI> scissors, StringView dbgName, RGBA8u dbgColor);
		void  _ReleaseObject () __NE_OV;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


#	define CMDDRAWBATCH		MDrawCommandBatch
#	include "graphics/Private/DrawTask.inl.h"
//-----------------------------------------------------------------------------
	


namespace AE::Graphics
{
/*
=================================================
	constructor
=================================================
*/
	inline MDrawCommandBatch::MDrawCommandBatch (uint indexInPool) :
		_indexInPool{ CheckCast<ubyte>( indexInPool )}
	{}

/*
=================================================
	Add
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  MDrawCommandBatch::Add (const Tuple<Ctor...>&	ctorArgs,
									   const Tuple<Deps...>&	deps,
									   StringView				dbgName)
	{
		//ASSERT( not IsSubmitted() );
		STATIC_ASSERT( IsBaseOf< DrawTask, TaskType >);

		auto	task = ctorArgs.Apply([this, dbgName] (auto&& ...args) { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., GetRC(), dbgName ); });

		if_likely( task->IsValid() and Scheduler().Run( task, deps ))
			return task;
		else
			return ;
	}

/*
=================================================
	Add
=================================================
*/
# ifdef AE_HAS_COROUTINE
	template <typename PromiseT, typename ...Deps>
	AsyncTask  VDrawCommandBatch::Add (AE::Threading::CoroutineHandle<PromiseT>	handle,
									   const Tuple<Deps...>&					deps,
									   StringView								dbgName)
	{
		//ASSERT( not IsSubmitted() );
		STATIC_ASSERT( IsSameTypes< AE::Threading::CoroutineHandle<PromiseT>, CoroutineDrawTask >);
		
		auto	task = MakeRC<AE::Threading::_hidden_::DrawTaskCoroutineRunner>( RVRef(handle), GetRC(), dbgName );
		
		if_likely( task->IsValid() and Scheduler().Run( task, deps ))
			return task;
		else
			return ;
	}
# endif

} // AE::Graphics

#endif // AE_ENABLE_METAL
