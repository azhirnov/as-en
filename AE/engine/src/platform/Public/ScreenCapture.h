// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread safe:  see method description.
*/

#pragma once

#include "platform/Public/Common.h"
#include "platform/Public/Monitor.h"
#include "pch/GraphicsRHI.h"

namespace AE::App
{

	//
	// Screen Capture interface
	//

	class IScreenCapture : public EnableRC<IScreenCapture>
	{
	// types
	public:
		struct MoveRect
		{
			int2	srcPoint;
			RectI	dstRect;
		};

		struct FrameInfo
		{
			// last update of screen
			HighResClock::time_point	lastPresentTime		{};
			HighResClock::time_point	lastMouseUpdateTime	{};

			// number of frames that accumulated in screen capture since the last update
			uint						accumulatedFrames	= 0;

			Array<MoveRect>				moveRects;
			Array<RectI>				dirtyRects;

			int2						pointerPos;
		};

		using SurfaceFormats_t = FixedArray< Graphics::EPixelFormat, 8 >;

		struct Config
		{
			// Will read memory from GPU and pass to 'IListener::NewFrame(ImageMemView, ...)'.
			// If not defined then image will be on GPU side and must be acquired by 'AcquireImage()' call.
			//
			Graphics::EPixelFormat		hostImageFormat		= Default;

			// List of required formats to create duplication screen.
			// From high priority to low.
			SurfaceFormats_t			surfaceFormats;

			Monitor::NativeMonitor_t	monitorHandle		= null;
		};


		enum class ErrorCode
		{
			OK,
			Timeout,				// try again later or increase 'timeout' argument.
			_Errors,
			Error_NotSupported,		// unsupported method, use 'AcquireImage()' for GPU side access or 'ReadHostImage()' for CPU side.
			Error_NeedRecreate,		//
			Failed_Acquire,			// after this error you must restart screen capture.
			Failed_VkInterop,		// failed to share resource with Vulkan, try another parameters in 'Config'.
			Failed_StagingAlloc,	// failed to allocate staging memory.
			Failed_UserException,	//
		};

		using ReadImageFn_t		= Function< void (const Graphics::ImageMemView &memView, const FrameInfo &info) >;
		using SyncReadImageFn_t = Function< bool (const Graphics::ImageMemView &memView, const FrameInfo &info, ErrorCode err) >;


		struct Description
		{
			uint2						dimension;
			float						refreshRate		= 0.f;
			Monitor::EOrientation		orientation		= Default;
			Graphics::EPixelFormat		outputFormat	= Default;		// equal to 'Config::hostImageFormat' or 'surfaceFormat'
			Graphics::EPixelFormat		surfaceFormat	= Default;
			Monitor::NativeMonitor_t	monitorHandle	= null;
		};

		enum class EState
		{
			Active,
			Paused,
			WillFinish,				// not active, but not finished yet
			Finished,
			ActiveWithError,		// active but have error, will finished soon
			Unknown,
		};


	// interface
	public:

		// Stop capture and free resources.
		//   Thread safe: yes
		//
			virtual void  Finish ()													__NE___ = 0;	// deprecated

		ND_ virtual AsyncTask  FinishAsync ()										__NE___ = 0;


		// Get current state of screen capture.
		//   Thread safe: yes
		//
		ND_ virtual EState  GetState ()												C_NE___ = 0;


	// GPU side image //

		// Acquire next image and use it on GPU side.
		//   Thread safe: main thread only
		//
		ND_ virtual ErrorCode  AcquireImage (OUT Graphics::ImageID		&imageId,
											 OUT Graphics::ImageViewID	&viewId,
											 OUT FrameInfo				&info,
											 Graphics::CommandBatch		&cmdBatch,
											 milliseconds				timeout)	__NE___ = 0;

	// CPU size image //

		// Read new frame on CPU side.
		// Returns 'false' if there are no new images.
		// First frame may contains non-empty 'moveRects' but user should ignore it and make full copy to initialize capture.
		// Subsequent frames should use 'moveRects' to move regions and 'dirtyRects' to copy regions from 'memView' to local copy.
		// Callback must not throw exceptions.
		//   Thread safe: yes
		//
			virtual ErrorCode  ReadHostImage (const ReadImageFn_t &)				__NE___ = 0;


		// Read image in the screen capture thread.
		// Returns 'false' if not supported.
		// Callback may throw exceptions, any exception will stop the capture.
		//   Thread safe: yes
		//
			virtual bool  SetReadImageCallback (SyncReadImageFn_t fn)				__NE___ = 0;


		// Returns information about selected screen.
		//   Thread safe: yes
		//
		ND_ virtual Description  GetDescription ()									C_NE___ = 0;
	};


} // AE::App
