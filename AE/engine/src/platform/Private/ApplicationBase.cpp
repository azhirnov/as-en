// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/ApplicationBase.h"

#include "platform/OpenVR/OpenVRDevice.h"
#include "platform/Private/VRDeviceEmulator.h"

namespace AE::App
{

/*
=================================================
	constructor
=================================================
*/
	ApplicationBase::ApplicationBase (Unique<IAppListener> listener) __NE___ :
		_listener{ RVRef(listener) }
	{
		CHECK( _listener );
	}

/*
=================================================
	CreateVRDevice
=================================================
*/
	VRDevicePtr  ApplicationBase::CreateVRDevice (VRDevListenerPtr listener, IInputActions* dst, IVRDevice::EDeviceType type) __NE___
	{
		using EDeviceType = IVRDevice::EDeviceType;

		CHECK_ERR( _isRunning.load() );
		CHECK_ERR( listener );

		switch_enum( type )
		{
			case EDeviceType::Emulator :
			{
			#if 1
				SharedPtr<VRDeviceEmulator>	vr{ new VRDeviceEmulator{ *this, RVRef(listener), dst }};
				if ( not vr->Create() ) return Default;
				{
					EXLOCK( _vrDeviceGuard );
					CHECK_ERR( _vrDevice.lock() == null );
					_vrDevice = vr;
				}
				return vr;
			#else
				break;
			#endif
			}

			case EDeviceType::OpenVR :
			{
			#ifdef AE_ENABLE_OPENVR
				SharedPtr<OpenVRDevice>	vr{ new OpenVRDevice{ RVRef(listener), dst }};
				if ( not vr->Create() ) return Default;
				{
					EXLOCK( _vrDeviceGuard );
					CHECK_ERR( _vrDevice.lock() == null );
					_vrDevice = vr;
				}
				return vr;
			#else
				break;
			#endif
			}

			case EDeviceType::OpenXR :
			{
			//#ifdef AE_ENABLE_OPENXR
			//#else
				break;
			//#endif
			}

			case EDeviceType::Unknown : break;
		}
		switch_end

		return Default;
	}
/*
=================================================
	_BeforeUpdate
=================================================
*/
	void  ApplicationBase::_BeforeUpdate () __NE___
	{
		ASSERT( _isRunning.load() );

		_timeSinceStart.store( _timer.TimeSince<Duration_t>().count() );

		if_likely( _listener )
			_listener->BeforeWndUpdate( *this );

		// update VR
		{
			EXLOCK( _vrDeviceGuard );

			SharedPtr<VRDeviceBase>	vr = _vrDevice.lock();

			if ( vr )
				vr->Update( GetTimeSinceStart() );
		}
	}

/*
=================================================
	_AfterUpdate
=================================================
*/
	void  ApplicationBase::_AfterUpdate () __NE___
	{
		if_likely( _listener )
			_listener->AfterWndUpdate( *this );
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  ApplicationBase::_Destroy () __NE___
	{
		_isRunning.store( false );

		{
			EXLOCK( _vrDeviceGuard );
			_vrDevice.reset();
		}

		if ( _listener )
		{
			_listener->OnStop( *this );
			_listener.reset();
		}
	}

/*
=================================================
	Terminate
=================================================
*/
	void  ApplicationBase::Terminate () __NE___
	{
		_isRunning.store( false );
	}


} // AE::App
