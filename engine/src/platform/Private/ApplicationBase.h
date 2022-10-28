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
		using TimePoint_t	= std::chrono::high_resolution_clock::time_point;
		
		using WeakVRDevice	= WeakPtr< VRDeviceBase >;


	// variables
	protected:
		Atomic< uint >			_timeSinceStart	{0};
		Atomic< bool >			_isRunning		{true};
		
		Unique< IAppListener >	_listener;
		const TimePoint_t		_timerStart;
		
		RecursiveMutex			_vrDeviceGuard;
		WeakVRDevice			_vrDevice;

		DRC_ONLY(
			SingleThreadCheck	_stCheck;
		)


	// methods
	protected:
		explicit ApplicationBase (Unique<IAppListener>);
		
		void  _BeforeUpdate ();
		void  _AfterUpdate ();
		void  _Destroy ();

	public:
		VRDevicePtr  CreateVRDevice (VRDevListenerPtr, IInputActions* dst, IVRDevice::EDeviceType type) override;
		
		Duration_t  GetTimeSinceStart () const override final	{ return Duration_t{_timeSinceStart.load()}; }
		
		void  Terminate () override;

		DRC_ONLY( ND_ SingleThreadCheck&	GetSingleThreadCheck ()		{ return _stCheck; })
	};


} // AE::App
