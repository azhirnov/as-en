// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
	public:
		using GraphicsCreateInfo	= Graphics::GraphicsCreateInfo;
		using VRImageDesc			= IVRSession::VRImageDesc;
		using EThread				= Threading::EThread;
		using VRDeviceTypes			= FixedArray< IVRSession::EDeviceType, 4 >;
		using WindowDesc			= App::WindowDesc;

		struct ThreadConfig : Threading::TaskScheduler::Config
		{
			EnumSet<EThread>	mask			{EThread::PerFrame, EThread::Renderer, EThread::Background, EThread::IO};
			uint				maxThreads		= 2;
		};

		struct VRConfig
		{
			bool				enableVR		= false;
			bool				onlyVR			= true;		// don't create desktop window if created VR device
			VRDeviceTypes		devices;
			VRImageDesc			imageDesc;
		};


	// variables
	public:
		GraphicsCreateInfo	graphics;
		ThreadConfig		threading;
		WindowDesc			window;
		VRConfig			vr;
		bool				enableNetwork	= false;
		bool				enableAudio		= false;

	};


} // AE::AppV1
