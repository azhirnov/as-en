// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IApplication.h"
#include "platform/DefaultImpl/AppConfig.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VDevice.h"
#endif

namespace AE::App
{

	//
	// Application Event Listener
	//

	class ApplicationImpl : public IApplication::IAppListener
	{
	// variables
	protected:
		#ifdef AE_ENABLE_VULKAN
			Graphics::VDeviceInitializer	_vulkan;
		#endif
			
		Array<WindowPtr>	_windows;
		VRDevicePtr			_vrDevice;
		const AppConfig		_config;


	// methods
	protected:
		explicit ApplicationImpl (const AppConfig &cfg);
	public:
		~ApplicationImpl () override;

		ND_ AppConfig const&  Config () const	{ return _config; }


	// IAppListener //
		void  OnStart (IApplication &) override;
		void  OnStop  (IApplication &) override;

		void  AfterWndUpdate (IApplication &) override;


	private:
		#ifdef AE_ENABLE_VULKAN
		bool  _CreateVulkan (IApplication &app);
		bool  _DestroyVulkan ();
		#endif
	};


} // AE::App
