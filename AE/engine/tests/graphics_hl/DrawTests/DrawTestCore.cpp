// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "DrawTestCore.h"

#include "res_loaders/DDS/DDSImageSaver.h"

static constexpr uint  c_MaxRenderThreads = 3;

/*
=================================================
	Test_DrawTests
=================================================
*/
extern void Test_DrawTests (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage)
{
	DrawTestCore	test;
	CHECK_FATAL( test.Run( assetStorage, refStorage ));

	TEST_PASSED();
}

/*
=================================================
	constructor
=================================================
*/
DrawTestCore::DrawTestCore () :
	_device{ True{"enable info log"} }
{
	_tests.emplace_back( &DrawTestCore::Test_Canvas_Rect );
}

/*
=================================================
	Run
=================================================
*/
bool  DrawTestCore::Run (FStorage_t assetStorage, FStorage_t refStorage)
{
	CHECK_ERR( _Create( refStorage ));

	for (uint i = 0; i < c_MaxRenderThreads; ++i) {
		Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
				EThreadArray{ EThread::PerFrame, EThread::Renderer },
				"render thread"s << ToString(i)
			}));
	}
	CHECK_ERR( _CompilePipelines( assetStorage ));

	_canvas.reset( new Canvas{} );

	bool	result = _RunTests();

	_Destroy();

	return result;
}

/*
=================================================
	_Destroy
=================================================
*/
void  DrawTestCore::_Destroy ()
{
  #ifdef AE_ENABLE_VULKAN
	_syncLog.Deinitialize( INOUT _device.EditDeviceFnTable() );
  #endif

	_canvasPpln		= null;
	_canvasPplnDesk	= null;
	_canvas.reset();

	RenderTaskScheduler::InstanceCtor::Destroy();

	CHECK( _device.DestroyLogicalDevice() );
	CHECK( _device.DestroyInstance() );
}

/*
=================================================
	_LoadReference
=================================================
*/
Unique<ImageComparator>  DrawTestCore::_LoadReference (StringView name) const
{
	Unique<ImageComparator>	img_cmp{ new ImageComparator{} };

	const Path	path	= (_refImagePath / name).replace_extension( "png" );
	bool		loaded	= false;

	if ( not UpdateAllReferences )
	{
		RC<RStream>	rfile;
		if ( _refImageStorage->Open( OUT rfile, VFS::FileName{ToString(path)} ))
		{
			loaded = img_cmp->LoadReference( RVRef(rfile), path );
		}
	}

	if ( not loaded )
	{
		VFS::FileName	fname;
		CHECK_ERR( _refImageStorage->CreateFile( fname, path ));

		RC<WStream>		wfile;
		CHECK_ERR( _refImageStorage->Open( OUT wfile, fname ));
		img_cmp->Reset( RVRef(wfile), path );
	}
	return img_cmp;
}

/*
=================================================
	SaveImage
=================================================
*/
bool  DrawTestCore::SaveImage (StringView name, const ImageMemView &view) const
{
	using namespace AE::ResLoader;

	const Path	path = (_refImagePath / name).replace_extension( "dds" );

	VFS::FileName	fname;
	CHECK_ERR( _refImageStorage->CreateFile( fname, path ));

	RC<WStream>		wfile;
	CHECK_ERR( _refImageStorage->Open( OUT wfile, fname ));

	DDSImageSaver	saver;
	IntermImage		img;	CHECK( img.SetData( view, null ));

	CHECK_ERR( Cast<IImageSaver>(&saver)->SaveImage( *wfile, img, EImageFormat::DDS ));
	return true;
}

/*
=================================================
	_CompilePipelines
=================================================
*/
bool  DrawTestCore::_CompilePipelines (FStorage_t assetStorage)
{
	auto&	res_mngr = GraphicsScheduler().GetResourceManager();

	{
	  #ifdef AE_ENABLE_METAL
		RC<RStream>	file;
		CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Tests.GraphicsHL.mtlPipelines.bin"} ));
	  #else
		RC<RStream>	file;
		CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Tests.GraphicsHL.vkPipelines.bin"} ));
	  #endif

		PipelinePackDesc	desc;
		desc.stream = file;

		auto	pack_id = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( res_mngr.InitializeResources( RVRef(pack_id) ));
	}

	_canvasPpln		= res_mngr.LoadRenderTech( Default, RenderTechName{"CanvasDrawTest"}, Default );
	_canvasPplnDesk	= res_mngr.LoadRenderTech( Default, RenderTechName{"DesktopCanvasDrawTest"}, Default );		// optional
	CHECK_ERR( _canvasPpln );

	return true;
}

/*
=================================================
	_GetGraphicsCreateInfo
=================================================
*/
GraphicsCreateInfo  DrawTestCore::_GetGraphicsCreateInfo ()
{
	GraphicsCreateInfo	info;

	info.maxFrames		= 2;
	info.staging.readStaticSize .fill( 2_Mb );
	info.staging.writeStaticSize.fill( 2_Mb );

	info.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;

	info.useRenderGraph = true;

	return info;
}

/*
=================================================
	_CompareDumps
=================================================
*/
bool  DrawTestCore::_CompareDumps (StringView right, StringView filename) const
{
	Path	fname = _refImagePath;
	fname.append( String{filename} << ".txt" );


	// read from file
	bool	update_ref	= true;
	String	left;
	{
		RC<RStream>		rfile;
		update_ref = not (_refImageStorage->Open( OUT rfile, VFS::FileName{ToString(fname)} ) and
						  rfile->Read( usize(rfile->Size()), OUT left ));
	}

	// override dump
	if ( update_ref or UpdateAllReferences )
	{
		VFS::FileName	name;
		CHECK_ERR( _refImageStorage->CreateFile( name, fname ));

		RC<WStream>		wfile;
		CHECK_ERR( _refImageStorage->Open( OUT wfile, name ));

		CHECK_ERR( wfile->Write( StringView{right} ));
		return true;
	}

	Parser::CompareLineByLine( left, right,
		[filename, i = 0] (uint lline, StringView lstr, uint rline, StringView rstr) M_NE___
		{
			AE_LOGW( "in: "s << filename << " (" << ToString(i++) << ")\n"
						<< "line mismatch:" << "\n(" << ToString( lline ) << "): " << lstr
						<< "\n(" << ToString( rline ) << "): " << rstr );
			return false; // continue
		},
		[filename] () __NE___ {
			AE_LOGW( "in: "s << filename << "\n\n" << "sizes of dumps are not equal!" );
		});
	return true;
}
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_VULKAN
/*
=================================================
	_Create
=================================================
*/
bool  DrawTestCore::_Create (FStorage_t refStorage)
{
	VDeviceInitializer::InstanceCreateInfo	inst_ci;
	inst_ci.appName			= "TestApp";
	inst_ci.instanceLayers	= _device.GetRecommendedInstanceLayers();
	inst_ci.version			= {1,3};

	CHECK_ERR( _device.CreateInstance( inst_ci ));

	// this is a test and the test should fail for any validation error
	_device.CreateDebugCallback( DefaultDebugMessageSeverity,
								 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOGW(rep.message);  CHECK_FATAL(not rep.isError); });

	CHECK_ERR( _device.ChooseHighPerformanceDevice() );
	CHECK_ERR( _device.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
	CHECK_ERR( _device.CreateLogicalDevice() );
	CHECK_ERR( _device.CheckConstantLimits() );
	CHECK_ERR( _device.CheckExtensions() );

	{
		FlatHashMap<VkQueue, String>	qnames;
		for (auto& q : _device.GetQueues()) {
			qnames.emplace( q.handle, q.debugName );
		}
		_syncLog.Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
	}

	RenderTaskScheduler::InstanceCtor::Create( _device );

	const GraphicsCreateInfo	info = _GetGraphicsCreateInfo();

	auto&	rts = GraphicsScheduler();
	CHECK_ERR( rts.Initialize( info ));

	_refImageStorage	= refStorage;
	_refImagePath		= Path{_device.GetDeviceName()};
	CHECK_ERR( _refImageStorage );

	return true;
}

/*
=================================================
	_RunTests
=================================================
*/
bool  DrawTestCore::_RunTests ()
{
	for (;;)
	{
		if ( not _tests.empty() )
		{
			_syncLog.Enable();

			TestFunc_t&	func	= _tests.front();
			bool		passed	= (this->*func)();

			_syncLog.Disable();

			_testsPassed += uint(passed);
			_testsFailed += uint(not passed);
			_tests.pop_front();

			Scheduler().ProcessTask( ETaskQueue::Main, EThreadSeed(0) );
		}
		else
		{
			AE_LOGI( "Tests passed: " + ToString( _testsPassed ) + ", failed: " + ToString( _testsFailed ));
			break;
		}
	}
	return not _testsFailed;
}

/*
=================================================
	_CompareDumps
=================================================
*/
bool  DrawTestCore::_CompareDumps (StringView filename) const
{
	String	right;
	_syncLog.GetLog( OUT right );
	return _CompareDumps( right, filename );
}


#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_METAL
/*
=================================================
	_Create
=================================================
*/
bool  DrawTestCore::_Create (FStorage_t refStorage)
{
	CHECK_ERR( _device.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
	CHECK_ERR( _device.CreateLogicalDevice() );
	CHECK_ERR( _device.CheckConstantLimits() );
	CHECK_ERR( _device.CheckExtensions() );

	RenderTaskScheduler::InstanceCtor::Create( _device );

	const GraphicsCreateInfo	info = _GetGraphicsCreateInfo();

	auto&	rts = GraphicsScheduler();
	CHECK_ERR( rts.Initialize( info ));

	_refImageStorage	= refStorage;
	_refImagePath		= Path{_device.GetDeviceName()};
	CHECK_ERR( _refImageStorage );

	return true;
}

/*
=================================================
	_CompareDumps
=================================================
*/
bool  DrawTestCore::_CompareDumps (StringView) const
{
	return true;	// not supported for Metal
}

/*
=================================================
	_RunTests
=================================================
*/
bool  DrawTestCore::_RunTests ()
{
	for (;;)
	{
		if ( not _tests.empty() )
		{
			TestFunc_t&	func	= _tests.front();
			bool		passed	= (this->*func)();

			_testsPassed += uint(passed);
			_testsFailed += uint(not passed);
			_tests.pop_front();

			for (; Scheduler().ProcessTask( ETaskQueue::Main, EThreadSeed(0) );) {}
		}
		else
		{
			AE_LOGI( "Tests passed: " + ToString( _testsPassed ) + ", failed: " + ToString( _testsFailed ));
			break;
		}
	}
	return not _testsFailed;
}

#endif // AE_ENABLE_METAL
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_REMOTE_GRAPHICS
/*
=================================================
	_Create
=================================================
*/
bool  DrawTestCore::_Create (FStorage_t refStorage)
{
	using namespace AE::Networking;

	GraphicsCreateInfo	info = _GetGraphicsCreateInfo();

	info.device.appName			= "TestApp";
	info.device.requiredQueues	= EQueueMask::Graphics;
	info.device.optionalQueues	= EQueueMask::All;
	info.device.validation		= EDeviceValidation::Enabled;

	info.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;
	info.swapchain.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
	info.swapchain.options		= EImageOpt::BlitDst;
	info.swapchain.presentMode	= EPresentMode::FIFO;
	info.swapchain.minImageCount= 2;

//	info.deviceAddr				= Networking::IpAddress::FromInt( 192,168,0,105, 0 );	// APC
//	info.deviceAddr				= Networking::IpAddress::FromInt( 192,168,0,102, 0 );	// MacMini
	info.deviceAddr				= Networking::IpAddress::FromInt( 192,168,0,100, 0 );	// Mobile

	info.enableSyncLog			= true;

	CHECK_ERR( _device.Init( info ));
	CHECK_ERR( _device.CheckConstantLimits() );
	CHECK_ERR( _device.CheckExtensions() );

	RenderTaskScheduler::InstanceCtor::Create( _device );

	auto&	rts = GraphicsScheduler();
	CHECK_ERR( rts.Initialize( info ));

	_refImageStorage	= refStorage;
	_refImagePath		= Path{_device.GetDeviceName()};
	CHECK_ERR( _refImageStorage );

	return true;
}

/*
=================================================
	_CompareDumps
=================================================
*/
bool  DrawTestCore::_CompareDumps (StringView filename) const
{
	String	log;
	_device.GetSyncLog( OUT log );
	if ( log.empty() )
		return true;	// not supported if remote device uses Metal API

	return _CompareDumps( log, filename );
}

/*
=================================================
	_RunTests
=================================================
*/
bool  DrawTestCore::_RunTests ()
{
	for (;;)
	{
		if ( not _tests.empty() )
		{
			_device.EnableSyncLog( true );

			TestFunc_t&	func	= _tests.front();
			bool		passed	= (this->*func)();

			_device.EnableSyncLog( false );

			_testsPassed += uint(passed);
			_testsFailed += uint(not passed);
			_tests.pop_front();

			for (; Scheduler().ProcessTask( ETaskQueue::Main, EThreadSeed(0) );) {}
		}
		else
		{
			AE_LOGI( "Tests passed: " + ToString( _testsPassed ) + ", failed: " + ToString( _testsFailed ));
			break;
		}
	}
	return not _testsFailed;
}

#endif // AE_ENABLE_REMOTE_GRAPHICS
