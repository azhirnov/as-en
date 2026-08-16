// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "UnitTest_Common.h"

using namespace AE;
using namespace AE::Threading;
using namespace AE::Graphics;

using ImageComparator = GraphicsTest::ImageComparator;

static constexpr seconds	c_MaxTimeout	{30*60};
static const EThreadArray	c_ThreadArr		{EThread::PerFrame, EThread::Renderer};


class RGTest
{
// types
protected:
	enum class ECode
	{
		Passed,
		Failed,
		Skipped,
	};

	using TestFunc_t	= ECode (RGTest::*) ();
	using TestQueue_t	= RingBuffer< TestFunc_t >;
	using FStorage_t	= RC<VFS::IVirtualFileStorage>;

	static const bool	c_UpdateAllReferences;


// variables
protected:
	RenderTechPipelinesPtr		_pipelines;
	RenderTechPipelinesPtr		_dbgPipelines;
	RenderTechPipelinesPtr		_acPipelines;			// async compute
	RenderTechPipelinesPtr		_msPipelines;			// mesh shader
	RenderTechPipelinesPtr		_rtPipelines;			// ray tracing
	RenderTechPipelinesPtr		_rqPipelines;			// ray query
	RenderTechPipelinesPtr		_vrsPipelines;			// fragment shading rate
	RenderTechPipelinesPtr		_ycbcrPipelines;		// video image, ycbcr
	RenderTechPipelinesPtr		_mvPipelines;			// multiview, viewport array
	RenderTechPipelinesPtr		_icbPipelines;			// indirect command buffer (device generated commands)
	RenderTechPipelinesPtr		_ommPipelines;			// opacity micromap
	RenderTechPipelinesPtr		_blsPipelines;			// bindless
	RenderTechPipelinesPtr		_dhPipelines;			// descriptor heap (bindless v2)

	TestQueue_t					_tests;
	uint						_testsPassed		= 0;
	uint						_testsFailed		= 0;
	uint						_testsSkipped		= 0;

	FStorage_t					_refImageStorage;
	Path						_refImagePath;

	ArrayView<const char*>		_consoleArgs;

  #if defined(AE_ENABLE_VULKAN)
	VDeviceInitializer			_device;
	VulkanSyncLog				_syncLog;

  #elif defined(AE_ENABLE_METAL)
	MDeviceInitializer			_device;

  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
	RDeviceInitializer			_device;

  #else
  #	error not implemented
  #endif


// methods
public:
	RGTest (StringView, ArrayView<const char*>);
	~RGTest () {}

	bool  Run (FStorage_t assetStorage, FStorage_t refStorage);

	bool  SaveImage (StringView name, const ImageMemView &view) const;

private:
	ND_ Unique<ImageComparator>  _LoadReference (StringView filename) const;

	ND_ bool  _CompareDumps (StringView filename) const;
	ND_ bool  _CompareDumps (StringView syncLog, StringView filename) const;

	ND_ bool  _Create (FStorage_t refStorage);
	ND_ bool  _RunTests ();
		void  _Destroy ();

	ND_ bool  _CompilePipelines (FStorage_t assetStorage);

	ND_ static GraphicsCreateInfo  _GetGraphicsCreateInfo ();

private:
	ECode  Test_Image ();
	ECode  Test_Buffer ();
	ECode  Test_Allocator ();
	ECode  Test_FeatureSets ();
	ECode  Test_FrameCounter ();
	ECode  Test_ImageFormat ();

	ECode  Test_CopyBuffer1 ();
	ECode  Test_CopyBuffer2 ();
	ECode  Test_CopyImage1 ();
	ECode  Test_CopyImage2 ();
	ECode  Test_UploadStream1 ();
	ECode  Test_UploadStream2 ();

	ECode  Test_Compute1 ();
	ECode  Test_Compute2 ();			// with RG
	ECode  Test_AsyncCompute1 ();
	ECode  Test_AsyncCompute2 ();
	ECode  Test_AsyncCompute3 ();

	ECode  Test_Draw1 ();
	ECode  Test_Draw2 ();			// vertex buffer
	ECode  Test_Draw3 ();			// push constants
	ECode  Test_Draw4 ();			// with RG & vstream
	ECode  Test_Draw5 ();			// with RG & vstream
	ECode  Test_DrawMesh1 ();
	ECode  Test_DrawMesh2 ();
	//ECode  Test_DrawMultipass1 ();
	ECode  Test_DrawAsync1 ();

	ECode  Test_RayQuery1 ();

	ECode  Test_RayTracing1 ();
	ECode  Test_RayTracing2 ();				// indirect build
	ECode  Test_RayTracing3 ();
	ECode  Test_RayTracingCluster1 ();		// CLAS
	ECode  Test_RayTracingPartitioned1 ();	// PTLAS
	ECode  Test_OpacityMicromap1 ();
	// TODO: AS serialization, compaction

	ECode  Test_ShadingRate1 ();
	ECode  Test_Ycbcr1 ();
	ECode  Test_MultiView ();
	ECode  Test_ViewportArray ();
	ECode  Test_Bindless1 ();
	ECode  Test_Bindless2 ();
	ECode  Test_UpdateTemplate1 ();

	ECode  Test_IndirectCommandBuffer1 ();
	ECode  Test_IndirectCommandBuffer2 ();

	ECode  Test_VideoEncoder1 ();

	ECode  Test_Debugger1 ();		// compute
	ECode  Test_Debugger2 ();		// graphics
	ECode  Test_Debugger3 ();		// mesh
	ECode  Test_Debugger4 ();		// ray tracing
	ECode  Test_Debugger5 ();		// ray query
	ECode  Test_Debugger6 ();		// asserts in compute

	// TODO:
	//	MSAA resolve
};



Nd__In String  _GetFuncName (StringView src)
{
	usize	pos = src.find_last_of( "::" );

	if ( pos != StringView::npos )
		return String{ src.substr( pos+1 )};
	else
		return String{ src };
}


# define TEST_NAME			_GetFuncName( AE_FUNCTION_NAME )

# define RG_CHECK( ... )	{ bool res = (__VA_ARGS__);  CHECK_MSG( res, AE_TOSTRING( __VA_ARGS__ ));  result &= res; }
