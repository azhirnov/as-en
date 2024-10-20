// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "TestsGraphics.pch.h"

using namespace AE;
using namespace AE::Threading;
using namespace AE::Graphics;

using EStatus = AE::Threading::IAsyncTask::EStatus;

using ImageComparator = GraphicsTest::ImageComparator;

static constexpr seconds	c_MaxTimeout	{30*60};
static const EThreadArray	c_ThreadArr		{EThread::PerFrame, EThread::Renderer};


class RGTest
{
// types
protected:
	using TestFunc_t	= bool (RGTest::*) ();
	using TestQueue_t	= RingBuffer< TestFunc_t >;
	using FStorage_t	= RC<VFS::IVirtualFileStorage>;

	static constexpr bool	UpdateAllReferences = false;


// variables
protected:
	RenderTechPipelinesPtr		_pipelines;
	RenderTechPipelinesPtr		_dbgPipelines;
	RenderTechPipelinesPtr		_acPipelines;		// async compute
	RenderTechPipelinesPtr		_msPipelines;		// mesh shader
	RenderTechPipelinesPtr		_rtPipelines;		// ray tracing
	RenderTechPipelinesPtr		_rqPipelines;		// ray query
	RenderTechPipelinesPtr		_vrsPipelines;		// fragment shading rate
	RenderTechPipelinesPtr		_ycbcrPipelines;	// video image, ycbcr

	TestQueue_t					_tests;
	uint						_testsPassed		= 0;
	uint						_testsFailed		= 0;

	FStorage_t					_refImageStorage;
	Path						_refImagePath;

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
	RGTest ();
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
	bool  Test_Image ();
	bool  Test_Buffer ();
	bool  Test_Allocator ();
	bool  Test_FeatureSets ();
	bool  Test_FrameCounter ();
	bool  Test_ImageFormat ();

	bool  Test_CopyBuffer1 ();
	bool  Test_CopyBuffer2 ();
	bool  Test_CopyImage1 ();
	bool  Test_CopyImage2 ();
	bool  Test_UploadStream1 ();
	bool  Test_UploadStream2 ();

	bool  Test_Compute1 ();
	bool  Test_Compute2 ();			// with RG
	bool  Test_AsyncCompute1 ();
	bool  Test_AsyncCompute2 ();
	bool  Test_AsyncCompute3 ();

	bool  Test_Draw1 ();
	bool  Test_Draw2 ();			// vertex buffer
	bool  Test_Draw3 ();			// push constants
	bool  Test_Draw4 ();			// with RG & vstream
	bool  Test_Draw5 ();			// with RG & vstream
	bool  Test_DrawMesh1 ();
	bool  Test_DrawMesh2 ();
	//bool  Test_DrawMultipass1 ();
	bool  Test_DrawAsync1 ();

	bool  Test_RayQuery1 ();

	bool  Test_RayTracing1 ();
	bool  Test_RayTracing2 ();		// indirect build
	bool  Test_RayTracing3 ();

	bool  Test_ShadingRate1 ();
	bool  Test_Ycbcr1 ();

	bool  Test_Debugger1 ();		// compute
	bool  Test_Debugger2 ();		// graphics
	bool  Test_Debugger3 ();		// mesh
	bool  Test_Debugger4 ();		// ray tracing
	bool  Test_Debugger5 ();		// ray query

	// TODO:
	//	MSAA resolve
};



ND_ inline String  _GetFuncName (StringView src)
{
	usize	pos = src.find_last_of( "::" );

	if ( pos != StringView::npos )
		return String{ src.substr( pos+1 )};
	else
		return String{ src };
}


# define TEST_NAME			_GetFuncName( AE_FUNCTION_NAME )

# define RG_CHECK( ... )	{ bool res = (__VA_ARGS__);  CHECK_MSG( res, AE_TOSTRING( __VA_ARGS__ ));  result &= res; }
