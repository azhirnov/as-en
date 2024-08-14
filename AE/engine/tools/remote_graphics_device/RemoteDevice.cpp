// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "remote_graphics_device/RemoteDevice.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/ApplicationAndroid.h"
#endif

namespace AE::RemoteGraphics
{
/*
=================================================
	OnSurfaceDestroyed
=================================================
*/
	void  RmGWndListener::OnSurfaceDestroyed (IWindow &) __NE___
	{
		_device._StopServer();
	}

/*
=================================================
	OnSurfaceCreated
=================================================
*/
	void  RmGWndListener::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		_device._nativeWnd = wnd.GetNative();
		CHECK_FATAL( _device._nativeWnd );

		_device._windowSize.store( ushort2{wnd.GetSurfaceSize()} );

		_device._StartServer();
	}

/*
=================================================
	OnStateChanged
=================================================
*/
	void  RmGWndListener::OnStateChanged (IWindow &wnd, EState state) __NE___
	{
		switch_enum( state )
		{
			case EState::Focused :
				_device._PrintSelfIP();

			case EState::InForeground :
				_device._windowSize.store( ushort2{wnd.GetSurfaceSize()} );
				break;

			case EState::InBackground :
			case EState::Stopped :
			case EState::Destroyed :
				_device._windowSize.store( ushort2{0} );	break;

			case EState::Unknown :
			case EState::Created :
			case EState::Started :
				break;
		}
		switch_end
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RmGAppListener::RmGAppListener () __NE___ :
		_device{ True{"enable log"} },
		_connectionLostTimer{ seconds{30} }
	{
		TaskScheduler::InstanceCtor::Create();

		CHECK_FATAL( Networking::SocketService::Instance().Initialize() );

		VFS::VirtualFileSystem::InstanceCtor::Create();

		TaskScheduler::Config	cfg;
		CHECK_FATAL( Scheduler().Setup( cfg ));

		CHECK_FATAL( _RegisterMessages() );
	}

/*
=================================================
	destructor
=================================================
*/
	RmGAppListener::~RmGAppListener () __NE___
	{
		Scheduler().Release();

		VFS::VirtualFileSystem::InstanceCtor::Destroy();
		Networking::SocketService::Instance().Deinitialize();

		TaskScheduler::InstanceCtor::Destroy();
	}

/*
=================================================
	OnStart
=================================================
*/
	void  RmGAppListener::OnStart (IApplication &app) __NE___
	{
		_app = &app;

		WindowDesc	desc;
		desc.size	= {1600, 896};
		desc.title	= "RemoteGraphicsDevice";
		desc.mode	= EWindowMode::Resizable;

		_window = app.CreateWindow( MakeUnique<RmGWndListener>( *this ), desc );
		CHECK_FATAL( _window );
	}

/*
=================================================
	BeforeWndUpdate
=================================================
*/
	void  RmGAppListener::BeforeWndUpdate (IApplication &) __NE___
	{
		Unused( Scheduler().ProcessTasks( EThreadArray{ EThread::PerFrame, EThread::Renderer, EThread::Background }, EThreadSeed(0) ));

		/*if ( _hasConnection.load() and _connectionLostTimer->Tick() )
		{
			AE_LOGW( "connection lost, restart..." );
			_restartServer.store( true );
		}*/

		if_unlikely( _restartServer.load() )
			_RestartServer();
	}

/*
=================================================
	AfterWndUpdate
=================================================
*/
	void  RmGAppListener::AfterWndUpdate (IApplication &app) __NE___
	{
		if ( _window and _window->GetState() == IWindow::EState::Destroyed )
		{
			_StopServer();
			app.Terminate();
		}

		if ( _window )
			_windowSize.store( ushort2{_window->GetSurfaceSize()} );

		ThreadUtils::Sleep_15ms();
	}

/*
=================================================
	OnStop
=================================================
*/
	void  RmGAppListener::OnStop (IApplication &) __NE___
	{
		_window		= null;
		_app		= null;
		_nativeWnd	= Default;
	}

/*
=================================================
	_PrintSelfIP
=================================================
*/
	void  RmGAppListener::_PrintSelfIP ()
	{
		using namespace AE::Networking;

		IpAddress	server_addr;
		CHECK( SocketService::Instance().GetSelfIPAddress( IpAddress::FromServiceUDP( "192.168.0.1", "8080" ), OUT server_addr ));

		server_addr.SetPort( 0 );

		AE_LOGI( "Start RemoteGraphicsDevice on address: "s << server_addr.ToString() );

	  #ifdef AE_PLATFORM_ANDROID
		if ( _app )
			Cast<App::ApplicationAndroid>(_app)->ShowToast( server_addr.ToString(), False{"short"} );
	  #endif
	}

/*
=================================================
	_StartServer
----
	main thread
=================================================
*/
	void  RmGAppListener::_StartServer ()
	{
		_res.semaphoreMap->reserve( _res.semaphorePool.capacity() );

		_PrintSelfIP();

		for (usize i = 0; i < _threadArr.size(); ++i)
		{
			_threadArr[i].looping.store( true );
			_threadArr[i].thread = StdThread{ [this, i] ()
			{
				auto&	looping	= _threadArr[i].looping;
				auto&	conn	= _threadArr[i].conn;

				CHECK_ERRV( conn.InitServer( ushort(RmNetConfig::serverPort + i), &_objFactory ));

				_GetThreadData() = &_threadArr[i];

				try {
					for (; looping.load();)
					{
						if ( conn.WaitForClient() )
							break;
					}

					if ( not _hasConnection.exchange( true ))
						_connectionLostTimer->Restart();

					for (; looping.load();)
					{
						bool	ok = conn.Receive();

						if ( auto msg = conn.Encode() )
						{
							_ProcessMessage( *msg );
							ok = true;
						}

						if ( ok )
							_connectionLostTimer->Restart();
						else
							ThreadUtils::Sleep_1us();

					}
				}
				catch (...)
				{
					_RequireRestartServer();
				}

				_GetThreadData() = null;
				conn.Close();
			}};
		}
	}

/*
=================================================
	_RequireRestartServer
=================================================
*/
	void  RmGAppListener::_RequireRestartServer ()
	{
		for (auto& t : _threadArr)
			t.looping.store( false );

		_restartServer.store( true );
	}

/*
=================================================
	_RestartServer
----
	main thread
=================================================
*/
	void  RmGAppListener::_RestartServer ()
	{
		_restartServer.store( false );

		_StopServer();

		AE_LOGI( "\n--------------------------------\nRestart\n--------------------------------" );

		for (auto& dev_to_host : _devToHostCopy) {
			dev_to_host.Write( Default );
		}
		_res.gfxMemAllocPool.UnassignAll();
		_res.dsAllocPool.UnassignAll();
		_res.rtechPplnPool.UnassignAll();
		_res.cmdbufPool.UnassignAll();
		_res.batchPool.UnassignAll();
		_res.drawCmdbufPool.UnassignAll();
		_res.semaphorePool.UnassignAll();
		_res.semaphoreMap->clear();
		_res.drawBatchMap.clear();
		_res.queryPool.UnassignAll();
		_res.queryMap->clear();

		_StartServer();
	}

/*
=================================================
	_StopServer
----
	main thread
=================================================
*/
	void  RmGAppListener::_StopServer ()
	{
		_hasConnection.store( false );

		for (auto& t : _threadArr)
			t.looping.store( false );

		for (auto& t : _threadArr)
		{
			if ( t.thread.joinable() )
				t.thread.join();
		}
		//----------------------------

	  #ifdef AE_ENABLE_ARM_PMU
		_profilers.arm.Deinitialize();
	  #endif
	  #ifdef AE_ENABLE_MALI_HWCPIPE
		_profilers.mali.Deinitialize();
	  #endif
	  #ifdef AE_ENABLE_ADRENO_PERFCOUNTER
		_profilers.adreno.Deinitialize();
	  #endif
	  #ifdef AE_ENABLE_PVRCOUNTER
		_profilers.pvr.Deinitialize();
	  #endif
	  #ifdef AE_ENABLE_NVML
		_profilers.nv.Deinitialize();
	  #endif

		_swapchain.Destroy();
		_swapchain.DestroySurface();

		if ( _resMngr != null )
		{
			GraphicsScheduler().Deinitialize();
			RenderTaskScheduler::InstanceCtor::Destroy();
		}
		_resMngr = null;

	  #ifdef AE_ENABLE_VULKAN
		_syncLog.Deinitialize( INOUT _device.EditDeviceFnTable() );
	  #endif

		_device.DestroyLogicalDevice();
		_device.DestroyInstance();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_ProcessMessage
=================================================
*/
	void  RmGAppListener::_ProcessMessage (Msg::BaseMsg &msg)
	{
		auto	id = TypeIdOf( msg );
		auto	it = _cbMap.find( id );

		if_unlikely( it == _cbMap.end() )
			RETURN_ERRV( "Callback for command '"s << id.Name() << "' is not found" );

		//AE_LOG_DBG( "Process "s << id.Name() );

		auto	fn = it->second.Ref< void (RmGAppListener::*)(Msg::BaseMsg &) >();
		(this->*fn)( msg );
	}

/*
=================================================
	_GetThreadData
=================================================
*/
	RmGAppListener::PerThreadData*&  RmGAppListener::_GetThreadData ()
	{
		static thread_local PerThreadData*	data = null;
		return data;
	}

/*
=================================================
	_Send
=================================================
*/
	void  RmGAppListener::_Send (const void *data, Bytes dataSize) __Th___
	{
		auto*	td = _GetThreadData();
		CHECK_THROW( td != null );
		CHECK_THROW( td->conn.Send( data, dataSize ));
	}

	void  RmGAppListener::_Send (const Msg::BaseResponse &msg) __Th___
	{
		auto*	td = _GetThreadData();
		CHECK_THROW( td != null );
		CHECK_THROW( td->conn.Send( msg ));
	}

/*
=================================================
	_ReadReceived
=================================================
*/
	void  RmGAppListener::_ReadReceived (OUT void* data, Bytes size) __Th___
	{
		auto*	td = _GetThreadData();
		CHECK_THROW( td != null );
		CHECK_THROW( td->conn.ReadReceived( OUT data, size, size, td->looping ) == size );
	}

/*
=================================================
	_PushMemStack
=================================================
*/
	void  RmGAppListener::_PushMemStack (RC<SharedMem> obj) __Th___
	{
		auto*	td = _GetThreadData();
		CHECK_THROW( td != null );
		td->memStack.push_back( RVRef(obj) );
	}

/*
=================================================
	_GetMemStack
=================================================
*/
	Array<RC<SharedMem>>  RmGAppListener::_GetMemStack () __Th___
	{
		auto*	td = _GetThreadData();
		CHECK_THROW( td != null );
		CHECK_THROW( td->memStack.size() > 0 );

		Array<RC<SharedMem>>	temp;
		std::swap( td->memStack, temp );

		return temp;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Get
=================================================
*/
	template <typename T, typename ID, typename P>
	T  RmGAppListener::_Get2 (ID id, P& pool)
	{
		if ( auto* item = pool.At( id.Index() );  item and item->gen.load() == id.Generation() )
			return item->ptr;

		return Default;
	}

	GfxMemAllocatorPtr  RmGAppListener::_Get (RmGfxMemAllocatorID id)
	{
		return _Get2<GfxMemAllocatorPtr>( id, _res.gfxMemAllocPool );
	}

	DescriptorAllocatorPtr  RmGAppListener::_Get (RmDescriptorAllocatorID id)
	{
		return _Get2<DescriptorAllocatorPtr>( id, _res.dsAllocPool );
	}

	RenderTechPipelinesPtr  RmGAppListener::_Get (RmRenderTechPipelinesID id)
	{
		return _Get2<RenderTechPipelinesPtr>( id, _res.rtechPplnPool );
	}

	RC<RmGAppListener::RenderTaskImpl>  RmGAppListener::_Get (RmCommandBufferID id)
	{
		return _Get2<RC<RenderTaskImpl>>( id, _res.cmdbufPool );
	}

	CommandBatchPtr  RmGAppListener::_Get (RmCommandBatchID id)
	{
		return _Get2<CommandBatchPtr>( id, _res.batchPool );
	}

	RC<RmGAppListener::DrawTaskImpl>  RmGAppListener::_Get (RmDrawCommandBufferID id)
	{
		return _Get2<RC<DrawTaskImpl>>( id, _res.drawCmdbufPool );
	}

	RmGAppListener::GpuSemaphore  RmGAppListener::_GetSemaphore (RmSemaphoreID id)
	{
		return _Get2<GpuSemaphore>( id, _res.semaphorePool );
	}

	RmGAppListener::GpuQuery  RmGAppListener::_GetQuery (RmQueryID id)
	{
		return _Get2<GpuQuery>( id, _res.queryPool );
	}

/*
=================================================
	_Set
=================================================
*/
	template <typename ID, typename T, typename P>
	ID  RmGAppListener::_Set2 (T ptr, P& pool)
	{
		Index_t	idx;
		CHECK_ERR( pool.Assign( OUT idx ));

		Index_t	gen = pool[idx].gen.load();
		pool[idx].ptr = RVRef(ptr);

		return ID{ idx, gen };
	}

	RmGfxMemAllocatorID  RmGAppListener::_Set (GfxMemAllocatorPtr ptr)
	{
		CHECK_ERR( ptr );
		return _Set2<RmGfxMemAllocatorID>( RVRef(ptr), _res.gfxMemAllocPool );
	}

	RmDescriptorAllocatorID  RmGAppListener::_Set (DescriptorAllocatorPtr ptr)
	{
		CHECK_ERR( ptr );
		return _Set2<RmDescriptorAllocatorID>( RVRef(ptr), _res.dsAllocPool );
	}

	RmRenderTechPipelinesID  RmGAppListener::_Set (RenderTechPipelinesPtr ptr)
	{
		CHECK_ERR( ptr );
		return _Set2<RmRenderTechPipelinesID>( RVRef(ptr), _res.rtechPplnPool );
	}

	RmCommandBufferID  RmGAppListener::_Set (RC<RenderTaskImpl> ptr)
	{
		CHECK_ERR( ptr );
		return _Set2<RmCommandBufferID>( RVRef(ptr), _res.cmdbufPool );
	}

	RmCommandBatchID  RmGAppListener::_Set (CommandBatchPtr ptr)
	{
		CHECK_ERR( ptr );
		return _Set2<RmCommandBatchID>( RVRef(ptr), _res.batchPool );
	}

	RmDrawCommandBufferID  RmGAppListener::_Set (RC<DrawTaskImpl> ptr)
	{
		CHECK_ERR( ptr );
		return _Set2<RmDrawCommandBufferID>( RVRef(ptr), _res.drawCmdbufPool );
	}

/*
=================================================
	_SetQuery
=================================================
*/
	RmQueryID  RmGAppListener::_SetQuery (GpuQuery query)
	{
	  #if defined(AE_ENABLE_VULKAN)
		CHECK_ERR( query != Default );
	  #elif defined(AE_ENABLE_METAL)
		CHECK_ERR( query );
	  #endif

		{
			auto	map = _res.queryMap.ReadLock();
			auto	it  = map->find( query );
			if ( it != map->end() )
				return it->second;
		}

		RmQueryID	id = _Set2<RmQueryID>( RVRef(query), _res.queryPool );
		RmQueryID	result;
		bool		inserted;

		{
			auto	map = _res.queryMap.WriteLock();

			auto [it, inserted2] = map->emplace( query, id );
			result		= it->second;
			inserted	= inserted2;
		}

		if_unlikely( not inserted )
			_Remove2( id, _res.queryPool );

		return result;
	}

/*
=================================================
	_SetSemaphore
=================================================
*/
	RmSemaphoreID  RmGAppListener::_SetSemaphore (GpuSemaphore sem)
	{
	  #if defined(AE_ENABLE_VULKAN)
		CHECK_ERR( sem != Default );
	  #elif defined(AE_ENABLE_METAL)
		CHECK_ERR( sem );
	  #endif

		{
			auto	map = _res.semaphoreMap.ReadLock();
			auto	it  = map->find( sem );
			if ( it != map->end() )
				return it->second;
		}

		RmSemaphoreID	id = _Set2<RmSemaphoreID>( RVRef(sem), _res.semaphorePool );
		RmSemaphoreID	result;
		bool			inserted;

		{
			auto	map = _res.semaphoreMap.WriteLock();

			auto [it, inserted2] = map->emplace( sem, id );
			result		= it->second;
			inserted	= inserted2;
		}

		if_unlikely( not inserted )
			_Remove2( id, _res.semaphorePool );

		return result;
	}

/*
=================================================
	_Remove
=================================================
*/
	template <typename ID, typename P>
	void  RmGAppListener::_Remove2 (ID id, P& pool)
	{
		if ( auto* item = pool.At( id.Index() );  item and item->gen.load() == id.Generation() )
		{
			item->gen.Inc();
			item->ptr = Default;
			pool.Unassign( id.Index() );
		}
	}

	void  RmGAppListener::_Remove (RmGfxMemAllocatorID id)
	{
		_Remove2( id, _res.gfxMemAllocPool );
	}

	void  RmGAppListener::_Remove (RmDescriptorAllocatorID id)
	{
		_Remove2( id, _res.dsAllocPool );
	}

	void  RmGAppListener::_Remove (RmRenderTechPipelinesID id)
	{
		_Remove2( id, _res.rtechPplnPool );
	}

	void  RmGAppListener::_Remove (RmCommandBufferID id)
	{
		_Remove2( id, _res.cmdbufPool );
	}

	void  RmGAppListener::_Remove (RmCommandBatchID id)
	{
		_Remove2( id, _res.batchPool );
	}

	void  RmGAppListener::_Remove (RmDrawCommandBufferID id)
	{
		_Remove2( id, _res.drawCmdbufPool );
	}

/*
=================================================
	_Get / _Set / _Remove (RmDrawCommandBatchID)
=================================================
*/
	DrawCommandBatchPtr  RmGAppListener::_Get (RmDrawCommandBatchID id)
	{
		auto	it = _res.drawBatchMap.find( id );
		if ( it != _res.drawBatchMap.end() )
			return it->second;

		return null;
	}

	void  RmGAppListener::_Set (RmDrawCommandBatchID id, DrawCommandBatchPtr ptr) __Th___
	{
		CHECK_THROW( ptr );
		_res.drawBatchMap.insert_or_assign( id, RVRef(ptr) );
	}

	void  RmGAppListener::_Remove (RmDrawCommandBatchID id)
	{
		_res.drawBatchMap.erase( id );
	}

/*
=================================================
	_Convert
=================================================
*/
	void  RmGAppListener::_Convert (const Msg::CmdBuf_Bake::Graphics_BeginRenderPass::SerRenderPassDesc &srcDesc, OUT RenderPassDesc &dstDesc)
	{
		for (auto& src : srcDesc.attachments)
		{
			ASSERT( src.name.IsDefined() );

			RenderPassDesc::Attachment	dst;
			dst.imageView	= RmCast( src.imageView );
			dst.clearValue	= src.clearValue;
			dst.initial		= src.initial;
			dst.final		= src.final;
			dst.relaxedStateTransition = src.relaxedStateTransition;
			dstDesc.attachments.emplace( AttachmentName{src.name}, dst );
		}
		dstDesc.viewports		= srcDesc.viewports;
		dstDesc.area			= srcDesc.area;
		dstDesc.layerCount		= srcDesc.layerCount;
		dstDesc.renderPassName	= RenderPassName{srcDesc.renderPassName};
		dstDesc.subpassName		= SubpassName{srcDesc.subpassName};
		dstDesc.packId			= RmCast( srcDesc.packId );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	LogToHost::Process
=================================================
*/
	ILogger::EResult  RmGAppListener::LogToHost::Process (const MessageInfo &info) __Th___
	{
		Msg::Log  msg;
		msg.message		= info.message;
		msg.func		= info.func;
		msg.file		= info.file;
		msg.line		= info.line;
		msg.level		= info.level;
		msg.scope		= info.scope;

		auto*	td = _GetThreadData();
		if ( td != null and td->looping.load() )
			Unused( td->conn.Send( msg ));

		return EResult::Continue;
	}

} // AE::RemoteGraphics
//-----------------------------------------------------------------------------


using namespace AE::Base;
using namespace AE::App;
using namespace AE::RemoteGraphics;


Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
	StaticLogger::InitDefault();
	return MakeUnique<RmGAppListener>();
}

void  AE_OnAppDestroyed ()
{
	StaticLogger::Deinitialize( true );
}
//-----------------------------------------------------------------------------


#ifdef AE_PLATFORM_ANDROID
#	include "platform/Android/ApplicationAndroid.h"

	extern "C" JNIEXPORT jint  JNI_OnLoad (JavaVM* vm, void*)
	{
		return ApplicationAndroid::OnJniLoad( vm );
	}

	extern "C" void JNI_OnUnload (JavaVM* vm, void *)
	{
		return ApplicationAndroid::OnJniUnload( vm );
	}

#endif // AE_PLATFORM_ANDROID
