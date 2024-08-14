// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/BaseAppV1/AppConfig.h"
#include "platform/BaseAppV1/DefaultAppV1.h"
#include "graphics/Remote/RMessages.h"
#include "graphics/Remote/RConnection.h"

#ifdef AE_ENABLE_VULKAN
# include "VulkanSyncLog.h"
#endif

namespace AE::RemoteGraphics
{
	using namespace AE::Threading;
	using namespace AE::Graphics;
	using namespace AE::App;

	class RmGAppListener;


	//
	// Remote Graphics Device Window
	//
	class RmGWndListener final : public IWindow::IWndListener
	{
	private:
		RmGAppListener&		_device;

	public:
		RmGWndListener (RmGAppListener &dev)		__NE___ : _device{dev} {}
		~RmGWndListener ()							__NE_OV	{}

		void  OnStateChanged (IWindow &, EState)	__NE_OV;
		void  OnSurfaceDestroyed (IWindow &)		__NE_OV;
		void  OnSurfaceCreated (IWindow &)			__NE_OV;
	};



	//
	// Remote Graphics Device Application
	//
	class RmGAppListener final : public IApplication::IAppListener
	{
	// types
	private:
	  #if defined(AE_ENABLE_VULKAN)
		using StagingBufferMngr		= VStagingBufferManager;
		using GpuSemaphore			= VkSemaphore;
		using CmdBatchDependency	= VulkanCmdBatchDependency;
		using GpuQuery				= VkQueryPool;
		using Query					= VQueryManager::Query;

	  #elif defined(AE_ENABLE_METAL)
		using StagingBufferMngr		= MStagingBufferManager;
		using GpuSemaphore			= MetalEvent;
		using CmdBatchDependency	= MetalCmdBatchDependency;
		using GpuQuery				= MetalBuffer;

	  #else
	  #	error not implemented
	  #endif

		using Index_t = ushort;

		template <typename T>
		struct PtrAndGen
		{
			T					ptr;
			Atomic<Index_t>		gen	{0};
		};

		using GfxMemAllocatorPool_t		= LfStaticIndexedPool< PtrAndGen< GfxMemAllocatorPtr	 >,	Index_t, 64 >;
		using DescriptorAllocatorPool_t	= LfStaticIndexedPool< PtrAndGen< DescriptorAllocatorPtr >,	Index_t, 64 >;
		using RenderTechPipelinesPool_t	= LfStaticIndexedPool< PtrAndGen< RenderTechPipelinesPtr >,	Index_t, 64 >;

		using Callback_t				= TrivialStorage< sizeof(void*)*2, alignof(void*) >;
		using CallbackMap_t				= FlatHashMap< TypeId, Callback_t >;
		using EContextType				= IGraphicsProfiler::EContextType;


		struct alignas(AE_CACHE_LINE) PerThreadData
		{
			Atomic<bool>			looping	{false};
			StdThread				thread;
			RConnection				conn;
			Array<RC<SharedMem>>	memStack;
		};
		using ThreadArr_t = StaticArray< PerThreadData, RmNetConfig::socketCount >;


	public:
		struct CmdCtx
		{
		// types
			using GetDeviceMem_t	= Function< void* (RmDeviceOffset, Bytes size) __Th___ >;
			using AddHostMem_t		= Function< void (void*, RmHostPtr, Bytes size) __Th___ >;

		// variables
			EContextType				type	= Default;
			union {
				DirectCtx::Graphics		graphics;
				DirectCtx::Compute		compute;
				DirectCtx::RayTracing	rayTracing;
				DirectCtx::ASBuild		asBuild;
				DirectCtx::Transfer		transfer;
				char					_unused1	= 0;
			};
			union {
				DirectCtx::Draw			draw;
				char					_unused2	= 0;
			};
			DirectCtx::CommandBuffer	cmdbuf;
			Ptr<const RenderTask>		rtask;
			Ptr<const DrawTask>			dtask;

			GetDeviceMem_t				getMem;
			AddHostMem_t				addMem;

			RmGAppListener &			rmdev;

		// methods
			CmdCtx (RenderTask &task, RmGAppListener &rmdev) : rtask{&task}, rmdev{rmdev} {}
			CmdCtx (DrawTask &task, RmGAppListener &rmdev)   : dtask{&task}, rmdev{rmdev} {}
			~CmdCtx ();

			void  End () __Th___;

			ND_ IBaseContext&  GetBaseCtx () __Th___;
		};


	private:
		class RenderTaskImpl final : public Graphics::RenderTask
		{
		public:
			CmdCtx		ctx;

		public:
			RenderTaskImpl (RmGAppListener &rmdev, CommandBatchPtr batch, uint exeIndex) __NE___ :
				RenderTask{ RVRef(batch), CmdBufExeIndex::Exact(exeIndex), Default }, ctx{ *this, rmdev } {}

				void  Run ()			__Th_OV {}
				void  Execute ()		__Th___;
			ND_ bool  IsSubmitted ()	C_NE___	{ return not IsValid(); }
		};

		class DrawTaskImpl final : public Graphics::DrawTask
		{
		public:
			CmdCtx		ctx;

		public:
			DrawTaskImpl (RmGAppListener &rmdev, DrawCommandBatchPtr batch, uint drawIndex) __NE___ :
				DrawTask{ RVRef(batch), CmdBufExeIndex::Exact(drawIndex), Default }, ctx{ *this, rmdev } {}

				void  Run ()			__Th_OV {}
				void  Execute ()		__Th___;
			ND_ bool  IsSubmitted ()	C_NE___	{ return not IsValid(); }
		};

		using RenderTaskPool_t		= LfStaticIndexedPool< PtrAndGen< RC<RenderTaskImpl> >,		Index_t, 64 >;
		using CmdBatchPool_t		= LfStaticIndexedPool< PtrAndGen< CommandBatchPtr >,		Index_t, 64 >;

		using DrawTaskPool_t		= LfStaticIndexedPool< PtrAndGen< RC<DrawTaskImpl> >,		Index_t, 64 >;
		using DrawCmdBatchPool_t	= LfStaticIndexedPool< PtrAndGen< DrawCommandBatchPtr >,	Index_t, 64 >;

		using GpuSemaphorePool_t	= LfStaticIndexedPool< PtrAndGen< GpuSemaphore >,			Index_t, 256 >;
		using GpuSemaphoreMap_t		= Synchronized< SharedMutex, FlatHashMap< GpuSemaphore,  RmSemaphoreID >>;

		using GpuQueryPool_t		= LfStaticIndexedPool< PtrAndGen< GpuQuery >,				Index_t, 64 >;
		using GpuQueryMap_t			= Synchronized< SharedMutex, FlatHashMap< GpuQuery,  RmQueryID >>;

		struct DevToHostCopy
		{
			FrameUID									frameId;
			Array<Tuple< void*, RmHostPtr, Bytes >>		arr;
		};
		using DevToHostCopySync_t		= Threading::Synchronized< SharedMutex, DevToHostCopy >;
		using PerFrameDevToHostCopy_t	= StaticArray< DevToHostCopySync_t, GraphicsConfig::MaxFrames >;

		using DrawBatchMap_t			= FlatHashMap< RmDrawCommandBatchID, DrawCommandBatchPtr >;

		class LogToHost final : public ILogger
		{
		public:
			EResult  Process (const MessageInfo &info)	__Th_OV;
		};

		using SyncTimer_t = Synchronized< SpinLock, Timer >;


	// variables
	private:
	  #if defined(AE_ENABLE_VULKAN)
		Graphics::VDeviceInitializer		_device;
		Graphics::VSwapchainInitializer		_swapchain;
		Graphics::VulkanSyncLog				_syncLog;

	  #elif defined(AE_ENABLE_METAL)
		Graphics::MDeviceInitializer		_device;
		Graphics::MSwapchainInitializer		_swapchain;

	  #else
	  #	error not implemented
	  #endif

		Ptr<Graphics::ResourceManager>		_resMngr;	// TODO: atomic

		WindowPtr							_window;
		Ptr<IApplication>					_app;
		NativeWindow						_nativeWnd;
		StructAtomic<ushort2>				_windowSize;

		Serializing::ObjectFactory			_objFactory;
		Serializing::ObjectFactory			_cmdFactory;
		Serializing::ObjectFactory			_descFactory;

		CallbackMap_t						_cbMap;
		ThreadArr_t							_threadArr;
		Atomic<bool>						_restartServer	{false};
		Atomic<bool>						_hasConnection	{false};
		SyncTimer_t							_connectionLostTimer;

		PerFrameDevToHostCopy_t				_devToHostCopy;
		Atomic<uint>						_lastSubmitIdx	{0};

		struct {
			GfxMemAllocatorPool_t				gfxMemAllocPool;
			DescriptorAllocatorPool_t			dsAllocPool;
			RenderTechPipelinesPool_t			rtechPplnPool;

			RenderTaskPool_t					cmdbufPool;
			CmdBatchPool_t						batchPool;

			DrawTaskPool_t						drawCmdbufPool;
			GpuSemaphorePool_t					semaphorePool;
			GpuSemaphoreMap_t					semaphoreMap;

			DrawBatchMap_t						drawBatchMap;

			GpuQueryPool_t						queryPool;
			GpuQueryMap_t						queryMap;
		}									_res;

		struct {
		  #ifdef AE_ENABLE_ARM_PMU
			Profiler::ArmProfiler				arm;
		  #endif
		  #ifdef AE_ENABLE_MALI_HWCPIPE
			Profiler::MaliProfiler				mali;
		  #endif
		  #ifdef AE_ENABLE_ADRENO_PERFCOUNTER
			Profiler::AdrenoProfiler			adreno;
		  #endif
		  #ifdef AE_ENABLE_PVRCOUNTER
			Profiler::PowerVRProfiler					pvr;
			Profiler::PowerVRProfiler::TimeScopeArr_t	pvrTimings;
		  #endif
		  #ifdef AE_ENABLE_NVML
			Profiler::NVidiaProfiler			nv;
		  #endif
		}									_profilers;


	// methods
	public:
		RmGAppListener ()							__NE___;
		~RmGAppListener ()							__NE_OV;

		void  OnStart (IApplication &)				__NE_OV;
		void  BeforeWndUpdate (IApplication &)		__NE_OV;
		void  AfterWndUpdate (IApplication &)		__NE_OV;
		void  OnStop (IApplication &)				__NE_OV;

	private:
		friend class RmGWndListener;
		void  _StartServer ();
		void  _StopServer ();
		void  _RequireRestartServer ();
		void  _RestartServer ();

		void  _PrintSelfIP ();

		template <typename M, typename C>
		ND_ bool  _Register (Serializing::SerializedID::Ref id, void (C::*fn)(M &));

		template <typename M>
		ND_ bool  _Register (Serializing::SerializedID::Ref id, const Callback_t &fn);

		template <typename M>
		ND_ bool  _Register (Serializing::SerializedID::Ref id);

		ND_ bool  _RegisterMessages ();

			void  _ProcessMessage (Msg::BaseMsg &);

			void  _RunTests ();

	public:
		ND_ GfxMemAllocatorPtr		_Get (RmGfxMemAllocatorID);
		ND_ DescriptorAllocatorPtr	_Get (RmDescriptorAllocatorID);
		ND_ RenderTechPipelinesPtr	_Get (RmRenderTechPipelinesID);
		ND_ RC<RenderTaskImpl>		_Get (RmCommandBufferID);
		ND_ RC<DrawTaskImpl>		_Get (RmDrawCommandBufferID);
		ND_ CommandBatchPtr			_Get (RmCommandBatchID);
		ND_ DrawCommandBatchPtr		_Get (RmDrawCommandBatchID);
		ND_ GpuSemaphore			_GetSemaphore (RmSemaphoreID);
		ND_ GpuQuery				_GetQuery (RmQueryID);

		ND_ RmGfxMemAllocatorID		_Set (GfxMemAllocatorPtr);
		ND_ RmDescriptorAllocatorID	_Set (DescriptorAllocatorPtr);
		ND_ RmRenderTechPipelinesID	_Set (RenderTechPipelinesPtr);
		ND_ RmCommandBufferID		_Set (RC<RenderTaskImpl>);
		ND_ RmDrawCommandBufferID	_Set (RC<DrawTaskImpl>);
		ND_ RmCommandBatchID		_Set (CommandBatchPtr);
		ND_ RmSemaphoreID			_SetSemaphore (GpuSemaphore);
		ND_ RmQueryID				_SetQuery (GpuQuery);
			void					_Set (RmDrawCommandBatchID, DrawCommandBatchPtr) __Th___;

			void  _Remove (RmGfxMemAllocatorID);
			void  _Remove (RmDescriptorAllocatorID);
			void  _Remove (RmRenderTechPipelinesID);
			void  _Remove (RmCommandBufferID);
			void  _Remove (RmDrawCommandBufferID);
			void  _Remove (RmCommandBatchID);
			void  _Remove (RmDrawCommandBatchID);

			void  _Convert (const Msg::CmdBuf_Bake::Graphics_BeginRenderPass::SerRenderPassDesc &src, OUT RenderPassDesc &dst);

		ND_ Query  _UnpackQuery (const Msg::Query_Alloc_Response::SerQuery &);

	private:
		template <typename ID, typename T, typename P>	ND_ ID   _Set2 (T, P& pool);
		template <typename T, typename ID, typename P>	ND_ T    _Get2 (ID, P& pool);
		template <typename ID, typename P>				    void _Remove2 (ID, P& pool);

	private:
		ND_ static PerThreadData*&  _GetThreadData ();

		void  _Send (const Msg::BaseResponse &)					__Th___;
		void  _Send (const void *data, Bytes dataSize)			__Th___;
		void  _ReadReceived (OUT void* data, Bytes size)		__Th___;

		void  _PushMemStack (RC<SharedMem>)						__Th___;
		ND_ Array<RC<SharedMem>>	_GetMemStack ()				__Th___;

		void  _OnFrameComplete (FrameUID);

		template <typename RespType, typename CBID, typename TaskType, typename MsgType>
		void  _CmdBuf_Bake (const MsgType &);

		template <typename RespType, typename PplnID>
		void  _ResMngr_GetPipelineInfo (OUT RespType &res, PplnID id);

		template <typename RespType>
		void  _ResMngr_CleanupPipelineInfo (INOUT RespType &res);

		void  _RTS_WaitForSubmit (uint submitIdx);

		template <typename T, typename RespType, typename MsgType>
		void  _Query_GetTimestamp (const MsgType &);

		template <typename T, typename RespType, typename MsgType>
		void  _Query_GetTimestampCalibrated (const MsgType &);

	private:
		void  _Cb_UploadData (const Msg::UploadData &);

		void  _Cb_Device_Init (const Msg::Device_Init &);
		void  _Cb_Device_DestroyLogicalDevice (const Msg::Device_DestroyLogicalDevice &);
		void  _Cb_Device_DestroyInstance (const Msg::Device_DestroyInstance &);
		void  _Cb_Device_EnableSyncLog (const Msg::Device_EnableSyncLog &);
		void  _Cb_Device_GetSyncLog (const Msg::Device_GetSyncLog &);

		void  _Cb_Surface_Create (const Msg::Surface_Create &);
		void  _Cb_Surface_Destroy (const Msg::Surface_Destroy &);
		void  _Cb_Swapchain_Create (const Msg::Swapchain_Create &);
		void  _Cb_Swapchain_Destroy (const Msg::Swapchain_Destroy &);
		void  _Cb_Swapchain_IsSupported (const Msg::Swapchain_IsSupported &);
		void  _Cb_Swapchain_AcquireNextImage (const Msg::Swapchain_AcquireNextImage &);
		void  _Cb_Swapchain_Present (const Msg::Swapchain_Present &);

		void  _Cb_RTS_Initialize (const Msg::RTS_Initialize &);
		void  _Cb_RTS_Deinitialize (const Msg::RTS_Deinitialize &);
		void  _Cb_RTS_BeginFrame (const Msg::RTS_BeginFrame &);
		void  _Cb_RTS_EndFrame (const Msg::RTS_EndFrame &);
		void  _Cb_RTS_WaitNextFrame (const Msg::RTS_WaitNextFrame &);
		void  _Cb_RTS_WaitAll (const Msg::RTS_WaitAll &);
		void  _Cb_RTS_SkipCmdBatches (const Msg::RTS_SkipCmdBatches &);
		void  _Cb_RTS_CreateBatch (const Msg::RTS_CreateBatch &);
		void  _Cb_RTS_DestroyBatch (const Msg::RTS_DestroyBatch &);
		void  _Cb_RTS_SubmitBatch (const Msg::RTS_SubmitBatch &);
		void  _Cb_RTS_WaitBatch (const Msg::RTS_WaitBatch &);

		void  _Cb_ResMngr_CreateImage (const Msg::ResMngr_CreateImage &);
		void  _Cb_ResMngr_CreateImageView (const Msg::ResMngr_CreateImageView &);
		void  _Cb_ResMngr_CreateBuffer (const Msg::ResMngr_CreateBuffer &);
		void  _Cb_ResMngr_CreateBufferView (const Msg::ResMngr_CreateBufferView &);
		void  _Cb_ResMngr_CreateRTGeometry (const Msg::ResMngr_CreateRTGeometry &);
		void  _Cb_ResMngr_CreateRTScene (const Msg::ResMngr_CreateRTScene &);
		void  _Cb_ResMngr_GetRTGeometrySizes (const Msg::ResMngr_GetRTGeometrySizes &);
		void  _Cb_ResMngr_GetRTSceneSizes (const Msg::ResMngr_GetRTSceneSizes &);
		void  _Cb_ResMngr_IsSupported_BufferDesc (const Msg::ResMngr_IsSupported_BufferDesc &);
		void  _Cb_ResMngr_IsSupported_ImageDesc (const Msg::ResMngr_IsSupported_ImageDesc &);
		void  _Cb_ResMngr_IsSupported_VideoImageDesc (const Msg::ResMngr_IsSupported_VideoImageDesc &);
		void  _Cb_ResMngr_IsSupported_VideoBufferDesc (const Msg::ResMngr_IsSupported_VideoBufferDesc &);
		void  _Cb_ResMngr_IsSupported_VideoSessionDesc (const Msg::ResMngr_IsSupported_VideoSessionDesc &);
		void  _Cb_ResMngr_IsSupported_BufferViewDesc (const Msg::ResMngr_IsSupported_BufferViewDesc &);
		void  _Cb_ResMngr_IsSupported_ImageViewDesc (const Msg::ResMngr_IsSupported_ImageViewDesc &);
		void  _Cb_ResMngr_IsSupported_RTGeometryDesc (const Msg::ResMngr_IsSupported_RTGeometryDesc &);
		void  _Cb_ResMngr_IsSupported_RTGeometryBuild (const Msg::ResMngr_IsSupported_RTGeometryBuild &);
		void  _Cb_ResMngr_IsSupported_RTSceneDesc (const Msg::ResMngr_IsSupported_RTSceneDesc &);
		void  _Cb_ResMngr_IsSupported_RTSceneBuild (const Msg::ResMngr_IsSupported_RTSceneBuild &);
		void  _Cb_ResMngr_ReleaseResource (const Msg::ResMngr_ReleaseResource &);
		void  _Cb_ResMngr_CreateDescriptorSets2 (const Msg::ResMngr_CreateDescriptorSets2 &);
		void  _Cb_ResMngr_CreateDescriptorSets3 (const Msg::ResMngr_CreateDescriptorSets3 &);
		void  _Cb_ResMngr_CreateGraphicsPipeline (const Msg::ResMngr_CreateGraphicsPipeline &);
		void  _Cb_ResMngr_CreateMeshPipeline (const Msg::ResMngr_CreateMeshPipeline &);
		void  _Cb_ResMngr_CreateComputePipeline (const Msg::ResMngr_CreateComputePipeline &);
		void  _Cb_ResMngr_CreateRayTracingPipeline (const Msg::ResMngr_CreateRayTracingPipeline &);
		void  _Cb_ResMngr_CreateTilePipeline (const Msg::ResMngr_CreateTilePipeline &);
		void  _Cb_ResMngr_CreateVideoSession (const Msg::ResMngr_CreateVideoSession &);
		void  _Cb_ResMngr_CreateVideoBuffer (const Msg::ResMngr_CreateVideoBuffer &);
		void  _Cb_ResMngr_CreateVideoImage (const Msg::ResMngr_CreateVideoImage &);
		void  _Cb_ResMngr_CreatePipelineCache (const Msg::ResMngr_CreatePipelineCache &);
		void  _Cb_ResMngr_InitializeResources (const Msg::ResMngr_InitializeResources &);
		void  _Cb_ResMngr_LoadPipelinePack (const Msg::ResMngr_LoadPipelinePack &);
		void  _Cb_ResMngr_GetSupportedRenderTechs (const Msg::ResMngr_GetSupportedRenderTechs &);
		void  _Cb_ResMngr_LoadRenderTech (const Msg::ResMngr_LoadRenderTech &);
		void  _Cb_ResMngr_CreateLinearGfxMemAllocator (const Msg::ResMngr_CreateLinearGfxMemAllocator &);
		void  _Cb_ResMngr_CreateBlockGfxMemAllocator (const Msg::ResMngr_CreateBlockGfxMemAllocator &);
		void  _Cb_ResMngr_CreateUnifiedGfxMemAllocator (const Msg::ResMngr_CreateUnifiedGfxMemAllocator &);
		void  _Cb_ResMngr_GetRTechPipeline (const Msg::ResMngr_GetRTechPipeline &);
		void  _Cb_ResMngr_GetRenderPass (const Msg::ResMngr_GetRenderPass &);
		void  _Cb_ResMngr_ForceReleaseResources (const Msg::ResMngr_ForceReleaseResources &);
		void  _Cb_ResMngr_GetShaderGroupStackSize (const Msg::ResMngr_GetShaderGroupStackSize &);

		void  _Cb_Query_Init (const Msg::Query_Init &);
		void  _Cb_Query_Alloc (const Msg::Query_Alloc &);
		void  _Cb_Query_GetTimestampUL (const Msg::Query_GetTimestampUL &);
		void  _Cb_Query_GetTimestampD (const Msg::Query_GetTimestampD &);
		void  _Cb_Query_GetTimestampNs (const Msg::Query_GetTimestampNs &);
		void  _Cb_Query_GetTimestampCalibratedUL (const Msg::Query_GetTimestampCalibratedUL &);
		void  _Cb_Query_GetTimestampCalibratedD (const Msg::Query_GetTimestampCalibratedD &);
		void  _Cb_Query_GetTimestampCalibratedNs (const Msg::Query_GetTimestampCalibratedNs &);
		void  _Cb_Query_GetPipelineStatistic (const Msg::Query_GetPipelineStatistic &);

		void  _Cb_ProfArm_Initialize (const Msg::ProfArm_Initialize &);
		void  _Cb_ProfArm_Sample (const Msg::ProfArm_Sample &);

		void  _Cb_ProfMali_Initialize (const Msg::ProfMali_Initialize &);
		void  _Cb_ProfMali_Sample (const Msg::ProfMali_Sample &);

		void  _Cb_ProfAdreno_Initialize (const Msg::ProfAdreno_Initialize &);
		void  _Cb_ProfAdreno_Sample (const Msg::ProfAdreno_Sample &);

		void  _Cb_ProfPVR_Initialize (const Msg::ProfPVR_Initialize &);
		void  _Cb_ProfPVR_Tick (const Msg::ProfPVR_Tick &);
		void  _Cb_ProfPVR_Sample (const Msg::ProfPVR_Sample &);

		void  _Cb_ProfNVidia_Initialize (const Msg::ProfNVidia_Initialize &);
		void  _Cb_ProfNVidia_Sample (const Msg::ProfNVidia_Sample &);

		void  _Cb_DescUpd_Flush (const Msg::DescUpd_Flush &);

		void  _Cb_SBM_GetBufferRanges (const Msg::SBM_GetBufferRanges &);
		void  _Cb_SBM_GetImageRanges (const Msg::SBM_GetImageRanges &);
		void  _Cb_SBM_GetImageRanges2 (const Msg::SBM_GetImageRanges2 &);
		void  _Cb_SBM_AllocVStream (const Msg::SBM_AllocVStream &);
		void  _Cb_SBM_GetFrameStat (const Msg::SBM_GetFrameStat &);

		void  _Cb_CmdBuf_Bake (const Msg::CmdBuf_Bake &);
		void  _Cb_CmdBuf_BakeDraw (const Msg::CmdBuf_BakeDraw &);
	};


/*
=================================================
	_Register
=================================================
*/
	template <typename M, typename C>
	bool  RmGAppListener::_Register (Serializing::SerializedID::Ref id, void (C::*fn)(M &))
	{
		StaticAssert( sizeof(fn) >= sizeof(Callback_t) );

		Callback_t	fn_data;
		MemCopy( OUT fn_data.Data(), &fn, Sizeof(fn) );

		return _Register<M>( id, fn_data );
	}

	template <typename T>
	bool  RmGAppListener::_Register (Serializing::SerializedID::Ref id, const Callback_t &cb)
	{
		using M = RemoveConst<T>;
		StaticAssert( IsBaseOf< Msg::BaseMsg, M >);

		CHECK_ERR( _cbMap.emplace( TypeIdOf<M>(), cb ).second );

		CHECK_ERR( _objFactory.Register<M>( id ));
		return true;
	}

	template <typename T>
	bool  RmGAppListener::_Register (Serializing::SerializedID::Ref id)
	{
		using M = RemoveConst<T>;
		StaticAssert( IsBaseOf< Msg::BaseMsg, M >);

		CHECK_ERR( _objFactory.Register<M>( id ));
		return true;
	}


} // AE::RemoteGraphics
