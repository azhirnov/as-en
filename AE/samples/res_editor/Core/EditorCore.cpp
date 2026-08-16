// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "vfs/Disk/DiskStaticStorage.h"
#include "vfs/Disk/DiskDynamicStorage.h"
#include "vfs/Network/Messages.h"

#include "Core/EditorCore.h"
#include "Scripting/ScriptExe.h"


#if AE_PORTABLE_APP and AE_GRAPHICS_STRONG_VALIDATION == 0
# error AE_GRAPHICS_STRONG_VALIDATION must be enabled for public version
#endif

#if defined(AE_ENABLE_VULKAN) and not defined(AE_CFG_RELEASE)
# define ENABLE_RDC		1
#else
# define ENABLE_RDC		0
#endif

namespace AE::ResEditor
{
	using namespace AE::Threading;

namespace
{

/*
=================================================
	GetAppConfig
=================================================
*/
	static AppV1::AppConfig  GetInitialAppConfig ()
	{
		AppV1::AppConfig	cfg;

		// threading
		{
			cfg.threading.maxThreads			= 2;
			cfg.threading.maxIOAccessThreads	= 1;
			cfg.threading.mask					= { EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::IO };
		}

		cfg.enableNetwork = true;

		return cfg;
	}

	static AppV1::AppConfig  GetAppConfig ()
	{
		AppV1::AppConfig	cfg		= GetInitialAppConfig();
		const auto&			re_cfg	= ResEditorAppConfig::Get();

		// graphics
		{
			cfg.graphics.maxFrames				= 2;

			cfg.graphics.device.appName			= "ResourceEditor";
			cfg.graphics.device.requiredQueues	= EQueueMask::Graphics;
			cfg.graphics.device.optionalQueues	= Default;
			cfg.graphics.device.devFlags		= (re_cfg.setStableGPUClock ? EDeviceFlags::SetStableClock : Default) |
												  (re_cfg.enableRenderDoc ? EDeviceFlags::EnableRenderDoc : Default) |
												  EDeviceFlags::EnablePerfCounters;
			cfg.graphics.device.deviceName		= re_cfg.deviceName;

		  #ifdef AE_CFG_RELEASE
			cfg.graphics.device.validation		= EDeviceValidation::Disabled;
		  #else
			cfg.graphics.device.validation		= re_cfg.gapiValidation;
		  #endif

			cfg.graphics.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;

			cfg.graphics.swapchain.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
			cfg.graphics.swapchain.options		= EImageOpt::BlitDst;
			cfg.graphics.swapchain.presentMode	= EPresentMode::FIFO;		// vsync
			cfg.graphics.swapchain.minImageCount= 2;
			cfg.graphics.swapchain.usePreTransform = false;
		//	cfg.graphics.swapchain.scaling		= EPresentScaling::AspectRatioStretch;
		//	cfg.graphics.swapchain.scale		= 0.5f;

			cfg.graphics.useRenderGraph			= true;

			for (usize i = 0, cnt = Min( cfg.graphics.driverList.size(), re_cfg.driverList.size() ); i < cnt; ++i) {
				cfg.graphics.driverList[i] = re_cfg.driverList[i];
			}

			// custom size for staging
		//	cfg.graphics.staging.maxWriteDynamicSize = 256_MiB;
			cfg.graphics.staging.maxReadDynamicSize	 = 128_MiB;

		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			cfg.graphics.staging.maxWriteDynamicSize = 8_MiB;	// limited by network bandwidth
			cfg.graphics.staging.maxReadDynamicSize	 = 8_MiB;
			cfg.graphics.staging.dynamicBlockSize	 = 8_MiB;

			cfg.window.mode							= EWindowMode::NonResizable;
			cfg.graphics.maxFrames					= 2;
			cfg.graphics.swapchain.minImageCount	= 2;
			cfg.graphics.graphicsLibPath			= re_cfg.graphicsLibPath;
			cfg.graphics.enableSyncLog				= false;

			cfg.graphics.deviceAddr	= Networking::IpAddress::FromInt( re_cfg.ipAddress[0], re_cfg.ipAddress[1], re_cfg.ipAddress[2], re_cfg.ipAddress[3], re_cfg.ipPort );
			CHECK_THROW_MSG( cfg.graphics.deviceAddr.IsValid(),
				"Invalid RemoveDevice IP address, in 'res_editor_cfg.as' set 'cfg.RemoteDeviceIpAddress(...)' to an existing IP address" );
		  #endif

			CHECK( cfg.graphics.maxFrames <= cfg.graphics.swapchain.minImageCount );
		}

		// window
		{
			auto&	window	= cfg.window;
			window.title	= "ResourceEditor";
			window.size		= uint2{ re_cfg.screenWidth, re_cfg.screenHeight };
			window.mode		= re_cfg.windowMode;
			window.monitorId = Monitor::ID( re_cfg.monitorId );
		}

		// VR
		{
			auto&	vr				= cfg.vr;
			vr.enableVR				= false;
			vr.imageDesc.dimension	= ImageDim2_t{2048};
			vr.imageDesc.colorFormat= EPixelFormat::BGRA8_UNorm;
			vr.imageDesc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer;	// default
			vr.imageDesc.options	= EImageOpt::BlitDst;

		//	vr.devices.push_back( IVRSession::EDeviceType::OpenXR );
		//	vr.devices.push_back( IVRSession::EDeviceType::OpenVR );
			vr.devices.push_back( IVRSession::EDeviceType::Emulator );
		}

		return cfg;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ResEditorApplication::ResEditorApplication () __NE___ :
		AppCoreV1{ GetInitialAppConfig(), MakeRCTh<ResEditorCore>() }
	{
		Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DisplayAlwaysOn ));
	}

/*
=================================================
	destructor
=================================================
*/
	ResEditorApplication::~ResEditorApplication () __NE___
	{
		Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::Default ));
	}

/*
=================================================
	_InitVFS
=================================================
*/
	bool  ResEditorApplication::_InitVFS () __NE___
	{
		CHECK_ERR( _app );

		ResEditorAppConfig::Init( *_app );

		auto&	re_cfg = ResEditorAppConfig::Get();

		for (auto& [path, in_prefix] : re_cfg.vfsPaths)
		{
			ASSERT( not in_prefix.empty() );
			String	prefix = in_prefix;

			if ( prefix.empty() )			continue;
			if ( prefix.back() != '/' )		prefix << '/';

			CHECK_ERR( GetVFS().AddStorage( VFS::VirtualFileStorageFactory::CreateStaticFolder( path, prefix )));
		}

		for (auto& [host, service, in_prefix] : re_cfg.netVFS)
		{
			Networking::IpAddress	addr = Networking::IpAddress::FromServiceTCP( host, service );

			ASSERT( not in_prefix.empty() );
			String	prefix = in_prefix;

			if ( prefix.empty() )			continue;
			if ( prefix.back() != '/' )		prefix << '/';

			auto&	client = _Core()._vfsClients.emplace_back( MakeRC<ResEditorCore::VFSClient>() );
			CHECK_ERR( client->Init( addr, prefix ));
			CHECK_ERR( GetVFS().AddStorage( client->Storage() ));
		}

		if ( not re_cfg.exportFolder.empty() )
		{
			CHECK_ERR( GetVFS().AddStorage( VFS::StorageName{"export"},
										    VFS::VirtualFileStorageFactory::CreateDynamicFolder( re_cfg.exportFolder, "export/" )));
		}

		GetVFS().MakeImmutable();

		// for imgui
		{
			const auto&		ui_path = re_cfg.uiDataFolder;

			CHECK_ERR( FileSystem::IsDirectory( ui_path ));
			CHECK_ERR( FileSystem::SetCurrentPath( ui_path ));	// TODO: use VFS ?
		}

		CHECK_ERR( FileSystem::IsDirectory( re_cfg.scriptFolder ));
		return true;
	}

/*
=================================================
	OnStart
=================================================
*/
	void  ResEditorApplication::OnStart (IApplication &app) __NE___
	{
		_app = &app;
		CHECK_FATAL( _InitVFS() );

		const auto	cfg = GetAppConfig();

		CHECK_FATAL( _InitGraphics( app, cfg.graphics ));

		CHECK_FATAL( Cast<ResEditorCore>(&GetBaseApp())->OnStart() );

		CHECK_FATAL( _CreateWindow( app, cfg.window, cfg.vr ));
	}

/*
=================================================
	OnStop
=================================================
*/
	void  ResEditorApplication::OnStop (IApplication &app) __NE___
	{
		AppCoreV1::OnStop( app );
		_app = null;
	}

/*
=================================================
	_Core
=================================================
*/
	ResEditorCore&  ResEditorApplication::_Core () __NE___
	{
		return RefCast<ResEditorCore>( GetBaseApp() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	VFSClient::Init
=================================================
*/
	bool  ResEditorCore::VFSClient::Init (const Networking::IpAddress &addr, StringView prefix)
	{
		using namespace AE::Networking;

		auto	mf = MakeRC<Networking::MessageFactory>();
		CHECK_ERR( Networking::Register_NetVFS( *mf ));

		CHECK_ERR( _Initialize( RVRef(mf), MakeRC<DefaultServerProviderV1>( addr ), null, _frameId ));
		CHECK_ERR( _AddChannelReliableTCP( "VFS "s << prefix ));

		_storage = VFS::VirtualFileStorageFactory::CreateNetworkStorage( *this, prefix );
		CHECK_ERR( _storage );

		return true;
	}

/*
=================================================
	VFSClient::Tick
=================================================
*/
	AsyncTask  ResEditorCore::VFSClient::Tick ()
	{
		return	Scheduler().Run(
					ETaskQueue::Background,
					[] (RC<VFSClient> client) -> AsyncCoro
					{
						auto	stat = client->Update( client->_frameId );
						if ( stat )
							client->_frameId.Inc();

						co_return;
					}( GetRC() ),
					Tuple{},
					"VFS client tick" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	RemoteInputServer::Init
=================================================
*/
	bool  ResEditorCore::RemoteInputServer::Init (ushort port, IInputActions &ia) __NE___
	{
		using namespace AE::Networking;

		_remoteIA.Init( ia );

		auto	mf = MakeRC<MessageFactory>();

		CHECK_ERR( Register_RemoteControl( *mf ));
		CHECK_ERR( _Initialize( RVRef(mf), MakeRC<DefaultClientListener>(), null, _frameId ));
		CHECK_ERR( _AddChannelReliableTCP( port, "RemoteInput" ));

		CHECK_ERR( Add( _msgProducer.GetRC() ));
		CHECK_ERR( Add( _remoteIA.GetMsgConsumer() ));

		return true;
	}

/*
=================================================
	RemoteInputServer::Tick
=================================================
*/
	AsyncTask  ResEditorCore::RemoteInputServer::Tick () __NE___
	{
		// in main thread
		{
			_remoteIA.EnableSensors( *_msgProducer );
		}

		return	Scheduler().Run(
					ETaskQueue::Background,
					[] (RC<RemoteInputServer> server) -> AsyncCoro
					{
						auto	stat = server->Update( server->_frameId );
						if ( stat )
							server->_frameId.Inc();

						co_return;
					}( GetRC() ),
					Tuple{},
					"IA Server tick" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ResEditorCore::ResEditorCore () :
		_ui{ *this }
	{}

/*
=================================================
	destructor
=================================================
*/
	ResEditorCore::~ResEditorCore ()
	{
	  #if ENABLE_RDC
		GraphicsScheduler().GetDevice().GetRenderDocApi().PrintCaptures();
	  #endif

		_mainLoop.Write( Default );
	}

/*
=================================================
	OnStart
=================================================
*/
	bool  ResEditorCore::OnStart ()
	{
		NOTHROW_ERR(
			_rg.reset( new RenderGraphImpl{} );

			auto&				re_cfg = ResEditorAppConfig::Get();
			ScriptExe::Config	cfg;
			cfg.cppTypesFolder			= re_cfg.cppTypesFolder;
			cfg.scriptHeaderOutFolder	= re_cfg.scriptHeaderOutFolder;
			cfg.vfsPaths				= re_cfg.vfsPaths;
			cfg.scriptIncludeDirs		= re_cfg.scriptIncludeDirs;

			_script.reset( new ScriptExe{ RVRef(cfg) });	// throw

			_test.scripts.clear();
			for (auto& folder : re_cfg.testFolders)
			{
				for (auto& entry : FileSystem::EnumRecursive( folder ))
				{
					if ( entry.IsFile()										and
						 entry.Get().extension() == ".as"					and
						 not StartsWith( entry.Get().stem().string(), "wip-" ))
					{
						_test.scripts.push_back( entry.Get() );
					}
				}
			}

			if ( not _test.scripts.empty() )
			{
				CHECK_ERR( not re_cfg.testOutput.empty() );

				Path&	test_output =ResEditorAppConfig::Edit().testOutput;

				test_output /= GraphicsScheduler().GetDevice().GetDeviceName();
				FileSystem::CreateDirectories( test_output );

				_test.isActive.store( true );
			}
		)
		return	_LoadInputActions();
	}

/*
=================================================
	_LoadInputActions
=================================================
*/
	bool  ResEditorCore::_LoadInputActions ()
	{
		// load input actions
		_inputActionsData = MakeRC<ArrayRStream>();

		FileRStream		file {Path{"controls.bin"}};		// TODO: use VFS
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( _inputActionsData->LoadRemainingFrom( file ));

		return true;
	}

/*
=================================================
	OnSurfaceCreated
----
	Thread-safe: only main thread
=================================================
*/
	bool  ResEditorCore::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		auto&	re_cfg = ResEditorAppConfig::Get();

	  #if ENABLE_RDC
		// initialize render doc
		{
			if ( re_cfg.enableRenderDoc and not re_cfg.renderDocFolder.empty() )
			{
				GraphicsScheduler().GetDevice().GetRenderDocApi().CaptureFolder( ToString( re_cfg.renderDocFolder ) << '/' );

				EFileSystemWatchBits filter;
				filter.insert( EFileSystemWatch::FileCreated );
				CHECK( _rdCaptureWatch.Start( re_cfg.renderDocFolder, filter ));
			}
		}
	  #endif

		_window = &wnd;
		return _ui.Init( wnd.GetSurface(), re_cfg.windowMode, wnd.GetMonitor().uiScale, re_cfg.scriptFolder );
	}

/*
=================================================
	_InitInputActions
=================================================
*/
	void  ResEditorCore::_InitInputActions (IInputActions &ia) __NE___
	{
		MemRefRStream	stream{ _inputActionsData->GetData() };

		CHECK( ia.LoadSerialized( stream ));

		CHECK( ia.SetMode( InputModeName{"Main.UI"} ));

		if ( auto port = ResEditorAppConfig::Get().remoteIAPort;  port != 0 )
		{
			_remoteIAServer = MakeRC<RemoteInputServer>();
			CHECK( _remoteIAServer->Init( port, ia ));
		}
	}

/*
=================================================
	StartRendering
----
	Thread-safe: only main thread
=================================================
*/
	void  ResEditorCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output, EWndState state) __NE___
	{
		ASSERT( bool{input} == bool{output} );

		const bool	focused	= (state == EWndState::Focused);
		bool		ia_changed;

		{
			auto	main_loop = _mainLoop.WriteLock();

			if ( not focused and main_loop->output != null )
				return;

			ia_changed = (main_loop->input != input) and (input != null);

			main_loop->input  = input;
			main_loop->output = output;
		}

		if ( ia_changed )
			_InitInputActions( *input );
	}

/*
=================================================
	StopRendering
----
	Thread-safe: only main thread
=================================================
*/
	void  ResEditorCore::StopRendering (Ptr<IOutputSurface> output) __NE___
	{
		auto	main_loop	= _mainLoop.WriteLock();

		if ( output == null or main_loop->output == output )
			main_loop->output = null;
	}

/*
=================================================
	RunRenderScriptAsync
----
	should be used in background thread
=================================================
*/
	bool  ResEditorCore::RunRenderScriptAsync (const Path &scriptPath)
	{
		auto&	re_cfg = ResEditorAppConfig::Get();

		ScriptExe::ScriptConfig	cfg;
		cfg.dynSize			= UIInteraction::Instance().GetDynamicSize();
		cfg.shaderDirs		= re_cfg.shaderSearchDirs;
		cfg.includeDirs		= re_cfg.shaderIncludeDirs;
		cfg.pipelineDirs	= re_cfg.pipelineSearchDirs;
		cfg.scriptDir		= re_cfg.scriptCallableFolder;
		cfg.monitor			= _monitor.Read();
		cfg.enableRandomizer= not _test.isActive.load();

		auto	output	= _mainLoop.ConstPtr()->output;
		if ( output )
		{
			auto	infos = output->GetTargetInfo();
			CHECK_ERR( infos.size() == 1 );
			cfg.dynSize->Resize( infos[0].Dimension() );
		}

		auto	renderer = _script->Run( scriptPath, cfg );
		if ( not renderer )
			return false;

		if ( _test.isActive.load() )
			renderer->SetFreezeTime( true );

		Unused( CreateAsyncRev(
			GetRC<ResEditorCore>(), renderer,
			[] (RC<ResEditorCore> self, RC<Renderer> renderer) -> ScheduledCoro<ETaskQueue::Main>
			{
				self->_ui.SetHelpText( renderer->GetHelpText() );
				self->_ui.SetSurfaceFormat( renderer->GetSurfaceFormat() );
				self->_mainLoop->renderer = RVRef(renderer);
				UIInteraction::Instance().NewScriptLoaded();
				co_return;
			}));

		return true;
	}

/*
=================================================
	_ProcessInput
=================================================
*/
	AsyncCoro  ResEditorCore::_ProcessInput (TsInputActions input, RC<Renderer> renderer, Ptr<EditorUI> ui, ActionQueueReader reader)
	{
		if ( ui )
		{
			bool	switch_mode = false;
			ui->ProcessInput( reader, OUT switch_mode );

			if_unlikely( switch_mode and renderer and renderer->GetInputMode().IsDefined() )
			{
				Scheduler().Run( ETaskQueue::Main,
								 _SetInputMode( input.Unsafe(), renderer->GetInputMode() ),
								 Tuple{}, "Core::SetInputMode" );
			}
		}

		if ( renderer )
		{
			bool	switch_mode = false;
			renderer->ProcessInput( reader, OUT switch_mode );

			if_unlikely( switch_mode )
			{
				Scheduler().Run( ETaskQueue::Main,
								 _SetInputMode( input.Unsafe(), InputModeName{"Main.UI"} ),
								 Tuple{}, "Core::SetInputMode" );
			}
		}

		co_return;
	}

/*
=================================================
	_SetInputMode
=================================================
*/
	AsyncCoro  ResEditorCore::_SetInputMode (Ptr<IInputActions> input, InputModeName mode)
	{
		CHECK( input->SetMode( mode ));
		co_return;
	}

/*
=================================================
	RenderFrame
----
	Thread-safe: only main thread
=================================================
*/
	void  ResEditorCore::RenderFrame () __NE___
	{
		#if ENABLE_RDC
		if ( _ui.IsCaptureRequested() )
		{
			auto&	dev = GraphicsScheduler().GetDevice();
			if ( dev.HasRenderDocApi() )
			{
				CHECK( dev.GetRenderDocApi().TriggerFrameCapture() );
				AE_LOGI( "Trigger frame capture..." );
			}
			else
				AE_LOGI( "RenderDoc is not attached, can't capture frame" );
		}
		_CheckRdEvents();
		#endif

		Ptr<IInputActions>		input;
		Ptr<IOutputSurface>		output;
		RC<Renderer>			renderer;
		{
			auto	main_loop = _mainLoop.ReadLock();

			input		= main_loop->input;
			output		= main_loop->output;
			renderer	= main_loop->renderer;

			if ( output == null or not output->IsInitialized() )
				renderer = null;
		}

		auto&			rg			= RenderGraph();
		Ptr<EditorUI>	ui			= output and _ui.IsInitialized() ? &_ui : null;
		AsyncTask		proc_input;

		if ( input )
		{
			ActionQueueReader reader = input->ReadInput( rg.GetPrevFrameId() );

			proc_input = Scheduler().Run( ETaskQueue::PerFrame,
										  _ProcessInput( input, renderer, ui, reader ),
										  Tuple{}, "Core::ProcessInput" );
		}

		Array<AsyncTask>	client_tasks;
		for (auto& client : _vfsClients) {
			client_tasks.push_back( client->Tick() );
		}

		// rendering depends on input processing
		if ( (renderer or ui) and rg.BeginFrame( output ))
		{
			if ( _remoteIAServer )
				GraphicsScheduler().AddNextFrameDeps( _remoteIAServer->Tick() );

			AsyncTask	draw_task	= renderer	? renderer->Execute({ proc_input })	: null;
			AsyncTask	ui_task		= ui		? ui->Draw({ proc_input })			: null;

			AsyncTask	end_frame	= rg.EndFrame( Tuple{ draw_task, ui_task, WeakDepArray{client_tasks} });

			if ( input )
				input->NextFrame( rg.GetNextFrameId() );
		}
		else
		{
			ThreadUtils::Sleep_15ms();
		}

		_UpdateTests( renderer );
	}

/*
=================================================
	_UpdateTests
=================================================
*/
	void  ResEditorCore::_UpdateTests (RC<Renderer> renderer)
	{
		const auto	LoadNextScript = [this] ()
		{{
			_mainLoop->renderer = null;

			if_unlikely( _test.scripts.empty() )
			{
				_test.isActive.store( false );
				ResEditorAppConfig::Get().screenshotPrefix.Write( Default );
				AE_LOGW( "---- All tests are completed ----" );
			}
			else
			{
				auto	path = _test.scripts.ExtractFront();
				ResEditorAppConfig::Get().screenshotPrefix.Write( ToString(path.parent_path().filename()) << '-' << ToString(path.filename()) );
				_test.status.store( ETestStatus::Load );

				Scheduler().Run(
					ETaskQueue::Background,
					[] (RC<ResEditorCore> core, Path inPath) -> AsyncCoro
					{
						if ( not core->RunRenderScriptAsync( inPath ))
							core->_test.status.store( ETestStatus::Complete );
						co_return;
					}
					( GetRC<ResEditorCore>(), RVRef(path) ),
					Tuple{},
					"Tests::LoadScript"
				);
			}
		}};

		if_likely( not _test.isActive.load() )
			return;

		switch_enum( _test.status.load() )
		{
			case ETestStatus::Load :
				if ( renderer )
				{
					_test.status.store( ETestStatus::Upload );
					_test.framesToCapture.store( 1000 );
				}
				return;

			case ETestStatus::Upload :
			{
				if ( not renderer )
				{
					_test.status.store( ETestStatus::Complete );
					return;
				}

				if ( renderer->IsUploadComplete() or _test.framesToCapture.Dec() < 0 )
				{
					// make screenshot
					auto	capture = UIInteraction::Instance().capture.WriteLock();

					capture->testScreenshot = true;
					capture->imageFormat	= EImageFormat::PNG;

					_test.framesToSwitch.store( GraphicsConfig::MaxFrames*2 );
					_test.status.store( ETestStatus::Screenshot );
				}
				return;
			}

			case ETestStatus::Screenshot :
				if ( _test.framesToSwitch.Dec() < 0 )
					_test.status.store( ETestStatus::Complete );
				return;

			case ETestStatus::Complete :
				return LoadNextScript();
		}
		switch_end
	}

/*
=================================================
	WaitFrame
----
	Thread-safe: only main thread
=================================================
*/
	void  ResEditorCore::WaitFrame (const Threading::EThreadArray	&threadMask,
									Ptr<IWindow>					window) __NE___
	{
		milliseconds	timeout = AE::DefaultTimeout;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		timeout = seconds{2};
	  #endif
	  #ifndef AE_CFG_RELEASE
		if ( PlatformUtils::IsUnderDebugger() )
			timeout = minutes{60};
	  #endif

		CHECK( GraphicsScheduler().WaitNextFrame( threadMask, timeout ));

		if ( window )
		{
			if ( _window != window )
			{
				_window = window;
				_monitor.Write( window->GetMonitor() );
			}

			if ( auto new_mode = _ui.GetNewWindowMode();  new_mode.has_value() )
			{
				if ( auto* desk_wnd = window->AsDesktopWindow() )
					Unused( desk_wnd->SetMode( *new_mode ));
			}
		}
	}

/*
=================================================
	_CheckRdEvents
----
	Thread-safe: only main thread
=================================================
*/
	void  ResEditorCore::_CheckRdEvents ()
	{
		FileWatch::EventArray_t	events;
		Unused( _rdCaptureWatch.GetEvents( OUT events ));

		for (auto& ev : events)
		{
			if ( ev.action == EFileSystemAction::Added )
				AE_LOGI( "Added RenderDoc capture '"s << ToString(ev.path) << "'" );
		}
	}

} // AE::ResEditor
//-----------------------------------------------------------------------------


using namespace AE;
using namespace AE::Base;
using namespace AE::App;
using namespace AE::ResEditor;

#ifdef AE_ENABLE_CDT
	#define REQUIRE_LGPLv3
#else
	#define REQUIRE_APACHE_2
#endif
#include "base/Defines/DetectLicense.inl.h"


/*
=================================================
	AE_OnAppCreated
=================================================
*/
Unique<IApplication::IAppListener>  AE_OnAppCreated (const int argc, char const* argv[])
{
	Unused( argc, argv );

#if AE_PORTABLE_APP
	StaticLogger::Initialize();
	StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );

#  ifdef AE_DEBUG
	StaticLogger::AddLogger( ILogger::CreateIDEOutput() );
	StaticLogger::AddLogger( ILogger::CreateHtmlOutput( "log" ));
#  endif
#else
	StaticLogger::InitDefault();
#endif

	AE_LOGI( String{AE_ENGINE_NAME} << ' ' << ToString(AE_VERSION.Get<0>()) << '.' << ToString(AE_VERSION.Get<1>()) << '.' << ToString(AE_VERSION.Get<2>()) );
	AE_LOG_DBG( "License: "s << AE_LICENSE );

	return MakeUnique<ResEditorApplication>();
}

/*
=================================================
	AE_OnAppDestroyed
=================================================
*/
void  AE_OnAppDestroyed ()
{
	// Don't check for memleaks because of
	// false positive in glslang when used dynamic allocation in static variable.
	StaticLogger::Deinitialize( false );
}

/*
=================================================
	JNI_OnLoad / JNI_OnUnload
=================================================
*/
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
