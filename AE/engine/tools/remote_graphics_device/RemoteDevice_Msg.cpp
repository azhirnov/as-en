// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "remote_graphics_device/RemoteDevice.h"

namespace AE::RemoteGraphics
{
	static constexpr seconds	c_MaxTimeout	{30*60};	// for debugging
	static const EThreadArray	c_ThreadArr		{EThread::PerFrame, EThread::Renderer};


	void  RmGAppListener::_Cb_UploadData (const Msg::UploadData &msg)
	{
		auto	mem = SharedMem::Create( AE::GetDefaultAllocator(), msg.size );
		CHECK_THROW( mem );

		_ReadReceived( OUT mem->Data(), msg.size );
		_PushMemStack( RVRef(mem) );
	}

	void  RmGAppListener::_Cb_Device_Init (const Msg::Device_Init &msg)
	{
		const auto	Create = [this, &msg] () -> bool
		{{
			CHECK_ERR( _app != null );
			CHECK_ERR( _nativeWnd );
			CHECK_ERR( not _device.IsInitialized() );

			StaticLogger::ClearLoggers();
			StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );
			StaticLogger::AddLogger( ILogger::CreateIDEOutput() );
			StaticLogger::AddLogger( Unique<ILogger>{ new LogToHost{} });
		//	StaticLogger::AddLogger( ILogger::CreateBreakOnError() );
		//	StaticLogger::AddLogger( ILogger::CreateDialogOutput() );

		  #if defined(AE_ENABLE_VULKAN)
		  # if not AE_VK_TIMELINE_SEMAPHORE
			ConstCast( msg.info ).device.optionalQueues = Default;
		  # endif

			CHECK_ERR( _device.Init( msg.info, _app->GetVulkanInstanceExtensions() ));

		  #elif defined(AE_ENABLE_METAL)
			CHECK_ERR( _device.Init( msg.info ));

		  #else
		  #	error not implemented
		  #endif

			return true;
		}};

		using EFeature = Msg::Device_Init_Response::EFeature;

		Msg::Device_Init_Response	res;
		if ( Create() )
		{
			res.engineVersion			= { AE_VERSION.Get<0>(), AE_VERSION.Get<1>(), AE_VERSION.Get<2>() };
			res.name					= _device.GetDeviceName();
			res.props					= _device.GetDeviceProperties();
			res.resFlags				= _device.GetResourceFlags();
			res.memInfo					= _device.GetMemoryInfo();
			res.checkConstantLimitsOK	= _device.CheckConstantLimits();
			res.checkExtensionsOK		= _device.CheckExtensions();
			res.initialized				= _device.IsInitialized();
			res.underDebugger			= _device.IsUnderDebugger();
			res.adapterType				= _device.AdapterType();
			res.cpuArch					= CpuArchInfo::Get().cpu.arch;
			res.os						= PlatformUtils::GetOSType();

			for (const auto [q, i] : WithIndex( _device.GetQueues() ))
			{
				res.queues[i].type							= q.type;
				res.queues[i].minImageTransferGranularity	= q.minImageTransferGranularity;
			}
			_device.InitFeatureSet( OUT res.fs );

		  #if defined(AE_ENABLE_VULKAN)
			res.api = EGraphicsAPI::Vulkan;

			{
				auto&	feats		= _device.GetVProperties().features;
				auto&	rt_feats	= _device.GetVProperties().rayTracingPipelineFeats;
				auto&	rt1_feats	= _device.GetVProperties().rayTracingMaintenance1Feats;
				auto&	as_feats	= _device.GetVProperties().accelerationStructureFeats;
				auto&	fs_feats	= _device.GetVProperties().fragShadingRateFeats;
				auto&	ext			= _device.GetVExtensions();

				if ( feats.depthBounds )								 res.features.insert( EFeature::DepthBounds );

				res.features.insert( EFeature::StencilCompareMask );
				res.features.insert( EFeature::StencilWriteMask );

				if ( fs_feats.attachmentFragmentShadingRate )			res.features.insert( EFeature::FragmentShadingRate );
				if ( ext.drawIndirectCount )							res.features.insert( EFeature::DrawIndirectCount );
				if ( ext.drawIndirectCount )							res.features.insert( EFeature::DrawIndexedIndirectCount );
				if ( ext.meshShader )									res.features.insert( EFeature::DrawMeshTasksIndirectCount );

				res.features.insert( EFeature::ClearColorImage );
				res.features.insert( EFeature::ClearDepthStencilImage );
				res.features.insert( EFeature::ResolveImage );

				if ( rt_feats.rayTracingPipelineTraceRaysIndirect )		res.features.insert( EFeature::TraceRaysIndirect_DevAddr );
				if ( rt1_feats.rayTracingPipelineTraceRaysIndirect2 )	res.features.insert( EFeature::TraceRaysIndirect2_DevAddr );

				if ( as_feats.accelerationStructure )					res.features.insert( EFeature::SerializeToMemory );
				if ( as_feats.accelerationStructure )					res.features.insert( EFeature::DeserializeFromMemory );
				if ( as_feats.accelerationStructureIndirectBuild )		res.features.insert( EFeature::BuildIndirect );

				res.features.insert( EFeature::WriteTimestamp );

				if ( ext.timelineSemaphore )							res.features.insert( EFeature::TimelineSemaphore );
				if ( ext.hostQueryReset )								res.features.insert( EFeature::HostQueryReset );

				StaticAssert( uint(EFeature::_Count) == 19 );
			}
		  #elif defined(AE_ENABLE_METAL)
			res.api = EGraphicsAPI::Metal;

		  #else
		  #	error not implemented
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			if ( msg.initSyncLog )
			{
				FlatHashMap<VkQueue, String>	qnames;
				for (auto& q : _device.GetQueues()) {
					qnames.emplace( q.handle, String{q.debugName} );
				}
				_syncLog.Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
			}
		  #endif
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_Device_DestroyLogicalDevice (const Msg::Device_DestroyLogicalDevice &)
	{
		Msg::DefaultResponse	res;
	  #ifdef AE_ENABLE_VULKAN
		_syncLog.Deinitialize( INOUT _device.EditDeviceFnTable() );
	  #endif

		CHECK( not _swapchain.IsInitialized() );
		_device.DestroyLogicalDevice();

		res.ok = true;
		_Send( res );
	}

	void  RmGAppListener::_Cb_Device_DestroyInstance (const Msg::Device_DestroyInstance &)
	{
		_device.DestroyInstance();

		Msg::DefaultResponse	res;
		res.ok = true;
		_Send( res );

		_RequireRestartServer();
	}

	void  RmGAppListener::_Cb_Device_EnableSyncLog (const Msg::Device_EnableSyncLog &msg)
	{
	  #ifdef AE_ENABLE_VULKAN
		if ( msg.enable )
			_syncLog.Enable();
		else
			_syncLog.Disable();
	  #endif
	}

	void  RmGAppListener::_Cb_Device_GetSyncLog (const Msg::Device_GetSyncLog &)
	{
		const usize	max_size = 0xFFFF;

		Msg::Device_GetSyncLog_Response		res;

	  #ifdef AE_ENABLE_VULKAN
		_syncLog.GetLog( OUT res.log );
	  #endif

		if ( res.log.size() >= max_size )
			res.log.resize( max_size );

		_Send( res );
	}
//-----------------------------------------------------------------------------


	void  RmGAppListener::_Cb_Surface_Create (const Msg::Surface_Create &)
	{
		Msg::Surface_Create_Response	res;
		if ( _swapchain.CreateSurface( _nativeWnd ))
		{
			res.formats.resize( _swapchain.GetSurfaceFormats( null, 0 ));
			res.presentModes.resize( _swapchain.GetPresentModes( null, 0 ));

			Unused( _swapchain.GetSurfaceFormats( OUT res.formats.data(), res.formats.size() ));
			Unused( _swapchain.GetPresentModes( OUT res.presentModes.data(), res.presentModes.size() ));

			res.created = true;

			if ( _resMngr != null )
				CHECK( _resMngr->OnSurfaceCreated( _swapchain ));
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_Surface_Destroy (const Msg::Surface_Destroy &)
	{
		_swapchain.DestroySurface();
	}

	void  RmGAppListener::_Cb_Swapchain_Create (const Msg::Swapchain_Create &msg)
	{
		SwapchainDesc	desc = msg.desc;
		desc.usage |= EImageUsage::TransferSrc;

		Msg::Swapchain_Create_Response	res;
		if_likely( _swapchain.Create( msg.viewSize, desc, msg.dbgName ))
		{
			res.viewSize	= _swapchain.GetSurfaceSize();
			res.desc		= _swapchain.GetDescription();
			res.created		= true;
			res.minimized	= false;

		  #if defined(AE_ENABLE_VULKAN)
			VSwapchain::Semaphores_t	image_available = {};
			VSwapchain::Semaphores_t	render_finished = {};
			_swapchain.GetSemaphores( OUT image_available, OUT render_finished );

			for (uint i = 0; i < GraphicsConfig::MaxSwapchainLength; ++i)
			{
				res.imageAvailable[i] = _SetSemaphore( image_available[i] );
				res.renderFinished[i] = _SetSemaphore( render_finished[i] );
			}

			uint						image_count;
			VSwapchain::WeakImageIDs_t	image_ids	= {};
			VSwapchain::WeakViewIDs_t	view_ids	= {};
			_swapchain.GetImages( OUT image_ids, OUT view_ids, OUT image_count );

			res.imageIds	= ArrayView{ Cast<RmImageID>(image_ids.data()),		image_count };
			res.viewIds		= ArrayView{ Cast<RmImageViewID>(view_ids.data()),	image_count };

			return _Send( res );

		  #elif defined(AE_ENABLE_METAL)
			UNTESTED;

		  #endif
		}
		else
		{
			res.viewSize	= uint2{_windowSize.load()};
			res.minimized	= All( res.viewSize == uint2{0} );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_Swapchain_Destroy (const Msg::Swapchain_Destroy &)
	{
		_swapchain.Destroy();
	}

	void  RmGAppListener::_Cb_Swapchain_IsSupported (const Msg::Swapchain_IsSupported &msg)
	{
		Msg::DefaultResponse	res;
		res.ok = _swapchain.IsSupported( msg.presentMode, msg.colorFormat, msg.colorImageUsage );
		_Send( res );
	}

	void  RmGAppListener::_Cb_Swapchain_AcquireNextImage (const Msg::Swapchain_AcquireNextImage &)
	{
		using EAcquireResult = Msg::Swapchain_AcquireNextImage_Response::EAcquireResult;
		Msg::Swapchain_AcquireNextImage_Response  res;

		const uint2	wnd_size = uint2{_windowSize.load()};

		if_unlikely( All( wnd_size == Zero ))
		{
			res.result = EAcquireResult::Error_RecreateImmediately;
			return _Send( res );
		}

		if_unlikely( Any( wnd_size != _swapchain.GetSurfaceSize() ))
		{
			res.result = EAcquireResult::Error_RecreateImmediately;
			return _Send( res );
		}

	  #if defined(AE_ENABLE_VULKAN)
		VkResult	err = _swapchain.AcquireNextImage();
		switch ( err )
		{
			case_likely VK_SUCCESS :
				res.result = EAcquireResult::OK;  break;

			case VK_SUBOPTIMAL_KHR :
			{
				auto	q = _device.GetQueue( EQueueType::Graphics );
				{
					VkPipelineStageFlags	wait_dst	= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
					VkSemaphore				wait_sem	= _swapchain.GetImageAvailableSemaphore();
					VkSemaphore				signal_sem	= _swapchain.GetRenderFinishedSemaphore();
					VkSubmitInfo			submit		= {};

					submit.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
					submit.waitSemaphoreCount	= 1;
					submit.pWaitSemaphores		= &wait_sem;
					submit.pWaitDstStageMask	= &wait_dst;
					submit.signalSemaphoreCount	= 1;
					submit.pSignalSemaphores	= &signal_sem;

					EXLOCK( q->guard );
					VK_CHECK( _device.vkQueueSubmit( q->handle, 1, &submit, Default ));
				}
				VK_CHECK( _swapchain.Present( q ));
				res.result = EAcquireResult::Error_RecreateImmediately;
				break;
			}

			case VK_ERROR_OUT_OF_DATE_KHR :
				res.result = EAcquireResult::Error_RecreateImmediately;  break;

			case VK_TIMEOUT :
			case VK_NOT_READY :
			case VK_ERROR_SURFACE_LOST_KHR :
			default :
				res.result = EAcquireResult::Error;  break;
		}

	  #elif defined(AE_ENABLE_METAL)
		UNTESTED;

	  #endif

		res.semaphoreId	= ushort(_swapchain.GetCurrentSemaphoreIndex());
		res.imageIdx	= ushort(_swapchain.GetCurrentImageIndex());

		_Send( res );
	}

	void  RmGAppListener::_Cb_Swapchain_Present (const Msg::Swapchain_Present &msg)
	{
		using EPresentResult = Msg::Swapchain_Present_Response::EPresentResult;
		Msg::Swapchain_Present_Response  res;

		CHECK_THROW( msg.imageIdx == _swapchain.GetCurrentImageIndex() );
		CHECK_THROW( msg.semaphoreId == _swapchain.GetCurrentSemaphoreIndex() );

		_RTS_WaitForSubmit( msg.submitIdx );

	  #if defined(AE_ENABLE_VULKAN)

		auto		q	= GraphicsScheduler().GetDevice().GetQueue( msg.presentQueue );
		VkResult	err = _swapchain.Present( q );
		switch ( err )
		{
			case_likely VK_SUCCESS :
				res.result = EPresentResult::OK;  break;

			case VK_ERROR_SURFACE_LOST_KHR :
			case VK_ERROR_OUT_OF_DATE_KHR :
				res.result = EPresentResult::Error_RecreateImmediately;  break;

			case VK_SUBOPTIMAL_KHR :
			  #ifdef AE_PLATFORM_ANDROID
				res.result = EPresentResult::OK;	// TODO
			  #else
				res.result = EPresentResult::OK_RecreateLater;
			  #endif
				break;

			default :
				res.result = EPresentResult::Error;  break;
		}

	  #elif defined(AE_ENABLE_METAL)
		UNTESTED;

	  #endif

		_Send( res );
	}
//-----------------------------------------------------------------------------


	void  RmGAppListener::_Cb_RTS_Initialize (const Msg::RTS_Initialize &msg)
	{
		RenderTaskScheduler::InstanceCtor::Create( _device );

		Msg::DefaultResponse	res;
		res.ok = GraphicsScheduler().Initialize( msg.info );
		if ( res.ok )
		{
			_resMngr = &GraphicsScheduler().GetResourceManager();

			if ( _swapchain.GetVkSurface() != Default )
				CHECK( _resMngr->OnSurfaceCreated( _swapchain ));
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_RTS_Deinitialize (const Msg::RTS_Deinitialize &)
	{
		_resMngr = null;
		GraphicsScheduler().Deinitialize();
		RenderTaskScheduler::InstanceCtor::Destroy();
	}

	void  RmGAppListener::_Cb_RTS_BeginFrame (const Msg::RTS_BeginFrame &msg)
	{
		_lastSubmitIdx.store( 0 );

		Msg::DefaultResponse	res;

		auto&	rts = GraphicsScheduler();
		res.ok = rts.BeginFrame( msg.cfg );

		if ( res.ok )
		{
			CHECK_THROW( rts.GetFrameId() == msg.frameId );

			auto&	sync = _devToHostCopy[ msg.frameId.Index() ];
			sync->frameId = msg.frameId;
		}
		_Send( res );

	  #ifdef AE_ENABLE_PVRCOUNTER
		_profilers.pvr.ReadTimingData( OUT _profilers.pvrTimings );
	  #endif
	}

	void  RmGAppListener::_Cb_RTS_EndFrame (const Msg::RTS_EndFrame &msg)
	{
		_RTS_WaitForSubmit( msg.submitIdx );

		auto&	rts		= GraphicsScheduler();
		auto	task	= rts.EndFrame();

		Msg::DefaultResponse	res;
		res.ok = Scheduler().Wait( {task}, c_ThreadArr, c_MaxTimeout );

		if ( res.ok )
		{
			CHECK_THROW( rts.GetFrameId() == msg.frameId );
		}
		_Send( res );
	}

	void  RmGAppListener::_OnFrameComplete (FrameUID frameId)
	{
		auto	dev_to_host = _devToHostCopy[ frameId.Index() ].WriteLock();

		CHECK(	not dev_to_host->frameId.IsValid()			or
				dev_to_host->frameId.NextCycle() == frameId );

		for (auto& cmd : dev_to_host->arr)
		{
			Msg::UploadDataAndCopy	upload;
			upload.size	= cmd.Get<Bytes>();
			upload.dst	= cmd.Get<RmHostPtr>();
			upload.data	= cmd.Get<void*>();
			_Send( upload );
		}
		dev_to_host->arr.clear();
	}

	void  RmGAppListener::_Cb_RTS_WaitNextFrame (const Msg::RTS_WaitNextFrame &msg)
	{
		Msg::RTS_WaitNextFrame_Response	res;

		auto&	rts = GraphicsScheduler();
		res.ok = rts.WaitNextFrame( c_ThreadArr, msg.timeout );

		if ( res.ok )
		{
			CHECK_THROW( rts.GetFrameId().Inc() == msg.frameId );
			_OnFrameComplete( msg.frameId );

			res.memUsage = rts.GetDevice().GetMemoryUsage();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_RTS_WaitAll (const Msg::RTS_WaitAll &msg)
	{
		Msg::DefaultResponse	res;

		auto&	rts = GraphicsScheduler();
		res.ok = rts.WaitAll( c_ThreadArr, msg.timeout );

		if ( res.ok )
		{
			CHECK_THROW( rts.GetFrameId() == msg.frameId );

			FrameUID	fid = msg.frameId;
			for (uint i = 1; i < msg.frameId.MaxFrames(); ++i)
			{
				fid.Inc();
				_OnFrameComplete( fid );
			}
			fid.Inc();
			_OnFrameComplete( fid );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_RTS_SkipCmdBatches (const Msg::RTS_SkipCmdBatches &msg)
	{
		Msg::DefaultResponse	res;
		GraphicsScheduler().SkipCmdBatches( msg.queue, msg.bits );
		res.ok = true;
		_Send( res );
	}

	void  RmGAppListener::_Cb_RTS_CreateBatch (const Msg::RTS_CreateBatch &msg)
	{
		Msg::RTS_CreateBatch_Response	res;

		auto	batch = GraphicsScheduler().BeginCmdBatch( CmdBatchDesc{ msg.queue, msg.submitIdx, msg.dbgLabel, null, msg.flags });
		CHECK( batch );
		if ( batch )
		{
			#ifdef AE_ENABLE_VULKAN
			# if AE_VK_TIMELINE_SEMAPHORE
				res.semaphoreId	= _SetSemaphore( batch->GetSemaphore().semaphore );
			# endif
			#else
				res.semaphoreId	= _SetSemaphore( batch->GetSemaphore().semaphore );
			#endif

			res.batchId = _Set( RVRef(batch) );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_RTS_DestroyBatch (const Msg::RTS_DestroyBatch &msg)
	{
		_Remove( msg.batchId );
	//	_Remove( msg.semaphoreId );		// batch may be released when it in use
	}

	void  RmGAppListener::_Cb_RTS_SubmitBatch (const Msg::RTS_SubmitBatch &msg)
	{
		const auto	Run = [&] () -> bool
		{{
			for (auto cmdbuf : msg.cmdbufs)
			{
				auto	task = _Get( cmdbuf );
				CHECK_ERR( task and task->IsSubmitted() );
			}

			auto	batch = _Get( msg.id );
			CHECK_ERR( batch );
			CHECK_ERR( not batch->IsSubmitted() );

			for (auto& [sem, val] : msg.inputDeps)
			{
				CmdBatchDependency	dep;
				dep.semaphore	= _GetSemaphore( sem );
				dep.value		= val;
				CHECK_ERR( batch->AddInputSemaphore( dep ));
			}
			for (auto& [sem, val] : msg.outputDeps)
			{
				CmdBatchDependency	dep;
				dep.semaphore	= _GetSemaphore( sem );
				dep.value		= val;
				CHECK_ERR( batch->AddOutputSemaphore( dep ));
			}

			CHECK_ERR( batch->EndRecordingAndSubmit() );

			for (auto cmdbuf : msg.cmdbufs) {
				_Remove( cmdbuf );
			}
			return true;
		}};

		_RTS_WaitForSubmit( msg.submitIdx );

		Msg::DefaultResponse	res;
		res.ok = Run();
		_Send( res );
	}

	void  RmGAppListener::_Cb_RTS_WaitBatch (const Msg::RTS_WaitBatch &msg)
	{
		const auto	Run = [&] () -> bool
		{{
			auto	batch = _Get( msg.id );
			CHECK_ERR( batch );
			return batch->Wait( msg.timeout );
		}};

		Msg::DefaultResponse	res;
		res.ok = Run();
		_Send( res );
	}

	void  RmGAppListener::_RTS_WaitForSubmit (const uint submitIdx)
	{
		for (;;)
		{
			uint	expected = submitIdx-1;

			if ( _lastSubmitIdx.CAS( INOUT expected, submitIdx ))
				return;

			ASSERT_LE( expected, submitIdx );
			ThreadUtils::Sleep_500us();
		}
	}
//-----------------------------------------------------------------------------


	void  RmGAppListener::_Cb_ResMngr_CreateImage (const Msg::ResMngr_CreateImage &msg)
	{
		Msg::ResMngr_CreateImage_Response	res;
		if ( auto id = _resMngr->CreateImage( msg.desc, msg.dbgName, _Get(msg.gfxAlloc) ))
		{
			auto&	img = _resMngr->GetResourcesOrThrow( id.Get() );

			res.imageId		= RmCast( id.Release() );
			res.memoryId	= RmCast( img.MemoryId() );
			res.desc		= img.Description();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateImageView (const Msg::ResMngr_CreateImageView &msg)
	{
		Msg::ResMngr_CreateImageView_Response	res;
		if ( auto id = _resMngr->CreateImageView( msg.desc, RmCast(msg.imageId), msg.dbgName ))
		{
			auto&	view = _resMngr->GetResourcesOrThrow( id.Get() );

			res.viewId	= RmCast( id.Release() );
			res.desc	= view.Description();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateBuffer (const Msg::ResMngr_CreateBuffer &msg)
	{
		Msg::ResMngr_CreateBuffer_Response	res;
		if ( auto id = _resMngr->CreateBuffer( msg.desc, msg.dbgName, _Get(msg.gfxAlloc) ))
		{
			auto&	buf		= _resMngr->GetResourcesOrThrow( id.Get() );
			auto&	mem		= _resMngr->GetResourcesOrThrow( buf.MemoryId() );

			ResourceManager::NativeMemObjInfo_t	mem_info;
			CHECK( mem.GetMemoryInfo( OUT mem_info ));

			res.bufferId	= RmCast( id.Release() );
			res.memoryId	= RmCast( buf.MemoryId() );
			res.addr		= buf.HasDeviceAddress() ? buf.GetDeviceAddress() : Default;
			res.desc		= buf.Description();
			res.mapped		= BitCastRlx<RmDevicePtr>( mem_info.mappedPtr );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateBufferView (const Msg::ResMngr_CreateBufferView &msg)
	{
		Msg::ResMngr_CreateBufferView_Response	res;
		if ( auto id = _resMngr->CreateBufferView( msg.desc, RmCast(msg.bufferId), msg.dbgName ))
		{
			auto&	view = _resMngr->GetResourcesOrThrow( id.Get() );

			res.viewId	= RmCast( id.Release() );
			res.desc	= view.Description();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateRTGeometry (const Msg::ResMngr_CreateRTGeometry &msg)
	{
		Msg::ResMngr_CreateRTGeometry_Response	res;
		if ( auto id = _resMngr->CreateRTGeometry( msg.desc, msg.dbgName, _Get(msg.gfxAlloc) ))
		{
			auto&	geom	= _resMngr->GetResourcesOrThrow( id.Get() );

			res.geomId		= RmCast( id.Release() );
			res.memoryId	= RmCast( geom.MemoryId() );
			res.desc		= geom.Description();
			res.addr		= geom.GetDeviceAddress();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateRTScene (const Msg::ResMngr_CreateRTScene &msg)
	{
		Msg::ResMngr_CreateRTScene_Response	res;
		if ( auto id = _resMngr->CreateRTScene( msg.desc, msg.dbgName, _Get(msg.gfxAlloc) ))
		{
			auto&	scene	= _resMngr->GetResourcesOrThrow( id.Get() );

			res.sceneId		= RmCast( id.Release() );
			res.memoryId	= RmCast( scene.MemoryId() );
			res.desc		= scene.Description();
			res.addr		= scene.GetDeviceAddress();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_GetRTGeometrySizes (const Msg::ResMngr_GetRTGeometrySizes &msg)
	{
		Msg::ResMngr_GetRTGeometrySizes_Response	res;
		res.sizes = _resMngr->GetRTGeometrySizes( msg.desc );	// TODO: RmCast
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_GetRTSceneSizes (const Msg::ResMngr_GetRTSceneSizes &msg)
	{
		Msg::ResMngr_GetRTSceneSizes_Response	res;
		res.sizes = _resMngr->GetRTSceneSizes( msg.desc );	// TODO: RmCast
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_BufferDesc (const Msg::ResMngr_IsSupported_BufferDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_ImageDesc (const Msg::ResMngr_IsSupported_ImageDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_VideoImageDesc (const Msg::ResMngr_IsSupported_VideoImageDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_VideoBufferDesc (const Msg::ResMngr_IsSupported_VideoBufferDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_VideoSessionDesc (const Msg::ResMngr_IsSupported_VideoSessionDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_BufferViewDesc (const Msg::ResMngr_IsSupported_BufferViewDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( RmCast(msg.bufferId), msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_ImageViewDesc (const Msg::ResMngr_IsSupported_ImageViewDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( RmCast(msg.imageId), msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_RTGeometryDesc (const Msg::ResMngr_IsSupported_RTGeometryDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_RTGeometryBuild (const Msg::ResMngr_IsSupported_RTGeometryBuild &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_RTSceneDesc (const Msg::ResMngr_IsSupported_RTSceneDesc &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_IsSupported_RTSceneBuild (const Msg::ResMngr_IsSupported_RTSceneBuild &msg)
	{
		Msg::ResMngr_IsSupported_Response	res;
		res.supported = _resMngr->IsSupported( msg.desc );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_ReleaseResource (const Msg::ResMngr_ReleaseResource &msg)
	{
		using Types = Msg::ResMngr_ReleaseResource::Types;
		switch ( msg.index )
		{
			#define CASE( _type_ )	case Types::Index<Rm##_type_> : { Strong<_type_>  id {BitCastRlx<_type_>(msg.id)};  _resMngr->ImmediatelyRelease( id );  break; }
			CASE( ImageID )
			CASE( BufferID )
			CASE( ImageViewID )
			CASE( BufferViewID )
			CASE( RTGeometryID )
			CASE( RTSceneID )
			CASE( DescriptorSetID )
			CASE( RayTracingPipelineID )
			CASE( TilePipelineID )
			CASE( ComputePipelineID )
			CASE( GraphicsPipelineID )
			CASE( MeshPipelineID )
			CASE( PipelinePackID )
			CASE( PipelineCacheID )
			#undef CASE

			case Types::Index<RmGfxMemAllocatorID> :		_Remove( BitCastRlx<RmGfxMemAllocatorID>( msg.id ));			break;
			case Types::Index<RmDescriptorAllocatorID> :	_Remove( BitCastRlx<RmDescriptorAllocatorID>( msg.id ));		break;
			case Types::Index<RmRenderTechPipelinesID> :	_Remove( BitCastRlx<RmRenderTechPipelinesID>( msg.id ));		break;

			default :										CHECK_MSG( false, "unknown resource type" );					break;
		}
		StaticAssert( Types::Count == 17 );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateDescriptorSets2 (const Msg::ResMngr_CreateDescriptorSets2 &msg)
	{
		Msg::ResMngr_CreateDescriptorSets_Response	res;
		Array< Strong<DescriptorSetID> >			ds_arr;
		ds_arr.resize( msg.count );

		if ( _resMngr->CreateDescriptorSets( OUT ds_arr.data(), ds_arr.size(), RmCast(msg.packId), DSLayoutName{msg.dslName}, _Get(msg.dsAlloc), msg.dbgName ))
		{
			res.ds.resize( ds_arr.size() );
			for (usize i = 0; i < ds_arr.size(); ++i)
				res.ds[i] = RmCast( ds_arr[i].Release() );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateDescriptorSets3 (const Msg::ResMngr_CreateDescriptorSets3 &msg)
	{
		Msg::ResMngr_CreateDescriptorSets_Response	res;
		Array< Strong<DescriptorSetID> >			ds_arr;
		ds_arr.resize( msg.count );

		if ( _resMngr->CreateDescriptorSets( OUT ds_arr.data(), ds_arr.size(), RmCast(msg.layoutId), _Get(msg.dsAlloc), msg.dbgName ))
		{
			res.ds.resize( ds_arr.size() );
			for (usize i = 0; i < ds_arr.size(); ++i)
				res.ds[i] = RmCast( ds_arr[i].Release() );
		}
		_Send( res );
	}

	template <typename RespType, typename PplnID>
	void  RmGAppListener::_ResMngr_GetPipelineInfo (OUT RespType &res, PplnID id)
	{
		auto&	ppln = _resMngr->GetResourcesOrThrow( id );

		res.pplnId				= RmCast( id );
		res.dynamicState		= ppln.DynamicState();
		res.options				= ppln.Options();
		res.pipelineLayoutId	= RmCast( ppln.LayoutId() );

		if constexpr( IsSameTypes< PplnID, ComputePipelineID > or IsSameTypes< PplnID, TilePipelineID >)
		{
			res.shaderTrace.reset( ppln.GetShaderTrace().get() );
		}else{
			for (auto ptr : ppln.GetShaderTrace())
				res.shaderTrace.emplace_back( ptr.get() );
		}

		if constexpr( IsSameTypes< PplnID, GraphicsPipelineID >)
		{
			res.topology		= ppln.Topology();
			res.subpassIndex	= ubyte(ppln.RenderPassSubpassIndex());
			res.vertexBuffers	= ppln.GetVertexBufferMap();
		}
		if constexpr( IsSameTypes< PplnID, MeshPipelineID >)
		{
			res.meshLocalSize	= ushort3{ppln.MeshLocalSize()};
			res.taskLocalSize	= ushort3{ppln.TaskLocalSize()};
			res.subpassIndex	= ubyte(ppln.RenderPassSubpassIndex());
		}
		if constexpr( IsSameTypes< PplnID, ComputePipelineID >)
		{
			res.localSize		= ppln.LocalSize();
		}
		if constexpr( IsSameTypes< PplnID, TilePipelineID >)
		{
			res.localSize		= ushort2{ppln.LocalSize()};
			res.subpassIndex	= ubyte(ppln.RenderPassSubpassIndex());
		}
		if constexpr( IsSameTypes< PplnID, RayTracingPipelineID >)
		{
			res.shaderGroupHandleSize = Bytes{_resMngr->GetDevice().GetVProperties().rayTracingPipelineProps.shaderGroupHandleSize};
			res.groupHandles.assign( ppln._GroupHandlesData().begin(), ppln._GroupHandlesData().end() );

			res.nameToHandle.reserve( ppln._NameToHandleMap().size() );
			for (auto& [key, val] : ppln._NameToHandleMap())
				res.nameToHandle.emplace( key, val );
		}
	}

	template <typename RespType>
	void  RmGAppListener::_ResMngr_CleanupPipelineInfo (INOUT RespType &res)
	{
		// Unique<> required for deserializer, but here Unique<> doesn't own a pointer and destructor must never be used

		if constexpr( IsSameTypes< RespType, Msg::ResMngr_CreateComputePipeline_Response > or
					  IsSameTypes< RespType, Msg::ResMngr_CreateTilePipeline_Response >)
		{
			Unused( res.shaderTrace.release() );
		}
		else
		{
			for (auto& ptr : res.shaderTrace)
				Unused( ptr.release() );
			res.shaderTrace.clear();
		}
	}

	void  RmGAppListener::_Cb_ResMngr_CreateGraphicsPipeline (const Msg::ResMngr_CreateGraphicsPipeline &msg)
	{
		Msg::ResMngr_CreateGraphicsPipeline_Response	res;
		if ( auto id = _resMngr->CreateGraphicsPipeline( RmCast(msg.packId), PipelineTmplName{msg.name}, msg.desc, RmCast(msg.cacheId) ))
		{
			_ResMngr_GetPipelineInfo( OUT res, id.Release() );
		}
		_Send( res );
		_ResMngr_CleanupPipelineInfo( INOUT res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateMeshPipeline (const Msg::ResMngr_CreateMeshPipeline &msg)
	{
		Msg::ResMngr_CreateMeshPipeline_Response	res;
		if ( auto id = _resMngr->CreateMeshPipeline( RmCast(msg.packId), PipelineTmplName{msg.name}, msg.desc, RmCast(msg.cacheId) ))
		{
			_ResMngr_GetPipelineInfo( OUT res, id.Release() );
		}
		_Send( res );
		_ResMngr_CleanupPipelineInfo( INOUT res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateComputePipeline (const Msg::ResMngr_CreateComputePipeline &msg)
	{
		Msg::ResMngr_CreateComputePipeline_Response		res;
		if ( auto id = _resMngr->CreateComputePipeline( RmCast(msg.packId), PipelineTmplName{msg.name}, msg.desc, RmCast(msg.cacheId) ))
		{
			_ResMngr_GetPipelineInfo( OUT res, id.Release() );
		}
		_Send( res );
		_ResMngr_CleanupPipelineInfo( INOUT res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateRayTracingPipeline (const Msg::ResMngr_CreateRayTracingPipeline &msg)
	{
		Msg::ResMngr_CreateRayTracingPipeline_Response	res;
		if ( auto id = _resMngr->CreateRayTracingPipeline( RmCast(msg.packId), PipelineTmplName{msg.name}, msg.desc, RmCast(msg.cacheId) ))
		{
			_ResMngr_GetPipelineInfo( OUT res, id.Release() );
		}
		_Send( res );
		_ResMngr_CleanupPipelineInfo( INOUT res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateTilePipeline (const Msg::ResMngr_CreateTilePipeline &msg)
	{
		Msg::ResMngr_CreateTilePipeline_Response	res;
		if ( auto id = _resMngr->CreateTilePipeline( RmCast(msg.packId), PipelineTmplName{msg.name}, msg.desc, RmCast(msg.cacheId) ))
		{
			_ResMngr_GetPipelineInfo( OUT res, id.Release() );
		}
		_Send( res );
		_ResMngr_CleanupPipelineInfo( INOUT res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateVideoSession (const Msg::ResMngr_CreateVideoSession &msg)
	{
		Msg::ResMngr_CreateVideoSession_Response	res;
		if ( auto id = _resMngr->CreateVideoSession( msg.desc, msg.dbgName, _Get(msg.gfxAlloc) ))
		{
			auto&	vs = _resMngr->GetResourcesOrThrow( id.Get() );

			res.id		= RmCast( id.Release() );
			res.desc	= vs.Description();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateVideoBuffer (const Msg::ResMngr_CreateVideoBuffer &msg)
	{
		Msg::ResMngr_CreateVideoBuffer_Response		res;
		if ( auto id = _resMngr->CreateVideoBuffer( msg.desc, msg.dbgName, _Get(msg.gfxAlloc) ))
		{
			auto&	vb	= _resMngr->GetResourcesOrThrow( id.Get() );

			res.id		= RmCast( id.Release() );
			res.desc	= vb.Description();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateVideoImage (const Msg::ResMngr_CreateVideoImage &msg)
	{
		Msg::ResMngr_CreateVideoImage_Response		res;
		if ( auto id = _resMngr->CreateVideoImage( msg.desc, msg.dbgName, _Get(msg.gfxAlloc) ))
		{
			auto&	vi	= _resMngr->GetResourcesOrThrow( id.Get() );

			res.id		= RmCast( id.Release() );
			res.desc	= vi.Description();
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreatePipelineCache (const Msg::ResMngr_CreatePipelineCache &)
	{
		Msg::ResMngr_CreatePipelineCache_Response	res;
		if ( auto id = _resMngr->CreatePipelineCache() )
		{
			res.id = RmCast( id.Release() );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_InitializeResources (const Msg::ResMngr_InitializeResources &msg)
	{
		Msg::DefaultResponse	res;

		auto	id = _resMngr->AcquireResource( RmCast( msg.packId ));
		if ( id )
			res.ok = _resMngr->InitializeResources( RVRef(id) );

		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_LoadPipelinePack (const Msg::ResMngr_LoadPipelinePack &msg)
	{
		auto	mem_arr = _GetMemStack();
		CHECK_THROW( mem_arr.size() == 1 );

		auto	desc		= msg.desc;
		desc.sbtAllocator	= _Get(msg.sbtAllocatorId);
		desc.stream			= MakeRC<SharedMemRStream>( mem_arr[0] );

		Msg::ResMngr_LoadPipelinePack_Response	res;
		if ( auto id = _resMngr->LoadPipelinePack( desc ))
		{
			auto&	pack	= _resMngr->GetResourcesOrThrow( id.Get() );
			auto	dsl		= pack._GetDescriptorSetLayouts();
			auto&	samp	= pack._GetSamplerRefs();
			auto&	rp		= pack._GetRenderPassRefs().specMap;
			auto	pl		= pack._GetPipelineLayoutIDs();

			res.packId = RmCast( id.Release() );
			HashTable_Copy( OUT res.unsupportedFS, pack._GetUnsupportedFS() );

			res.dsLayouts.resize( dsl.size() );
			for (usize i = 0; i < dsl.size(); ++i)
				res.dsLayouts[i] = RmCast( dsl[i] );

			res.samplerRefs.reserve( samp.size() );
			for (auto& [name, rm] : samp)
				res.samplerRefs.emplace( name, RmCast(rm) );
			CHECK_Eq( res.samplerRefs.size(), samp.size() );

			res.renderPasses.reserve( rp.size() );
			for (auto& [name, rp_id] : rp)
				res.renderPasses.push_back( name );

			res.pplnLayouts.reserve( pl.size() );
			for (auto& pl_id : pl)
				res.pplnLayouts.push_back( RmCast(pl_id) );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_GetSupportedRenderTechs (const Msg::ResMngr_GetSupportedRenderTechs &msg)
	{
		Msg::ResMngr_GetSupportedRenderTechs_Response	res;
		res.result = _resMngr->GetSupportedRenderTechs( RmCast(msg.id) );
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_GetRTechPipeline (const Msg::ResMngr_GetRTechPipeline &msg)
	{
		auto	rtech = _Get( msg.rtechId );
		CHECK_THROW( rtech );

		using Types = Msg::ResMngr_GetRTechPipeline::Types;
		switch ( msg.index )
		{
			case Types::Index<RmGraphicsPipelineID> :
			{
				Msg::ResMngr_CreateGraphicsPipeline_Response	res;
				_ResMngr_GetPipelineInfo( OUT res, rtech->GetGraphicsPipeline( PipelineName{msg.name} ));
				_Send( res );
				_ResMngr_CleanupPipelineInfo( INOUT res );
				break;
			}
			case Types::Index<RmMeshPipelineID> :
			{
				Msg::ResMngr_CreateMeshPipeline_Response	res;
				_ResMngr_GetPipelineInfo( OUT res, rtech->GetMeshPipeline( PipelineName{msg.name} ));
				_Send( res );
				_ResMngr_CleanupPipelineInfo( INOUT res );
				break;
			}
			case Types::Index<RmComputePipelineID> :
			{
				Msg::ResMngr_CreateComputePipeline_Response	res;
				_ResMngr_GetPipelineInfo( OUT res, rtech->GetComputePipeline( PipelineName{msg.name} ));
				_Send( res );
				_ResMngr_CleanupPipelineInfo( INOUT res );
				break;
			}
			case Types::Index<RmRayTracingPipelineID> :
			{
				Msg::ResMngr_CreateRayTracingPipeline_Response	res;
				_ResMngr_GetPipelineInfo( OUT res, rtech->GetRayTracingPipeline( PipelineName{msg.name} ));
				_Send( res );
				_ResMngr_CleanupPipelineInfo( INOUT res );
				break;
			}
			case Types::Index<RmTilePipelineID> :
			{
				Msg::ResMngr_CreateTilePipeline_Response	res;
				_ResMngr_GetPipelineInfo( OUT res, rtech->GetTilePipeline( PipelineName{msg.name} ));
				_Send( res );
				_ResMngr_CleanupPipelineInfo( INOUT res );
				break;
			}
		}
	}

	void  RmGAppListener::_Cb_ResMngr_GetRenderPass (const Msg::ResMngr_GetRenderPass &msg)
	{
		auto&	pack		= _resMngr->GetResourcesOrThrow( RmCast(msg.packId) );
		auto	id			= pack.GetRenderPass( RenderPassName{msg.name} );
		auto&	rp			= _resMngr->GetResourcesOrThrow( id );
		auto	att_states	= rp.AttachmentStates();

		Msg::ResMngr_GetRenderPass_Response	res;
		res.subpassCount = ubyte(rp.Subpasses().size());

		for (auto [name, idx] : rp.AttachmentMap()) {
			res.pixFormats.emplace( name, MakePair( rp.GetPixelFormat( idx.Index() ), ubyte(idx.Index()) ));
		}
		for (usize i = 0; i < att_states.size(); ++i) {
			res.attStates[i] = {att_states[i].initial, att_states[i].final};
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_LoadRenderTech (const Msg::ResMngr_LoadRenderTech &msg)
	{
		Msg::ResMngr_LoadRenderTech_Response	res;

		const auto	Copy = [&res] (auto* rtech)
		{{
			auto&	pplns	= rtech->_GetPipelineSpecs();
			auto&	sbts	= rtech->_GetSBTs();
			auto&	passes	= rtech->_GetPasses();

			res.pipelines.reserve( pplns.size() );
			res.rtSbtMap.reserve( sbts.size() );
			res.passes.reserve( passes.size() );

			for (auto& [name, info] : pplns) {
				res.pipelines.emplace_back( name, info.uid );
			}
			for (auto& [name, info] : sbts) {
				res.rtSbtMap.emplace_back( name, RmCast(info.sbtId) );
			}
			for (auto& ser : rtech->_GetPasses())
			{
				Msg::ResMngr_LoadRenderTech_Response::SerPass	pass;
				pass.info = rtech->GetPass( ser.name );
				pass.name = ser.name;
				CHECK( pass.info.IsDefined() );
				res.passes.push_back( pass );
			}
		}};

		if ( auto ptr = _resMngr->LoadRenderTech( RmCast(msg.packId), RenderTechName{msg.name}, RmCast(msg.cacheId) ))
		{
			res.id		= _Set( ptr );
			res.name	= ptr->Name();

		  #if defined(AE_ENABLE_VULKAN)
			Copy( Cast<VPipelinePack::RenderTech>( ptr.get() ));

		  #elif defined(AE_ENABLE_METAL)
			Copy( Cast<MPipelinePack::RenderTech>( ptr.get() ));

		  #endif
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateLinearGfxMemAllocator (const Msg::ResMngr_CreateLinearGfxMemAllocator &msg)
	{
		Msg::ResMngr_CreateGfxMemAllocator_Response		res;
		if ( auto ptr = _resMngr->CreateLinearGfxMemAllocator( msg.pageSize ))
		{
			res.id = _Set( ptr );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateBlockGfxMemAllocator (const Msg::ResMngr_CreateBlockGfxMemAllocator &msg)
	{
		Msg::ResMngr_CreateGfxMemAllocator_Response		res;
		if ( auto ptr = _resMngr->CreateBlockGfxMemAllocator( msg.blockSize, msg.pageSize ))
		{
			res.id = _Set( ptr );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_CreateUnifiedGfxMemAllocator (const Msg::ResMngr_CreateUnifiedGfxMemAllocator &msg)
	{
		Msg::ResMngr_CreateGfxMemAllocator_Response		res;
		if ( auto ptr = _resMngr->CreateUnifiedGfxMemAllocator( msg.pageSize ))
		{
			res.id = _Set( ptr );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_ForceReleaseResources (const Msg::ResMngr_ForceReleaseResources &)
	{
		Msg::DefaultResponse	res;
		res.ok = _resMngr->ForceReleaseResources();
		_Send( res );
	}

	void  RmGAppListener::_Cb_ResMngr_GetShaderGroupStackSize (const Msg::ResMngr_GetShaderGroupStackSize &msg)
	{
		Msg::ResMngr_GetShaderGroupStackSize_Response  res;
		res.size = _resMngr->GetShaderGroupStackSize( RmCast(msg.pplnId), msg.names, msg.type );
		_Send( res );
	}
//-----------------------------------------------------------------------------


	RmGAppListener::Query  RmGAppListener::_UnpackQuery (const Msg::Query_Alloc_Response::SerQuery &src)
	{
		#ifdef AE_ENABLE_VULKAN
		StaticAssert( sizeof(VQueryManager::Query) == 16 );
		#endif

		Query	dst;
		dst.pool		= _GetQuery( src.queryId );
		dst.first		= src.first;
		dst.count		= src.count;
		dst.numPasses	= src.numPasses;
		dst.type		= src.type;
		dst.queue		= src.queue;
		return dst;
	}

	void  RmGAppListener::_Cb_Query_Init (const Msg::Query_Init &)
	{
		Msg::Query_Init_Response  res;
		res.calibratedTimestamps = _resMngr->GetQueryManager().SupportsCalibratedTimestamps();
		_Send( res );
	}

	void  RmGAppListener::_Cb_Query_Alloc (const Msg::Query_Alloc &msg)
	{
		Msg::Query_Alloc_Response	res;

		Query	query = _resMngr->GetQueryManager().AllocQuery( msg.queueType, msg.queryType, msg.count );
		if ( query )
		{
			res.query.queryId	= _SetQuery( query.pool );
			res.query.first		= query.first;
			res.query.count		= query.count;
			res.query.numPasses	= query.numPasses;
			res.query.type		= query.type;
			res.query.queue		= query.queue;
		}
		_Send( res );
	}

	template <typename T, typename RespType, typename MsgType>
	void  RmGAppListener::_Query_GetTimestamp (const MsgType &msg)
	{
		RespType		res;
		T*				ptr;
		const Bytes		size = Min( 1_Kb, msg.size );

		AllocateOnStack2( OUT ptr, size );

		if ( _resMngr->GetQueryManager().GetTimestamp( _UnpackQuery(msg.query), OUT ptr, size ))
			res.result = ArrayView<T>{ ptr, usize{size / SizeOf<T>} };

		_Send( res );
	}

	void  RmGAppListener::_Cb_Query_GetTimestampUL (const Msg::Query_GetTimestampUL &msg)
	{
		_Query_GetTimestamp< ulong, Msg::Query_GetTimestampUL_Response >( msg );
	}

	void  RmGAppListener::_Cb_Query_GetTimestampD (const Msg::Query_GetTimestampD &msg)
	{
		_Query_GetTimestamp< double, Msg::Query_GetTimestampD_Response >( msg );
	}

	void  RmGAppListener::_Cb_Query_GetTimestampNs (const Msg::Query_GetTimestampNs &msg)
	{
		_Query_GetTimestamp< nanosecondsd, Msg::Query_GetTimestampNs_Response >( msg );
	}

	template <typename T, typename RespType, typename MsgType>
	void  RmGAppListener::_Query_GetTimestampCalibrated (const MsgType &msg)
	{
		RespType		res;
		T*				ptr0;
		T*				ptr1;
		const Bytes		size = Min( 512_b, msg.size );

		AllocateOnStack2( OUT ptr0, size );
		AllocateOnStack2( OUT ptr1, size );

		if ( _resMngr->GetQueryManager().GetTimestampCalibrated( _UnpackQuery(msg.query), OUT ptr0, OUT ptr1, size ))
		{
			res.result			= ArrayView<T>{ ptr0, usize{size / SizeOf<T>} };
			res.maxDeviation	= ArrayView<T>{ ptr1, usize{size / SizeOf<T>} };
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_Query_GetTimestampCalibratedUL (const Msg::Query_GetTimestampCalibratedUL &msg)
	{
		_Query_GetTimestampCalibrated< ulong, Msg::Query_GetTimestampCalibratedUL_Response >( msg );
	}

	void  RmGAppListener::_Cb_Query_GetTimestampCalibratedD (const Msg::Query_GetTimestampCalibratedD &msg)
	{
		_Query_GetTimestampCalibrated< double, Msg::Query_GetTimestampCalibratedD_Response >( msg );
	}

	void  RmGAppListener::_Cb_Query_GetTimestampCalibratedNs (const Msg::Query_GetTimestampCalibratedNs &msg)
	{
		_Query_GetTimestampCalibrated< nanosecondsd, Msg::Query_GetTimestampCalibratedNs_Response >( msg );
	}

	void  RmGAppListener::_Cb_Query_GetPipelineStatistic (const Msg::Query_GetPipelineStatistic &msg)
	{
		using PipelineStatistic = IQueryManager::GraphicsPipelineStatistic;
		Msg::Query_GetPipelineStatistic_Response  res;

		PipelineStatistic*	ptr;
		const Bytes			size = Min( 1_Kb, msg.size );

		AllocateOnStack2( OUT ptr, size );

		if ( _resMngr->GetQueryManager().GetPipelineStatistic( _UnpackQuery(msg.query), OUT ptr, size ))
			res.result = ArrayView<PipelineStatistic>{ ptr, usize{size / SizeOf<PipelineStatistic>} };

		_Send( res );
	}
//-----------------------------------------------------------------------------



	void  RmGAppListener::_Cb_ProfArm_Initialize (const Msg::ProfArm_Initialize &msg)
	{
		Msg::ProfArm_Initialize_Response  res;
		#ifdef AE_ENABLE_ARM_PMU
			res.ok = _profilers.arm.Initialize( msg.required );
			if ( res.ok )
				res.enabled	= _profilers.arm.EnabledCounterSet();
		#else
			Unused( msg );
			res.ok = false;
		#endif
		_Send( res );
	}

	void  RmGAppListener::_Cb_ProfArm_Sample (const Msg::ProfArm_Sample &)
	{
		Msg::ProfArm_Sample_Response  res;
		#ifdef AE_ENABLE_ARM_PMU
			_profilers.arm.Sample( OUT res.counters );
		#endif
		_Send( res );
	}


	void  RmGAppListener::_Cb_ProfMali_Initialize (const Msg::ProfMali_Initialize &msg)
	{
		Msg::ProfMali_Initialize_Response  res;
		#ifdef AE_ENABLE_MALI_HWCPIPE
			res.ok = _profilers.mali.Initialize( msg.required );
			if ( res.ok ) {
				res.enabled = _profilers.mali.EnabledCounterSet();
				res.info	= _profilers.mali.GetHWInfo();
			}
		#else
			Unused( msg );
			res.ok = false;
		#endif
		_Send( res );
	}

	void  RmGAppListener::_Cb_ProfMali_Sample (const Msg::ProfMali_Sample &)
	{
		Msg::ProfMali_Sample_Response  res;
		#ifdef AE_ENABLE_MALI_HWCPIPE
			_profilers.mali.Sample( OUT res.counters );
		#endif
		_Send( res );
	}


	void  RmGAppListener::_Cb_ProfAdreno_Initialize (const Msg::ProfAdreno_Initialize &msg)
	{
		Msg::ProfAdreno_Initialize_Response  res;
		#ifdef AE_ENABLE_ADRENO_PERFCOUNTER
			res.ok = _profilers.adreno.Initialize( msg.required );
			if ( res.ok )
				res.enabled = _profilers.adreno.EnabledCounterSet();
		#else
			Unused( msg );
			res.ok = false;
		#endif
		_Send( res );
	}

	void  RmGAppListener::_Cb_ProfAdreno_Sample (const Msg::ProfAdreno_Sample &)
	{
		Msg::ProfAdreno_Sample_Response  res;
		#ifdef AE_ENABLE_ADRENO_PERFCOUNTER
			_profilers.adreno.Sample( OUT res.counters );
		#endif
		_Send( res );
	}


	void  RmGAppListener::_Cb_ProfPVR_Initialize (const Msg::ProfPVR_Initialize &msg)
	{
		Msg::ProfPVR_Initialize_Response  res;
		#ifdef AE_ENABLE_PVRCOUNTER
			res.ok = _profilers.pvr.Initialize( msg.required );
			if ( res.ok )
				res.enabled	= _profilers.pvr.EnabledCounterSet();
		#else
			Unused( msg );
			res.ok = false;
		#endif
		_Send( res );
	}

	void  RmGAppListener::_Cb_ProfPVR_Tick (const Msg::ProfPVR_Tick &)
	{
		Msg::ProfPVR_Tick_Response  res;
		#ifdef AE_ENABLE_PVRCOUNTER
			_profilers.pvr.Tick();
			res.timings = _profilers.pvrTimings;
		#endif
		_Send( res );
	}

	void  RmGAppListener::_Cb_ProfPVR_Sample (const Msg::ProfPVR_Sample &)
	{
		Msg::ProfPVR_Sample_Response  res;
		#ifdef AE_ENABLE_PVRCOUNTER
			_profilers.pvr.Sample( OUT res.counters );
		#endif
		_Send( res );
	}


	void  RmGAppListener::_Cb_ProfNVidia_Initialize (const Msg::ProfNVidia_Initialize &msg)
	{
		Msg::ProfNVidia_Initialize_Response  res;
		#ifdef AE_ENABLE_NVML
			res.ok = _profilers.nv.Initialize( msg.required );
			if ( res.ok )
				res.enabled = _profilers.nv.EnabledCounterSet();
		#else
			Unused( msg );
			res.ok = false;
		#endif
		_Send( res );
	}

	void  RmGAppListener::_Cb_ProfNVidia_Sample (const Msg::ProfNVidia_Sample &)
	{
		Msg::ProfNVidia_Sample_Response  res;
		#ifdef AE_ENABLE_NVML
			_profilers.nv.Sample( OUT res.counters );
		#endif
		_Send( res );
	}
//-----------------------------------------------------------------------------



	ND_ static Bytes  CalcBufferSlicePitch (EPixelFormat format, const UploadImageDesc &uploadDesc, const uint3 &dim)
	{
		const auto&		fmt_info		= EPixelFormat_GetInfo( format );
		const uint2		texblock_dim	= fmt_info.TexBlockDim();
		const uint		texblock_bits	= uploadDesc.aspectMask != EImageAspect::Stencil ? fmt_info.bitsPerBlock : fmt_info.bitsPerBlock2;
		const Bytes		row_pitch		= Max( uploadDesc.dataRowPitch, Bytes{dim.x * texblock_bits + texblock_dim.x-1} / (texblock_dim.x * 8) );
		const Bytes		min_slice_pitch	= (dim.y * row_pitch + texblock_dim.y-1) / texblock_dim.y;
		const Bytes		slice_pitch		= Max( uploadDesc.dataSlicePitch, min_slice_pitch );
		return slice_pitch;
	}

	void  RmGAppListener::_Cb_SBM_GetBufferRanges (const Msg::SBM_GetBufferRanges &msg)
	{
		StagingBufferMngr::BufferRanges_t	result;
		_resMngr->GetStagingManager().GetBufferRanges( OUT result, msg.reqSize, msg.blockSize, msg.memOffsetAlign, msg.frameId, msg.heap, Bool{msg.upload} );

		Msg::SBM_GetBufferRanges_Response	res;
		for (usize i = 0; i < result.size(); ++i)
		{
			auto&	src = result[i];
			auto&	dst = res.ranges.emplace_back();

			dst.buffer			= RmCast( src.bufferId );
			dst.bufferOffset	= src.bufferOffset;
			dst.size			= src.size;
			dst.mapped			= BitCastRlx<RmDevicePtr>( src.mapped );
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_SBM_GetImageRanges (const Msg::SBM_GetImageRanges &msg)
	{
		StagingBufferMngr::StagingImageResultRanges	result;
		_resMngr->GetStagingManager().GetImageRanges( OUT result, msg.uploadDesc, msg.imageDesc, msg.imageGranularity, msg.frameId, Bool{msg.upload} );

		Msg::SBM_GetImageRanges_Response	res;
		for (usize i = 0; i < result.buffers.size(); ++i)
		{
			auto&	src = result.buffers[i];
			auto&	dst = res.ranges.emplace_back();

			dst.buffer				= RmCast( src.bufferId );
			dst.bufferOffset		= src.bufferOffset;
			dst.size				= src.size;
			dst.mapped				= BitCastRlx<RmDevicePtr>( src.mapped );
			dst.imageOffset			= src.imageOffset;
			dst.imageDim			= src.imageDim;
		  #if defined(AE_ENABLE_VULKAN)
			dst.bufferSlicePitch	= CalcBufferSlicePitch( result.format, msg.uploadDesc, src.imageDim );
		  #elif defined(AE_ENABLE_METAL)
			dst.bufferSlicePitch	= src.bufferSlicePitch;
		  #endif
		}
		res.bufferRowLength	= result.bufferRowLength;
		res.planeScaleY		= result.planeScaleY;
		res.format			= result.format;
		res.dataRowPitch	= result.dataRowPitch;
		res.dataSlicePitch	= result.dataSlicePitch;

		_Send( res );
	}

	void  RmGAppListener::_Cb_SBM_GetImageRanges2 (const Msg::SBM_GetImageRanges2 &msg)
	{
		StagingBufferMngr::StagingImageResultRanges	result;
		_resMngr->GetStagingManager().GetImageRanges( OUT result, msg.uploadDesc, msg.videoDesc, msg.imageGranularity, msg.frameId, Bool{msg.upload} );

		Msg::SBM_GetImageRanges_Response	res;
		for (usize i = 0; i < result.buffers.size(); ++i)
		{
			auto&	src = result.buffers[i];
			auto&	dst = res.ranges.emplace_back();

			dst.buffer				= RmCast( src.bufferId );
			dst.bufferOffset		= src.bufferOffset;
			dst.size				= src.size;
			dst.mapped				= BitCastRlx<RmDevicePtr>( src.mapped );
			dst.imageOffset			= src.imageOffset;
			dst.imageDim			= src.imageDim;
		  #if defined(AE_ENABLE_VULKAN)
			dst.bufferSlicePitch	= CalcBufferSlicePitch( result.format, msg.uploadDesc, src.imageDim );
		  #elif defined(AE_ENABLE_METAL)
			dst.bufferSlicePitch	= src.bufferSlicePitch;
		  #endif
		}
		res.bufferRowLength	= result.bufferRowLength;
		res.planeScaleY		= result.planeScaleY;
		res.format			= result.format;
		res.dataRowPitch	= result.dataRowPitch;
		res.dataSlicePitch	= result.dataSlicePitch;

		_Send( res );
	}

	void  RmGAppListener::_Cb_SBM_AllocVStream (const Msg::SBM_AllocVStream &msg)
	{
		Msg::SBM_AllocVStream_Response	res;
		VertexStream					result;

		if ( _resMngr->GetStagingManager().AllocVStream( msg.frameId, msg.size, OUT result ))
		{
			res.mapped	= BitCastRlx<RmDevicePtr>( result.mappedPtr );
			res.handle	= RmCast( result.id );
			res.offset	= result.offset;
			res.size	= result.size;
		}
		_Send( res );
	}

	void  RmGAppListener::_Cb_SBM_GetFrameStat (const Msg::SBM_GetFrameStat &msg)
	{
		Msg::SBM_GetFrameStat_Response	res;
		res.stat = _resMngr->GetStagingManager().GetFrameStat( msg.frameId );
		_Send( res );
	}
//-----------------------------------------------------------------------------



	void  RmGAppListener::_Cb_DescUpd_Flush (const Msg::DescUpd_Flush &msg)
	{
		const auto	Run = [this, &msg] () -> bool
		{{
			auto	mem_arr = _GetMemStack();
			CHECK_THROW( not mem_arr.empty() );

			auto	stream = MakeRC<SharedMemRStream>( mem_arr.back() );
			CHECK_ERR( stream->IsOpen() );

			Serializing::Deserializer	des {stream};
			des.factory = &_descFactory;

			DescriptorUpdater	updater;

			bool	ok = true;
			try {
				for (uint i = 0; i < msg.count; ++i)
				{
					Unique<Msg::DescUpd_Flush::BaseUpdCmd>	cmd;
					CHECK_ERR( des( OUT cmd ) and cmd );
					cmd->Execute( &updater );
				}
			}
			catch (...) {
				ok = false;
			}

			ok = ok and updater.Flush();
			return ok;
		}};

		Msg::DefaultResponse	res;
		res.ok = Run();
		_Send( res );
	}
//-----------------------------------------------------------------------------



	using CmdCtx		= RmGAppListener::CmdCtx;
	using EContextType	= IGraphicsProfiler::EContextType;


	RmGAppListener::CmdCtx::~CmdCtx ()
	{
		CHECK( type == EContextType::Unknown );
	}

	void  RmGAppListener::CmdCtx::End () __Th___
	{
		switch_enum( type )
		{
			case EContextType::Unknown :		break;
			case EContextType::Transfer :		cmdbuf = transfer.ReleaseCommandBuffer();	PlacementDelete( transfer );	break;
			case EContextType::Compute :		cmdbuf = compute.ReleaseCommandBuffer();	PlacementDelete( compute );		break;
			case EContextType::Graphics :		cmdbuf = graphics.ReleaseCommandBuffer();	PlacementDelete( graphics );	break;
			case EContextType::ASBuild :		cmdbuf = asBuild.ReleaseCommandBuffer();	PlacementDelete( asBuild );		break;
			case EContextType::RayTracing :		cmdbuf = rayTracing.ReleaseCommandBuffer();	PlacementDelete( rayTracing );	break;
			case EContextType::RenderPass :
			case EContextType::VideoDecode :
			case EContextType::VideoEncode :
			default :							CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end
		type = EContextType::Unknown;
	}

	Graphics::IBaseContext&  RmGAppListener::CmdCtx::GetBaseCtx () __Th___
	{
		switch_enum( type )
		{
			case EContextType::Transfer :		return transfer;	break;
			case EContextType::Compute :		return compute;		break;
			case EContextType::Graphics :		return graphics;	break;
			case EContextType::ASBuild :		return asBuild;		break;
			case EContextType::RayTracing :		return rayTracing;	break;
			case EContextType::RenderPass :
			case EContextType::VideoDecode :
			case EContextType::VideoEncode :
			case EContextType::Unknown :
			default :							CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end
	}

	void  RmGAppListener::RenderTaskImpl::Execute () __Th___
	{
		switch_enum( ctx.type )
		{
			case EContextType::Transfer :		RenderTask::Execute( ctx.transfer );	PlacementDelete( ctx.transfer );	break;
			case EContextType::Compute :		RenderTask::Execute( ctx.compute );		PlacementDelete( ctx.compute );		break;
			case EContextType::Graphics :		RenderTask::Execute( ctx.graphics );	PlacementDelete( ctx.graphics );	break;
			case EContextType::ASBuild :		RenderTask::Execute( ctx.asBuild );		PlacementDelete( ctx.asBuild );		break;
			case EContextType::RayTracing :		RenderTask::Execute( ctx.rayTracing );	PlacementDelete( ctx.rayTracing );	break;
			case EContextType::RenderPass :
			case EContextType::VideoDecode :
			case EContextType::VideoEncode :
			case EContextType::Unknown :
			default :							CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end
		ctx.type = EContextType::Unknown;
	}

	void  RmGAppListener::DrawTaskImpl::Execute () __Th___
	{
		DrawTask::Execute( ctx.draw );
		PlacementDelete( ctx.draw );
		ctx.type = EContextType::Unknown;
	}


	template <typename RespType, typename CBID, typename TaskType, typename MsgType>
	void  RmGAppListener::_CmdBuf_Bake (const MsgType &msg)
	{
		const auto	Run = [this, &msg] () -> CBID
		{{
			CBID	id;
			try {
				auto	mem_arr = _GetMemStack();
				CHECK_ERR( not mem_arr.empty() );
				CHECK_ERR( msg.blockSize > 0 );

				auto	ser_mem = mem_arr.back();
				mem_arr.pop_back();

				const auto	GetDeviceMem = [&msg, &mem_arr] (RmDeviceOffset devOffset, Bytes size) __Th___
				{{
					const Bytes		offset	= RmCast( devOffset );
					const usize		mem_idx	= usize{offset / msg.blockSize};
					const Bytes		mem_off	= offset % msg.blockSize;

					CHECK_THROW( mem_idx < mem_arr.size() );
					CHECK_THROW( mem_off + size <= mem_arr[mem_idx]->Size() );

					return mem_arr[mem_idx]->Data() + mem_off;
				}};

				const auto	frame_id	= GraphicsScheduler().GetFrameId();
				auto&		dev_to_host	= _devToHostCopy[ frame_id.Index() ];
				CHECK( dev_to_host->frameId == frame_id );

				const auto	AddHostMem = [&dev_to_host] (void* src, RmHostPtr dst, Bytes size) __Th___
				{{
					dev_to_host->arr.emplace_back( src, dst, size );
				}};

				for (auto& copy : msg.hostToDev)
				{
					void*			dst		= RmCast( copy.template Get<RmDevicePtr>() );
					const Bytes		size	= copy.template Get<Bytes>();
					void*			src		= GetDeviceMem( copy.template Get<RmDeviceOffset>(), size );
					MemCopy( OUT dst, src, size );
				}

				if constexpr( IsSameTypes< MsgType, Msg::CmdBuf_Bake >)
				{
					auto	dst = dev_to_host.WriteLock();
					for (auto& src : msg.devToHost) {
						dst->arr.emplace_back( RmCast(src.template Get<RmDevicePtr>()), src.template Get<RmHostPtr>(), src.template Get<Bytes>() );
					}
				}

				CHECK_ERR( msg.batchId );

				auto	batch = _Get( msg.batchId );
				CHECK_ERR( batch );

				auto	stream = MakeRC<SharedMemRStream>( ser_mem );
				CHECK_ERR( stream->IsOpen() );

				LinearAllocator<>			alloc;
				Serializing::Deserializer	des {stream};

				des.factory		= &_cmdFactory;
				des.allocator	= &alloc;

				auto	task = MakeRC<TaskType>( *this, RVRef(batch), msg.exeIndex );
				auto*	ctx  = &task->ctx;

				ctx->getMem = GetDeviceMem;
				ctx->addMem = AddHostMem;
				CHECK_ERR( task->IsValid() );

				for (uint i = 0; i < msg.count; ++i)
				{
					alloc.Discard();
					Unique<typename MsgType::BaseCmd>  cmd;
					CHECK_ERR( des( OUT cmd ) and cmd );
					cmd->Execute( ctx );
				}

				ctx->getMem = {};
				ctx->addMem = {};
				task->Execute();

				id = _Set( task );
			}
			catch (...)
			{}
			return id;
		}};

		RespType	res;
		res.cmdbuf = Run();
		_Send( res );
	}


	void  RmGAppListener::_Cb_CmdBuf_Bake (const Msg::CmdBuf_Bake &msg)
	{
		_CmdBuf_Bake< Msg::CmdBuf_Bake_Response, RmCommandBufferID, RenderTaskImpl >( msg );
	}

	void  RmGAppListener::_Cb_CmdBuf_BakeDraw (const Msg::CmdBuf_BakeDraw &msg)
	{
		_CmdBuf_Bake< Msg::CmdBuf_BakeDraw_Response, RmDrawCommandBufferID, DrawTaskImpl >( msg );
	}
//-----------------------------------------------------------------------------


} // AE::RemoteGraphics

namespace AE::RemoteGraphics::Msg
{

	void  DescUpd_Flush::SetDescSet::Execute (void* updater) __Th___ {
		CHECK_THROW( Cast<DescriptorUpdater>(updater)->Set( RmCast(descSet), mode ));
	}

	void  DescUpd_Flush::BindVideoImage::Execute (void* updater) __Th___ {
		CHECK( Cast<DescriptorUpdater>(updater)->BindVideoImage( UniformName{unName}, RmCast(videoImageId), firstIndex ));
	}

	void  DescUpd_Flush::BindImages::Execute (void* updater) __Th___ {
		CHECK( Cast<DescriptorUpdater>(updater)->BindImages( UniformName{unName}, RmCast(images), firstIndex ));
	}

	void  DescUpd_Flush::BindBuffers::Execute (void* updater) __Th___ {
		CHECK( Cast<DescriptorUpdater>(updater)->BindBuffers( UniformName{unName}, ShaderStructName{typeName}, RmCast(buffers), firstIndex ));
	}

	void  DescUpd_Flush::BindBufferRange::Execute (void* updater) __Th___ {
		CHECK( Cast<DescriptorUpdater>(updater)->BindBuffer( UniformName{unName}, ShaderStructName{typeName}, RmCast(buffer), offset, size ));
	}

	void  DescUpd_Flush::BindTextures::Execute (void* updater) __Th___ {
		CHECK( Cast<DescriptorUpdater>(updater)->BindTextures( UniformName{unName}, RmCast(images), SamplerName{sampler}, firstIndex ));
	}

	void  DescUpd_Flush::BindSamplers::Execute (void* updater) __Th___
	{
	  #if AE_OPTIMIZE_IDS
		Cast<DescriptorUpdater>(updater)->BindSamplers( UniformName{unName}, samplers, firstIndex );
	  #else
		Array<SamplerName>	temp_samplers;
		temp_samplers.resize( samplers.size() );
		for (usize i = 0; i < samplers.size(); ++i)
			temp_samplers[i] = SamplerName{samplers[i]};
		Cast<DescriptorUpdater>(updater)->BindSamplers( UniformName{unName}, temp_samplers, firstIndex );
	  #endif
	}

	void  DescUpd_Flush::BindTexelBuffers::Execute (void* updater) __Th___ {
		Cast<DescriptorUpdater>(updater)->BindTexelBuffers( UniformName{unName}, RmCast(views), firstIndex );
	}

	void  DescUpd_Flush::BindRayTracingScenes::Execute (void* updater) __Th___ {
		Cast<DescriptorUpdater>(updater)->BindRayTracingScenes( UniformName{unName}, RmCast(scenes), firstIndex );
	}
	//-------------------------------------------------


	void  CmdBuf_Bake::BeginGraphics::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		ctx.End();
		PlacementNew<DirectCtx::Graphics>( INOUT &ctx.graphics, *ctx.rtask, RVRef(ctx.cmdbuf), dbgLabel );
		ctx.type = EContextType::Graphics;
	}

	void  CmdBuf_Bake::BeginCompute::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		ctx.End();
		PlacementNew<DirectCtx::Compute>( INOUT &ctx.compute, *ctx.rtask, RVRef(ctx.cmdbuf), dbgLabel );
		ctx.type = EContextType::Compute;
	}

	void  CmdBuf_Bake::BeginTransfer::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		ctx.End();
		PlacementNew<DirectCtx::Transfer>( INOUT &ctx.transfer, *ctx.rtask, RVRef(ctx.cmdbuf), dbgLabel );
		ctx.type = EContextType::Transfer;
	}

	void  CmdBuf_Bake::BeginASBuild::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		ctx.End();
		PlacementNew<DirectCtx::ASBuild>( INOUT &ctx.asBuild, *ctx.rtask, RVRef(ctx.cmdbuf), dbgLabel );
		ctx.type = EContextType::ASBuild;
	}

	void  CmdBuf_Bake::BeginRayTracing::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		ctx.End();
		PlacementNew<DirectCtx::RayTracing>( INOUT &ctx.rayTracing, *ctx.rtask, RVRef(ctx.cmdbuf), dbgLabel );
		ctx.type = EContextType::RayTracing;
	}

	void  CmdBuf_Bake::DebugMarkerCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		switch_enum( ctx.type )
		{
			case EContextType::Transfer :		ctx.transfer.DebugMarker( dbgLabel );	break;
			case EContextType::Compute :		ctx.compute.DebugMarker( dbgLabel );	break;
			case EContextType::Graphics :		ctx.graphics.DebugMarker( dbgLabel );	break;
			case EContextType::ASBuild :		ctx.asBuild.DebugMarker( dbgLabel );	break;
			case EContextType::RayTracing :		ctx.rayTracing.DebugMarker( dbgLabel );	break;
			case EContextType::RenderPass :		ctx.draw.DebugMarker( dbgLabel );		break;
			case EContextType::VideoDecode :
			case EContextType::VideoEncode :
			case EContextType::Unknown :
			default :							CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end
	}

	void  CmdBuf_Bake::PushDebugGroupCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		switch_enum( ctx.type )
		{
			case EContextType::Transfer :		ctx.transfer.PushDebugGroup( dbgLabel );	break;
			case EContextType::Compute :		ctx.compute.PushDebugGroup( dbgLabel );		break;
			case EContextType::Graphics :		ctx.graphics.PushDebugGroup( dbgLabel );	break;
			case EContextType::ASBuild :		ctx.asBuild.PushDebugGroup( dbgLabel );		break;
			case EContextType::RayTracing :		ctx.rayTracing.PushDebugGroup( dbgLabel );	break;
			case EContextType::RenderPass :		ctx.draw.PushDebugGroup( dbgLabel );		break;
			case EContextType::VideoDecode :
			case EContextType::VideoEncode :
			case EContextType::Unknown :
			default :							CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end
	}

	void  CmdBuf_Bake::PopDebugGroupCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		switch_enum( ctx.type )
		{
			case EContextType::Transfer :		ctx.transfer.PopDebugGroup();	break;
			case EContextType::Compute :		ctx.compute.PopDebugGroup();	break;
			case EContextType::Graphics :		ctx.graphics.PopDebugGroup();	break;
			case EContextType::ASBuild :		ctx.asBuild.PopDebugGroup();	break;
			case EContextType::RayTracing :		ctx.rayTracing.PopDebugGroup();	break;
			case EContextType::RenderPass :		ctx.draw.PopDebugGroup();		break;
			case EContextType::VideoDecode :
			case EContextType::VideoEncode :
			case EContextType::Unknown :
			default :							CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end
	}

	void  CmdBuf_Bake::WriteTimestampCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		auto	q	= ctx.rmdev._UnpackQuery( query );
		switch_enum( ctx.type )
		{
			case EContextType::Transfer :		ctx.transfer.WriteTimestamp( q, index, scope );		break;
			case EContextType::Compute :		ctx.compute.WriteTimestamp( q, index, scope );		break;
			case EContextType::Graphics :		ctx.graphics.WriteTimestamp( q, index, scope );		break;
			case EContextType::ASBuild :		ctx.asBuild.WriteTimestamp( q, index, scope );		break;
			case EContextType::RayTracing :		ctx.rayTracing.WriteTimestamp( q, index, scope );	break;
			case EContextType::RenderPass :		ctx.draw.WriteTimestamp( q, index, scope );			break;
			case EContextType::VideoDecode :
			case EContextType::VideoEncode :
			case EContextType::Unknown :
			default :							CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end
	}

	//-------------------------------------------------
	// pipeline barriers

	void  CmdBuf_Bake::BufferBarrierCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().BufferBarrier( RmCast(buffer), srcState, dstState );
	}

	void  CmdBuf_Bake::BufferViewBarrierCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().BufferViewBarrier( RmCast(bufferView), srcState, dstState );
	}

	void  CmdBuf_Bake::ImageBarrierCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().ImageBarrier( RmCast(image), srcState, dstState );
	}

	void  CmdBuf_Bake::ImageRangeBarrierCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().ImageBarrier( RmCast(image), srcState, dstState, subRes );
	}

	void  CmdBuf_Bake::ImageViewBarrierCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().ImageViewBarrier( RmCast(imageView), srcState, dstState );
	}

	void  CmdBuf_Bake::MemoryBarrierCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().MemoryBarrier( srcState, dstState );
	}

	void  CmdBuf_Bake::MemoryBarrier2Cmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().MemoryBarrier( srcScope, dstScope );
	}

	void  CmdBuf_Bake::MemoryBarrier3Cmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().MemoryBarrier();
	}

	void  CmdBuf_Bake::ExecutionBarrierCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().ExecutionBarrier( srcScope, dstScope );
	}

	void  CmdBuf_Bake::ExecutionBarrier2Cmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().ExecutionBarrier();
	}

	void  CmdBuf_Bake::AcquireBufferOwnershipCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().AcquireBufferOwnership( RmCast(buffer), srcQueue, srcState, dstState );
	}

	void  CmdBuf_Bake::ReleaseBufferOwnershipCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().ReleaseBufferOwnership( RmCast(buffer), srcState, dstState, dstQueue );
	}

	void  CmdBuf_Bake::AcquireImageOwnershipCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().AcquireImageOwnership( RmCast(image), srcQueue, srcState, dstState );
	}

	void  CmdBuf_Bake::ReleaseImageOwnershipCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().ReleaseImageOwnership( RmCast(image), srcState, dstState, dstQueue );
	}

	void  CmdBuf_Bake::CommitBarriersCmd::Execute (void* inCtx) __Th___
	{
		Cast<CmdCtx>(inCtx)->GetBaseCtx().CommitBarriers();
	}

	//-------------------------------------------------
	// transfer commands

	void  CmdBuf_Bake::ClearColorImageCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		Visit( color,
			[&](const RGBA32f &c) { ctx.transfer.ClearColorImage( RmCast(image), c, ranges ); },
			[&](const RGBA32i &c) { ctx.transfer.ClearColorImage( RmCast(image), c, ranges ); },
			[&](const RGBA32u &c) { ctx.transfer.ClearColorImage( RmCast(image), c, ranges ); }
		);
	}

	void  CmdBuf_Bake::ClearDepthStencilImageCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.ClearDepthStencilImage( RmCast(image), depthStencil, ranges );
	}

	void  CmdBuf_Bake::UpdateBufferCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.UpdateBuffer( RmCast(buffer), offset, size, ctx.getMem( data, size ));
	}

	void  CmdBuf_Bake::FillBufferCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.FillBuffer( RmCast(buffer), offset, size, data );
	}

	void  CmdBuf_Bake::CopyBufferCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.CopyBuffer( RmCast(srcBuffer), RmCast(dstBuffer), ranges );
	}

	void  CmdBuf_Bake::CopyImageCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.CopyImage( RmCast(srcImage), RmCast(dstImage), ranges );
	}

	void  CmdBuf_Bake::CopyBufferToImageCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.CopyBufferToImage( RmCast(srcBuffer), RmCast(dstImage), ranges );
	}

	void  CmdBuf_Bake::CopyImageToBufferCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.CopyImageToBuffer( RmCast(srcImage), RmCast(dstBuffer), ranges );
	}

	void  CmdBuf_Bake::CopyBufferToImage2Cmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.CopyBufferToImage( RmCast(srcBuffer), RmCast(dstImage), ranges );
	}

	void  CmdBuf_Bake::CopyImageToBuffer2Cmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.CopyImageToBuffer( RmCast(srcImage), RmCast(dstBuffer), ranges );
	}

	void  CmdBuf_Bake::BlitImageCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.BlitImage( RmCast(srcImage), RmCast(dstImage), filter, regions );
	}

	void  CmdBuf_Bake::ResolveImageCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.ResolveImage( RmCast(srcImage), RmCast(dstImage), regions );
	}

	void  CmdBuf_Bake::GenerateMipmapsCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		ctx.transfer.GenerateMipmaps( RmCast(image), ranges, state );
	}

	void  CmdBuf_Bake::UpdateHostBufferCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );
		CHECK_THROW( ctx.transfer.UpdateHostBuffer( RmCast(buffer), offset, size, ctx.getMem( memOffset, size )));
	}

	void  CmdBuf_Bake::ReadHostBufferCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Transfer );

		void*	mapped = null;
		CHECK_THROW( ctx.transfer.MapHostBuffer( RmCast(buffer), offset, INOUT size, OUT mapped ));
		ctx.addMem( mapped, hostPtr, size );
	}

	//-------------------------------------------------
	// compute commands

	void  CmdBuf_Bake::Compute_BindPipelineCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Compute );
		ctx.compute.BindPipeline( RmCast(ppln) );
	}

	void  CmdBuf_Bake::Compute_BindDescriptorSetCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Compute );
		ctx.compute.BindDescriptorSet( index, RmCast(ds), dynamicOffsets );
	}

	void  CmdBuf_Bake::Compute_PushConstantCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Compute );
		ctx.compute.PushConstant( PushConstantIndex{pcIndex_offset, pcIndex_stage}, ArraySizeOf(data), data.data(), ShaderStructName{typeName} );
	}

	void  CmdBuf_Bake::Compute_Dispatch::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Compute );
		ctx.compute.Dispatch( groupCount );
	}

	void  CmdBuf_Bake::Compute_DispatchIndirect::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Compute );
		ctx.compute.DispatchIndirect( RmCast(buffer), offset );
	}

	//-------------------------------------------------
	// graphics commands

	void  CmdBuf_Bake::Graphics_BeginRenderPass::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Graphics );

		RenderPassDesc	rp_desc;
		ctx.rmdev._Convert( this->desc, OUT rp_desc );

		PlacementNew<DirectCtx::Draw>( INOUT &ctx.draw, ctx.graphics.BeginRenderPass( rp_desc, dbgLabel ));
		ctx.type = EContextType::RenderPass;
	}

	void  CmdBuf_Bake::Graphics_NextSubpass::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		PlacementNew<DirectCtx::Draw>( INOUT &ctx.draw, ctx.graphics.NextSubpass( INOUT ctx.draw, dbgLabel ));
	}

	void  CmdBuf_Bake::Graphics_EndRenderPass::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.graphics.EndRenderPass( INOUT ctx.draw );
		PlacementDelete( ctx.draw );
		ctx.type = EContextType::Graphics;
	}

	void  CmdBuf_Bake::Graphics_BeginMtRenderPass::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Graphics );

		RenderPassDesc	rp_desc;
		ctx.rmdev._Convert( this->desc, OUT rp_desc );

		auto	batch = ctx.graphics.BeginMtRenderPass( rp_desc, dbgLabel );
		ctx.rmdev._Set( batchId, RVRef(batch) );
	}

	void  CmdBuf_Bake::Graphics_NextMtSubpass::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Graphics );

		auto	prev_batch = ctx.rmdev._Get( prevBatchId );
		CHECK_THROW( prev_batch );

		auto	batch = ctx.graphics.NextMtSubpass( *prev_batch, dbgLabel );
		ctx.rmdev._Set( batchId, RVRef(batch) );
	}

	void  CmdBuf_Bake::Graphics_EndMtRenderPass::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Graphics );
		ctx.graphics.EndMtRenderPass();
	}

	void  CmdBuf_Bake::Graphics_ExecuteSecondary::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::Graphics );

		auto	batch = ctx.rmdev._Get( batchId );
		CHECK_THROW( batch );

		ctx.graphics.ExecuteSecondary( *batch );

		for (auto cmdbuf : cmdbufs) {
			ctx.rmdev._Remove( cmdbuf );
		}
	}

	//-------------------------------------------------
	// draw commands

	void  CmdBuf_Bake::Draw_BindGraphicsPipelineCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.BindPipeline( RmCast(ppln) );
	}

	void  CmdBuf_Bake::Draw_BindMeshPipelineCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.BindPipeline( RmCast(ppln) );
	}

	void  CmdBuf_Bake::Draw_BindTilePipelineCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.BindPipeline( RmCast(ppln) );
	}

	void  CmdBuf_Bake::Draw_BindDescriptorSetCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.BindDescriptorSet( index, RmCast(ds), dynamicOffsets );
	}

	void  CmdBuf_Bake::Draw_PushConstantCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.PushConstant( PushConstantIndex{pcIndex_offset, pcIndex_stage}, ArraySizeOf(data), data.data(), ShaderStructName{typeName} );
	}

	void  CmdBuf_Bake::Draw_SetViewportsCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetViewports( viewports );
	}

	void  CmdBuf_Bake::Draw_SetScissorsCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetScissors( scissors );
	}

	void  CmdBuf_Bake::Draw_SetDepthBiasCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetDepthBias( depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}

	void  CmdBuf_Bake::Draw_SetStencilCompareMaskCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetStencilCompareMask( frontCompareMask, backCompareMask );
	}

	void  CmdBuf_Bake::Draw_SetStencilWriteMaskCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetStencilWriteMask( frontWriteMask, backWriteMask );
	}

	void  CmdBuf_Bake::Draw_SetStencilReferenceCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetStencilReference( frontReference, backReference );
	}

	void  CmdBuf_Bake::Draw_SetBlendConstantsCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetBlendConstants( color );
	}

	void  CmdBuf_Bake::Draw_SetDepthBoundsCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetDepthBounds( minDepthBounds, maxDepthBounds );
	}

	void  CmdBuf_Bake::Draw_SetFragmentShadingRateCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.SetFragmentShadingRate( rate, primitiveOp, textureOp );
	}

	void  CmdBuf_Bake::Draw_BindIndexBufferCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.BindIndexBuffer( RmCast(buffer), offset, indexType );
	}

	void  CmdBuf_Bake::Draw_BindVertexBuffersCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.BindVertexBuffers( firstBinding, RmCast(buffers), offsets );
	}

	void  CmdBuf_Bake::DrawCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.Draw( vertexCount, instanceCount, firstVertex, firstInstance );
	}

	void  CmdBuf_Bake::DrawIndexedCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawIndexed( indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
	}

	void  CmdBuf_Bake::DrawIndirectCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawIndirect( RmCast(indirectBuffer), indirectBufferOffset, drawCount, stride );
	}

	void  CmdBuf_Bake::DrawIndexedIndirectCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawIndexedIndirect( RmCast(indirectBuffer), indirectBufferOffset, drawCount, stride );
	}

	void  CmdBuf_Bake::DrawMeshTasksCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawMeshTasks( taskCount );
	}

	void  CmdBuf_Bake::DrawMeshTasksIndirectCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawMeshTasksIndirect( RmCast(indirectBuffer), indirectBufferOffset, drawCount, stride );
	}

	void  CmdBuf_Bake::DrawIndirectCountCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawIndirectCount( RmCast(indirectBuffer), indirectBufferOffset, RmCast(countBuffer), countBufferOffset, maxDrawCount, stride );
	}

	void  CmdBuf_Bake::DrawIndexedIndirectCountCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawIndexedIndirectCount( RmCast(indirectBuffer), indirectBufferOffset, RmCast(countBuffer), countBufferOffset, maxDrawCount, stride );
	}

	void  CmdBuf_Bake::DrawMeshTasksIndirectCountCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DrawMeshTasksIndirectCount( RmCast(indirectBuffer), indirectBufferOffset, RmCast(countBuffer), countBufferOffset, maxDrawCount, stride );
	}

	void  CmdBuf_Bake::DispatchTileCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.DispatchTile();
	}

	void  CmdBuf_Bake::Draw_AttachmentBarrierCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.AttachmentBarrier( AttachmentName{name}, srcState, dstState );
	}

	void  CmdBuf_Bake::Draw_CommitBarriersCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		ctx.draw.CommitBarriers();
	}

	void  CmdBuf_Bake::Draw_ClearAttachmentCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RenderPass );
		Visit( clearValue,
			[&](const RGBA32f &c)		{ ctx.draw.ClearAttachment( AttachmentName{name}, c, region, baseLayer, layerCount ); },
			[&](const RGBA32u &c)		{ ctx.draw.ClearAttachment( AttachmentName{name}, c, region, baseLayer, layerCount ); },
			[&](const RGBA32i &c)		{ ctx.draw.ClearAttachment( AttachmentName{name}, c, region, baseLayer, layerCount ); },
			[&](const DepthStencil &c)	{ ctx.draw.ClearAttachment( AttachmentName{name}, c, region, baseLayer, layerCount ); },
			[&](NullUnion)				{}
		);
	}

	//-------------------------------------------------
	// acceleration structure build commands

	void  CmdBuf_Bake::ASBuild_BuildGeometryCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.Build( build, RmCast(dstId) );
	}

	void  CmdBuf_Bake::ASBuild_BuildSceneCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.Build( build, RmCast(dstId) );
	}

	void  CmdBuf_Bake::ASBuild_UpdateGeometryCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.Update( build, RmCast(srcId), RmCast(dstId) );
	}

	void  CmdBuf_Bake::ASBuild_UpdateSceneCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.Update( build, RmCast(srcId), RmCast(dstId) );
	}

	void  CmdBuf_Bake::ASBuild_CopyGeometryCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.Copy( RmCast(srcId), RmCast(dstId), mode );
	}

	void  CmdBuf_Bake::ASBuild_CopySceneCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.Copy( RmCast(srcId), RmCast(dstId), mode );
	}

	void  CmdBuf_Bake::ASBuild_WriteGeometryPropertyCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.WriteProperty( property, RmCast(as), RmCast(dstBuffer), offset, size );
	}

	void  CmdBuf_Bake::ASBuild_WriteScenePropertyCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::ASBuild );
		ctx.asBuild.WriteProperty( property, RmCast(as), RmCast(dstBuffer), offset, size );
	}

	//-------------------------------------------------
	// ray tracing commands

	void  CmdBuf_Bake::RayTracing_BindPipelineCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RayTracing );
		ctx.rayTracing.BindPipeline( RmCast(ppln) );
	}

	void  CmdBuf_Bake::RayTracing_BindDescriptorSetCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RayTracing );
		ctx.rayTracing.BindDescriptorSet( index, RmCast(ds), dynamicOffsets );
	}

	void  CmdBuf_Bake::RayTracing_PushConstantCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RayTracing );
		ctx.rayTracing.PushConstant( PushConstantIndex{pcIndex_offset, pcIndex_stage}, ArraySizeOf(data), data.data(), ShaderStructName{typeName} );
	}

	void  CmdBuf_Bake::RayTracing_SetStackSizeCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RayTracing );
		ctx.rayTracing.SetStackSize( size );
	}

	void  CmdBuf_Bake::RayTracing_TraceRaysCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RayTracing );
		ctx.rayTracing.TraceRays( dim, RmCast(sbt) );
	}

	void  CmdBuf_Bake::RayTracing_TraceRaysIndirectCmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RayTracing );
		ctx.rayTracing.TraceRaysIndirect( RmCast(sbt), RmCast(indirectBuffer), indirectOffset );
	}

	void  CmdBuf_Bake::RayTracing_TraceRaysIndirect2Cmd::Execute (void* inCtx) __Th___
	{
		auto&	ctx = *Cast<CmdCtx>(inCtx);
		CHECK_THROW( ctx.type == EContextType::RayTracing );
		ctx.rayTracing.TraceRaysIndirect2( RmCast(indirectBuffer), indirectOffset );
	}

} // AE::RemoteGraphics::Msg
