// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Command batch allows to records multiple command buffer in parallel and submit them as a single batch.
	Software command buffers are supported to and will be automatically recorded to Vulkan command buffer before submitting.

		Dependencies.

	Dependencies are added to the whole batch.
		
	Batch in graphics queue depends on batch in compute/transfer queue -> insert semaphore dependency.
	Batch depends on batch in the same queue -> reorder batches before submit (not supported yet).

	Warning: don't use VCmdBatchOnSubmit with deferred submission!
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DeviceToHostSync.h"
# include "graphics/Vulkan/Commands/VCommandPoolManager.h"
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Private/LfCmdBufferPool.h"

namespace AE::Graphics
{

	//
	// Vulkan Command Batch
	//

	class VCommandBatch final : public IDeviceToHostSync
	{
		friend class VRenderTaskScheduler;
		friend class VRenderTask;
		friend class VDrawCommandBatch;

	// types
	private:

		//
		// Command Buffer Pool
		//
		struct CmdBufPool : LfCmdBufferPool< VkCommandBuffer, VBakedCommands >
		{
		// methods
		public:
			CmdBufPool () {}
			
			void  GetCommands (OUT VkCommandBuffer* cmdbufs, OUT uint &cmdbufCount, uint maxCount)		{ _GetCommands( OUT cmdbufs, OUT cmdbufCount, maxCount ); }
			void  GetCommands (VkCommandBufferSubmitInfoKHR* cmdbufs, OUT uint &cmdbufCount, uint maxCount);
			bool  CommitIndirectBuffers (VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType,
										 const VPrimaryCmdBufState* primaryState = null);

		private:
			ND_ bool  _CommitIndirectBuffers_Ordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
			ND_ bool  _CommitIndirectBuffers_Unordered (uint cmdTypes, VCommandPoolManager &cmdPoolMngr, EQueueType queue, ECommandBufferType cmdbufType, const VPrimaryCmdBufState* primaryState);
		};


		//
		// Submit Task
		//
		class SubmitTask final : public Threading::IAsyncTask
		{
		private:
			RC<VCommandBatch>	_batch;
			ESubmitMode			_mode;

		public:
			SubmitTask (RC<VCommandBatch> batch, ESubmitMode mode) :
				IAsyncTask{ EThread::Renderer },
				_batch{RVRef(batch)}, _mode{mode} {}

			void  Run () override
			{
				CHECK( _batch->Submit( _mode ));
			}

			NtStringView  DbgName () const override	{ return "Submit vulkan command batch"; }
		};

		
	  #if not AE_VK_TIMELINE_SEMAPHORE
		//
		// Virtual Fence
		//
		class VirtualFence final : public EnableRC<VirtualFence>
		{
		// variables
		private:
			Atomic<bool>	_complete		{false};
			ubyte			_indexInPool	= UMax;
			VkFence			_fence			= Default;


		// methods
		public:
			VirtualFence () {}
			~VirtualFence ();

			ND_ VkFence	Raw () const	{ return _fence; }

			ND_ bool	IsComplete (const VDevice &dev);
			ND_ bool	Wait (const VDevice &dev, nanoseconds timeout);

			ND_ bool	Create (const VDevice &dev, uint indexInPool);

				void	_ReleaseObject () override;
		};
	  #endif


		using GpuDependencies_t	= FixedMap< VkSemaphore, ulong, 7 >;
		using OutDependencies_t = FixedTupleArray< 15, AsyncTask, uint >;	// { task, bitIndex }

		enum class EStatus : uint
		{
			Initial,		// after _Create()
			Pending,		// after Submit()		// command batch is ready to be submitted to the GPU
			Submitted,		// after _OnSubmit()	// command batch is already submitted to the GPU
			Complete,		// after _OnComlete()	// command batch has been executed on the GPU
			Destroyed,		// after _ReleaseObject()
		};


	// variables
	private:
		// for render tasks
		alignas(AE_CACHE_LINE)
		  CmdBufPool			_cmdPool;

		alignas(AE_CACHE_LINE)
		  Atomic<EStatus>		_status			{EStatus::Destroyed};
		
		FrameUID				_frameId;
		EQueueType				_queueType		= Default;
		const ubyte				_indexInPool;
		ubyte					_submitIdx		= UMax;

		#if AE_VK_TIMELINE_SEMAPHORE
		  VkSemaphore			_tlSemaphore	= Default;
		  Atomic<ulong>			_tlSemaphoreVal	{0};
		#else
		   RC<VirtualFence>		_fence;
		#endif

		// dependencies from another batches on another queue
		// or dependencies for swapchain image
		alignas(AE_CACHE_LINE)
		  SpinLock				_gpuInDepsGuard;
		GpuDependencies_t		_gpuInDeps;
		alignas(AE_CACHE_LINE)
		  SpinLock				_gpuOutDepsGuard;
		GpuDependencies_t		_gpuOutDeps;
		
		// tasks which wait for batch to be submitted to the GPU
		alignas(AE_CACHE_LINE)
		  SpinLock				_onSubmitDepsGuard;
		OutDependencies_t		_onSubmitDeps;
		
		// tasks which wait for batch to complete on the GPU side
		alignas(AE_CACHE_LINE)
		  SpinLock				_onCompleteDepsGuard;
		OutDependencies_t		_onCompleteDeps;

		// TODO:
		//	- query pool for timestamps

		DEBUG_ONLY( String		_dbgName; )
			

	// methods
	public:
		~VCommandBatch ();


	// user api (thread safe)
	public:

		// command buffer api
			template <typename TaskType, typename ...Ctor, typename ...Deps>
			AsyncTask	Add (const Tuple<Ctor...>&	ctor	= Default,
							 const Tuple<Deps...>&	deps	= Default,
							 StringView				dbgName	= Default);

			template <typename ...Deps>
			AsyncTask  SubmitAsTask (const Tuple<Deps...>&	deps,
									 ESubmitMode			mode = ESubmitMode::Deferred);

		ND_ bool  Submit (ESubmitMode mode = ESubmitMode::Deferred);


		// GPU to GPU dependency
			bool  AddInputDependency (RC<VCommandBatch> batch);
			bool  AddInputSemaphore (VkSemaphore sem, ulong value);
			bool  AddInputSemaphore (const VulkanCmdBatchDependency &dep);

			bool  AddOutputSemaphore (VkSemaphore sem, ulong value);
			bool  AddOutputSemaphore (const VulkanCmdBatchDependency &dep);

		ND_ ECommandBufferType	GetCmdBufType ()		const	{ return ECommandBufferType::Primary_OneTimeSubmit; }
		ND_ EQueueType			GetQueueType ()			const	{ return _queueType; }
		ND_ FrameUID			GetFrameId ()			const	{ return _frameId; }
		ND_ uint				GetSubmitIndex ()		const	{ return _submitIdx; }
		ND_ uint				GetCmdBufIndex ()		const	{ return _cmdPool.Current(); }
		ND_ bool				IsSubmitted ();
		ND_ NtStringView		DbgName ()				const;


	// IDeviceToHostSync
	public:
		ND_ bool  Wait (nanoseconds timeout) override;
		ND_ bool  IsComplete () override;

		
	// render task scheduler api
	private:
		explicit VCommandBatch (uint indexInPool);

		ND_ bool  _Create (EQueueType queue, FrameUID frameId, uint submitIdx, StringView dbgName);
			void  _OnSubmit2 ();
			void  _OnComplete ();
			
		#if AE_VK_TIMELINE_SEMAPHORE
			void  _OnSubmit ();
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count);
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphoreSubmitInfoKHR const* &semInfos, OUT uint &count);
		#else
			void  _OnSubmit (RC<VirtualFence> fence);	
		ND_	bool  _GetWaitSemaphores   (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT VkPipelineStageFlags const* &stages, OUT uint &count);
		ND_	bool  _GetSignalSemaphores (VTempStackAllocator &, OUT VkSemaphore const* &sems, OUT uint &count);
		#endif


	// helper functions
	private:

		// CPU to CPU dependency
		ND_ bool  _AddOnCompleteDependency (AsyncTask task, uint index);
		ND_ bool  _AddOnSubmitDependency (AsyncTask task, uint index);

			void  _ReleaseObject () override;
	};
	


	//
	// Vulkan Render Task interface
	//

	class VRenderTask : public Threading::IAsyncTask
	{
		friend class VCommandBatch;

	// variables
	private:
		RC<VCommandBatch>	_batch;
		uint				_exeIndex	= UMax;		// execution order index
		
		DEBUG_ONLY(
			const String	_dbgName;
		)


	// methods
	public:
		VRenderTask (RC<VCommandBatch> batch, StringView dbgName) :
			IAsyncTask{ EThread::Renderer },
			_batch{ RVRef(batch) },
			_exeIndex{ _GetPool().Acquire() }
			DEBUG_ONLY(, _dbgName{ dbgName })
		{
			Unused( dbgName );
		}

		~VRenderTask ()
		{
			ASSERT( _exeIndex == UMax );
		}
		
		ND_ RC<VCommandBatch>	GetBatch ()				const	{ return _batch; }
		ND_ Ptr<VCommandBatch>	GetBatchPtr ()			const	{ return _batch.get(); }
		ND_ FrameUID			GetFrameId ()			const	{ return _batch->GetFrameId(); }
		ND_ uint				GetExecutionIndex ()	const	{ return _exeIndex; }


	// IAsyncTask
	public:
		void  OnCancel () override;

		DEBUG_ONLY( NtStringView  DbgName () const override final { return _dbgName; })


	protected:
		void  OnFailure ();

		template <typename CmdBufType>
		void  Execute (CmdBufType &cmdbuf);
		
		template <typename CmdBufType>
		ND_ bool  ExecuteAndSubmit (CmdBufType &cmdbuf, ESubmitMode mode = ESubmitMode::Deferred);

	private:
		ND_ VCommandBatch::CmdBufPool&  _GetPool ()		{ return _batch->_cmdPool; }
	};



	//
	// Vulkan Command Batch on Submit dependency
	//
	struct VCmdBatchOnSubmit
	{
		RC<VCommandBatch>	ptr;

		VCmdBatchOnSubmit () {}
		VCmdBatchOnSubmit (VCommandBatch &batch) : ptr{batch.GetRC()} {}
		VCmdBatchOnSubmit (VCommandBatch* batch) : ptr{batch} {}
		VCmdBatchOnSubmit (RC<VCommandBatch> batch) : ptr{RVRef(batch)} {}
	};
	
	
}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN