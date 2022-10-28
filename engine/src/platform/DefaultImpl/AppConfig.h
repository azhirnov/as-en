// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/GraphicsCreateInfo.h"
#include "platform/Public/IWindow.h"
#include "platform/Public/VRDevice.h"

namespace AE::App
{
	using namespace AE::Base;


	//
	// Application Config
	//

	struct AppConfig
	{
		struct SchedulerCreateInfo
		{
			uint	maxWorkerThreads	= 1;
		};

		Graphics::GraphicsCreateInfo	graphics;
		SchedulerCreateInfo				threading;
		WindowDesc						window;

		bool							enableVR	= false;
		IVRDevice::VRImageDesc			vr;
	};


} // AE::App
