// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

#include "res_loaders/DDS/DDSImageSaver.h"

using namespace AE::App;
using namespace AE::Threading;

extern void Test_Image (ResourceManager &resMngr);
extern void Test_Buffer (ResourceManager &resMngr);

static constexpr uint  c_MaxRenderThreads = 3;

/*
=================================================
	constructor
=================================================
*/
RGTest::RGTest () :
	_device{ True{"enable info log"} }
{
	// too slow
	//_tests.emplace_back( &RGTest::Test_Buffer );
	//_tests.emplace_back( &RGTest::Test_Image );

	_tests.emplace_back( &RGTest::Test_Allocator );
	_tests.emplace_back( &RGTest::Test_FeatureSets );
//	_tests.emplace_back( &RGTest::Test_FrameCounter );
	_tests.emplace_back( &RGTest::Test_ImageFormat );

	_tests.emplace_back( &RGTest::Test_UploadStream1 );
	_tests.emplace_back( &RGTest::Test_UploadStream2 );

	_tests.emplace_back( &RGTest::Test_CopyBuffer1 );
	_tests.emplace_back( &RGTest::Test_CopyBuffer2 );
	_tests.emplace_back( &RGTest::Test_CopyImage1 );
	_tests.emplace_back( &RGTest::Test_CopyImage2 );
	_tests.emplace_back( &RGTest::Test_Compute1 );
	_tests.emplace_back( &RGTest::Test_Compute2 );
	_tests.emplace_back( &RGTest::Test_Draw1 );
	_tests.emplace_back( &RGTest::Test_Draw2 );
	_tests.emplace_back( &RGTest::Test_Draw3 );
	_tests.emplace_back( &RGTest::Test_Draw4 );
	_tests.emplace_back( &RGTest::Test_Draw5 );
	_tests.emplace_back( &RGTest::Test_DrawAsync1 );
	_tests.emplace_back( &RGTest::Test_AsyncCompute1 );
	_tests.emplace_back( &RGTest::Test_AsyncCompute2 );
	_tests.emplace_back( &RGTest::Test_AsyncCompute3 );

  #ifndef AE_ENABLE_METAL
	_tests.emplace_back( &RGTest::Test_DrawMesh1 );
	_tests.emplace_back( &RGTest::Test_DrawMesh2 );
	_tests.emplace_back( &RGTest::Test_RayQuery1 );
	_tests.emplace_back( &RGTest::Test_RayTracing1 );
	_tests.emplace_back( &RGTest::Test_RayTracing2 );
	_tests.emplace_back( &RGTest::Test_RayTracing3 );
	_tests.emplace_back( &RGTest::Test_ShadingRate1 );
	_tests.emplace_back( &RGTest::Test_Ycbcr1 );
  #endif
  #ifdef AE_TEST_SHADER_DEBUGGER
	_tests.emplace_back( &RGTest::Test_Debugger1 );
	_tests.emplace_back( &RGTest::Test_Debugger2 );
	_tests.emplace_back( &RGTest::Test_Debugger3 );
	_tests.emplace_back( &RGTest::Test_Debugger4 );
	_tests.emplace_back( &RGTest::Test_Debugger5 );
  #endif

	RenderTaskScheduler::InstanceCtor::Create( _device );
}

/*
=================================================
	Test_Image/Buffer
=================================================
*/
bool  RGTest::Test_Image ()
{
	::Test_Image( GraphicsScheduler().GetResourceManager() );
	return true;
}

bool  RGTest::Test_Buffer ()
{
	::Test_Buffer( GraphicsScheduler().GetResourceManager() );
	return true;
}

/*
=================================================
	_LoadReference
=================================================
*/
Unique<ImageComparator>  RGTest::_LoadReference (StringView name) const
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
		if ( _refImageStorage->Open( OUT wfile, fname ))
			img_cmp->Reset( RVRef(wfile), path );
	}
	return img_cmp;
}

/*
=================================================
	SaveImage
=================================================
*/
bool  RGTest::SaveImage (StringView name, const ImageMemView &view) const
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
	Run
=================================================
*/
bool  RGTest::Run (FStorage_t assetStorage, FStorage_t refStorage)
{
	CHECK_ERR( _Create( refStorage ));

	for (uint i = 0; i < c_MaxRenderThreads; ++i) {
		Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
				EThreadArray{ EThread::PerFrame, EThread::Renderer },
				"render thread"s << ToString(i)
			}));
	}
	CHECK_ERR( _CompilePipelines( assetStorage ));

	bool	result = _RunTests();

	_Destroy();

	return result;
}

/*
=================================================
	_Destroy
=================================================
*/
void  RGTest::_Destroy ()
{
	_pipelines		= null;
	_dbgPipelines	= null;
	_acPipelines	= null;
	_msPipelines	= null;
	_rtPipelines	= null;
	_rqPipelines	= null;
	_vrsPipelines	= null;
	_ycbcrPipelines	= null;

	RenderTaskScheduler::InstanceCtor::Destroy();

  #ifdef AE_ENABLE_VULKAN
	_syncLog.Deinitialize( INOUT _device.EditDeviceFnTable() );
  #endif

	CHECK( _device.DestroyLogicalDevice() );
	CHECK( _device.DestroyInstance() );
}

/*
=================================================
	_CompilePipelines
=================================================
*/
bool  RGTest::_CompilePipelines (FStorage_t assetStorage)
{
	auto&	res_mngr = GraphicsScheduler().GetResourceManager();

	{
	  #ifdef AE_ENABLE_METAL
		RC<RStream>	file;
		CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Tests.Graphics.mtlPipelines.bin"} ));
	  #else
		RC<RStream>	file;
		CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Tests.Graphics.vkPipelines.bin"} ));
	  #endif

		PipelinePackDesc	desc;
		desc.stream = file;

		auto	pack_id = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( res_mngr.InitializeResources( RVRef(pack_id) ));
	}

	_pipelines = res_mngr.LoadRenderTech( Default, RenderTechs::DrawTestRT, Default );
	CHECK_ERR( _pipelines );

	_dbgPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::DebugDrawTestRT,	Default );
	_acPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::AsyncCompTestRT,	Default );
	_msPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::DrawMeshesTestRT,	Default );
	_rtPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::RayTracingTestRT,	Default );
	_rqPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::RayQueryTestRT,	Default );
	_vrsPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::VRSTestRT,			Default );
	_ycbcrPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::Ycbcr_RTech,		Default );

	return true;
}

/*
=================================================
	_GetGraphicsCreateInfo
=================================================
*/
GraphicsCreateInfo  RGTest::_GetGraphicsCreateInfo ()
{
	GraphicsCreateInfo	info;
	info.maxFrames		= 2;
	info.staging.readStaticSize		= 2_Mb;
	info.staging.writeStaticSize	= 2_Mb;

	info.swapchain.colorFormat	= EPixelFormat::RGBA8_UNorm;

	info.useRenderGraph = true;

	return info;
}

/*
=================================================
	_CompareDumps
=================================================
*/
bool  RGTest::_CompareDumps (StringView right, StringView filename) const
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
bool  RGTest::_Create (FStorage_t refStorage)
{
	{
		VDeviceInitializer::InstanceCreateInfo	inst_ci;
		inst_ci.appName			= "TestApp";
		inst_ci.instanceLayers	= _device.GetRecommendedInstanceLayers();
		inst_ci.version			= {1,3};

		#if 0
		const VkValidationFeatureEnableEXT	sync_enable_feats  [] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
		const VkValidationFeatureDisableEXT	sync_disable_feats [] = { VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT };

		inst_ci.enableValidations	= sync_enable_feats;
		inst_ci.disableValidations	= sync_disable_feats;
		#endif

		CHECK_ERR( _device.CreateInstance( inst_ci ));
	}

	// this is a test and the test should fail for any validation error
	_device.CreateDebugCallback( DefaultDebugMessageSeverity,
								 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOGW(rep.message);  CHECK_FATAL(not rep.isError); });

  #if AE_VK_TIMELINE_SEMAPHORE
	const EQueueMask	opt_queues = EQueueMask::All;
  #else
	const EQueueMask	opt_queues = Default;
  #endif

	CHECK_ERR( _device.ChooseHighPerformanceDevice() );
	CHECK_ERR( _device.CreateDefaultQueues( EQueueMask::Graphics, opt_queues ));
	CHECK_ERR( _device.CreateLogicalDevice() );
	CHECK_ERR( _device.CheckConstantLimits() );
	CHECK_ERR( _device.CheckExtensions() );

	{
		FlatHashMap<VkQueue, String>	qnames;
		for (auto& q : _device.GetQueues()) {
			qnames.emplace( q.handle, String{q.debugName} );
		}
		_syncLog.Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
	}

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
bool  RGTest::_RunTests ()
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

/*
=================================================
	_CompareDumps
=================================================
*/
bool  RGTest::_CompareDumps (StringView filename) const
{
	String	right;
	_syncLog.GetLog( OUT right );
	return _CompareDumps( right, filename );
}

/*
=================================================
	Test_VulkanRenderGraph
=================================================
*/
extern void  Test_VulkanRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage)
{
	RGTest		test;
	CHECK_FATAL( test.Run( assetStorage, refStorage ));
	TEST_PASSED();
}

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_METAL
/*
=================================================
	_Create
=================================================
*/
bool  RGTest::_Create (FStorage_t refStorage)
{
	CHECK_ERR( _device.ChooseHighPerformanceDevice() );
	CHECK_ERR( _device.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
	CHECK_ERR( _device.CreateLogicalDevice() );
	CHECK_ERR( _device.CheckConstantLimits() );
	CHECK_ERR( _device.CheckExtensions() );

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
bool  RGTest::_CompareDumps (StringView) const
{
	return true;	// not supported for Metal
}

/*
=================================================
	_RunTests
=================================================
*/
bool  RGTest::_RunTests ()
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

/*
=================================================
	Test_MetalRenderGraph
=================================================
*/
extern void  Test_MetalRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage)
{
	RGTest		test;
	CHECK_FATAL( test.Run( assetStorage, refStorage ));
	TEST_PASSED();
}

#endif // AE_ENABLE_METAL
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_REMOTE_GRAPHICS
/*
=================================================
	_Create
=================================================
*/
bool  RGTest::_Create (FStorage_t refStorage)
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

	info.deviceAddr				= Networking::IpAddress::FromInt( 192,168,0,0, 0 );	// TODO: set RemoveDevice IP address

	info.enableSyncLog			= true;

	CHECK_ERR( _device.Init( info ));
	CHECK_ERR( _device.CheckConstantLimits() );
	CHECK_ERR( _device.CheckExtensions() );

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
bool  RGTest::_CompareDumps (StringView filename) const
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
bool  RGTest::_RunTests ()
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

/*
=================================================
	Test_RemoteRenderGraph
=================================================
*/
extern void  Test_RemoteRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage)
{
	RGTest		test;
	CHECK_FATAL( test.Run( assetStorage, refStorage ));
	TEST_PASSED();
}

#endif // AE_ENABLE_REMOTE_GRAPHICS
