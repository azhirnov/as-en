// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

#include "res_loaders/DDS/DDSImageSaver.h"

extern void Test_Image (ResourceManager &resMngr);
extern void Test_Buffer (ResourceManager &resMngr);

static constexpr uint  c_MaxRenderThreads = 3;

const bool	RGTest::c_UpdateAllReferences = false;

/*
=================================================
	constructor
=================================================
*/
RGTest::RGTest (StringView testName, ArrayView<const char*> args) :
	_consoleArgs{ args },
	_device{ True{"enable info log"}, False{"disable allocator stats"} }
{
	#ifdef AE_ENABLE_VULKAN
	# ifdef AE_PLATFORM_WINDOWS
	//	_device.ChooseDriver( List{ EDriver::LavaPipe });
	# endif
	# ifdef AE_PLATFORM_LINUX
		_device.ChooseDriver( List{ EDriver::RADV });
	# endif
	#endif

	#undef RUN_TEST
	#define RUN_TEST( _name_ )\
		if ( testName.empty() or testName == AE_TOSTRING(_name_) )\
			_tests.emplace_back( &RGTest::_name_ );

	// too slow
	//RUN_TEST( Test_Buffer );
	//RUN_TEST( Test_Image );

	RUN_TEST( Test_Allocator );
	RUN_TEST( Test_FeatureSets );
//	RUN_TEST( Test_FrameCounter );
	RUN_TEST( Test_ImageFormat );

	RUN_TEST( Test_UploadStream1 );
	RUN_TEST( Test_UploadStream2 );

	RUN_TEST( Test_CopyBuffer1 );
	RUN_TEST( Test_CopyBuffer2 );
	RUN_TEST( Test_CopyImage1 );
	RUN_TEST( Test_CopyImage2 );
	RUN_TEST( Test_Compute1 );
	RUN_TEST( Test_Compute2 );
	RUN_TEST( Test_Draw1 );
	RUN_TEST( Test_Draw2 );
	RUN_TEST( Test_Draw3 );
	RUN_TEST( Test_Draw4 );
	RUN_TEST( Test_Draw5 );
	RUN_TEST( Test_DrawAsync1 );
	RUN_TEST( Test_AsyncCompute1 );
	RUN_TEST( Test_AsyncCompute2 );
	RUN_TEST( Test_AsyncCompute3 );

  #ifndef AE_ENABLE_METAL
	RUN_TEST( Test_DrawMesh1 );
	RUN_TEST( Test_DrawMesh2 );
	RUN_TEST( Test_RayQuery1 );
	RUN_TEST( Test_RayTracing1 );
	RUN_TEST( Test_RayTracing2 );
	RUN_TEST( Test_RayTracing3 );
	//RUN_TEST( Test_RayTracingCluster1 );
	//RUN_TEST( Test_RayTracingPartitioned1 );
	RUN_TEST( Test_OpacityMicromap1 );
	RUN_TEST( Test_ShadingRate1 );
	RUN_TEST( Test_Ycbcr1 );
	RUN_TEST( Test_MultiView );
	RUN_TEST( Test_ViewportArray );
	RUN_TEST( Test_IndirectCommandBuffer1 );
	RUN_TEST( Test_IndirectCommandBuffer2 );
	//RUN_TEST( Test_VideoEncoder1 );
  #endif
  #ifdef AE_TEST_SHADER_DEBUGGER
	RUN_TEST( Test_Debugger1 );
	RUN_TEST( Test_Debugger2 );
	RUN_TEST( Test_Debugger3 );
	RUN_TEST( Test_Debugger4 );
	RUN_TEST( Test_Debugger5 );
	RUN_TEST( Test_Debugger6 );
  #endif

	RenderTaskScheduler::InstanceCtor::Create( _device );
	#undef RUN_TEST
}

/*
=================================================
	Test_Image/Buffer
=================================================
*/
RGTest::ECode  RGTest::Test_Image ()
{
	::Test_Image( GraphicsScheduler().GetResourceManager() );
	return ECode::Passed;
}

RGTest::ECode  RGTest::Test_Buffer ()
{
	::Test_Buffer( GraphicsScheduler().GetResourceManager() );
	return ECode::Passed;
}

/*
=================================================
	_LoadReference
=================================================
*/
Unique<ImageComparator>  RGTest::_LoadReference (StringView name) const
{
	Unique<ImageComparator>	img_cmp{ new ImageComparator{} };

	const Path	path	= (_refImagePath / name).replace_extension( ".png" );
	bool		loaded	= false;
	auto		open_file = [s = _refImageStorage, p = Path{path}.replace_extension( ".diff.png" )] ()
	{{
		VFS::FileName	fname;
		RC<WStream>		diff_file;
		CHECK( s->CreateFile( OUT fname, p ));
		CHECK( s->Open( OUT diff_file, fname ));
		return diff_file;
	}};

	if ( not c_UpdateAllReferences )
	{
		RC<RStream>	rfile;
		if ( _refImageStorage->Open( OUT rfile, VFS::FileName{ToString(path)} ))
		{
			loaded = img_cmp->LoadReference( RVRef(rfile), path, open_file );
		}
	}

	if ( not loaded )
	{
		VFS::FileName	fname;
		CHECK_ERR( _refImageStorage->CreateFile( OUT fname, path ), null );

		RC<WStream>		wfile;
		if ( _refImageStorage->Open( OUT wfile, fname ))
			img_cmp->Reset( RVRef(wfile), path, open_file );
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

	const Path	path = (_refImagePath / name).replace_extension( ".dds" );

	VFS::FileName	fname;
	CHECK_ERR( _refImageStorage->CreateFile( OUT fname, path ));

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
	_mvPipelines	= null;
	_icbPipelines	= null;
	_ommPipelines	= null;

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
		CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Tests.GraphicsRHI.mtlPipelines.bin"} ));
	  #else
		RC<RStream>	file;
		CHECK_ERR( assetStorage->Open( OUT file, VFS::FileName{"Tests.GraphicsRHI.vkPipelines.bin"} ));
	  #endif

		PipelinePackDesc	desc;
		desc.stream = file;

		auto	pack_id = res_mngr.LoadPipelinePack( desc );
		CHECK_ERR( res_mngr.InitializeResources( RVRef(pack_id) ));
	}

	_pipelines = res_mngr.LoadRenderTech( Default, RenderTechs::DrawTest_RTech );
	CHECK_ERR( _pipelines );

	_dbgPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::DebugDraw_RTech );
	_acPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::AsyncComp_RTech );
	_msPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::DrawMeshes_RTech );
	_rtPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::RayTracing_RTech );
	_rqPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::RayQuery_RTech );
	_vrsPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::VRS_RTech );
	_ycbcrPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::Ycbcr_RTech );
	_mvPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::MultiView_RTech );
	_icbPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::IndirectCmds_RTech );
	_ommPipelines	= res_mngr.LoadRenderTech( Default, RenderTechs::OpacityMicromap_RTech );

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
	info.maxFrames					= 2;
	info.staging.readStaticSize		= 2_MiB;
	info.staging.writeStaticSize	= 2_MiB;

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
	if ( update_ref or c_UpdateAllReferences )
	{
		VFS::FileName	name;
		CHECK_ERR( _refImageStorage->CreateFile( OUT name, fname ));

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
		inst_ci.version			= {1,4};

		#if 0
		const VkValidationFeatureEnableEXT	sync_enable_feats  [] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
		const VkValidationFeatureDisableEXT	sync_disable_feats [] = { VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT };

		inst_ci.enableValidations	= sync_enable_feats;
		inst_ci.disableValidations	= sync_disable_feats;
		#endif

		CHECK_ERR( _device.CreateInstance( inst_ci ));
	}

	// this is a test and the test should fail for any validation error
	_device.CreateDebugCallback( VDeviceInitializer::c_DefaultDebugMessageSeverity,
								 VDeviceInitializer::c_DefaultDebugMessageTypes,
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
			ECode		code	= (this->*func)();

			_syncLog.Disable();

			_testsPassed  += uint(code == ECode::Passed);
			_testsFailed  += uint(code == ECode::Failed);
			_testsSkipped += uint(code == ECode::Skipped);
			_tests.pop_front();

			for (; Scheduler().ProcessTask( ETaskQueue::Main, EThreadSeed(0) );) {}
		}
		else
		{
			AE_LOGI( "Tests passed: "s << ToString( _testsPassed ) << ", skipped: " << ToString( _testsSkipped ) << ", failed: " << ToString( _testsFailed ));
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
extern void  Test_VulkanRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView testName, ArrayView<const char*> args)
{
	RGTest		test {testName, args};
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
			ECode		code	= (this->*func)();

			_testsPassed  += uint(code == ECode::Passed);
			_testsFailed  += uint(code == ECode::Failed);
			_testsSkipped += uint(code == ECode::Skipped);
			_tests.pop_front();

			for (; Scheduler().ProcessTask( ETaskQueue::Main, EThreadSeed(0) );) {}
		}
		else
		{
			AE_LOGI( "Tests passed: "s << ToString( _testsPassed ) << ", skipped: " << ToString( _testsSkipped ) << ", failed: " << ToString( _testsFailed ));
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
extern void  Test_MetalRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView testName, ArrayView<const char*> args)
{
	RGTest		test {testName, args};
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

	String	ip_addr	= Parser::GetCommandLineArg( _consoleArgs, "-rm-addr" );
	String	port	= Parser::GetCommandLineArg( _consoleArgs, "-rm-port" );

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

	if ( not ip_addr.empty() and not port.empty() )
		info.deviceAddr = IpAddress::FromServiceTCP( ip_addr, port );
	else
		info.deviceAddr = AE_RMG_IPv4;

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
			ECode		code	= (this->*func)();

			_device.EnableSyncLog( false );

			_testsPassed  += uint(code == ECode::Passed);
			_testsFailed  += uint(code == ECode::Failed);
			_testsSkipped += uint(code == ECode::Skipped);
			_tests.pop_front();

			for (; Scheduler().ProcessTask( ETaskQueue::Main, EThreadSeed(0) );) {}
		}
		else
		{
			AE_LOGI( "Tests passed: "s << ToString( _testsPassed ) << ", skipped: " << ToString( _testsSkipped ) << ", failed: " << ToString( _testsFailed ));
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
extern void  Test_RemoteRenderGraph (RC<VFS::IVirtualFileStorage> assetStorage, RC<VFS::IVirtualFileStorage> refStorage, StringView testName, ArrayView<const char*> args)
{
	RGTest		test {testName, args};
	CHECK_FATAL( test.Run( assetStorage, refStorage ));
	TEST_PASSED();
}

#endif // AE_ENABLE_REMOTE_GRAPHICS
