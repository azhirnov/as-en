// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
#include "base/Platforms/Platform.h"
#include "base/Stream/FileStream.h"
#include "base/Algorithms/StringParser.h"

#include "threading/TaskSystem/WorkerThread.h"

#include "Test_RenderGraph.h"

#include "../UnitTest_Common.h"

using namespace AE::App;
using namespace AE::Threading;

extern void Test_Image (IResourceManager &resMngr);
extern void Test_Buffer (IResourceManager &resMngr);


/*
=================================================
	constructor
=================================================
*/
RGTest::RGTest () :
	#if defined(AE_ENABLE_VULKAN)
		_refDumpPath{ AE_CURRENT_DIR "/Vulkan/ref" },
		_vulkan{ True{"enable info log"} },
		_swapchain{ _vulkan }
	
	#elif defined(AE_ENABLE_METAL)
		_refDumpPath{ AE_CURRENT_DIR "/Metal/ref" }
	#endif
{
	// too slow
	//_tests.emplace_back( &RGTest::Test_Buffer );
	//_tests.emplace_back( &RGTest::Test_Image );
	_tests.emplace_back( &RGTest::Test_FrameCounter );
	
	_tests.emplace_back( &RGTest::Test_UploadStream1 );
	_tests.emplace_back( &RGTest::Test_UploadStream2 );

	_tests.emplace_back( &RGTest::Test_CopyBuffer1 );
	_tests.emplace_back( &RGTest::Test_CopyBuffer2 );
	_tests.emplace_back( &RGTest::Test_CopyImage1 );
	_tests.emplace_back( &RGTest::Test_Compute1 );
	_tests.emplace_back( &RGTest::Test_AsyncCompute1 );
	_tests.emplace_back( &RGTest::Test_Draw1 );
	_tests.emplace_back( &RGTest::Test_Draw2 );
	_tests.emplace_back( &RGTest::Test_DrawAsync1 );
	_tests.emplace_back( &RGTest::Test_DrawMesh1 );
	_tests.emplace_back( &RGTest::Test_RayQuery1 );

	// TODO:
	//_tests.emplace_back( &RGTest::Test_Draw3 );
	//_tests.emplace_back( &RGTest::Test_AsyncCompute2 );
	//_tests.emplace_back( &RGTest::Test_RayTracing1 );
}

/*
=================================================
	Test_Image/BUffer
----
	too slow
=================================================
*/
bool  RGTest::Test_Image ()
{
	::Test_Image( RenderTaskScheduler().GetResourceManager() );
	return true;
}

bool  RGTest::Test_Buffer ()
{
	::Test_Buffer( RenderTaskScheduler().GetResourceManager() );
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
	
	Path	path{ AE_VULKAN_REF_IMG_PATH };
	FileSystem::CreateDirectories( path );

	path.append( name );
	path.replace_extension( "dds" );

	img_cmp->LoadReference( RVRef(path) );

	return img_cmp;
}

/*
=================================================
	SaveImage
=================================================
*/
bool  RGTest::SaveImage (StringView name, const ImageMemView &view)
{
	return true;
}

/*
=================================================
	Run
=================================================
*/
bool  RGTest::Run (IApplication &app, IWindow &wnd)
{
	CHECK_ERR( _Create( app, wnd ));

	bool	result = _RunTests();

	_Destroy();

	return result;
}

/*
=================================================
	_CompilePipelines
=================================================
*/
bool  RGTest::_CompilePipelines ()
{
	return true;
}
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
/*
=================================================
	_Create
=================================================
*/
bool  RGTest::_Create (IApplication &app, IWindow &wnd)
{
	ArrayView<const char*>	window_ext = app.GetVulkanInstanceExtensions();

	CHECK_ERR( _vulkan.CreateInstance( "TestApp", "AE", _vulkan.GetRecomendedInstanceLayers(), window_ext, {1,2} ));

	// this is a test and the test should fail for any validation error
	_vulkan.CreateDebugCallback( DefaultDebugMessageSeverity,
								 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOGI(rep.message);  /*CHECK_FATAL(not rep.isError);*/ });
	
	CHECK_ERR( _vulkan.ChooseHighPerformanceDevice() );
	CHECK_ERR( _vulkan.CreateDefaultQueues( EQueueMask::Graphics, EQueueMask::All ));
	CHECK_ERR( _vulkan.CreateLogicalDevice() );
	CHECK_ERR( _vulkan.CheckConstantLimits() );
	CHECK_ERR( _vulkan.CheckExtensions() );

	_refDumpPath /= _vulkan.GetProperties().properties.deviceName;
	FileSystem::CreateDirectories( _refDumpPath );

	{
		FlatHashMap<VkQueue, String>	qnames;
		for (auto& q : _vulkan.GetQueues()) {
			qnames.emplace( q.handle, q.debugName );
		}
		_syncLog.Initialize( INOUT _vulkan.EditDeviceFnTable(), RVRef(qnames) );
	}

	VRenderTaskScheduler::CreateInstance( _vulkan );
	
	GraphicsCreateInfo	info;
	info.maxFrames		= 2;
	info.staging.readStaticSize .fill( 2_Mb );
	info.staging.writeStaticSize.fill( 2_Mb );
	info.staging.maxReadDynamicSize		= 16_Mb;
	info.staging.maxWriteDynamicSize	= 16_Mb;
	info.staging.dynamicBlockSize		= 4_Mb;

	auto&	rts = RenderTaskScheduler();
	CHECK_ERR( rts.Initialize( info ));

	CHECK_ERR( _swapchain.CreateSurface( wnd.GetNative() ));
	CHECK_ERR( rts.GetResourceManager().OnSurfaceCreated( _swapchain ));
	
	VSwapchainInitializer::CreateInfo	swapchain_ci;
	swapchain_ci.viewSize = wnd.GetSurfaceSize();
	CHECK_ERR( _swapchain.Create( &rts.GetResourceManager(), swapchain_ci ));

	for (uint i = 0; i < 2; ++i) {
		Scheduler().AddThread( MakeRC<WorkerThread>( WorkerThread::ThreadMask{}.insert( WorkerThread::EThread::Worker ).insert( WorkerThread::EThread::Renderer ),
													 nanoseconds{1}, milliseconds{4}, "render thread" ));
	}

	CHECK_ERR( _CompilePipelines() );

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

			Scheduler().ProcessTask( IAsyncTask::EThread::Main, 0 );
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
	_Destroy
=================================================
*/
void  RGTest::_Destroy ()
{
	_syncLog.Deinitialize( INOUT _vulkan.EditDeviceFnTable() );

	_pipelines		= null;
	_acPipelines	= null;
	_msPipelines	= null;
	_rtPipelines	= null;
	_rqPipelines	= null;

	_swapchain.Destroy();
	_swapchain.DestroySurface();

	VRenderTaskScheduler::DestroyInstance();

	CHECK( _vulkan.DestroyLogicalDevice() );
	CHECK( _vulkan.DestroyInstance() );
}

/*
=================================================
	_CompareDumps
=================================================
*/
bool  RGTest::_CompareDumps (StringView filename) const
{
	Path	fname = _refDumpPath;	fname.append( String{filename} << ".txt" );

	String	right;
	_syncLog.GetLog( OUT right );
		
	// override dump
	if ( UpdateAllReferenceDumps )
	{
		FileWStream		wfile{ fname };
		CHECK_ERR( wfile.IsOpen() );
		CHECK_ERR( wfile.Write( StringView{right} ));
		return true;
	}

	// read from file
	String	left;
	{
		FileRStream		rfile{ fname };
		CHECK_ERR( rfile.IsOpen() );
		CHECK_ERR( rfile.Read( usize(rfile.Size()), OUT left ));
	}

	return StringParser::CompareLineByLine( left, right,
				[filename] (uint lline, StringView lstr, uint rline, StringView rstr)
				{
					AE_LOGE( "in: "s << filename << "\n\n"
								<< "line mismatch:" << "\n(" << ToString( lline ) << "): " << lstr
								<< "\n(" << ToString( rline ) << "): " << rstr );
				},
				[filename] () { AE_LOGE( "in: "s << filename << "\n\n" << "sizes of dumps are not equal!" ); }
			);
}

/*
=================================================
	Test_VulkanRenderGraph
=================================================
*/
extern void Test_VulkanRenderGraph (IApplication &app, IWindow &wnd)
{
	RGTest		test;
	CHECK_FATAL( test.Run( app, wnd ));

	TEST_PASSED();
}

#endif	// AE_ENABLE_VULKAN
