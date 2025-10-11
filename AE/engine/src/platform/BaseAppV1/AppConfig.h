// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/GraphicsCreateInfo.h"
#include "platform/Public/Window.h"
#include "platform/Public/VRSession.h"

namespace AE::AppV1
{
	using namespace AE::App;


	//
	// Application Config
	//

	struct AppConfig
	{
	// types
	private:
		using GraphicsCreateInfo	= Graphics::GraphicsCreateInfo;
		using VRImageDesc			= IVRSession::VRImageDesc;
		using EThread				= Threading::EThread;
		using VRDeviceTypes			= FixedArray< IVRSession::EDeviceType, 4 >;

		struct ThreadConfig : Threading::TaskScheduler::Config
		{
			EnumSet<EThread>		mask		{EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::FileIO};
			uint					maxThreads	= 2;
		};


	// variables
	public:
		GraphicsCreateInfo	graphics;
		ThreadConfig		threading;
		WindowDesc			window;
		bool				enableNetwork	= false;
		bool				enableAudio		= false;

		bool				enableVR		= false;
		bool				onlyVR			= true;		// don't create desktop window if created VR device
		VRDeviceTypes		vrDevices;
		VRImageDesc			vr;
	};


} // AE::AppV1
