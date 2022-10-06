// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/VRDevice.h"
#include "platform/Public/IApplication.h"

namespace AE::App
{

	//
	// VR Device Base
	//

	class VRDeviceBase : public IVRDevice
	{
	// types
	protected:
		using VRDeviceListener	= Unique< IVRDeviceEventListener >;
		using Duration_t		= IApplication::Duration_t;


	// variables
	protected:
		Atomic< bool >			_isRunning		{true};

		VRDeviceListener		_listener;
		EState					_hmdState		= EState::Unknown;
		
		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		explicit VRDeviceBase (VRDeviceListener listener) :
			_listener{ RVRef(listener) }
		{
			CHECK( _listener );
		}

		~VRDeviceBase ()
		{}
		
		EState  GetState () const override final	{ DRC_SHAREDLOCK( _drCheck );  return _hmdState; }
		
		void  Terminate () override
		{
			_isRunning.store( false );
		}

		// used only in IApplication implementation
		virtual bool  Update (Duration_t timeSinceStart) = 0;
		

	protected:
		void  _SetState (EState newState);
	};


} // AE::App
