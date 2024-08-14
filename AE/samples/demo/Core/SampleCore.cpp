// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/SampleCore.h"

// samples
#include "demo/Examples/Canvas2D.h"
#include "demo/Examples/Camera3D.h"
#include "demo/Examples/ImGuiSample.h"
#include "demo/Examples/Audio.h"

namespace AE::Samples::Demo
{
	using namespace AE::Threading;
	using namespace AE::Graphics;

/*
=================================================
	SampleCore ctor
=================================================
*/
	SampleCore::SampleCore () __NE___
	{
        _sample = MakeRC< ImGuiSample >();
        //_sample = MakeRC< Canvas2DSample >();
		//_sample = MakeRC< Camera3DSample >();
		//_sample = MakeRC< AudioSample >();
	}
//-----------------------------------------------------------------------------



namespace
{
/*
=================================================
	GetAppConfig
=================================================
*/
	static AppV1::AppConfig  GetAppConfig ()
	{
		AppV1::AppConfig	cfg;

		// threading
		{
			cfg.threading.maxThreads	= 1;
			cfg.threading.mask			= { EThread::PerFrame, EThread::Renderer, EThread::Background };
		}

		// graphics
		{
			cfg.graphics.maxFrames = 2;

			cfg.graphics.device.appName			= "Demo";
			cfg.graphics.device.requiredQueues	= EQueueMask::Graphics;
			cfg.graphics.device.optionalQueues	= Default; //EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer;
			cfg.graphics.device.validation		= EDeviceValidation::Enabled;
		//	cfg.graphics.device.devFlags		= EDeviceFlags::SetStableClock;

		  #if 0
			cfg.graphics.swapchain.colorFormat	= EPixelFormat::RGBA16F;
			cfg.graphics.swapchain.colorSpace	= EColorSpace::Extended_sRGB_linear;
		  #else
			cfg.graphics.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;
		  #endif
			cfg.graphics.swapchain.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
			cfg.graphics.swapchain.options		= EImageOpt::BlitDst;
			cfg.graphics.swapchain.presentMode	= EPresentMode::FIFO;
			cfg.graphics.swapchain.minImageCount= ubyte(cfg.graphics.maxFrames);
		}

		// window
		{
			cfg.window.title	= "Demo";
			cfg.window.size		= {1600, 896};
			cfg.window.mode		= EWindowMode::Resizable;
		}

		// VR
		{
			cfg.enableVR		= false;
			cfg.vr.dimension	= {1024, 1024};
			cfg.vr.format		= EPixelFormat::BGRA8_UNorm;
			cfg.vr.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer;	// default

		//	cfg.vrDevices.push_back( IVRDevice::EDeviceType::OpenXR );
		//	cfg.vrDevices.push_back( IVRDevice::EDeviceType::OpenVR );
			cfg.vrDevices.push_back( IVRDevice::EDeviceType::Emulator );
		}

		cfg.enableAudio = true;

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		cfg.enableNetwork			= true;
		cfg.window.mode				= EWindowMode::NonResizable;
		cfg.graphics.enableSyncLog	= false;
		//cfg.graphics.deviceAddr 	= // set ip address
	  #endif

		return cfg;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	SampleApplication::SampleApplication () __NE___ :
		AppCoreV1{ GetAppConfig(), MakeRC<SampleCore>() }
	{
	  #ifndef AE_PLATFORM_ANDROID
		if ( not FileSystem::SetCurrentPath( AE_RES_FOLDER ))
			FileSystem::FindAndSetCurrent( "samples/demo", 5 );
	  #endif
	}

/*
=================================================
	_InitVFS
=================================================
*/
	bool  SampleApplication::_InitVFS (IApplication &app)
	{
	#ifdef AE_PLATFORM_ANDROID
		auto	assets = app.OpenStorage( EAppStorage::Builtin );
		CHECK_ERR( assets );

		RC<RDataSource>	ds;
		CHECK_ERR( assets->Open( OUT ds, VFS::FileName{"resources.bin"} ));

		auto	storage = VFS::VirtualFileStorageFactory::CreateStaticArchive( RVRef(ds) );
	#else
		Unused( app );
		auto	storage = VFS::VirtualFileStorageFactory::CreateStaticArchive( "resources.bin" );
	#endif

		CHECK_ERR( storage );
		CHECK_ERR( GetVFS().AddStorage( storage ));

		GetVFS().MakeImmutable();
		return true;
	}

/*
=================================================
	OnStart
=================================================
*/
	void  SampleApplication::OnStart (IApplication &app) __NE___
	{
		CHECK_FATAL( _InitVFS( app ));

		auto&	core = *Cast<SampleCore>(&GetBaseApp());
		core.SetApplication( app );
		CHECK_FATAL( core.LoadInputActions() );

		AppCoreV1::OnStart( app );

		CHECK_FATAL( _OnStartImpl( app ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	destructor
=================================================
*/
	SampleCore::~SampleCore ()
	{
		_mainLoop.Write( Default );
		_sample = null;

		GraphicsScheduler().GetResourceManager().ReleaseResource( _pplnPack );
	}

/*
=================================================
	LoadInputActions
=================================================
*/
	bool  SampleCore::LoadInputActions ()
	{
		// load input actions
		_inputActionsData = MakeRC<ArrayRStream>();

		auto	file = GetVFS().Open<RStream>( VFS::FileName{"controls"} );
		CHECK_ERR( file );
		CHECK_ERR( _inputActionsData->LoadRemainingFrom( *file ));

		return true;
	}

/*
=================================================
	_InitInputActions
=================================================
*/
	void  SampleCore::_InitInputActions (IInputActions &ia)
	{
		MemRefRStream	stream{ _inputActionsData->GetData() };

		CHECK( ia.LoadSerialized( stream ));

		auto	mode = _sample->GetInputMode();
		if ( mode != Default )
			CHECK( ia.SetMode( mode ));
	}

/*
=================================================
	OnSurfaceCreated
----
	in main thread
=================================================
*/
	bool  SampleCore::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		if ( _initialized.load() )
			return true;

		auto&	output = wnd.GetSurface();

		if ( _CompileBaseResources( output ) and _CompileResources( output ))
		{
			_initialized.store( true );
			return true;
		}
		return false;
	}

/*
=================================================
	_CompileBaseResources
=================================================
*/
	bool  SampleCore::_CompileBaseResources (IOutputSurface &output)
	{
		CHECK_ERR( output.IsInitialized() );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto	rp_info = output.GetRenderPassInfo();
		CHECK_ERR( rp_info.attachments.size() == 1 );
		CHECK_ERR( rp_info.attachments[0].samples == 1_samples );

		VFS::FileName	fname;
		switch_enum( res_mngr.GetDevice().GetGraphicsAPI() )
		{
			case EGraphicsAPI::Vulkan :		fname = VFS::FileName{"vk/render_passes"};	break;
			case EGraphicsAPI::Metal :		fname = VFS::FileName{"mac/render_passes"};	break;
		}
		switch_end

		auto	file = GetVFS().Open<RStream>( fname );
		CHECK_ERR( file );

		PipelinePackDesc	desc;
		desc.stream			= file;
		desc.options		= EPipelinePackOpt::FeatureSets | EPipelinePackOpt::Samplers | EPipelinePackOpt::RenderPasses;
		desc.surfaceFormat	= rp_info.attachments[0].format;
		desc.dbgName		= "base pack";

		CHECK_ERR( desc.surfaceFormat != Default );

		auto	pack_id = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( res_mngr.InitializeResources( RVRef(pack_id) ));

		return true;
	}

/*
=================================================
	_CompileResources
=================================================
*/
	bool  SampleCore::_CompileResources (IOutputSurface &output)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		res_mngr.ReleaseResource( _pplnPack );

		auto	rp_info = output.GetRenderPassInfo();
		CHECK_ERR( rp_info.attachments.size() == 1 );
		CHECK_ERR( rp_info.attachments[0].samples == 1_samples );

		VFS::FileName	fname;
		switch_enum( res_mngr.GetDevice().GetGraphicsAPI() )
		{
			case EGraphicsAPI::Vulkan :		fname = VFS::FileName{"vk/pipelines"};	break;
			case EGraphicsAPI::Metal :		fname = VFS::FileName{"mac/pipelines"};	break;
		}
		switch_end

		auto	file = GetVFS().Open<RStream>( fname );
		CHECK_ERR( file );

		PipelinePackDesc	desc;
		desc.stream			= file;
		desc.options		= EPipelinePackOpt::Pipelines;
		desc.surfaceFormat	= rp_info.attachments[0].format;
		desc.dbgName		= "pipelines";

		_pplnPack = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( _pplnPack );

		CHECK_ERR( _sample->Init( _pplnPack, _app ));

		return true;
	}

/*
=================================================
	StartRendering
=================================================
*/
	void  SampleCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output, EWndState state) __NE___
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
=================================================
*/
	void  SampleCore::StopRendering (Ptr<IOutputSurface> output) __NE___
	{
		auto	main_loop = _mainLoop.WriteLock();

		if ( output == null or main_loop->output == output )
			main_loop->output = null;
	}

/*
=================================================
	WaitFrame
=================================================
*/
	void  SampleCore::WaitFrame (const Threading::EThreadArray	&threadMask,
								 Ptr<IWindow>					,
								 Ptr<IVRDevice>					) __NE___
	{
		CHECK( GraphicsScheduler().WaitNextFrame( threadMask, AE::DefaultTimeout ));
	}

/*
=================================================
	RenderFrame
=================================================
*/
	void  SampleCore::RenderFrame () __NE___
	{
		Ptr<IInputActions>		input;
		Ptr<IOutputSurface>		output;
		RenderGraph				rg;

		{
			auto	main_loop = _mainLoop.ReadLock();

			if ( main_loop->input == null				or
				 main_loop->output == null				or
				 not main_loop->output->IsInitialized() )
				return;

			input	= main_loop->input;
			output	= main_loop->output;
		}

		if ( not rg.BeginFrame( output ))
			return;

		AsyncTask	proc_input	= _sample->Update( input->ReadInput( rg.GetPrevFrameId() ), Default );
		// 'proc_input' can be null

		AsyncTask	draw_task	= _sample->Draw( rg, {proc_input} );
		CHECK_ERRV( draw_task );

		Unused( rg.EndFrame( Default, Tuple{ draw_task }));

		input->NextFrame( rg.GetNextFrameId() );
	}

} // AE::Samples::Demo
//-----------------------------------------------------------------------------


using namespace AE::Base;
using namespace AE::App;
using namespace AE::Samples::Demo;

#define REQUIRE_APACHE_2
//#include "base/Defines/DetectLicense.inl.h"


Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
	StaticLogger::InitDefault();

	//AE_LOG_DBG( "License: "s << AE_LICENSE );

	return MakeUnique<SampleApplication>();
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
