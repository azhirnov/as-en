// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "TestsGraphicsHL.pch.h"

using namespace AE;
using namespace AE::Threading;

using EStatus = AE::Threading::IAsyncTask::EStatus;

using ImageComparator = GraphicsTest::ImageComparator;

static constexpr seconds	c_MaxTimeout	{100};
static const EThreadArray	c_ThreadArr		{EThread::PerFrame, EThread::Renderer};


class DrawTestCore
{
// types
protected:
	using TestFunc_t	= bool (DrawTestCore::*) ();
	using TestQueue_t	= RingBuffer< TestFunc_t >;
	using FStorage_t	= RC<AE::VFS::IVirtualFileStorage>;

	static constexpr bool	UpdateAllReferences = false;


// variables
protected:
	Unique<Canvas>				_canvas;
	RenderTechPipelinesPtr		_canvasPpln;
	RenderTechPipelinesPtr		_canvasPplnDesk;		// for MinDesktop FS

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
	const ushort				_serverPort		= 3000;

  #else
  #	error not implemented
  #endif


// methods
public:
	DrawTestCore ();
	~DrawTestCore () {}

	bool  Run (FStorage_t assetStorage, FStorage_t refStorage);

	bool  SaveImage (StringView name, const ImageMemView &view) const;

protected:
	ND_ Unique<ImageComparator>  _LoadReference (StringView filename) const;

	ND_ bool  _Create (FStorage_t refStorage);
	ND_ bool  _RunTests ();
		void  _Destroy ();

	ND_ bool  _CompilePipelines (FStorage_t assetStorage);

	ND_ bool  _CompareDumps (StringView filename) const;
	ND_ bool  _CompareDumps (StringView syncLog, StringView filename) const;

	ND_ static GraphicsCreateInfo  _GetGraphicsCreateInfo ();

private:
	bool  Test_Canvas_Rect ();
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
