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
	// types
	private:
		using GraphicsCreateInfo	= Graphics::GraphicsCreateInfo;
		using VRImageDesc			= IVRDevice::VRImageDesc;
		using EThread				= Threading::EThread;

		struct ThreadConfig : Threading::TaskScheduler::Config
		{
			EnumBitSet<EThread>		mask				{EThread::Worker, EThread::Renderer, EThread::FileIO};
			uint					maxWorkerThreads	= 2;
		};


	// variables
	public:
		GraphicsCreateInfo	graphics;
		ThreadConfig		threading;
		WindowDesc			window;

		bool				enableVR	= false;
		VRImageDesc			vr;
	};


} // AE::App
