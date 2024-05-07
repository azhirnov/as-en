// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IApplication.h"
#include "platform/Private/VRDeviceBase.h"

namespace AE::App
{

	//
	// Application Base
	//

	class ApplicationBase : public IApplication, public Noncopyable
	{
	// types
	protected:
		using WeakVRDevice	= WeakPtr< VRDeviceBase >;
	public:
		using Monitors_t	= FixedArray< Monitor, PlatformConfig::MaxMonitors >;


	// variables
	protected:
		Atomic< int >			_timeSinceStart	{0};
		Atomic< bool >			_isRunning		{true};

		Unique< IAppListener >	_listener;
		const Clock				_timer;

		RecursiveMutex			_vrDeviceGuard;
		WeakVRDevice			_vrDevice;

		DRC_ONLY(
			SingleThreadCheck	_stCheck;
		)


	// methods
	protected:
		explicit ApplicationBase (Unique<IAppListener>)											__NE___;

		void  _BeforeUpdate ()																	__NE___;
		void  _AfterUpdate ()																	__NE___;
		void  _Destroy ()																		__NE___;

	public:
		VRDevicePtr  CreateVRDevice (VRDevListenerPtr, IInputActions*, IVRDevice::EDeviceType)	__NE_OV;

		Duration_t  GetTimeSinceStart ()														C_NE_OF	{ return Duration_t{_timeSinceStart.load()}; }

		void  Terminate ()																		__NE_OV;

		DRC_ONLY( ND_ SingleThreadCheck&	GetSingleThreadCheck ()								__NE___	{ return _stCheck; })
	};


} // AE::App
