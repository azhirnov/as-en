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
	  #if defined(AE_ENABLE_VULKAN)
		Graphics::VDeviceInitializer	_vulkan;

	  #elif defined(AE_ENABLE_METAL)
		Graphics::MDeviceInitializer	_metal;
		
	  #else
	  #	error not implemented
	  #endif
			
		Array<WindowPtr>	_windows;
		VRDevicePtr			_vrDevice;
		const AppConfig		_config;


	// methods
	protected:
		explicit ApplicationImpl (const AppConfig &cfg)	__NE___;
	public:
		~ApplicationImpl ()								__NE_OV;

		ND_ AppConfig const&  Config ()					C_NE___	{ return _config; }


	// IAppListener //
		void  OnStart (IApplication &)					__NE_OV;
		void  OnStop  (IApplication &)					__NE_OV;

		void  AfterWndUpdate (IApplication &)			__NE_OV;


	private:
	  #if defined(AE_ENABLE_VULKAN)
		bool  _CreateVulkan (IApplication &app);
		bool  _DestroyVulkan ();

	  #elif defined(AE_ENABLE_METAL)
		bool  _CreateMetal (IApplication &app);
		bool  _DestroyMetal ();
		
	  #else
	  #	error not implemented
	  #endif
	};


} // AE::App
