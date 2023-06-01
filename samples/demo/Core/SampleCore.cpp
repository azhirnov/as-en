// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/SampleCore.h"
#include "base/DataSource/MemStream.h"
#include "base/Utils/Helpers.h"

#include "graphics/Private/EnumToString.h"
#include "vfs/Archive/ArchiveStaticStorage.h"

// samples
#include "demo/Examples/Canvas2D.h"
#include "demo/Examples/Simple3D.h"
#include "demo/Examples/ImGuiSample.h"
#include "demo/Examples/RayQuery.h"
#include "demo/Examples/RayTracing.h"
#include "demo/Examples/YcbcrImage.h"
#include "demo/Examples/HdrTest.h"

namespace AE::Samples::Demo
{
	using namespace AE::Threading;
	using namespace AE::Graphics;
	
	
/*
=================================================
	SampleCore ctor
=================================================
*/
	SampleCore::SampleCore ()
	{
		//_sample.reset( new RayQuerySample{} );
		//_sample.reset( new RayTracingSample{} );
		_sample.reset( new ImGuiSample{} );
		//_sample.reset( new Canvas2DSample{} );
		//_sample.reset( new Simple3DSample{} );
		//_sample.reset( new YcbcrImageSample{} );
		//_sample.reset( new HdrTestSample{} );
	}
//-----------------------------------------------------------------------------



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
			cfg.threading.maxThreads	= 0;
			cfg.threading.mask			= {EThread::PerFrame, EThread::Renderer, EThread::Background};
		}
		
		// graphics
		{
			cfg.graphics.maxFrames = 2;

			cfg.graphics.staging.readStaticSize .fill( 2_Mb );
			cfg.graphics.staging.writeStaticSize.fill( 2_Mb );
			cfg.graphics.staging.maxReadDynamicSize		= 16_Mb;
			cfg.graphics.staging.maxWriteDynamicSize	= 256_Mb;
			cfg.graphics.staging.dynamicBlockSize		= 16_Mb;
			cfg.graphics.staging.vstreamSize			= 4_Mb;

			cfg.graphics.device.appName			= "Demo";
			cfg.graphics.device.requiredQueues	= EQueueMask::Graphics;
			cfg.graphics.device.optionalQueues	= Default; //EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer;
			cfg.graphics.device.validation		= EDeviceValidation::Enabled;
		//	cfg.graphics.device.devFlags		= EDeviceFlags::SetStableClock;
			
		  #if 0
			cfg.graphics.swapchain.format		= EPixelFormat::RGBA16F;
			cfg.graphics.swapchain.colorSpace	= EColorSpace::Extended_sRGB_linear;
		  #else
			cfg.graphics.swapchain.format		= EPixelFormat::RGBA8_UNorm;
		  #endif
			cfg.graphics.swapchain.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
			cfg.graphics.swapchain.options		= EImageOpt::BlitDst;
		//	cfg.graphics.swapchain.presentMode	= EPresentMode::Mailbox;	// faster vsync
			cfg.graphics.swapchain.presentMode	= EPresentMode::FIFO;		// vsync
			cfg.graphics.swapchain.minImageCount= 3;
		}

		// window
		{
			cfg.window.title		= "Demo";
			cfg.window.size			= {1024, 768};
			cfg.window.resizable	= true;
			cfg.window.fullscreen	= false;
		}

		// VR
		{
			cfg.enableVR		= false;
			cfg.vr.dimension	= {1024, 1024};
			cfg.vr.format		= EPixelFormat::BGRA8_UNorm;
			cfg.vr.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer;	// default
		}

		return cfg;
	}
//-----------------------------------------------------------------------------

	
	
/*
=================================================
	constructor
=================================================
*/
	SampleApplication::SampleApplication () :
		DefaultAppListener{ GetAppConfig(), MakeRC<SampleCore>() }
	{
		if ( not FileSystem::SetCurrentPath( AE_RES_FOLDER ))
			FileSystem::FindAndSetCurrent( "samples/demo", 5 );
	}
	
/*
=================================================
	_InitVFS
=================================================
*/
	bool  SampleApplication::_InitVFS (IApplication &app)
	{
	#ifdef AE_PLATFORM_ANDROID
		auto	assets = app.OpenBuiltinStorage();
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
		CHECK_FATAL( Cast<SampleCore>(&GetBaseApp())->LoadInputActions() );

		DefaultAppListener::OnStart( app );

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

		RenderTaskScheduler().GetResourceManager().ReleaseResource( _pplnPack );
	}

/*
=================================================
	LoadInputActions
=================================================
*/
	bool  SampleCore::LoadInputActions ()
	{
		// load input actions
		_inputActionsData = MakeRC<MemRStream>();
		
		auto	file = GetVFS().OpenAsStream( VFS::FileName{"controls"} );
		CHECK_ERR( file );
		CHECK_ERR( _inputActionsData->LoadRemaining( *file ));
		
		return true;
	}
	
/*
=================================================
	InitInputActions
=================================================
*/
	void  SampleCore::InitInputActions (IInputActions &ia) __NE___
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
=================================================
*/
	bool  SampleCore::OnSurfaceCreated (IOutputSurface &output) __NE___
	{
		if ( _initialized.load() )
			return true;

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

		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();
		
		auto	rp_info = output.GetRenderPassInfo();
		CHECK_ERR( rp_info.attachments.size() == 1 );
		CHECK_ERR( rp_info.attachments[0].samples == 1_samples );
		
		#ifdef AE_PLATFORM_APPLE
			constexpr auto	fname = VFS::FileName{"mac/render_passes"};
		#else
			constexpr auto	fname = VFS::FileName{"vk/render_passes"};
		#endif

		auto	file = GetVFS().OpenAsStream( fname );
		CHECK_ERR( file );

		PipelinePackDesc	desc;
		desc.stream			= file;
		desc.options		= EPipelinePackOpt::FeatureSets | EPipelinePackOpt::Samplers | EPipelinePackOpt::RenderPasses;
		desc.surfaceFormat	= rp_info.attachments[0].format;
		desc.dbgName		= "base pack";

		CHECK_ERR( desc.surfaceFormat != Default );
		CHECK_ERR( res_mngr.InitializeResources( desc ));

		return true;
	}
	
/*
=================================================
	_CompileResources
=================================================
*/
	bool  SampleCore::_CompileResources (IOutputSurface &output)
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();
		res_mngr.ReleaseResource( _pplnPack );
		
		auto	rp_info = output.GetRenderPassInfo();
		CHECK_ERR( rp_info.attachments.size() == 1 );
		CHECK_ERR( rp_info.attachments[0].samples == 1_samples );

		#ifdef AE_PLATFORM_APPLE
			constexpr auto	fname = VFS::FileName{"mac/pipelines"};
		#else
			constexpr auto	fname = VFS::FileName{"vk/pipelines"};
		#endif
			
		auto	file = GetVFS().OpenAsStream( fname );
		CHECK_ERR( file );

		PipelinePackDesc	desc;
		desc.stream			= file;
		desc.options		= EPipelinePackOpt::Samplers | EPipelinePackOpt::Pipelines;
		desc.surfaceFormat	= rp_info.attachments[0].format;
		desc.dbgName		= "pipelines";

		_pplnPack = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( _pplnPack );

		CHECK_ERR( _sample->Init( _pplnPack ));

		return true;
	}

/*
=================================================
	StartRendering
=================================================
*/
	void  SampleCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output) __NE___
	{
		ASSERT( bool{input} == bool{output} );
		
		if ( output != null and not output->IsInitialized() )
			return;

		auto	main_loop = _mainLoop.WriteNoLock();
		EXLOCK( main_loop );

		main_loop->input  = input;
		main_loop->output = output;
	}

/*
=================================================
	StopRendering
=================================================
*/
	void  SampleCore::StopRendering () __NE___
	{
		auto	main_loop = _mainLoop.WriteNoLock();
		EXLOCK( main_loop );
		
		main_loop->input  = null;
		main_loop->output = null;
	}
	
/*
=================================================
	SurfaceDestroyed
=================================================
*/
	void  SampleCore::SurfaceDestroyed () __NE___
	{
		_mainLoop.Write( Default );
	}

/*
=================================================
	WaitFrame
=================================================
*/
	void  SampleCore::WaitFrame (const Threading::EThreadArray &threads) __NE___
	{
		AsyncTask	task;
		std::swap( task, _mainLoop->endFrame );

		for (;;)
		{
			if ( task == null or task->IsFinished() )
				break;

			Scheduler().ProcessTasks( threads, 0 );

			Scheduler().DbgDetectDeadlock();
		}
	}
	
/*
=================================================
	RenderFrame
=================================================
*/
	void  SampleCore::RenderFrame () __NE___
	{
		_mainLoop->endFrame = _DrawFrame();
	}
		
/*
=================================================
	_DrawFrame
=================================================
*/
	AsyncTask  SampleCore::_DrawFrame ()
	{
		Ptr<IInputActions>		input;
		Ptr<IOutputSurface>		output;
		RenderGraph				rg;

		{
			auto	main_loop = _mainLoop.ReadNoLock();
			SHAREDLOCK( main_loop );

			if ( main_loop->input == null or main_loop->output == null )
				return null;

			input	= main_loop->input;
			output	= main_loop->output;
		}


		AsyncTask	proc_input	= _sample->Update( input->ReadInput( rg.GetPrevFrameId() ), Default );
		// 'proc_input' can be null

		AsyncTask	begin_frame	= rg.BeginFrame( output );
		if ( begin_frame->IsInterropted() )
			return null;

		AsyncTask	draw_task	= _sample->Draw( rg, { begin_frame, proc_input });
		CHECK_ERR( draw_task );

		AsyncTask	end_frame	= rg.EndFrame( Default, Tuple{ draw_task });
		
		input->NextFrame( rg.GetNextFrameId() );

		return end_frame;
	}

} // AE::Samples::Demo
//-----------------------------------------------------------------------------


using namespace AE::Base;
using namespace AE::App;
using namespace AE::Samples::Demo;

#include "base/Algorithms/StringUtils.h"


Unique<IApplication::IAppListener>  AE_OnAppCreated ()
{
	StaticLogger::InitDefault();
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

	extern "C" void JNI_OnUnload (JavaVM *vm, void *)
	{
		return ApplicationAndroid::OnJniUnload( vm );
	}

#endif // AE_PLATFORM_ANDROID
