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
		explicit VRDeviceBase (VRDeviceListener listener)		__NE___	:
			_listener{ RVRef(listener) }
		{
			CHECK( _listener );
		}

		~VRDeviceBase ()										__NE___
		{}

		EState  GetState ()										C_NE_OF	{ DRC_SHAREDLOCK( _drCheck );  return _hmdState; }

		void  Terminate ()										__NE_OV	{ _isRunning.store( false ); }

		// Used only in IApplication implementation.
		//   Thread safe: no
		//
		virtual bool  Update (Duration_t timeSinceStart)		__NE___	= 0;


	protected:
			void  _SetState (EState newState)					__NE___;
			void  _SetStateV3 (EState newState)					__NE___;
		ND_ bool  _StateChanged (EState newState)				C_NE___;

			void  _DestroyListener ()							__NE___;
	};


} // AE::App
